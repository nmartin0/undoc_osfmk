       implicit double precision (x-z)
       implicit real (u-w)
       parameter (vectsize=10)

       common /dvects/ x(vectsize), y(vectsize), z(vectsize)
       common /dfracs/ x1(vectsize), y1(vectsize), z1(vectsize)
       common /svects/ u(vectsize), v(vectsize), w(vectsize)
       common /sfracs/ u1(vectsize), v1(vectsize), w1(vectsize)
       common /ints/  n, incu, incv, incw, incx, incy, incz
