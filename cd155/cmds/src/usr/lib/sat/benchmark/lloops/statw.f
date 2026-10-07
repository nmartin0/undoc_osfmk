C***********************************************
      SUBROUTINE STATW( STAT,OX,IX, X,W,n)
C***********************************************
C
C     WEIGHTED STATISTICS: MEAN, STADEV, MIN, MAX, HARMONIC MEAN, MEDIAN.
C
C     STAT( 1)=  THE MEAN OF X.
C     STAT( 2)=  THE STANDARD DEVIATION OF THE MEAN OF X.
C     STAT( 3)=  THE MINIMUM OF X.
C     STAT( 4)=  THE MAXIMUM OF X.
C     STAT( 5)=  THE HARMONIC MEAN
C     STAT( 6)=  THE TOTAL WEIGHT.
C     STAT( 7)=  THE MEDIAN.
C     STAT( 8)=  THE MEDIAN INDEX, ASCENDING.
C     STAT( 9)=  THE ROBUST MEDIAN ABSOLUTE DEVIATION.
C     STAT(10)=  THE GEOMETRIC MEAN
C     STAT(11)=  THE MOMENTAL SKEWNESS
C     STAT(12)=  THE KURTOSIS
C     STAT(13)=  THE LOWER QUARTILE BOUND Q1/Q2 VALUE
C     STAT(14)=  THE UPPER QUARTILE BOUND Q3/Q4 VALUE
C     STAT(15)=  THE DEVIATION OF THE GEOMETRIC MEAN OF X
C
C     OX      IS THE ARRAY  OF ORDERED (DECENDING) Xs.
C     IX      IS THE ARRAY  OF INDEX LIST MAPS X TO OX.
C
C     X       IS THE ARRAY  OF INPUT VALUES.
C     W       IS THE ARRAY  OF INPUT WEIGHTS.
C     n       IS THE NUMBER OF INPUT VALUES IN X.
C
C***********************************************
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,O-Z)
C
      DIMENSION STAT(20), OX(n), IX(n), X(n), W(n)
cLLL. OPTIMIZE LEVEL G
C
      CALL TRACE ('STATW   ')
         stin09= 0.00d0
         stin13= 0.00d0
         stin14= 0.00d0
C
      DO 50   k= 1,15
   50 STAT(k)= 0.0d0
C
      IF( n.LE.0 )  GO TO 73
C
      IF( n.EQ.1 )  THEN
          STAT( 1)= X(1)
          STAT( 3)= X(1)
          STAT( 4)= X(1)
          STAT( 5)= X(1)
          STAT( 6)= W(1)
          STAT( 7)= X(1)
          STAT( 8)= 1.0d0
          STAT(10)= X(1)
          GO TO 73
      ENDIF
C
C
C                             CALCULATE MEAN OF X.
      A= 0.0d0
      S= 0.0d0
      T= 0.0d0
C
      DO 1 k= 1,n
      S= S + W(k)*X(k)
    1 T= T + W(k)
          IF( T.NE.0.0d0) A= S/T
      STAT(1)= A
C                             CALCULATE STANDARD DEVIATION OF X.
      D= 0.0d0
      E= 0.0d0
      F= 0.0d0
      Q= 0.0d0
      U= 0.0d0
C
      DO 2 k= 1,n
      B= W(k) *( X(k) -A)**2
      D= D + B
      E= E + B*( X(k) -A)
    2 F= F + B*( X(k) -A)**2
          IF( T.NE.0.0d0) Q= 1.0d0/T
                          D= D*Q
                          E= E*Q
                          F= F*Q
          IF( D.GE.0.0d0) U= SQRT(D)
      STAT(2)= U
C                             CALCULATE MINIMUM OF X.
      U= X(1)
      DO 3 k= 2,n
    3 U= MIN(U,X(k))
      STAT(3)= U
C                             CALCULATE MAXIMUM OF X.
      V= X(1)
      DO 4 k= 2,n
    4 V= MAX(V,X(k))
      STAT(4)= V
C                             CALCULATE HARMONIC MEAN OF X.
      H= 0.0d0
      DO 5 k= 1,n
          IF( X(k).NE.0.0d0) H= H + W(k)/X(k)
    5 CONTINUE
          IF( H.NE.0.0d0) H= T/H
      STAT(5)= H
      STAT(6)= T
C                             CALCULATE WEIGHTED MEDIAN
      CALL SORDID( IX, OX, X, n, 1)
C
           ew= 0.0d0
      DO 7  k= 2,n
           IF( W(1) .NE. W(k))  GO TO 75
    7 continue
           ew= 1.0d0
   75 continue
C
        qt= 0.500d0
      CALL  TILE( STAT( 7), STAT(8), OX,IX,W,ew,T, qt,n)
C
        qt= 0.250d0
      CALL  TILE( STAT(13),  stin13, OX,IX,W,ew,T, qt,n)
C
        qt= 0.750d0
      CALL  TILE( STAT(14),  stin14, OX,IX,W,ew,T, qt,n)
C
C
C                           CALCULATE ROBUST MEDIAN ABSOLUTE DEVIATION (MAD)
      DO 90 k= 1,n
   90   OX(k)= ABS( X(k) - STAT(7))
C
      CALL SORDID( IX, OX, OX, n, 1)
C
        qt= 0.700d0
      CALL  TILE( STAT( 9),  stin09, OX,IX,W,ew,T, qt,n)
C
C                             CALCULATE GEOMETRIC MEAN
            R= 0.0d0
      DO 10 k= 1,n
           IF( X(k).LE. 0.0d0)  GO TO 10
            R= R + W(k) *LOG10( X(k))
   10 CONTINUE
             U= R*Q
             G= 10.0d0
            IF( U.LT. 0.0d0)  G= 0.1D0
        POWTEN= 50.0d0
            IF( ABS(U) .GT. POWTEN)  U= SIGN( POWTEN, U)
      STAT(10)=  G** ABS(U)
C
C                             CALCULATE MOMENTAL SKEWNESS
             G= 0.0d0
           DXD= D*D
            IF( DXD.NE.0.0d0) G= 1.0d0/(DXD)
      STAT(11)= 0.50d0*E*G*STAT(2)
C
C                             CALCULATE KURTOSIS
      STAT(12)= 0.50d0*( F*G -3.0d0)
C
C                             CALCULATE DEVIATION OF GEOMETRIC MEAN
      D= 0.0d0
      Q= 0.0d0
      U= 0.0d0
      GM= STAT(10)
C
      DO 15 k= 1,n
      B= W(k) *( X(k) -GM)**2
   15 D= D + B
          IF( T.NE.0.0d0) Q= 1.0d0/T
                          D= D*Q
          IF( D.GE.0.0d0) U= SQRT(D)
      STAT(15)= U
C
C                             CALCULATE DESCENDING ORDERED X.
      CALL SORDID( IX, OX, X, n, 2)
C
   73 CONTINUE
      CALL TRACK ('STATW   ')
      RETURN
      END
