*
*******************************************************************************
*  This routine sets up the radiosity matrix for parallel patches.            *
*                                                                             *
*  John Gustafson, Diane Rover, Stephen Elbert, and Michael Carter            *
*  Ames Laboratory, Ames, Iowa                                                *
*******************************************************************************
      SUBROUTINE SetUp1 (box, coeff, px, pxplace, pxsize, py, pyplace,
     &                   pysize, info, loop, nx, ny)
*
*  Passed variables:
*    box     In vector, used to find distance to opposite face.
*    coeff   Out matrix, the coefficients of the system to solve.
*    px      Out vector, cumulative column sums for SetUp3, plural x-subset.
*    pxplace In vectors, width-height-depth places of patches, plural x-subset.
*    pxsize  In vectors, width-height sizes of patches, plural x-subset.
*    py      Out vector, cumulative row sums for SetUp3, plural y-subset.
*    pyplace In vectors, width-height-depth places of patches, plural y-subset.
*    pysize  In vectors, width-height sizes of patches, plural y-subset.
*    info    In vector, useful quantities related to parallelization.
*    loop    In vectors, patch number ranges for faces.
*    nx      In, size of problem subset in the x-direction.
*    ny      In, size of problem subset in the y-direction.
*
      INTEGER*4 info(16), loop(6, 2), nx, ny
      REAL*8 box(7), coeff(*), px(nx), pxplace(nx, 3)
      REAL*8 pxsize(nx, 2), py(ny), pyplace(ny, 3), pysize(ny, 2)
*
*  Local variables:
*    d       Array for point-to-point couplings between patch corners.
*    d2      Array of squares of d values, to save recomputation.
*    tmp1-9  Real scratch variables.
*    i, j    General loop counters.
*    idim    Dimension of coeff in the i direction.
*    iface   Loop counter over the number of faces.
*    ipatch  Loop counter over the number of patches.
*    istart  Loop start for the i index.
*    ixproc  Node offset in the array in the x-direction.
*    iyproc  Node offset in the array in the y-direction.
*    jface   Face coupled to iface when computing matrix elements.
*    jpatch  Patch coupled to ipatch when computing matrix elements.
*    jstart  Loop start for the j index.
*    nxproc  Number of processors in x-direction.
*    nxtop   One less than nxproc, useful as a bit mask.
*    nyproc  Number of processors in y-direction.
*    nytop   One less than nyproc, useful as a bit mask.
*
      REAL*8 d(2, 2, 2), d2(2, 2, 2)
      REAL*8 tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8
      INTEGER*4 i, idim, iface, iloc, ipatch, istart, ixj, ixproc
      INTEGER*4 iyproc, j, jface, jloc, jpatch, jstart, k, m, n
      INTEGER*4 nxproc, nxtop, nyproc, nytop
*
      nxproc = info(4)
      nxtop = nxproc - 1
      nyproc = info(5)
      nytop = nyproc - 1
      ixproc = info(6)
      iyproc = info(7)
      idim = info(12)
      ixj = info(14)
      DO 301 i = 1, nx
        px(i) = 0.
 301  CONTINUE
      DO 302 i = 1, ny
        py(i) = 0.
 302  CONTINUE
*
      DO 314 iface = 4, 6
        tmp1 = box(iface - 1) ** 2
        tmp6 = tmp1 + tmp1
        jface = iface - 3
*
*  Find loop start in j direction.
*
        jstart = ((loop(jface, 1) - 1) / nxproc) * nxproc + ixproc + 1
c	write(6,*) 'jstart',iproc,jstart,jstart1
        IF (jstart .LT. loop(jface, 1)) jstart = jstart + nxproc
        istart = ((loop(iface, 1) - 1) / nyproc) * nyproc + iyproc + 1
c	write(6,*) 'istart',iproc,istart,istart1
        IF (istart .LT. loop(iface, 1)) istart = istart + nyproc
        jloc = (jstart - 1) / nxproc + 1
        DO 313 jpatch = jstart, loop(jface, 2), nxproc
          iloc = (istart - 1) / nyproc + 1
          DO 312 ipatch = istart, loop(iface, 2), nyproc
            DO 303 j = 1, 2
              d(1, 1, j) = pxplace(jloc, j) - pyplace(iloc, j)
              d(2, 1, j) = d(1, 1, j) + pxsize(jloc, j)
              d(1, 2, j) = d(1, 1, j) - pysize(iloc, j)
              d(2, 2, j) = d(2, 1, j) - pysize(iloc, j)
              d2(1, 1, j) = d(1, 1, j) ** 2
              d2(2, 1, j) = d(2, 1, j) ** 2
              d2(1, 2, j) = d(1, 2, j) ** 2
              d2(2, 2, j) = d(2, 2, j) ** 2
 303        CONTINUE
            tmp2 = 0.D0
            DO 307 m = 1, 2
              DO 306 i = 1, 2
                tmp3 = d2(m, i, 2) + tmp1
                tmp4 = DSQRT(tmp3)
                tmp5 = 1.D0 / tmp4
                tmp8 = 0.D0
                DO 305 k = 1, 2
                  DO 304 n = 1, 2
                    tmp7 = d(k, n, 1)
                    tmp8 = -tmp7 * ATAN(tmp7 * tmp5) - tmp8
                    tmp2 = tmp6 * LOG(d2(k, n, 1) + tmp3) - tmp2
 304              CONTINUE
                  tmp2 = -tmp2
                  tmp8 = -tmp8
 305            CONTINUE
                tmp2 = -4.D0 * tmp4 *  tmp8 - tmp2
 306          CONTINUE
              tmp2 = -tmp2
 307        CONTINUE
            DO 311 m = 1, 2
              DO 310 i = 1, 2
                tmp3 = d2(m, i, 1) + tmp1
                tmp4 = DSQRT(tmp3)
                tmp5 = 1.D0 / tmp4
                tmp8 = 0.D0
                DO 309 k = 1, 2
                  DO 308 n = 1, 2
                    tmp7 = d(k, n, 2)
                    tmp8 = -tmp7 * ATAN(tmp7 * tmp5) - tmp8
 308              CONTINUE
                  tmp2 = -tmp2
                  tmp8 = -tmp8
 309            CONTINUE
                tmp2 = -4.D0 * tmp4 * tmp8 - tmp2
 310          CONTINUE
              tmp2 = -tmp2
 311        CONTINUE
*
*  Store coeff(iloc, jloc) in the reflected upper triangle of coeff, and
*  accumulate row and column sums for use in the SetUp3 routine.
*
            coeff(ixj + 1 - iloc - (jloc - 1) * idim) = tmp2
            py(iloc) = py(iloc) + tmp2
            px(jloc) = px(jloc) + tmp2
            iloc = iloc + 1
 312      CONTINUE
        jloc = jloc + 1
 313    CONTINUE
 314  CONTINUE
      END
