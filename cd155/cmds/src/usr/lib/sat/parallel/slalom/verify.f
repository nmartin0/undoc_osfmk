*
*******************************************************************************
*  The following routine computes the residual of the matrix solution to      *
*  verify that ||Ax - b|| is within tolerance limits.                         *
*                                                                             *
*  John Gustafson, Diane Rover, Stephen Elbert, and Michael Carter            *
*  Ames Laboratory, Ames, Iowa                                                *
*******************************************************************************
      SUBROUTINE Verify (coeff, ptemp, px, pxans, pxdiag, pxrhs, py,
     &                   info, npatch, nx, ny)
*
*  Passed variables:
*    coeff   Matrix, the coefficients of the equations.
*    ptemp   Vector, work area; used for both x and y subsets.
*    px      Vector, work area; plural x-subset.
*    pxans   In vectors, radiosities from solved system; plural x-subset.
*    pxdiag  Vectors, diagonal terms of the equations (R-G-B), plural x-subset.
*    pxrhs   In vectors, right-hand sides of R-G-B equations; plural x-subset.
*    py      In vector, work area; plural y-subset.
*    info    In vector, useful quantities related to parallelization.
*    npatch  In, problem size (number of patches).
*    nx      In, size of problem subset in the x-direction.
*    ny      In, size of problem subset in the y-direction.
*
      INTEGER*4 info(16), netn, nets, npatch, nx, ny
      REAL*8 coeff(*), ptemp(*), px(nx), pxans(nx, 3), pxdiag(nx, 3)
      REAL*8 pxrhs(nx, 3), py(ny)
      integer node, messtype, netns(-31:31)
*
*  Local variables:
*    anorm   Norm of matrix, maximum absolute value element.
*    resid   Norm of ||Ax - b||, maximum absolute value element.
*    temp    Real temporary variable.
*    xnorm   Norm of answer, maximum absolute value element.
*    i, j    General loop counters
*
      REAL*8 anorm, resid, temp(2), xnorm
      INTEGER*4 i, idim, index, iproc, istart, ixj, ixproc, iyproc
      INTEGER*4 j, jdim, k, lenx, leny, m, net, nproc
      INTEGER*4 nskip, ntran, nxproc, nxtop, nyproc, nytop
*
      iproc = info(1)
      nproc = info(3)
      nxproc = info(4)
      nxtop = nxproc - 1
      nyproc = info(5)
      nytop = nyproc - 1
      ixproc = info(6)
      iyproc = info(7)
      netn = info(9)
      nets = info(10)
      idim = info(12)
      jdim = info(13)
      ixj  = info(14) + 1
      ntran = info(15)
      nskip = idim + 1
      lenx = nx * 8
      leny = ny * 8
      netns(0) = iproc
      do 601 i = 1, nytop
 	netns(i) = netns(i-1) + nxproc
 	if(netns(i) .ge. nproc) netns(i) = netns(i) - nproc
 601  continue
      do 701 i = -1, -nytop, -1
 	netns(i) = netns(i+1) - nxproc
 	if (netns(i) .lt. 0) netns(i) = netns(i) + nproc
 701  continue

*
      DO 916 m = 1, 3
        resid = 0.
        anorm = 0.
        xnorm = 0.
*
*  Circulate pxans down columns, then to transpose neighbor as a py vector:
*
        IF (iyproc .NE. ixproc)
     &    call crecv(101200+iproc, pxans(1,m), lenx)
        call csend(101200+nets, pxans(1,m), lenx, nets, 0)
        call csend(104201+ntran, pxans(1,m), lenx, ntran, 0)
        call crecv(104201+iproc, ptemp, leny)
*
*  Matrix-vector multiply coeff by pxans to get partial dot-products;
*  at the same time, get anorm (since entire submatrix is being traversed):
*
        DO 901 j = 1, nx
          px(j) = 0.
 901    CONTINUE
        DO 902 i = 1, ny
          py(i) = 0.
 902    CONTINUE
        IF (ixproc .EQ. iyproc) THEN
          i = ixj - 1
          DO 903 j = 1, nx
            coeff(i) = pxdiag(j, m)
            i = i - nskip
 903      CONTINUE
        END IF
        istart = 1
        IF (ixproc .GT. iyproc) istart = 2
        DO 905 i = istart, ny
          DO 904 j = 1, i - istart + 1
            index = ixj - (j - 1) * idim - i
            px(j) = px(j) + ptemp(i) * coeff(index)
            anorm = MAX(anorm, ABS(coeff(index)))
 904      CONTINUE
 905    CONTINUE
        IF (ixproc .EQ. iyproc) istart = 2
        DO 907 i = istart, ny
          DO 906 j = 1, i - istart + 1
            index = ixj - (j - 1) * idim - i
            py(i) = py(i) + pxans(j, m) * coeff(index)
 906      CONTINUE
 907    CONTINUE
