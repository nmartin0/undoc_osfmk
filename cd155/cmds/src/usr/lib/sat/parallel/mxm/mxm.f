c     Matrix Times Matrix Multiplication
c     Version using 2D decomposition of arbitrary number of nodes

c     Original for Paragon : November 1993 - Bob Norin

      program mxmf

      implicit none
      include 'param.h'

      common, allocatable /memory/ buffer
      real*8 buffer(MAXMEM)

      integer nm(MAXPROC),nmbytes(MAXPROC),mindex(MAXPROC)
      integer nk(MAXPROC),nkbytes(MAXPROC),kindex(MAXPROC)
      integer nn(MAXPROC),nnbytes(MAXPROC),nindex(MAXPROC)
      integer imsg(MAXPROC)
      common /procbufs/ nm, nmbytes, mindex, nk, nkbytes, kindex,
     &   nn, nnbytes, nindex, imsg

      integer my_proc, num_proc, num_mproc, num_nproc,
     &   my_mapos, my_napos, my_mbpos, my_nbpos, my_mcpos, my_ncpos,
     &   my_right, my_down, my_down2
      common /idents/ my_proc, num_proc, num_mproc, num_nproc,
     &   my_mapos, my_napos, my_mbpos, my_nbpos, my_mcpos, my_ncpos,
     &   my_right, my_down, my_down2

      integer m, k, n, my_m, my_k, my_n
      common /sizes/ m, k, n, my_m, my_k, my_n

      integer my_m1, my_k1, my_n1
      common /indexes/ my_m1, my_k1, my_n1

      integer ptra, ptrb, ptrc, ptrw0
      common /ptrs/ ptra, ptrb, ptrc, ptrw0

      real*8 second, dummy, to0, to1, t1, t2, time_ohead, time_calc
      integer mynode, numnodes
      integer index(4), i, iset, mem_req, irep, itrial, icheck, nreps
      integer check_results, memory_req
      integer idebug
      parameter (idebug = 0)

      my_proc = mynode()
      num_proc = numnodes()
      if (my_proc .eq. 0) then
         write(istdo,900)' MXM -- Matrix Times Matrix Benchmark on ',
     &      num_proc,' Processors'
  900    format(A,i5,A)
      endif

      allocate (/memory/, stat=i)
c
c     establish the layout of processors
c
      call layout
      if (my_proc .eq. 0) then
         write(istdo,910)'Processor array: ',num_mproc,' rows by ',
     &      num_nproc,' columns'
  910    format(A,i4,A,i4,A)
      endif

      iset = 0

    1 continue

c     read and check the inputs
c     inputs - NREPS : number of times to repeat the matrix
c                      multiplication loop
c              matrix A is of dimension M*K
c              matrix B is of dimension K*N
c              matrix C is of dimension M*N

      iset = iset + 1

      if (my_proc .eq. 0) then
         write(istdo, 1000)
 1000    format('Please input the dimensions of NREPS, M, K, N '
     &          '(0 to quit)')
         read(istdi, *) index(1), index(2), index(3), index(4)
         if (num_proc .gt. 1) then
            call csend(100, index, 16, -1, 0)
         endif
      else
         call crecv(100, index, 16)
      endif

      nreps = index(1)
      m     = index(2)
      k     = index(3)
      n     = index(4)
      if (nreps .eq. 0) go to 9999
c
      if (nreps.lt.1 .or. nreps.gt.1000) then
         write(istdo,1010) nreps
 1010    format(' Value of NREPS = ',i10,' out of range')
         call exit(1)
         stop 'Bad NREPS'
      endif
c
      if (m .lt. num_mproc ) then
         write(istdo, 1002) num_mproc
 1002    format(' Value of M must be >= ',i4)
         call exit(1)
         stop 'Bad Dim'
      endif
c
      if (k .lt. num_nproc ) then
         write(istdo, 1003) num_nproc
 1003    format(' Value of K must be >= ',i4)
         call exit(1)
         stop 'Bad Dim'
      endif
c
      if (n .lt. num_nproc ) then
         write(istdo, 1004) num_nproc
 1004    format(' Value of N must be >= ',i4)
         call exit(1)
         stop 'Bad Dim'
      endif
c
c     set local dimensions and other node parameters
c
      call set_node_params
c
c     set memory pointers and return required memory
c
      mem_req = memory_req()
c
      if (my_proc .eq. 0) then
         write(istdo,930) ' Matrix Partition: A[',my_m,',',
     &      my_k,'] B[',my_k,',',my_n,'] C[',my_m,
     &      ',',my_n,'] '
  930    format(8(A,i4),A)
      endif
c
      if (my_proc .eq. 0) then
	 write(istdo,1018)'Memory required = ',mem_req*8,' bytes'
 1018    format(A,i10,A)
      endif
c
      if (mem_req .gt. MAXMEM) then
	 if (my_proc .eq. 0) then
            write(istdo,1020) MAXMEM*8
 1020       format('Insufficient memory,',i10,' bytes available')
	 endif
         call exit(1)
         stop 'Insufficient memory'
      endif
