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
      INTEGER*4 Gray, i, idim, iloc, imod, index, iproc
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
c     write(6,*) 'begin',iproc,ixproc,iyproc,nete,
c    &	netn,nets,netw,idim,jdim,ixj,ntran
      levx = ixproc
      levy = nxproc * iyproc
      DO 601 i = -nytop, nytop
        netns(i) = levx .OR. ((iyproc + i .AND. nytop) * nxproc)
 601  CONTINUE
      DO 602 j = -nxtop, nxtop
        netew(j) = levy .OR. ((ixproc + j .AND. nxtop))
 602  CONTINUE
c     write(6,*) 'ns',iproc,(netns(i),i=-nytop,nytop)
c     write(6,*) 'ew',iproc,(netew(i),i=-nxtop,nxtop)
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
c	    if(iproc.eq.0.or.iproc.eq.8) write(6,*) 'copy',iproc,j,i,
c    &		itmp,coeff(i),ny
 603      CONTINUE
          iloc = iloc + nskip
 604    CONTINUE
*
        IF ((npatch .AND. nytop) .EQ. iyproc) then
          CALL Dcopy (nx, pxrhs(0, m), 1, scratch(ny*idim), 1)
c	  write(6,*) 'rhs',m,iproc,(scratch(ny*idim+i),i=0,nx-1)
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
        jmod = (iyproc - non0 + 1) .AND. nytop
 	l = (non0 - 2 -iyproc + nyproc) / nyproc
        jstart = (non0 - 2 - ixproc + nxproc) / nxproc
      	call dsdiv (jstart, 1.0d0, coeff(0), nskip, coeff(0), nskip)
c	write(6,*) 'jmod',iproc,jmod, l, jstart
        IF (jmod .EQ. 0) THEN
 	  tmp1 = 0
 	  tmp2 = 0
c	  write(6,*) 'start',iproc,scratch(idim*l),coeff(0),
c    &	    coeff((jstart-1)*nskip),scratch(idim*l+jstart-1),iproc
 	  call dvmul(jstart, scratch(idim*l), 1, coeff(0),
     &		nskip, px, 1)
 	  do 506 i = 0, jstart-1
 	    tmp1 = tmp1 + scratch(idim*l+i) * px(i)
 506	  continue
c	  write(6,*) 'tmp1 = ',tmp1
 	  messtype = 0
          IF (ixproc .EQ. iyproc) then
 	    do 542 i = 1, nxproc-1
 	      call crecv(messtype, tmp2, 8)
 	      tmp1 = tmp1 +tmp2
 542	    continue
c	  write(6,*) 'tmp1 tot = ',tmp1
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
c	  call gsync()
          jmod = (iyproc - k + 1) .AND. nytop
          kmod = (iyproc - k) .AND. nytop
          jstart = (k - 2 - ixproc + nxproc) / nxproc
          jstartp1 = (k - 1 - ixproc + nxproc) / nxproc
          imod = (ixproc - k + 1) .AND. nxtop
c	  write(6,*) 'me = ',iproc,k,jmod,kmod,imod,
c    &		l,jstart,jstartp1,itmp,nxproc
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
c	    if(k.ge.25) write(6,*) 'bef 1st',jstart,py(0),scratch(l*idim),
c    &		py(jstart-1),scratch(l*idim+jstart-1)
 	    messtype = 10575 + k
 	    if(imod .eq. 0) then
 	      do 642 i = 1, nxproc-1
 	        call crecv(messtype, tmp2, 8)
c		if(k.ge.25) write(6,*) 'do642',iproc,d_dot,tmp2
 	        d_dot = d_dot + tmp2
 642	      continue
 	      scratch(l*idim + jstart) = scratch(l*idim + jstart) - d_dot
c	      write(6,*) 'first din', iproc, d_dot, scratch(l*idim+jstart)
 	    else
 	      node = netew(-imod)
 	      call csend(messtype, d_dot, 8, node, 0)
 	    endif
 	    if(k .le. npatch) then
 	      tmp1 = 0
 	      tmp2 = 0
 	      call dvmul(jstartp1, scratch(idim*l), 1, coeff(0),
     &		nskip, px, 1)
c	      write(6,*) 'start do608',iproc,scratch(idim*l),px(0),
c    &	      coeff(l*nskip),scratch(idim*l+jstartp1-1),px(jstartp1-1),
c    &		jstartp1
 	      do 606 i = 0, jstartp1-1
 	        tmp1 = tmp1 + scratch(idim*l+i) * px(i)
 606	      continue
c	     if(k.ge.26) write(6,*) 'tmp',k,iproc,px(0),px(jstartp1-1),
c    &	       scratch(idim*l),scratch(idim*l+jstartp1-1),idim*l,jstartp1
 	      messtype = 20576 + k
              IF (ixproc .EQ. iyproc) then
 		do 752 i = 1, nxproc-1
 		  call crecv(messtype, tmp2, 8)
 		  tmp1 = tmp1 + tmp2
 752		continue
c	        write(6,*) 'bef diag',iproc, coeff(l*nskip),tmp1,tmp2
 	        coeff(l*nskip) = coeff(l*nskip) - tmp1
 	        coeff(l*nskip) = 1.0d0 / coeff(l*nskip)
 	        px(jstartp1) = coeff(l*nskip)
c	        write(6,*) 'diag',iproc,k,px(jstartp1),px(0),
c    &		  scratch(idim*l)
 	        messtype = 576 + k
 	        do 652 i = 1, nxproc-1
 	          call csend(messtype, px, 8*(jstartp1+1), netns(i), 0)
 652	        continue
              else
 		node = (nxproc+1) * (iproc / nxproc)
