*
*******************************************************************************
*  This routine factors and backsolves a real, symmetric, near-dense matrix   *
*  by LDL factorization.  No pivoting; the matrix is diagonally dominant.     *
*                                                                             *
*  John Gustafson, Diane Rover, Stephen Elbert, and Michael Carter            *
*  Ames Laboratory, Ames, Iowa                                                *
*                                                                             *
*  Calls: Daxpy   Sets y = y + a * x for vectors x and y, scalar a            *
*         Dcopy   Moves vector to vector.                                     *
*         Dscal   Multiplies vector by scalar.                                *
*******************************************************************************
*
      SUBROUTINE Solver (coeff, scratch, ptemp, px, pxans, pxdiag,pxrhs,
     &                   py, info, non0, npatch, nx, ny)
*
*  Passed variables:
*    coeff   In/out; coefficients of system to solve.
*    ptemp   Vector, work area; used as plural in both x and y.
*    px      Vector, work area; plural x-subset.
*    pxans   Out; radiosities (R-G-B) from solved system; plural x-subset
*    pxdiag  In/scratch; diagonal of system (R-G-B); plural x-subset.
*    pxrhs   In; emissivities of faces (R-G-B); plural x-subset.
*    py      Vector, work area; plural y-subset.
*    info    In vector, useful quantities related to parallelization.
*    non0    Index of first non-zero element in coeff, for sparse methods.
*    npatch  In, total number of patches.
*    nx      In, size of local coeff subset in the x-direction.
*    ny      In, size of local coeff subset in the y-direction.
*
      REAL*8 coeff(0:*), ptemp(0:*), px(0:*), pxans(0:nx - 1, 0:2)
      real*8 scratch(0:*)
      REAL*8 pxdiag(0:nx - 1, 0:2), pxrhs(0:nx - 1, 0:2), py(0:*)
      INTEGER*4 info(0:15), non0, npatch, nx, ny
*
*  Local variables:
*    scale   Pivot element used to scale columns, and backsolve scalar.
*    i-n     General loop counters.
*    idim    Local submatrix dimension in the i (vertical) direction.
*    iloc    Vertical index into local submatrix for each pivot step.
*    istart  Index for start of loops.
*    ixj     Index of last element of coeff array.
*    ixproc  Position of this processor in the x-direction.
*    iyproc  Position of this processor in the y-direction.
*    jdim    Local submatrix dimension in the j (horizontal) direction.
*    lenx    Length of x-direction vectors.
*    leny    Length of y-direction vectors.
*    nete    Neighbor processor in the east direction.
*    netn    Neighbor processor in the north direction.
*    nets    Neighbor processor in the south direction.
*    netw    Neighbor processor in the west direction.
*    netew   Vector, network relative neighbors in the east-west direction
*    netns   Vector, network relative neighbors in the north-south direction
*    nskip   Stride for diagonal traversal of coeff array.
*    nxproc  Number of processors in ensemble in the x-direction.
*    nxtop   One less than nxproc; useful as a bit mask.
*    nyproc  Number of processors in ensemble in the y-direction.
*    nytop   One less than nyproc; useful as a bit mask.
*
      REAL*8 scale
      INTEGER*4 i, idim, iloc, imod, index, iproc
      INTEGER*4 istart, ixj, ixproc, iyproc, j, jdim, jloc, jmod
      INTEGER*4 jstart, k, kend, kloc, kmod, kblock, kstart, l
      INTEGER*4 len, lenx, leny, levx, levy, m, nblock, net, nete
      INTEGER*4 netn, nets, netw, netew(-31:31), netns(-31:31)
      INTEGER*4 nproc, nskip, ntran, nxproc, nxtop
      INTEGER*4 nyproc, nytop
      include 'fnx.h'
      integer node, itmp, messtype, jstartp1, leng_buf,
     &	 nxp1
      real*8 tim_i, tim_fact, tim_solve
      real*8 tmp1, tmp2, d_dot
