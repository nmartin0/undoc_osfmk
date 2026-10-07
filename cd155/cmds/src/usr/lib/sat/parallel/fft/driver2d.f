	program fft2d_860
c
c	2-D FFT driver FOR iPSC/860 DMMP COMPUTERS
c
c	David Scott and Ed Kushner
c	Supercomputer Systems Division, Intel Corp.
c
        COMPLEX*8 wsave(12288)
  	complex*8 ctemp
 	real*8 tem1, tem, val
	INTEGER n, m, p, me, mpid, ndivp, mdivp, j, i
 	integer sign, distrib, ncase
	include 'fnx.h'
	real time_t, dummy, time_i, rate
c
c Dynamic memory allocation
        include 'dyn_mem.h'
c       COMPLEX*8 a(524288), buf(131072)
        COMPLEX*8 a(len_a), buf(len_buf)
        pointer (p1, a)
        pointer (p2, buf)
	me = mynode()
        if(me .eq. 0) write(6,*) 'first node started'
	if(me.eq.0) then
           open(unit=7,file='in.dat',status='old')
 	   read(7,999) ncase
 	   call csend( 5, ncase, 4, -1, 0)
 	else
 	   call crecv( 5, ncase, 4)
        endif
* Allocate arrays
        allocate (a, stat=istat)
c 	write(6,*) 'a allocated on node ', me, istat
        allocate (buf, stat=istat)
c 	write(6,*) 'buf allocated on node ', me, istat
        icase = 0
c .... The following line specifies that the data is to be returned to
c        the original decomposition (i.e., the first dimension distributed).
c        Use distrib = 2 to return the data with the second dimension distrib-
c 	 uted.
 	distrib = 1
 5      continue
        icase = icase+1
	if(mynode().eq.0) then
           print *, 'Case = ',icase
           read(7,999) m
           call csend(2,m,4,-1,0)
	else
           call crecv(2,m,4)
 	endif
	if(mynode().eq.0) then
	   read(7,999) n
	   call csend(1,n,4,-1,0)
 	else
	   call crecv(1,n,4)
	endif
        if(m*n.le.0) go to 100

	p = numnodes()
	ndivp = n/p
        mdivp = m/p
        if(mdivp.eq.0.or.ndivp.eq.0) then
          if(me.eq.0) print *, 'Problem too small for configuration.'
          if(me.eq.0) print *, 'Both dimensions must be >= numnodes.'
          go to 95
        endif
c .... The following line assumes 4-8 Mbytes of usable memory/node.
        if(n*mdivp.gt.262144) then
          if(me.eq.0) print *, 'Problem too big for configuration.'
          if(me.eq.0) print *, 'Add nodes or reduce size of problem.'
          go to 95
        endif 
	do 10 i = 1, n*mdivp
           tem1 = (me+1)*(i/n+1)
	   a(i) = cmplx(tem1,tem1)
10	continue
 	call cfft1d(buf, n, 0, wsave)
        call gsync()
	time_i = dclock()
 	sign = -1
 	call fft2d(a, m, n, mdivp, p, sign, buf, wsave, distrib)
 	sign = 1
 	if (distrib .ne. 2) then
 	  if(n .ne. m) call cfft1d(a, n, 0, wsave)
 	  call fft2d(a, m, n, mdivp, p, sign, buf, wsave, distrib)
 	else
 	  call fft2d(a, n, m, ndivp, p, sign, buf, wsave, distrib)
 	endif

	time_t = (dclock()-time_i)/2
	call gshigh(time_t, 1, dummy)
        if(me.eq.0) then
          flops1 = 7.21*alog(float(m))*m
          flops2 = 7.21*alog(float(n))*n
          rate = 1.0e-6*(n*flops1+m*flops2)/time_t
          write(6,998) m,n,time_t,rate
        endif
        val = 0
        do 90 i = 1,n*mdivp
          tem1 = (me+1)*(i/n+1)
          ctemp = cmplx(tem1,tem1)
          tem = cabs(a(i)-ctemp) / cabs(ctemp)
          val = amax1(val,tem)
  90    continue
 	call gdhigh(val, 1, tem)
        if(me .eq. 0) print *, 'max error = ',val
c       if(me.eq.0.and.val.gt..001) then
c         print *, 'first dim',(a(i),i=1,mdivp)
c         print *, 'second dim',(a(i),i=1,mdivp*n,mdivp)
c       endif
  95    continue
        if(icase .lt. ncase) go to 5
 100    continue
        deallocate (a)
        deallocate (buf)
 998    format('rows = ',i4,x,'cols = ',i4,x,'time fwd = ',f7.3,x,
     &          'Mflops = ',f8.2)
 999    format(i4)
 	if(me .eq. 0) stop
	end
