      subroutine  col_dcolx_post( index_col, work2 )
      int index_col( * )
      double work2( * )

      return
      end


      subroutine col_dcolx( x, index, xall )

      integer             me, nprocs, nprows, npcols, myrow, 
     $     mycol, pid
      common /gridinfo/   me, nprocs, nprows, npcols, myrow, 
     $     mycol, pid
      integer             force_type
      parameter           (force_type = 1100000000)
      double precision x( * ), xall( * ), work( * )
      integer          index( * ), up, length, in, i, 
     $     msgid( 32 ), down

      if (nprows .eq. 1) then
         call dcopy( index(2) - index( 1 ), x, 1, xall, 1 )
      else
c
c        post receives
c
         do i=1, nprows-1
            in = mod( myrow+i, nprows )
            length = index( in+2 ) - index( in+1 ) 
            msgid( i ) =  irecv( force_type + 9999+i-1, 
     $           xall( index( in+1 ) ), 8*length )
         enddo
c
c        synchronize
c
         down = mod( myrow+1, nprows ) + mycol * nprows
         call csend( 9998, 0, 0, down, pid )
         call crecv( 9998, 0, 0 ) 
c         
         up = mod( myrow-1+nprows, nprows ) + mycol * nprows
c
c        copy this node's part into correct place
c
         length = index( myrow+2 ) - index( myrow+1 ) 
         call dcopy( length, x, 1, xall( index( myrow+1 ) ), 1 )
         call csend( force_type+9999, x, 8*length, up, pid )
         do i=1, nprows-1
            in = mod( myrow+i, nprows )
            length = index( in+2 ) - index( in+1 ) 
c            call crecv( 9999+i-1, xall( index( in+1 ) ), 8*length )
            call msgwait( msgid( i ) )
            if (i .lt. nprows-1) 
     $           call csend( force_type+9999+i, 
     $           xall( index( in+1 ) ), 8*length, up, pid )
         enddo
      endif

      return
      end

            
         
