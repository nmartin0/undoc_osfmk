	subroutine local23(n1, n2, n3, a, buf)
	integer n1, n2, n3
	complex*16 a(n1, n2, n3), buf(n1, n3, n2)
	integer i, j
 	include 'fnx.h'
c
c  this subroutine locally transposes the 2nd and 3rd dimension of a 3d array
c
	do 20 i = 1, n2
	   do 10 j = 1, n3
	      call zcopy(n1, a(1,i,j), 1, buf(1,j,i), 1)
c	      if(mynode().eq.0) write(6,*) i,j,a(1,i,j)
10	   continue
20	continue
	call zcopy(n1*n2*n3, buf, 1, a, 1)
c	if (mynode() .eq. 0) then
c	  do 50 i = 1, n2
c	    do 60 j = 1, n3
c	      write(6,*) i, j, buf(1,j,i)
c60         continue
c50	  continue
c	endif
	return
	end
