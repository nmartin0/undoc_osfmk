*
*******************************************************************************
*  This routine sets up the radiosity matrix for orthogonal patches.          *
*                                                                             *
*  John Gustafson, Diane Rover, Stephen Elbert, and Michael Carter            *
*  Ames Laboratory, Ames, Iowa                                                *
*******************************************************************************
*
      SUBROUTINE SetUp2 (coeff, px, pxplace, pxsize, py, pyplace,
     &                   pysize, info, loop, nx, ny)
*
*  Passed variables:
*    coeff   Out matrix, the coefficients of the system to solve.
*    px      Out vector, cumulative column sums for SetUp3, plural x-subset.
*    pxplace In vectors, width-height-depth places of patches, plural x-subset.
*    pxsize  In vectors, width-height sizes of patches, plural x-subset.
*    py      Out vector, cumulative row sums for SetUp3, plural y-subset.
*    pyplace In vectors, width-height-depth places of patches, plural y-subset.
*    pysize  In vectors, width-height sizes of patches, plural y-subset.
*    info    In vector, useful quantities related to parallelization.
*    loop    In vectors, patch number range for faces.
*    nx      In, size of problem subset in the x-direction.
*    ny      In, size of problem subset in the y-direction.
*
      INTEGER*4 info(16), loop(6, 2), nx, ny
      REAL*8 coeff(*), px(nx), pxplace(nx, 3)
      REAL*8 pxsize(nx, 2), py(ny), pyplace(ny, 3), pysize(ny, 2)
*
*  Local variables:
*    r, s    Arrays for point-to-point couplings between patch corners.
*    t       Array of squares of r values, to save recomputation.
*    tmp2-8  Real scratch variables.
*    i-n     General loop counters.
*    idim    Dimension of coeff in the i direction.
*    iface   Loop counter over the number of faces.
*    ipatch  Loop counter over the number of patches.
*    istart  Loop start for the i index.
*    ixproc  Node offset in the array in the x-direction.
*    iyproc  Node offset in the array in the y-direction.
*    jface   Face coupled to iface when computing matrix elements.
*    jpatch  Patch coupled to ipatch when computing matrix elements.
*    jstart  Loop start for the j index.
*
      REAL*8 r11, r12, r21, r22, s11, s12, s21, s22, t11, t12, t21, t22
      REAL*8 u11, u12, u21, u22, v11, v12, v21, v22, w11, w12, w21, w22
      REAL*8 tmp2, tmp8
      REAL*8 t1111, t2211, t1211, t2111, t1212, t2112, t1112, t2212
      REAL*8 t1221, t2121, t1121, t2221, t1122, t2222, t1222, t2122
      INTEGER*4 idim, iface, iloc, ipatch, istart, ixj, ixproc
      INTEGER*4 iyproc, jface, jloc, jpatch, jstart, k, m, n
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
      DO 410 k = 1, 6
        DO 409 n = 1, 2
          iface = k + n
          IF (iface .GT. 6) THEN
            jface = iface - 6
            iface = k
            m = 3 - n
          ELSE
            jface = k
            m = n
          END IF
          jstart = ((loop(jface, 1) - 1) / nxproc) * nxproc + ixproc + 1
          IF (jstart .LT. loop(jface, 1)) jstart = jstart + nxproc
          istart = ((loop(iface, 1) - 1) / nyproc) * nxproc + iyproc + 1
          IF (istart .LT. loop(iface, 1)) istart = istart + nyproc
          jloc = (jstart - 1) / nxproc + 1
          DO 408 jpatch = jstart, loop(jface, 2), nxproc
            iloc = (istart - 1) / nyproc + 1
              s11 = pxplace(jloc, m) - pyplace(iloc, 3)
              s21 = s11 + pxsize(jloc, m)
              s11 = s11 ** 2
              s21 = s21 ** 2
            DO 407 ipatch = istart, loop(iface, 2), nyproc
              r11 = pyplace(iloc, m) - pxplace(jloc, 3 - m)
              r21 = r11 + pysize(iloc, m)
              r12 = r11 - pxsize(jloc, 3 - m)
              r22 = r21 - pxsize(jloc, 3 - m)
              s12 = pyplace(iloc, 3 - m) - pxplace(jloc, 3)
              s22 = s12 + pysize(iloc, 3 - m)
              t11 = r11 ** 2
              t12 = r12 ** 2
              t21 = r21 ** 2
              t22 = r22 ** 2
              s12 = s12 ** 2 + 1.D-35
              s22 = s22 ** 2 + 1.D-35
*
              u11 = s11 + s12
              u12 = s11 + s22
              u21 = s21 + s12
              u22 = s21 + s22
*
              v11 = SQRT (u11)
              v12 = SQRT (u12)
              v21 = SQRT (u21)
              v22 = SQRT (u22)
*
              w11 = 1.D0 / v11
              w12 = 1.D0 / v12
              w21 = 1.D0 / v21
              w22 = 1.D0 / v22
*
              t1111 = t11 + u11
              t2211 = t22 + u11
              t1211 = t12 + u11
              t2111 = t21 + u11
              t1212 = t12 + u12
              t2112 = t21 + u12
              t1112 = t11 + u12
              t2212 = t22 + u12
              t1221 = t12 + u21
              t2121 = t21 + u21
              t1121 = t11 + u21
              t2221 = t22 + u21
              t1122 = t11 + u22
              t2222 = t22 + u22
              t1222 = t12 + u22
              t2122 = t21 + u22
*
              tmp2 = + u11 * LOG(t1111 * t2211 / (t1211 * t2111))
     &               + u12 * LOG(t1212 * t2112 / (t1112 * t2212))
     &               + u21 * LOG(t1221 * t2121 / (t1121 * t2221))
     &               + u22 * LOG(t1122 * t2222 / (t1222 * t2122))
     &               + t11 * LOG(t1112 * t1121 / (t1111 * t1122))
     &               + t12 * LOG(t1211 * t1222 / (t1212 * t1221))
     &               + t21 * LOG(t2111 * t2122 / (t2121 * t2112))
     &               + t22 * LOG(t2212 * t2221 / (t2211 * t2222))
              tmp8 = v11 * (-r11 * ATAN(r11 * w11)
     &                      +r12 * ATAN(r12 * w11)
     &                      +r21 * ATAN(r21 * w11)
     &                      -r22 * ATAN(r22 * w11))
     &              +v12 * (+r11 * ATAN(r11 * w12)
     &                      -r12 * ATAN(r12 * w12)
     &                      -r21 * ATAN(r21 * w12)
     &                      +r22 * ATAN(r22 * w12))
     &              +v21 * (+r11 * ATAN(r11 * w21)
     &                      -r12 * ATAN(r12 * w21)
     &                      -r21 * ATAN(r21 * w21)
     &                      +r22 * ATAN(r22 * w21))
     &              +v22 * (-r11 * ATAN(r11 * w22)
     &                      +r12 * ATAN(r12 * w22)
     &                      +r21 * ATAN(r21 * w22)
     &                      -r22 * ATAN(r22 * w22))
*
              tmp2 = ABS (4.D0 * tmp8 + tmp2)
*
*  Store coeff(i, j) in the reflected upper triangle of coeff, and
*  accumulate the row and column sums for use in the SetUp3 routine.
*
              coeff(ixj + 1 - iloc - (jloc - 1) * idim) = tmp2
              py(iloc) = py(iloc) + tmp2
              px(jloc) = px(jloc) + tmp2
              iloc = iloc + 1
 407        CONTINUE
            jloc = jloc + 1
 408      CONTINUE
 409    CONTINUE
 410  CONTINUE
      END