c
c     the repeat loop consisting of setting up of the matrix and
c     the actual matrix times matrix multiplication, done twice 
c
      call gsync()
      do irep = 1, nreps
       do itrial = 1, 2

         to0 = second(dummy)
c
c        generate input matrices A and B
c
         call set_inputdata( my_m, my_k, my_n, m, k, n)
         to1 = second(dummy)
         call gsync()
         t1 = second(dummy)
c
c        perform matrix times matrix multiply
c
         call do_mxm( my_m, my_k, my_n, m, k, n)
         t2 = second(dummy)
       enddo
      enddo

      call gsync()
c     checks if the resultant matrix contains the right values

      icheck = check_results( m, k, n)

c     collect and output timing statistics

      if (icheck .eq. 0) then
         time_ohead = to1 - to0
         time_calc = t2 - t1
         call collect_stats(time_ohead, time_calc, buffer(ptrw0))
      endif
      goto 1

 9999 continue
      deallocate (/memory/)

      end

c     ******************************************************************
c     set_inputdata sets the values of the local matrices.
c     The particular set of input data depends on the parameter DATASET.
c     If DATASET = 1:
c     matrix A of dim M*K has the reciprocal of its column times row
c     number as its values, matrix B of dim K*N has the row number 
c     times 1/2 the column number with alternating signs as its values.
c     If DATASET = 2:
c     A has zeros on diagonal, 1's above diagonal, -1's below diagonal,
c     B has repetative pattern of 1 -1 0 across a row with start first
c     element in next row shifted left one position.
c     ******************************************************************

      subroutine set_inputdata(im, ik, in, itm, itk, itn)
      implicit none
      integer im, ik, in, itm, itk, itn
      include 'param.h'
      integer idebug
      parameter (idebug = 0)

      common, allocatable /memory/ buffer
      real*8 buffer(MAXMEM)

      integer nm(MAXPROC),nmbytes(MAXPROC),mindex(MAXPROC)
      integer nk(MAXPROC),nkbytes(MAXPROC),kindex(MAXPROC)
      integer nn(MAXPROC),nnbytes(MAXPROC),nindex(MAXPROC)
      integer imsg(MAXPROC)
      common /procbufs/ nm, nmbytes, mindex, nk, nkbytes, kindex,
     &   nn, nnbytes, nindex, imsg

      integer my_proc, num_proc, num_mproc, num_nproc,
     &   my_mapos, my_napos, my_mbpos, my_nbpos, my_mcpos, my_ncpos,
     &   my_right, my_down, my_down2
      common /idents/ my_proc, num_proc, num_mproc, num_nproc,
     &   my_mapos, my_napos, my_mbpos, my_nbpos, my_mcpos, my_ncpos,
     &   my_right, my_down, my_down2

      integer m, k, n, my_m, my_k, my_n
      common /sizes/ m, k, n, my_m, my_k, my_n

      integer my_m1, my_k1, my_n1
      common /indexes/ my_m1, my_k1, my_n1

      integer ptra, ptrb, ptrc, ptrw0
      common /ptrs/ ptra, ptrb, ptrc, ptrw0

      real*8 rcipcol
      integer i, j, joff, ks, kk, ii, jt, it, ij3

      if (idebug .ne. 0) then
         write(*,933)'me=',my_proc,' m=',my_m,' m1=',my_m1,' k=',my_k,
     &      ' k1=',my_k1,' n=',my_n,' n1=',my_n1
         write(*,933)'me=',my_proc,' im=',im,' ik=',ik,' in=',in,
     &      ' in=',in,' mcpos=',my_mcpos,' ncpos=',my_ncpos,
     &      ' dwn=',my_down,' dwn2=',my_down2
         write(*,933)'me=',my_proc,' mapos=',my_mapos,' napos=',
     &      my_napos,' mbpos=',my_mbpos,' nbpos=',my_nbpos,' rht=',
     &      my_right
  933    format(10(A,i3))
      endif
      if (DATASET .eq. 1) then
c
c     Create data set 1
c
c
c     generate data for matrix A
c             1   1/2   1/3   1/4 ...    1/K
c             2   2/2   2/3   2/4 ...    2/K
c             .    .     .     .  ...     .
c             .    .     .     .  ...     .
c             M   M/2   M/3   M/4 ...    M/K
c
      do j = 1, ik
         rcipcol = 1.0d0 / dble(my_k1 + j - 1)
         joff = (j - 1) * im
         do i = 1, im
            buffer(ptra+i-1+joff) = dble(my_m1 + i - 1) * rcipcol
         enddo
      enddo
