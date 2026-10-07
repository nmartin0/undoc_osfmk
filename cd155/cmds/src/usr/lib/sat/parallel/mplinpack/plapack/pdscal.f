      subroutine pdscal( dir, diag, n, nb, alpha, x ) 
*
*     .. scalar arguments ..
      character*1         dir, diag
      integer             n, nb
      double precision    alpha
*     ..
*     .. array arguments ..
      double precision    x( * )
*
*  purpose
*  =======
*
*  pdscal scales the elements of vector x by alpha, 
*  where x is a vector of length n distributed 
*  as indicated by dir and diag, using blocksize nb for 
*  wrapping.
*
*  arguments
*  =========
*
*
*  dir     (input) character*1
*          direction in which vector is originally distributed
*
*  diag    (input) character*1
*          indicates whether the vector is originally distributed
*          among the "diagonal" nodes 
*
*  n       (input) integer
*          length of vector
*
*  nb      (input) integer
*          the block size used for wrapping
*
*  alpha   (input) double precision
*          scaling factor
*
*  x       (input/output) double precision array of dimension 
*          ( myn or mym )
*          holds local portion of vector
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
* 
*     i, ii          indices
*     icurrow,       indices of node that holds current 
*     icurcol        diagonal elements
*     myn            length of local portion of vector
*
      integer           i, ii, icurrow, icurcol, myn, idummy, 
     $                  info 
*     .. 
*     .. external functions ..
      logical           lsame
*     ..
*     .. external subroutines ..
      external          plamch2, dscal
*     .. 
*     .. intrinsic functions ..
      intrinsic         min
*
*     get machine parameters
*
      call plamch2( nprow, npcol, myrow, mycol )
*      
*     check input parameters
*
      info = 0
      if     ( .not.lsame( dir , 'c' ).and.
     $         .not.lsame( dir , 'r' )      )then
         info = 1
      endif
      if (info .ne. 0) then
         call xerbla( 'pdscal', info )
         return
      endif

      if ( lsame( dir, 'c' ) ) then
         if ( .not.lsame( diag, 'd' ) ) then
*
*           determine length of local vector
*
            call imypart( 1, n, nb, idummy, myn, myrow, nprow )
*
*           scale local part
*
            call dscal( myn, alpha, x, 1 )
         else
            icurrow = 0
            icurcol = 0
            ii = 1
            do 100 i=1,n,nb
               if ( icurrow .eq. myrow ) then
                  if (icurcol .eq. mycol ) then
                     call dscal( min( nb, n-i+1), alpha, x(ii), 
     $                    1 )
                  endif
                  ii = ii+nb
               endif
               icurrow = mod( icurrow+1, nprow )
               icurcol = mod( icurcol+1, npcol )
 100        continue
         endif
      else 
         if ( .not.lsame( diag, 'd' ) ) then
*
*           determine length of local vector
*
            call imypart( 1, n, nb, idummy, myn, mycol, npcol )
*
*           scale local part
*
            call dscal( myn, alpha, x, 1 )
         else
            icurrow = 0
            icurcol = 0
            ii = 1
            do 110 i=1,n,nb
               if ( icurcol .eq. mycol ) then
                  if (icurrow .eq. myrow ) then
                     call dscal( min( nb, n-i+1 ), alpha, x(ii), 
     $                    1 )
                  endif
                  ii = ii+nb
               endif
               icurrow = mod( icurrow+1, nprow )
               icurcol = mod( icurcol+1, npcol )
 110        continue
         endif
      endif         
         
      return
      end

       
      