*
*  Retrieve or construct quantities related to parallelization:
*
      iproc = info(0)
      nproc = info(2)
      nxproc = info(3)
      nyproc = info(4)
      nxtop = nxproc - 1
      nytop = nyproc - 1
      ixproc = info(5)
      iyproc = info(6)
      nete = info(7)
      netn = info(8)
      nets = info(9)
      netw = info(10)
      idim = info(11)
      jdim = info(12)
      ixj = info(13) - 1
      ntran = info(14)
      nskip = idim + 1
      levx = ixproc
      levy = nxproc * iyproc
      netns(0) = iproc
      do 601 i = 1, nytop
 	netns(i) = netns(i-1) + nxproc
 	if(netns(i) .ge. nproc) netns(i) = netns(i) - nproc
 601  continue
      do 701 i = -1, -nytop, -1
 	netns(i) = netns(i+1) - nxproc
 	if(netns(i) .lt. 0) netns(i) = netns(i) + nproc
 701  continue
      netew(0) = iproc
      do 602 i = 1, nxtop
 	netew(i) = netew(i-1) + 1
 	if(mod(netew(i),nxproc) .eq. 0) netew(i) = netew(i) -nxproc
 602  continue
      do 702 i = -1, -nxtop, -1
 	if(mod(netew(i+1),nxproc) .eq. 0) then
 	  netew(i) = netew(i+1) - 1 + nxproc
 	else
 	  netew(i) = netew(i+1) - 1
 	endif
 702  continue
*
*  Repeat solver for each of three frequencies (red, green, blue):
*
      tim_fact = 0.0
      tim_solve = 0.0
      DO 618 m = 0, 2
*
*  Load upper triangle of coefficients, diagonal, and right-hand side:
*
        tim_i = dclock()
        index = 0
        IF (ixproc .GE. iyproc)  index = 1
        iloc = index
        DO 604 j = ny - index, 1, -1
          DO 603 i = iloc, iloc + j - 1
            coeff(i) = coeff(ixj - i)
 	    itmp = i/idim + mod(i, idim) * idim
 	    scratch(itmp) = coeff(i)
 603      CONTINUE
          iloc = iloc + nskip
 604    CONTINUE
*
        IF (mod(npatch,nyproc) .EQ. iyproc) then
          CALL Dcopy (nx, pxrhs(0, m), 1, scratch(ny*idim), 1)
 	  nxp1 = nx + 1
 	else
 	  nxp1 = nx
 	endif
        IF (ixproc .EQ. iyproc) then
 	  do 650 i = 1, nxproc-1
 	    call csend(500, pxdiag(0, m), 8*nx, netns(i), 0)
 650	  continue
      	  call dcopy (nx, pxdiag(0, m), 1, coeff(0), nskip)
 	else
 	  call crecv(500, pxdiag(0, m), 8*nx)
      	  call dcopy (nx, pxdiag(0, m), 1, coeff(0), nskip)
 	endif
*
*  Factor matrix, writing factor on top of original matrix:
*
        jmod = mod(iyproc - non0 + 1, nyproc)
 	l = (non0 - 2 -iyproc + nyproc) / nyproc
        jstart = (non0 - 2 - ixproc + nxproc) / nxproc
      	call dsdiv (jstart, 1.0d0, coeff(0), nskip, coeff(0), nskip)
        IF (jmod .EQ. 0) THEN
 	  tmp1 = 0
 	  tmp2 = 0
 	  call dvmul(jstart, scratch(idim*l), 1, coeff(0),
     &		nskip, px, 1)
 	  do 506 i = 0, jstart-1
 	    tmp1 = tmp1 + scratch(idim*l+i) * px(i)
 506	  continue
 	  messtype = 0
          IF (ixproc .EQ. iyproc) then
 	    do 542 i = 1, nxproc-1
 	      call crecv(messtype, tmp2, 8)
 	      tmp1 = tmp1 +tmp2
 542	    continue
 	  else
 	    node = (nxproc+1) * (iproc / nxproc)
 	    call csend(messtype, tmp1, 8, node, 0)
 	  endif
          IF (ixproc .EQ. iyproc) then
 	    coeff(l*nskip) = coeff(l*nskip) - tmp1
 	    coeff(l*nskip) = 1.0d0 / coeff(l*nskip)
 	    px(jstart) = coeff(l*nskip)
 	    messtype = 575 + non0
 	    do 552 i = 1, nxproc-1
 	      call csend(messtype, px, 8*(jstart+1), netns(i), 0)
 552	    continue
          else
 	    messtype = 575 + non0
 	    do 554 i = 1, nxproc-1
 	      call csend(messtype, px, 8*jstart, netns(i), 0)
 554	    continue
 	  endif
 	  l = l + 1
 	endif
        DO 608 k = non0, npatch+1
          jmod = mod(iyproc - k + 1, nyproc)
          kmod = mod(iyproc - k, nyproc)
          jstart = (k - 2 - ixproc + nxproc) / nxproc
          jstartp1 = (k - 1 - ixproc + nxproc) / nxproc
          imod = mod(ixproc - k + 1,nxproc)