c
c     generate data for matrix B
c             1  -1   2  -2   3  -3  ...   -(N+1)/2  (N+1)/2
c             2  -2   4  -4   6  -6  ...  -2(N+1)/2 2(N+1)/2
c             .   .   .   .   .   .  ...      .        .
c             .   .   .   .   .   .  ...      .        .
c             K  -K  2K -2K  3K -3K  ...  -K(N+1)/2 K(N+1)/2
c
      if (mod(my_n1, 2) .eq. 1) then
         ks = -1
      else
         ks = 1
      endif
      do j = 1, in
         ks = -ks
         kk = (my_n1 + j - 1) * ks
         if (kk .gt. 0) then
            kk = (kk + 1) / 2
         else
            kk = (kk - 1) / 2
         endif
         joff = (j - 1) * ik
         do i = 1, ik
               buffer(ptrb+i-1+joff) = dble(kk * (my_k1 + i - 1))
         enddo
      enddo
c
c     clear results matrix C
c        expected results will be:
c             K   -K   2K  -2K   3K  -3K  ...  -K(N+1)/2  K(N+1)/2
c            2K  -2K   4K  -4K   6K  -6K  ... -2K(N+1)/2 2K(N+1)/2
c             .    .    .    .    .    .  ...      .        .
c             .    .    .    .    .    .  ...      .        .
c            MK  -MK  2MK -2MK  3MK -3MK  ... -MK(N+1)/2 MK(N+1)/2
c
      do j = 1, in
         joff = (j - 1) * im
         do i = 1, im
            buffer(ptrc+i-1+joff) = 0.0d0
         enddo
      enddo
c
c     end of DATASET 1
c
      else
      if (DATASET .eq. 2) then
c
c     Create data set 2
c
c     generate data for matrix A
c             0   1   1   1   1  ...
c            -1   0   1   1   1  ...
c            -1  -1   0   1   1  ...
c            -1  -1  -1   0   1  ...
c            -1  -1  -1  -1   0  ...
c             .   .   .   .   .  ...
c             .   .   .   .   .  ...
c
      do j = 1, ik
         joff = (j - 1) * im
         jt = my_k1 + j - 1
         do i = 1, im
            ii = ptra + i - 1 + joff
            it = my_m1 + i - 1
            if (it .eq. jt) then
               buffer(ii) = 0.0d0
            else
               if (it .lt. jt) then
                  buffer(ii) = 1.0d0
               else
                  buffer(ii) = -1.0d0
               endif
            endif
         enddo
      enddo
c
c     generate data for matrix B
c            -1   0   1  ...
c             0   1  -1  ...
c             1  -1   0  ...
c             .   .   .  ...
c             .   .   .  ...
c
      do j = 1, in
         joff = (j - 1) * ik
         jt = my_n1 + j - 1
         do i = 1, ik
            ii = ptrb + i - 1 + joff
            it = my_k1 + i - 1
            ij3 = mod(it+jt,3)
            if (ij3 .eq. 0) then
               buffer(ii) = 0.0d0
            else
               if (ij3 .eq. 1) then
                  buffer(ii) = 1.0d0
               else
                  buffer(ii) = -1.0d0
               endif
            endif
         enddo
      enddo
c
c     clear results matrix C
c        expected results will depend on K and will repeat in
c        3 x 3 blocks:
c        If K mod 3 = 0 then C will be
c             1   0  -1  ...
c             2  -1  -1  ...
c             1  -1   0  ...
c             .   .   .  ...
c             .   .   .  ...
c             0   0   0  ... for all M > K
c        If K mod 3 = 1 then C will be
c             0   0   0  ...
c             1  -1   0  ...
c             0  -1   1  ...
c             .   .   .  ...
c             .   .   .  ...
c             1   0  -1  ... for all M > K
c        If K mod 3 = 2 then C will be
c             0   1  -1  ...
c             1   0  -1  ...
c             0   0   0  ...
c             .   .   .  ...
c             .   .   .  ...
c             1  -1   0  ... for all M > K
c
      do j = 1, in
         joff = (j - 1) * im
         do i = 1, im
            buffer(ptrc+i-1+joff) = 0.0d0
         enddo
      enddo
c
c     end of DATASET 2
c
      endif
      endif
      if (idebug .ne. 0) then
         print *,'A gen: '
         do i = 1, im
            write(*,901)'me=',my_proc,' Ai=',i,' ',
     &         (buffer(ptra+i-1+(j-1)*im),j=1,ik)
  901       format(2(A,i3),A,10f7.2)
         enddo
         print *,'B gen:'
         do i = 1, ik
            write(*,901)'me=',my_proc,' Bi=',i,' ',
     &         (buffer(ptrb+i-1+(j-1)*ik),j=1,in)
         enddo
      endif
c
      end


