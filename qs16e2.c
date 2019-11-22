/***************************************************/
/*                                                 */
/*          qs16   (Quick sort function)           */
/*                                                 */
/* by 河村 知行 (kawamura tomoyuki)    2019.10.3   */
/*  〒745-0845 山口県周南市河東町3-2 （株）曽呂利  */
/*             t-kawa@crux.ocn.ne.jp               */
/*                                                 */
/*本ソフトウェア(含mm88k.c)はパブリックドメインです*/
/***************************************************/

int _QS_MID1=120;  // 要素数n  <= _QS_MID1 のときは　3つの要素から分割要素を決定する（３点処理）
int _QS_MID2=580;  // 要素数n  <= _QS_MID2 のときは　9つの要素から分割要素を決定する（９点処理）
int _QS_MID3=180;  // 要素size >= _QS_MID3 のときは　mps・間接ソートの可能性を検査する
int _QS_MID4=360;  // 要素size >= _QS_MID4 のときのみ　mpsの可能性を検査する

#include <stdio.h>
//typedef long unsigned int size_t;
void exit(int);
void *memcpy(void *, const void *, size_t);
void *malloc(size_t); void free(void*);

#ifdef DEBUG
unsigned int ass_cnt;                   /*代入回数を計測しないときは、削除可能*/
#define ASS_CNT(x)  {ass_cnt += (x);}
static void assert(int x, char *s) {
  if (x) return;
  fprintf(stderr, "++++ %s ++++\n", s); printf("++++ %s ++++ \n", s);
  exit(1);
}
#else
#define ASS_CNT(x) {}
static void assert(int x, char *s) {}
#endif

#include "mm88k.c"

static int (*cmp_org)( void *a, void *b );
static int cmp_indirect( void *a, void *b ) {return (*cmp_org)( *(void**)a, *(void**)b );}
static void move_base_by_ptr( void *base, size_t nel, size_t size, void **ptr, char *tmp );

typedef struct { char *LLss, *RRss; } stack_node;   /*L,Rを積むスタックの構造体*/
#define PUSH(llss,rrss) {top->LLss = (llss); top->RRss = (rrss); ++top;}    /*L,Rを積む*/
#define POP(llss,rrss)  {--top; llss = top->LLss; rrss = top->RRss;}        /*L,Rを戻す*/

#define med3(a,b,c) ((t=(*cmp)(a,b))<=0 ? ((*cmp)(b,c)<=0 ? b : (t==0 ? b : ((*cmp)(a,c)<=0 ? c : a))) : \
                                          ((*cmp)(b,c)>=0 ? b :             ((*cmp)(a,c)<=0 ? a : c) ))

#define  LT(a,b)  if ((t=(*cmp)(a,b)) <  0) 
#define  LE(a,b)  if ((t=(*cmp)(a,b)) <= 0) 
#define  GT(a,b)  if ((t=(*cmp)(a,b)) >  0) 
#define  GE(a,b)  if ((t=(*cmp)(a,b)) >= 0) 
#define  else_GT  else if (t > 0)
#define  else_LT  else if (t < 0)

#define S(x,y)   {mmswap(x,y);}
#define K(x,y,z) {mmrot3(x,y,z);}
#define I(x)     {x+=size;}
#define D(x)     {x-=size;}



