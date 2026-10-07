      subroutine pmatvec_row_dsumx( n_row, a, ixstart, 
     $     iyindex, xall,
     $     y, index, yloc, work ) 

      double precision y( * ), yloc( * ), work( * ), xall( * )
      double precision a( * )
      integer ixstart( * ), iyindex( * ), n_row

      integer             me, nprocs, nprows, npcols, myrow, 
     $     mycol, pid
      common /gridinfo/   me, nprocs, nprows, npcols, myrow, 
     $     mycol, pid
      integer          index( * ), left, i, length, icur,
     $     msgid( 33 ), right, smsgid( 33 )
      integer             force_type
      parameter           (force_type = 1100000000)

c      n_row = index(nprows+1) - 1
c      call matvec  ( n_row, a, ixstart, iyindex, xall, yloc )

c      icur = myrow+1
c      do i=1,nprows
c         irow = index(icur)
c         call matvec( index(icur+1) - irow, 
c     $        a, ixstart(irow), 
c     $        iyindex, xall ,
c     $        yloc( irow ) )
c         icur = mod( icur, nprows ) + 1
c      enddo

c      call gsync()
c      print *, "=====colsum=================="
c      call gsync()

c      do i=1,npcols+1
c         print *, myrow, mycol, "index(i)", i, index(i)
c      enddo
      
c      call gsync()
c      print *, "============================="
c      call gsync()

      if (npcols .eq. 1) then
         call dcopy( index(2) - index( 1 ), yloc, 1, y, 1 )
      else
c
c        post receives
c
         do i=1, npcols-1
c            call gsync()
c            print *, "=============================", i
c            call gsync()
            icur = mod( mycol+i+1, npcols )
            length = index( icur+2 ) - index( icur+1 ) 
            msgid( i ) = irecv( force_type+99999+i-1, 
     $           work( index( icur+1 ) ), 
     $           8*length )
         enddo
c
c        synchronze
         right = myrow + mod( mycol+1, npcols ) * nprows
         call csend( 99998, 0, 0, right, pid )
         call crecv( 99998, 0, 0 )

         left = myrow + mod( mycol-1+npcols, npcols ) * nprows
         icur = mod( mycol+1, npcols )
         length = index( icur+2 ) - index( icur+1 ) 
c         print *, myrow, mycol, "length = ", length
c         print *, myrow, mycol, "index( icur+1 ) ", 
c     $        index( icur+1 )

         irow = index(icur+1)
         call matvec( index(icur+2) - irow, 
     $        a, ixstart(irow), 
     $        iyindex, xall ,
     $        yloc( irow ) )

         smsgid(1) = isend( force_type+99999, 
     $        yloc( index( icur+1 ) ), 
     $        8*length, left, pid )

         do i=1, npcols-1
c            call gsync()
c            print *, "=============================", i
c            call gsync()
            icur = mod( mycol+i+1, npcols )
            length = index( icur+2 ) - index( icur+1 ) 
            irow = index(icur+1)

            call matvec( length,
     $           a, ixstart(irow), 
     $           iyindex, xall ,
     $           yloc( irow ) )
c     $           work( irow ) )

            call msgwait( msgid( i ) )

c            do j=irow,irow+length-1
c               work( j ) = work(j) + yloc(j)
c            enddo
            call daxpy( length, 1.0d00, yloc( irow ),
     $           1, work( irow ), 1 )
            if (icur .ne. mycol ) then
               smsgid(i+1) = isend( force_type+99999+i, 
     $              work( irow ), 
     $              8*length, left, pid )
            else
               call dcopy( length, work( irow ), 1,
     $              y, 1 )
            endif
         enddo
         
         do i=1,npcols-1
            call msgwait( smsgid( i ) )
         enddo
      endif

      return
      end

            
         


