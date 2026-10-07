C
C
C***********************************************
      SUBROUTINE SIZES(i)
C***********************************************
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,O-Z)
C
C            SIZES      test and set the loop controls before each kernel test
C
C     i    :=  kernel number
C
C     mk    :=  number of kernels to test
C     Nruns :=  number of timed runs of complete test.
C     tclock:=  cpu clock resolution or minimum time in seconds.
C     Loop  :=  multiple pass control to execute kernel long enough to time.
C     n     :=  DO loop control for each kernel.
C     ******************************************************************
C
C
C/      PARAMETER( l1= 1001, l2=  101, l1d= 2*1001 )
C/      PARAMETER( l13=  64, l13h= l13/2, l213= l13+l13h, l813= 8*l13 )
C/      PARAMETER( l14=2048, l16=  75, l416= 4*l16 , l21= 25 )
C
C/      PARAMETER( l1=   27, l2=   15, l1d= 2*1001 )
C/      PARAMETER( l13= 8, l13h= 8/2, l213= 8+4, l813= 8*8 )
C/      PARAMETER( l14=  16, l16= 15, l416= 4*15 , l21= 15)
C
C/      PARAMETER( l1=   1001, l2=   101, l1d= 2*1001 )
C/      PARAMETER( l13= 64, l13h= 64/2, l213= 64+32, l813= 8*64 )
C/      PARAMETER( l14= 2048, l16= 75, l416= 4*75 , l21= 25)
C
C/      PARAMETER( kn= 47, kn2= 95, np= 3, ls= 3*47, krs= 24)
C
C/      PARAMETER( NNI=  2*l1 +2*l213 +l416 )
C/      PARAMETER( NN1= 16*l1 +13*l2 +2*l416 + l14 )
C/      PARAMETER( NN2= 4*l813 + 3*l21*l2 +121*l2 +3*l13*l13 )
C/      PARAMETER( Nl1= 19*l1, Nl2= 131*l2 +3*l21*l2 )
C/      PARAMETER( Nl13= 3*l13*l13 +34*l13 +32)
C
C/      PARAMETER( nk= 47, nl= 3, nr= 8 )
C
      COMMON /ALPHA/ mk,ik,im,ml,il,Mruns,Nruns,jr,iovec,NPFS(8,3,47)
      COMMON /TAU/   tclock, tsecov, testov, cumtim(4)
      COMMON /BETA / tic, TIMES(8,3,47), SEE(5,3,8,3),
     1              TERRS(8,3,47), CSUMS(8,3,47),
     2              FOPN(8,3,47), DOS(8,3,47)
C
C
      COMMON /SPACES/ ion,j5,k2,k3,MULTI,laps,Loop,m,kr,LP,n13h,ibuf,nx,
     1 L,npass,nfail,n,n1,n2,n13,n213,n813,n14,n16,n416,n21,nt1,nt2,
     2 last,idebug,mpy,Loops2,mucho,mpylim, intbuf(16)
C
      COMMON /SPACER/ A11,A12,A13,A21,A22,A23,A31,A32,A33,
     2                AR,BR,C0,CR,DI,DK,
     3  DM22,DM23,DM24,DM25,DM26,DM27,DM28,DN,E3,E6,EXPMAX,FLX,
     4  Q,QA,R,RI,S,SCALE,SIG,STB5,T,XNC,XNEI,XNM
C
      COMMON /SPACE0/ TIME(47), CSUM(47), WW(47), WT(47), ticks,
     1                FR(9), TERR1(47), SUMW(7), START,
     2              SKALE(47), BIAS(47), WS(95), TOTAL(47), FLOPN(47),
     3                IQ(7), NPF, NPFS1(47)
C
      COMMON /SPACEI/ WTP(3), MUL(3), ISPAN(47,3), IPASS(47,3)
C
C     ******************************************************************
C
      CALL TRACE ('SIZES   ')
C
      nif= 0
c                        Set  mk .LE. 47  number of kernels to test.
             mk= 24
             im= 1
             ml= 3
c                        Set  Nruns .LT. 8  number of timed runs of KERNEL test
c                        Set  Nruns= 1   to REDUCE RUN TIME for debug runs.
          Nruns= 1
c                        Set  Nruns= 7   for Standard BENCHMARK Test. Maximum.
          Nruns= 7
             IF( Nruns.GT. 7) Nruns= 7
c
c                        Set  Mruns= 7   for Standard BENCHMARK Test.
          Mruns= Nruns
c
C****************************************************************************
c         OPTIONAL LONG ENDURANCE TEST FOR NEW HARDWARE ACCEPTANCE TESTING.
c         OPTIONAL       Set  Mruns=     for Hardware ENDURANCE TRIAL
c
c         Mruns= Nruns * ( Desired Trial Time(sec) / totjob Time(sec))
c                          where totjob-time is LFK Standard benchmark
c                          test Job-time printed at end of output file.
c
c   e.g.  12 Hour run on CRAY-XMP :   laps = 43200./ 17.5 = 2468
c         12 Hour run on VaxS3500 :   laps = 43200./478.4 =   90
c
c          laps= 1
C****************************************************************************
C
          Mruns= Nruns * laps
      IF( Mruns.LT.Nruns .OR. Mruns.GT.500000 ) Mruns= Nruns