c     ******************************************************************
c     do_mxm performs matrix times matrix multiplication using 
c     the dgemm(BLAS) math routine. 
c     ******************************************************************

      subroutine do_mxm(im, ik, in, itm, itk, itn)
      implicit none
      integer im, ik, in, itm, itk, itn
      include 'param.h'

      common, allocatable /memory/ buffer
      real*8 buffer(MAXMEM)

      integer nm(MAXPROC),nmbytes(MAXPROC),mindex(MAXPROC)
      integer nk(MAXPROC),nkbytes(MAXPROC),kindex(MAXPROC)
      integer nn(MAXPROC),nnbytes(MAXPROC),nindex(MAXPROC)
      integer imsg(MAXPROC)
      common /procbufs/ nm, nmbytes, mindex, nk, nkbytes, kindex,
     &   nn, nnbytes, nindex, imsg

      integer my_proc, num_proc, num_mproc, num_nproc,
     &   my_mapos, my_napos, my_mbpos, my_nbpos, my_mcpos, my_ncpos,
     &   my_right, my_down, my_down2
      common /idents/ my_proc, num_proc, num_mproc, num_nproc,
     &   my_mapos, my_napos, my_mbpos, my_nbpos, my_mcpos, my_ncpos,
     &   my_right, my_down, my_down2

      integer m, k, n, my_m, my_k, my_n
      common /sizes/ m, k, n, my_m, my_k, my_n

      integer my_m1, my_k1, my_n1
      common /indexes/ my_m1, my_k1, my_n1

      integer ptra, ptrb, ptrc, ptrw0
      common /ptrs/ ptra, ptrb, ptrc, ptrw0

      integer irot, naspos, narpos, kx, lenas, lenar, mbspos, mbrpos
      integer i, lenbs, lenbr, iptra, iptrb, iptrw, itemp
      integer hsendid, hrecvid, vsendid, vsendid2, vrecvid
      integer irecv, isend

c
c     matrix product will be computed by rotating a submatrix of A
c     and a submatrix of B into the destination C processor where the
c     matrix multiply will be performed and summed into the result using
c     dgemm
c
c     initialize pointers and submatrix position and size parameters
c
      naspos = my_napos
      narpos = naspos - 1
      if (narpos .lt. 0) narpos = num_nproc - 1
      kx = nk(naspos+1)
      lenas = my_m * kx * 8
      lenar = my_m * nk(narpos+1) * 8
c
      mbspos = my_mbpos
      mbrpos = mbspos - 1
      if (mbrpos .lt. 0) mbrpos = num_nproc - 1
      lenbs = my_n * kx * 8
      lenbr = my_n * nk(narpos+1) * 8
c
      iptra = ptra
      iptrb = ptrb
      iptrw = ptrw0
c
c     this is the main loop that performs a partial matrix multiply and
c     rotates the next portion of A and B into position
c
      do irot = 1, num_nproc - 1
c
c        rotate A -- overlaps with processing
c
         hsendid = isend(1000, buffer(iptra), lenas, my_right, 0)
         hrecvid = irecv(1000, buffer(iptrw), lenar)
c
c        do matrix multiply and accumulate
c
         call dgemm('n', 'n', my_m, my_n, kx, 1.0d0, buffer(iptra),
     &      my_m, buffer(iptrb), kx, 1.0d0, buffer(ptrc), my_m)
c
c        wait for A to be sent and received
c
         call msgwait(hrecvid)
         call msgwait(hsendid)
c
c        switch A pointers and update parameters for next pass
c
         itemp = iptra
         iptra = iptrw
         iptrw = itemp
c
         lenas = lenar
         naspos = narpos
         narpos = narpos - 1
         if (narpos .lt. 0) narpos = num_nproc - 1
         kx = nk(naspos+1)
         lenar = my_m * nk(narpos+1) * 8
c
c        rotate B -- each processor will receive one buffer, but if
c        num_mproc not an even multiple of num_nproc, then last
c        processor in column will not send a buffer, and the num_mproc
c        th processor in column will send its buffer twice, once to
c        the next processor down the column, and once to the first
c        processor in the column
c
         if (my_down .ge. 0) then
            vsendid = isend(2000, buffer(iptrb), lenbs, my_down, 0)
         endif
         if (my_down2 .ge. 0) then
            vsendid2 = isend(2000, buffer(iptrb), lenbs, my_down2, 0)
         endif
c
         vrecvid = irecv(2000, buffer(iptrw), lenbr)
c
c        wait for B to be sent and received
c
         call msgwait(vrecvid)
         if (my_down .ge. 0) call msgwait(vsendid)
         if (my_down2 .ge. 0) call msgwait(vsendid2)
c
c        switch B pointers and update parameters for next pass
c
         itemp = iptrb
         iptrb = iptrw
         iptrw = itemp
c
         lenbs = lenbr
         mbspos = mbrpos
         mbrpos = mbrpos - 1
         if (mbrpos .lt. 0) mbrpos = num_nproc - 1
         lenbr = my_n * nk(narpos+1) * 8
      enddo
c
c     do the last multiply and accumulate pass
c
      call dgemm('n', 'n', my_m, my_n, kx, 1.0d0, buffer(iptra),
     &   my_m, buffer(iptrb), kx, 1.0d0, buffer(ptrc), my_m)

      return
      end