c	        write(6,*) 'ddot for diag',iproc,node,tmp1
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
c	    if(k.ge.15.and.j.eq.l) write(6,*) 'debug',iproc,jstart,d_dot,
c    &		py(0),scratch(index),py(jstart-1),scratch(index+jstart-1)
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
c	      if(k.ge.15.and.j.eq.l) write(6,*) 'more debug',iproc,
c    &		scratch(index),px(0),index
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
          jmod = (iyproc - k + 1) .AND. nytop
          if (jmod .eq. 0) then
            l = (k - 2 - iyproc + nyproc) / nyproc
            jstart = (k - 2 - ixproc + nxproc) / nxproc
 	    index = l * idim
 	    do 657 j = 1, jstart
c	      if(k.eq.npatch+1) write(6,*) 'debug',j,scratch(index),
c    &		coeff(nskip*(j-1))
 	      scratch(index) = scratch(index) * coeff(nskip*(j-1))
c	      if(m.eq.0) write(6,*) 'fact',iproc,scratch(index),index
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
c	    if(iproc.eq.4.or.iproc.eq.5.or.iproc.eq.6.or.iproc.eq.7) 
c    &		write(6,*) 'copy',iproc,j,i,itmp,coeff(i),ny
 703      CONTINUE
          iloc = iloc + nskip
 704    CONTINUE
        IF ((npatch .AND. nytop) .EQ. iyproc)
     &    CALL Dcopy (nx, scratch(ny*idim), 1, coeff(ny), idim)
c       IF ((npatch .AND. nytop) .EQ. iyproc) then
c     	  if(m.eq.0) write(6,*) 'forw',iproc,(coeff(j),j=ny,
c    &	  nx*idim,idim)
c	endif
 	tim_i = dclock()
*
*  Backsolve (L transpose). Owners of bottom coeff row copy it back to pxans
*  and send it to the diagonal processor:
*
        IF ((npatch .AND. nytop) .EQ. iyproc) THEN
          CALL Dcopy (nx, coeff(ny), idim, pxans(0, m), 1)
          net = (iproc .AND. nxtop) * (nxproc + 1)
 	  call csend(50000+net, pxans(0,m), 8*nx, net, 0)
        END IF
        IF (ixproc .EQ. iyproc) THEN
          l = -1
 	  call crecv(50000+iproc, pxans(0,m), 8*nx)
c	  write(6,*) '2004',iproc,nx,pxans(0,m)
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
            kmod = k .AND. nytop
*
*  Owner of scale element finishes computation of global k-th answer element
*  and sends it west to other processors in row:
*
            IF (kmod .EQ. iyproc) THEN
              IF (k .NE. kend)
     &          call crecv(70000+nets, px(kloc), 8*lenx)
c             IF (k .NE. kend)
c    &          write(6,*) '2006',iproc,lenx,nets,px(kloc)
              iloc = (k - iyproc + nytop) / nyproc
              jloc = (k - ixproc + nxtop) / nxproc
              IF (kmod .EQ. ixproc) THEN
 		tmp1 = pxans(jloc, m)
                scale = pxans(jloc, m) - px(jloc)
                pxans(jloc, m) = scale
c	 	write(6,*) 'answer',iproc,k,jloc,pxans(jloc,m),px(jloc),
c    &		  tmp1
                DO 610 j = -1, -nxtop, -1
 		  node = netew(j)
                  call csend(60000+node+k, scale, 8, node, 0)
 610            CONTINUE
              ELSE
                l = -1
                call crecv(60000+iproc+k, scale, 8)
c		write(6,*) '2005',iproc,scale
              END IF
              py(iloc) = scale
*
*  Processors in row update their scratch vector using the scale value:
*
c	      if(k.eq.49) write(6,*) 'bef dax', (coeff(iloc+(kloc+i)*idim),
c    &		i=0,jloc-kloc-1)
              CALL Daxpy (jloc - kloc, scale,
     &          coeff(iloc + kloc * idim), idim, px(kloc), 1)
c	      write(6,*) 'dax',iproc,k,scale,px(kloc),coeff(iloc+kloc*idim),
c    &		px(jloc-1),coeff(iloc+(jloc-1)*idim),
c    &		iloc+kloc*(idim-1)+jloc-1,iloc,jloc,kloc
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
c	    if(i.eq.kloc+leny-1) write(6,*) 'final dax',iproc,kloc,py(i),
c    &		coeff(i),coeff(i+idim*(kloc-1)),leny
 613      CONTINUE
          len = 8 * kloc
          j = nxproc
 614      IF (j .NE. nproc) THEN
            net = iproc .NEQV. j
            call csend(80000+iproc+j, ptemp, len, net, 0)
            call crecv(80000+net+j, py, len)
c	    write(6,*) '2007',iproc,len,py(1),kblock
            DO 615 i = 0, kloc - 1
              ptemp(i) = ptemp(i) + py(i)
 615        CONTINUE
            j = j + j
            GO TO 614
          END IF
          DO 616 j = 0, kloc - 1
            px(j) = px(j) + ptemp(j)
 616      CONTINUE
 617    CONTINUE
*
*  Next frequency (red, green, and blue for m = 0, 1, 2):
*
        tim_solve = tim_solve + dclock() - tim_i
 618  CONTINUE
      if(iproc .eq. 0) write(6,*) 'time for factor = ', tim_fact,
     &	'time for backsolve',tim_solve
      END
