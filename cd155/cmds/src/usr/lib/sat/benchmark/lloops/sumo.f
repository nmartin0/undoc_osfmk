C
C***********************************************
      FUNCTION SUMO( V,n)
C***********************************************
C
C     CHECK-SUM WITH ORDINAL DEPENDENCY.
C
C     V   - input array,   floating-point numbers
C     n   - input integer, number of elements in V.
C
C***********************************************
                DOUBLE PRECISION  SUMO, V
cIBM  REAL*8            SUMO, V
c
c     Use the following Double Precision declaration to improve Real*4 tests.
c     Use the following Real*16          declaration to improve Real*8 tests.
c
         DOUBLE     PRECISION  S
cIBM  REAL*16           S
C
      DIMENSION  V(n)
C
      CALL TRACE ('SUMO    ')
           S= 0.00d0
C
      DO 1 k= 1,n
    1      S= S + REAL(k)*V(k)
       SUMO = S
      CALL TRACK ('SUMO    ')
      RETURN
      END
