      subroutine aitken( localn, a, ixstart, iyindex, work )

      integer me, nprocs, nprows, npcols, myrow, mycol, pid 
      common /gridinfo/ me, nprocs, nprows, npcols, myrow, mycol,
     $     pid

      integer ixstart( * ), iyindex( * ), localn
      double precision  a ( * ), work ( * ), chi, chi1, chi2
      integer i, it, itemp
      integer     idamax
      double precision ddummy, temp, rnorm
      double precision    result1(15), result2(15)
      common /RESULT/     result1, result2
c
c     x is stored in work( 1 ) through work ( localn )
c
      double precision  dabs

c       
c     initialize local portion of x
c
      do i=1,localn
         work(i) = 1.0d00
      enddo

      chi2 = 0
      chi1 = 0
      chi  = 0

      it = 0
      do i = 1,15
         it = it+1
c
c        solve A z = x
c
         call cg( localn, a, ixstart, iyindex, work, 
     $        work( localn+1 ), rnorm ) 
         
         chi2 = chi1
         chi1 = chi
c
c        compute max | x_i |
c
         itemp = idamax( localn, work, 1 )
         chi = dabs( work(itemp) )
         call gdhigh( chi, 1, ddummy )
         chi = 1/chi
c         print *, "chi = ", chi
c
c        scale x
c
         call dscal( localn, chi, work, 1 )

         temp = chi - (chi - chi1) * (chi - chi1)/
     $               (chi - 2*chi1 + chi2)

         if (mynode() .eq. 0) print *, it, temp, rnorm

         result1(it) = temp
         result2(it) = rnorm
      enddo
c
c     print final extrapolation and residual
c 
      return
      end
