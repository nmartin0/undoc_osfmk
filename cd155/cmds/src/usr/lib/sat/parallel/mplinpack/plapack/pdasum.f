      double precision function pdasum( dir, diag, n, nb, x ) 
*
*     .. scalar arguments ..
      character*1         dir, diag
      integer             n, nb
*     ..
*     .. array arguments ..
      double precision    x( * )
*
*  purpose
*  =======
*
*  pdasum computes the sum of the absolute values of the
*  elements of x, where x is a vector of length n distributed 
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
*     work           work space for dgsum
*     sum            holds current partial sum
*     i, ii          indices
*     icurrow,       indices of node that holds current 
*     icurcol        diagonal elements
*     myn            length of local portion of vector
*
      double precision  work, sum
      integer           i, j, ii, icurrow, icurcol, myn, idummy, 
     $                  info 
*     .. 
*     .. external functions ..
      double precision  dasum, dabs
      logical           lsame
*     ..
*     .. external subroutines ..
      external          plamch2
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
         call xerbla( 'pdasum', info )
         return
      endif

      if ( lsame( dir, 'c' ) ) then
         if ( .not.lsame( diag, 'd' ) ) then
*
*           determine length of local vector
*
            call imypart( 1, n, nb, idummy, myn, myrow, nprow )
*
*           compute one-norm of local part of vector
*
            sum = 0.0d00
            do 10 i=1,myn
               sum = sum + dabs( x(i) )
 10         continue
c            sum = dasum( myn, x, 1 )
*
*           add local contibution to one norms to those of
*           other rows of nodes
*
            call dgsum2d( 'column', 1, 1, sum, 1, work, 1,
     $           -1, -1 )

            pdasum = work
         else
            icurrow = 0
            icurcol = 0
            ii = 1
            sum = 0.0d00
            do 100 i=1,n,nb
               if ( icurrow .eq. myrow ) then
                  if (icurcol .eq. mycol ) then
                     do 90 j=1, min( nb, n-i+1 )
c                     sum = sum + dasum( min( nb, n-i+1), x(ii),
c     $                    1 )
                        sum = sum + dabs( x( ii+j-1 ) )
 90                  continue
                  endif
                  ii = ii+nb
               endif
               icurrow = mod( icurrow+1, nprow )
               icurcol = mod( icurcol+1, npcol )
 100        continue
*
*           add local contibution to one norms to those of
*           other nodes
*
            call dgsum2d( 'column', 1, 1, sum, 1, work, 1,
     $           -1, -1 )

            call dgsum2d( 'row', 1, 1, work, 1, sum, 1,
     $           -1, -1 )

            pdasum = sum
         endif
      else 
         if ( .not.lsame( diag, 'd' ) ) then
*
*           determine length of local vector
*
            call imypart( 1, n, nb, idummy, myn, mycol, npcol )
*
*           compute one-norm of local part of vector
*
            sum = 0.0d00
            do 110 i=1, myn
               sum = sum + dabs( x( i ) )
 110        continue

c            sum = dasum( myn, x, 1 )
*
*           add local contibution to one norms to those of
*           other columns of nodes
*
            call dgsum2d( 'row', 1, 1, sum, 1, work, 1,
     $           -1, -1 )

            pdasum = work
         else
            icurrow = 0
            icurcol = 0
            ii = 1
            sum = 0.0d00
            do 130 i=1,n,nb
               if ( icurcol .eq. mycol ) then
                  if (icurrow .eq. myrow ) then
c                     sum = sum + dasum( min( nb, n-i+1), x(ii), 
c     $                    1 )
                     do 120 j=1,min( nb, n-i+1 )
                        sum = sum + dabs( x( ii+j-1 ) )
 120                 continue
                  endif
                  ii = ii+nb
               endif
               icurrow = mod( icurrow+1, nprow )
               icurcol = mod( icurcol+1, npcol )
 130        continue
*
*           add local contibution to one norms to those of
*           other nodes
*
            call dgsum2d( 'column', 1, 1, sum, 1, work, 1,
     $           -1, -1 )

            call dgsum2d( 'row', 1, 1, work, 1, sum, 1,
     $           -1, -1 )

            pdasum = sum
         endif
      endif         
         
      return
*
*  end of pdasum
*
      end

       
      
