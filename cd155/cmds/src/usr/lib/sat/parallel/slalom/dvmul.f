      subroutine dvmul (n,x,incx,y,incy,z,incz)
C
C		INTEL CORPORATION PROPRIETARY INFORMATION
C
C  This software is supplied under the terms of a license
C  agreement or nondisclosure agreement with Intel Corporation
C  and may not be copied or disclosed except in accordance
C  with the terms of that agreement.
C
C  @(#)Version 3.1
C
C dvmul.f 8.1 90/07/05 12:38:37
C
      integer n,incx,incy,incz
      double precision x(*),y(*),z(*),a
c
c     Vector multiplication, element-by-element.
c     z = x * y
c
      integer i,ix,iy,iz
c
      if (incx.ne.1 .or. incy.ne.1 .or. incz.ne.1) go to 20
      do 10 i = 1, n
         z(i) = x(i) * y(i)
   10 continue
      return
c
   20 ix = 1
      iy = 1
      iz = 1
      if (incx .lt. 0) ix = (-n+1)*incx + 1
      if (incy .lt. 0) iy = (-n+1)*incy + 1
      if (incz .lt. 0) iz = (-n+1)*incz + 1
      do 30 i = 1, n
         z(iz) = x(ix) * y(iy)
         ix = ix + incx
         iy = iy + incy
         iz = iz + incz
   30 continue
      return
      entry dsmul (n,a,x,incx,z,incz)
c
c     Vector times scalar.
c     z = a * x
c
      if (incx.ne.1 .or. incz.ne.1) go to 120
      do 110 i = 1, n
         z(i) = a * x(i)
  110 continue
      return
c
  120 ix = 1
      iz = 1
      if (incx .lt. 0) ix = (-n+1)*incx + 1
      if (incz .lt. 0) iz = (-n+1)*incz + 1
      do 130 i = 1, n
         z(iz) = a * x(ix)
         ix = ix + incx
         iz = iz + incz
  130 continue
      return
      end

