C   n       the number (an integer) of elements in the vector(s)
C
C   x, y, z names of argument vectors; z is the result vector.
C           For one input, routine computes
C                 z = <intr>(x), e.g., sin(x)
C           For two inputs, routine computes
C                 z = <intr>(x, y)
C
C       Special cases:
C                 divide:  z = y / x
C
C                 recip:   z = alpha / x  (alpha is a scalar)
C
C   incx, incy, incz
C           the strides of vectors x, y, and z, respectively.
C           A stride may be zero.
C
C   Substitute u,v, and w with above x,y, and z for single precision test
C   Substitute x1, y1, u1, and v1 for vectors of values < 1 for some tests
C
C***************************************************************************

       options /check=overflow /check=bounds /check=underflow

       program testvects

       include 'block.inc'

CCCCC  Initialize arrays

       do 10, i = 1, vectsize
          x( i ) = i + i
          x1( i ) = 1.0 / i
          y( i ) = i * i
          y1( i ) = 1.0 / i
          u( i ) = i + i
          u1( i ) = 1.0 / i
          v( i ) = i * i
          v1( i ) = 1.0 / i
 10    continue

       print *, 'X vector:'
       print '(10F16.8)',(x(i), i=1, vectsize)
       print *, ' '
       print *, 'X1 vector:'
       print '(10F16.8)',(x1(i), i=1, vectsize)
       print *, ' '
       print *, 'Y vector:'
       print '(10F16.8)',(y(i), i=1, vectsize)
       print *, ' '
       print *, 'Y1 vector:'
       print '(10F16.8)',(y1(i), i=1, vectsize)
       print *, ' '
       print *, 'U vector:'
       print '(10F16.8)',(u(i), i=1, vectsize)
       print *, ' '
       print *, 'U1 vector:'
       print '(10F16.8)',(u1(i), i=1, vectsize)
       print *, ' '
       print *, 'V vector:'
       print '(10F16.8)',(v(i), i=1, vectsize)
       print *, ' '
       print *, 'V1 vector:'
       print '(10F16.8)',(v1(i), i=1, vectsize)
       print *, ' '

       n = vectsize
       incx = 1
       incy = 1
       incz = 1
       incu = 1
       incv = 1
       incw = 1

CCCCC  Call each of the test vector routines

       call init_z()
       call dv_acos_test()

       call init_z()
       call dv_asin_test()

       call init_z()
       call dv_atan_test()

       call init_z()
       call dv_atan2_test()

       call init_z()
       call dv_cos_test()

       call init_z()
       call dv_div_test()

       call init_z()
       call dv_exp_test()

       call init_z()
       call dv_log_test()

       call init_z()
       call dv_pow_test()

       call init_z()
       call dv_recp_test()

       call init_z()
       call dv_rsqrt_test()

       call init_z()
       call dv_sin_test()

       call init_z()
       call dv_sqrt_test()

       call init_z()
       call dv_tan_test()

       call init_w()
       call sv_acos_test()

       call init_w()
       call sv_asin_test()

       call init_w()
       call sv_atan_test()

       call init_w()
       call sv_atan2_test()

       call init_w()
       call sv_cos_test()

       call init_w()
       call sv_div_test()

       call init_w()
       call sv_exp_test()

       call init_w()
       call sv_log_test()

       call init_w()
       call sv_pow_test()

       call init_w()
       call sv_recp_test()

       call init_w()
       call sv_rsqrt_test()

       call init_w()
       call sv_sin_test()

       call init_w()
       call sv_sqrt_test()

       call init_w()
       call sv_tan_test()

       end

CCCCC  routine to initialize the Z array

       subroutine init_z
          include 'block.inc'
          integer i
          do 20, i = 1, vectsize
             z( i ) = 0.0
 20       continue
          print *, 'Z vector zeroed'
          print *, ' '
       end

CCCCC  routine to initialize the W array

       subroutine init_w
          include 'block.inc'
          integer i
          do 20, i = 1, vectsize
             w( i ) = 0.0
 20       continue
          print *, 'W vector zeroed'
          print *, ' '
       end

