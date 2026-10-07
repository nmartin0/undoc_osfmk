      PROGRAM NASKER
C
C   NAS KERNEL BENCHMARK PROGRAM
C   12/17/84      DAVID H BAILEY
C
C
      CHARACTER*8 PN(8)
      REAL ER(8), FP(8), TM(8), RT(8)
      COMMON /ARRAYS/ DATA(360000)
      DATA PN/'MXM', 'CFFT2D', 'CHOLSKY', 'BTRIX', 'GMTRY', 'EMIT',
     $ 'VPENTA', 'TOTAL'/
C
      WRITE (6, 1)
1     FORMAT (/16X, 'THE NAS KERNEL BENCHMARK PROGRAM'//)
C
      CALL MXMTST (ER(1), FP(1), TM(1))
      CALL FFTTST (ER(2), FP(2), TM(2))
      CALL CHOTST (ER(3), FP(3), TM(3))
      CALL BTRTST (ER(4), FP(4), TM(4))
      CALL GMTTST (ER(5), FP(5), TM(5))
      CALL EMITST (ER(6), FP(6), TM(6))
      CALL VPETST (ER(7), FP(7), TM(7))
C
      TE = 0.
      TF = 0.
      TT = 0.
      DO 100 I = 1, 7
        TE = TE + ER(I)
        TF = TF + FP(I)
        TT = TT + TM(I)
        RT(I) = 1E-6 * FP(I) / TM(I)
100   CONTINUE
      ER(8) = TE
      FP(8) = TF
      TM(8) = TT
      RT(8) = 1E-6 * TF / TT
C
      WRITE (6, 2)  (PN(I), ER(I), FP(I), TM(I), RT(I), I = 1, 8)
2     FORMAT (' PROGRAM', 8X, 'ERROR', 10X, 'FP OPS', 7X, 'SECONDS',
     $  6X, 'MFLOPS'// 7(1X, A8, 1P,2E15.4, 0P,F12.4, F12.2/)/
     $  1X, A8, 1P,2E15.4, 0P,F12.4, F12.2//)
C
      STOP
      END
C
      FUNCTION CPTIME ()
C
C   RETURNS THE CPU TIME SINCE THE LAST CALL TO CPTIME.
C   THIS SUBPROGRAM MAY BE CHANGED AS NEEDED FOR A PARTICULAR COMPUTER
C   SYSTEM WITHOUT PENALTY, PROVIDED IT PERFORMS THIS FUNCTION.
C
      save tx
      DATA TX/0./
      T = dclock ()
      CPTIME = T - TX
      TX = T
      RETURN
      END
C
      SUBROUTINE COPY (N, A, B)
C
C   ARRAY COPY ROUTINE
C
      REAL A(N), B(N)
      DO 100 I = 1, N
        B(I) = A(I)
100   CONTINUE
      RETURN
      END
C
      SUBROUTINE MXMTST (ER, FP, TM)
C
C   FLOATING-POINT MATRIX MULTIPLY TEST
C
      PARAMETER (L=256, M=128, N=64, F7=78125., T30=1073741824.)
      COMMON /ARRAYS/ A(L,M), S1, B(M,N), S2, C(L,N)
      DATA IT/100/, ANS/35.2026179738722/
C
C   INITIALIZATION
C
C   THE ARRAYS A AND B ARE FILLED WITH PSEUDO-RANDOM  (0., 1.)  DATA
C   USING A RANDOM NUMBER GENERATOR BASED ON THE RECURSION
C           X(N+1) = 5**7 * X(N)  (MOD 2**30)
C   THIS RECURSION WILL GENERATE 2**28 (APPROX. 268 MILLION) NUMBERS
C   BEFORE REPEATING.  FOR THIS SCHEME TO WORK PROPERLY, THE HARDWARE
C   MULTIPLY OPERATION MUST BE CORRECT TO 47 BITS OF PRECISION.
C   THIS SAME SCHEME IS USED TO INITIALIZE DATA ARRAYS FOR ALL TESTS.
C
      T = F7 / T30
      DO 100 J = 1, M
        DO 100 I = 1, L
          T = MOD (F7 * T, 1.)
          A(I,J) = T
100   CONTINUE
      DO 110 J = 1, N
        DO 110 I = 1, M
          T = MOD (F7 * T, 1.)
          B(I,J) = T
110   CONTINUE
      TM = CPTIME ()
C
C   TIMING TEST
C
      DO 120 II = 1, IT
        CALL MXM (A, B, C, L, M, N)
120   CONTINUE
C
      TM = CPTIME ()
      ER = ABS ((C(19,19) - ANS) / ANS)
      FP = 2. * IT * L * M * N
C
      RETURN
      END
C
      SUBROUTINE MXM (A, B, C, L, M, N)
      DIMENSION A(L,M), B(M,N), C(L,N)
C
C   4-WAY UNROLLED MATRIX MULTIPLY ROUTINE FOR VECTOR COMPUTERS.
C   M MUST BE A MULTIPLE OF 4.  CONTIGUOUS DATA ASSUMED.
C   D H BAILEY  11/15/84
C
      DO 100 K = 1, N
        DO 100 I = 1, L
          C(I,K) = 0.
100     CONTINUE
      DO 110 J = 1, M, 4
        DO 110 K = 1, N
          DO 110 I = 1, L
            C(I,K) = C(I,K) + A(I,J) * B(J,K)
     $             + A(I,J+1) * B(J+1,K) + A(I,J+2) * B(J+2,K)
     $             + A(I,J+3) * B(J+3,K)
110   CONTINUE
C
      RETURN
      END
C
      SUBROUTINE FFTTST (ER, FP, TM)
C
C   2-D FFT TEST PROGRAM
C
      PARAMETER (M=128, N=256, M1=128, F7=78125., T30=1073741824.)
      COMPLEX X, Y, CT
      COMMON /ARRAYS/ X(M1,N), W1(M), W2(N), IP(2*N)
      DATA IT/100/, ANS/0.894799941219277/
C
C   INITIALIZE
C
      AMN = M * N
      RMN = 1. / AMN
      T2 = F7 / T30
      DO 100 J = 1, N
        DO 100 I = 1, M
          T1 = MOD (F7 * T2, 1.)
          T2 = MOD (F7 * T1, 1.)
          X(I,J) = CMPLX (T1, T2)
100   CONTINUE
      CALL CFT2D1 (0, M, M1, N, X, W1, IP)
      CALL CFT2D2 (0, M, M1, N, X, W2, IP)
      TM = CPTIME ()
C
C   TEST ITERATIONS
C
      DO 120 K = 1, IT
        DO 110 J = 1, N
          DO 110 I = 1, M
            X(I,J) = RMN * X(I,J)
110     CONTINUE
C
        CALL CFT2D1 (1, M, M1, N, X, W1, IP)
        CALL CFT2D2 (1, M, M1, N, X, W2, IP)
        CALL CFT2D2 (-1, M, M1, N, X, W2, IP)
        CALL CFT2D1 (-1, M, M1, N, X, W1, IP)
120   CONTINUE
C
      TM = CPTIME ()
      ER = ABS ((REAL(X(19,19)) - ANS) / ANS)
      FP = IT * AMN * (2.  +  10. * LOG (AMN)/LOG (2.))
C
      RETURN
      END
C
      SUBROUTINE CFT2D1 (IS, M, M1, N, X, W, IP)
C
C   PERFORMS COMPLEX RADIX 2 FFTS ON THE FIRST DIMENSION OF THE 2-D ARRAY X
C   D H BAILEY  11/15/84
C
      COMPLEX X(M1,N), W(M), CT, CX
      INTEGER IP(2,M)
      DATA PI/3.141592653589793/
C
C   IF  IS = 0  THEN INITIALIZE ONLY
C
      M2 = M / 2
      IF (IS .EQ. 0)  THEN
        DO 100 I = 1, M2
          T = 2. * PI * (I-1) / M
          W(I) = CMPLX (COS (T), SIN (T))
100     CONTINUE
        RETURN
      ENDIF
C
C   PERFORM FORWARD OR BACKWARD FFTS ACCORDING TO  IS = 1 OR -1
C
      DO 110 I = 1, M
        IP(1,I) = I
110   CONTINUE
      L = 1
      I1 = 1
C
120   I2 = 3 - I1
      DO 130 J = L, M2, L
        CX = W(J-L+1)
        IF (IS .LT. 0)  CX = CONJG (CX)
        DO 130 I = J-L+1, J
          II = IP(I1,I)
          IP(I2,I+J-L) = II
          IM = IP(I1,I+M2)
          IP(I2,I+J) = IM
          DO 130 K = 1, N
            CT = X(II,K) - X(IM,K)
            X(II,K) = X(II,K) + X(IM,K)
            X(IM,K) = CT * CX
130   CONTINUE
      L = 2 * L
      I1 = I2
      IF (L .LE. M2)  GOTO 120
C
      DO 150 I = 1, M
        II = IP(I1,I)
        IF (II .GT. I)  THEN
          DO 140 K = 1, N
            CT = X(I,K)
            X(I,K) = X(II,K)
            X(II,K) = CT
140       CONTINUE
        ENDIF
150   CONTINUE
C
      RETURN
      END
C
      SUBROUTINE CFT2D2 (IS, M, M1, N, X, W, IP)
C
C   PERFORMS COMPLEX RADIX 2 FFTS ON THE SECOND DIMENSION OF THE 2-D ARRAY X
C   D H BAILEY  11/15/84
C
      COMPLEX X(M1,N), W(N), CT, CX
      INTEGER IP(2,N)
      DATA PI/3.141592653589793/
C
C   IF  IS = 0  THEN INITIALIZE ONLY
C
      N2 = N / 2
      IF (IS .EQ. 0)  THEN
        DO 100 I = 1, N2
          T = 2. * PI * (I-1) / N
          W(I) = CMPLX (COS (T), SIN (T))
100     CONTINUE
        RETURN
      ENDIF
C
C   PEFORM FORWARD OR BACKWARD FFTS ACCORDING TO  IS = 1 OR -1
C
      DO 110 I = 1, N
        IP(1,I) = I
110   CONTINUE
      L = 1
      I1 = 1
C
120   I2 = 3 - I1
      DO 130 J = L, N2, L
        CX = W(J-L+1)
        IF (IS .LT. 0)  CX = CONJG (CX)
        DO 130 I = J-L+1, J
          II = IP(I1,I)
          IP(I2,I+J-L) = II
          IM = IP(I1,I+N2)
          IP(I2,I+J) = IM
          DO 130 K = 1, M
            CT = X(K,II) - X(K,IM)
            X(K,II) = X(K,II) + X(K,IM)
            X(K,IM) = CT * CX
130   CONTINUE
      L = 2 * L
      I1 = I2
      IF (L .LE. N2)  GOTO 120
C
      DO 150 I = 1, N
        II = IP(I1,I)
        IF (II .GT. I)  THEN
          DO 140 K = 1, M
            CT = X(K,I)
            X(K,I) = X(K,II)
            X(K,II) = CT
140       CONTINUE
        ENDIF
150   CONTINUE
C
      RETURN
      END
C
      SUBROUTINE CHOTST (ER, FP, TM)
C
C   CHOLSKY TEST PROGRAM
C
      PARAMETER (IDA=250, NMAT=250, M=4, N=40, NRHS=3, F7=78125.,
     $ T30=1073741824.)
      COMMON /ARRAYS/ A(0:IDA, -M:0, 0:N), B(0:NRHS, 0:NMAT, 0:N),
     $   AX(0:IDA, -M:0, 0:N), BX(0:NRHS, 0:NMAT, 0:N)
      DATA  IT/200/, ANS/5177.88531774562/
C
C   INITIALIZE
C
      LA = (IDA+1) * (M+1) * (N+1)
      LB = (NRHS+1) * (NMAT+1) * (N+1)
      T = F7 / T30
      DO 100 K = 0, N
        DO 100 J = -M, 0
          DO 100 I = 0, IDA
            T = MOD (F7 * T, 1.)
            AX(I,J,K) = T
100     CONTINUE
      DO 110 K = 0, N
        DO 110 J = 0, NMAT
          DO 110 I = 0, NRHS
            T = MOD (F7 * T, 1.)
            BX(I,J,K) = T
110   CONTINUE
      TM = CPTIME ()
C
C   BEGIN TIMING TEST
C
      DO 120 J = 1, IT
        CALL COPY (LA, AX, A)
        CALL COPY (LB, BX, B)
        CALL CHOLSK (IDA, NMAT, M, N, A, NRHS, IDA, B)
120   CONTINUE
C
      TM = CPTIME ()
      ER = ABS ((B(1,19,19) - ANS) / ANS)
      FP = IT * (NMAT + 1) * 4403.
C
      RETURN
      END
C
      SUBROUTINE CHOLSK (IDA, NMAT, M, N, A, NRHS, IDB, B)
C
C   CHOLESKY DECOMPOSITION/SUBSTITUTION SUBROUTINE.
C
C   11/28/84  D H BAILEY  MODIFIED FOR NAS KERNEL TEST
C
      REAL A(0:IDA, -M:0, 0:N), B(0:NRHS, 0:IDB, 0:N), EPSS(0:256)
      DATA EPS/1E-13/
C
C  CHOLESKY DECOMPOSITION
C
      DO 1 J = 0, N
        I0 = MAX ( -M, -J )
C
C  OFF DIAGONAL ELEMENTS
C
        DO 2 I = I0, -1
          DO 3 JJ = I0 - I, -1
            DO 3 L = 0, NMAT
3             A(L,I,J) = A(L,I,J) - A(L,JJ,I+J) * A(L,I+JJ,J)
          DO 2 L = 0, NMAT
2           A(L,I,J) = A(L,I,J) * A(L,0,I+J)
C
C  STORE INVERSE OF DIAGONAL ELEMENTS
C
        DO 4 L = 0, NMAT
4         EPSS(L) = EPS * A(L,0,J)
        DO 5 JJ = I0, -1
          DO 5 L = 0, NMAT
5           A(L,0,J) = A(L,0,J) - A(L,JJ,J) ** 2
        DO 1 L = 0, NMAT
1         A(L,0,J) = 1. / SQRT ( ABS (EPSS(L) + A(L,0,J)) )
C
C  SOLUTION
C
      DO 6 I = 0, NRHS
        DO 7 K = 0, N
          DO 8 L = 0, NMAT
8           B(I,L,K) = B(I,L,K) * A(L,0,K)
          DO 7 JJ = 1, MIN (M, N-K)
            DO 7 L = 0, NMAT
7             B(I,L,K+JJ) = B(I,L,K+JJ) -  A(L,-JJ,K+JJ) * B(I,L,K)
C
        DO 6 K = N, 0, -1
          DO 9 L = 0, NMAT
9           B(I,L,K) = B(I,L,K) * A(L,0,K)
          DO 6 JJ = 1, MIN (M, K)
            DO 6 L = 0, NMAT
6             B(I,L,K-JJ) = B(I,L,K-JJ) - A(L,-JJ,K) * B(I,L,K)
C
      RETURN
      END
C
      SUBROUTINE BTRTST (ER, FP, TM)
C
C   BTRIX TEST PROGRAM
C
      PARAMETER (JD=30, KD=30, LD=30, MD=30, F7=78125., T30=1073741824.)
      COMMON /ARRAYS/ S(JD,KD,LD,5), A(5,5,MD,MD), B(5,5,MD,MD),
     $     C(5,5,MD,MD), SX(JD,KD,LD,5), BX(5,5,MD,MD)
      DATA JS/2/, JE/29/, LS/2/, LE/29/, IT/20/, ANS/-0.286282658663962/
C
C   INITIALIZATION
C
      NB = 25 * MD * MD
      NS = JD * KD * LD * 5
      T = F7 / T30
      DO 100 L = 1, MD
        DO 100 K = 1, MD
          DO 100 J = 1, 5
            DO 100 I = 1, 5
              T = MOD (F7 * T, 1.)
              A(I,J,K,L) = T
              T = MOD (F7 * T, 1.)
              BX(I,J,K,L) = T
              T = MOD (F7 * T, 1.)
              C(I,J,K,L) = T
100   CONTINUE
      DO 110 L = 1, 5
        DO 110 K = 1, LD
          DO 110 J = 1, KD
            DO 110 I = 1, JD
              T = MOD (F7 * T, 1.)
              SX(I,J,K,L) = T
110   CONTINUE
      TM = CPTIME ()
C
C   TIMING TEST
C
      DO 120 II = 1, IT
        CALL COPY (NS, SX, S)
        DO 120 K = 1, KD
          CALL COPY (NB, BX, B)
          CALL BTRIX (JS, JE, LS, LE, K)
120   CONTINUE
C
      TM = CPTIME ()
      ER = ABS ((S(19,19,19,1) - ANS) / ANS)
      FP = IT * MD * (LE - 1) * 19165.
C
      RETURN
      END
C
      SUBROUTINE BTRIX (JS, JE, LS, LE, K)
C
C     VECTORIZED BLOCK TRI-DIAGONAL SOLVER IN THE J DIRECTION
C          FOR K = CONSTANT PLANES
C
C   11/15/84  D H BAILEY  MODIFIED FOR NAS KERNEL TEST
C
      PARAMETER (JD=30, KD=30, LD=30, MD=30)
      COMMON /ARRAYS/ S(JD,KD,LD,5), A(5,5,MD,MD), B(5,5,MD,MD),
     $     C(5,5,MD,MD)
C
      DIMENSION    U12(MD), U13(MD), U14(MD), U15(MD), U23(MD),
     $             U24(MD), U25(MD), U34(MD), U35(MD), U45(MD)
C
      REAL         L11(MD), L21(MD), L31(MD), L41(MD), L51(MD),
     $             L22(MD), L32(MD), L42(MD), L52(MD), L33(MD),
     $             L43(MD), L53(MD), L44(MD), L54(MD), L55(MD)
C
C     PART 1.  FORWARD BLOCK SWEEP
C
C
      DO 100 J   = JS,JE
C
C**********   STEP 1.  CONSTRUCT L(I) IN B    **************************
C
      IF(J.EQ.JS) GO TO 4
      DO 3 M = 1,5
        DO 3 N = 1,5
          DO 3 L = LS,LE
            B(M,N,J,L) = B(M,N,J,L) - A(M,1,J,L)*B(1,N,J-1,L)
     $           - A(M,2,J,L)*B(2,N,J-1,L) - A(M,3,J,L)*B(3,N,J-1,L)
     $           - A(M,4,J,L)*B(4,N,J-1,L) - A(M,5,J,L)*B(5,N,J-1,L)
    3 CONTINUE
C
    4 CONTINUE
C
C**********    STEP 2.  CONPUTE L INVERSE    ***************************
C
C
C        A.  DECOMPOSE L(I) INTO L AND U
C
      DO 20 L = LS,LE
        L11(L)  = 1. / B(1,1,J,L)
        U12(L)  = B(1,2,J,L)*L11(L)
        U13(L)  = B(1,3,J,L)*L11(L)
        U14(L)  = B(1,4,J,L)*L11(L)
        U15(L)  = B(1,5,J,L)*L11(L)
        L21(L)  = B(2,1,J,L)
        L22(L)  = 1. / (B(2,2,J,L) - L21(L)*U12(L))
        U23(L)  = (B(2,3,J,L) - L21(L)*U13(L)) * L22(L)
        U24(L)  = (B(2,4,J,L) - L21(L)*U14(L)) * L22(L)
        U25(L)  = (B(2,5,J,L) - L21(L)*U15(L)) * L22(L)
        L31(L)  = B(3,1,J,L)
        L32(L)  = B(3,2,J,L) - L31(L)*U12(L)
        L33(L)  = 1. / (B(3,3,J,L) - L31(L)*U13(L) - L32(L)*U23(L))
        U34(L)  = (B(3,4,J,L) - L31(L)*U14(L) - L32(L)*U24(L)) * L33(L)
        U35(L)  = (B(3,5,J,L) - L31(L)*U15(L) - L32(L)*U25(L)) * L33(L)
   20 CONTINUE
C
      DO 25 L = LS,LE
        L41(L)  = B(4,1,J,L)
        L42(L)  = B(4,2,J,L) - L41(L)*U12(L)
        L43(L)  = B(4,3,J,L) - L41(L)*U13(L) - L42(L)*U23(L)
        L44(L)  = 1. / (B(4,4,J,L) - L41(L)*U14(L) - L42(L)*U24(L)
     $      - L43(L)*U34(L))
        U45(L)  = (B(4,5,J,L) - L41(L)*U15(L) - L42(L)*U25(L)
     $      - L43(L)*U35(L)) * L44(L)
        L51(L)  = B(5,1,J,L)
        L52(L)  = B(5,2,J,L) - L51(L)*U12(L)
        L53(L)  = B(5,3,J,L) - L51(L)*U13(L) - L52(L)*U23(L)
        L54(L)  = B(5,4,J,L) - L51(L)*U14(L) - L52(L)*U24(L)
     $      - L53(L)*U34(L)
        L55(L)  = 1. / (B(5,5,J,L) - L51(L)*U15(L) - L52(L)*U25(L)
     $      - L53(L)*U35(L) - L54(L)*U45(L))
   25 CONTINUE
C
C**********    STEP 3.  SOLVE FOR INTERMEDIATE VECTOR    ***************
C
C         A.  CONSTRUCT RHS
C
      IF(J.EQ.JS) GO TO 34
      DO 33 M = 1,5
        DO 33 L = LS,LE
          S(J,K,L,M) = S(J,K,L,M) - A(M,1,J,L)*S(J-1,K,L,1)
     $         - A(M,2,J,L)*S(J-1,K,L,2) - A(M,3,J,L)*S(J-1,K,L,3)
     $         - A(M,4,J,L)*S(J-1,K,L,4) - A(M,5,J,L)*S(J-1,K,L,5)
   33 CONTINUE
C
C         B. INTERMEDIATE VECTOR
C
   34 CONTINUE
C
C         FWD SUBSTITUTION
C
      DO 35 L = LS,LE
        D1 = S(J,K,L,1)*L11(L)
        D2 = (S(J,K,L,2) - L21(L)*D1) * L22(L)
        D3 = (S(J,K,L,3) - L31(L)*D1 - L32(L)*D2) * L33(L)
        D4 = (S(J,K,L,4) - L41(L)*D1 - L42(L)*D2 - L43(L)*D3) * L44(L)
        D5 = (S(J,K,L,5) - L51(L)*D1 - L52(L)*D2 - L53(L)*D3
     $       - L54(L)*D4) * L55(L)
C
C         BWD SUBSTITUTION
C
        S(J,K,L,5)  = D5
        S(J,K,L,4)  = D4 - U45(L)*D5
        S(J,K,L,3)  = D3 - U34(L)*S(J,K,L,4) - U35(L)*D5
        S(J,K,L,2)  = D2 - U23(L)*S(J,K,L,3) - U24(L)*S(J,K,L,4)
     $       - U25(L)*D5
        S(J,K,L,1)  = D1 - U12(L)*S(J,K,L,2) - U13(L)*S(J,K,L,3)
     $       - U14(L)*S(J,K,L,4) - U15(L)*D5
   35 CONTINUE
C
C**********    STEP 4.  CONSTRUCT U(I) = L(I)**(-1)*C(I+1)    **********
C**********             BY COLUMNS AND STORE IN B            **********
C
      IF(J.EQ.JE) GO TO 100
      DO 40 N = 1,5
        DO 40 L = LS,LE
C
C         FWD SUBSTITUTION
C
          C1 = C(1,N,J,L)*L11(L)
          C2 = (C(2,N,J,L) - L21(L)*C1) * L22(L)
          C3 = (C(3,N,J,L) - L31(L)*C1 - L32(L)*C2) * L33(L)
          C4 = (C(4,N,J,L) - L41(L)*C1 - L42(L)*C2 - L43(L)*C3)
     $          * L44(L)
          C5 = (C(5,N,J,L) - L51(L)*C1 - L52(L)*C2 - L53(L)*C3
     $          - L54(L)*C4) * L55(L)
C
C         BWD SUBSTITUTION
C
          B(5,N,J,L)  = C5
          B(4,N,J,L)  = C4 - U45(L)*C5
          B(3,N,J,L)  = C3 - U34(L)*B(4,N,J,L) - U35(L)*C5
          B(2,N,J,L)  = C2 - U23(L)*B(3,N,J,L) - U24(L)*B(4,N,J,L)
     $         - U25(L)*C5
          B(1,N,J,L)  = C1 - U12(L)*B(2,N,J,L) - U13(L)*B(3,N,J,L)
     $         - U14(L)*B(4,N,J,L) - U15(L)*C5
   40 CONTINUE
C
C
  100 CONTINUE
C
C     PART 2.  BACKWARD BLOCK SWEEP
C
      JEM1 = JE - 1
C
      DO 200 J = JEM1,JS,-1
        DO 200 M = 1,5
          DO 200 L = LS,LE
            S(J,K,L,M) = S(J,K,L,M) - B(M,1,J,L)*S(J+1,K,L,1)
     $           - B(M,2,J,L)*S(J+1,K,L,2) - B(M,3,J,L)*S(J+1,K,L,3)
     $           - B(M,4,J,L)*S(J+1,K,L,4) - B(M,5,J,L)*S(J+1,K,L,5)
  200 CONTINUE
C
      RETURN
      END
C
      SUBROUTINE GMTTST (ER, FP, TM)
C
      PARAMETER (NW=100, NB=5, F7=78125., T30=1073741824.)
      COMPLEX WALL, ZCR, PROJ, ZI, Z1, ZZ
      COMMON /ARRAYS/ NWALL(NB), WALL(NW,NB), RMATRX(NW*NB,NW*NB),
     $ ZCR(NW,NB), PROJ(NW,NB), XMAX(NB)
      DATA IT/2/, ANS/-2.57754233214174/
C
C   INITIALIZATION
C
      LW = 2 * NW * NB
      T2 = F7 / T30
      DO 100 J = 1, NB
        NWALL(J) = NW
100   CONTINUE
      DO 110 J = 1, NB
        DO 110 I = 1, NW
          T1 = MOD (F7 * T2, 1.)
          T2 = MOD (F7 * T1, 1.)
          WALL(I,J) = CMPLX (T1, T2)
110   CONTINUE
      TM = CPTIME ()
C
C   TIMING TEST
C
      DO 120 I = 1, IT
        CALL GMTRY
120   CONTINUE
C
      TM = CPTIME ()
      ER = ABS ((RMATRX(19,19) - ANS) / ANS)
      FP = IT * (120. * (NB*NW) ** 2  +  0.666 * (NB*NW) ** 3)
C
      RETURN
      END
C
      SUBROUTINE GMTRY
C
C  COMPUTE SOLID-RELATED ARRAYS,  GAUSS ELIMINATE THE MATRIX OF WALL
C  INFLUENCE COEFFICIENTS.
C
C    11/30/84  D H BAILEY    REVISED CODE FOR NAS KERNEL TEST
C
      PARAMETER (NW=100, NB=5)
      COMPLEX WALL, ZCR, PROJ, ZI, Z1, ZZ
      COMMON /ARRAYS/ NWALL(NB), WALL(NW,NB), RMATRX(NW*NB,NW*NB),
     $ ZCR(NW,NB), PROJ(NW,NB), XMAX(NB)
C
      DATA  ARCL /5./, PI /3.141592653589793/, PERIOD/3./
C
C  COMPUTE ARCLENGTH.
C
      MATDIM = 0
      ARCL = 0.
      YMIN = 1.E+50
      YMAX = -1.E+50
      PIDP = PI / PERIOD
C
      DO 9 L = 1, NB
        MATDIM = MATDIM + NWALL(L)
        DO 9 K = 1,NWALL(L)
          ARCL = ARCL + ABS(WALL(K,L) - WALL(1+MOD(K,NWALL(L)), L))
9     CONTINUE
C
C  COMPUTE CORE RADIUS.
C
      R0 = ARCL / (MATDIM*2.)
      SIGMA = R0 / 2.
C
C  DEFINE CREATION POINTS.
C
      DO 6 L = 1,NB
        DO 5 K = 1,NWALL(L)
          ZZ = WALL(1+MOD(K+NWALL(L)-2,NWALL(L)), L)
     &         - WALL(1+MOD(K,NWALL(L)), L)
          ZCR(K,L) = WALL(K,L) + CMPLX(0., R0/ABS(ZZ)) * ZZ
5       CONTINUE
C
C  CHECK THAT WALL AND CREATION POINTS ARE NOT CROSSED DUE TO
C  TOO SHARP A CONCAVE KINK OR AN ERROR IN DEFINING THE BODY.
C  ALSO FIND HIGHEST,  LOWEST AND RIGHT-MOST POINT.
C
        XMAX(L) = REAL(ZCR(1,L))
        LS = 0
        DO 6 K = 1,NWALL(L)
          YMIN = MIN (YMIN, AIMAG(ZCR(K,L)))
          YMAX = MAX (YMAX, AIMAG(ZCR(K,L)))
          XMAX(L) = MAX (XMAX(L), REAL(ZCR(K,L)))
          KP = 1 + MOD(K, NWALL(L))
          IF (REAL((ZCR(KP,L) - ZCR(K,L)) *
     &        CONJG(WALL(KP,L) - WALL(K,L))).GT.0.) THEN
            LS = L
            KS = K
          ENDIF
6     CONTINUE
C
C       IF (LS .NE. 0) THEN
C         WRITE (6, 102) LS, KS
C102      FORMAT(' ON BODY NUMBER ', I3, ' YOU HAVE TOO SHARP A',
C     &      ' KINK NEAR POINT ', I4)
C         STOP
C       ENDIF
C
C  THE "MAIN PERIOD" WILL BE BETWEEN YLIMIT AND YLIMIT + PERIOD.
C
      YLIMIT = (YMIN - PERIOD + YMAX)/2
C
C  PROJECT CREATION POINTS INTO MAIN PERIOD. THIS IS TECHNICAL.
C
      DO 1 L = 1,NB
        DO 1 K = 1,NWALL(L)
          PROJ(K,L) = ZCR(K,L) - CMPLX(0., PERIOD*
     &           (INT(5. + (AIMAG(ZCR(K,L)) - YLIMIT) / PERIOD) - 5.))
1     CONTINUE
C
C   COMPUTE MATRIX.
C
      SIG2 = (2. * PIDP * SIGMA) ** 2
      I0 = 0
      DO 2 L1 = 1,NB
        J0 = 0
        DO 4 L2 = 1,NB
          KRON = 0
          IF (L1 .EQ. L2) KRON = 1
          DO 3 J = 1,NWALL(L2)
            RMATRX(I0+1,J0+J) = KRON
            Z1 = EXP ((WALL(1,L1) - ZCR(J,L2)) * PIDP)
            Z1 = Z1 - 1. / Z1
            DUM = SIG2 + REAL(Z1)**2 + AIMAG(Z1)**2
            DO 3 I = 2,NWALL(L1)
              ZI = EXP ((WALL(I,L1) - ZCR(J,L2)) * PIDP)
              ZZ = ZI - 1. / ZI
              RMATRX(I0+I,J0+J) = -0.25 / PI * LOG (DUM /
     &              (SIG2 + REAL(ZZ) ** 2 + AIMAG(ZZ) ** 2))
3         CONTINUE
          J0 = J0 + NWALL(L2)
4       CONTINUE
        I0 = I0 + NWALL(L1)
2     CONTINUE
C
C  GAUSS ELIMINATION
C
      DO 8 I = 1, MATDIM
        RMATRX(I,I) = 1. / RMATRX(I,I)
        DO 8 J = I+1, MATDIM
          RMATRX(J,I) = RMATRX(J,I) * RMATRX(I,I)
          DO 8 K = I+1, MATDIM
            RMATRX(J,K) = RMATRX(J,K) - RMATRX(J,I) * RMATRX(I,K)
8     CONTINUE
C
      RETURN
      END
C
      SUBROUTINE EMITST (ER, FP, TM)
C
C   EMIT TEST SUBROUTINE
C
      PARAMETER (NW=100, NB=5, NV=1000, NVM=1500, F7=78125.,
     $ T30=1073741824.)
      COMPLEX Z, WALL, ZCR, REFPT,  EXPWKL, EXPMWK, FORCE,
     & UUPSTR, DUM3, EXPZ, EXPMZ
      COMMON /ARRAYS/ NWALL(NB), WALL(NW,NB), RMATRX(NW*NB,NW*NB),
     $ ZCR(NW,NB), Z(NVM), GAMMA(NVM), REFPT(NB), RHS(NW*NB),
     $ FORCE(NB), RMOM(NB), CP(NW,NB), DPDS(NW,NB), EXPZ(NVM),
     $ EXPMZ(NVM), PSI(NW), PS(NVM)
      DATA IT/10/, ANS/6.0088546832072/
C
      T2 = F7 / T30
      DO 100 J = 1, NB
        NWALL(J) = NW
        REFPT(J) = 0.
        FORCE(J) = 0.
        RMOM(J) = 0.
        DO 100 I = 1, NW
          T1 = MOD (F7 * T2, 1.)
          T2 = MOD (F7 * T1, 1.)
          WALL(I,J) = CMPLX (T1, T2)
          T1 = MOD (F7 * T2, 1.)
          T2 = MOD (F7 * T1, 1.)
          ZCR(I,J) = CMPLX (T1, T2)
          DPDS(I,J) = 0.
100   CONTINUE
      DO 110 J = 1, NW*NB
        RMATRX(J,J) = 1.
        DO 110 I = 1, J-1
          T2 = MOD (F7 * T2, 1.)
          RMATRX(I,J) = .001 * T2
          RMATRX(J,I) = .001 * T2
110   CONTINUE
      DO 120 I = 1, NVM
        T1 = MOD (F7 * T2, 1.)
        T2 = MOD (F7 * T1, 1.)
        Z(I) = CMPLX (T1, T2)
        T2 = MOD (F7 * T2, 1.)
        GAMMA(I) = T2
120   CONTINUE
      TM = CPTIME ()
C
C   TIMING TEST
C
      DO 130 I = 1, IT
        CALL EMIT
130   CONTINUE
C
      TM = CPTIME ()
      ER = ABS ((RHS(19) - ANS) / ANS)
      FP = IT * (56.*NV + NB*NW * (97. + 44.*NV + 2.*NB*NW))
C
      RETURN
      END
C
      SUBROUTINE EMIT
C
C  EMIT NEW VORTICES TO SATISFY BOUNDARY CONDITION.
C  FINISH COMPUTING PRESSURE, FORCES, ETC.
C
C   11/28/84  D H BAILEY  MODIFIED FOR NAS KERNEL TEST
C
      PARAMETER (NW=100, NB=5, NVM=1500)
      COMPLEX Z, WALL, ZCR, REFPT,  EXPWKL, EXPMWK, FORCE,
     & UUPSTR, DUM3, EXPZ, EXPMZ, ZZ
      COMMON /ARRAYS/ NWALL(NB), WALL(NW,NB), RMATRX(NW*NB,NW*NB),
     $ ZCR(NW,NB), Z(NVM), GAMMA(NVM), REFPT(NB), RHS(NW*NB),
     $ FORCE(NB), RMOM(NB), CP(NW,NB), DPDS(NW,NB), EXPZ(NVM),
     $ EXPMZ(NVM), PSI(NW), PS(NVM)
C
      DATA  PERIOD/3./, SIG2/3./, U0/4./, MATDIM/500/, DELT/1./,
     $  CHORD/5./, PI/3.141592653589793/, UUPSTR/(3., 4.)/
C
C  STORE EXP(Z(I)) AND EXP(-Z(I)) TO REDUCE WORK IN INNER LOOP 4.
C
      NV = 1000
      PIDP = PI / PERIOD
C
      DO 2 I = 1, NV
        EXPZ(I) = EXP (Z(I) * PIDP)
        EXPMZ(I) = 1. / EXPZ(I)
2     CONTINUE
C
      I0 = 0
      CUPST = REAL(UUPSTR) ** 2 + AIMAG(UUPSTR) ** 2
C
      DO 5 L = 1, NB
        DO 6 K = 1, NWALL(L)
          EXPWKL = EXP (WALL(K,L) * PIDP)
          EXPMWK = 1. / EXPWKL
          SPS = 0.
          DO 4 I = 1, NV
            DUM3 = EXPZ(I) * EXPMWK - EXPWKL * EXPMZ(I)
            PS(I) = GAMMA(I) * LOG (REAL(DUM3) ** 2 +
     &              AIMAG(DUM3) ** 2 + SIG2)
            SPS = SPS + PS(I)
4         CONTINUE
          PSI(K) = AIMAG(WALL(K,L) * CONJG (UUPSTR + CMPLX (0., U0)))
     &             - SPS * 0.25 / PI
6       CONTINUE
C
C  COMPUTE RIGHT-HAND SIDE.
C
        DO 8 K = 1, NWALL(L)
          RHS(I0+K) = PSI(K) - PSI(1)
8       CONTINUE
        I0 = I0 + NWALL(L)
5     CONTINUE
C
C  SOLVE SYSTEM
C
      DO 10 I = 1, MATDIM
        DO 10 J = I+1, MATDIM
          RHS(J) = RHS(J) - RMATRX(J,I) * RHS(I)
10    CONTINUE
      DO 11 I = MATDIM, 1, -1
        RHS(I) = RMATRX(I,I) * RHS(I)
        DO 11 J = 1, I-1
          RHS(J) = RHS(J) - RMATRX(J,I) * RHS(I)
11    CONTINUE
C
C  CREATE NEW VORTICES.
C
      NOLLD = NV
      I0 = 0
      DO 7 L = 1, NB
        DO 3 K = 1, NWALL(L)
C
C  PUT THE NEW VORTEX AT THE END OF THE ARRAY.
C
          NV = NV+1
          Z(NV) = ZCR(K,L)
          GAMMA(NV) = RHS(I0+K)
C
C  RECORD THE GAIN OF LINEAR AND ANGULAR MOMENTUM
C
          FORCE(L) = FORCE(L) + GAMMA(NV) * Z(NV)
          RMOM(L) = RMOM(L) + GAMMA(NV) * (REAL (Z(NV) - REFPT(L)) ** 2
     &              + AIMAG (Z(NV) - REFPT(L)) ** 2)
          DPDS(K,L) = DPDS(K,L) - GAMMA(NV)
3       CONTINUE
C
C  FILTER AND INTEGRATE PRESSURE GRADIENT TO GET PRESSURE
C
        CP(1,L) = 0.
        CPM = -1E50
        DO 9 K = 2, NWALL(L)
          CP(K,L) = CP(K-1,L) + (3. * (DPDS(K,L) + DPDS(K-1,L))
     &              + DPDS(1+MOD(K,NWALL(L)), L)
     &              + DPDS(1+MOD(K+NWALL(L)-3, NWALL(L)), L))
     &              / (4. * DELT * CUPST)
          CPM = MAX (CPM, CP(K,L))
9       CONTINUE
C
C  NORMALIZE PRESSURE
C
        DO 12 K = 1, NWALL(L)
          CP(K,L) = CP(K,L) - CPM
12      CONTINUE
C
C  FINISH COMPUTING FORCE AND MOMENT,  AS TIME RATE OF CHANGE OF LINEAR
C  AND ANGULAR MOMENTUM
C
        FORCE(L) = FORCE(L) * CMPLX (0., 2. / (DELT * CHORD * CUPST))
        RMOM(L) = RMOM(L) * 2. / (DELT * CHORD ** 2 * CUPST)
C
        I0=I0+NWALL(L)
7     CONTINUE
C
      RETURN
      END

      SUBROUTINE VPETST (ER, FP, TM)
C
C   VPENTA TEST PROGRAM
C
      PARAMETER (NJA=128, NJB=128, JL=1, JU=128, KL=1, KU=128,
     $ F7=78125., T30=1073741824.)
      COMMON /ARRAYS/ A(NJA,NJB), B(NJA,NJB), C(NJA,NJB), D(NJA,NJB),
     $ E(NJA,NJB), F(NJA,NJB,3), X(NJA,NJB), Y(NJA,NJB), FX(NJA,NJB,3)
      DATA IT/400/, ANS/-0.354649411858726/
C
      LF = NJA * NJB * 3
      T = F7 / T30
      DO 100 J = KL, KU
        DO 100 I = JL, JU
          T = MOD (F7 * T, 1.)
          A(I,J) = T
          T = MOD (F7 * T, 1.)
          B(I,J) = T
          T = MOD (F7 * T, 1.)
          C(I,J) = T
          T = MOD (F7 * T, 1.)
          D(I,J) = T
          T = MOD (F7 * T, 1.)
          E(I,J) = T
          DO 100 K = 1, 3
            T = MOD (F7 * T, 1.)
            FX(I,J,K) = T
100   CONTINUE
      TM = CPTIME ()
C
C   TIMING TEST
C
      DO 110 I = 1, IT
        CALL COPY (LF, FX, F)
        CALL VPENTA
110   CONTINUE
C
      TM = CPTIME ()
      ER = ABS ((F(19,19,1) - ANS) / ANS)
      FP = IT * KU * (40. * KU - 53.)
C
      RETURN
      END
C
      SUBROUTINE VPENTA
C
C    ROUTINE TO INVERT 3 PENTADIAGONALS SIMULTANEOUSLY
C
C   12/05/84  D H BAILEY   MODIFIED FOR NAS KERNEL TEST
C
      PARAMETER (NJA=128, NJB=128, JL=1, JU=128, KL=1, KU=128)
      COMMON /ARRAYS/ A(NJA,NJB), B(NJA,NJB), C(NJA,NJB), D(NJA,NJB),
     $ E(NJA,NJB), F(NJA,NJB,3), X(NJA,NJB), Y(NJA,NJB), FX(NJA,NJB,3)
C
C	! START FORWARD GENERATION PROCESS AND SWEEP
C
      J = JL
      DO 1 K = KL,KU
        RLD = C(J,K)
        RLDI = 1./RLD
        F(J,K,1) = F(J,K,1)*RLDI
        F(J,K,2) = F(J,K,2)*RLDI
        F(J,K,3) = F(J,K,3)*RLDI
        X(J,K) = D(J,K)*RLDI
        Y(J,K) = E(J,K)*RLDI
1     CONTINUE
C
      J = JL+1
      DO 2 K = KL,KU
        RLD1 = B(J,K)
        RLD = C(J,K) - RLD1*X(J-1,K)
        RLDI = 1./RLD
        F(J,K,1) = (F(J,K,1) - RLD1*F(J-1,K,1))*RLDI
        F(J,K,2) = (F(J,K,2) - RLD1*F(J-1,K,2))*RLDI
        F(J,K,3) = (F(J,K,3) - RLD1*F(J-1,K,3))*RLDI
        X(J,K) = (D(J,K) - RLD1*Y(J-1,K))*RLDI
        Y(J,K) = E(J,K)*RLDI
2     CONTINUE
C
      DO 3 J = JL+2,JU-2
        DO 11 K = KL,KU
          RLD2 = A(J,K)
          RLD1 = B(J,K) - RLD2*X(J-2,K)
          RLD = C(J,K) - (RLD2*Y(J-2,K) + RLD1*X(J-1,K))
          RLDI = 1./RLD
          F(J,K,1) = (F(J,K,1) - RLD2*F(J-2,K,1) - RLD1*F(J-1,K,1))*RLDI
          F(J,K,2) = (F(J,K,2) - RLD2*F(J-2,K,2) - RLD1*F(J-1,K,2))*RLDI
          F(J,K,3) = (F(J,K,3) - RLD2*F(J-2,K,3) - RLD1*F(J-1,K,3))*RLDI
          X(J,K) = (D(J,K) - RLD1*Y(J-1,K))*RLDI
          Y(J,K) = E(J,K)*RLDI
11      CONTINUE
3     CONTINUE
C
      J = JU-1
      DO 12 K = KL,KU
        RLD2 = A(J,K)
        RLD1 = B(J,K) - RLD2*X(J-2,K)
        RLD = C(J,K) - (RLD2*Y(J-2,K) + RLD1*X(J-1,K))
        RLDI = 1./RLD
        F(J,K,1) = (F(J,K,1) - RLD2*F(J-2,K,1) - RLD1*F(J-1,K,1))*RLDI
        F(J,K,2) = (F(J,K,2) - RLD2*F(J-2,K,2) - RLD1*F(J-1,K,2))*RLDI
        F(J,K,3) = (F(J,K,3) - RLD2*F(J-2,K,3) - RLD1*F(J-1,K,3))*RLDI
        X(J,K) = (D(J,K) - RLD1*Y(J-1,K))*RLDI
12    CONTINUE
C
      J = JU
      DO 13 K = KL,KU
        RLD2 = A(J,K)
        RLD1 = B(J,K) - RLD2*X(J-2,K)
        RLD = C(J,K) - (RLD2*Y(J-2,K) + RLD1*X(J-1,K))
        RLDI = 1./RLD
        F(J,K,1) = (F(J,K,1) - RLD2*F(J-2,K,1) - RLD1*F(J-1,K,1))*RLDI
        F(J,K,2) = (F(J,K,2) - RLD2*F(J-2,K,2) - RLD1*F(J-1,K,2))*RLDI
        F(J,K,3) = (F(J,K,3) - RLD2*F(J-2,K,3) - RLD1*F(J-1,K,3))*RLDI
13    CONTINUE
C
C        !  BACK SWEEP SOLUTION
C
      DO 14 K = KL,KU
        F(JU,K,1) = F(JU,K,1)
        F(JU,K,2) = F(JU,K,2)
        F(JU,K,3) = F(JU,K,3)
        F(JU-1,K,1) = F(JU-1,K,1) - X(JU-1,K)*F(JU,K,1)
        F(JU-1,K,2) = F(JU-1,K,2) - X(JU-1,K)*F(JU,K,2)
        F(JU-1,K,3) = F(JU-1,K,3) - X(JU-1,K)*F(JU,K,3)
14    CONTINUE
C
      DO 4 J = 2,JU-JL
        JX = JU-J
        DO 15 K = KL,KU
          F(JX,K,1) = F(JX,K,1) - X(JX,K)*F(JX+1,K,1) -
     *                Y(JX,K)*F(JX+2,K,1)
          F(JX,K,2) = F(JX,K,2) - X(JX,K)*F(JX+1,K,2) -
     *                Y(JX,K)*F(JX+2,K,2)
          F(JX,K,3) = F(JX,K,3) - X(JX,K)*F(JX+1,K,3) -
     *                Y(JX,K)*F(JX+2,K,3)
15      CONTINUE
4     CONTINUE
C
      RETURN
      END
