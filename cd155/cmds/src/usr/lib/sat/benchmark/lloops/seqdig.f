C
C***************************************
      SUBROUTINE SEQDIG( ND, isum, A, B, nr)
C***********************************************************************
C                                                                      *
c     SEQDIG - compute relative error and significant digits of precisi*
C                                                                      *
C     ND     - Out.Ary  Number of Significant Equal Digits in A.eq.B   *
C     isum   - Result   Total number of Equal digits.                  *
C     A      -  In.Ary  Reference values for comparison.               *
C     B      -  In.Ary  New values of unknown precision.               *
C     nr     -  Input   number of results in ND                        *
C***********************************************************************
C
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,O-Z)
C
      parameter( maxsd= 16 )
c/    parameter( maxsd= 30 )
      DIMENSION  ND(nr), A(nr), B(nr)
C
      ISIGDG(reler)= INT( ABS( LOG10( ABS( reler))) + 0.500d0 )
      CALL TRACE ('SEQDIG  ')
c
c     Try to determine floating-point precision used: Max Sig Digits
c
        maxs= maxsd
         one= 1.00d0
          sd= 1073741824.00d0
         sum= sd + one
          IF( sum .EQ. sd )  maxs= 8
c
        isum= 0
      DO 1 k= 1,nr
          se= SIGN( one, A(k)) * SIGN( one, B(k))
          IF( se .LT. 0.0)  THEN
              ND(k)= 0
          ELSE
c
c             compute relative error and digits of precision in B.
c
                  re=  RELERR( A(k), B(k))
              IF((re.GT. 0.0d0 ) .AND. (re.LT. 1.0d0))  THEN
                       ND(k)= ISIGDG(re)
              ELSEIF( re .EQ. 0.0d0 )  THEN
                       ND(k)= maxs
              ELSEIF( re .GE. 1.0d0 )  THEN
                       ND(k)= 0
              ENDIF
              IF( ND(k).GT. maxs  )  ND(k)= maxs
          ENDIF
        isum= isum + ND(k)
    1 CONTINUE
C
      CALL TRACK ('SEQDIG  ')
      RETURN
      END
