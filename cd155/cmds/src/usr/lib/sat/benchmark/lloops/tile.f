C
C***********************************************
      SUBROUTINE TILE( sm, si, OX,IX,W,ew,T,tiles,n)
C***********************************************
C
C     TILE       computes  m-tile value and corresponding index
C
C     sm      -  RESULT VALUE  IS m-TILE VALUE
C     si      -  RESULT VALUE  IS CORRESPONDING INDEX.r IN W
C
C     OX      -  INPUT  ARRAY  OF ORDERED (DECENDING) Xs.
C     IX      -  INPUT  ARRAY  OF INDEX LIST MAPS X TO OX.
C     W       -  INPUT  ARRAY  OF INPUT  WEIGHTS.
C     ew      -  INPUT  VALUE  FLAGS EQUAL WEIGHTS= 1.0; ELSE 0.0d0
C     T       -  INPUT  VALUE  IS SUM OF WEIGHTS
C     tiles   -  INPUT  VALUE  IS FRACTION OF RANGE, E.G. 0.25
C     n       -  INPUT  NUMBER OF INPUT  VALUES IN X.
C
C***********************************************
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,O-Z)
C
      DIMENSION  OX(n), IX(n), W(n)
C
      CALL TRACE ('TILE    ')
C
       thresh= tiles*T + 0.50d0*ew*W(1)
            R= 0.0d0
            S= R
      DO 70 k= 1,n
            S= R
            R= R + W( IX(k))
           IF( R .GT. thresh )  GO TO 7
   70 CONTINUE
            k= n
    7       z= 0.0d0
            y= 0.0d0
           IF( k.GT.1 )    y =   OX(k-1)
           IF( R.NE.S )    z = ( thresh - S)/( R - S)
           sm= y         + z * ( OX(k)  - y)
           si= REAL(k-1) + z
C
      CALL TRACK ('TILE    ')
      RETURN
      END
