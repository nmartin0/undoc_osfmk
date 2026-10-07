      program main

      integer             me, nprocs, nprows, npcols, myrow, 
     $     mycol, pid
      common /gridinfo/   me, nprocs, nprows, npcols, myrow, 
     $     mycol, pid
      integer             localn, maxn, maxnnz, n
      parameter           ( maxn = 15000, maxnnz = 130000 ) 
c     parameter           ( maxn = 20000, maxnnz = 200000 ) 
c      parameter           ( maxn = 20000, maxnnz = 100000 ) 
      integer             ixstart( maxn ), iyindex( maxnnz )
      double precision    a( maxnnz ), work( 5 * maxn ) 
      integer             mypid
      intrinsic           mod
      double precision    time, dclock, mvtime
      common / timings / mvtime
      double precision    result1(15), result2(15)
      common /RESULT/     result1, result2
c
c     initialize logical processor grid 
c
      if (mynode() .eq. 0) then
c         print *, "enter n:"
c         read *, n
         n = 14000
         call csend( 1, n, 4, -1, mypid() )
      else
         call crecv( 1, n, 4 )
      endif

c      n = 14000
c      n = 1400
c      n = 14
      call initgrid( nprocs, me, nprows, npcols, myrow, mycol )
      pid = mypid()
c      
c     compute length of local portion of distributed vectors
c
      localn = n/nprocs
c      if ( me .lt. mod( n, nprocs ) ) localn = localn+1
      if ( mycol+myrow*npcols .lt. mod( n, nprocs ) ) 
     $     localn = localn+1

c
c     initialize the local portion of the matrix as well as
c     related information given the distrubtion of a 
c     distributed vector.
c
      call init_matrix( n, localn, a, ixstart, iyindex )

      if (me .eq. 0) print *, "back from init_matrix"

      do i=1,2
      mvtime = 0.0d0
      call gsync()
      time = dclock()

      call aitken( localn, a, ixstart, iyindex, work )

      call gsync()
      time = dclock() - time

      if (me .eq. 0) then
         print *, "TIME = ", time, " MFLOPS = ", 1508 / time
         print *, "mvTIME = ", mvtime
c         do i=1,15
c            print *, i, result1(i), result2(i)
c         enddo
      endif

      enddo

      if (mynode() .eq. 0) stop
      end



