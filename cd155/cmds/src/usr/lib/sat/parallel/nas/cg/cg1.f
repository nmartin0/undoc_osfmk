      subroutine cg( localn, a, ixstart, iyindex, x, work, 
     $     rnorm ) 

      integer             me, nprocs, nprows, npcols, myrow, 
     $     mycol, pid
      common /gridinfo/   me, nprocs, nprows, npcols, myrow, 
     $     mycol, pid
      integer localn, ixstart( * ), iyindex( * )  
      double precision  a( * ), x( * ), work( * ), work1( 14000)
      double precision work2(14000), z( 14000 )
      double precision  rho, alpha, rho0, beta
      integer   ir, ip, iq, iz, i
      double precision  ddot, ddots(3), dtemps(3), rhonew,
     $     alphanew, rhoupdate1, rhoupdate2
      double precision  ddummy, dtemp, rnorm, sqrt, dabs
      integer index_all( 513 ), ntot, index_row( 33 ),
     $     index_col( 33 ), n_row, n_col
      common /vectdist/ index_all, index_row, index_col, 
     $     n_row, n_col
      
c
c     all vectors are distributed vectors
c 
c     note:  r starts at work(1)
c            p starts at work( localn+1 )
c            q starts at work( 2*localn+1 )
c            z starts at work( 3*localn+1 )
c
c
      ir = 1
      ip = localn+1
      iq = 2*localn+1
      iz = 3*localn+1
c
c     r = p = x
c
      call dcopy( localn, x, 1, work(ir), 1 )
      call dcopy( localn, x, 1, work(ip), 1 )
c
c     z = 0
c
      do i=1, localn
         work( iz+i-1 )  = 0.0d00
c         x( i )  = 0.0d00
      enddo
      
      call row_dsumx_post( index_row , work1 )
      call col_dcolx_post( index_col, work2 )
c
c     rho = < r , r >
c
      rho = ddot( localn, work, 1, work, 1 )
      call gdsum( rho, 1, ddummy )
c
c     do 25 times
c
      do i=1,25
c
c        q = A p  
c
         call pmatvec( localn, a, ixstart, iyindex, work( ip ),
     $        work( iq ), work1, work2 )

c         ddots(1) = ddot( localn, work( ip), 1, work( iq ), 1 )
c         ddots(2) = ddot( localn, work( ir), 1, work( iq ), 1 )
c         ddots(3) = ddot( localn, work( iq), 1, work( iq ), 1 )

         ddots(1) = 0
         ddots(2) = 0
         ddots(3) = 0

         do j=0,localn-1
            dtemp = work( iq+j)
            ddots(1) = ddots(1) + work( ip+j ) * dtemp
            ddots(2) = ddots(2) + work( ir+j ) * dtemp
            ddots(3) = ddots(3) + dtemp * dtemp
         enddo

c         if (i .lt. 25) then
            call row_dsumx_post( index_row , work1 )
            call col_dcolx_post( index_col , work2 )
c         endif

         call gdsum( ddots, 3, dtemps)

         rho0 = rho 
c
c        alpha = rho / < p , q >
c
         alpha = rho / ddots(1)
c
c        rho = rho - 2 alpha < r, q > + alpha^2 < q,q >
c 
         rho = rho - 2.0d00 * alpha * ddots(2)  + 
     $        alpha * alpha * ddots(3) 
c
c        z = z + alpha p
c
         call daxpy( localn, alpha, work(ip), 1, work( iz ), 1 )
c
c        r = r - alpha q
c
         call daxpy( localn, -alpha, work(iq), 1, work( ir ), 1 )
c
         beta = rho / rho0
c
c        p = r + beta p
c
         do j=0, localn-1
            work( ip+j ) = work( ir+j ) + beta * work( ip+j )
         enddo
      enddo

      call pmatvec( localn, a, ixstart, iyindex, work( iz ),
     $     work( iq ), work1, work2 )

      call daxpy( localn, -1.0d00, work( iq ), 1, x, 1 )
      dtemp =  ddot( localn, x, 1, x, 1 )
      
      call gdsum( dtemp, 1, ddummy )

      rnorm = sqrt( dtemp )

      call dcopy( localn, work( iz ), 1, x, 1 )

      return
      end


