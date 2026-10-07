	subroutine fft3d( l, m, n, p, isign, a, buf, wsave)
c	History
c	  -- 11-1-89, D. Scott:  Original implementation
c	  -- 1-31-90, E. Kushner: Capability expanded to cover non_square
c		matrices
c	  -- 11-1-90, E. Kushner: Modified for the iPSC/860
c	  -- 7-10-91, E. Kushner: Modified for the FFTPDE NAS parallel bench-
c	  	mark
	complex*16 a(l,m,*), buf(*), wsave(*)
	integer l, m, n, p, isign
	integer me, mpid, ndivp, i
	include 'fnx.h'

	me = mynode()
        p = numnodes()
	mpid = mypid()
	ndivp = n/p
        mdivp = m/p

C  CHECKS ON INPUTS

	if(ndivp .le. 0) then
	   print *, 'third dimension too small for configuration', n, p
	   stop
	endif
        item = 1
        do 2 i=1,20
          if(l.eq.item) go to 4
          item = 2*item
 2      continue
        print*, 'First dimension not a power of 2. Zero fill to a power 
     &of 2.'
        stop
 4      continue
        item = 1
        do 6 i=1,16
          if(m.eq.item) go to 7
          item = 2*item
 6      continue
        print*, 'Second dimension not a power of 2. Zero fill to a power
     & of 2.'
        stop
 7      continue
        item = 1
        do 8 i=1,20
          if(n.eq.item) go to 9
          item = 2*item
 8      continue
        print*, 'Third dimension not a power of 2. Zero fill to a power 
     &of 2.'
        stop
 9      continue

C  FORWARD TRANSFORM PORTION

 	call zfft1d(a, m, 0, wsave)
	do 10 i = 1, ndivp
	   call bfft( m, l, a(1,1,i), 1, l, isign, wsave, buf)
10	continue
c	if(me.eq.0) write(6,*) 'after first bfft'
 	if(l .ne. m) call zfft1d(a, l, 0, wsave)
 	do 15 i = 1, ndivp
	   call bfft( l, m, a(1,1,i), l, 1, isign, wsave, buf)
15	continue
c	if(me.eq.0) write(6,*) 'after secnd bfft'

C   INTERPRET DATA SLICE AS L(X) x MDIVP(Y) x P(Y) x [P(T)]
C  AND CHANGE DATA SLICE TO L(X) x NDIVP(Y) x P(T) x [P(Y)] 

 	if(p .gt. 1) then
	  call local23(l*mdivp, p, ndivp, a, buf)
c	  if(me.eq.0) write(6,*) 'after first local'
	  call global23(l*mdivp*ndivp, p, a, buf, me, mpid)
c	  if(me.eq.0) write(6,*) 'after first globl'
 	  if(n .ne. l) call zfft1d(a, n, 0, wsave)
	  do 20 i = 1, mdivp
	    call bfft( n, l, a(1,i,1), 1, l*mdivp, isign, wsave, buf)
20	  continue
c	  if(me.eq.0) write(6,*) 'after third bfft'
 	else
 	  if(n .ne. l) call zfft1d(a, n, 0, wsave)
	  do 25 i = 1, m
	    call bfft( n, l, a(1,i,1), 1, l*m, isign, wsave, buf)
25	  continue
 	endif

C   DO A COPY TO TRANSFORM DATA SLICE FROM L(X) x NDIVP(Y) x P(T) x [P(Y)]
C  TO A DATA SLICE OF L(X) x N(T) x MDIV(Y) x [P(Y)]
 	if(p .gt. 1) then
 	  call local23_sm(l, mdivp, ndivp, p, a, buf)
 	endif

	return
	end