c     ******************************************************************
c     function checks the results in the product matrix and returns
c     the number of errors
c     ******************************************************************


      integer function check_results( itm, itk, itn )
      implicit none
      integer itm, itk, itn
      include 'param.h'
      integer idebug
      parameter( idebug = 0 )

      common, allocatable /memory/ buffer
      real*8 buffer(MAXMEM)

      integer nm(MAXPROC),nmbytes(MAXPROC),mindex(MAXPROC)
      integer nk(MAXPROC),nkbytes(MAXPROC),kindex(MAXPROC)
      integer nn(MAXPROC),nnbytes(MAXPROC),nindex(MAXPROC)
      integer imsg(MAXPROC)
      common /procbufs/ nm, nmbytes, mindex, nk, nkbytes, kindex,
     &   nn, nnbytes, nindex, imsg

      integer my_proc, num_proc, num_mproc, num_nproc,
     &   my_mapos, my_napos, my_mbpos, my_nbpos, my_mcpos, my_ncpos,
     &   my_right, my_down, my_down2
      common /idents/ my_proc, num_proc, num_mproc, num_nproc,
     &   my_mapos, my_napos, my_mbpos, my_nbpos, my_mcpos, my_ncpos,
     &   my_right, my_down, my_down2

      integer m, k, n, my_m, my_k, my_n
      common /sizes/ m, k, n, my_m, my_k, my_n

      integer my_m1, my_k1, my_n1
      common /indexes/ my_m1, my_k1, my_n1

      integer ptra, ptrb, ptrc, ptrw0
      common /ptrs/ ptra, ptrb, ptrc, ptrw0

      real*8 errmax, actval, expval, diff, rdiffsq, diff_sqrd, exp_sqrd
      integer i, j, ks, kk, jt, it, joff, i3p1, j3p1, k3p1, nerr
      
c
c     expected results for data set 2
c
      real*8 eval(4,3,3)
      data eval 
     & /0.0, 1.0, 0.0, 1.0, 0.0, -1.0, -1.0, 0.0, 0.0, 0.0, 1.0, -1.0,
     &  0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 0.0, -1.0, -1.0, -1.0, 0.0, 0.0,
     &  1.0, 2.0, 1.0, 0.0, 0.0, -1.0, -1.0, 0.0, -1.0, -1.0, 0.0, 0.0/

      errmax = 1.0e-5
      errmax = errmax*errmax
      nerr = 0

      if (DATASET .eq. 1) then
c
c     check data set 1
c
c     result matrix values should be equal to K * row number *
c         ((-1)**(column number + 1)) * ((column number + 1) / 2)
c
      if (mod(my_n1, 2) .eq. 1) then
         ks = -1
      else
         ks = 1
      endif
      do j = 1, my_n
         jt = my_n1 + j - 1
         ks = -ks
         kk = jt * ks
         if (kk .gt. 0) then
            kk = (kk + 1) / 2
         else
            kk = (kk - 1) / 2
         endif
         joff = ptrc - 1 + (j - 1) * my_m
         do i = 1, my_m
            it = my_m1 + i - 1
            expval = dble(it) * dble(itk) * dble(kk)
            actval = buffer(joff+i)
	    if (idebug .ne. 0) then
               write(istdo,900)'me=',my_proc,' it=',it,' jt=',jt,
     &            ' i=',i,' j=',j,' exp, act = ',expval,actval
	    endif
            diff = expval - actval
            diff_sqrd = diff * diff
            exp_sqrd = expval * expval
c Avoid dividing by 0
            if (exp_sqrd .ne. 0.0) then
              rdiffsq = diff_sqrd / exp_sqrd
            else if (diff_sqrd .ne. 0.0) then
              rdiffsq = errmax + 1.0
            else
              rdiffsq = 0.0
            endif

            if (rdiffsq .gt. errmax) then
               if (nerr .le. 5) then
                  write(istdo,900)'me=',my_proc,' it=',it,' jt=',jt,
     &               ' i=',i,' j=',j,' exp, act = ',expval,actval
  900             format(5(A,i4),A,2f8.2)
               endif
               nerr = nerr + 1
            endif
         enddo
      enddo
c
c     end of checking data set 1
c
      else
      if (DATASET .eq. 2) then
c
c     check data set 2
c
c     result matrix values repeat in 3 x 3 blocks, and depend on
c     the dimension K modulo 3:
c        If K mod 3 = 0 then C will be
c             1   0  -1  ...
c             2  -1  -1  ...
c             1  -1   0  ...
c             .   .   .  ...
c             .   .   .  ...
c             0   0   0  ... for all M > K
c        If K mod 3 = 1 then C will be
c             0   0   0  ...
c             1  -1   0  ...
c             0  -1   1  ...
c             .   .   .  ...
c             .   .   .  ...
c             1   0  -1  ... for all M > K
c        If K mod 3 = 2 then C will be
c             0   1  -1  ...
c             1   0  -1  ...
c             0   0   0  ...
c             .   .   .  ...
c             .   .   .  ...
c             1  -1   0  ... for all M > K
c
c
      k3p1 = mod(itk,3)
      if (k3p1 .eq. 0) k3p1 = 3
      do j = 1, my_n
         jt = my_n1 + j - 1
         joff = ptrc - 1 + (j - 1) * my_m
         j3p1 = mod(jt,3) 
	 if (j3p1 .eq. 0) j3p1 = 3
         do i = 1, my_m
            it = my_m1 + i - 1
	    i3p1 = mod(it,3) 
	    if (i3p1 .eq. 0) i3p1 = 3
	    if (it .gt. itk) i3p1 = 4
	    expval = eval(i3p1,j3p1,k3p1)
            actval = buffer(joff+i)
	    if (idebug .ne. 0) then
               write(istdo,900)'me=',my_proc,' it=',it,' jt=',jt,
     &            ' i=',i,' j=',j,' exp, act = ',expval,actval
	    endif
            diff = expval - actval
            diff_sqrd = diff * diff
            exp_sqrd = expval * expval
