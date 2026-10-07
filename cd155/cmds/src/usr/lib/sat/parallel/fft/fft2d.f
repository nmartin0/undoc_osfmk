	subroutine fft2d(
     &    a,
c	    -- in out
     &    m,
c	    -- in
     &    n,
c	    -- in
     &    mdivp, 
c	    -- in
     &    p, 
c	    -- in
     &	  isign,
c	    -- in
     &    buf,
c	    -- scratch
     &	  wsave,
c	    __ fft constants
     &	  distrib)
c	    -- in
c
c  -- 	Source name: fft2d
c
c  --	File type: FORTRAN
c
c  -- 	Function:
c	  -- This routine computes a forward or inverse 2d fft with memory-
c	  -- resident data. The decomposition that is assumed is a division
c	  -- of the rows of the matrix of data. That is, for a 64 x 64
c	  -- problem on 4 nodes, the first 16 rows must be stored in the
c	  -- the first node, the second 16 rows must be in the second node, etc.
c	  -- Upon execution, the transformed data is stored in exactly the
c	  -- same way as the input data when the parameter distrib .ne. 2.
c    	  -- When distrib = 2, the data is returned with the second dimension
c	  -- distriduted.
c	  -- Both dimensions must be a power of 2
	
c  --	Error messages:
c	  -- mdivp is not m/p
c	  -- # rows is not a power of 2
c	  -- # columns is not a power of 2
c
c  --	Warnings: This routine will not work if
c	  -- n*mdivp > (memory per node in Mbytes)/32. This means that the
c	  --   problem is too big to fit in core.
c
c  --	Application calls:
c	  -- bfft (file bfft.f)
c	  -- global23 (file global23.f)
c	  -- local12 (file local12.f)
c
c  --   History:
c	  --  11-1-89, D. Scott:   Original implementation
c	  --  1-31-90, E. Kushner:  Capability expanded to cover non-square
c		          matrices
c	  --  11-1-90, E. Kushner:  Modified for the iPSC/860
c  --	Parameters:
	integer m, 
c	    -- First dimension of global matrix (number of rows)
     &    n, 
c	    -- Second dimension of global matrix (number of columns)
     &    mdivp,
c	    -- m/p
     &    p,
c	    -- Number of nodes utilized - must be a power of 2
     &	  isign,
c	    -- Indicated if forward or inverse fft
     &    distrib
c	    -- If equal to 2 the data is returned with the second dimension
c		distributed. Otherwise, the data is returned with the first
c		dimension distributed (i.e.,the original decomposition)
	complex*8 a(mdivp, *), 
c	    -- Matrix to be transformed (input). Transformed matrix (output)
     &    buf(mdivp, *),
c	    -- Buffer used to communicate data globally and transpose data
c 	    -- locally
     &	  wsave(*)
c	    -- fft constants
c
c  --	End header
c
	include 'fnx.h'
	integer me,mpid,item,i,iadd,i1,j1,large_dim,ndivp
	data large_dim /20/

	me = mynode()
	mpid = mypid()
	ndivp = n/p

c  checks on inputs

	if(mdivp .ne.  m/p) then
	   print *, 'mdivp is not m/p'
	   stop
	endif
        item = 1
        do 2 i=1,large_dim
          if(n.eq.item) go to 4
          item = 2*item
 2      continue
        print *, '# rows not a power of 2. Zero fill to a power of 2'
        stop
 4      continue
        item = 1
        do 6 i=1,large_dim
          if(m.eq.item) go to 8
          item = 2*item
 6      continue
        print *, '# columns not a power of 2. Zero fill to a power of 2'
        stop
 8      continue

c  first dimension

	call bfft(n, mdivp, a, 1, mdivp, isign, wsave)

c  transpose

 	if(numnodes().gt.1) then
	  call global23(ndivp*mdivp, p, a, buf, me, mpid)

	  do 10 i = 1, n, ndivp
	    call local12(mdivp,ndivp, a(1,i), buf)
10	  continue
        endif

c  2nd dimension

 	if(m .ne. n) call cfft1d(a, m, 0, wsave)
 	if(numnodes().eq.1) then
 	  call bfft(m, ndivp, a, mdivp, 1, isign, wsave)
 	else
	  call bfft(m, ndivp, a, 1, ndivp, isign, wsave)

c transpose back

 	  if (distrib .ne. 2) then
 	    call global23(ndivp*mdivp, p, a, buf, me, mpid)
 	    do 20 i = 1, n, ndivp
 	      call local12(ndivp,mdivp, a(1,i),buf)
 20	    continue
 	  endif
 	endif

	return
	end
