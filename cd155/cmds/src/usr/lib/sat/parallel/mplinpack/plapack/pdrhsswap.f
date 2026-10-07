      subroutine  pdrhsswap( n, nb, x, ipiv )
*
*     parallel vector permutation routine
*
*     .. scalar arguments
      integer               n, nb
*     ..
*     .. array arguments ..
      double precision      x( * )
      integer               ipiv( * )
*     ..
*
*  purpose
*  =======
*
*  pdrhsswap permutes vector x according to the pivot information
*  in ipiv to correspond to the row interchanges performed by
*  pdlubr/pdlur.
*
*  arguments
*  =========
*
*  n        (input) integer
*           length of vector x
*
*  nb       (input) integer
*           the block size used for the parallel data distribution
*
*  x        (input/output) double precision array of length (myn)
*           local portion of the vector.  permuted upon completion
*           of the routine
*
*  ipiv     (input) integer array, dimension (n)
*           the pivot indices.  row i of the matrix was interchanged with
*          row ipiv(i) if ipiv(i) is a row that resides on this node.
*          otherwise -ipiv(i) equals the row index of the node that
*          holds the pivot row.
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
*
*     misc
*
      integer          icurrow, ii, i
*     ..
*     .. external subroutines ..
      external         pdlaswp
*     ..
*     .. external functions ..
*     ..
*     .. intrinsic functions ..
      intrinsic mod
*     ..
*     .. executable statements ..
*
*     get machine parameters
*
      call plamch2(nprow, npcol, myrow, mycol)
*
*     swap one block at a time
*
      icurrow = 0
      ii = 1
      do 100 i=1, n, nb
         call pdlaswp( 1, x, n, i, min(i+nb-1, n), ii, 
     $        icurrow, ipiv, 1 )
         if (icurrow .eq. myrow) ii = ii+nb
         icurrow = mod( icurrow+1, nprow )
 100  continue
      
      return
*
*     end of pdrhsswap
*
      end