c Avoid dividing by 0
            if (exp_sqrd .ne. 0.0) then
              rdiffsq = diff_sqrd / exp_sqrd
            else if (diff_sqrd .ne. 0.0) then
              rdiffsq = errmax + 1.0
            else
              rdiffsq = 0.0
            endif

            if (rdiffsq .gt. errmax) then
               if (nerr .le. 5) then
                  write(istdo,900)'me=',my_proc,' it=',it,' jt=',jt,
     &               ' i=',i,' j=',j,' exp, act = ',expval,actval
               endif
               nerr = nerr + 1
            endif
         enddo
      enddo
c
c     end of checking data set 2
c
      endif
      endif
      if (nerr .gt. 0) then
         write(istdo,910) my_proc, nerr
  910    format(' Processor ',i4,' reports ',i7,' ERRORS.',
     &      '  Timing and Mflop Results *INVALID*.')
      endif
      check_results = nerr

      return
      end

c     ******************************************************************
c     collects and writes timing statistics
c     ******************************************************************

      subroutine collect_stats (time_ohead, time_calc, work)
      implicit none
      include 'param.h'

      real*8 time_ohead, time_calc, work(*)

      integer nm(MAXPROC),nmbytes(MAXPROC),mindex(MAXPROC)
      integer nk(MAXPROC),nkbytes(MAXPROC),kindex(MAXPROC)
      integer nn(MAXPROC),nnbytes(MAXPROC),nindex(MAXPROC)
      integer imsg(MAXPROC)
      common /procbufs/ nm, nmbytes, mindex, nk, nkbytes, kindex,
     &   nn, nnbytes, nindex, imsg

      integer my_proc, num_proc, num_mproc, num_nproc,
     &   my_mapos, my_napos, my_mbpos, my_nbpos, my_mcpos, my_ncpos,
     &   my_right, my_down, my_down2
      common /idents/ my_proc, num_proc, num_mproc, num_nproc,
     &   my_mapos, my_napos, my_mbpos, my_nbpos, my_mcpos, my_ncpos,
     &   my_right, my_down, my_down2

      integer m, k, n, my_m, my_k, my_n
      common /sizes/ m, k, n, my_m, my_k, my_n

      integer my_m1, my_k1, my_n1
      common /indexes/ my_m1, my_k1, my_n1

      integer ptra, ptrb, ptrc, ptrw0
      common /ptrs/ ptra, ptrb, ptrc, ptrw0

      real*8 rnum_proc, tmax_ohead, tmin_ohead, tavg_ohead
      real*8 tmax_calc, tmin_calc, tavg_calc, xmops
      real*8 xmflopmax, xmflopmin, xmflopavg
      real*8 xpmflopmax, xpmflopmin, xpmflopavg

      rnum_proc = 1.0d0 / dble(num_proc)
c
c     collect statistics from all processors
c
         tmax_ohead = time_ohead
         tmin_ohead = time_ohead
         tavg_ohead = time_ohead
         call gdhigh(tmax_ohead,1,work)
         call gdlow(tmin_ohead,1,work)
         call gdsum(tavg_ohead,1,work)
         tavg_ohead = tavg_ohead * rnum_proc
         tmax_calc = time_calc
         tmin_calc = time_calc
         tavg_calc = time_calc
         call gdhigh(tmax_calc,1,work)
         call gdlow(tmin_calc,1,work)
         call gdsum(tavg_calc,1,work)
         tavg_calc = tavg_calc * rnum_proc

         xmops = n * ((2 * k - 1) / 1000.0d0) * (m / 1000.0d0)
         xmflopmin = xmops / tmax_calc
         xpmflopmin = xmflopmin * rnum_proc
         xmflopmax = xmops / tmin_calc
         xpmflopmax = xmflopmax * rnum_proc
         xmflopavg = xmops / tavg_calc
         xpmflopavg = xmflopavg * rnum_proc

         if (my_proc .eq. 0) then
            write(istdo,1030) m, k, n, tavg_ohead
 1030       format('MXM Setup for M,K,N =',3i6, e14.3, ' Sec')
            write(istdo,1045) tavg_calc, tmin_calc, tmax_calc
 1045       format(' Times(sec):  Avg=',e10.3,' Min=',e10.3,
     &         ' Max=',e10.3)
            write(istdo,1047) xmflopavg, xmflopmin, xmflopmax
 1047       format(' Mflops:      Avg=',f10.2,' Min=',f10.2,
     &         ' Max=',f10.2)
            write(istdo,1049) xpmflopavg, xpmflopmin, xpmflopmax
 1049       format(' Mflops/node: Avg=',f10.2,' Min=',f10.2,
     &         ' Max=',f10.2)
         endif

      return
      end