CCCCC  set of test vector routines

       subroutine dv_acos_test
          include 'block.inc'
          call dv_acos(n, x1, incx, z, incz)
          print *, 'dv_acos:'
          print *, 'Z vector:'
          print '(10F20.8)',(z(i), i=1, vectsize)
          print *, ' '
       end

C----

       subroutine dv_asin_test
          include 'block.inc'
          call dv_asin(n, x1, incx, z, incz)
          print *, 'dv_asin:'
          print *, 'Z vector:'
          print '(10F20.8)',(z(i), i=1, vectsize)
          print *, ' '
       end

C----

       subroutine dv_atan_test
          include 'block.inc'
          call dv_atan(n, x, incx, z, incz)
          print *, 'dv_atan:'
          print *, 'Z vector:'
          print '(10F20.8)',(z(i), i=1, vectsize)
          print *, ' '
       end

C----

       subroutine dv_atan2_test
          include 'block.inc'
          call dv_atan2(n, x, incx, y, incy, z, incz)
          print *, 'dv_atan2:'
          print *, 'Z vector:'
          print '(10F20.8)',(z(i), i=1, vectsize)
          print *, ' '
       end

C----

       subroutine dv_cos_test
          include 'block.inc'
          call dv_cos(n, x, incx, z, incz)
          print *, 'dv_cos:'
          print *, 'Z vector:'
          print '(10F20.8)',(z(i), i=1, vectsize)
          print *, ' '
       end

C----

       subroutine dv_div_test
          include 'block.inc'
          call dv_div(n, x, incx, y, incy, z, incz)
          print *, 'dv_div:'
          print *, 'Z vector:'
          print '(10F20.8)',(z(i), i=1, vectsize)
          print *, ' '
       end

C----

       subroutine dv_exp_test
          include 'block.inc'
          call dv_exp(n, x, incx, z, incz)
          print *, 'dv_exp:'
          print *, 'Z vector:'
          print '(10F20.8)',(z(i), i=1, vectsize)
          print *, ' '
       end

C----

       subroutine dv_log_test
          include 'block.inc'
          call dv_log(n, x, incx, z, incz)
          print *, 'dv_log:'
          print *, 'Z vector:'
          print '(10F20.8)',(z(i), i=1, vectsize)
          print *, ' '
       end

C----

       subroutine dv_pow_test
          include 'block.inc'
          call dv_pow(n, x1, incx, y1, incy, z, incz)
          print *, 'dv_pow:'
          print *, 'Z vector:'
          print '(10F20.8)',(z(i), i=1, vectsize)
          print *, ' '
       end

C----

       subroutine dv_recp_test
          include 'block.inc'
          double precision xalpha
          xalpha = 3.0
          call dv_recp(n, xalpha, x, incx, z, incz)
          print *, 'dv_recp:'
          print *, 'Z vector:'
          print '(10F20.8)',(z(i), i=1, vectsize)
          print *, ' '
       end

C----

       subroutine dv_rsqrt_test
          include 'block.inc'
          call dv_rsqrt(n, x, incx, z, incz)
          print *, 'dv_rsqrt:'
          print *, 'Z vector:'
          print '(10F20.8)',(z(i), i=1, vectsize)
          print *, ' '
       end

C----

       subroutine dv_sin_test
          include 'block.inc'
          call dv_sin(n, x, incx, z, incz)
          print *, 'dv_sin:'
          print *, 'Z vector:'
          print '(10F20.8)',(z(i), i=1, vectsize)
          print *, ' '
       end

C----

       subroutine dv_sqrt_test
          include 'block.inc'
          call dv_sqrt(n, x, incx, z, incz)
          print *, 'dv_sqrt:'
          print *, 'Z vector:'
          print '(10F20.8)',(z(i), i=1, vectsize)
          print *, ' '
       end

C----

       subroutine dv_tan_test
          include 'block.inc'
          call dv_tan(n, x, incx, z, incz)
          print *, 'dv_tan:'
          print *, 'Z vector:'
          print '(10F20.8)',(z(i), i=1, vectsize)
          print *, ' '
       end

