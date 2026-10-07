      integer function   icnvrt   ( x, ipwr2 )

      double precision  x

      integer           ipwr2
c
c     scale a real*8 number x in (0,1) by a power of 2 and chop it
c
      icnvrt = int (ipwr2 * x)

      return

      end
