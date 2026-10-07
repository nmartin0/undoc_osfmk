C
C***********************************************
      SUBROUTINE TDIGIT( derr, nzd, s )
C***********************************************************************
C                                                                      *
C     TDIGIT  -  Count Lead Digits Followed By Trailing Zeroes.        *
C                                                                      *
C       derr  -  Result,  Digital Error in percent.                    *
C        nzd  -  Result,  Number Of Lead Digits                        *
C          s  -  Input ,  A Floated Integer                            *
C                                                                      *
C***********************************************************************
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,O-Z)
Cout  DOUBLE  PRECISION  frac, fuzz, x, y, v, z                         REDUNDNT
C
c     frac(z)= (SIGN((ABS(z) - AINT(ABS(z))),z))
      frac(z)= ( ABS( ABS(z) - AINT(ABS(z))))
C
      CALL TRACE ('TDIGIT  ')
C
            x= 0.00d0
            n= 14
            x= ABS(s)
         fuzz= 1.0d-6
         derr= 100.0d0
          nzd= 0
           IF( x.EQ. 0.0d0)  GO TO 73
C                                  Normalize x
            y= LOG10(x)
            v= REAL( 10**( ABS( INT(y)) + 1 ))
C
           IF( (y.GE. 0.0d0) .AND. (v.NE. 0.0d0))  THEN
            x= (x/v) * 10.0d0
           ELSE
            x= x*v
           ENDIF
C                                  Multiply x Until Trailing Digits= Fuzz
       DO 1 k= 1,n
           IF( ((1.0d0-frac(x)).LE.fuzz) .OR. (frac(x).LE.fuzz)) GO TO 2
            x= 10.0d0*x
    1 continue
C
    2      IF( x.NE. 0.0d0)  THEN
               derr= 50.0d0/x
                nzd= INT( LOG10( ABS( 9.999999990d0*x )))
           ENDIF
C
   73 CONTINUE
      CALL TRACK ('TDIGIT  ')
      RETURN
      END
