      PROGRAM EMBAR
C
C   This is the standard Fortran-77 version of the APP Benchmark 1,
C   the "embarassingly parallel" benchmark.
C   On 64 bit systems, double precision should be disabled.
C
C   David H. Bailey     January 8, 1991
C
C   M is the Log_2 of the number of complex pairs of uniform (0, 1) random
C   numbers.  MK is the Log_2 of the size of each batch of uniform random
C   numbers.  MK can be set for convenience on a given system, since it does
C   not affect the results.  dclock is a double precision function that
C   returns elapsed CPU time in seconds.
C
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      include 'dyn_mem.h'
      DIMENSION Q(0:NQ-1)
c
c Dynamic memory allocation
      DIMENSION X(2*NK)
      pointer (p1, x)
c
C
      common /jrb/ gc,q
      dimension work(0:NQ-1)
      me = mynode()
      numno = numnodes()
c Allocate array
      allocate (x, stat = istat)
      do i = 1, 2*nk
 	x(i) = 0
      end do

      call gsync()
      TM0 = dclock ()
      CALL VRANLC (0, T1, A, X)
C
C   Compute AN = A ^ (2 * NK) (mod 2^46).
C
      T1 = A
C
      DO 100 I = 1, MK + 1
        T2 = RANDLC (T1, T1)
 100  CONTINUE
C
      AN = T1
      TT = S
      GC = 0.D0
C
      DO 110 I = 0, NQ - 1
        Q(I) = 0.D0
 110  CONTINUE
C
C   Each instance of this loop may be performed independently.
C

      DO 150 K = me+1, NN, numno
        KK = K - 1
        T1 = S
        T2 = AN
C
C   Find starting seed T1 for this KK.
C
        DO 120 I = 1, 100
          IK = KK / 2
          IF (2 * IK .NE. KK) T3 = RANDLC (T1, T2)
          IF (IK .EQ. 0) GOTO 130
          T3 = RANDLC (T2, T2)
          KK = IK
 120    CONTINUE
C
C   Compute uniform pseudorandom numbers.
C
 130    CALL VRANLC (2 * NK, T1, A, X)
C
C   On a single processor system, the 120 loop and line 130 can be replaced
C   by the following single line.
C
C        CALL VRANLC (2 * NK, TT, A, X)
C
C   Compute Gaussian deviates by acceptance-rejection method and tally counts
C   in concentric square annuli.   This loop is not vectorizable.
C
        DO 140 I = 1, NK
          X1 = 2.D0 * X(2*I-1) - 1.D0
          X2 = 2.D0 * X(2*I) - 1.D0
          T1 = X1 * X1 + X2 * X2
          IF (T1 .LE. 1.D0) THEN
            GC = GC + 1.D0
            T2 = SQRT (-2.D0 * LOG (T1) / T1)
            T3 = ABS (X1 * T2)
            T4 = ABS (X2 * T2)
            L = MAX (T3, T4)
            Q(L) = Q(L) + 1.D0
          ENDIF
 140    CONTINUE
C
 150  CONTINUE
C
      call gdsum(gc,nq+1,work)
      deallocate (x)
      TM1 = dclock () - TM0
      call gdhigh(tm1, 1, dummy)
      if (me.eq.0) WRITE(6,1) TM1,M, GC, (I, Q(I), I = 0, NQ - 1)
 1    FORMAT ('BENCHMARK 1 RESULTS:'//'CPU TIME =',F10.4/'N = 2^',I5/   
     $  'NO. GAUSSIAN PAIRS =',F15.0/'COUNTS:'/(I3,F15.0))
      if (me.eq.0) write(6,*) 'MFLOPS = ', 26680. / tm1
      if (me .eq. 0) STOP
      END
