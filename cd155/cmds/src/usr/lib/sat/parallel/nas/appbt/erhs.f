C******************************************************************************C
C******************************************************************************C
C***         compute the right hand side based on exact solution            ***C
C******************************************************************************C
C******************************************************************************C

        subroutine erhs

        include 'appbt.incl'

        dimension dtemp(5)
        dimension dtemp1(5),dtemp2(5),dtemp3(5),dtemp4(5),dtemp5(5)

        dimension buf(isiz1,5),cuf(isiz1),ue(isiz1,5)

C******************************************************************************C
C******************************************************************************C

        do 20 j = 1, ny
           do 30 i = 1, nx

              frct(i,j,1) = 0.d0
              frct(i,j,2) = 0.d0
              frct(i,j,3) = 0.d0
              frct(i,j,4) = 0.d0
              frct(i,j,5) = 0.d0

 30        continue
 20     continue

C******************************************************************************C
C******************************************************************************C
C***                      xi-direction flux differences                     ***C
C******************************************************************************C
C******************************************************************************C

         if ((my_col.gt.1).and.(my_col.lt.node_col)) then

            knode = my_col
            
            zeta = dble(knode-1)*dnzm1
            
            do 100 j = 2, nym1
               
               eta = dble(j-1)*dnym1
               
               do 110 i = 1, nx
                  
                  xi = dble(i-1)*dnxm1
                  
                  call exact (xi,eta,zeta,dtemp)
                  
                  ue(i,1) = dtemp(1)
                  ue(i,2) = dtemp(2)
                  ue(i,3) = dtemp(3)
                  ue(i,4) = dtemp(4)
                  ue(i,5) = dtemp(5)
                  
                  dtpp = 1.d0/dtemp(1)
                  
                  buf(i,2) = dtpp*dtemp(2)
                  buf(i,3) = dtpp*dtemp(3)
                  buf(i,4) = dtpp*dtemp(4)
                  buf(i,5) = dtpp*dtemp(5)
                  
                  cuf(i)   = buf(i,2)*buf(i,2)
                  buf(i,1) = cuf(i)+buf(i,3)*buf(i,3)+
     &                 buf(i,4)*buf(i,4)
                  
 110           continue
               
               do 150 i = 2, nxm1
                  
                  im1 = i-1
                  ip1 = i+1
                  
                  qm1 = 0.5d0*(buf(im1,2)*ue(im1,2)+
     &                 buf(im1,3)*ue(im1,3)+buf(im1,4)*ue(im1,4))

                  qp1 = 0.5d0*(buf(ip1,2)*ue(ip1,2)+
     &                 buf(ip1,3)*ue(ip1,3)+buf(ip1,4)*ue(ip1,4))
                  
                  frct(i,j,1) = frct(i,j,1)-tx2*(ue(ip1,2)-ue(im1,2))+
     &                 dx1tx1*(ue(im1,1)-2.d0*ue(i,1)+ue(ip1,1))
                  
                  frct(i,j,2) = frct(i,j,2)-tx2*(
     &                 (ue(ip1,2)*buf(ip1,2)+c2*(ue(ip1,5)-qp1))-
     &                 (ue(im1,2)*buf(im1,2)+c2*(ue(im1,5)-qm1)))+
     &                 xxcon1*(buf(ip1,2)-2.d0*buf(i,2)+buf(im1,2))+
     $                 dx2tx1*(ue(im1,2)-2.d0*ue(i,2)+ue(ip1,2))
                  
                  frct(i,j,3) = frct(i,j,3)-tx2*(
     &                 ue(ip1,3)*buf(ip1,2)-ue(im1,3)*buf(im1,2))+
     &                 xxcon2*(buf(ip1,3)-2.d0*buf(i,3)+buf(im1,3))+
     $                 dx3tx1*(ue(im1,3)-2.d0*ue(i,3)+ue(ip1,3))
                  
                  frct(i,j,4) = frct(i,j,4)-tx2*(
     &                 ue(ip1,4)*buf(ip1,2)-ue(im1,4)*buf(im1,2))+
     &                 xxcon2*(buf(ip1,4)-2.d0*buf(i,4)+buf(im1,4))+
     $                 dx4tx1*(ue(im1,4)-2.d0*ue(i,4)+ue(ip1,4))
                  
                  frct(i,j,5) = frct(i,j,5)-tx2*(
     &                 buf(ip1,2)*(c1*ue(ip1,5)-c2*qp1)-
     &                 buf(im1,2)*(c1*ue(im1,5)-c2*qm1))+
     &                 xxcon3*(buf(ip1,1)-2.d0*buf(i,1)+buf(im1,1))+
     &                 xxcon4*(cuf(ip1)-2.d0*cuf(i)+cuf(im1))+
     &                 xxcon5*(buf(ip1,5)-2.d0*buf(i,5)+buf(im1,5))+
     $                 dx5tx1*(ue(im1,5)-2.d0*ue(i,5)+ue(ip1,5))
                  
 150           continue
               