//typedef int (*__compar_fn_t) (const _PTR, const _PTR);
//void	qsort( _PTR  base, size_t nel, size_t size,  __compar_fn_t cmp)
void    qsort( void *base, size_t nel, size_t size,  int (*cmp)(void *a, void *b) )
              /* base : ソートしようとする配列へのポインタ    */
              /* nel  : 配列baseの要素数                      */
              /* size : 配列baseの要素の大きさ（バイト単位）  */
              /* cmp  : 要素の大小比較をする関数へのポインタ  */
{
 char *L = (char*)base;                  /*分割中の区間の左端の要素の先頭*/
 char *R = &((char*)base)[size*(nel-1)]; /*分割中の区間の右端の要素の先頭*/
 char *l,*f,*m,*g,*r;                    /*左、中央、右 区間を保持するためのポインタ*/
 int  t,v,w;                             /*作業用変数*/
 int  n;                                 /*分割中の区間の要素数*/
 int  eqcnt;                             /*適当に選択した32組の比較をして、等しい組の数*/
 stack_node stack[32], *top = stack;     /*現在のところでは３２で十分*/

#define VBUN 64
 if (nel  <  VBUN    ) goto direct_sort; // 要素数が64未満は、直接qsortを実施
 if (size >= _QS_MID3) goto chk_sort;    /* mpsに失敗したら戻る _QS_MID3=180 */

//ここから下は、qs9による直接ソートのプログラムです
direct_sort:
 mmprepare( base, size );

LOOP:
 if (L>=R) {goto nxt;}
loop:
 if (L + size == R) {if ((*cmp)(L,R) > 0) S(L,R) goto nxt;} /*要素数２*/
 
 n = (R - L + size) / size;  /*要素数*/
 
 if (n <= 4) {
   m = R - size;
   LT(L,m)  GT(m,R)  LE(L,R)  S(m,R)    /*3-5-3,4*/
                     else     K(R,m,L)  /*3-5-2*/
            else     {}                 /*3-5-5,7 のときは何もしない*/
   else_GT  GE(m,R)  S(L,R)             /*7-5-5,3*/
            else     LE(L,R)  S(L,m)    /*7-5-7,8*/
                     else     K(L,m,R)  /*7-5-6*/
   else     GT(m,R)  S(L,R)             /*5-5-3*/
            else     {}                 /*5-5-5,7 のときは何もしない*/
 if (n == 4) {
   char *p = L+size;
   LT(p,m)  GT(L,p)  S(L,p)           /*3-2-5-7*/
            else     {}               /*3-3,4-5-7 のときは何もしない*/
   else_GT  LE(p,R)  S(p,m)           /*3-6,7-5-7*/
            else     K(p,m,R)         /*3-8-5-7*/
   else     {}                        /*3-5-5-7 のときは何もしない*/
 }
 goto nxt;
 } /* n <= 4 */



 m = L + size * (n >> 1);    /*配列の中央を計算*/

 if (n <= _QS_MID1) {        //３点処理
   l=L; r=R; f=m-size; g=m+size;
   LT(l,m) LT(m,r)         {              I(l) D(r) goto _lfgr;} // 3 5 7
           else_GT LT(l,r) {S(m,r)        I(l) D(r) goto _lfgr;} // 3 7 5
                   else_GT {K(l,r,m)      I(l) D(r) goto _lfgr;} // 5 7 3
                   else    {S(m,r)             D(r) goto _5fgr;} // 5 7 5
           else            {S(g,r)        I(l) I(g) goto _lfgr;} // 3 5 5          I(l) lfg5 もあり

   else_GT LT(l,r)         {S(l,m)        I(l) D(r) goto _lfgr;} // 5 3 7
           else_GT LT(m,r) {K(l,m,r)      I(l) D(r) goto _lfgr;} // 7 3 5
                   else_GT {S(l,r)        I(l) D(r) goto _lfgr;} // 7 5 3
                   else    {K(l,f,r)      D(f) D(r) goto _lfgr;} // 7 5 5   S(l,r) D(r) 5fgr もあり
           else            {S(l,m) S(g,r) I(l) I(g) goto _lfgr;} // 5 3 5   S(l,m) I(l) lfg5 もあり

   else    LT(m,r)         {              D(r)      goto _5fgr;} // 5 5 7
           else_GT         {K(l,r,g) I(l) I(g)      goto _lfgr;} // 5 5 3   S(l,r) I(l) lfg5 もあり
           else            {S(g,r)        I(g)      goto _5fgr;} // 5 5 5               5fg5 もあり
 }



 if (n <= _QS_MID2) { //９点処理           // t=(n>>3)*size;                配列の中央に最大(最小)近傍の
   l=med3(m-size*4, m-size*3, m-size*2);   // l=med3(L    , m-t*3, m-t*2);  要素が集まりやすいときは、
   r=med3(m+size*2, m+size*3, m+size*4);   // r=med3(m+t*2, m+t*3, R    );  右側が良い。
   m=med3(m-size  ,    m    , m+size  );   // f=med3(m-t  , m    , m+t  );  左側だと 1 + 1/log(n) 倍に
   m=med3(   l    ,    m    ,    r    );   // g=med3(l    , f    , r    );  遅くなる可能性あり。しかし
                                           // if (m!=g) {S(m,g);}           これは、悲惨な遅さではない。
   l=L; r=R; f=m-size; g=m+size;
   goto _lfgr;
 }



 {char *p,*z1,*z2,*z3; int w;            //２７点処理 
 p=m-(w=size*3)*4;  f=p+size; g=f+size; z1=med3(p, f, g);
           p+=w;    f+=w;     g+=w;     z2=med3(p, f, g);
           p+=w;    f+=w;     g+=w;     z3=med3(p, f, g); l=med3(z1, z2, z3);
           p+=w;    f+=w;     g+=w;     z1=med3(p, f, g);
           p+=w;    f+=w;     g+=w;     z2=med3(p, f, g);
           p+=w;    f+=w;     g+=w;     z3=med3(p, f, g); m=med3(z1, z2, z3);
           p+=w;    f+=w;     g+=w;     z1=med3(p, f, g);
           p+=w;    f+=w;     g+=w;     z2=med3(p, f, g);
           p+=w;    f+=w;     g+=w;     z3=med3(p, f, g); r=med3(z1, z2, z3);
 m=med3(l, m, r);
 l=L; r=R; f=m-size; g=m+size;
 goto _lfgr;
 }



/*
333...555...777　　lfgr のlの位置から比較を始める。357を貯める。 l[fg]r系　 この系は l<=f g<=r を保証
L  l f m g r  R 
mは分割要素を指す。mもその要素も1回の分割終了の直前まで、変更なし。
「5」分割要素と同キーの要素を表す。「3」5より小さいキーの要素を表す。「7」5より大きいキーの要素を表す。 
「l」333の右隣の要素を指す 「r」777の左隣の要素を指す 「L」先頭要素を指す 「R」最終要素を指す
「f」555の左隣の要素を指す 「g」555の右隣の要素を指す
「.」未比較の要素の列(長さ0以上)を表す  「_」未比較の要素1つを表す

333355555...777 になったら 333355555...777 にする。333355555333...777 として第2の「3」を貯める。
   fl    g r                  f     l r               f     g  l r   
   f<l になったら                   l=g; する。             gとlの間に「3」を貯める。 [fg]lr系  r<lもあり

333...555557777 になったら 333...555557777 にする。333...777555557777 として第2の「7」を貯める。
   l f    rg  R               l r     g  R            l r  f     g   
          r<g になったら        r=f; する。             rとfの間に「7」を貯める。     lr[fg]系  r<lもあり
*/

chk:                                          // L l f  g r R
 if (l<=f) if (g<=r) {          goto _lfgr;}  // 33...55...77
           else      {r=f;      goto _lrfg;}  // 33..5577
 else      if (g<=r) {l=g;      goto _fglr;}  // 3355..77
           else      {D(l) I(r) goto fin;  }  // 333555777
            
chk_lf:
 if (l>f)  {l=g; goto _fglr;}    // 3355..77
                                                    //L l f  g r R
_lfgr:                                              //33...55...77
 GT(l,m) {_7fgr: LT(r,m) {_7fg3: S(l,r) I(l) D(r) goto chk;   }  //337..55..377
                 else_GT {_7fg7: D(r) if (g<=r)  {goto _7fgr;}   //337..55..777
                                      else  {r=f; goto _7rfg;}}  //337..55777
                 else    {                        goto _7fg5; }} //337..55..577
 else_LT {_3fgr: I(l) if (l<=f) {                 goto _lfgr; }  //333..55...77
                      else {l=g;                  goto _fglr; }} //33355...77
 else    {                                        goto _5fgr;  } //335..55...77



       //  lf  gr
_5fgr: //335.55..77    二通りある
 if ((f-l+g) < (r)) goto _5fgr_g;     // (f-l) < (r-g) の意味

_5fgr_f:
 if (l==f) {D(f) l=g;                                          goto _fglr;     }  //33555..77
 LT(f,m)   {_53gr: S(l,f) I(l) D(f)                            goto chk_lf;    }  //335.355..77
 else_GT   {_57gr: LT(r,m) {_57g3: K(l,r,f) I(l) D(f) D(r)     goto chk;     }    //335.755.377
                   else_GT {_57g7: if (g==r) {r=f-size;        goto _5rfg;  }     //335.755777
                                   else      {D(r)             goto _57gr;  }}    //335.755_777
                   else    {_57g5: if (g==r) {I(g) r=f-size;   goto _5rfg;  }     //335.755577
                                   else      {S(f,r) D(f) D(r) goto _5fgr_f;}} }  //335.755_577
 else      {_55gr: D(f)                        /*goto _5fgr;*/ goto _5fgr_f;   }  //335.555..77
assert(0,"_5fgr_f");

_5fgr_g:
 LT(g,m) {_5f3r: S(l,g) I(l) I(g)                             goto chk;     } //335.553.77
 else_GT {_5f7r: if (g==r) {r=f;                              goto _5rfg;  }
                 else LT(r,m) {_5f73: K(l,r,g) I(l) I(g) D(r) goto chk;    }  //335.557377
                      else_GT {_5f77: D(r)                    goto _5f7r;  }
                      else    {_5f75: S(g,r) I(g) D(r)        goto _5fgr_ ;}}
 else    {_5f5r: I(g)  _5fgr_: if (g<=r) {    /*goto _5fgr;*/ goto _5fgr_g;} 
                               else      {r=f;                goto _5rfg;  }}
assert(0,"_5fgr_g");



       //  l f  g r
_7fg5: //337..55..577    二通りある
 if ((f-l+g) < (r)) goto _7fg5_g;     // (f-l) < (r-g) の意味

_7fg5_f:
 if (l==f) {S(l,r) D(f) l=g; D(r)              goto _fglr; } //33755.577
 LT(f,m)   {_73g5: K(l,f,r) I(l) D(f) D(r)     goto chk;   } //337.355.577
 else_GT   {_77g5: S(f,r) D(f) D(r) if (g<=r) {goto _7fgr;}
                                    else {r=f; goto _7rfg;}} //337.755.577
 else      {_75g5: D(f)                        goto _7fg5; }
assert(0,"_7fg5_f");

_7fg5_g:
 if (g==r) {_7f5:  I(g) r=f;                   goto _7rfg; }
 LT(g,m)   {_7f35: K(l,g,r) I(l) I(g) D(r)     goto chk;   }
 else_GT   {_7f75: S(g,r) I(g) D(r) if (g<=r) {goto _7fgr;}
                                    else {r=f; goto _7rfg;}} //337.557.577
 else      {_7f55: I(g)                        goto _7fg5; }
assert(0,"_7fg5_g");



       //  l r f  g
_5rfg: //335..775577
 if (l==r) {       I(l) D(r)                                       goto fin_rlfg;}  //本本3357775577
 LT(r,m)   {_53fg: {if (r==f) S(l,r) else K(l,r,f)} I(l) D(r) D(f) goto _lrfg;   }  //      r  f  g
 else_GT   {_57fg: D(r)                                            goto _5rfg;   }
 else      {_55fg: {if (r!=f) S(r,f)} D(r) D(f)                    goto _5rfg;   }
assert(0,"_5rfg");

_7rfg: //337..775577
 if (l==r) {       D(r)                         goto fin_rlfg;}  //33775577 3375577
 LT(r,m)   {_73fg: S(l,r) I(l) D(r)             goto _lrfg;   }  //337.3775577
 else_GT   {_77fg: D(r)                         goto _7rfg;   }  //337.7775577
 else      {_75fg: {if (r!=f) S(r,f)} D(r) D(f) goto _7rfg;   }  //337.5775577
assert(0,"_7rfg");



_lrfg: //mの要素が移動することはない
 if (l<r)  GT(r,m) {_l7fg: D(r)                                           goto _lrfg;}
           else_LT {_l3fg: GT(l,m) {_73fG: S(l,r) I(l) D(r)               goto _lrfg;}
                           else_LT {_33fg: I(l) if (l<r) {                goto _l3fg;}
                                                else     {I(l)            goto fin_rlfg;}}
                           else    {_53fG: if (r==f) S(l,r) else K(l,r,f)
                                           I(l) D(r) D(f)                 goto _lrfg;}}
           else    {_l5fg: {if (r!=f) S(r,f)} D(r) D(f)                   goto _lrfg;}
 else
 if (l==r) LT(l,m) {_3fg:  I(l)                         goto fin_rlfg;}
           else_GT {_7fg:  D(r)                         goto fin_rlfg;}
           else    {_5fg:  I(l) D(r)                    goto fin_rlfg;}  //本本333577555777
 else              {                                    goto fin_rlfg;}  //       r f   g

fin_rlfg:
 assert( r+size == l || r+size*2 == l , "fin_rlfg");
 I(f); 
 if ((v=f-l)<=0) {l=r; r=g; goto fin;}
 if ((w=g-f)==size) S(l,f)
 else if (v>=w) mmswapblock(l,f  ,w);
 else           mmswapblock(l,g-v,v);
 l=r; r=g-v;
 goto fin;



_fglr:
 if (l<r)  LT(l,m) {_fg3r: I(l)                                           goto _fglr;}
           else_GT {_fg7r: LT(r,m) {_fg73: S(l,r) I(l) D(r)               goto _fglr;}
                           else_GT {_fg77: D(r) if (l<r) {                goto _fg7r;}
                                                else     {D(r)            goto fin_fgrl;}}
                           else    {_fg75: if (g==l) S(l,r) else K(g,r,l)
                                           I(g) I(l) D(r)                 goto _fglr;}}
           else    {_fg5r: {if (g!=l) S(g,l)} I(g) I(l)                   goto _fglr;}
 else
 if (l==r) LT(l,m) {_fg3:  I(l)       goto fin_fgrl;}  //            r
           else_GT {_fg7:  D(r)       goto fin_fgrl;}  //     f   g  l
           else    {_fg5:  I(l) D(r)  goto fin_fgrl;}  //本本33555333577
 else              {                  goto fin_fgrl;}

fin_fgrl:                                              //333355533333777   3333555333335777
 assert( r+size == l || r+size*2 == l , "fin_fgrl" );  //   f   g   rl        f   g   r l
 I(f); 
 if ((v=r-g+size)<=0) {r=l; l=f-size; goto fin;}
 if ((w=g-f)==size) S(f,r)
 else if (v>=w) mmswapblock(f,r-w+size,w);
 else           mmswapblock(f,g  ,v);
 f=l; l=r-w; r=f;
 goto fin;



fin:
 if (l-L < R-r) {PUSH(r,R); R = l;} /*左から先にソートする*/ //if(r,L<R,l)がないと少し遅くなる
 else           {PUSH(L,l); L = r;} /*右から先にソートする*/
 if (L<R) goto loop;                         /*要素が２個以上ある場合*/

nxt:
 if (stack == top) {return;}    /*スタックが空になったとき終了する*/
 POP(L,R);                      /*スタックに積んである値を取り出す*/
 goto LOOP;
//ここまでが、直接ソートのプログラムです



//ここから下は、間接ソートのためのプログラムです
  // 間接ソートを行うためには、要素sizeが大きいことが必要。同値キーが多いほど、size大が必要。
  // 要素数(nel)とeqcntごとに、必要なsizeが変化する。下記は、nel(1000000～64) X eqcnt(0～8)での値。
  //   同値キーの個数(eqcnt)   0    1    2    3    4    5    6     7     8
  static int ind_start1M  []={540,1120,1520,1760,1900,1900,1910,17000,20000}; //この表の右上4個は
  static int ind_start400K[]={540,1080,1480,1680,1760,1820,1880,17000,20000}; //桁あふれ防止の
  static int ind_start200K[]={460, 940,1380,1600,1680,1720,1710, 7000,10200}; //ための数値である。
  static int ind_start100K[]={460, 740,1200,1480,1520,1580,1560, 4000, 5400}; //直接・間接ソートの
  static int ind_start40K []={380, 590, 860,1180,1340,1350,1340, 3000, 3900}; //判定ミスが有りうる。
  static int ind_start20K []={380, 440, 620, 740, 860, 910, 910,  960, 1080}; //遅くなる可能性は
  static int ind_start10K []={340, 450, 540, 670, 780, 800, 800,  860,  960}; //あるが
  static int ind_start4K  []={220, 350, 620, 700, 780, 820, 850,  930, 1030}; //正しいソートは維持。
  static int ind_start2K  []={180, 300, 390, 580, 710, 920,1040, 1200, 1360};
  static int ind_start1K  []={180, 230, 340, 460, 620, 770,1000, 1380, 1680};
  static int ind_start400 []={180, 220, 300, 380, 590, 720, 840, 1080, 1360};
  static int ind_start200 []={220, 250, 320, 380, 540, 640, 860, 1160, 1360};
  static int ind_start100 []={260, 330, 380, 450, 550, 650, 740,  880, 1000};
  static int ind_start64  []={320, 360, 420, 500, 660, 730, 820,  960, 1060};
  //上記は size=8x 用。他に size=8x+4 用と size=8x+1,2,3 用の表もあるべきだが、重要度が低いため省略。

 {int *pre_start, *ind_start; int pre_nel, ind_nel;
indirect_sort:
 // 適当な32組の要素を比較して、等しい組の数を eqcnt（同値キーの個数）とする。
 // eqcnt は chk_sort: で既に計算が終わっている

  if (nel>=10000)
       if (nel>=200000) if (nel>=1000000)        /*1000000*/{pre_start=ind_start1M;   pre_nel=9000000;
                                                             ind_start=ind_start1M;   ind_nel=1000000;}
                        else              if (nel>= 400000) {pre_start=ind_start1M;   pre_nel=1000000;
                                                             ind_start=ind_start400K; ind_nel=400000;}
                                          else   /* 200000*/{pre_start=ind_start400K; pre_nel=400000;
                                                             ind_start=ind_start200K; ind_nel=200000;}
       else             if (nel>=  40000) if (nel>= 100000) {pre_start=ind_start200K; pre_nel=200000;
                                                             ind_start=ind_start100K; ind_nel=100000;}
                                          else   /*  40000*/{pre_start=ind_start100K; pre_nel=100000;
                                                             ind_start=ind_start40K;  ind_nel=40000;}
                        else              if (nel>=  20000) {pre_start=ind_start40K;  pre_nel=40000;
                                                             ind_start=ind_start20K;  ind_nel=20000;}
                                          else   /*  10000*/{pre_start=ind_start20K;  pre_nel=20000;
                                                             ind_start=ind_start10K;  ind_nel=10000;}
  else if (nel>=   400) if (nel>=   2000) if (nel>=   4000) {pre_start=ind_start10K;  pre_nel=10000;
                                                             ind_start=ind_start4K;   ind_nel=4000;} 
                                          else   /*   2000*/{pre_start=ind_start4K;   pre_nel=4000;
                                                             ind_start=ind_start2K;   ind_nel=2000;}
                        else              if (nel>=   1000) {pre_start=ind_start2K;   pre_nel=2000;
                                                             ind_start=ind_start1K;   ind_nel=1000;}
                                          else   /*    400*/{pre_start=ind_start1K;   pre_nel=1000;
                                                             ind_start=ind_start400;  ind_nel=400;}
       else             if (nel>=    100) if (nel>=    200) {pre_start=ind_start400;  pre_nel=400;
                                                             ind_start=ind_start200;  ind_nel=200;}
                                          else   /*    100*/{pre_start=ind_start200;  pre_nel=200;
                                                             ind_start=ind_start100;  ind_nel=100;}
                        else              if (nel>=     64) {pre_start=ind_start100;  pre_nel=100;
                                                             ind_start=ind_start64;   ind_nel=64;}
                                          else              goto direct_sort;
  //if (nel>=2000000) での値は、メモリ不足で計算できなかった。
  //if (nel< 64) でも間接ソートは効果あり。しかし VBUN 32,16,8 が必要となる。今回は実装せず。

  // ind_start***の表の当該の値と一つ上の値を補間して、要求値とする
  int ind_size=ind_start[eqcnt];
  int ind_size2=( (pre_start[eqcnt]-ind_size) * ((int)nel-ind_nel) ) / (pre_nel-ind_nel) + ind_size;
  if (size < ind_size2) goto direct_sort;  //sizeが要求値より少ないとき、直接ソートを実行
  //補間なしの場合、上記3行は次の1行となる　if (size < ind_start[eqcnt]) goto direct_sort;
 }

 {void **arr, **tp;  char *tmp, *ip;
  if ((arr = malloc( nel * sizeof(char*) + size)) == NULL) goto direct_sort;
  tmp = (char*)arr + nel * sizeof(char*);
  for (ip=base, tp=arr; ip<=R; ip+=size, tp++) *tp=(void*)ip;

  cmp_org = cmp;
  qsort( arr, nel, sizeof(char*), cmp_indirect );

  move_base_by_ptr( base, nel, size, arr, tmp );
  free(arr);
 }
 return;
//ここまでが、間接ソートのプログラムです。qs15 クイックソートもここまで。



chk_sort:
 // 適当な32組の要素を比較して、等しい組の数を eqcnt（同値キーの個数）とする。
 eqcnt=0;
 int vsize=(nel/VBUN)*size; int half=vsize*(VBUN/2); char *ip_end=base+half;
 for (char *ip=base; ip<ip_end; ip+=vsize) if ((*cmp)(ip,ip+half)==0) eqcnt++;

 assert(eqcnt <= 32, "eqcnt > 32");
 assert(eqcnt >=  0, "eqcnt <  0");
 if (eqcnt >= 9      ) goto direct_sort;   //同値キーが多過ぎるとき、直接ソートを実行。 /*9は実験より*/
 if (size  < _QS_MID4) goto indirect_sort; // _QS_MID4==360。size < 320 がベスト　ここではsize>=180

 /*直接mps・間接mps・qs15の振り分けを行う*/
 int mps_direct = 1;
 if (eqcnt == 0) {                        //eqcnt==0では直接mpsまたは間接mpsまたは間接qs15
   if (nel >= 1000000) {if (size>=960) {mps_direct=0;} goto mps_common;}
   if (nel >=  400000) {if (size>=560) {mps_direct=0;} goto mps_common;}
   if (nel >=  200000) {if (size>=480) {mps_direct=0;} goto mps_common;}
   goto indirect_sort;                    
 }
 if (nel < 700) {goto indirect_sort;}     //eqcnt>0では直接mpsまたは間接qs15
 if (eqcnt <= 3) {if (size>= 400) {goto mps_common;} goto indirect_sort;}
 if (eqcnt <= 4) {if (size>= 560) {goto mps_common;} goto indirect_sort;}
 if (eqcnt <= 6) {if (size>= 840) {goto mps_common;} goto indirect_sort;}
   /*eqcnt <= 8*/{if (size>=1600) {goto mps_common;} goto indirect_sort;}



//ここから下は、mps(multi partition sort) のためのプログラムです
//mpsは、キーの種類が10～100(県コードなど)で有効です。最大2割ほど速くなります
// 同値キーの個数(eqcnt)     0    1    2   3   4   5   6   7   8 
 static int bun_arr2M  []={1024,1024,1024,512,512,512,512,256,256};
 static int bun_arr1M  []={ 512,1024, 512,512,512,256,256,256,256};
 static int bun_arr400K[]={ 512,1024, 512,512,256,512,256,256,256};
 static int bun_arr200K[]={ 256, 512, 512,256,256,512,512,128,128};
 static int bun_arr100K[]={ 256, 512, 256,256,256,256,256,128,128};
 static int bun_arr40K []={ 256, 256, 256,256,128,256,256,128,128};
 static int bun_arr20K []={ 256, 256, 256,128,128,128,128, 64, 64};
 static int bun_arr10K []={ 128, 256, 128,128, 64, 64, 64, 64, 64};
 static int bun_arr4K  []={ 128, 128, 128, 64, 64, 64, 64, 32, 32};
 static int bun_arr2K  []={  64, 128,  64, 64, 64, 32, 32, 32, 32};
 static int bun_arr1K  []={  64,  64,  64, 64, 32, 32, 32, 32, 32};
 static int bun_arr400 []={  32,  64,  32, 32, 32, 32, 16, 16, 16};
 //static int bun_arr200 []={16,  32,  32, 32, 16, 16, 16, 16, 16};
 //static int bun_arr100 []={ 8,  16,  16, 16, 16, 16,  8,  8,  8};
 //static int bun_arr64  []={ 4,   8,   8,  8,  8,  8,  8,  8,  8};
 static int *bun_arr;

#define H(x) ((char*)base+size*(x))
#define IDX_T  int
#define GRP_T  short
#define PTR_T  void*

 {
 int bunkatu;     /*分割数*/
 int beki;        /*区間の個数(bunkatu*2)   1～beki-1が区間番号  0は使用しない*/
 int i;           /*iはbase中の要素の位置*/
 int g;           /*gは区間番号*/
 char  *tmp;      /*配列baseの要素1つを退避するための領域（下記4個とメモリを共用）*/
 GRP_T *g_grp;    /*g_grp[i]は要素iの区間番号*/
 IDX_T *g_cnt;    /*g_cnt[g]は区間gの要素数*/
 IDX_T *g_head;   /*g_head[g]は区間gの未確定要素の先頭位置*/
 IDX_T *g_end;    /*g_end[g]は区間gの最後の要素の次の要素の添字*/
 IDX_T *stk,*sp;  /*直接mpsでのみ使用。stk[]に玉突きの並びを保存　spはスタックポインタ*/
 PTR_T *ptr;      /*サンプル要素へのポインタ配列。間接mpsでは、base全要素へのポインタ配列にもなる*/

mps_common:
 /*分割数を決定する*/
 if      (nel >= 2000000) bun_arr = bun_arr2M  ;
 else if (nel >= 1000000) bun_arr = bun_arr1M  ;
 else if (nel >=  400000) bun_arr = bun_arr400K;
 else if (nel >=  200000) bun_arr = bun_arr200K;
 else if (nel >=  100000) bun_arr = bun_arr100K;
 else if (nel >=   40000) bun_arr = bun_arr40K ;
 else if (nel >=   20000) bun_arr = bun_arr20K ;
 else if (nel >=   10000) bun_arr = bun_arr10K ;
 else if (nel >=    4000) bun_arr = bun_arr4K  ;
 else if (nel >=    2000) bun_arr = bun_arr2K  ;
 else if (nel >=    1000) bun_arr = bun_arr1K  ;
 else /* (nel >=  700) */ bun_arr = bun_arr400 ;
 //else if (nel >=   400) bun_arr = bun_arr400 ;
 //else if (nel >=   200) bun_arr = bun_arr200 ;
 //else if (nel >=   100) bun_arr = bun_arr100 ;
 //else /* (nel >=  64)*/ bun_arr = bun_arr64  ;
 bunkatu = bun_arr[eqcnt];
 assert(nel >= bunkatu,"nel < bunkatu");


 /*区間の個数を決定する  bekiが32の時1～31が区間番号となる  0は使用しない*/
 beki = bunkatu << 1;

#define UP(x) (((x)+(sizeof(char*)-1)) & (-sizeof(char*)))
 {
  size_t SIZ_tmp    = UP(              size                        ); //間接mpsでは下の4個と共用可
  size_t SIZ_g_grp  = UP(              nel           *sizeof(GRP_T));
  size_t SIZ_g_cnt  = UP(              beki          *sizeof(IDX_T));
  size_t SIZ_g_head = UP(              beki          *sizeof(IDX_T));
  size_t SIZ_g_end  = UP(              beki          *sizeof(IDX_T));
  size_t SIZ_stk    = UP((mps_direct ? nel     :  0 )*sizeof(IDX_T)); //間接mpsでは使用しない
  size_t SIZ_ptr    = UP((mps_direct ? bunkatu : nel)*sizeof(PTR_T)); //直接mpsでは上の1個と共用可
 
  tmp = malloc(SIZ_tmp + SIZ_g_grp + SIZ_g_cnt + SIZ_g_head + SIZ_g_end + SIZ_stk + SIZ_ptr);
  if (tmp == NULL) goto indirect_sort;

  g_grp    = (GRP_T *)((char*)tmp    + SIZ_tmp   );
  g_cnt    = (IDX_T *)((char*)g_grp  + SIZ_g_grp );
  g_head   = (IDX_T *)((char*)g_cnt  + SIZ_g_cnt );
  g_end    = (IDX_T *)((char*)g_head + SIZ_g_head);
  stk      = (IDX_T *)((char*)g_end  + SIZ_g_end );
  ptr      = (PTR_T *)((char*)stk    + SIZ_stk   );
 }             
 
 /*サンプルを集める*/ 
 {int delta = ( nel / bunkatu ) * size;    /*サンプルを取り出すときの要素間の距離を決定する*/
  char *tp=(char*)base+delta;
  for (int s = 1; s < bunkatu; s++, tp+=delta) ptr[s] = tp; //直接mpsではptrをサンプル配列のみで使用
 }
 
 /*サンプルをソートする*/
 cmp_org = cmp;   // sizeof(char*)<=8 なので cmp_org が上書きされることはない
 qsort( &ptr[1], bunkatu-1, sizeof(PTR_T), cmp_indirect );
 
 for (g = 1; g < beki; g++) g_cnt[g] = 0; /*要素数をカウントするための初期化*/
 
 for (i = 0; i < nel; i++)   /* base中の各要素がどの区間に入るかを調べる*/
 {
   int g, n, t; char *p = (char*)base+i*size;   /*pは調べようとする要素へのポインタ*/
   
   /************************* beki＝16 の場合 *********************************/
   /*                      4　　　　　　　　　　　　（２分木の根）            */
   /*          2                       6           サンプルの添え字(g>>1の値) */
   /*    1           3           5           7　　　（２分木の葉）            */
   /* -------------------------------------------                             */
   /* 1  2  3  4  5  6  7  8  9 10 11 12 13 14 15　区間番号(添え字)（ｇの値） */
   /***************************************************************************/
   
   for (g = beki>>1, n = g>>1; n >= 2; n >>= 1)
     if ((t = cmp( p, ptr[g>>1] )) > 0) g += n;  /*木構造を右へ進む*/
     else if (t < 0)                    g -= n;  /*木構造を左へ進む*/
     else                               goto xx; /*==時はその区間へ*/
     
   if ((t = cmp( p, ptr[g>>1] )) > 0) g++;       /*２分木の葉の右へ*/
   else if (t < 0)                    g--;       /*２分木の葉の左へ*/
   
   xx:
   g_grp[i] = g;   /*要素iの区間番号を記憶する*/
   g_cnt[g]++;      /*区間gの要素数をカウントアップする*/
 }

 /*各区間の先頭と末尾を決定する*/
 for (i=0, g=1; g<beki; g++) { g_head[g]=i; i+=g_cnt[g]; g_end[g]=i; }
//ここまでは直接mps・間接mpsに共通するプログラムです


 if (mps_direct==0) goto mps_indirect;  // これにより、直接mps・間接mpsを振り分けます


 /*ここからは、直接mpsのプログラムです。（直接mps：配列base中の要素をそのまま区間に再配置する）*/
 int src, src_grp, src_grp0; /*srcは移動元のbase中の位置 src_grpはその区間番号*/
 int dst, dst_grp;           /*dst_grpは移動先の区間番号 dstはそのbase中の位置*/

mps_direct:
 for (sp=stk, src_grp=0;;) {
   do{
     if (++src_grp >= beki) goto mov_end;
     src=g_head[src_grp];
   }while (src == g_end[src_grp]);

   for (;;) {
     for (;;) {
       dst_grp=g_grp[src];
       if (dst_grp != src_grp) break;
       g_head[src_grp]++; src++;
       while (src == g_end[src_grp]) {
         if (++src_grp >= beki) goto mov_end;
         src=g_head[src_grp];
       }
     }
     dst=g_head[dst_grp];
     for (;;) {
       if (dst == g_end[dst_grp]) {g_head[src_grp]++; goto chain_end;}
       if (g_grp[dst] != dst_grp) break;
       g_head[dst_grp]++; dst++;
     }
       
     if (stk == sp) src_grp0=src_grp;  //デバッグ用　なくても動く
     (*sp)=src; sp++; g_head[src_grp]++;
     src    =dst    ;
     src_grp=dst_grp;
   }
   chain_end:
   assert(src_grp0 == dst_grp,"error src_grp0 != dst_grp");

   mmprepare( base, size );
   mmcopy(tmp, H(*stk), size);     mmcopy(H(*stk), H(src), size); dst=src;
   while (--sp != stk) {src=(*sp); mmcopy(H(dst),  H(src), size); dst=src;}
   mmcopy(H(dst), tmp, size);
   
   assert(dst_grp < beki-1,"error dst_grp >= beki-1");
   src_grp=dst_grp;
 }
 mov_end:

 /*奇数番号の区間を一つ一つソートする*/
 for (g=1; g < beki; g+=2) {
   int cnt = g_cnt[g];
   if (cnt >= 2) qsort( (char*)base + (g_end[g]-cnt)*size, cnt, size, cmp );
 }

 free( tmp );
 return;
 /*ここまでが、直接mpsのプログラムです。*/



 /*ここからは、間接mpsのプログラムです。（間接mps：配列baseの全要素を指すptr配列を作ってmpsする）*/
mps_indirect:
 /*各要素のグループ番号を調べて、その要素のあるべき位置（何番目の要素か）を決定し、
   ポインター配列ptrのその位置に当該要素に先頭番地を代入する*/
 {char *ip = base;
  for (i = 0; i < nel; i++) {ptr[g_head[g_grp[i]]++]=ip; ip+=size;} //ここよりptrはbase中の全要素を指す
 }
 // ここですべての区間で g_head[g]==g_end[g] となっている  qs14ではg_end[]は省略されている

 //printf("  - bunkatu=%d\n",bunkatu); // この印刷をすれば、mps の概観がつかめるはずです。
 //for (g=1; g < beki; g++) {
 //  if (g%2) printf("%d ", g_cnt[g]); else printf("[%d] ", g_cnt[g]);
 //  if (g%20==0) puts("");
 //}puts("");

 /*奇数番号の区間を一つ一つソートする*/
 cmp_org = cmp;   // sizeof(char*)<=8 なので cmp_org が上書きされることはない
 for (g=1; g < beki; g+=2) {
   int cnt = g_cnt[g];
   if (cnt >= 2) qsort( &ptr[g_head[g]-cnt], cnt, sizeof(PTR_T), cmp_indirect );
 }

 move_base_by_ptr( base, nel, size, ptr, tmp );
 free( tmp );
 return;
 /*ここまでが、間接mpsのプログラムです。*/
//ここまでが、mps(multi partition sort) のプログラムです
 }
}


static void move_base_by_ptr( void *base, size_t nel, size_t size, void **ptr, char *tmp ) {
 mmprepare( base, size );
 void **tp;  char *ip, *kp; size_t i;
 /*  Knuth vol. 3 (2nd ed.) exercise 5.2-10.  */
 // tp[i]はループの糸口(先頭)を指す。ipは退避要素の元の位置,kpは空き地を指す
 for (tp = ptr, i = 0, ip = base; i < nel; i++, ip += size)
   if ((kp = tp[i]) != ip) {
     size_t j = i;
     char *jp = ip;
     mmcopy(tmp, ip, size);
     do {
       size_t k = (kp - (char *) base) / size;
       tp[j] = jp;
       mmcopy(jp, kp, size);
       j = k;
       jp = kp;
       kp = tp[k];
     }while (kp != ip);
     tp[j] = jp;
     mmcopy(jp, tmp, size);
   }
}
