	subroutine local23_sm(n1, n2, n3, p, a, buf)
	integer n1, n2, n3, p
	complex*16 a(*), buf(*)
	integer i, j, iadd1, iadd2, iadda, len
 	include 'fnx.h'
c
c  this subroutine locally transposes the 2nd and 3rd dimension of a 3d array
c  It is a modified version of local23.f that is used when it is not necessary
c   to transpose the data back such that the third dimension is
c   distributed among the processors.
c
 	iadda = 1
 	do 30 k = 0, p-1
 	  iadd1 = n1 * n3 * k
	  do 20 j = 1, n3
 	     iadd2 = iadd1 + n1 * (j-1) + 1
	     do 10 i = 1, n2
	        call zcopy(n1, a(iadda), 1, buf(iadd2), 1)
 		iadd2 = iadd2 + n1 * n3 * p
 		iadda = iadda + n1
10	     continue
20	  continue
 30	continue
 	len = n1 * n2 * n3 * p
	call zcopy(len, buf, 1, a, 1)
	return
	end
