C
C***********************************************
      SUBROUTINE VALID( VX,MAP,LX,  BL,X,BU,n )
C***********************************************
C
C      Compress valid data sets;  form compression list.
C
C
C     VX    - ARRAY  OF RESULT COMPRESSED Xs.
C     MAP   - ARRAY  OF RESULT COMPRESSION INDICES
C     LX     -           RESULT COMPRESSED LENGTH OF VX, MAP
C           -
C     BL    -           INPUT LOWER BOUND FOR VX
C     X     - ARRAY  OF INPUT VALUES.
C     BU    -           INPUT UPPER BOUND FOR VX
C     n     - NUMBER OF INPUT VALUES IN X.
C
C***********************************************
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,O-Z)
C
      DIMENSION  VX(n), MAP(n), X(n)
CLLL. OPTIMIZE LEVEL G
C
      CALL TRACE ('VALID   ')
C
           m= 0
           LX= 0
          IF( n.LE.0 )  GO TO 73
      DO 1 k= 1,n
              IF( X(k).LE. BL .OR. X(k).GE. BU )  GO TO 1
                     m= m + 1
                MAP(m)= k
                 VX(m)= X(k)
    1 CONTINUE
C
      LX= m
      IF( m.GT.0 )  THEN
          CALL TRAP( MAP, 8H VALID   , 1, n,m)
      ENDIF
   73 CONTINUE
      CALL TRACK ('VALID   ')
      RETURN
      END
