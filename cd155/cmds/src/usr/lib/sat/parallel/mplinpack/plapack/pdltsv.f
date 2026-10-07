      subroutine pdltsv( diag, n, nb, a, lda, b, work) 
      integer           n, nb, lda
      character*1       diag
      double precision  a( lda, *), b( * ), work( * )
*
* parallel lower triangular unit solve
*
* purpose
* =======
*
* pdltusv computes the solution of u x = b.
*     
* arguments
* =========
*    
* diag   (input) character*1
*        indicates whether on not unit diagonal 
*        ('unit' or 'nonunit')
*
* n      (input) integer
*        dimension of matrix u.
*
* nb     (input) integer
*        block size
*
* a      (input) double precision array, dimension (lda,n)
*        upper triangular portion of a contains the uppertriangular 
*        matrix.
*
* lda    (input) integer
*        the leading dimension of the array a.  lda >= max(1,n).
*
* b      (input/output) double precision array, dimension n
*        on entry, this node's contribution to the
*        right hand side of the linear system.
*        on exit, this node's contribution to the solution vector x.
*
* work   double precision array, dimension mynrow
*        array used to collect partial results
*
* info   (output) integer
*        = 0: successful exit
*        < 0: if info = -k, the k-th argument had an illegal value
*        > 0: if info = k, u(k,k) is exactly zero, no computation is
*             preformed.
*
* =======================================================================
*
*     this version dated 09/18/92
*     r. van de geijn
*
*     all rights reserved
*
*     .. parameters ..
      double precision  one, zero
      parameter         ( one = 1.0d+00, zero = 0.0d+00 )
*     ..
*     .. local scalars ..
*
*     nprow          row dimension of node grid
*     npcol          column dimension of node grid
*
*     myrow          my row index
*     mycol          my column index
*
*     icurrow        index of node row that holds current diagonal block
*     icurcol        index of node column that holds current diagonal block
*
      integer        nprow, npcol, myrow, mycol, icurrow, icurcol
*
*     i              loop index
*     istart, ifin   start and end of local part of subvector (row index)
*     jstart         start of local part of current matrix block (column index)
*     isize          size of local part of subvector
*     jb             size of current diagonal block
*     mynrow         number of rows assigned to this node
*     idummy         dummy parameter
*
      integer           i, istart, ifin, jstart, isize, jb, mynrow,
     $                  idummy, ii, j
*
*     neighbor indices
*
      integer           inorth, isouth, jwest, jeast
*     ..
*     .. intrinsic functions ..
*
      intrinsic         mod, min
*     ..
*     .. external functions ..
      integer           itype_from, itype_to
*     ..
*     .. external procedures ..
      external          daxpy, dcopy, plamch2
     $                  dgemv, dtrsv, imypart
*     ..
*     .. start of executable code ..

*     get machine parameters
*
      call plamch2( nprow, npcol, myrow, mycol )
*
*     get neighbor indices
*
      inorth = mod( myrow-1+nprow, nprow )
      isouth = mod( myrow+1, nprow )
      jwest = mod( mycol-1+npcol, npcol )
      jeast = mod( mycol+1, npcol )
*
*     initialize  indices of node holds current diagonal block
*
      icurrow = 0
      icurcol = 0
* 
*     mynrow = number of rows assigned to this node
*
      call imypart( 1, n, nb, i, mynrow, myrow, nprow )
*
*     start of main loop
*
      do 100 i = 1, n, nb
         if (icurcol .eq. mycol) then
            if (i .gt. 1) then
*
*              compute where this node's part of the subvector starts
*              and ends
*
               call imypart( i, min( i+(npcol-1)*nb-1, n ) , nb, 
     $              istart, ifin, myrow, nprow )
               isize = ifin-istart+1
*
*              receive update information and add to local information
*
               call dgerv2d( isize, 1, work, isize,
     $              itype_from( myrow, jwest ))
*
*              add to local result
*
               call daxpy ( isize, one, work, 1, b( istart ), 1 )
            endif
            
            jb = min( nb, n-i+1 )

            if (icurrow .eq. myrow) then
*
*              solve triangular system
*
               call imypart( i, min( i+nb-1, n) , nb, istart, ifin, 
     $              myrow, nprow )
               isize = ifin-istart+1

               call imypart( i, i, nb, jstart, idummy, mycol, npcol )

               call dtrsv ( 'lowertriangular', 'notranspose', diag,
     $              jb, a( istart, jstart ), lda, b( istart ), 1 )
*
*              pass result around ring
*
               if (nprow .gt. 1 .and. i+nb .le. n) then
                  call dgesd2d( jb, 1, b( istart ), jb, isouth, 
     $                 mycol, itype_to( isouth, mycol ) )
               endif
               call dcopy ( jb, b( istart ), 1, work, 1 )
            else
               if (i+nb .le. n) then
                  call dgerv2d( jb, 1, work, jb, 
     $                 itype_from( inorth, mycol ))

                  if (isouth .ne. icurrow) then
                     call dgesd2d( jb, 1, work, jb, isouth, mycol,
     $                    itype_to( isouth, mycol ) )
                  endif
               endif
            endif
            if (i+nb .le. n) then
*
*              update subvector
*
               call imypart( i+nb , min( i+nb*npcol-1, n) , nb, 
     $              istart, ifin, myrow, nprow )
               isize = ifin-istart+1

               call imypart( i, i, nb, jstart, idummy, mycol, npcol )

               call dgemv ( 'notranspose', isize, jb, -one,
     $              a( istart, jstart ), lda, work, 1, one, 
     $              b( istart ), 1 )

*
*              pass subvector to right
*
               call dgesd2d( isize, 1, b(istart), isize, myrow, 
     $              jeast, itype_to( myrow, jeast ) )
*
*              update rest of local result            
*
               call dgemv ( 'notranspose', mynrow-ifin, jb, -one,
     $              a( ifin+1, jstart ), lda, work, 1, one, 
     $              b( ifin+1 ), 1 )
            endif
         endif
         icurrow = mod( icurrow+1, nprow )
         icurcol = mod( icurcol+1, npcol )
 100  continue

*
*     zero all parts of x except those responding to the
*     diagonal elements of a
*
      icurrow = 0
      icurcol = 0
      ii = 1
      do 200 i=1, n, nb
         if (icurrow .eq. myrow) then
            if (icurcol .ne. mycol) then
               do 150 j=1, min(nb, n-i+1)
                  b(ii+j-1) = zero
 150           continue
            endif
            ii = ii+nb
         endif
         icurrow = mod( icurrow+1, nprow)
         icurcol = mod( icurcol+1, npcol)
 200  continue

      return
      end

