C******************************************************************************C
C******************************************************************************C
C***                         compute the surface integral                   ***C
C******************************************************************************C
C******************************************************************************C

        subroutine pintgr

        include 'appbt.incl'

        dimension pk1(isizmax), pk2(isizmax)

C******************************************************************************C
C******************************************************************************C
c*** Replace phi1 and phi2 with frct and frct2; respectively.

        ibeg = 2
        iend = nxm1

        jbeg = 2
        jend = nym2

        kbeg = 3
        kend = nzm1

C******************************************************************************C
C******************************************************************************C

        do 35 m = 1, 5
           do 20 j = 1, ny
              do 30 i = 1, nx
                 frct(i,j,m)  = 0.d0
                 frct2(i,j,m) = 0.d0
 30           continue
 20        continue
 35     continue

        if ((my_col.eq.kbeg).and.(my_row.eq.1)) then
                 
           do 100 j = jbeg, jend
              do 110 i = ibeg, iend
                 frct(i,j,1) = c2*(u(i,j,5)-qu(i,j))
 110          continue
 100       continue

        elseif ((my_col.eq.kend).and.(my_row.eq.1)) then
                 
           do 130 j = jbeg, jend
              do 140 i = ibeg, iend
                 frct2(i,j,1) = c2*(u(i,j,5)-qu(i,j))
 140          continue
 130       continue
           
        endif
        
        frc1 = 0.d0
        
        do 150 j = jbeg, jend-1
           jp1 = j+1
           do 160 i = ibeg, iend-1
              ip1 = i+1
              frc1 = frc1 +
     &             frct(i,j,1)+frct(ip1,j,1)+
     &             frct(i,jp1,1)+frct(ip1,jp1,1)+
     &             frct2(i,j,1)+frct2(ip1,j,1)+
     &             frct2(i,jp1,1)+frct2(ip1,jp1,1)
 160       continue
 150    continue
        
        call gdsum (frc1,1,temp)

        frc1 = dxi*deta*frc1
        
        do 170 i = 1, nx
           pk1(i) = 0.d0
           pk2(i) = 0.d0
 170    continue

        if ((my_col.ge.kbeg).and.(my_col.le.kend)) then
           
           do 180 i = ibeg, iend
              
              pk1(i) = c2*(u(i,jbeg,5)-qu(i,jbeg))
              
              pk2(i) = c2*(u(i,jend,5)-qu(i,jend))
              
 180       continue
           
        endif

c***collect all components of phi1 and phi2
        
        nbytes1 = nx*8
        nbytes2 = nz*8*nnodes

        call gcol (pk1,nbytes1,frct,nbytes2,ncnt1)
        call gcol (pk2,nbytes1,frct2,nbytes2,ncnt2)
        
        frc2 = 0.d0
        
        do 200 k = kbeg, kend-1
           kp1 = k+1
           do 210 i = ibeg, iend-1
              ip1 = i+1
              frc2 = frc2+
     &             frct(i,k,1)+frct(ip1,k,1)+
     &             frct(i,kp1,1)+frct(ip1,kp1,1)+
     &             frct2(i,k,1)+frct2(ip1,k,1)+
     &             frct2(i,kp1,1)+frct2(ip1,kp1,1)
 210       continue
 200    continue

        frc2 = dxi*dzeta*frc2

        do 220 i = 1, isiz2
           pk1(i) = 0.d0
           pk2(i) = 0.d0
 220    continue
        
        if ((my_col.ge.kbeg).and.(my_col.le.kend)) then

           if (my_row.eq.1) then
              do 230 j = jbeg, jend
                 
                 pk1(j) = c2*(u(2,j,5)-qu(2,j))
                 pk2(j) = c2*(u(nxm1,j,5)-qu(nxm1,j))
                 
 230          continue
           endif

        endif

c***collect all components of phi1 and phi2
        
        nbytes1 = ny*8
        nbytes2 = nz*8*nnodes

        call gcol (pk1,nbytes1,frct,nbytes2,ncnt1)
        call gcol (pk2,nbytes1,frct2,nbytes2,ncnt2)
        
        frc3 = 0.d0
        
        do 250 k = kbeg, kend-1
           kp1 = k+1
           do 260 j = jbeg, jend-1
              jp1 = j+1
              frc3 = frc3+
     &             frct(j,k,1)+frct(jp1,k,1)+
     &             frct(j,kp1,1)+frct(jp1,kp1,1)+
     &             frct2(j,k,1)+frct2(jp1,k,1)+
     &             frct2(j,kp1,1)+frct2(jp1,kp1,1)
 260       continue
 250    continue

        frc3 = deta*dzeta*frc3

        frc  = 0.25d0*(frc1+frc2+frc3)

        if (my_node.eq.node_wr) then
           write (6,*) 'frc1 frc2 frc3 = ',frc1,frc2,frc3
        endif

        if (my_node.eq.node_wr) write (6,1001) frc
        
        return
        
 1001   format (//5x,'surface integral = ',1pe20.13//)

        end

C******************************************************************************C
C******************************************************************************C
