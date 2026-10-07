      subroutine pdlubr( m, n, nb, a, lda, work, ldwork, ipiv, info )
*
*     parallel blocked lu factorization
*
*
*
*     .. scalar arguments ..
      integer            lda, m, n, nb, ldwork, info
*     ..
*     .. array arguments ..
      integer            ipiv( * )
      double precision   a( lda, * ), work( * )
*     ..
*
*  purpose
*  =======
*
*  pdlubr computes an lu factorization of a general m-by-n matrix a
*  using partial pivoting with row interchanges.  it assumes this
*  node's portion of the matrix is in array a.  a block wrapped 
*  storage scheme with blocksize nb is assumed.
*
*  the factorization has the form
*     a = p * l * u
*  where p is a permutation matrix, l is lower triangular with unit
*  diagonal elements (lower trapezoidal if m > n), and u is upper
*  triangular (upper trapezoidal if m < n).
*
*  this is based on the right-looking level 3 blas lapack version
*  of the algorithm.
*
*  note: not currently fully tested for n .ne. m
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
*  nb      (input) integer
*          the block size for the blocked algorithm.  nb > 0.
*
*  a       (input/output) double precision array, dimension (lda,localn)
*          on entry, this node's portion of m by n matrix to be factored.
*          on exit, this node's factors l and u; the unit diagonal 
*          elements of l are not stored.
*
*  lda     (input) integer
*          the leading dimension of the array a that holds the local
*          portion of the matrix to be factored.  lda >= max(1,localm).
*
*  work    double precision array, dimension ldwork
*          work array.
*
*  ldwork  (input) integer
*          size of work array work.  ldwork >= (localm + localn + nb )*nb
*
*  ipiv    (output) integer array, dimension (min(m,n))
*          the pivot indices.  row i of the matrix was interchanged with
*          row ipiv(i) if ipiv(i) is a row that resides on this node.
*          otherwise -ipiv(i) equals the row index of the node that
*          holds the pivot row.
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
*     this version dated 08/18/92
*     r. van de geijn
*
*     all rights reserved
*
*     .. parameters ..
      double precision   one
      parameter          ( one = 1.0d+0 )
*     ..
*     .. local scalars ..
*
*     ..
*     .. misc. ..
      integer            i, iinfo, j, jb
*
*     nprow          row dimension of node grid
*     npcol          column dimension of node grid
*     myrow          my row index
*     mycol          my column index
*
      integer           nprow, npcol, myrow, mycol
      integer           localm, localn
      integer           icurrow, icurcol, iwork2
      integer           ii, jj, idummy
*     ..
*     .. external subroutines ..
      external           dgemm, dlaswp, pdlur, dtrsm, xerbla
*
      integer            itype_row, itype_col
*     ..
*     .. intrinsic functions ..
      intrinsic          max, min
*     ..
      double precision   timecpy, time, dclock, timemm, timelur
      integer            testing
      common    /timings/  timecpy, timemm, timelur
      common    /testing/ testing 
*     .. executable statements ..
*
*     test the input parameters.
*
      info = 0

*
*     get machine parameters
*
      call plamch2( nprow, npcol, myrow, mycol )
*
*     determine local number of rows and columns
*
      call imypart( 1, m, nb, idummy, localm, myrow, nprow )
      call imypart( 1, n, nb, idummy, localn, mycol, npcol )
      
*
*     check input parameters
*
      if( m.lt.0 ) then
         info = -1
      else if( n.lt.0 ) then
         info = -2
      else if( lda.lt.max( 1, localm ) ) then
         info = -5
      else if( nb.le.0 ) then
         info = -3
      else if (ldwork .lt. (localm+nb+localn)*nb ) then
         info =-7
      end if
      if( info.ne.0 ) then
         call xerbla( 'pdlubr ', -info )
         return
      end if

      iwork2 = nb*(localm+nb)+1
*     
      icurrow = 0
      icurcol = 0

      ii = 1
      jj = 1
      do 20 j = 1, min( m, n ), nb

         if (testing .eq. 1 .and.
     $        myrow .eq. 0 .and. mycol .eq. 0) then
            print *, "j = ", j
         endif

         jb = min( min( m, n )-j+1, nb )
