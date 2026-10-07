      subroutine pdlaswp( n, a, lda, jfirst, jlast, ii, icurrow,
     $     ipiv, incx )

      integer              n, lda, jfirst, jlast, ii, icurrow, incx

      integer myrow, mycol, nprow, npcol

      integer i, jb

      double precision a( lda, * )
      integer ipiv( * ), itype_to, itype_from
*
*     this version dated 09/18/92
*     r. van de geijn
*
      call plamch2( nprow, npcol, myrow, mycol )

      if (incx .ne. 1) then
         print *, "pdlawsp: incx .ne. 1 not implemented"
         stop
      endif

      jb = jlast - jfirst + 1

      do 10 i=1,jb
         if (myrow .eq. icurrow) then
            if (ipiv(jfirst+i-1) .ge. 1) then
               call dswap( n, a( ii+i-1, 1 ), lda, 
     $              a( ipiv(jfirst+i-1), 1 ), lda)
            else
               call dgesd2d( 1, n, a( ii+i-1, 1 ), lda,
     $              -ipiv(jfirst+i-1), mycol, 
     $              itype_to(-ipiv(jfirst+i-1), mycol) )
               call dgerv2d( 1, n, a( ii+i-1, 1 ), lda,
     $              itype_from(-ipiv(jfirst+i-1), mycol) )
            endif
         else
            if (ipiv(jfirst+i-1) .ge. 1) then
               call dgesd2d( 1, n, a(ipiv(jfirst+i-1),1), 
     $              lda, icurrow, mycol, 
     $              itype_to(icurrow, mycol) )
               call dgerv2d( 1, n, a(ipiv(jfirst+i-1),1), 
     $              lda, itype_from(icurrow, mycol) )
            endif
         endif
 10   continue
      
      return
      end
