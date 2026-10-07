      subroutine pdutsv( diag, n, nb, a, lda, b, work) 
      integer           n, nb, lda
      character*1       diag
      double precision  a( lda, *), b( * ), work( * )
*
*     parallel double precision upper triangular solve
*
* purpose
* =======
*
* pdutsv computes the solution of u x = b 
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
*     to be done: plug in ring broadcast instead of sd/rc pairs
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
*     nbdim          number of blocks
*
      integer           i, istart, ifin, jstart, isize, jb, mynrow,
     $                  idummy, nbdim, ii, j
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
      nbdim = n/nb
      if (nbdim*nb .ne. n) nbdim = nbdim+1
      icurrow = mod( nbdim-1, nprow )
      icurcol = mod( nbdim-1, npcol )
* 
*     mynrow = number of rows assigned to this node
*
      call imypart( 1, n, nb, i, mynrow, myrow, nprow )
*
*     start of main loop
*
      do 100 i = (nbdim-1)*nb+1, 1, -nb
         if (icurcol .eq. mycol) then
            if (i .lt. (nbdim-1)*nb+1) then
*
*              compute where this node's part of the subvector starts
*              and ends
*
               call imypart( max( 1, i-(npcol-2)*nb), min( i+nb-1, n ),
     $              nb, istart, ifin, myrow, nprow )
               isize = ifin-istart+1
*
*              receive update information and add to local information
*
               call dgerv2d( isize, 1, work, isize,
     $              itype_from( myrow, jeast ))
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

               call dtrsv ( 'uppertriangular', 'notranspose', diag,
     $              jb, a( istart, jstart ), lda, b( istart ), 1 )
*
*              pass result around ring
*
               if (nprow .gt. 1 .and. i-nb .ge. 1) then
                  call dgesd2d( jb, 1, b( istart ), jb, inorth, 
     $                 mycol, itype_to( inorth, mycol ) )
               endif
               call dcopy ( jb, b( istart ), 1, work, 1 )
            else
               if (i-nb .ge. 1) then
                  call dgerv2d( jb, 1, work, jb, 
     $                 itype_from( isouth, mycol ))

                  if (inorth .ne. icurrow) then
                     call dgesd2d( jb, 1, work, jb, inorth, mycol,
     $                    itype_to( inorth, mycol ) )
                  endif
               endif
            endif
            if (i-nb .ge. 1) then
*
*              update subvector
*
               call imypart( max( 1, i-(npcol-1)*nb ) , i-1,
     $              nb, istart, ifin, myrow, nprow )
               isize = ifin-istart+1

               call imypart( i, i, nb, jstart, idummy, mycol, npcol )

               call dgemv ( 'notranspose', isize, jb, -one,
     $              a( istart, jstart ), lda, work, 1, one, 
     $              b( istart ), 1 )

*
*              pass subvector to left
*
               call dgesd2d( isize, 1, b(istart), isize, myrow, 
     $              jwest, itype_to( myrow, jwest ) )
*
*              update rest of local result            
*
               if (istart-1 .gt. 0) 
     $              call dgemv ( 'notranspose', istart-1, jb, -one,
     $              a( 1, jstart ), lda, work, 1, one, 
     $              b( 1 ), 1 )
            endif
         endif
         icurrow = mod( icurrow-1+nprow, nprow )
         icurcol = mod( icurcol-1+npcol, npcol )
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