c***Fourth-order dissipation
                  
               do 160 m = 1, 5

                  frct(2,j,m) = frct(2,j,m)-dssp*(5.d0*ue(2,m)-
     $                 4.d0*ue(3,m)+ue(4,m))
                  frct(3,j,m) = frct(3,j,m)-dssp*(-4.d0*ue(2,m)+
     $                 6.d0*ue(3,m)-4.d0*ue(4,m)+ue(5,m))

                  frct(nxm2,j,m) = frct(nxm2,j,m)-dssp*(ue(nxm4,m)-
     $                 4.d0*ue(nxm3,m)+6.d0*ue(nxm2,m)-4.d0*ue(nxm1,m))
                  frct(nxm1,j,m) = frct(nxm1,j,m)-dssp*(ue(nxm3,m)-
     $                 4.d0*ue(nxm2,m)+5.d0*ue(nxm1,m))

 160           continue
               
               do 200 i = 4, nxm3
                  
                  im1 = i-1
                  im2 = i-2
                  ip1 = i+1
                  ip2 = i+2
                  
                  frct(i,j,1) = frct(i,j,1)-dssp*(ue(im2,1)-
     &                 4.d0*ue(im1,1)+6.d0*ue(i,1)-
     $                 4.d0*ue(ip1,1)+ue(ip2,1))
                  frct(i,j,2) = frct(i,j,2)-dssp*(ue(im2,2)-
     &                 4.d0*ue(im1,2)+6.d0*ue(i,2)-
     $                 4.d0*ue(ip1,2)+ue(ip2,2))
                  frct(i,j,3) = frct(i,j,3)-dssp*(ue(im2,3)-
     &                 4.d0*ue(im1,3)+6.d0*ue(i,3)-
     $                 4.d0*ue(ip1,3)+ue(ip2,3))
                  frct(i,j,4) = frct(i,j,4)-dssp*(ue(im2,4)-
     &                 4.d0*ue(im1,4)+6.d0*ue(i,4)-
     $                 4.d0*ue(ip1,4)+ue(ip2,4))
                  frct(i,j,5) = frct(i,j,5)-dssp*(ue(im2,5)-
     &                 4.d0*ue(im1,5)+6.d0*ue(i,5)-
     $                 4.d0*ue(ip1,5)+ue(ip2,5))
                  
 200           continue
               
 100        continue
            