*
*
*
          IF (jmod .EQ. 0) THEN
 	    call dvmul(jstart, scratch(idim*(l-1)), 1, coeff(0),
     &		nskip, py, 1)
 	  elseif(imod .eq. 0) then
 	    messtype = 575 + k
 	    call crecv(messtype, py(0), (jstart+1)*8)
 	    coeff(jstart*nskip) = py(jstart)
 	  else
 	    messtype = 575 + k
 	    call crecv(messtype, py(0), jstart*8)
          END IF
*
*
*
          IF (kmod .EQ. 0) THEN
 	    call dinner(jstart, py, scratch(l*idim), d_dot)
 	    messtype = 10575 + k
 	    if(imod .eq. 0) then
 	      do 642 i = 1, nxproc-1
 	        call crecv(messtype, tmp2, 8)
 	        d_dot = d_dot + tmp2
 642	      continue
 	      scratch(l*idim + jstart) = scratch(l*idim + jstart) - d_dot
 	    else
 	      node = netew(-imod)
 	      call csend(messtype, d_dot, 8, node, 0)
 	    endif
 	    if(k .le. npatch) then
 	      tmp1 = 0
 	      tmp2 = 0
 	      call dvmul(jstartp1, scratch(idim*l), 1, coeff(0),
     &		nskip, px, 1)
 	      do 606 i = 0, jstartp1-1
 	        tmp1 = tmp1 + scratch(idim*l+i) * px(i)
 606	      continue
 	      messtype = 20576 + k
              IF (ixproc .EQ. iyproc) then
 		do 752 i = 1, nxproc-1
 		  call crecv(messtype, tmp2, 8)
 		  tmp1 = tmp1 + tmp2
 752		continue
 	        coeff(l*nskip) = coeff(l*nskip) - tmp1
 	        coeff(l*nskip) = 1.0d0 / coeff(l*nskip)
 	        px(jstartp1) = coeff(l*nskip)
 	        messtype = 576 + k
 	        do 652 i = 1, nxproc-1
 	          call csend(messtype, px, 8*(jstartp1+1), netns(i), 0)
 652	        continue
              else
 		node = (nxproc+1) * (iproc / nxproc)
 		call csend(messtype, tmp1, 8, node, 0)
 	        messtype = 576 + k
 	        do 654 i = 1, nxproc-1
 	          call csend(messtype, px, 8*jstartp1, netns(i), 0)
 654	        continue
 	      endif
 	      l = l + 1
 	    endif
          END IF
*
*  The bulk of SLALOM execution time is generally spent in the next loop:
*
          index = l * idim
          DO 607 j = l, nxp1
 	    call dinner(jstart, py, scratch(index), d_dot)
 	    px(j-l) = d_dot
            index = index + idim
 607      CONTINUE
 	  px(nxp1-l+1) = 0.0
 	  leng_buf = (npatch + 1 - k) / nxproc + 1
 	  messtype = 40000 + k
 	  if(imod .eq. 0) then
 	    do 727 j = 1, nxproc-1
 	      call crecv(messtype, py(0), 8*leng_buf)
 	      call daxpy(nxp1-l+1, 1.0d0, py(0), 1, px(0), 1)
 727	    continue
            index = l * idim + jstart
            DO 627 j = l, nxp1
 	      scratch(index) = scratch(index) - px(j-l)
 	      index = index + idim
 627	    continue
 	  else
 	    node = netew(-imod)
 	    call csend(messtype, px(0), 8*leng_buf, node, 0)
 	  endif
 608    CONTINUE
        tim_fact = dclock() - tim_i + tim_fact
 	if(iproc .eq. 0) write(6,*) 'factor done'
*
*  Final scaling of each element by its diagonal:
*
 	do 667 k = non0, npatch+1
          jmod = mod(iyproc - k + 1, nyproc)
          if (jmod .eq. 0) then
            l = (k - 2 - iyproc + nyproc) / nyproc
            jstart = (k - 2 - ixproc + nxproc) / nxproc
 	    index = l * idim
 	    do 657 j = 1, jstart
 	      scratch(index) = scratch(index) * coeff(nskip*(j-1))
 	      index = index + 1
 657	    continue
 	  endif
 667	continue