c     ******************************************************************
c     given a number of parameters, determines the layout of processors
c     ******************************************************************

      subroutine layout
      implicit none
      include 'param.h'

      integer nm(MAXPROC),nmbytes(MAXPROC),mindex(MAXPROC)
      integer nk(MAXPROC),nkbytes(MAXPROC),kindex(MAXPROC)
      integer nn(MAXPROC),nnbytes(MAXPROC),nindex(MAXPROC)
      integer imsg(MAXPROC)
      common /procbufs/ nm, nmbytes, mindex, nk, nkbytes, kindex,
     &   nn, nnbytes, nindex, imsg

      integer my_proc, num_proc, num_mproc, num_nproc,
     &   my_mapos, my_napos, my_mbpos, my_nbpos, my_mcpos, my_ncpos,
     &   my_right, my_down, my_down2
      common /idents/ my_proc, num_proc, num_mproc, num_nproc,
     &   my_mapos, my_napos, my_mbpos, my_nbpos, my_mcpos, my_ncpos,
     &   my_right, my_down, my_down2

      integer m, k, n, my_m, my_k, my_n
      common /sizes/ m, k, n, my_m, my_k, my_n

      integer my_m1, my_k1, my_n1
      common /indexes/ my_m1, my_k1, my_n1

      integer ptra, ptrb, ptrc, ptrw0
      common /ptrs/ ptra, ptrb, ptrc, ptrw0

      integer i, irregbrot
c
c     Make 2-d layout as square as possible
c
      i = sqrt(float(num_proc)+0.5)
      do while ((mod(num_proc,i)) .ne. 0)
         i = i - 1
      enddo
      num_nproc = i
      num_mproc = num_proc / num_nproc
c
c     Find position of this processor in C array
c
      my_ncpos = my_proc / num_mproc
      my_mcpos = my_proc - my_ncpos * num_mproc
c
c     Find starting position of this processor in A array
c
      my_mapos = my_mcpos
      my_napos = my_ncpos
      do i = 1, my_mapos
         my_napos = my_napos + 1
         if (my_napos .eq. num_nproc) my_napos = 0
      enddo
c
c     Find position of this processor in B array
c
      my_nbpos = my_ncpos
      my_mbpos = mod(my_mcpos,num_nproc)
      do i = 1, my_nbpos
         my_mbpos = my_mbpos + 1
         if (my_mbpos .eq. num_nproc) my_mbpos = 0
      enddo
c
c     Find processor to the right of this one in array
c
      my_right = my_proc + num_mproc
      if (my_right .ge. num_proc) then
         my_right = my_right - num_proc
      endif
c
c     Find processor below this one in array
c
      my_down = my_proc + 1
      if (my_mcpos .eq. num_mproc - 1) then
         my_down = my_down - num_mproc
      endif
c
c     Determine if num_mproc and even multiple of num_nproc;
c       if so, then rotation of B matrix is regular,
c       if not, then processors where mod(my_proc+1,num_mproc) = num_nproc
c       must send array twice, to first processor in column as well as next
c       one down, and last processor in column does not pass its data on.
c
      irregbrot = mod(num_mproc,num_nproc)
      my_down2 = -1
      if (irregbrot .gt. 0) then
c
c        Set extra destination for one processor in each column
c
         if (mod(my_proc+1,num_mproc) .eq. num_nproc) then
            my_down2 = my_nbpos * num_mproc
         endif
c
c        Set flag so that last processor in column doesn't send B data
c
         if (mod(my_proc+1,num_mproc) .eq. 0) then
            my_down = -1
         endif
      endif

      return
      end

c     ******************************************************************
c     sets the dimensions and parameters associated with each node
c     ******************************************************************

      subroutine set_node_params
      implicit none
      include 'param.h'

      integer nm(MAXPROC),nmbytes(MAXPROC),mindex(MAXPROC)
      integer nk(MAXPROC),nkbytes(MAXPROC),kindex(MAXPROC)
      integer nn(MAXPROC),nnbytes(MAXPROC),nindex(MAXPROC)
      integer imsg(MAXPROC)
      common /procbufs/ nm, nmbytes, mindex, nk, nkbytes, kindex,
     &   nn, nnbytes, nindex, imsg

      integer my_proc, num_proc, num_mproc, num_nproc,
     &   my_mapos, my_napos, my_mbpos, my_nbpos, my_mcpos, my_ncpos,
     &   my_right, my_down, my_down2
      common /idents/ my_proc, num_proc, num_mproc, num_nproc,
     &   my_mapos, my_napos, my_mbpos, my_nbpos, my_mcpos, my_ncpos,
     &   my_right, my_down, my_down2

      integer m, k, n, my_m, my_k, my_n
      common /sizes/ m, k, n, my_m, my_k, my_n

      integer my_m1, my_k1, my_n1
      common /indexes/ my_m1, my_k1, my_n1

      integer ptra, ptrb, ptrc, ptrw0
      common /ptrs/ ptra, ptrb, ptrc, ptrw0
 
      integer i, mrem, krem, nrem, isum
