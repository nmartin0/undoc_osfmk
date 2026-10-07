	subroutine global23(n1, p, a, buf, me, nodepid)
	integer n, p, me, nodepid, msgid1, msgtype
	complex*8 a(n1,p), buf(n1)
	include 'fnx.h'
c
c  This subroutine globally transposes the 2nd and 3rd dimension of
c     an n1xpxp matrix which is distributed in third dimension.
c  It implements forced message types.
c
	integer xor, msgid, myswap, index, mslice
	integer csize
	parameter (csize=8)
	do 10 index = 1, p-1
	      myswap = xor(me, index)
	      mslice = mod(myswap, p)
 	      msgtype = index+1100000000
 	      msgid1 = irecv(msgtype, buf, n1*csize)
	      call csend(index, a(1,mslice+1), 0, 
     $   	  myswap, nodepid)
 	      call crecv(index, buf, 0)
	      call csend(msgtype, a(1,mslice+1), n1*csize, 
     $   	  myswap, nodepid)
 	      call msgwait(msgid1)
 	      call ccopy(n1, buf, 1, a(1, mslice+1), 1)
10	continue
	
	return
	end