*
*           factor diagonal and subdiagonal blocks and test for exact
*           singularity.
*
         if (mycol .eq. icurcol) then
            time = dclock()
            call pdlur( m-j+1, jb, a( ii, jj ), lda, ipiv( j ), 
     $           icurrow, work, nb, nb, 
     $           iinfo )
            timelur = timelur + dclock() - time 
         endif
*
*           adjust info and the pivot indices.
*
         if( info.eq.0 .and. iinfo.gt.0 )
     $        info = iinfo + j - 1
         do 10 i = j, j + jb - 1
            if (ipiv(i) .gt. 0) 
     $           ipiv( i ) = ii - 1 + ipiv( i )
 10      continue
*
*     broadcast pivot info
*
         call igebc2d( 'ring', 'row', jb, 1, ipiv(j), jb,            
     $        myrow, icurcol, itype_row())
*           apply interchanges to columns outside this block.
*
         if (mycol .ne. icurcol) then
            call pdlaswp( localn, a, lda, j, j+jb-1, ii, 
     $           icurrow, ipiv, 1 )
         else
            if (jj .gt. 1) then
               call pdlaswp( jj-1, a, lda, j, j+jb-1, ii, 
     $              icurrow, ipiv, 1 )
            endif
            if (jj+jb .le. localn) then
               call pdlaswp( localn-jj-jb+1, a(1,jj+jb), lda, 
     $              j, j+jb-1, ii, icurrow, ipiv, 1)
            endif
         endif
*
         if (j+jb .le. n) then
*
*     broadcast factored panel
*
            if (myrow .eq. icurrow) ii = ii + jb
            if (mycol .eq. icurcol) then
*
*     copy this node's part of factored panel into work array
*
               time = dclock()
               call dlacpy( 'g', localm-ii+1, jb, a( ii, jj ),
     $              lda, work( nb*nb+1 ), localm-ii+1 )
               timecpy = timecpy + dclock() - time 
            endif
*     
*     notice: for some rows, sending too much
*
c            call dgebc2d ('ring', 'row', localm-ii+1+nb, jb, 
            call dgebcs ('ring', 'row', localm-ii+1+nb, jb, 
     $           work, localm-ii+1+nb, myrow, icurcol, 
     $           itype_row() )
            if (myrow .eq. icurrow) ii = ii-jb
         endif
*            
         if (mycol .eq. icurcol) jj = jj+jb
*
         if (jj .le. localn) then
*     
*     update horizontal panel
*
            if ( myrow .eq. icurrow) then
               call dtrsm( 'left', 'lower', 'no transpose', 
     $              'unit', jb, localn-jj+1, one, work, nb,
     $              a( ii, jj ), lda )
               time = dclock()
               call dlacpy( 'g', jb, localn-jj+1, a( ii, jj ),
     $              lda, work(iwork2), jb )
               timecpy = timecpy + dclock() - time
c              call dgebc2d ( 'ctree', 'column', jb, localn-jj+1, 
               call dgebcs ( 'ctree', 'column', jb, localn-jj+1, 
     $              work(iwork2), jb, icurrow, mycol,
     $              itype_col() )
               ii = ii+jb
            else
c               call dgebc2d ( 'ctree', 'column', jb, localn-jj+1, 
               call dgebcs ( 'ctree', 'column', jb, localn-jj+1, 
     $              work(iwork2), jb, icurrow, mycol,
     $              itype_col() )
            endif
*
            if( ii .le. localm ) then
*
*                 update trailing submatrix.
*
               time = dclock()

               call dgemm( 'no transpose', 'no transpose', 
     $              localm-ii+1, localn-jj+1, jb, -one, 
     $              work(nb*nb+1), 
     $              localm-ii+1, work(iwork2), jb, 
     $              one, a(ii, jj),
     $              lda )

               timemm = timemm + dclock() - time
            end if
         elseif (myrow .eq. icurrow) then
            ii = ii+jb
         end if
         icurrow = mod( icurrow+1, nprow )
         icurcol = mod( icurcol+1, npcol )
 20      continue
      return
*
*     end of pdlubr
*
      end