c
c     get local dimensions of the decomposed matrices
c
      my_m = m / num_mproc
      mrem = m - my_m * num_mproc
      my_k = k / num_nproc
      krem = k - my_k * num_nproc
      my_n = n / num_nproc
      nrem = n - my_n * num_nproc
c
c     set dimension M assigned to each processor; if not even, allocate
c     the remaining r = mod(M,num_mproc) indexes to the first r processors
c
      isum = 0
      do i = 1, num_mproc
         if (mrem .ge. i) then
            nm(i) = my_m + 1
         else
            nm(i) = my_m
         endif
         nmbytes(i) = nm(i)*8
         mindex(i) = isum + 1
         isum = isum + nm(i)
      enddo
c
c     get starting M index and portion of M for this processor
c
      my_m1 = mindex(my_mcpos+1)
      my_m  = nm(my_mcpos+1)
c
c     set dimension K assigned to each processor; 
c     if not even, allocate the remaining r = mod(K,num_nproc) 
c     indexes to the first r processors
c
      isum = 0
      do i = 1, num_nproc
         if (krem .ge. i) then
            nk(i) = my_k + 1
         else
            nk(i) = my_k
         endif
         nkbytes(i) = nk(i)*8
         kindex(i) = isum + 1
         isum = isum + nk(i)
      enddo
c
c     get starting K index and portion of K for this processor
c
      my_k1 = kindex(my_napos+1)
      my_k  = nk(my_napos+1)
c
c     set dimension N for matrix C assigned to each processor; 
c     if not even, allocate the remaining r = mod(N,num_nproc) 
c     indexes to the first r processors
c
      isum = 0
      do i = 1, num_nproc
         if (nrem .ge. i) then
            nn(i) = my_n + 1
         else
            nn(i) = my_n
         endif
         nnbytes(i) = nn(i)*8
         nindex(i) = isum + 1
         isum = isum + nn(i)
      enddo
c
c     get starting N index and portion of N for this processor
c
      my_n1 = nindex(my_ncpos+1)
      my_n  = nn(my_ncpos+1)

      return
      end

c     ******************************************************************
c     computes memory requirements and sets memory pointers
c     ******************************************************************

      integer function memory_req
      implicit none
      include 'param.h'

      integer nm(MAXPROC),nmbytes(MAXPROC),mindex(MAXPROC)
      integer nk(MAXPROC),nkbytes(MAXPROC),kindex(MAXPROC)
      integer nn(MAXPROC),nnbytes(MAXPROC),nindex(MAXPROC)
      integer imsg(MAXPROC)
      common /procbufs/ nm, nmbytes, mindex, nk, nkbytes, kindex,
     &   nn, nnbytes, nindex, imsg

      integer my_proc, num_proc, num_mproc, num_nproc,
     &   my_mapos, my_napos, my_mbpos, my_nbpos, my_mcpos, my_ncpos,
     &   my_right, my_down, my_down2
      common /idents/ my_proc, num_proc, num_mproc, num_nproc,
     &   my_mapos, my_napos, my_mbpos, my_nbpos, my_mcpos, my_ncpos,
     &   my_right, my_down, my_down2

      integer m, k, n, my_m, my_k, my_n
      common /sizes/ m, k, n, my_m, my_k, my_n

      integer my_m1, my_k1, my_n1
      common /indexes/ my_m1, my_k1, my_n1

      integer ptra, ptrb, ptrc, ptrw0
      common /ptrs/ ptra, ptrb, ptrc, ptrw0

      integer maxwk, mem_a, mem_b, mem_c, mem_w0
c
c     compute memory requirements for each array, 
c     allow for quad alignment 
c
      maxwk = nm(1) * nk(1)
      if (nn(1) .gt. nm(1)) maxwk = nn(1) * nk(1)
      mem_a  = ((maxwk + 1) / 2) * 2
      mem_b  = ((maxwk + 1) / 2) * 2
      mem_c  = ((nm(1) * nn(1) + 1) / 2) * 2
      mem_w0 = ((maxwk + 1) / 2) * 2
c
c     get total memory needed
c
      memory_req = mem_a + mem_b + mem_c + mem_w0 
c
c     get pointers to the memory allocated to each of the local
c     matrices
c
      ptra  = 1
      ptrb  = ptra  + mem_a
      ptrc  = ptrb  + mem_b
      ptrw0 = ptrc  + mem_c

      return
      end