C******************************************************************************C
C******************************************************************************C
C***                      eta-direction flux differences                    ***C
C******************************************************************************C
C******************************************************************************C
            
            do 300 i = 2, nxm1
               
               xi = dble(i-1)*dnxm1
               
               do 310 j = 1, ny
                  
                  eta = dble(j-1)*dnym1
                  
                  call exact (xi,eta,zeta,dtemp)
                  
                  ue(j,1) = dtemp(1)
                  ue(j,2) = dtemp(2)
                  ue(j,3) = dtemp(3)
                  ue(j,4) = dtemp(4)
                  ue(j,5) = dtemp(5)
                  
                  dtpp = 1.d0/dtemp(1)
                  
                  buf(j,2) = dtpp*dtemp(2)
                  buf(j,3) = dtpp*dtemp(3)
                  buf(j,4) = dtpp*dtemp(4)
                  buf(j,5) = dtpp*dtemp(5)
                  
                  cuf(j)   = buf(j,3)*buf(j,3)
                  buf(j,1) = buf(j,2)*buf(j,2)+cuf(j)+
     &                 buf(j,4)*buf(j,4)
                  
 310           continue
               
               do 350 j = 2, nym1
                  
                  jm1 = j-1
                  jp1 = j+1
                  
                  qm1 = 0.5d0*(buf(jm1,2)*ue(jm1,2)+
     &                 buf(jm1,3)*ue(jm1,3)+buf(jm1,4)*ue(jm1,4))
                  
                  qp1 = 0.5d0*(buf(jp1,2)*ue(jp1,2)+
     &                 buf(jp1,3)*ue(jp1,3)+buf(jp1,4)*ue(jp1,4))
                  
                  frct(i,j,1) = frct(i,j,1)-ty2*(ue(jp1,3)-ue(jm1,3))+
     &                 dy1ty1*(ue(jm1,1)-2.d0*ue(j,1)+ue(jp1,1))

                  frct(i,j,2) = frct(i,j,2)-ty2*(
     &                 ue(jp1,2)*buf(jp1,3)-ue(jm1,2)*buf(jm1,3))+
     &                 yycon2*(buf(jp1,2)-2.d0*buf(j,2)+buf(jm1,2))+
     $                 dy2ty1*(ue(jm1,2)-2.d0*ue(j,2)+ue(jp1,2))

                  frct(i,j,3) = frct(i,j,3)-ty2*(
     &                 (ue(jp1,3)*buf(jp1,3)+c2*(ue(jp1,5)-qp1))-
     &                 (ue(jm1,3)*buf(jm1,3)+c2*(ue(jm1,5)-qm1)))+
     &                 yycon1*(buf(jp1,3)-2.d0*buf(j,3)+buf(jm1,3))+
     $                 dy3ty1*(ue(jm1,3)-2.d0*ue(j,3)+ue(jp1,3))

                  frct(i,j,4) = frct(i,j,4)-ty2*(
     &                 ue(jp1,4)*buf(jp1,3)-ue(jm1,4)*buf(jm1,3))+
     &                 yycon2*(buf(jp1,4)-2.d0*buf(j,4)+buf(jm1,4))+
     $                 dy4ty1*(ue(jm1,4)-2.d0*ue(j,4)+ue(jp1,4))

                  frct(i,j,5) = frct(i,j,5)-ty2*(
     &                 buf(jp1,3)*(c1*ue(jp1,5)-c2*qp1)-
     &                 buf(jm1,3)*(c1*ue(jm1,5)-c2*qm1))+
     &                 yycon3*(buf(jp1,1)-2.d0*buf(j,1)+buf(jm1,1))+
     &                 yycon4*(cuf(jp1)-2.d0*cuf(j)+cuf(jm1))+
     &                 yycon5*(buf(jp1,5)-2.d0*buf(j,5)+buf(jm1,5))+
     $                 dy5ty1*(ue(jm1,5)-2.d0*ue(j,5)+ue(jp1,5))

 350           continue

