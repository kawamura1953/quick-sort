#!/bin/sh

cc  -O  -o qsort      -w   main_prog.c         # 自システム用の計測プログラムを生成

cc  -O                -w                -c qsnewlib.c    # -DDEBUG で代入回数を計測
cc  -O  -o qsnewlib   -w   main_prog.c     qsnewlib.o

cc  -O                -w                -c qs_glibc.c    # -DDEBUG で代入回数を計測
cc  -O  -o qs_glibc   -w   main_prog.c     qs_glibc.o

cc  -O                -w                -c qs15c2.c      # -DDEBUG で代入回数を計測
cc  -O  -o qs15c2     -w   main_prog.c     qs15c2.o

cc  -O                -w                -c qs16e2.c      # -DDEBUG で代入回数を計測
cc  -O  -o qs16e2     -w   main_prog.c     qs16e2.o

prin=benchmark

echo '----------------- benchmark.txt begin -------------------- '     >$prin.txt
echo ' '                                                              >>$prin.txt
echo 'キー種別:同値なし　要素数:1万個　要素サイズ:8,80,1000byte '     >>$prin.txt
 time ./qsort       -3    10000     0       5000   -1  -1  -1  0      >>$prin.txt
 time ./qsnewlib    -3    10000     0       5000   -1  -1  -1  0      >>$prin.txt
 time ./qs_glibc    -3    10000     0       5000   -1  -1  -1  0      >>$prin.txt
 time ./qs15c2      -3    10000     0       5000   -1  -1  -1  0      >>$prin.txt
 time ./qs16e2      -3    10000     0       5000   -1  -1  -1  0      >>$prin.txt
 echo ' '                                                             >>$prin.txt
 time ./qsort       -3    10000    80       5000   -1  -1  -1  0      >>$prin.txt
 time ./qsnewlib    -3    10000    80       5000   -1  -1  -1  0      >>$prin.txt
 time ./qs_glibc    -3    10000    80       5000   -1  -1  -1  0      >>$prin.txt
 time ./qs15c2      -3    10000    80       5000   -1  -1  -1  0      >>$prin.txt
 time ./qs16e2      -3    10000    80       5000   -1  -1  -1  0      >>$prin.txt
 echo ' '                                                             >>$prin.txt
 time ./qsort       -3    10000  1000       1000   -1  -1  -1  0      >>$prin.txt
 time ./qsnewlib    -3    10000  1000       1000   -1  -1  -1  0      >>$prin.txt
 time ./qs_glibc    -3    10000  1000       1000   -1  -1  -1  0      >>$prin.txt
 time ./qs15c2      -3    10000  1000       1000   -1  -1  -1  0      >>$prin.txt
 time ./qs16e2      -3    10000  1000       1000   -1  -1  -1  0      >>$prin.txt
 echo ' '                                                             >>$prin.txt
echo 'キー種別:100種　要素数:1万個　要素サイズ:8,80,1000byte '        >>$prin.txt
 time ./qsort      100    10000     0      10000   -1  -1  -1  0      >>$prin.txt
 time ./qsnewlib   100    10000     0      10000   -1  -1  -1  0      >>$prin.txt
 time ./qs_glibc   100    10000     0      10000   -1  -1  -1  0      >>$prin.txt
 time ./qs15c2     100    10000     0      10000   -1  -1  -1  0      >>$prin.txt
 time ./qs16e2     100    10000     0      10000   -1  -1  -1  0      >>$prin.txt
 echo ' '                                                             >>$prin.txt
 time ./qsort      100    10000    80      10000   -1  -1  -1  0      >>$prin.txt
 time ./qsnewlib   100    10000    80      10000   -1  -1  -1  0      >>$prin.txt
 time ./qs_glibc   100    10000    80      10000   -1  -1  -1  0      >>$prin.txt
 time ./qs15c2     100    10000    80      10000   -1  -1  -1  0      >>$prin.txt
 time ./qs16e2     100    10000    80      10000   -1  -1  -1  0      >>$prin.txt
 echo ' '                                                             >>$prin.txt
 time ./qsort      100    10000  1000       2000   -1  -1  -1  0      >>$prin.txt
 time ./qsnewlib   100    10000  1000       2000   -1  -1  -1  0      >>$prin.txt
 time ./qs_glibc   100    10000  1000       2000   -1  -1  -1  0      >>$prin.txt
 time ./qs15c2     100    10000  1000       2000   -1  -1  -1  0      >>$prin.txt
 time ./qs16e2     100    10000  1000       2000   -1  -1  -1  0      >>$prin.txt
 echo ' '                                                             >>$prin.txt
