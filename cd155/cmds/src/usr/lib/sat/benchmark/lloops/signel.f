C
C
C***********************************************
      SUBROUTINE  SIGNEL( V, SCALE,BIAS, n)
C***********************************************
C
C    SIGNEL GENERATES VERY FRIENDLY FLOATING-POINT NUMBERS NEAR 1.0
C                     WHEN SCALE= 1.0 AND BIAS= 0.
C
C     V      - result array,  floating-point test data
C     SCALE  - input scalar,  scales magnitude of results
C     BIAS   - input scalar,  offsets magnitude of results
C     n      - input integer, number of results in V.
C
C***********************************************
                DOUBLE PRECISION  V, SCALE, BIAS
cIBM  REAL*8            V, SCALE, BIAS
c
c     Use the following Double Precision declaration to improve Real*4 tests.
c     Use the following Real*16          declaration to improve Real*8 tests.
c
         DOUBLE     PRECISION  SCALED,BIASED,FUZZ,BUZZ,FIZZ,ONE
cIBM  REAL*16           SCALED,BIASED,FUZZ,BUZZ,FIZZ,ONE
      DIMENSION  V(n)
C
      CALL TRACE ('SIGNEL  ')
C
        SCALED= SCALE
        BIASED= BIAS
C
        SCALED= 10.00d0
        SCALED=  1.00d0/SCALED
        BIASED=  0.00d0
C
C         FUZZ= 1.234500d-9
          FUZZ= 1.234500d-3
          BUZZ= 1.000d0  + FUZZ
          FIZZ= 1.100d0  * FUZZ
           ONE= 1.000d0
C
        DO 1 k= 1,n
          BUZZ= (ONE - FUZZ)*BUZZ +FUZZ
          FUZZ= -FUZZ
c         V(k)=((BUZZ- FIZZ) -BIASED)*SCALED
          V(k)= (BUZZ- FIZZ)*SCALED
    1 CONTINUE
C
      CALL TRACK ('SIGNEL  ')
      RETURN
      END