c***fourth-order dissipation

               do 360 m = 1, 5

                  frct(i,2,m) = frct(i,2,m)-dssp*(5.d0*ue(2,m)-
     $                 4.d0*ue(3,m)+ue(4,m))
                  frct(i,3,m) = frct(i,3,m)-dssp*(-4.d0*ue(2,m)+
     $                 6.d0*ue(3,m)-4.d0*ue(4,m)+ue(5,m))

                  frct(i,nym2,m) = frct(i,nym2,m)-dssp*(ue(nym4,m)-
     $                 4.d0*ue(nym3,m)+6.d0*ue(nym2,m)-4.d0*ue(nym1,m))
                  frct(i,nym1,m) = frct(i,nym1,m)-dssp*(ue(nym3,m)-
     $                 4.d0*ue(nym2,m)+5.d0*ue(nym1,m))

 360           continue

               do 400 j = 4, nym3
                  
                  jm1 = j-1
                  jm2 = j-2
                  jp1 = j+1
                  jp2 = j+2

                  frct(i,j,1) = frct(i,j,1)-dssp*(ue(jm2,1)-
     &                 4.d0*ue(jm1,1)+6.d0*ue(j,1)-
     &                 4.d0*ue(jp1,1)+ue(jp2,1))
                  frct(i,j,2) = frct(i,j,2)-dssp*(ue(jm2,2)-
     &                 4.d0*ue(jm1,2)+6.d0*ue(j,2)-
     &                 4.d0*ue(jp1,2)+ue(jp2,2))
                  frct(i,j,3) = frct(i,j,3)-dssp*(ue(jm2,3)-
     &                 4.d0*ue(jm1,3)+6.d0*ue(j,3)-
     &                 4.d0*ue(jp1,3)+ue(jp2,3))
                  frct(i,j,4) = frct(i,j,4)-dssp*(ue(jm2,4)-
     &                 4.d0*ue(jm1,4)+6.d0*ue(j,4)-
     &                 4.d0*ue(jp1,4)+ue(jp2,4))
                  frct(i,j,5) = frct(i,j,5)-dssp*(ue(jm2,5)-
     &                 4.d0*ue(jm1,5)+6.d0*ue(j,5)-
     &                 4.d0*ue(jp1,5)+ue(jp2,5))
                  
 400           continue

 300        continue

