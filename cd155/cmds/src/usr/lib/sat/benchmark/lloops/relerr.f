C
C********************************************
      FUNCTION  RELERR( U,V)
C********************************************
C
C       RELERR - RELATIVE ERROR BETWEEN  U,V  (0.,1.)
C            U - INPUT
C            V - INPUT
C********************************************
C
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,O-Z)
Cout  DOUBLE  PRECISION  x, y                                           REDUNDNT
C
      CALL TRACE ('RELERR  ')
                 w= 0.00d0
      IF( u .NE. v ) THEN
                 w= 1.00d0
                 o= 1.00d0
          IF( SIGN( o, u) .EQ. SIGN( o, v)) THEN
              a= ABS( u)
              b= ABS( v)
              x= MAX( a, b)
              y= MIN( a, b)
             IF( x .NE.  0.00d0) THEN
                 w= 1.00d0 - y/x
             ENDIF
          ENDIF
      ENDIF
C
      RELERR= w
      CALL TRACK ('RELERR  ')
      RETURN
      END
