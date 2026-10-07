C
C***************************************
      SUBROUTINE IQRANF( M, Mmin,Mmax, n)
C***********************************************************************
C                                                                      *
c     IQRANF  - computes a vector of psuedo-random indices             *
c               in the domain (Mmin,Mmax)                              *
C                                                                      *
C     M     - result array ,  psuedo-random positive integers          *
C     Mmin  - input integer,  lower bound for random integers          *
C     Mmax  - input integer,  upper bound for random integers          *
C     n     - input integer,  number of results in M.                  *
C                                                                      *
C       M(i)= Mmin + INT( (Mmax-Mmin) * RANF(0))                       *
C                                                                      *
c        CALL IQRAN0( 256 )                                            *
c        CALL IQRANF( IX, 1,1001, 30)      should produce in IX:       *
c           3  674  435  415  389   54   44  790  900  282             *
c         177  971  728  851  687  604  815  971  155  112             *
c         877  814  779  192  619  894  544  404  496  505  ...        *
C                                                                      *
C     S.K.Park, K.W.Miller, Random Number Generators: Good Ones        *
C     Are Hard To Find, Commun ACM, 31(10), 1192-1201 (1988).          *
C***********************************************************************
C
      IMPLICIT  DOUBLE PRECISION (A-H,K,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,K,O-Z)
Cout  DOUBLE  PRECISION  dq, dp, per, dk, spin, span                    REDUNDNT
C
      dimension  M(n)
      COMMON /IQRAND/ k0, k, k9
c     save k
      CALL TRACE ('IQRANF  ')
            IF( n.LE.0 )  GO TO 73
         inset= Mmin
          span= Mmax - Mmin
c         spin= 16807.00d0
c          per= 2147483647.00d0
          spin= 16807
           per= 2147483647
         realn= n
         scale= 1.0000100d0
             q= scale*(span/realn)
C
            dk= k
      DO  1  i= 1,n
            dp= dk*spin
c           dk=    DMOD( dp, per)
            dk= dp -INT( dp/per)*per
            dq= dk*span
          M(i)= inset + ( dq/ per)
            IF( M(i).LT.Mmin .OR. M(i).GT.Mmax )  M(i)= inset + i*q
    1 continue
             k= dk
C
C
ciC     double precision  k, ip, iq, id
ci         inset= Mmin
ci         ispan= Mmax - Mmin
ci         ispin= 16807
ci            id= 2147483647
ci             q= (REAL(ispan)/REAL(n))*1.00001
ciC
ci      DO  2  i= 1,n
ci            ip= k*ispin
ci             k=      MOD( ip, id)
ci            iq= k*ispan
ci          M(i)= inset + ( iq/ id)
ci            IF( M(i).LT.Mmin .OR. M(i).GT.Mmax )  M(i)= inset + i*q
ci    2 continue
C
            CALL TRAP( M, 8H IQRANF  , 1, Mmax, n)
C
   73 CONTINUE
      CALL TRACK ('IQRANF  ')
      RETURN
c     DATA  k /256/
c                        IQRANF TEST PROGRAM:
c      parameter( nrange= 10000, nmaps= 1001 )
c      DIMENSION  IX(nrange), IY(nmaps), IZ(nmaps), IR(nmaps)
c      COMMON /IQRAND/ k0, k, k9
cc
c        CALL  LINK( 'UNIT6=( output,create,text)//')
c         iou= 8
c      DO 7 j= 1,256,255
c      CALL IQRAN0( j )
c      CALL IQRANF( IX, 1, nmaps, nrange)
c      DO 1 i= 1,nmaps
c       IY(i)= 0
c    1  IZ(i)= 0
cc                     census for each index generated in (1:nmaps)
c      DO 2 i= 1,nrange
c    2 IY( IX(i))= IY( IX(i)) + 1
cc                     distribution of census tallies about nrange/nmaps
c      DO 3 i= 1,nmaps
c    3 IZ( IY(i))= IZ( IY(i)) + 1
c       IR(1)= IZ(1)
cc                     integral of distribution
c      DO 4 i= 1,nmaps
c    4  IR(i)= IR(i-1) + IZ(i)
c      WRITE( iou,112)   j, IR(nmaps), k
c      WRITE( iou,113) ( IX(i), i= 1,20 )
c      WRITE( iou,113) ( IY(i), i= 1,20 )
c      WRITE( iou,113) ( IZ(i), i= 1,20 )
c      WRITE( iou,113) ( IR(i), i= 1,20 )
c  112 FORMAT(/,1X,4I20)
c  113 FORMAT(20I4)
c    7 continue
c      STOP
c
c                   1                1000          1043618065
c  1 132 756 459 533 219  48 679 680 935 384 520 831  35  54 530 672   8 384  67
c 17  12   7  10  10  10  10  12   9   9   4  15  10   7   7   9   9   9  10  11
c  0   1   8  19  40  60  86 109 133 128 107 104  70  52  39  26   7   7   2   2
c  0   1   9  28  68 128 214 323 456 584 691 795 865 917 956 982 989 996 9981000
c
c                 256                1000           878252412
c  3 674 435 415 389  54  44 790 900 282 177 971 728 851 687 604 815 971 155 112
c 11  17  19   6  11  11   7   9  12   7  13   7   9  11  14   9   9  12   9   9
c  1   2  10  16  30  71  93 109 131 119 118 105  69  47  28  15  15   9   5   3
c  1   3  13  29  59 130 223 332 463 582 700 805 874 921 949 964 979 988 993 996
      END
