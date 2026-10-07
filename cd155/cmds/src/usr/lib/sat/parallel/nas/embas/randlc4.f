
C
      FUNCTION RANDLC (X, A)
C
C   This routine returns a uniform pseudorandom double precision number in the
C   range (0, 1) by using the linear congruential generator
C
C   x_{k+1} = a x_k  (mod 2^46)
C
C   where 0 < x_k < 2^46 and 0 < a < 2^46.  This scheme generates 2^44 numbers
C   before repeating.  The argument A is the same as 'a' in the above formula,
C   and X is the same as x_0.  A and X must be odd double precision integers
C   in the range (1, 2^46).  The returned value RANDLC is normalized to be
C   between 0 and 1, i.e. RANDLC = 2^(-46) * x_1.  X is updated to contain
C   the new seed x_1, so that subsequent calls to RANDLC using the same
C   arguments will generate a continuous sequence.
C
C   This routine should produce the same results on any computer with at least
C   48 mantissa bits in double precision floating point data.  On 64 bit
C   systems, double precision should be disabled.
C
C   David H. Bailey     October 26, 1990
C
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      SAVE KS, R23, R46, T23, T46
      DATA KS/0/
C
C   If this is the first call to RANDLC, compute R23 = 2 ^ -23, R46 = 2 ^ -46,
C   T23 = 2 ^ 23, and T46 = 2 ^ 46.  These are computed in loops, rather than
C   by merely using the ** operator, in order to insure that the results are
C   exact on all systems.  This code assumes that 0.5D0 is represented exactly.
C
      IF (KS .EQ. 0) THEN
        R23 = 1.D0
        R46 = 1.D0
        T23 = 1.D0
        T46 = 1.D0
C
        DO 100 I = 1, 23
          R23 = 0.5D0 * R23
          T23 = 2.D0 * T23
 100    CONTINUE
C
        DO 110 I = 1, 46
          R46 = 0.5D0 * R46
          T46 = 2.D0 * T46
 110    CONTINUE
C
        KS = 1
      ENDIF
C
C   Break A into two parts such that A = 2^23 * A1 + A2.
C
      T1 = R23 * A
      A1 = int (T1)
      A2 = A - T23 * A1
C
C   Break X into two parts such that X = 2^23 * X1 + X2, compute
C   Z = A1 * X2 + A2 * X1  (mod 2^23), and then
C   X = 2^23 * Z + A2 * X2  (mod 2^46).
C
      T1 = R23 * X
      X1 = int (T1)
      X2 = X - T23 * X1
      T1 = A1 * X2 + A2 * X1
      T2 = int (R23 * T1)
      Z = T1 - T23 * T2
      T3 = T23 * Z + A2 * X2
      T4 = int (R46 * T3)
      X = T3 - T46 * T4
      RANDLC = R46 * X
C
      RETURN
      END
