C*******************************************************************************C
C*******************************************************************************C
C***     Solution of multiple, independent systems of block tridiagonal      ***C
C***     systems using Gaussian elimination (without pivoting) algorithm     ***C
C*******************************************************************************C
C*******************************************************************************C

        subroutine btridxy1
      
        include 'appbt.incl'
        include 'appbt.incl2'

C*******************************************************************************C
C*******************************************************************************C

        do 200 j = 1, nynriv
           jsd = j + jj_pointer
           do 100 i = 1, nx
              temp(1,i,j) = rsd(i,jsd,1)
              temp(2,i,j) = rsd(i,jsd,2)
              temp(3,i,j) = rsd(i,jsd,3)
              temp(4,i,j) = rsd(i,jsd,4)
              temp(5,i,j) = rsd(i,jsd,5)
 100       continue
 200    continue

C*******************************************************************************C
C*******************************************************************************C
C*** Combine forward elimination and Back substitution phases.

        jpp1 = jbeg_loop - jj_pointer
        nx5  = nx*5

	call kdmbfs (aa(1,1,1,jpp1),bb(1,1,1,jpp1),cc(1,1,1,jpp1),
     &       5,5,nx,jlength,ierr,temp(1,1,jpp1),nx5,1)

C*******************************************************************************C
C*******************************************************************************C

        do 300 j = 1, nynriv
           jsd = j + jj_pointer
           do 400 i = 1, nx
              rsd (i,jsd,1) = temp(1,i,j)
              rsd (i,jsd,2) = temp(2,i,j)
              rsd (i,jsd,3) = temp(3,i,j)
              rsd (i,jsd,4) = temp(4,i,j)
              rsd (i,jsd,5) = temp(5,i,j)
 400       continue
 300    continue

        return

        end         

C*******************************************************************************C
C*******************************************************************************C
