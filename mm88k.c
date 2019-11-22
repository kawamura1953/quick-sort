/***************************************************/
/*                                                 */
/*          mm88   (配列の要素のスワップ関数)      */
/*                                                 */
/* by 河村 知行 (kawamura tomoyuki)    2019.8.18   */
/*  〒745-0845 山口県周南市河東町3-2 （株）曽呂利  */
/*             t-kawa@crux.ocn.ne.jp               */
/***************************************************/

/*８バイト整数(long long int)を使用して高速化  繰り返しは8回(64byte)分展開する*/

static unsigned int mmsize, high, low;  //mmkind, , high32_4, mmquick

#define A8 ((long long int*)a)
#define B8 ((long long int*)b)
#define C8 ((long long int*)c)
#define A4 ((int*)a)
#define B4 ((int*)b)
#define C4 ((int*)c)

#define  MV8(i) {A8[i] = B8[i];} 
#define  MV4(i) {A4[i] = B4[i];} 
#define  MV1(i) { a[i] =  b[i];} 
#define  SW8(i) {long long int v; v = A8[i]; A8[i] = B8[i]; B8[i] = v;} 
#define  SW4(i) {          int v; v = A4[i]; A4[i] = B4[i]; B4[i] = v;} 
#define  SW1(i) {         char v; v =  a[i];  a[i] =  b[i];  b[i] = v;} 
#define  RT8(i) {long long int v; v = A8[i]; A8[i] = B8[i]; B8[i] = C8[i]; C8[i] = v;}
#define  RT4(i) {          int v; v = A4[i]; A4[i] = B4[i]; B4[i] = C4[i]; C4[i] = v;}
#define  RT1(i) {         char v; v =  a[i];  a[i] =  b[i];  b[i] =  c[i];  c[i] = v;}

#define HIGHLOW(HIGH,LOW,MOV,WS,CC) { \
   if (HIGH) { \
     char *e = a + HIGH; \
     do {MOV(0) MOV(1) MOV(2) MOV(3) MOV(4) MOV(5) MOV(6) MOV(7) CC; b += 8*WS; a += 8*WS; \
     }while (a < e); \
   } \
   switch ((LOW) & 7) { \
     case 7: MOV(6)  \
     case 6: MOV(5)  \
     case 5: MOV(4)  \
     case 4: MOV(3)  \
     case 3: MOV(2)  \
     case 2: MOV(1)  \
     case 1: MOV(0)  \
     case 0: {}      \
   } \
} 
   //case 7:MOV(6) case 6:MOV(5) case 5:MOV(4) case 4:MOV(3) case 3:MOV(2) case 2:MOV(1) case 1:MOV(0) case 0:{}\

static void sbfnc8n ( char *a, char *b, int size ) HIGHLOW((size&(-64)),((size&(64-1))/8),SW8,8,1==1)
static void sbfnc4n ( char *a, char *b, int size ) HIGHLOW((size&(-32)),((size&(32-1))/4),SW4,4,1==1)
static void sbfnc1n ( char *a, char *b, int size ) HIGHLOW((size&( -8)),((size&( 8-1))/1),SW1,1,1==1)

static void rtfnc8n ( char *a, char *b, char *c ) HIGHLOW(high,low,RT8,8,c+=64)
static void rtfnc4n ( char *a, char *b, char *c ) HIGHLOW(high,low,RT4,4,c+=32)
static void rtfnc1n ( char *a, char *b, char *c ) HIGHLOW(high,low,RT1,1,c+= 8)
static void rtfnc8  ( char *a, char *b, char *c ) RT8(0)
static void rtfnc4  ( char *a, char *b, char *c ) RT4(0)

static void swfnc8n ( char *a, char *b ) HIGHLOW(high,low,SW8,8,1==1)
static void swfnc4n ( char *a, char *b ) HIGHLOW(high,low,SW4,4,1==1)
static void swfnc1n ( char *a, char *b ) HIGHLOW(high,low,SW1,1,1==1)
static void swfnc8  ( char *a, char *b ) SW8(0)
static void swfnc4  ( char *a, char *b ) SW4(0)

static void mvfnc8n ( char *a, char *b, int size ) HIGHLOW(high,low,MV8,8,1==1)
static void mvfnc4n ( char *a, char *b, int size ) HIGHLOW(high,low,MV4,4,1==1)
//static void mvfnc1n ( char *a, char *b, int size ) HIGHLOW(high,low,MV1,1,1==1)

static void (*sbfnc)( char *, char *, int    );
static void (*rtfnc)( char *, char *, char * );
static void (*swfnc)( char *, char *         );
static void (*mvfnc)( char *, char *, int    );

static inline void mmswapblock(char*a,char*b, int size){ASS_CNT((size/mmsize)*2); (*sbfnc)(a,b,size);}
static inline void mmrot3  (char *a, char *b, char *c ){ASS_CNT(3); (*rtfnc)(a,b,c);}
static inline void mmswap  (char *a, char *b          ){ASS_CNT(2); (*swfnc)(a,b);}
static inline void mmcopy  (char *a, char *b, int size){ASS_CNT(1); (*mvfnc)(a,b,size);}


//以下、「ポインタが８バイト(sizeof(char*)==8)なら、機械語の８バイト整数あり」と仮定している。
//「ポインタが４バイトなのに、機械語の８バイト整数あり」では、十分な性能はでない。
#define ENINT(x)  ((char*)(x) - (char*)0)
#define INT64_OK  (sizeof(char*)==8)

static void mmprepare( void *base, int siz )
{
 assert(sizeof(          int) == 4 , "sizeof(          int) != 4"); //cygwin64はsizeof(long int)==8
 assert(sizeof(long long int) == 8 , "sizeof(long long int) != 8");
 assert(                 siz  >= 1 , "mmsize <= 0"               );
 mmsize = siz;

 if (INT64_OK && (ENINT(base)&(8-1))==0 && (siz&(8-1))==0) {
   high=(siz&(-64)); low=(siz&(64-1))/8; sbfnc=sbfnc8n;
                                         rtfnc = (siz==8   ? rtfnc8  : rtfnc8n);
                                         swfnc = (siz==8   ? swfnc8  : swfnc8n);
                                         mvfnc = (siz<=608 ? mvfnc8n : (void(*)())memcpy);
 }else if (      (ENINT(base)&(4-1))==0 && (siz&(4-1))==0) {
   high=(siz&(-32)); low=(siz&(32-1))/4; sbfnc=sbfnc4n;
                                         rtfnc = (siz==4   ? rtfnc4  : rtfnc4n);
                                         swfnc = (siz==4   ? swfnc4  : swfnc4n);
                                         mvfnc = (siz<=532 ? mvfnc4n : (void(*)())memcpy);
 }else                                                     {
   high=(siz&( -8)); low=(siz&( 8-1))/1; 
                                         sbfnc=sbfnc1n;
                                         rtfnc=rtfnc1n;
                                         swfnc=swfnc1n;
   /*if (siz<24 || (siz<96&&(siz&4))) */ mvfnc=(void(*)())memcpy; // mvfnc= は、mmprepare2 でもよい
 }
}
