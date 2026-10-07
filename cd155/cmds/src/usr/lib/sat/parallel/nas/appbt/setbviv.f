C******************************************************************************C
C******************************************************************************C
C***       set the boundary and initial values of dependent variables       ***C
C******************************************************************************C
C******************************************************************************C

        subroutine setbviv

        include 'appbt.incl'

        dimension dtemp(5)

C******************************************************************************C
C******************************************************************************C
C***               set the boundary values for dependent varibales          ***C
C******************************************************************************C
C******************************************************************************C
c*** set the dependent variable values along the top and bottom faces duplicate
c*** calculations on copies of the bottom (ub) and top (ut) planes to avoid 
c*** communication costs in routine setiv().
c*** Tempoprarily replace ub with frct2 and ut with rsd.
        
        do 100 j = 1, ny
           
           eta = dble(j-1)*dnym1

           do 110 i = 1, nx
              
              xi = dble(i-1)*dnxm1

              zeta = 0.d0

              call exact (xi,eta,zeta,dtemp)

              if (my_col .eq. 1) then

                 u(i,j,1) = dtemp(1)
                 u(i,j,2) = dtemp(2)
                 u(i,j,3) = dtemp(3)
                 u(i,j,4) = dtemp(4)
                 u(i,j,5) = dtemp(5)

              endif

              frct2(i,j,1) = dtemp(1)
              frct2(i,j,2) = dtemp(2)
              frct2(i,j,3) = dtemp(3)
              frct2(i,j,4) = dtemp(4)
              frct2(i,j,5) = dtemp(5)
              
              zeta = 1.d0

              call exact (xi,eta,zeta,dtemp)
              
              if (my_col .eq. node_col) then
                 
                 u(i,j,1) = dtemp(1)
                 u(i,j,2) = dtemp(2)
                 u(i,j,3) = dtemp(3)
                 u(i,j,4) = dtemp(4)
                 u(i,j,5) = dtemp(5)
                 
              endif
              
              rsd(i,j,1) = dtemp(1)
              rsd(i,j,2) = dtemp(2)
              rsd(i,j,3) = dtemp(3)
              rsd(i,j,4) = dtemp(4)
              rsd(i,j,5) = dtemp(5)
              
 110       continue
           
 100    continue

C******************************************************************************C
C******************************************************************************C
c***set the dependent variable values along north and south faces
        
        zetanode = dble(my_colm1)*dnzm1

        do 210 i = 1, nx
           
           xi = dble(i-1)*dnxm1

           eta  = 0.d0
           zeta = zetanode

           call exact (xi,eta,zeta,dtemp)
           
           u(i,1,1) = dtemp(1)
           u(i,1,2) = dtemp(2)
           u(i,1,3) = dtemp(3)
           u(i,1,4) = dtemp(4)
           u(i,1,5) = dtemp(5)

           eta  = 0.d0
           zeta = 0.d0

           call exact (xi,eta,zeta,dtemp)
           
           frct2(i,1,1) = dtemp(1)
           frct2(i,1,2) = dtemp(2)
           frct2(i,1,3) = dtemp(3)
           frct2(i,1,4) = dtemp(4)
           frct2(i,1,5) = dtemp(5)
           
           eta  = 0.d0
           zeta = 1.d0

           call exact (xi,eta,zeta,dtemp)
           
           rsd(i,1,1) = dtemp(1)
           rsd(i,1,2) = dtemp(2)
           rsd(i,1,3) = dtemp(3)
           rsd(i,1,4) = dtemp(4)
           rsd(i,1,5) = dtemp(5)
           
           eta  = 1.d0
           zeta = zetanode

           call exact (xi,eta,zeta,dtemp)
           
           u(i,ny,1)  = dtemp(1)
           u(i,ny,2)  = dtemp(2)
           u(i,ny,3)  = dtemp(3)
           u(i,ny,4)  = dtemp(4)
           u(i,ny,5)  = dtemp(5)
           
           eta  = 1.d0
           zeta = 0.d0

           call exact (xi,eta,zeta,dtemp)
           
           frct2(i,ny,1) = dtemp(1)
           frct2(i,ny,2) = dtemp(2)
           frct2(i,ny,3) = dtemp(3)
           frct2(i,ny,4) = dtemp(4)
           frct2(i,ny,5) = dtemp(5)
           
           eta  = 1.d0
           zeta = 1.d0

           call exact (xi,eta,zeta,dtemp)
           
           rsd(i,ny,1) = dtemp(1)
           rsd(i,ny,2) = dtemp(2)
           rsd(i,ny,3) = dtemp(3)
           rsd(i,ny,4) = dtemp(4)
           rsd(i,ny,5) = dtemp(5)
           
 210    continue
        