echo 'キー種別:2種　要素数:1万個　要素サイズ:8,80,1000byte '          >>$prin.txt
 time ./qsort        2    10000     0      20000   -1  -1  -1  0      >>$prin.txt
 time ./qsnewlib     2    10000     0      20000   -1  -1  -1  0      >>$prin.txt
 time ./qs_glibc     2    10000     0      20000   -1  -1  -1  0      >>$prin.txt
 time ./qs15c2       2    10000     0      20000   -1  -1  -1  0      >>$prin.txt
 time ./qs16e2       2    10000     0      20000   -1  -1  -1  0      >>$prin.txt
 echo ' '                                                             >>$prin.txt
 time ./qsort        2    10000    80      20000   -1  -1  -1  0      >>$prin.txt
 time ./qsnewlib     2    10000    80      20000   -1  -1  -1  0      >>$prin.txt
 time ./qs_glibc     2    10000    80      20000   -1  -1  -1  0      >>$prin.txt
 time ./qs15c2       2    10000    80      20000   -1  -1  -1  0      >>$prin.txt
 time ./qs16e2       2    10000    80      20000   -1  -1  -1  0      >>$prin.txt
 echo ' '                                                             >>$prin.txt
 time ./qsort        2    10000  1000       3000   -1  -1  -1  0      >>$prin.txt
 time ./qsnewlib     2    10000  1000       3000   -1  -1  -1  0      >>$prin.txt
 time ./qs_glibc     2    10000  1000       3000   -1  -1  -1  0      >>$prin.txt
 time ./qs15c2       2    10000  1000       3000   -1  -1  -1  0      >>$prin.txt
 time ./qs16e2       2    10000  1000       3000   -1  -1  -1  0      >>$prin.txt
 echo ' '                                                             >>$prin.txt
 echo ' '                                                             >>$prin.txt
echo 'キー種別:10種　要素数:1万,10万,100万個　要素サイズ:400byte '    >>$prin.txt
 time ./qsort       10    10000   400       2000   -1  -1  -1  0      >>$prin.txt
 time ./qsnewlib    10    10000   400       2000   -1  -1  -1  0      >>$prin.txt
 time ./qs_glibc    10    10000   400       2000   -1  -1  -1  0      >>$prin.txt
 time ./qs15c2      10    10000   400       2000   -1  -1  -1  0      >>$prin.txt
 time ./qs16e2      10    10000   400       2000   -1  -1  -1  0      >>$prin.txt
 echo ' '                                                             >>$prin.txt
 time ./qsort       10   100000   400        200   -1  -1  -1  0      >>$prin.txt
 time ./qsnewlib    10   100000   400        200   -1  -1  -1  0      >>$prin.txt
 time ./qs_glibc    10   100000   400        200   -1  -1  -1  0      >>$prin.txt
 time ./qs15c2      10   100000   400        200   -1  -1  -1  0      >>$prin.txt
 time ./qs16e2      10   100000   400        200   -1  -1  -1  0      >>$prin.txt
 echo ' '                                                             >>$prin.txt
 time ./qsort       10  1000000   400         20   -1  -1  -1  0      >>$prin.txt
 time ./qsnewlib    10  1000000   400         20   -1  -1  -1  0      >>$prin.txt
 time ./qs_glibc    10  1000000   400         20   -1  -1  -1  0      >>$prin.txt
 time ./qs15c2      10  1000000   400         20   -1  -1  -1  0      >>$prin.txt
 time ./qs16e2      10  1000000   400         20   -1  -1  -1  0      >>$prin.txt
 echo ' '                                                             >>$prin.txt
echo '=================  benchmark.txt end  ==================== '    >>$prin.txt
echo '各行の最後の数値がソート１回あたりの処理時間(10μ秒単位)です '  >>$prin.txt
echo ' '                                                              >>$prin.txt
