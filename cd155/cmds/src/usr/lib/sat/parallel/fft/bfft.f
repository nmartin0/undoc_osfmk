	subroutine bfft(n,m,a,offset,stride,isign,wsave)
	integer n, m, offset, stride, isign
	complex*8 a(offset,*)
        complex*8 tem(8192),wsave(*)
	integer i


c
c  this subroutine computes a sequence of 1d ffts (or inverse ffts)
c  depending on which function is passed in.
c
c  n		is the length of the ffts
c
c  m		is the number of ffts
c
c  a		is the data (the row dimension is artificial to make
c		the starting addresses come out right)
c
c  offset	is the distance between starting addresses of the ffts
c
c  stride	is the stride of the individual ffts
c
c  wave		fft constants
c

	do 2000 i = 1, m
 	   if (stride.eq.1) then
 	      call cfft1d(a(1,i), n, isign, wsave)
 	   else
 	      call ccopy(n, a(1,i), stride, tem, 1)
 	      call cfft1d(tem, n, isign, wsave)
 	      call ccopy(n, tem, 1, a(1,i), stride)
 	   endif
2000	continue
	return
	end
