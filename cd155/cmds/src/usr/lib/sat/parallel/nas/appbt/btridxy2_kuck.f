C*******************************************************************************C
C*******************************************************************************C
C***     Solution of multiple, independent systems of block tridiagonal      ***C
C***     systems using Gaussian elimination (without pivoting) algorithm     ***C
C*******************************************************************************C
C*******************************************************************************C

        subroutine btridxy2
      
        include 'appbt.incl'
        include 'appbt.incl2'

C*******************************************************************************C
C*******************************************************************************C

        do 200 k = 1, nznriv
           ksd = k + kk_pointer
           do 100 j = 1, ny
              temp(1,j,k) = rsd(j,ksd,1)
              temp(2,j,k) = rsd(j,ksd,2)
              temp(3,j,k) = rsd(j,ksd,3)
              temp(4,j,k) = rsd(j,ksd,4)
              temp(5,j,k) = rsd(j,ksd,5)
 100       continue
 200    continue

C*******************************************************************************C
C*******************************************************************************C
C*** Combine forward elimination and substituion phases.

        kpp1 = kbeg_loop - kk_pointer
        ny5  = ny*5

	call kdmbfs (aa(1,1,1,kpp1),bb(1,1,1,kpp1),cc(1,1,1,kpp1),
     &       5,5,ny,klength,ierr,temp(1,1,kpp1),ny5,1)

C*******************************************************************************C
C*******************************************************************************C

        do 300 k = 1, nznriv
           ksd = k + kk_pointer
           do 400 j = 1, ny
              rsd (j,ksd,1) = temp(1,j,k)
              rsd (j,ksd,2) = temp(2,j,k)
              rsd (j,ksd,3) = temp(3,j,k)
              rsd (j,ksd,4) = temp(4,j,k)
              rsd (j,ksd,5) = temp(5,j,k)
 400       continue
 300    continue

        return

        end         

C*******************************************************************************C
C*******************************************************************************C
