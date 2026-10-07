      subroutine pmatvec( localn, a, ixstart, iyindex, x, y, work1,
     $     xall )
      
      integer             me, nprocs, nprows, npcols, myrow, 
     $     mycol, pid
      common /gridinfo/   me, nprocs, nprows, npcols, myrow, 
     $     mycol, pid
      integer ixstart( * ), iyindex( * ), i, localn
      double precision a( * ), x( * ), y( * ), xall( * ), 
     $     yloc( 14000 ), work1( * ), work(14000), yy(14000)
      integer index_all( 513 ), ntot, index_row( 33 ),
     $     index_col( 33 ), n_row, n_col
      common /vectdist/ index_all, index_row, index_col, 
     $     n_row, n_col
      double precision    dclock, mvtime
      common / timings / mvtime
      integer id
c
c        assemble this column's portion of x
c

c      id = irecv( 1100000000+12345, y, 8*localn )
 
c      call gsync()
c      print *, "============================="
c      call gsync()

c      do i=1,localn
c         print *, myrow, mycol, "x(i)", i, x(i)
c      enddo
 
c      call gsync()

c      print *, me, "===calling rowdcol ========"
c      call gsync()

      call col_dcolx( x, index_col, xall )

c      print *, myrow, mycol , "xall (1) = ", xall(1)
c      print *, myrow, mycol , "a(1) = ", a(1)
c      print *, myrow, mycol , "ixstart(1) = ", ixstart(1)
c      print *, myrow, mycol , "iyindex(1) = ", iyindex(1)

c      call gsync()
c      print *, "============================="
c      call gsync()

c      print *, myrow, mycol, "n_col = ", n_col
c      print *, myrow, mycol, "n_row = ", n_row

c      do i=1,n_col
c         print *, myrow, mycol, "xall(i)", i, xall(i)
c      enddo

c      call gsync()
c      print *, "===calling mv==============="
c      call gsync()

c      dtime = dclock()
c      dtime = dclock() - dtime
c      mvtime = mvtime + dtime

c      call gsync()
c      print *, "======yloc==================="
c      call gsync()

c      do i=1,n_row
c         print *, myrow, mycol , "yloc (i) = ", i, yloc(i)
c      enddo
c      call gsync()
c      print *, "====calling coldsum=============="
c      call gsync()

c      call matvec ( n_row, a, ixstart, iyindex, xall, yloc )

c       call row_dsumx_matvec( yy, index_row, yloc, n_row, a, 
       call row_dsumx_matvec( y, index_row, yloc, n_row, a, 
     $     ixstart, iyindex, xall )

c      call gsync()
c      print *, "====transposing=============="
c      call gsync()
c
c     transpose
c
c      call csend( 12345, y, 8*n_col, myrow * nprows + mycol, pid )
c      call crecv( 12345, y, 8*n_row )
       myyindex = myrow*npcols + mycol
       idest = myyindex/nprows
       jdest = mod(myyindex,nprows)
       kdest = idest*nprows+jdest
       kdest = iproc(jdest, idest)

c      call csend( 1100000000+12345, yy, 8*(index_row( mycol+2) - 
c     $     index_row(mycol+1)), kdest, pid )

c      call msgwait(id)

      

      return
      end
