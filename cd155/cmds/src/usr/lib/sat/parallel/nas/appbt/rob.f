From wijngaar Tue Sep 21 17:30:30 1993
Received: from fred.nas.nasa.gov by intelstar.nas.nasa.gov (5.67-NAS.4/5.67-NAS-1.1(sparc))
	id AA08053; Tue, 21 Sep 93 17:30:29 -0700
Received: by fred.nas.nasa.gov (5.67-NAS.2/5.67-NAS-1.1(SGI))
	id AA00277; Tue, 21 Sep 93 17:30:22 -0700
Date: Tue, 21 Sep 93 17:30:22 -0700
From: wijngaar (Rob F. Van Der Wijngaart)
Message-Id: <9309220030.AA00277@fred.nas.nasa.gov>
To: phung
Status: R

c This program tests the efficiency of two storage allocation schemes.
c One specifies the three dimensions of a multi-dimensional array as
c constants (parameters) up front, while the other allocates one
c large one-dimensional array and allocates dynamically within that
c array to create a three-dimensional array with variable dimensions.

       program efficient

       implicit logical (a-z)
       integer isize, jsize, ksize, tsize, i, j, k, size, idyna,
     $         jdyna, kdyna
       parameter (size = 60, isize = size, jsize = size, ksize = size,
     $            tsize = isize*jsize*ksize)
       double precision T1(isize,jsize,ksize), T2(tsize), time, dclock
       double precision T1r(isize,jsize,ksize), T2r(tsize)

       common /bigun/ T1r, T1

       do 10   k = 1, ksize
          do 20   j = 1, jsize
             do 30   i = 1, isize
                T1(i,j,k) = dfloat(i-1)*dfloat(isize-i) *
     $                      dfloat(j-1)*dfloat(jsize-j) *
     $                      dfloat(k-1)*dfloat(ksize-k) 
                T2(i+isize*((j-1)+(k-1)*jsize)) = T1(i,j,k)
 30          continue
 20       continue
 10    continue
       
       time = dclock()

       call relax1

       time = dclock() - time

       print *, "Time to relax the static array:", time

       idyna = 60
       jdyna = 60
       kdyna = 60

       time = dclock()
  
       call relax(T2, T2r, idyna, jdyna, kdyna)

       time = dclock() - time

       print *, "Time to relax the dynamic array:", time

       stop
       end

       subroutine relax( T, Tr, im, jm, km)

       implicit logical (a-z)
       integer i, j, k, im, jm, km
       double precision T(im,jm,km), Tr(im,jm,km)

       do 10   k = 2, km-1
          do 20   j = 2, jm-1
             do 30   i = 2, im-1
                Tr(i,j,k) = (T(i+1,j,k)+T(i-1,j,k)+T(i,j+1,k) +
     $                       T(i,j-1,k)+T(i,j,k+1)+T(i,j,k-1)) *
     $                      0.166666666666d0
 30          continue
 20       continue
 10    continue

       return
       end

                   

       subroutine relax1

       implicit logical (a-z)

       integer isize, jsize, ksize, i, j, k, size
       parameter (size = 60, isize = size, jsize = size, ksize = size)
       double precision T1(isize,jsize,ksize), T1r(isize,jsize,ksize)

       common /bigun/ T1r, T1

       do 10   k = 2, ksize-1
          do 20   j = 2, jsize-1
             do 30   i = 2, isize-1
                T1r(i,j,k) = (T1(i+1,j,k)+T1(i-1,j,k)+T1(i,j+1,k) +
     $                        T1(i,j-1,k)+T1(i,j,k+1)+T1(i,j,k-1)) *
     $                      0.166666666666d0
 30          continue
 20       continue
 10    continue

       return
       end


