c
c
c
C
C***********************************************************************
      FUNCTION  SECOVT( iou )
C***********************************************************************
C                                                                      *
C     SECOVT  - Measures the Overhead time for calling SECOND
C      toler  - tolerance for convergence= Relative error :  0.02
C        iou  - I/O unit number
C***********************************************************************
C
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,O-Z)
CLOX  REAL*8 SECOND
C
      DIMENSION   TIM(20), TER(20), TMX(20), INX(20)
      COMMON /FAKE1/ t0,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11(20),t12(20)
      COMMON /FAKE2/ tcum(20)
C
      CALL TRACE ('SECOVT  ')
C
C***********************************************************************
C     Measure  tsecov:  Overhead time for calling SECOND
C***********************************************************************
C
         tseco= 0.000d0
           klm= 1600
            io= ABS(iou)
            jj= 0
C
      DO 820 j= 1,15
C
      DO 803 i= 1,10
        t12(i)= 0.000d0
  803 continue
       tcum(1)= 0.000d0
            t0= SECOND( tcum(1))
C                       assure that 10 calls to SECOND are NOT optimized
      DO 810 k= 1,klm
      DO 805 i= 1,10
       tcum(i)= t12(i)
  805 continue
            t1= SECOND( tcum(1))
            t2= SECOND( tcum(2))
            t3= SECOND( tcum(3))
            t4= SECOND( tcum(4))
            t5= SECOND( tcum(5))
            t6= SECOND( tcum(6))
            t7= SECOND( tcum(7))
            t8= SECOND( tcum(8))
            t9= SECOND( tcum(9))
           t10= SECOND( tcum(10))
  810 continue
        elapst= t10 - t0
         tseco= elapst/( REAL(10*klm) + 1.0e-9)
         toler= 0.020d0
          rerr= 1.000d0
C
C                                  Convergence test:  Rel.error .LT. 1%
            IF( elapst.GT. 1.00d04 ) GO TO 911
            IF( elapst.LT. 1.00d-10 .AND. j.GT.10 ) GO TO 911
            IF( elapst.GT. 1.00d-9 ) THEN
                     jj= jj + 1
                TIM(jj)= tseco
                     IF( jj.GT.1 ) THEN
                         rerr= RELERR( TIM(jj), TIM(jj-1))
                     ENDIF
                TER(jj)= rerr
            ENDIF
C
            IF( iou.GT.0 ) THEN
         WRITE( iou,64) 10*klm,  tseco, rerr
            ENDIF
            IF( rerr  .LT. toler   ) GO TO 825
            IF( elapst.GT. 10.00d0 ) GO TO 822
           klm= klm + klm
  820 continue
C                                  Poor accuracy on exit from loop
  822     IF( j .LE. 1 )  GO TO 911
          IF( jj.LT. 1 )  GO TO 911
         CALL SORDID( INX,TMX,  TER,jj,1)
C
           i= 0
  823      i= i + 1
       tseco= TIM( INX(i))
        rerr= TMX(i)
          IF( tseco.LE. 0.00d0 .AND. i.LT.jj ) GO TO 823
C
          IF(  rerr.GT. 0.050d0 ) THEN
               WRITE( io,63)  100.00d0 * rerr
          ENDIF
C                                  Good convergence, satifies 1% error tolerence
  825 SECOVT = tseco
C
      CALL TRACK ('SECOVT  ')
      RETURN
C
  911         WRITE( io,61)
              WRITE( io,62) elapst, j
              CALL WHERE(0)
C
   61 FORMAT(1X,'FATAL(SECOVT): cant measure overhead time subr SECOND')
   62 FORMAT(/,13X,'using SECOND:  elapst=',1E20.8,6X,'J=',I4)
   63 FORMAT(1X,'WARNING(SECOVT): SECOND overhead time relerr',f9.4,'%')
   64 FORMAT('SECOVT:',I10,E12.4,F11.4)
      END
