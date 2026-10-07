C
C***********************************************
      SUBROUTINE TRAP( I, name, mini, maxi,meff)
C***********************************************
C
C      Checks that Index List values are in valid domain
C
C     I     - ARRAY  OF INPUT INDEX-LIST
C     name  -           INPUT CALLERS name
C     mini  - INPUT SMALLEST INDEX VALUE
C     maxi  - INPUT LARGEST  INDEX VALUE
C     meff  - NUMBER OF INPUT VALUES IN I.
C
C***********************************************
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,O-Z)
C
      COMMON /SPACES/ ion,j5,k2,k3,MULTI,laps,Loop,m,kr,LP,n13h,ibuf,nx,
     1 L,npass,nfail,n,n1,n2,n13,n213,n813,n14,n16,n416,n21,nt1,nt2,
     2 last,idebug,mpy,Loops2,mucho,mpylim, intbuf(16)
      DIMENSION  I(meff)
C
      CALL TRACE ('TRAP    ')
C
           LX= 0
      DO 1 k= 1,meff
          IF( I(k).LT.mini .OR. I(k).GT.maxi )  LX= k
    1 CONTINUE
C
          IF( LX.NE.0 )   THEN
              io= ABS( ion)
              IF( io.LE.0 .OR. io.GT.10 ) io=6
              WRITE( io,110)  LX, name
  110   FORMAT(////,' TRAP: ERROR IN INDEX-LIST(',i4,')  IN SUBR:  ',A )
              WRITE( io,113) I
  113         FORMAT(1X,10I6)
C
              CALL WHERE(0)
          ENDIF
C
      CALL TRACK ('TRAP    ')
      RETURN
      END
