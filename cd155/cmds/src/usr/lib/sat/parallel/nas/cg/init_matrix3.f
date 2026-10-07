      subroutine init_matrix( n, localn, a1, ixstart1, iyindex1 )

c
c     Purpose:
c     ========
c
c     Initialize Matrix
c
c     Parameters:
c     ===========
c
c     LOCALN       (Input) INTEGER
c                  length of local vector
c
c     A            (Output) DOUBLE PRECISION array
c                  Entries in the matrix
c
c     IXSTART      (Output) INTEGER array
c                  IXSTART(i) gives the index into array A
c                  and array IYINDEX where local row i starts
c
c     IYINDEX      (Output) INTEGER array
c                  IYINDEX(i) is the local column index of element
c                  A(i)
c
c     DISCUSSION
c     ==========
c
c     Given P = R x C, the logical dimensions of the processor
c     grid.  Processors are indexed by (MYROW,MYCOL), the 
c     row and column index into the grid.
c      
c     The vector of unknows, X is partitioned into 
c     P (= number of nodes) vectors, where the ith 
c     subvector, x_i, is of length nn_i and the subvectors
c     are assigned to the processor grid in column-major
c     order.  Note that localn = nn_(MYCOL*R+MYROW)
c      
c     Global matrix A is partitioned into RxC matrix of
c     blocks:
c                   /  A_00      ...  A_0(c-1)     \
c               A = |    :               :         |
c                   \  A_(r-1)0  ...  A_(r-1)(c-1) /   
c     and A_ij is assigned to node (i,j)
c
c     The algorithm for forming y = A x follows 
c     as described in
c       Lewis and van de Geijn, ""
c     Method 4.   This puts the following restrictions
c     on the dimensions of A_ij:
c
c     A submatrix must multiply a vector of length equal
c     to the sum of the subvectors owned by the 
c     processors in the column of nodes.  Hence,
c     if A_ij is MA x NA, then 
c        MA = nn_(j*NPROW) + ... + nn_((j+1)*NPROW-1)
c     Furthermore, the result vector y is partitioned
c     like x, and y_i is calculated so that is is
c     distributed in row-major order.  This means
c        NA = nn_(i*NPCOL) + ... + nn_((i+1)*NPCOL-1)
c
      integer             n, mxnz, nonzer, output, ilnwrk,
     $                    lenwrk, maxn, n1, nonzer1
      parameter           (n1 = 14000, mxnz = 130000,
c     parameter           (n1 = 14000, mxnz = 200000,
c       parameter           (n = 1400, mxnz = 150000,
     $                    nonzer1 = 11, output = 50,
c     $                    nonzer = 7, output = 50,
     $                    ilnwrk = 2*mxnz + 2*n1 +1,
     $                    lenwrk = mxnz + n1 + 1,
     $                    maxn = 20000)
      integer             ixstart(maxn), iyindex(mxnz)
      integer             nprocs, frstrw, lastrw
      integer             frstcl, lastcl, totnnz  
      integer             iwork(ilnwrk)
      double precision    rcond, a(mxnz), work(lenwrk)
      parameter           ( rcond = 1.0d-1 )
      double precision    tran, amult, zeta
      double precision    randlc
      logical             toobig
      integer             localn, j, i, ii, iproc, offset
      integer me, nprocs, nprows, npcols, myrow, mycol, pid
      common /gridinfo/ me, nprocs, nprows, npcols, myrow, 
     $     mycol, pid

      integer      nn( 513 ), ix( 33 ), iy( 33 ), nme( 513 ),
     $     nnn( 513), ixx(33), ixstart1(maxn), iyindex1(mxnz),
     $     len(maxn), msgid( 66 )

      double precision    a1(mxnz)  
      integer index_all( 513 ), ntot, index_row( 33 ),
     $     index_col( 33 ), n_row, n_col


      common /vectdist/ nn, ixx, ix, na, ma
      integer      mypid

      if (n .eq. 1400) then
         nonzer = 7
      else
         nonzer = 11
      endif

c
c     get global information on the vector distribution
c
      call gcol( localn, 4, nnn, nprocs*4, ntot )
      call gcol( me, 4, nme, nprocs*4, ntot )
c
c     permute
c
      do i=1,nprocs
         nn(nme(i)+1) = nnn(i)
      enddo

      if (mynode() .eq. 0) then
         do i=1,nprows
c            print *, nn(myrow+1), nn(myrow+nprows+1),
c     $           nn(myrow+2*nprows+1), 
c     $           nn(myrow+3*nprows+1)
         enddo
      endif

c
c     compute MA and NA
c
      ma = 0
      do i=0,nprows-1
         ma = ma + nn( mycol*nprows+i+1 )
      enddo

      na = 0
      do i=0,npcols-1
         na = na + nn( myrow*npcols+i+1 )
      enddo
c
c     consider the vector created by collecting the
c     subvectors of x in this column.  Let ix(i)
c     be the index into this vector where the subvector
c     provided by node (i-1,MYCOL) starts
c
      ix(1) = 1
      do i=1,nprows
         ix(i+1) = ix(i) + nn( mycol*nprows+i )
      enddo
c
c     consider the vector created by collecting the
c     subvectors of y in this row.  Let iy(i)
c     be the index into this vector where the subvector
c     provided by node (MYROW,i-1) starts
c
      iy(1) = 1
      do i=1,npcols
         iy(i+1) = iy(i) + nn( myrow*npcols+i )
      enddo
c
c     ifrow = global first row index of this node's 
c              matrix block
c            = 1 + nn_0 + ... + nn_(MYROW*NPCOLS-1)
c
c     ilrow = 1 + nn_0 + ... + nn_((MYROW+1)*NPCOLS-1)-1
c
c     ifcol = global first col index of this node's 
c              matrix block
c            = 1 + nn_0 + ... + nn_(MYCOL*NPROWS-1)
c
c     ilrow = 1 + nn_0 + ... + nn_((MYCOL+1)*NPROWS-1)-1
c
      ifrow = 1
      do i=1,myrow*npcols
         ifrow = ifrow + nn(i)
      enddo

      ilrow = ifrow-1
      do i=myrow*npcols+1, (myrow+1)*npcols
         ilrow = ilrow + nn(i)
      enddo

      ifcol = 1
      do i=1,mycol*nprows
         ifcol = ifcol + nn(i)
      enddo

      ilcol = ifcol-1
      do i=mycol*nprows+1, (mycol+1)*nprows
         ilcol = ilcol + nn(i)
      enddo

      tran    = 314159265.0d0
      amult   = 1220703125.0d0
      zeta    = randlc (tran, amult)

c      print *, me, "calling makea"

         call makea ( n               , mxnz           ,
     1                nonzer          , rcond          ,
     2                output          , a              ,
     3                ixstart         , iyindex        ,
     4                iwork (1)       , iwork (mxnz+1) ,
     5                work (1)        , work (mxnz+1)  ,
     6                iwork (2*mxnz+1), tran           ,
     7                amult           , nprocs         ,
     8                ifrow           , ilrow         ,
     9                ifcol           , ilcol         ,
     A                totnnz          , toobig           )


       ii = 1
       do i=ifrow, ilrow
          do j=ixstart(i), ixstart(i+1)-1
             iyindex(j) = iyindex(j) - ifcol + 1
c             print *, myrow, mycol, "a(j) == ", ii, j, a(j)
c             print *, myrow, mycol, "yindex(j) == ", j, iyindex(j)
          enddo
          ixstart(ii) = ixstart(i)
          ii = ii+1
       enddo
       ixstart(ii) = ixstart(ilrow+1)



      if (toobig .eq. .true.) then
         print *, "problem too big"
         stop
      endif

c
c   permute matrix for method 5
c
c      print *, "permuting matrix"

      ii = myrow*npcols
      do i=1, npcols
         idest = mod(ii,nprows)
         itemp = iy(i+1) - iy(i)
         call csend( ii+10000, itemp, 4, 
     $        iproc(idest, mycol), 
     $        mypid())

         do j=1, iy(i+1)-iy(i)
            len(j) = ixstart(iy(i)+j) - ixstart(iy(i)+j-1)
         enddo
         call csend( ii+20000, len, 4*(iy(i+1)-iy(i)), 
     $        iproc(idest, mycol), mypid() )

         ii = ii+1
      enddo

      ii = myrow
      ixx(1) = 1
      ixstart1(1) = 1
      do i=1, npcols
         call crecv( ii+10000, itemp, 4 )
         ixx( i+1 ) = ixx( i ) + itemp
         
         call crecv( ii+20000, len, 4*(ixx(i+1)-ixx(i)) )

         do j=1, ixx(i+1)-ixx(i)
            ixstart1( ixx(i) + j ) = ixstart1( ixx(i)+j-1 )
     $           + len(j) 
         enddo

         msgid(i) = irecv( ii+30000, a1( ixstart1(ixx(i))), 
     $        8* ( ixstart1(ixx(i+1)) - ixstart1(ixx(i))) )

         msgid(i+npcols) = 
     $        irecv( ii+40000, iyindex1( ixstart1(ixx(i))), 
     $        4* ( ixstart1(ixx(i+1)) - ixstart1(ixx(i))) )

         ii = ii+nprows
      enddo

      ii = myrow*npcols
      do i=1, npcols
         idest = mod(ii,nprows)

         call csend( ii+30000, a( ixstart(iy(i))), 
     $        8* ( ixstart(iy(i+1)) - ixstart(iy(i))),
     $        iproc(idest, mycol), mypid() )

         call csend( ii+40000, iyindex( ixstart(iy(i))), 
     $        4* ( ixstart(iy(i+1)) - ixstart(iy(i))),
     $        iproc(idest, mycol), mypid() )
         ii = ii+1
      enddo


      do i=1,npcols
         call msgwait(msgid(i))
         call msgwait(msgid(i+npcols))
      enddo

      ma = ixx( npcols+1 ) - 1

      return
      end


