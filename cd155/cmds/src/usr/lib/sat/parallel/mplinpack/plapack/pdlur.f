      subroutine pdlur( m, n, a, lda, ipiv, icurrow, 
     $     work, ldwork, nb, info )
*
*     .. scalar arguments ..
      integer            info, lda, m, n
*     ..
*     .. array arguments ..
      integer            ipiv( * )
      double precision   a( lda, * )
*
*     icur               index of row that holds current diagonal 
*                        block
*     nb                 block size of wrapping
*
      double precision   work(ldwork, *)
      integer            icurrow, nb, ldwork
*     ..
*
*  purpose
*  =======
*
*  pdlur computes an lu factorization of a general m-by-n matrix a
*  using partial pivoting with row interchanges.
*
*  the factorization has the form
*     a = p * l * u
*  where p is a permutation matrix, l is lower triangular with unit
*  diagonal elements (lower trapezoidal if m > n), and u is upper
*  triangular (upper trapezoidal if m < n).
*
*  this is the right-looking level 2 blas version of the algorithm.
*
*  arguments
*  =========
*
*  m       (input) integer
*          the number of rows of the matrix a.  m >= 0.
*
*  n       (input) integer
*          the number of columns of the matrix a.  n >= 0.
*
*  a       (input/output) double precision array, dimension (lda,n)
*          on entry, the m by n matrix to be factored.
*          on exit, the factors l and u; the unit diagonal elements of l
*          are not stored.
*
*  lda     (input) integer
*          the leading dimension of the array a.  lda >= max(1,m).
*
*  ipiv    (output) integer array, dimension (min(m,n))
*          the pivot indices.  row i of the matrix was interchanged with
*          row ipiv(i).
*
*  info    (output) integer
*          = 0: successful exit
*          < 0: if info = -k, the k-th argument had an illegal value
*          > 0: if info = k, u(k,k) is exactly zero. the factorization
*               has been completed, but the factor u is exactly
*               singular, and division by zero will occur if it is used
*               to solve a system of equations or to compute the inverse
*               of a.
*
*  =====================================================================
*
*     this version dated 09/18/92
*     r. van de geijn
*
*     all rights reserved
*
*     .. parameters ..
      double precision   one, zero
      parameter          ( one = 1.0d+00, zero = 0.0d+00 )
*     ..
*     .. local scalars ..
      integer            j, jp
*     ..
*     .. local scalars ..
*
*     nprow          row dimension of node grid
*     npcol          column dimension of node grid
*
*     important:  this code assumes nprow < npcol
*
*     myrow          my row index
*     mycol          my column index
*
      integer           nprow, npcol, myrow, mycol, myrrow
      integer           mymrow, ipivnode
      integer           idummy
      double precision  ddummy, dtemp
*     ..
*     .. external functions ..
      integer           idamax
      external          idamax
*     ..
*     .. external functions ..
      integer           itype_to, itype_from, itype_col
      double precision  dabs
*     ..
*     .. external subroutines ..
      external           dger, dscal, dswap, xerbla, plamch2
*     ..
*     .. intrinsic functions ..
      intrinsic          max, min
*     ..
*     .. executable statements ..
*
*     test the input parameters.
*
      info = 0
      if( m.lt.0 ) then
         info = -1
      else if( n.lt.0 ) then
         info = -2
*      else if( lda.lt.max( 1, m ) ) then
*         info = -4
      end if
      if( info.ne.0 ) then
         call xerbla( 'pdlur  ', -info )
         return
      end if
*
*     get machine parameters
*
      call plamch2( nprow, npcol, myrow, mycol )

*
*     myrrow =    row index of node relative to node with row index
*                 icurrow
*                 
      myrrow = mod( myrow - icurrow + nprow, nprow )

*
*     mymrow = number of rows assigned to this node
*
      call imypart( 1, m, nb, idummy, mymrow, myrrow, nprow )

*
      do 10 j = 1, min( nb, n )
*
*           find pivot and test for singularity.
*
         if ( myrow .eq. icurrow ) then
            jp = j - 1 + idamax( mymrow-j+1, a( j, j ), 1 )
            call dcopy( n, a( jp, 1 ), lda, work( j, 1 ), 
     $           ldwork )
         else
            if (mymrow .ge. 1) then
               jp = idamax( mymrow, a( 1, j ), 1 )
               call dcopy( n, a( jp, 1 ), lda, work( j, 1 ), 
     $              ldwork )
            else
               jp = 0
               work(j,j) = 0.0d0
            endif
         endif
c
c        determine pivot row
c
         dtemp = dabs( work( j, j ) )
         call dgamax2d( 'column', 1, 1, dtemp, 1, ddummy, 1, 
     $        ipivnode, 1, idummy, 1, -1, mycol )

         if ( ddummy .eq. 0.0d00 ) ipivnode = icurrow
c
c        send pivot row to all nodes in this column
c
         call dgebc2d( 'ctree', 'column', 1, n, work( j, 1 ), ldwork,
     $        ipivnode, mycol, itype_col() )

         if (myrow .eq. icurrow) then
            if ( ipivnode .eq. myrow ) then
               if ( j .ne. jp ) 
     $              call dswap( n, a( j, 1 ), lda, a( jp, 1 ), 
     $              lda )
               ipiv( j ) = jp
            else
               call dgesd2d( 1, n, a( j, 1 ), lda, ipivnode, 
     $              mycol, itype_to( ipivnode, mycol ) )
               call dcopy( n, work( j, 1 ), ldwork, a( j, 1 ), lda )
               ipiv( j ) = - ipivnode
            endif
         elseif ( myrow .eq. ipivnode ) then
            call dgerv2d( 1, n, a( jp, 1 ), lda, 
     $           itype_from( icurrow, mycol) )
            ipiv( j ) = jp
         else
            ipiv( j ) = - ipivnode
         endif
*
*           compute elements j+1:m of j-th column.
*
         if (work ( j, j ) .eq. zero ) then
            print *, "exact singularity detected:trouble !!"
         else   
            if (myrow .eq. icurrow) then
               if( j.lt.mymrow )
     $              call dscal( mymrow-j, one/work( j, j ), 
     $              a( j+1, j ), 1 )
            else
               if ( mymrow .gt. 0 )
     $              call dscal( mymrow, one/work( j, j ), a( 1, j ),
     $              1 )
            endif
         endif
*
*

         if( j+1.le.n .and. work (j, j) .ne. zero) then
*
*           update trailing submatrix.
*
            if ( myrow .eq. icurrow ) then
               if (mymrow .gt. j) 
     $              call dger( mymrow-j, n-j, -one, a( j+1, j ), 
     $              1, 
     $              work ( j, j+1 ), ldwork, a( j+1, j+1 ), lda )
            elseif ( mymrow .gt. 0 ) then
               call dger( mymrow, n-j, -one, a( 1, j ), 1, 
     $              work ( j, j+1 ), ldwork, a( 1, j+1 ), lda )
            endif
         end if

 10   continue

      return
*
*     end of pdlur
*
      end
 