C---- START SINGLE PRECISION TESTS

       subroutine sv_acos_test
          include 'block.inc'
          call sv_acos(n, u1, incu, w, incw)
          print *, 'sv_acos:'
          print *, 'W vector:'
          print '(10F20.8)',(w(i), i=1, vectsize)
          print *, ' '
       end

C----

       subroutine sv_asin_test
          include 'block.inc'
          call sv_asin(n, u1, incu, w, incw)
          print *, 'sv_asin:'
          print *, 'W vector:'
          print '(10F20.8)',(w(i), i=1, vectsize)
          print *, ' '
       end

C----

       subroutine sv_atan_test
          include 'block.inc'
          call sv_atan(n, u, incu, w, incw)
          print *, 'sv_atan:'
          print *, 'W vector:'
          print '(10F20.8)',(w(i), i=1, vectsize)
          print *, ' '
       end

C----

       subroutine sv_atan2_test
          include 'block.inc'
          call sv_atan2(n, u, incu, v, incv, w, incw)
          print *, 'sv_atan2:'
          print *, 'W vector:'
          print '(10F20.8)',(w(i), i=1, vectsize)
          print *, ' '
       end

C----

       subroutine sv_cos_test
          include 'block.inc'
          call sv_cos(n, u, incu, w, incw)
          print *, 'sv_cos:'
          print *, 'W vector:'
          print '(10F20.8)',(w(i), i=1, vectsize)
          print *, ' '
       end

C----

       subroutine sv_div_test
          include 'block.inc'
          call sv_div(n, u, incu, v, incv, w, incw)
          print *, 'sv_div:'
          print *, 'W vector:'
          print '(10F20.8)',(w(i), i=1, vectsize)
          print *, ' '
       end

C----

       subroutine sv_exp_test
          include 'block.inc'
          call sv_exp(n, u, incu, w, incw)
          print *, 'sv_exp:'
          print *, 'W vector:'
          print '(10F20.8)',(w(i), i=1, vectsize)
          print *, ' '
       end

C----

       subroutine sv_log_test
          include 'block.inc'
          call sv_log(n, u, incu, w, incw)
          print *, 'sv_log:'
          print *, 'W vector:'
          print '(10F20.8)',(w(i), i=1, vectsize)
          print *, ' '
       end

C----

       subroutine sv_pow_test
          include 'block.inc'
          call sv_pow(n, u1, incu, v1, incv, w, incw)
          print *, 'sv_pow:'
          print *, 'W vector:'
          print '(10F20.8)',(w(i), i=1, vectsize)
          print *, ' '
       end

C----

       subroutine sv_recp_test
          include 'block.inc'
          real alpha
          alpha = 3.0
          call sv_recp(n, alpha, u, incu, w, incw)
          print *, 'sv_recp:'
          print *, 'W vector:'
          print '(10F20.8)',(w(i), i=1, vectsize)
          print *, ' '
       end

C----

       subroutine sv_rsqrt_test
          include 'block.inc'
          call sv_rsqrt(n, u, incu, w, incw)
          print *, 'sv_rsqrt:'
          print *, 'W vector:'
          print '(10F20.8)',(w(i), i=1, vectsize)
          print *, ' '
       end

C----

       subroutine sv_sin_test
          include 'block.inc'
          call sv_sin(n, u, incu, w, incw)
          print *, 'sv_sin:'
          print *, 'W vector:'
          print '(10F20.8)',(w(i), i=1, vectsize)
          print *, ' '
       end

C----

       subroutine sv_sqrt_test
          include 'block.inc'
          call sv_sqrt(n, u, incu, w, incw)
          print *, 'sv_sqrt:'
          print *, 'W vector:'
          print '(10F20.8)',(w(i), i=1, vectsize)
          print *, ' '
       end

C----

       subroutine sv_tan_test
          include 'block.inc'
          call sv_tan(n, u, incu, w, incw)
          print *, 'sv_tan:'
          print *, 'W vector:'
          print '(10F20.8)',(w(i), i=1, vectsize)
          print *, ' '
       end
