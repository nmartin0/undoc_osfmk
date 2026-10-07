*
*******************************************************************************
*  This routine sets up the radiosity matrix... normalizes row sums to 1,     *
*  and includes terms derived from reflectivites and emissivities of faces.   *
*                                                                             *
*  John Gustafson, Diane Rover, Stephen Elbert, and Michael Carter            *
*  Ames Laboratory, Ames, Iowa                                                *
*******************************************************************************
      SUBROUTINE SetUp3 (coeff, emiss, px, pxarea, pxdiag, pxrhs, py,
     &                   rho, info, loop, nx, ny, ok)
*
*  Passed variables:
*    coeff   In matrix, first column used here as a work area.
*    emiss   In vectors, emissivities of the face interiors (R-G-B).
*    px      In vector, column sums from SetUp1 and SetUp2; plural x-subset.
*    pxarea  In vector, 8 * pi * areas of the patches; plural x-subset.
*    pxdiag  Out vectors, diagonal of the system (R-G-B); plural x-subset.
*    pxrhs   Out vectors, right-hand sides of system (R-G-B); plural x-subset.
*    py      In vector, row sums from SetUp1 and SetUp2; plural y-subset.
*    rho     In vectors, reflectivities of the face interiors (R-G-B).
*    info    In vector, useful quantities related to parallelization.
*    loop    In vectors, patch number ranges of the faces.
*    nx      In, size of the problem subset in the x-direction.
*    ny      In, size of the problem subset in the y-direction.
*    ok      Out, .TRUE. if no errors occur, else .FALSE.
*
      INTEGER*4 info(16), loop(6, 2), nx, ny
      REAL*8 coeff(*), emiss(6, 3), px(nx), pxarea(nx), pxdiag(nx, 3)
      REAL*8 pxrhs(nx, 3), py(ny), rho(6, 3)
      LOGICAL ok
*
*  Local variables:
*    tmp1      Floating-point scratch variable.
*    vtmp1-2   Floating-point vector scratch variables.
*    i         General loop counter.
*    iface     Loop counter over the number of faces.
*    ipatch    Outer loop counter over the number of patches.
*    iproc     Number of this node processor.
*    jpatch    Inner loop counter over the number of patches.
*    net       Number of neighbor node for sum reduction.
*    nproc     Number of processors in the allocated subcube.
*    nxproc    Number of processors in the x-direction.
*    nyproc    Number of processors in the y-direction.
*
      REAL*8 tmp1, vtmp1(3), vtmp2(3)
      INTEGER*4 i, iproc, ixproc, iyproc, j, jface, jstart, jpatch
      INTEGER*4 k, length, net, nproc, nxproc, nxtop
      INTEGER*4 nyproc, nytop
      integer node, messtype
*
      iproc = info(1)
      nproc = info(3)
      nxproc = info(4)
      nxtop = nxproc - 1
      nyproc = info(5)
      nytop = nyproc - 1
      ixproc = info(6)
      iyproc = info(7)
c     write(6,*) 'set3 stuf', iproc,nproc,nxproc,nyproc,ixproc,iyproc
*
*  Find row sums with global exchange of partial row sums from SetUp1, SetUp2;
*  Use log collapse across rows, then a log collapse across columns:
*
      length = 8 * ny
      IF (mod(iproc, nxproc) .eq. 0) then
 	messtype = 200
c	write(6,*) 'first',iproc,net,length
 	do 501 j = 1, nxproc-1
 	  call crecv(messtype, coeff, length)
          DO 502 i = 1, ny
            py(i) = py(i) + coeff(i)
 502      CONTINUE
 	  call gsync()
 501	continue
 	messtype = 210
 	node = iproc + 1
 	do 650 j = 1, nxproc-1
 	  call csend(messtype, py, length, node, 0)
 	  call gsync()
 	  node = node + 1
 650    continue
      else
 	messtype = 200
 	node = (iproc/nxproc) * nxproc
 	do 511 j = 1, nxproc-1
 	  if (mod(iproc, nxproc) .eq. j)
     &		call csend(messtype, py, length, node, 0)
          call gsync()
 511    continue
 	messtype = 210
 	do 512 j = 1, nxproc-1
 	  if (mod(iproc, nxproc) .eq. j)
     &		call crecv(messtype, py, length)
 	  call gsync()
 512    continue
      END IF
*
      length = 8 * nx
      IF (iproc .lt. nxproc) then
 	messtype = 250
c	write(6,*) 'first',iproc,net,length
 	do 503 j = 1, nyproc-1
 	  call crecv(messtype, coeff, length)
          DO 504 i = 1, ny
            px(i) = px(i) + coeff(i)
 504      CONTINUE
 	  call gsync()
 503	continue
 	messtype = 260
 	node = iproc + nxproc
 	do 670 j = 1, nyproc-1
 	  call csend(messtype, px, length, node, 0)
 	  call gsync()
 	  node = node + nxproc
 670    continue
      else
 	messtype = 250
 	node = mod(iproc, nxproc)
 	do 513 j = 1, nyproc-1
  	  if (iproc .ge. j*nxproc .and. iproc .lt. (j+1)*nxproc)
     &	    call csend(messtype, px, length, node, 0)
          call gsync()
 513    continue
 	messtype = 260
 	do 514 j = 1, nyproc-1
  	  if (iproc .ge. j*nxproc .and. iproc .lt. (j+1)*nxproc)
     &	    call crecv(messtype, px, length)
 	  call gsync()
 514    continue
      END IF
*
*  Combine row and column sums, since matrix is symmetric:
*
      net = info(15)
      j = 1
      call csend(22020+iproc, py(j), 8*ny, net, 0)
      call crecv(22020+net, coeff, 8*nx)
      DO 505 i = 1, nx
        px(i) = px(i) + coeff(i)
 505  CONTINUE
*
*  Make sure row sums (total form factor) are close to 1:
*
      j = 1
      DO 507 jface = 1, 6
        vtmp1(1) = 1.D0 / rho(jface, 1)
        vtmp1(2) = 1.D0 / rho(jface, 2)
        vtmp1(3) = 1.D0 / rho(jface, 3)
        vtmp2(1) = emiss(jface, 1) * vtmp1(1)
        vtmp2(2) = emiss(jface, 2) * vtmp1(2)
        vtmp2(3) = emiss(jface, 3) * vtmp1(3)
        jstart = ((loop(jface, 1) - 1) / nxproc) * nxproc + ixproc + 1
        IF (jstart .LT. loop(jface, 1)) jstart = jstart + nxproc
        DO 506 jpatch = jstart, loop(jface, 2), nxproc
          tmp1 = px(j)
          IF (ABS(tmp1 - pxarea(j)) .GT. (5.D-9 * tmp1)) THEN
             WRITE (*, *) ' Total form factor too far from unity.',
     &              ' Relative error: ', (tmp1 - pxarea(j)) / tmp1
            ok = .FALSE.
            RETURN
          END IF
          tmp1 = -tmp1
*
*  Assign diagonal entries and right-hand sides:
*
          pxdiag(j, 1) = vtmp1(1) * tmp1
          pxdiag(j, 2) = vtmp1(2) * tmp1
          pxdiag(j, 3) = vtmp1(3) * tmp1
          pxrhs(j, 1) = vtmp2(1) * tmp1
          pxrhs(j, 2) = vtmp2(2) * tmp1
          pxrhs(j, 3) = vtmp2(3) * tmp1
          j = j + 1
 506    CONTINUE
 507  CONTINUE
      ok = .TRUE.
      END