*
* Copy factored matrix back to coeff array
*
        index = 0
        IF (ixproc .GE. iyproc)  index = 1
        iloc = index
        DO 704 j = ny - index, 1, -1
          DO 703 i = iloc, iloc + j - 1
 	    itmp = i/idim + mod(i, idim) * idim
 	    coeff(i) = scratch(itmp)
 703      CONTINUE
          iloc = iloc + nskip
 704    CONTINUE
        IF (mod(npatch, nyproc) .EQ. iyproc)
     &    CALL Dcopy (nx, scratch(ny*idim), 1, coeff(ny), idim)
        tim_i = dclock()
*
*  Backsolve (L transpose). Owners of bottom coeff row copy it back to pxans
*  and send it to the diagonal processor:
*
        IF (mod(npatch, nyproc) .EQ. iyproc) THEN
          CALL Dcopy (nx, coeff(ny), idim, pxans(0, m), 1)
          net = mod(iproc,nxproc) * (nxproc + 1)
 	  call csend(50000+net, pxans(0,m), 8*nx, net, 0)
        END IF
        IF (ixproc .EQ. iyproc) THEN
          l = -1
 	  call crecv(50000+iproc, pxans(0,m), 8*nx)
        END IF
*
*  Clear a scratch vector:
*
        DO 609 j = 0, nx - 1
          px(j) = 0.
 609    CONTINUE
*
*  Loop based on global coeff index.  Tune the value of nblock to the multiple
*  of nxproc that gives the best performance for your system and problem size:
*
        nblock = nxproc * 8
        DO 617 kblock = ((npatch - 1) / nblock) * nblock, 0, -nblock
          kloc = kblock / nxproc
          kend = MIN(npatch, kblock + nblock) - 1
          lenx = (kend - kblock - ixproc + nxproc) / nxproc
          leny = (kend - kblock - iyproc + nyproc) / nyproc
          DO 611 k = kend, kblock, -1
            kmod = mod(k, nyproc)
*
*  Owner of scale element finishes computation of global k-th answer element
*  and sends it west to other processors in row:
*
            IF (kmod .EQ. iyproc) THEN
              IF (k .NE. kend)
     &          call crecv(70000+nets, px(kloc), 8*lenx)
              iloc = (k - iyproc + nytop) / nyproc
              jloc = (k - ixproc + nxtop) / nxproc
              IF (kmod .EQ. ixproc) THEN
 		tmp1 = pxans(jloc, m)
                scale = pxans(jloc, m) - px(jloc)
                pxans(jloc, m) = scale
                DO 610 j = -1, -nxtop, -1
 		  node = netew(j)
                  call csend(60000+node+k, scale, 8, node, 0)
 610            CONTINUE
              ELSE
                l = -1
                call crecv(60000+iproc+k, scale, 8)
              END IF
              py(iloc) = scale
*
*  Processors in row update their scratch vector using the scale value:
*
              CALL Daxpy (jloc - kloc, scale,
     &          coeff(iloc + kloc * idim), idim, px(kloc), 1)
              IF (k .NE. kblock)
     &          call csend(70000+iproc, px(kloc), 8*lenx, netn, 0)
            END IF
 611      CONTINUE
*
*  Use block of answer values to do an outer product with coeff vectors:
*
          DO 612 j = 0, kloc - 1
            ptemp(j) = 0.
 612      CONTINUE
          DO 613 i = kloc, kloc + leny - 1
            CALL Daxpy (kloc, py(i), coeff(i), idim, ptemp, 1)
 613      CONTINUE
          len = 8 * kloc
 	  messtype = 80000
 	  if (iyproc .eq. 0) then
 	    do 614 j = 1, nyproc - 1
 	      call crecv(messtype, py, len)
              DO 615 i = 0, kloc - 1
                ptemp(i) = ptemp(i) + py(i)
 615          CONTINUE
 614	    continue
 	    messtype = 90000
 	    node = iproc + nxproc
 	    do 714 j = 1, nyproc - 1
 	      call csend(messtype, ptemp, len, node, 0)
 	      node = node + nxproc
 714	    continue
 	  else
 	    node = netns(-iyproc)
 	    call csend(messtype, ptemp, len, node, 0)
 	    messtype = 90000
 	    call crecv(messtype, ptemp, len)
 	  endif
          DO 616 j = 0, kloc -1
            px(j) = px(j) +ptemp(j)
 616      CONTINUE
 617    CONTINUE
*
*  Next frequency (red, green, and blue for m = 0, 1, 2):
*
        tim_solve = tim_solve + dclock() - tim_i
 618  CONTINUE
      END
