      double precision function pdmnrm1( m, n, nb, a, lda, work ) 
*
*     .. scalar arguments ..
      integer             m, n, lda, nb
*     ..
*     .. array arguments ..
      double precision    a( lda, * )
      double precision    work( * )
*
*  purpose
*  =======
*
*  pdmnrm1 computes the one norm of the matrix whose local
*  portion is in array a.
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
*  a       (input) double precision array, dimension (lda,localn)
*          this node's portion of m by n matrix.
*
*  lda     (input) integer
*          the leading dimension of the array a that holds the local
*          portion of the matrix to be factored.  lda >= max(1,localm).
*
*  work    double precision array, dimension ( myn )
*          work array.
*
*  =====================================================================
*
*     this version dated 09/18/92
*     r. van de geijn
*
*     .. parameters ..
*     ..
*     .. local scalars ..
*
*     nprow          row dimension of node grid
*     npcol          column dimension of node grid
*     myrow          my row index
*     mycol          my column index
*
      integer           nprow, npcol, myrow, mycol
      double precision  xmax 
* 
*     mym, myn          dimensions of local portion of matrix
*
      integer           mym, myn, idummy, i, j, localmax, isrc, jsrc
*     .. 
*     .. external functions ..
      double precision  dabs
      integer           idamax
*     ..
*     .. external subroutines ..
      external          plamch2
*     .. 
*     .. intrinsic functions ..
      intrinsic         min, abs
*
*     get machine parameters
*
      call plamch2( nprow, npcol, myrow, mycol )
*
*     determine local number of rows and columns
*
      call imypart( 1, m, nb, idummy, mym, myrow, nprow )
      call imypart( 1, n, nb, idummy, myn, mycol, npcol )
*
*     compute one-norm of local part of local columns
*
      do 100 j=1, myn
         work(j) = 0.00d00
         do 90 i=1,mym
            work(j) = work(j) + dabs( a(i,j) )
 90      continue
 100  continue
*
*     add local contibutions to one norms to those of
*     other rows of nodes
*
      call dgsum2d( 'column', 1, myn, work, 1, work(myn+1), 1,
     $     -1, -1 )
*
*     determine local maximum of the one norms
*
      localmax = idamax( myn, work( myn+1 ), 1 )
*
*     determine global maximum of the one norms
*
      xmax = work( localmax+myn )
c      call dgmax( 'row', 1, 1, xmax, 1, work, 1, -1, -1 )
      call dgamax2d( 'row', 1, 1, xmax, 1, work, 1, isrc, 1, jsrc, 1,
     $     -1, -1 )
      pdmnrm1 = work( 1 )

      return
      end

       
      
