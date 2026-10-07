C***********************************************
      SUBROUTINE STATS( STAT, X,n)
C***********************************************
C
C     UNWEIGHTED STATISTICS: MEAN, STADEV, MIN, MAX, HARMONIC MEAN.
C
C     STAT(1)= THE MEAN OF X.
C     STAT(2)= THE STANDARD DEVIATION OF THE MEAN OF X.
C     STAT(3)= THE MINIMUM OF X.
C     STAT(4)= THE MAXIMUM OF X.
C     STAT(5)= THE HARMONIC MEAN
C     X       IS THE ARRAY  OF INPUT VALUES.
C     n       IS THE NUMBER OF INPUT VALUES IN X.
C
C***********************************************
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,O-Z)
C
      DIMENSION X(n), STAT(20)
cLLL. OPTIMIZE LEVEL G
C
      CALL TRACE ('STATS   ')
C
      DO 10   k= 1,9
   10 STAT(k)= 0.0
C
      IF(n.LE.0)  GO TO 73
C                             CALCULATE MEAN OF X.
      S= 0.0
      DO 1 k= 1,n
    1 S= S + X(k)
      A= S/n
      STAT(1)= A
C                             CALCULATE STANDARD DEVIATION OF X.
      D= 0.0
      DO 2 k= 1,n
    2 D= D + (X(k)-A)**2
      D= D/n
      STAT(2)= SQRT(D)
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
      H= 0.0
      DO 5 k= 1,n
          IF( X(k).NE.0.0) H= H + 1.0/X(k)
    5 CONTINUE
          IF( H.NE.0.0) H= REAL(n)/H
      STAT(5)= H
C
   73 CONTINUE
      CALL TRACK ('STATS   ')
      RETURN
      END
