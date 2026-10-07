      PROGRAM FFTPDE
C
C   This is the standard Fortran-77 version of the APP Benchmark 4, the
C   3-D FFT PDE benchmark.
C   On 64 bit systems, double precision should be disabled.
C   Computer specific and tuning notes may be located by searching for C>>.
C>>
C   David H. Bailey     January 8, 1991
C
C   In the following parameter statement, M1, M2 and M3 are the Log_2 of the
C   three dimensions of the 3-D input array.  Set MX = MAX (M1, M2, M3).
C   A is the multiplier of the random number generator (here set to 5^13),
C   and S is the initial seed.  AL is the value of alpha.  NT is the number
C   of iterations.  dclock is a double precision function that returns elapsed
C   CPU time in seconds.
C
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
c
      DIMENSION buf(4)
c Dynamic memory allocation
      include 'dyn_mem.h'
      complex*16 x0(len), x1(len), 
     &	x2(len)
      DIMENSION X3(len)
      pointer (p2, x0)
      pointer (p3, x1)
      pointer (p4, x2)
      pointer (p1, x3)
      complex*16  u(1024), ctmp
      integer m1, m2, m3, mx, mm, mp, n1, n2, n3, nn
      integer first, last, p, me, isign, n3divp
      integer n2divp, first_2, last_2
      integer itmp1, itmp2, iadd
      include 'fnx.h'
C
C   Initialize.
C
      p = numnodes()
      me = mynode()
      if (p .eq. 64 .or. p .eq. 128) then
 	m1 = 8
 	m2 = 8
 	m3 = 7
 	mx = 8
	mm = m1 + m2 + m3
 	mp = m3
	n1 = 256
 	n2 = 256
 	n3 = 128
 	nn = 2 ** mm
 	if (me .eq. 0) write(6,*) 'Solving the standard problem (256x256x128)
     &	on',p,' nodes'
      else if ( p .eq. 32) then
        m1 = 7
        m2 = 7
        m3 = 7
        mx = 7
        mm = m1 + m2 + m3
        mp = m3
        n1 = 128
        n2 = 128
        n3 = 128
        nn = 2 ** mm
 	if (me .eq. 0) write(6,*) 'Solving a reduced problem (128x128x128)
     &	on',p,' nodes'
      else
 	if (me .eq. 0) then
 	  write(6,*) 'PDEFFT only runs on 32, 64 or 128 nodes'
 	  stop
 	endif
      endif
* Allocate arrays
      allocate (x0, stat=istat)
c     write(6,*) 'x0 allocated on node ', me, istat
      allocate (x1, stat=istat)
c     write(6,*) 'x1 allocated on node ', me, istat
      allocate (x2, stat=istat)
c     write(6,*) 'x2 allocated on node ', me, istat
      allocate (x3, stat=istat)
c     write(6,*) 'x3 allocated on node ', me, istat
      if (me. eq. 0) WRITE (6, 1) N1, N2, N3, p
 1    FORMAT ('3-D FFT PDE TEST'/'DIMENSIONS =',4I5)
      call gsync()
      TM0 = dclock ()
      n3divp = n3/p
      first = me * n3divp + 1
      last = (me + 1) * n3divp
      n2divp = n2/p
      first_2 = me * n2divp + 1
      last_2 = (me + 1) * n2divp
c     if (me.eq.0) WRITE (6, *) 'before 1st call to zfft1d'
      CALL zfft1d(x0, n2, 0, u)
c     if (me. eq. 0) WRITE (6, *) '1st call to zfft1d completed'
c     CALL VRANLC (0, T1, A, X0)
      CALL VRANLC (0, 0, T1, A, X3)
      MQ = MM - MP
      NQ = 2 ** MQ
      RN = 1.D0 / NN
      AP = - 4.D0 * AL * PI ** 2
      N12 = N1 / 2
      N22 = N2 / 2
      N32 = N3 / 2
C
C   Compute AN = A ^ (2 * NQ) (mod 2^46).
C
      T1 = A
C
      DO 100 I = 1, MQ + 1
        T2 = RANDLC (T1, T1)
 100  CONTINUE
C
      AN = T1
      TT = S
C
C   Each instance of this loop may be performed independently.
C
      t_1 = dclock()
c     if(mynode() .eq. 0) write(6,*) 'init time = ',t_1 - TM0
      do 130 k = first, last
        KK = K - 1
        KL = KK
        T1 = S
        T2 = AN
C
C   Find starting seed T1 for this KK using the binary rule for exponentiation.
C
        DO 110 I = 1, 100
          IK = KK / 2
          IF (2 * IK .NE. KK) T3 = RANDLC (T1, T2)
          IF (IK .EQ. 0) GOTO 120
          T3 = RANDLC (T2, T2)
          KK = IK
 110    CONTINUE