C******************************************************************************C
C******************************************************************************C
C***                      zeta-direction flux differences                   ***C
C******************************************************************************C
C******************************************************************************C

            knode = my_col
            
            km1   = knode-1
            kp1   = knode+1

            do 500 j = 2, nym1

               eta = dble(j-1)*dnym1
            
               do 510 i = 2, nxm1
                  
                  xi = dble(i-1)*dnxm1
               
                  k = km1
                  zeta = dble(k-1)*dnzm1

                  call exact (xi,eta,zeta,dtemp)

                  ue(k,1) = dtemp(1)
                  ue(k,2) = dtemp(2)
                  ue(k,3) = dtemp(3)
                  ue(k,4) = dtemp(4)
                  ue(k,5) = dtemp(5)

                  dtpp1 = 1.d0/dtemp(1)

                  buf(k,2) = dtpp1*dtemp(2)
                  buf(k,3) = dtpp1*dtemp(3)
                  buf(k,4) = dtpp1*dtemp(4)
                  buf(k,5) = dtpp1*dtemp(5)

                  cuf(k)   = buf(k,4)*buf(k,4)
                  buf(k,1) = buf(k,2)*buf(k,2)+
     &                       buf(k,3)*buf(k,3)+cuf(k)

                  k = knode
                  zeta = dble(k-1)*dnzm1

                  call exact (xi,eta,zeta,dtemp)

                  ue(k,1) = dtemp(1)
                  ue(k,2) = dtemp(2)
                  ue(k,3) = dtemp(3)
                  ue(k,4) = dtemp(4)
                  ue(k,5) = dtemp(5)

                  dtpp2 = 1.d0/ue(k,1)

                  buf(k,2) = dtpp2*dtemp(2)
                  buf(k,3) = dtpp2*dtemp(3)
                  buf(k,4) = dtpp2*dtemp(4)
                  buf(k,5) = dtpp2*dtemp(5)

                  cuf(k)   = buf(k,4)*buf(k,4)
                  buf(k,1) = buf(k,2)*buf(k,2)+
     &                       buf(k,3)*buf(k,3)+cuf(k)

                  k = kp1
                  zeta = dble(k-1)*dnzm1

                  call exact (xi,eta,zeta,dtemp)

                  ue(k,1) = dtemp(1)
                  ue(k,2) = dtemp(2)
                  ue(k,3) = dtemp(3)
                  ue(k,4) = dtemp(4)
                  ue(k,5) = dtemp(5)

                  dtpp3 = 1.d0/dtemp(1)

                  buf(k,2) = dtpp3*dtemp(2)
                  buf(k,3) = dtpp3*dtemp(3)
                  buf(k,4) = dtpp3*dtemp(4)
                  buf(k,5) = dtpp3*dtemp(5)

                  cuf(k)   = buf(k,4)*buf(k,4)
                  buf(k,1) = buf(k,2)*buf(k,2)+
     &                       buf(k,3)*buf(k,3)+cuf(k)

                  qm1 = 0.5d0*(buf(km1,2)*ue(km1,2)+
     &                 buf(km1,3)*ue(km1,3)+buf(km1,4)*ue(km1,4))

                  qp1 = 0.5d0*(buf(kp1,2)*ue(kp1,2)+
     &                 buf(kp1,3)*ue(kp1,3)+buf(kp1,4)*ue(kp1,4))
                  
                  frct(i,j,1) = frct(i,j,1)-tz2*(ue(kp1,4)-ue(km1,4))+
     $                 dz1tz1*(ue(kp1,1)-2.d0*ue(knode,1)+ue(km1,1))

                  frct(i,j,2) = frct(i,j,2)-tz2*(
     &                 ue(kp1,2)*buf(kp1,4)-ue(km1,2)*buf(km1,4))+
     &                 zzcon2*(buf(kp1,2)-2.d0*buf(knode,2)+buf(km1,2))+
     $                 dz2tz1*(ue(kp1,2)-2.d0*ue(knode,2)+ue(km1,2))

                  frct(i,j,3) = frct(i,j,3)-tz2*(
     &                 ue(kp1,3)*buf(kp1,4)-ue(km1,3)*buf(km1,4))+
     &                 zzcon2*(buf(kp1,3)-2.d0*buf(knode,3)+buf(km1,3))+
     $                 dz3tz1*(ue(kp1,3)-2.d0*ue(knode,3)+ue(km1,3))

                  frct(i,j,4) = frct(i,j,4)-tz2*(
     &                 (ue(kp1,4)*buf(kp1,4)+c2*(ue(kp1,5)-qp1))-
     &                 (ue(km1,4)*buf(km1,4)+c2*(ue(km1,5)-qm1)))+
     &                 zzcon1*(buf(kp1,4)-2.d0*buf(knode,4)+buf(km1,4))+
     $                 dz4tz1*(ue(kp1,4)-2.d0*ue(knode,4)+ue(km1,4))

                  frct(i,j,5) = frct(i,j,5)-tz2*(
     &                 buf(kp1,4)*(c1*ue(kp1,5)-c2*qp1)-
     &                 buf(km1,4)*(c1*ue(km1,5)-c2*qm1))+
     &                 zzcon3*(buf(kp1,1)-2.d0*buf(knode,1)+buf(km1,1))+
     &                 zzcon4*(cuf(kp1)-2.d0*cuf(knode)+cuf(km1))+
     &                 zzcon5*(buf(kp1,5)-2.d0*buf(knode,5)+buf(km1,5))+
     $                 dz5tz1*(ue(kp1,5)-2.d0*ue(knode,5)+ue(km1,5))
                  
 510           continue
            
 500        continue
         
         endif

