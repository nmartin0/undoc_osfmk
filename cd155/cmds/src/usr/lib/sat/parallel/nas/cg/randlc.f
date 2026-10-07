      double precision FUNCTION   RANDLC   (X, A)

c     ==================================================================
c     ==================================================================
c     ==== randlc -- nas benchmark random number generator          ====
c     ==================================================================
c     ==================================================================

C     This routine returns a uniform pseudorandom double precision 
C     number in the range (0, 1) by using the linear congruential
c     generator

C     x_{k+1} = a x_k  (mod 2^46)

C     where 0 < x_k < 2^46 and 0 < a < 2^46.  This scheme generates
c     2^44 numbers  before repeating.  The argument A is the same as
c     'a' in the above formula, and X is the same as x_0.  A and X must
c     be odd double precision integers in the range (1, 2^46).  The
c     returned value RANDLC is normalized to be between 0 and 1, i.e.
c     RANDLC = 2^(-46) * x_1.  X is updated to contain the new seed
c     x_1, so that subsequent calls to RANDLC using the same
C     arguments will generate a continuous sequence.

C     This routine should produce the same results on any computer with
c     at least 48 mantissa bits in double precision floating point data.
c     On 64 bit systems, double precision should be disabled.

C     David H. Bailey     October 26, 1990

c     ==================================================================

      double precision  a, x

      integer           i, ks

      double precision  a1, a2, r23, r46, t1, t2, t23, t3, t4, t46,
     1                  x1, x2, z

      SAVE              KS, R23, R46, T23, T46
      DATA              KS /0/

c     ==================================================================

C     If this is the first call to RANDLC, compute R23 = 2 ^ -23,
c     R46 = 2 ^ -46, T23 = 2 ^ 23, and T46 = 2 ^ 46.  These are
c     computed in loops, rather than by merely using the ** operator,
c     in order to insure that the results are exact on all systems.
c     This code assumes that 0.5D0 is represented exactly.

      IF  ( KS .EQ. 0 )  THEN

         R23 = 1.D0
         R46 = 1.D0
         T23 = 1.D0
         T46 = 1.D0

         DO 100 I = 1, 23
            R23 = 0.5D0 * R23
            T23 = 2.D0 * T23
 100     CONTINUE

         DO 110 I = 1, 46
            R46 = 0.5D0 * R46
            T46 = 2.D0 * T46
 110     CONTINUE

        KS = 1

      ENDIF

C     Break A into two parts such that A = 2^23 * A1 + A2.

      T1 = R23 * A
      A1 = AINT (T1)
      A2 = A - T23 * A1

C     Break X into two parts such that X = 2^23 * X1 + X2, compute
C     Z = A1 * X2 + A2 * X1  (mod 2^23), and then
C     X = 2^23 * Z + A2 * X2  (mod 2^46).

      T1 = R23 * X
      X1 = AINT (T1)
      X2 = X - T23 * X1
      T1 = A1 * X2 + A2 * X1
      T2 = AINT (R23 * T1)
      Z = T1 - T23 * T2
      T3 = T23 * Z + A2 * X2
      T4 = AINT (R46 * T3)
      X = T3 - T46 * T4
      RANDLC = R46 * X

      RETURN

      END