C
C   Compute 2 * N1 * n2 pseudorandom numbers.
C
 120    CALL VRANLC (N1, n2, T1, A, X3)
 	itmp1 = (k - first) * n1 * n2
 	do 125 j = 1, n2
 	  itmp2 = (j - 1) * n1
 	  do 125 i = 1, n1
 	    iadd = itmp1 + itmp2 + i
 	    x1(iadd) = dcmplx(x3(itmp2+i), x3(itmp2+i+n1*n2))
 125	continue
 130  CONTINUE
c     if(mynode() .eq. 0) write(6,*) 'begin 3dfft '
C
C   Perform a forward 3-D FFT on X1.
C
c     CALL CFFT3 (-1, M1, M2, M3, N1, N2, N3, U, X1, X0, Y)
      isign = -1
c     write(6,*) me,
c    &	x1(n1 * n2 * n3divp)
      call fft3d(n1, n2, n3, p, isign, x1, x0, u)	
c     if (mynode() .eq. 0) write(6,*) 'forward done'
C
C   Compute exponential terms.
C
      DO 180 J = first_2, last_2
c	if (mynode() .eq. 0) write(6,*) 'j = ',j
        J1 = J - 1
        IF (J .GT. N22) J1 = J1 - N2
 	itmp1 = n1 * n3 * (j - first_2)
C
        DO 190 K = 1, n3
          K1 = K - 1
          IF (K .GT. N32) K1 = K1 - N3
          JK = J1 ** 2 + K1 ** 2
 	  itmp2 = n1 * (k - 1)
C
          DO 170 I = 1, N1
            I1 = I - 1
            IF (I .GT. N12) I1 = I1 - N1
 	    iadd = i + itmp2 + itmp1
            X3(Iadd) = EXP (AP * (I1 ** 2 + JK))
 170      CONTINUE
C
 190    CONTINUE
 180  CONTINUE
c     if(me.eq.0) write(6,*) 'start loop'
C
C   Perform the following for KT = 1, ..., NT.
C
      DO 270 KT = 1, NT
C
C   Multiply by the exponential term raised to the KT power.
C
        DO 210 J = first_2, last_2
 	  itmp1 = (j - first_2) * n3 * n1
          DO 220 K = 1, n3
 	    itmp2 = n1 * (k - 1)
            DO 200 I = 1, N1
 	      iadd = i + itmp2 + itmp1
              T1 = X3(iadd) ** KT
              X2(iadd) = T1 * X1(iadd)
 200        CONTINUE
c	    if(me.eq.p-1.and.j.eq.last_2.and.k.eq.n3) write(6,*) 'exp',
c    &	   	t1,x1(iadd),iadd
 220      CONTINUE
 210    CONTINUE
c     if(me.eq.0) write(6,*) 'exponential done'
C
C   Compute inverse 3-D FFT.
C
c       CALL CFFT3 (1, M1, M2, M3, N1, N2, N3, U, X2, X0, Y)
 	isign = 1
c     write(6,*) 'bef inverse ',me,
c    &	x2(n1*n3*n2divp)
 	call fft3d(n1, n3, n2, p, isign, x2, x0, u)
c       if (mynode() .eq. 0) write(6,*) 'aft inverse'
c     write(6,*) 'inv done',me,
c    &	x2(n1*n2*n3divp)
C
C   Compute checksum.
C
 	ctmp = (0.0d0, 0.0d0)
C
        DO 260 I = 1, 1024
          I1 = I - 1
          KK = MOD (5 * I1, N3) + 1
 	  if (kk .ge. first .and. kk .le. last) then
            II = MOD (I1, N1) + 1
            JJ = MOD (3 * I1, N2) + 1
 	    iadd = ii + n2 * (jj - 1) + n1 * n2 * (kk - first)
            ctmp = ctmp + X2(iadd)
c	    if(kt.eq.1) write(6,*) 'ctmp',me,i,x2(iadd)
 	  endif
 260    CONTINUE
 	buf(1) = dreal(ctmp)
 	buf(2) = dimag(ctmp)
 	buf(3) = 0.0d0
 	buf(4) = 0.0d0
 	call gdsum(buf(1), 2, buf(3))
C
        if (me .eq. 0) WRITE (6, *) 'T = ',KT,'CHECKSUM = ', buf(1),
     &	  buf(2)
 270  CONTINUE
C
      TM1 = dclock ()
      TM = TM1 - TM0
      call gdhigh(tm, 1, t1)
      if (me .eq. 0) then
 	if (m1 .eq. 7) WRITE (6, 3) TM, 1408 / TM
 	if (m1 .eq. 8) WRITE (6, 3) TM, 5631 / TM
      endif
 3    FORMAT ('TEST COMPLETED'/'CPU TIME =',F12.6,' MFLOPS = ',F12.6)
      deallocate (x0)
      deallocate (x1)
      deallocate (x2)
      deallocate (x3)
      if (me .eq. 0) STOP
      END
