      subroutine pdvredistr( dir1, diag1, k1, n, nb, x,
     $     dir2, diag2, k2, work )
*
*     .. scalar arguments ..
      character*1            dir1, dir2, diag1, diag2
      integer                k1, n, nb, k2
*     ..
*     .. array arguments ..
      double precision       x( * ), work( * )
*
*  purpose
*  =======
*
*  pdvredistr redistributes a vector among the logical grid
*  of nodes
*  arguments
*  =========
*
*  dir1    (input) character*1
*          direction in which vector is originally distributed
*
*  diag1   (input) character*1
*          indicates whether the vector is originally distributed
*          among the "diagonal" nodes 
*
*  k1      (input) integer
*          if diag1 = 'r' then k1 indicates which row or column
*          originally holds the vector
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
*  dir2    (input) character*1
*          direction in which vector is finally distributed
*
*  diag2   (input) character*1
*          indicates whether the vector is finally  distributed
*          among the "diagonal" nodes 
*
*  k2      (input) integer
*          if diag2 = 'r' then k2 indicates which row or column
*          finally holds the vector
*
*  work    double precision array of dimension ( myn or mym )
*          work array
*
*  ============================================================
*
*     this version dated 09/18/92
*     r. van de geijn
*
*     ..
*     .. local scalars ..
*
*     nprow          row dimension of node grid
*     npcol          column dimension of node grid
*     myrow          my row index
*     mycol          my column index
*
      integer           nprow, npcol, myrow, mycol
      integer           icurrow, icurcol
      integer           ii, i, j, isize, jj, k, myn, idummy
*     ..
*     .. external functions ..
      integer           itype_from, itype_to
      logical           lsame
*     ..
*     .. intrinsic functions ..
      intrinsic         min, mod
*     ..
*     .. executable statements ..

      call plamch2(nprow, npcol, myrow, mycol)
      
      if ( lsame( dir1, dir2 ) .and. 
     $     lsame( diag1, diag2 ) ) then
*
*     idir1  = idir2 
*     idiag1 = idiag2
*
*     do nothing
*
         return
      else if ( lsame( dir1, dir2 ) ) then
         print *, "dir1=dir2=c and diag1 != diag2 not 
     $        yet implemented"
         stop
      else if ( lsame( dir1, 'c' ) .and.
     $        lsame( dir2, 'r' ) .and.
     $        lsame( diag1, 'd' ) .and.
     $        .not.lsame( diag2, 'd' ) ) then
         call imypart( 1, n, nb, idummy, myn, mycol, npcol )
*     
*     send result to row of nodes that holds first 
*     row of matrix, distributing the result like
*     the first row.
*     
         
*     
*     pack local portion.  inefficient, but it works.
*     
         icurrow = 0
         icurcol = 0
         ii = 1
         jj = 1
         isize = 0
         do 100 i=1, n, nb
            if (icurrow .eq. myrow .and. icurcol .eq. mycol) then
               call dcopy( min( nb, n-i+1 ), x(jj), 1, work(ii), 1 )
               ii = ii+nb
               isize = isize+min( nb, n-i+1 )
            endif
            if (icurrow .eq. myrow) jj = jj+nb
            icurrow = mod( icurrow+1, nprow)
            icurcol = mod( icurcol+1, npcol)
 100     continue
         
         if (myrow .eq. k2) then
            jj = myrow
            do 120 i=0,nprow-1
               if (jj .ne. myrow) then
                  call igerv2d(1, 1, isize, 1, 
     $                 itype_from( jj, mycol ) )
                  call dgerv2d( isize, 1, work, isize, 
     $                 itype_from( jj, mycol ) )
               endif
               ii = nb*mycol
               icurrow = mod( mycol, nprow )
               k = 1
               do 110 j=1, myn, nb
                  if (icurrow .eq. jj) then
                     call dcopy( min( nb, myn-j+1), work(k), 1, 
     $                    x(j), 1 )
                     k = k+nb
                  endif
                  icurrow = mod( icurrow + npcol, nprow )
 110           continue
               jj = mod( jj+1, nprow )
 120        continue
         else
            call igesd2d( 1, 1, isize, 1, k2, mycol, 
     $           itype_to( k2, mycol ) )
            call dgesd2d( isize, 1, work, isize, k2, mycol, 
     $           itype_to( k2, mycol ) )
            do 130 i=1, myn
               x(i) = (0.0d0, 0.0d0)
 130        continue
         endif
      else if ( lsame( dir1, 'c' ) .and.
     $        lsame( dir2, 'r' ) )  then
         print *, "dir1=c, dir2=r, diag2=d or diag1=c not 
     $        yet implemented"
         stop
      else if ( lsame( dir1, 'r' ) .and. 
     $        lsame( dir2, 'c' ) .and.
     $        lsame( diag1, 'd' ) .and.
     $        .not.lsame( diag2, 'd' ) ) then
         call imypart( 1, n, nb, idummy, myn, myrow, nprow )
*     
*     send result to row of nodes that holds first 
*     column of matrix, distributing the result like
*     the first column.
*     
         
*     
*     pack local portion.  inefficient, but it works.
*     
         icurrow = 0
         icurcol = 0
         ii = 1
         jj = 1
         isize = 0
         do 140 i=1, n, nb
            if (icurrow .eq. myrow .and. icurcol .eq. mycol) then
               call dcopy( min( nb, n-i+1 ), x(jj), 1, work(ii), 1 )
               ii = ii+nb
               isize = isize+min( nb, n-i+1 )
            endif
            if (icurcol .eq. mycol) jj = jj+nb
            icurrow = mod( icurrow+1, nprow)
            icurcol = mod( icurcol+1, npcol)
 140     continue
         
         if (mycol .eq. k2) then
            jj = mycol
            do 160 i=0,npcol-1
               if (jj .ne. mycol) then
                  call igerv2d(1, 1, isize, 1, 
     $                 itype_from( myrow, jj ) )
                  call dgerv2d( isize, 1, work, isize, 
     $                 itype_from( myrow, jj ) )
               endif
               ii = nb*myrow
               icurcol = mod( myrow, npcol )
               k = 1
               do 150 j=1, myn, nb
                  if (icurcol .eq. jj) then
                     call dcopy( min( nb, myn-j+1), work(k), 1, 
     $                    x(j), 1 )
                     k = k+nb
                  endif
                  icurcol = mod( icurcol + nprow, npcol )
 150           continue
               jj = mod( jj+1, npcol )
 160        continue
         else
            call igesd2d( 1, 1, isize, 1, myrow, k2,
     $           itype_to( myrow, k2 ) )
            call dgesd2d( isize, 1, work, isize, myrow, k2,
     $           itype_to( myrow, k2 ) )
            do 170 i=1, myn
               x(i) = (0.0d0, 0.0d0)
 170        continue
         endif
      else
         print *, "pdvredist: default not implemented"
      endif
      
      return
*     
*  end of pdvredistr
*
      end
