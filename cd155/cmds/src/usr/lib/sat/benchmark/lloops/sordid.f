C***********************************************
      SUBROUTINE SORDID( I,W, V,n,KIND)
C***********************************************
C                    QUICK AND DIRTY PORTABLE SORT.
C
C                I - RESULT INDEX-LIST. MAPS V TO SORTED W.
C                W - RESULT ARRAY, SORTED V.
C
C                V - INPUT  ARRAY SORTED IN PLACE.
C                n - INPUT  NUMBER OF ELEMENTS IN V
C             KIND - SORT ORDER:   = 1  ASCENDING MAGNITUDE
C                                  = 2 DESCENDING MAGNITUDE
C
C***********************************************
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,O-Z)
C
      DIMENSION  I(n), W(n), V(n)
C
      CALL TRACE ('SORDID  ')
C
            IF( n.LE.0 )  GO TO 73
      DO  1  k= 1,n
          W(k)= V(k)
    1     I(k)= k
C
      IF( KIND.EQ.1)  THEN
C
          DO  3  j= 1,n-1
                 m= j
          DO  2  k= j+1,n
                IF( W(k).LT.W(m)) m= k
    2     CONTINUE
                 X= W(j)
                 k= I(j)
              W(j)= W(m)
              I(j)= I(m)
              W(m)= X
              I(m)= k
    3     CONTINUE
C
C
      ELSE
C
          DO  6  j= 1,n-1
                 m= j
          DO  5  k= j+1,n
                IF( W(k).GT.W(m)) m= k
    5     CONTINUE
                 X= W(j)
                 k= I(j)
              W(j)= W(m)
              I(j)= I(m)
              W(m)= X
              I(m)= k
    6     CONTINUE
      ENDIF
C
      IF( n.GT.0 )  THEN
          CALL TRAP( I, 8H SORDID  , 1, n,n)
      ENDIF
C
   73 CONTINUE
      CALL TRACK ('SORDID  ')
      RETURN
      END
