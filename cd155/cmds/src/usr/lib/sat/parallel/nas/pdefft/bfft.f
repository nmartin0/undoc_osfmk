	subroutine bfft(n, m, a, offset, stride, isign, wsave, buf)
	integer n, m, offset, stride, isign
	complex*16 a(offset,*)
        complex*16 buf(*),wsave(*),alpha
	integer i, isign_tmp


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
 	      call zfft1d(a(1,i), n, isign, wsave)
 	   else
 	      call zcopy(n, a(1,i), stride, buf, 1)
 	      call zfft1d(buf, n, isign, wsave)
 	      call zcopy(n, buf, 1, a(1,i), stride)
 	   endif
2000	continue
	return
	end
