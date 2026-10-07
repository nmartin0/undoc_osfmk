	subroutine local12(n, m, a, buf)
	integer n, m, i
	complex*8 a(n,m), buf(m,n)


c  this subroutine locally transposes a 2d array

	do 10 i = 1, m
	   call ccopy(n, a(1,i), 1, buf(i,1), m)
10	continue
	call ccopy(n*m, buf, 1, a, 1)
	return
	end