c***fourth-order dissipation

         if (my_col.eq.2) then

            do 520 j = 2, nym1

               eta = dble(j-1)*dnym1
            
               do 530 i = 2, nxm1
               
                  xi = dble(i-1)*dnxm1
               
                  k = 2
                  zeta = dble(k-1)*dnzm1
                  call exact (xi,eta,zeta,dtemp2)

                  k = 3
                  zeta = dble(k-1)*dnzm1
                  call exact (xi,eta,zeta,dtemp3)
               
                  k = 4
                  zeta = dble(k-1)*dnzm1
                  call exact (xi,eta,zeta,dtemp4)
               
                  frct(i,j,1) = frct(i,j,1)-dssp*(5.d0*dtemp2(1)-
     $                   4.d0*dtemp3(1)+dtemp4(1))
                  frct(i,j,2) = frct(i,j,2)-dssp*(5.d0*dtemp2(2)-
     $                   4.d0*dtemp3(2)+dtemp4(2))
                  frct(i,j,3) = frct(i,j,3)-dssp*(5.d0*dtemp2(3)-
     $                   4.d0*dtemp3(3)+dtemp4(3))
                  frct(i,j,4) = frct(i,j,4)-dssp*(5.d0*dtemp2(4)-
     $                   4.d0*dtemp3(4)+dtemp4(4))
                  frct(i,j,5) = frct(i,j,5)-dssp*(5.d0*dtemp2(5)-
     $                   4.d0*dtemp3(5)+dtemp4(5))

 530           continue

 520        continue

         elseif (my_col.eq.3) then
                  
            do 540 j = 2, nym1

               eta = dble(j-1)*dnym1
            
               do 550 i = 2, nxm1
               
                  xi = dble(i-1)*dnxm1
               
                  k = 2
                  zeta = dble(k-1)*dnzm1
                  call exact (xi,eta,zeta,dtemp2)

                  k = 3
                  zeta = dble(k-1)*dnzm1
                  call exact (xi,eta,zeta,dtemp3)
               
                  k = 4
                  zeta = dble(k-1)*dnzm1
                  call exact (xi,eta,zeta,dtemp4)

                  k = 5
                  zeta = dble(k-1)*dnzm1
                  call exact (xi,eta,zeta,dtemp5)
                  
                  frct(i,j,1) = frct(i,j,1)-dssp*(-4.d0*dtemp2(1)+
     $                6.d0*dtemp3(1)-4.d0*dtemp4(1)+dtemp5(1))
                  frct(i,j,2) = frct(i,j,2)-dssp*(-4.d0*dtemp2(2)+
     $                6.d0*dtemp3(2)-4.d0*dtemp4(2)+dtemp5(2))
                  frct(i,j,3) = frct(i,j,3)-dssp*(-4.d0*dtemp2(3)+
     $                6.d0*dtemp3(3)-4.d0*dtemp4(3)+dtemp5(3))
                  frct(i,j,4) = frct(i,j,4)-dssp*(-4.d0*dtemp2(4)+
     $                6.d0*dtemp3(4)-4.d0*dtemp4(4)+dtemp5(4))
                  frct(i,j,5) = frct(i,j,5)-dssp*(-4.d0*dtemp2(5)+
     $                6.d0*dtemp3(5)-4.d0*dtemp4(5)+dtemp5(5))
                  
 550           continue

 540        continue

         elseif (my_col.eq.ndcolm2) then

            do 580 j = 2, nym1

               eta = dble(j-1)*dnym1
            
               do 590 i = 2, nxm1
               
                  xi = dble(i-1)*dnxm1
               
                  k = nzm4
                  zeta = dble(k-1)*dnzm1
                  call exact (xi,eta,zeta,dtemp4)

                  k = nzm3
                  zeta = dble(k-1)*dnzm1
                  call exact (xi,eta,zeta,dtemp3)

                  k = nzm2
                  zeta = dble(k-1)*dnzm1
                  call exact (xi,eta,zeta,dtemp2)

                  k = nzm1
                  zeta = dble(k-1)*dnzm1
                  call exact (xi,eta,zeta,dtemp1)

                  frct(i,j,1) = frct(i,j,1)-dssp*(dtemp4(1)-
     $                 4.d0*dtemp3(1)+6.d0*dtemp2(1)-4.d0*dtemp1(1))
                  frct(i,j,2) = frct(i,j,2)-dssp*(dtemp4(2)-
     $                 4.d0*dtemp3(2)+6.d0*dtemp2(2)-4.d0*dtemp1(2))
                  frct(i,j,3) = frct(i,j,3)-dssp*(dtemp4(3)-
     $                 4.d0*dtemp3(3)+6.d0*dtemp2(3)-4.d0*dtemp1(3))
                  frct(i,j,4) = frct(i,j,4)-dssp*(dtemp4(4)-
     $                 4.d0*dtemp3(4)+6.d0*dtemp2(4)-4.d0*dtemp1(4))
                  frct(i,j,5) = frct(i,j,5)-dssp*(dtemp4(5)-
     $                 4.d0*dtemp3(5)+6.d0*dtemp2(5)-4.d0*dtemp1(5))

 590           continue

 580        continue

         elseif (my_col.eq.ndcolm1) then
                  
            do 600 j = 2, nym1

               eta = dble(j-1)*dnym1
            
               do 610 i = 2, nxm1
               
                  xi = dble(i-1)*dnxm1
               
                  k = nzm3
                  zeta = dble(k-1)*dnzm1
                  call exact (xi,eta,zeta,dtemp3)

                  k = nzm2
                  zeta = dble(k-1)*dnzm1
                  call exact (xi,eta,zeta,dtemp2)

                  k = nzm1
                  zeta = dble(k-1)*dnzm1
                  call exact (xi,eta,zeta,dtemp1)

                  frct(i,j,1) = frct(i,j,1)-dssp*(dtemp3(1)-
     $                 4.d0*dtemp2(1)+5.d0*dtemp1(1))
                  frct(i,j,2) = frct(i,j,2)-dssp*(dtemp3(2)-
     $                 4.d0*dtemp2(2)+5.d0*dtemp1(2))
                  frct(i,j,3) = frct(i,j,3)-dssp*(dtemp3(3)-
     $                 4.d0*dtemp2(3)+5.d0*dtemp1(3))
                  frct(i,j,4) = frct(i,j,4)-dssp*(dtemp3(4)-
     $                 4.d0*dtemp2(4)+5.d0*dtemp1(4))
                  frct(i,j,5) = frct(i,j,5)-dssp*(dtemp3(5)-
     $                 4.d0*dtemp2(5)+5.d0*dtemp1(5))
                  
 610           continue
            
 600        continue
         
         elseif ((my_col.gt.3).and.(my_col.lt.ndcolm2)) then
                  
            knode = my_col

            km1   = knode-1
            km2   = knode-2
            kp1   = knode+1
            kp2   = knode+2
            
            do 560 j = 2, nym1

               eta = dble(j-1)*dnym1
            
               do 570 i = 2, nxm1
               
                  xi = dble(i-1)*dnxm1
               
                  k = km2
                  zeta = dble(k-1)*dnzm1
                  call exact (xi,eta,zeta,dtemp1)

                  k = km1
                  zeta = dble(k-1)*dnzm1
                  call exact (xi,eta,zeta,dtemp2)

                  k = knode
                  zeta = dble(k-1)*dnzm1
                  call exact (xi,eta,zeta,dtemp3)

                  k = kp1
                  zeta = dble(k-1)*dnzm1
                  call exact (xi,eta,zeta,dtemp4)

                  k = kp2
                  zeta = dble(k-1)*dnzm1
                  call exact (xi,eta,zeta,dtemp5)

                  frct(i,j,1) = frct(i,j,1)-dssp*(dtemp1(1)-
     &                 4.d0*dtemp2(1)+ 6.d0*dtemp3(1)-
     $                 4.d0*dtemp4(1)+dtemp5(1))
                  frct(i,j,2) = frct(i,j,2)-dssp*(dtemp1(2)-
     &                 4.d0*dtemp2(2)+ 6.d0*dtemp3(2)-
     $                 4.d0*dtemp4(2)+dtemp5(2))
                  frct(i,j,3) = frct(i,j,3)-dssp*(dtemp1(3)-
     &                 4.d0*dtemp2(3)+6.d0*dtemp3(3)-
     $                 4.d0*dtemp4(3)+dtemp5(3))
                  frct(i,j,4) = frct(i,j,4)-dssp*(dtemp1(4)-
     &                 4.d0*dtemp2(4)+6.d0*dtemp3(4)-
     $                 4.d0*dtemp4(4)+dtemp5(4))
                  frct(i,j,5) = frct(i,j,5)-dssp*(dtemp1(5)-
     &                 4.d0*dtemp2(5)+6.d0*dtemp3(5)-
     $                 4.d0*dtemp4(5)+dtemp5(5))
                     
 570           continue

 560        continue

         endif

         return

         end

C******************************************************************************C
C******************************************************************************C
