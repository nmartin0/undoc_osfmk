C
C
C***************************************
      SUBROUTINE CALIBR
C***********************************************************************
C                                                                      *
c     CALIBR - Cpu clock calibration tests accuracy of SECOND function.*
C                                                                      *
C     CALIBR tests function SECOND by using it to time a computation   *
C     repeatedly.  These SECOND timings are written to stdout(terminal)*
C     one at a time as the cpu-clock is read, so we can observe a real *
C     external clock time and thus check the accuracy of SECOND code.  *
C     Comparisons with an external clock require a stand-alone run.    *
C     Otherwise compare with system charge for total job cpu time.     *
C                                                                      *
C     Sample Output from CRAY-YMP1:                                    *
C                                                                      *
C                                                                      *
C CPU CLOCK CALIBRATION:  START STOPWATCH NOW !                        *
C           TESTS ACCURACY OF FUNCTION SECOND()                        *
C           Monoprocess this test, stand-alone, no TSS                 *
C           Verify  T or DT  observe external clock:                   *
C                                                                      *
C        -------     -------      ------      -----                    *
C        Total T ?   Delta T ?    Mflops ?    Flops                    *
C        -------     -------      ------      -----                    *
C  1        0.00        0.00        9.15    4.00000e+04    4.98000e-02 *
C  2        0.01        0.01       11.67    1.20000e+05    8.98000e-02 *
C  3        0.02        0.01       12.84    2.80000e+05    1.69800e-01 *
C  4        0.04        0.02       13.47    6.00000e+05    3.29800e-01 *
C  5        0.09        0.05       13.81    1.24000e+06    6.49800e-01 *
C  6        0.18        0.09       14.00    2.52000e+06    1.28980e+00 *
C  7        0.36        0.18       14.12    5.08000e+06    2.56980e+00 *
C  8        0.72        0.36       14.19    1.02000e+07    5.12980e+00 *
C  9        1.44        0.72       14.20    2.04400e+07    1.02498e+01 *
C 10        2.88        1.44       14.23    4.09200e+07    2.04898e+01 *
C 11        5.74        2.87       14.27    8.18800e+07    4.09698e+01 *
C 12       11.48        5.74       14.27    1.63800e+08    8.19298e+01 *
C 13       22.98       11.50       14.26    3.27640e+08    1.63850e+02 *
C 14       45.92       22.94       14.27    6.55320e+08    3.27690e+02 *
C 15       91.88       45.96       14.26    1.31068e+09    6.55369e+02 *
C***********************************************************************
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,O-Z)
C
      parameter( limitn= 101, ndim= limitn+10 )
      DIMENSION  X(ndim), Y(ndim), cumtim(10)
C
c     CALL TRACE ('CALIBR  ')
      cumtim(1)= 0.0d0
             t0= SECOND( cumtim(1))
C
          WRITE( *,111)
          WRITE( *,110)
          WRITE( *,112)
          WRITE( *,113)
          WRITE( *,114)
          WRITE( *,115)
          WRITE( *,114)
  111    FORMAT(//,' CPU CLOCK CALIBRATION:  START STOPWATCH NOW !')
  110    FORMAT('           TESTS ACCURACY OF FUNCTION SECOND()')
  112    FORMAT('           Monoprocess this test, stand-alone, no TSS')
  113    FORMAT('           Verify  T or DT  observe external clock:',/)
  114    FORMAT('           -------     -------      ------      -----')
  115    FORMAT('           Total T ?   Delta T ?    Mflops ?    Flops')
  119    FORMAT(4X,I2,3F12.2,2E15.5)
C
              l= 0
              n= 0
              m= 200
          nflop= 0
         totalt= 0.00d0
         deltat= 0.00d0
          flops= 0.00d0
             rn= 0.00d0
             t1= 0.00d0
             t2= 0.00d0
      cumtim(1)= 0.0d0
             t2= SECOND( cumtim(1))
             IF( t2.GT. 1.00d04 ) GO TO 911
             IF( t2.LT. 1.00d-8 ) GO TO 911
C
   10         l= l + 1
              m= m + m
C
           X(1)= 0.0098000d0
           Y(1)= 0.0000010d0
         DO 2 i= 2,limitn
           Y(i)= Y(1)
    2 continue
C                                  Compute LFK Kernel 11  m times
         DO 5 j= 1,m
         DO 4 k= 2,limitn
           X(k)= X(k-1) + Y(k)
    4 continue
           X(1)= X(limitn)
    5 continue
C
             t1= t2
      cumtim(1)= 0.0d0
             t2= SECOND( cumtim(1))
C                                  IF elapsed time can be observed, Print Mark.
         totalt= t2 - t0
         deltat= t2 - t1
          nflop= nflop + (limitn - 1) * m
             IF( deltat .GT. 2.00d0  .OR.  l.GT.12 )  THEN
                     n= n + 1
                    rn= REAL( nflop)
                 flops= 1.00d-6 *( REAL( nflop)/( totalt +1.00d-9))
                 WRITE( *,119)  l, totalt, deltat, flops, rn, X(limitn)
             ENDIF
             IF( deltat .LT. 200.0d0  .OR.  n.LT.3 )  GO TO 10
C
             IF( n.LE.0 )  THEN
                 WRITE( *,119)  l, totalt, deltat, flops, rn, X(limitn)
             ENDIF
      STOP
C
  911     WRITE( *,61)
          WRITE( *,62) totalt
      STOP
C
   61 FORMAT(1X,'FATAL(CALIBR): cant measure time using func SECOND()')
   62 FORMAT(/,13X,'using SECOND():  totalt=',1E20.8,' ?')
C
      END
