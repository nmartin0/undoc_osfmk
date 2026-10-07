      subroutine   cmprss   ( n     , trirow, tricol, trival, nnza  ,
     1                        a     , colidx, rowstr, x     , mark  ,
     2                        nzloc , outunt, nprocs, frstrw, lastrw,
     3                        frstcl, lastcl, totnnz )

c     ==================================================================
c     ==================================================================
c     ====  cmprss -- compress sparse matrix representation by      ====
c     ====            triples to (rowstr, colidx), assemblying      ====
c     ====            values for duplicated indices                 ====
c     ==================================================================
c     ==================================================================

c     ... generate a sparse matrix in (rowstr, colidx, value) format
c         from a list of [row, column, element] triples with possibly 
c         redundant index pairs
c
c     input:
c         n -- order of matrix
c         (trirow, tricol, trival) -- row, column, value representation
c
c     output:
c         nnza   -- number of distinct index pairs (nonzeros)
c         (rowstr, colidx, a) -- compressed representation
c
c     work:
c         nzloc, mark, x

c     ==================================================================

      integer           n     , nnza  , outunt, nprocs, frstrw, lastrw, 
     1                  frstcl, lastcl, totnnz

      integer           trirow (*), tricol (*), colidx (*),
     1                  rowstr (n+1), nzloc (n)

      logical           mark (n)

      double precision  a (*), trival (*), x (n)

c     ==================================================================

      integer           avgnnz, i     , j     , iaip1 , k     , maxnnz, 
     1                  minnnz, myid  , mynode, nza   , nzcol

      double precision  xj

c     ==================================================================

      myid = mynode ()

c     ---------------------------------------------
c     ...count the number of triples in each column
c     ---------------------------------------------

      do 100 j = 1, n
         rowstr (j) = 0
 100  continue
      rowstr (n+1) = 0
      
      do 200 nza = 1, nnza
         j          = tricol (nza)
         i          = trirow (nza)
         if  ( j .lt. frstcl  .or.  j .gt. lastcl  .or. 
     1         i .lt. frstrw  .or.  i .gt. lastrw  )  then
            write ( outunt, * ) myid, 'submatrix violation -- ',
     1               'i, j, frstrw, lastrw, frstcl, lastcl:',
     2                i, j, frstrw, lastrw, frstcl, lastcl  
         endif
         i          = i + 1
         rowstr (i) = rowstr (i) + 1
 200  continue
      
c     -------------------------------------------------------
c     ... set rowstr (i) to the location of the first nonzero
c         of row i  of a
c     -------------------------------------------------------
      
      rowstr (1) = 1
      do 300 i = 2, n+1
         rowstr (i) = rowstr (i) + rowstr (i-1)
 300  continue
      
c     ----------------------------------------------------
c     ... do a bucket sort of the triples on the row index
c     ----------------------------------------------------
      
      do 400 nza = 1, nnza
         i          = trirow (nza)
         k          = rowstr (i)
         a (k)      = trival (nza)
         colidx (k) = tricol (nza)
         rowstr (i) = rowstr (i) + 1
 400  continue
      
c     ---------------------------------------------------------
c     ... rowstr (i) now points to the first element of row i+1
c     ---------------------------------------------------------
      
      do 500 i = n, 1, -1
         rowstr (i+1) = rowstr (i)
 500  continue
      rowstr (1) = 1
      
c     ------------------------------------------------------
c     ... generate the actual output rows by adding elements
c     ------------------------------------------------------
      
      nza = 0
      do 600 j = 1, n
         x (j)    = 0.0
         mark (j) = .false.
 600  continue
      
      iaip1 = rowstr (1)
      do 1000 i = 1, n

         nzcol = 0
         
c        ------------------------------
c        ...loop over the ith row of  a
c        ------------------------------
         
         do 700 k = iaip1, rowstr (i+1)-1

            j     = colidx (k)
            x (j) = x (j) + a (k)

            if  ( ( .not. mark (j))  .and.  (x (j) .ne. 0.d0 ) )  then
               mark (j)      = .true.
               nzcol         = nzcol + 1
               nzloc (nzcol) = j
            endif

 700     continue
         
c        ---------------------------------------
c        ... extract the nonzeros of this row
c        ---------------------------------------
         
         do 800 k = 1, nzcol

            j        = nzloc (k)
            mark (j) = .false.
            xj       = x (j)
            x (j)    = 0.d0

            if  (xj .ne. 0.d0)  then
               nza          = nza + 1
               a (nza)      = xj
               colidx (nza) = j
            endif
 800     continue

         iaip1       = rowstr (i+1)
         rowstr (i+1) = nza + rowstr (1)

 1000 continue

      totnnz = nza
      call gisum ( totnnz, 1, k )

      minnnz = nza
      call gilow ( minnnz, 1, k) 

      maxnnz = nza
      call gihigh ( maxnnz, 1, k) 

      avgnnz = totnnz / nprocs
      if  ( myid .eq. 0 )  then
         write (outunt, 12000) totnnz, avgnnz, 
     1                         minnnz, (100.0 * minnnz) / avgnnz,
     2                         maxnnz, (100.0 * maxnnz) / avgnnz
      endif

      return

11000 format ( 'p', i3, '  rows ', i5, ':',  i5,  
     1                   '   columns ', i5, ':',  i5,
     2          '   nonzeros', i8 )

12000 format ( // 'partitioning of random sparse matrix:',
     1          / '    total number of nonzeros:', i8,
     2          / '       average per processor:', i8,
     3          / '       minimum per processor:', i8, f15.2 
     3          / '       maximum per processor:', i8, f15.2  )

      end