C
      IF( i.EQ.-1)  GO TO 73
C
C****************************************************************************
C     Domain tests follow to detect overstoring of controls for array opns.
C****************************************************************************
C
      nif= 1
      iup= 999000
      IF( iup.LT.65000 ) iup= 65000
      IF( i.LT.1 .OR.  (i-1).GT.  24)      GO TO 911
      IF( n.LT.0 .OR.  n.GT.   1001)          GO TO 911
      IF(Loop.LT.0 .OR. Loop.GT.iup)        GO TO 911
C
      nif= 2
      IF(  il.LT.1 .OR. il.GT.3 )  GO TO 911
                 n= ISPAN(i,il)
      Loop        = IPASS(i,il) * MUL(il)
      Loop = MULTI * Loop
      LP   = Loop
c
c
c
c MULTI= 10
c        ------    ------    ------   -------   -------   ------------
c        kernel    L:Loop    n:loop   flops*1   flops*n   flops*n*Loop
c        ------    ------    ------   -------   -------   ------------
c   il= 1     1        70      1001         5      5005    350350
c             2       670        97         4       388    259960
c             3        90      1001         2      2002    180180
c             4       140       600         2      1200    168000
c             5       100      1000         2      2000    200000
c             6        30      1984         2      3968    119040
c             7        40       995        16     15920    636800
c             8       100       198        36      7128    712800
c             9       360       101        17      1717    618120
c            10       340       101         9       909    309060
c            11       110      1000         1      1000    110000
c            12       120      1000         1      1000    120000
c            13       360        64         7       448    161280
c            14        20      1001        11     11011    220220
c            15        10       500        33     16500    165000
c            16       250        53        10       530    132500
c            17       350       101         9       909    318150
c            18        20       495        44     21780    435600
c            19       390       101         6       606    236340
c            20        10      1000        26     26000    260000
c            21        10     63125         2    126250   1262500
c            22       110       101        17      1717    188870
c            23        80       495        11      5445    435600
c            24        50      1000         1      1000     50000
c   il= 2     1       800       101         5       505    404000
c             2       800        97         4       388    310400
c             3      1060       101         2       202    214120
c             4      1400        60         2       120    168000
c             5      1100       100         2       200    220000
c             6       140       480         2       960    134400
c             7       440       101        16      1616    711040
c             8       120       198        36      7128    855360
c             9       420       101        17      1717    721140
c            10       380       101         9       909    345420
c            11      1280       100         1       100    128000
c            12      1360       100         1       100    136000
c            13       820        32         7       224    183680
c            14       200       101        11      1111    222200
c            15        20       500        33     16500    330000
c            16       540        28        10       280    151200
c            17       400       101         9       909    363600
c            18        20       495        44     21780    435600
c            19       460       101         6       606    278760
c            20       160       100        26      2600    416000
c            21        20     31250         2     62500   1250000
c            22       140       101        17      1717    240380
c            23       100       495        11      5445    544500
c            24       620       100         1       100     62000
c   il= 3     1      2240        27         5       135    302400
c             2      3680        11         4        44    161920
c             3      2960        27         2        54    159840
c             4      3040        15         2        30     91200
c             5      3200        26         2        52    166400
c             6      1680        24         2        48     80640
c             7      1600        21        16       336    537600
c             8       720        26        36       936    673920
c             9      2080        15        17       255    530400
c            10      2000        15         9       135    270000
c            11      3680        26         1        26     95680
c            12      3840        26         1        26     99840
c            13      2480         8         7        56    138880
c            14       640        27        11       297    190080
c            15        80        70        33      2310    184800
c            16      1120        11        10       110    123200
c            17      2080        15         9       135    280800
c            18       160        65        44      2860    457600
c            19      2240        15         6        90    201600
c            20       560        26        26       676    378560
c            21        80     12500         2     25000   2000000
c            22       640        15        17       255    163200
c            23       560        65        11       715    400400
c            24      1840        26         1        26     47840
c
Computers with high resolution clocks tic= O(microsec.) should use Loop= 1
C     to show un-initialized as well as encached execution rates.
C
C     Loop= 1
C
      IF( Loop.LT. 1)   Loop= 1
      LP  = Loop
      L   = 1
      mpy = 1
      nif = 3
      IF( n.LT.0 .OR.  n.GT.   1001)  GO TO 911
      IF(Loop.LT.0 .OR. Loop.GT.iup)  GO TO 911
      n1  = 1001
      n2  = 101
      n13 = 64
      n13h= 32
      n213= 96
      n813= 512
      n14 = 2048
      n16 = 75
      n416= 300
      n21 = 25
C
      nt1= 16*1001 +13*101 +2*300 + 2048
      nt2= 4*512 + 3*25*101 +121*101 +3*64*64
C
   73 CONTINUE
      CALL TRACK ('SIZES   ')
      RETURN
C
C
  911 io= ABS( ion)
      IF( io.LE.0 .OR. io.GT.10 ) io=6
      WRITE( io,913) i, nif, n, Loop, il
  913 FORMAT(1H1,///,37H FATAL OVERSTORE/ DATA LOSS.  TEST=  ,6I6)
      CALL WHERE(0)
C
      END