*
*  Collapse partial dot-product sums:
*
 	if (mod(iproc, nxproc) .eq. 0) then
 	  messtype = 100000
 	  do 908 j = 1, nxproc-1
 	    call crecv(messtype, ptemp, leny)
 	    do 909 i = 1, ny
 	      py(i) = py(i) + ptemp(i)
 909	    continue
 908	  continue
 	  messtype = 110000
 	  node = iproc + 1
 	  do 1908 j = 1, nxproc-1
 	    call csend(messtype, py, leny, node, 0)
 	    node = node + 1
 1908	  continue
 	else
 	  messtype = 100000
 	  node = (iproc/nxproc) * nxproc
 	  call csend(messtype, py, leny, node, 0)
 	  messtype = 110000
 	  call crecv(messtype, py, leny)
 	endif
 	if (iyproc. eq. 0) then
 	  messtype = 120000
	  do 910 j = 1, nyproc - 1
 	    call crecv(messtype, ptemp, lenx)
 	    do 911 i = 1, nx
 	      px(i) = px(i) + ptemp(i)
 911	    continue
 910	  continue
 	  messtype = 130000
 	  node = iproc + nxproc
 	  do 1911 j = 1, nyproc - 1
 	    call csend(messtype, px, lenx, node, 0)
 	    node = node + nxproc
 1911	  continue
 	else    
 	  messtype = 120000
 	  node = netns(-iyproc)
 	  call csend(messtype, px, lenx, node, 0)
 	  messtype = 130000
 	  call crecv(messtype, px, lenx)
 	endif
*
*  Send py vector to transpose, and add for complete matrix-vector product:
*
          call csend(111204+ntran, py, leny, ntran, 0)
          call crecv(111204+iproc, ptemp, lenx)
          DO 912 i = 1, nx
            px(i) = px(i) + ptemp(i)
 912      CONTINUE
*
*  Compare matrix times answer with right-hand side, and compute norms:
*
          DO 913 j = 1, nx
            resid = MAX(resid, ABS(px(j) - pxrhs(j, m)))
            xnorm = MAX(xnorm, pxans(j, m))
 913      CONTINUE
c	  write(6,*) 'resid',iproc,resid,px(1),pxrhs(1,m),px(nx),
c    &		pxrhs(nx,m)
 	if (mod(iproc, nxproc) .eq. 0) then
 	  messtype = 120000
 	  do 958 j = 1, nxproc-1
 	    call crecv(messtype, temp, 16)
            resid = MAX(resid, temp(1))
            xnorm = MAX(xnorm, temp(2))
 958	  continue
 	  messtype = 130000
 	  node = iproc + 1
 	  temp(1) = resid
    	  temp(2) = xnorm
 	  do 1958 j = 1, nxproc-1
 	    call csend(messtype, temp, 16, node, 0)
 	    node = node + 1
 1958	  continue
 	else
 	  messtype = 120000
 	  node = (iproc/nxproc) * nxproc
 	  temp(1) = resid
    	  temp(2) = xnorm
 	  call csend(messtype, temp, 16, node, 0)
 	  messtype = 130000
 	  call crecv(messtype, temp, 16)
 	  resid = temp(1)
    	  xnorm = temp(2)
 	endif
*
*  Get global norm of coeff matrix:
*
 	if (mod(iproc, nxproc) .eq. 0) then
 	  messtype = 140000
 	  do 978 j = 1, nxproc-1
 	    call crecv(messtype, temp, 8)
            anorm = MAX(anorm, temp(1))
 978	  continue
 	  messtype = 150000
 	  node = iproc + 1
 	  do 1978 j = 1, nxproc-1
 	    call csend(messtype, anorm, 8, node, 0)
 	    node = node + 1
 1978	  continue
 	else
 	  messtype = 140000
 	  node = (iproc/nxproc) * nxproc
 	  call csend(messtype, anorm, 8, node, 0)
 	  messtype = 150000
 	  call crecv(messtype, anorm, 8)
 	endif
*
*  Compare against acceptable limit:
*
        resid = resid / (anorm * xnorm)
c	if(iproc .eq. 0) write(6,*) 'res anorm xnorm',resid,anorm,xnorm
        IF (resid .GT. 5.D-9) THEN
          IF (iproc .EQ. 0) WRITE (*, *) 'Residual too large: ', resid,
     &		'm = ',m
        END IF
 916  CONTINUE
      END
