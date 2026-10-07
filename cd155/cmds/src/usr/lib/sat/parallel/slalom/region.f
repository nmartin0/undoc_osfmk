*
*******************************************************************************
*  The following routine decomposes the surface of a variable-sized box       *
*  into patches that are as nearly equal in size and square as possible.      *
*                                                                             *
*  John Gustafson, Diane Rover, Stephen Elbert, and Michael Carter            *
*  Ames Laboratory, Ames, Iowa                                                *
*******************************************************************************
      SUBROUTINE Region (box, pxarea, pxplace, pxsize, pyplace, pysize,
     &                   info, loop, npatch, nx, ny, ok)
*
*  Passed variables:
*    box     In vector, the x, y, z dimensions of the box.
*    pxarea  Out vector, 8 * pi * areas of the patches; plural x subset.
*    pxplace Out vectors, width-height-depth place of patches; plural x subset.
*    pxsize  Out vectors, width-height sizes of patches; plural x subset.
*    pyplace Out vectors, width-height-depth place of patches; plural y subset.
*    pysize  Out vectors, width-height sizes of patches; plural y subset.
*    info    In vector, useful quantities related to parallelization.
*    loop    Out vectors, patch number ranges for faces.
*    npatch  In, problem size (number of patches).
*    nx      In, size of problem subset in the x-direction.
*    ny      In, size of problem subset in the y-direction.
*    ok      Out flag, .TRUE. if no errors, else .FALSE.
*
      INTEGER*4 nx, nxproc, ny, nyproc
      REAL*8 box(7), pxarea(nx), pxplace(nx, 3)
      REAL*8 pxsize(nx, 2), pyplace(ny, 3), pysize(ny, 2)
      INTEGER*4 info(16), ixproc, iyproc, loop(6, 2), npatch
      LOGICAL ok
*
*  Local variables:
*    height  Height of a patch within a column.
*    tmp1-2  Floating point scratch variables.
*    vtmp3   Vector of third coordinate value for faces.
*    width   Width of a patch within a row.
*    i       Index into local arrays.
*    icol    Loop counter over the number of columns.
*    iface   Loop counter over the number of faces.
*    ipat    Index of patch relative to first patch in face.
*    ipatch  Index of patch.
*    itmp1-2 Integer scratch variables.
*    ncol    Number of columns on a face.
*    npat    Number of patches on a face
*    numpat  Vector of the number of patches on a face.
*    numcol  Vector of the number of columns on a face.
*    nrow    Number of rows of patches in a column.
*
      REAL*8 height, tmp1, tmp2, vtmp3(6), width
      INTEGER*4 i, icol, iface, ipat, ipatch, itmp1, itmp2
      INTEGER*4 ncol, npat, numcol(6), numpat(6), nrow
*
*  Use array xy-size to compute only subset of geometry needed by this node.
*
      nxproc = info(4)
      nyproc = info(5)
      ixproc = info(6)
      iyproc = info(7)
*
*  Allocate patches to each face, proportionate to areas of each face:
*
      tmp1 = 2. * (box(1) * box(2) + box(2) * box(3) + box(3) * box(1))
      tmp2 = 0.D0
      loop(1, 1) = 1
      DO 201 iface = 1, 5
        tmp2 = tmp2 + box(iface) * box(iface + 1)
        loop(iface, 2) = INT(npatch * tmp2 / tmp1 + .5D0)
        loop(iface + 1, 1) = loop(iface, 2) + 1
 201  CONTINUE
      loop(6, 2) = npatch
*
*  Subdivide each face into numpat patches and numcol columns:
*
      DO 202 iface = 1, 6
        npat = loop(iface, 2) - loop(iface, 1) + 1
        vtmp3(iface) = 0.D0
        IF (iface .GT. 3) vtmp3(iface) = box(iface - 1)
        ncol = INT(SQRT(npat * box(iface) / box(iface + 1)) + .5D0)
        IF (ncol .GT. npat) ncol = npat
        IF (ncol .EQ. 0) ncol = 1
        numpat(iface) = npat
        numcol(iface) = ncol
 202  CONTINUE
*
*  For each patch in the x-subset, compute geometry.
*
      i = 1
      DO 203 ipatch = ixproc + 1, npatch, nxproc
        iface = 1
        IF (ipatch .GT. loop(1, 2)) iface = iface + 1
        IF (ipatch .GT. loop(2, 2)) iface = iface + 1
        IF (ipatch .GT. loop(3, 2)) iface = iface + 1
        IF (ipatch .GT. loop(4, 2)) iface = iface + 1
        IF (ipatch .GT. loop(5, 2)) iface = iface + 1
        ncol = numcol(iface)
        width = box(iface) / ncol
        npat = numpat(iface)
        ipat = ipatch - loop(iface, 1)
        icol = (ipat * ncol) / npat
        itmp1 = ncol - 1 + icol * npat
        itmp2 = itmp1 / ncol
        nrow = (itmp1 + npat) / ncol - itmp2
        IF (nrow .EQ. 0) THEN
          WRITE (*, *) ' Eccentric box requires more patches.'
          ok = .FALSE.
          RETURN
        END IF
        height = box(iface + 1) / nrow
        pxsize(i, 1) = width
        pxsize(i, 2) = height
        pxplace(i, 1) = icol * width
        pxplace(i, 2) = (ipat - itmp2) * height
        pxplace(i, 3) = vtmp3(iface)
        pxarea(i) = height * width * (8.D0 * 3.1415926535897932D0)
        i = i + 1
 203  CONTINUE
*
*  For each patch in the y-subset, compute geometry.
*
      i = 1
      DO 204 ipatch = iyproc + 1, npatch, nyproc
        iface = 1
        IF (ipatch .GT. loop(1, 2)) iface = iface + 1
        IF (ipatch .GT. loop(2, 2)) iface = iface + 1
        IF (ipatch .GT. loop(3, 2)) iface = iface + 1
        IF (ipatch .GT. loop(4, 2)) iface = iface + 1
        IF (ipatch .GT. loop(5, 2)) iface = iface + 1
        ncol = numcol(iface)
        width = box(iface) / ncol
        npat = numpat(iface)
        ipat = ipatch - loop(iface, 1)
        icol = (ipat * ncol) / npat
        itmp1 = ncol - 1 + icol * npat
        itmp2 = itmp1 / ncol
        nrow = (itmp1 + npat) / ncol - itmp2
        IF (nrow .EQ. 0) THEN
          WRITE (*, *) ' Eccentric box requires more patches.'
          ok = .FALSE.
          RETURN
        END IF
        height = box(iface + 1) / nrow
        pysize(i, 1) = width
        pysize(i, 2) = height
        pyplace(i, 1) = icol * width
        pyplace(i, 2) = (ipat - itmp2) * height
        pyplace(i, 3) = vtmp3(iface)
        i = i + 1
 204  CONTINUE
*
      ok = .TRUE.
      END