C******************************************************************************C
C******************************************************************************C
c***set the dependent variable values along east and west faces

        do 310 j = 1, ny
           
           eta = dble(j-1)*dnym1

           xi   = 0.d0
           zeta = zetanode

           call exact (xi,eta,zeta,dtemp)
           
           u(1,j,1)  = dtemp(1)
           u(1,j,2)  = dtemp(2)
           u(1,j,3)  = dtemp(3)
           u(1,j,4)  = dtemp(4)
           u(1,j,5)  = dtemp(5)
           
           xi   = 0.d0
           zeta = 0.d0

           call exact (xi,eta,zeta,dtemp)
           
           frct2(1,j,1) = dtemp(1)
           frct2(1,j,2) = dtemp(2)
           frct2(1,j,3) = dtemp(3)
           frct2(1,j,4) = dtemp(4)
           frct2(1,j,5) = dtemp(5)
           
           xi   = 0.d0
           zeta = 1.d0

           call exact (xi,eta,zeta,dtemp)
           
           rsd(1,j,1) = dtemp(1)
           rsd(1,j,2) = dtemp(2)
           rsd(1,j,3) = dtemp(3)
           rsd(1,j,4) = dtemp(4)
           rsd(1,j,5) = dtemp(5)
           
           xi = 1.d0
           zeta = zetanode

           call exact (xi,eta,zeta,dtemp)
           
           u(nx,j,1) = dtemp(1)
           u(nx,j,2) = dtemp(2)
           u(nx,j,3) = dtemp(3)
           u(nx,j,4) = dtemp(4)
           u(nx,j,5) = dtemp(5)
           
           xi   = 1.d0
           zeta = 0.d0

           call exact (xi,eta,zeta,dtemp)
           
           frct2(nx,j,1) = dtemp(1)
           frct2(nx,j,2) = dtemp(2)
           frct2(nx,j,3) = dtemp(3)
           frct2(nx,j,4) = dtemp(4)
           frct2(nx,j,5) = dtemp(5)
           
           xi   = 1.d0
           zeta = 1.d0

           call exact (xi,eta,zeta,dtemp)
           
           rsd(nx,j,1) = dtemp(1)
           rsd(nx,j,2) = dtemp(2)
           rsd(nx,j,3) = dtemp(3)
           rsd(nx,j,4) = dtemp(4)
           rsd(nx,j,5) = dtemp(5)
           
 310    continue
        
C******************************************************************************C
C******************************************************************************C
C***   set the initial values of independent variables based on tri-linear  ***C
C***   interpolation of boundary values in the computational space.         ***C
C******************************************************************************C
C******************************************************************************C

        if ((my_col.gt.1).and.(my_col.lt.node_col)) then
           
           zeta = dble(my_colm1)*dnzm1
           c1mzeta = 1.d0-zeta

           do 10 m = 1, 5

              do 20 j = 2, nym1

                 eta = dble(j-1)*dnym1
                 c1meta = 1.d0-eta

                 do 30 i = 2, nxm1

                    xi = dble(i-1)*dnxm1

                    pxi   = (1.d0-xi)*u(1,j,m) + xi*u(nx,j,m)
                    peta  =  c1meta*u(i,1,m)   + eta*u(i,ny,m)
                    pzeta =  c1mzeta*frct2(i,j,m) + zeta*rsd(i,j,m)

                    u(i,j,m) = pxi + peta + pzeta
     $                   - pxi*peta - peta*pzeta - pzeta*pxi
     $                   + pxi*peta*pzeta

 30              continue

 20           continue

 10        continue

        endif

        return
        
        end

C******************************************************************************C
C******************************************************************************C
