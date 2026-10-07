      subroutine matvec1  ( lastrw, a, ixstart, iyindex, x, y )

      integer             me, nprocs, nprows, npcols, myrow, 
     $     mycol, pid
      common /gridinfo/   me, nprocs, nprows, npcols, myrow, 
     $     mycol, pid
      integer lastrw, ixstart( * ), iyindex( * ), i, j
      double precision a( * ), x( * ),  y( * )
      double precision  ddoti

c      call gsync()
c      print *, "==========matvec===="
c      call gsync()

      do i = 1, lastrw
c         print *, myrow, mycol, i, ixstart(i+1)-ixstart(i) 
c         y (i) = ddoti ( ixstart (i+1) - ixstart (i), a (ixstart (i)),
c     1                   iyindex (ixstart (i)), x )
c         y( i ) = 0
         do j=ixstart(i), ixstart(i+1)-1
            y( i ) = y( i ) + a( j ) * x ( iyindex( j ) )
         enddo
      enddo


c      call gsync()
c      print *, "==========matvec===="
c      call gsync()

      return

      end
