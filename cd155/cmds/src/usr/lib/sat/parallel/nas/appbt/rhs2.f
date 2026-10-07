C******************************************************************************C
C******************************************************************************C
C***                      compute the right hand sides                      ***C
C******************************************************************************C
C******************************************************************************C
c*** Try to overlap communication with computation by psot a receive, send,
c*** compute, and receive by msgwait.

        subroutine rhs2

        include 'appbt.incl'

C******************************************************************************C
C******************************************************************************C
C*** First prepare u to be sent to +/-1 and +/-2 neighbors.

        do 1020 k = 1, nznriv

           ksd = kk_pointer + k

           do 1030 i = 1, nx

              utsd(i,k,1) = u(i,ksd,1)
              utsd(i,k,2) = u(i,ksd,2)
              utsd(i,k,3) = u(i,ksd,3)
              utsd(i,k,4) = u(i,ksd,4)
              utsd(i,k,5) = u(i,ksd,5)
              utsd(i,k,6) = uiv(i,ksd)

 1030      continue

 1020   continue

C******************************************************************************C
C******************************************************************************C
c*** Phase 1 of 4: send u to +1 right neighbor and post a receive for u01
c*** which is stored in utrc from -1 left neighbor.

        time0 = dclock()

        msgtp_lf1 = numtype2 + 10

        if (my_col.gt.1) then
           msgid_lf1 = irecv (msgtp_lf1,utrc,nbytes_plane6)
        endif
        
        call gsync ()
        
        if (my_col.lt.node_col) then
           call csend (msgtp_lf1,utsd,nbytes_plane6,node_plus1,my_pid)
        endif

        time1 = dclock()-time0
        
        time_xch = time_xch + time1
        txchng1  = txchng1 + time1
        
C******************************************************************************C
C******************************************************************************C
c*** After posting a receive and complete the send, each node perform their
c*** own tasks.

        do 20 k = kk_begin, kk_end
           do 30 i = 1, nx
              rsd(i,k,1) = -frct2(i,k,1)
              rsd(i,k,2) = -frct2(i,k,2)
              rsd(i,k,3) = -frct2(i,k,3)
              rsd(i,k,4) = -frct2(i,k,4)
              rsd(i,k,5) = -frct2(i,k,5)
 30        continue
 20     continue

C******************************************************************************C
C******************************************************************************C
C***                      xi-direction flux differences                     ***C
C******************************************************************************C
C******************************************************************************C

        if ((my_col.gt.1).and.(my_col.lt.node_col)) then

           do 100 k = kbeg_loop, kend_loop

              do 110 i = 1, nx

                 buff1(i) = 2.d0*uiv(i,k)*qu(i,k)
                 buff2(i) = uiv(i,k)*u(i,k,2)
                 buff3(i) = uiv(i,k)*u(i,k,3)
                 buff4(i) = uiv(i,k)*u(i,k,4)
                 buff5(i) = uiv(i,k)*u(i,k,5)
                 
                 cuff(i)  = buff2(i)*buff2(i)

                 save2(i) = buff2(i)*u(i,k,2)+c2*(u(i,k,5)-qu(i,k))
                 save3(i) = buff2(i)*u(i,k,3)
                 save4(i) = buff2(i)*u(i,k,4)
                 save5(i) = buff2(i)*(c1*u(i,k,5)-c2*qu(i,k))

 110          continue

              do 120 i = 2, nxm1
                 
                 im1 = i-1
                 ip1 = i+1
                 
                 rsd(i,k,2) = rsd(i,k,2)-tx2*(
     &                save2(ip1)-save2(im1))+
     &                xxcon1*(buff2(ip1)-2.d0*buff2(i)+buff2(im1))
                 
                 rsd(i,k,3) = rsd(i,k,3)-tx2*(
     &                save3(ip1)-save3(im1))+
     &                xxcon2*(buff3(ip1)-2.d0*buff3(i)+buff3(im1))
                 
                 rsd(i,k,4) = rsd(i,k,4)-tx2*(
     &                save4(ip1)-save4(im1))+
     &                xxcon2*(buff4(ip1)-2.d0*buff4(i)+buff4(im1))
                 
                 rsd(i,k,5) = rsd(i,k,5)-tx2*(
     &                save5(ip1)-save5(im1))+(
     &                xxcon3*(buff1(ip1)-2.d0*buff1(i)+buff1(im1))+
     &                xxcon4*(cuff(ip1)-2.d0*cuff(i)+cuff(im1))+
     &                xxcon5*(buff5(ip1)-2.d0*buff5(i)+buff5(im1)))
                 
 120          continue

 100       continue

           do 105 k = kbeg_loop, kend_loop
              do 125 i = 2, nxm1
                 
                 im1 = i-1
                 ip1 = i+1
                 
                 rsd(i,k,1) = rsd(i,k,1)-tx2*(
     &                u(ip1,k,2)-u(im1,k,2))+
     &                dx1tx1*(u(im1,k,1)-2.d0*u(i,k,1)+u(ip1,k,1))
                 
                 rsd(i,k,2) = rsd(i,k,2)+
     &                dx2tx1*(u(im1,k,2)-2.d0*u(i,k,2)+u(ip1,k,2))
                 
                 rsd(i,k,3) = rsd(i,k,3)+
     &                dx3tx1*(u(im1,k,3)-2.d0*u(i,k,3)+u(ip1,k,3))
                 
                 rsd(i,k,4) = rsd(i,k,4)+
     &                dx4tx1*(u(im1,k,4)-2.d0*u(i,k,4)+u(ip1,k,4))
                 
                 rsd(i,k,5) = rsd(i,k,5)+
     &                dx5tx1*(u(im1,k,5)-2.d0*u(i,k,5)+u(ip1,k,5))
                 
 125          continue
 105       continue

c***Fourth-order dissipation

           do 140 k = kbeg_loop, kend_loop
              
              rsd(2,k,1) = rsd(2,k,1)-dssp*(5.d0*u(2,k,1)-
     &             4.d0*u(3,k,1)+u(4,k,1))
              rsd(2,k,2) = rsd(2,k,2)-dssp*(5.d0*u(2,k,2)-
     &             4.d0*u(3,k,2)+u(4,k,2))
              rsd(2,k,3) = rsd(2,k,3)-dssp*(5.d0*u(2,k,3)-
     &             4.d0*u(3,k,3)+u(4,k,3))
              rsd(2,k,4) = rsd(2,k,4)-dssp*(5.d0*u(2,k,4)-
     &             4.d0*u(3,k,4)+u(4,k,4))
              rsd(2,k,5) = rsd(2,k,5)-dssp*(5.d0*u(2,k,5)-
     &             4.d0*u(3,k,5)+u(4,k,5))

              rsd(3,k,1) = rsd(3,k,1)-dssp*(-4.d0*u(2,k,1)+
     &             6.d0*u(3,k,1)-4.d0*u(4,k,1)+u(5,k,1))
              rsd(3,k,2) = rsd(3,k,2)-dssp*(-4.d0*u(2,k,2)+
     &             6.d0*u(3,k,2)-4.d0*u(4,k,2)+u(5,k,2))
              rsd(3,k,3) = rsd(3,k,3)-dssp*(-4.d0*u(2,k,3)+
     &             6.d0*u(3,k,3)-4.d0*u(4,k,3)+u(5,k,3))
              rsd(3,k,4) = rsd(3,k,4)-dssp*(-4.d0*u(2,k,4)+
     &             6.d0*u(3,k,4)-4.d0*u(4,k,4)+u(5,k,4))
              rsd(3,k,5) = rsd(3,k,5)-dssp*(-4.d0*u(2,k,5)+
     &             6.d0*u(3,k,5)-4.d0*u(4,k,5)+u(5,k,5))
              
              rsd(nxm2,k,1) = rsd(nxm2,k,1)-dssp*(u(nxm4,k,1)-
     &             4.d0*u(nxm3,k,1)+6.d0*u(nxm2,k,1)-
     &             4.d0*u(nxm1,k,1))
              rsd(nxm2,k,2) = rsd(nxm2,k,2)-dssp*(u(nxm4,k,2)-
     &             4.d0*u(nxm3,k,2)+6.d0*u(nxm2,k,2)-
     &             4.d0*u(nxm1,k,2))
              rsd(nxm2,k,3) = rsd(nxm2,k,3)-dssp*(u(nxm4,k,3)-
     &             4.d0*u(nxm3,k,3)+6.d0*u(nxm2,k,3)-
     &             4.d0*u(nxm1,k,3))
              rsd(nxm2,k,4) = rsd(nxm2,k,4)-dssp*(u(nxm4,k,4)-
     &             4.d0*u(nxm3,k,4)+6.d0*u(nxm2,k,4)-
     &             4.d0*u(nxm1,k,4))
              rsd(nxm2,k,5) = rsd(nxm2,k,5)-dssp*(u(nxm4,k,5)-
     &             4.d0*u(nxm3,k,5)+6.d0*u(nxm2,k,5)-
     &             4.d0*u(nxm1,k,5))

              rsd(nxm1,k,1) = rsd(nxm1,k,1)-dssp*(u(nxm3,k,1)-
     &             4.d0*u(nxm2,k,1)+5.d0*u(nxm1,k,1))
              rsd(nxm1,k,2) = rsd(nxm1,k,2)-dssp*(u(nxm3,k,2)-
     &             4.d0*u(nxm2,k,2)+5.d0*u(nxm1,k,2))
              rsd(nxm1,k,3) = rsd(nxm1,k,3)-dssp*(u(nxm3,k,3)-
     &             4.d0*u(nxm2,k,3)+5.d0*u(nxm1,k,3))
              rsd(nxm1,k,4) = rsd(nxm1,k,4)-dssp*(u(nxm3,k,4)-
     &             4.d0*u(nxm2,k,4)+5.d0*u(nxm1,k,4))
              rsd(nxm1,k,5) = rsd(nxm1,k,5)-dssp*(u(nxm3,k,5)-
     &             4.d0*u(nxm2,k,5)+5.d0*u(nxm1,k,5))
                 
 140       continue

           do 160 k = kbeg_loop, kend_loop
              do 170 i = 4, nxm3

                 im1 = i-1
                 im2 = i-2
                 ip1 = i+1
                 ip2 = i+2

                 rsd(i,k,1) = rsd(i,k,1)-dssp*(u(im2,k,1)-
     &                4.d0*u(im1,k,1)+6.d0*u(i,k,1)-
     &                4.d0*u(ip1,k,1)+u(ip2,k,1))
                 rsd(i,k,2) = rsd(i,k,2)-dssp*(u(im2,k,2)-
     &                4.d0*u(im1,k,2)+6.d0*u(i,k,2)-
     &                4.d0*u(ip1,k,2)+u(ip2,k,2))
                 rsd(i,k,3) = rsd(i,k,3)-dssp*(u(im2,k,3)-
     &                4.d0*u(im1,k,3)+6.d0*u(i,k,3)-
     &                4.d0*u(ip1,k,3)+u(ip2,k,3))
                 rsd(i,k,4) = rsd(i,k,4)-dssp*(u(im2,k,4)-
     &                4.d0*u(im1,k,4)+6.d0*u(i,k,4)-
     &                4.d0*u(ip1,k,4)+u(ip2,k,4))
                 rsd(i,k,5) = rsd(i,k,5)-dssp*(u(im2,k,5)-
     &                4.d0*u(im1,k,5)+6.d0*u(i,k,5)-
     &                4.d0*u(ip1,k,5)+u(i+2,k,5))

 170          continue
 160       continue

        endif

C******************************************************************************C
C******************************************************************************C
C***                      eta-direction flux differences                    ***C
C******************************************************************************C
C******************************************************************************C
c*** End phase 1 of 4: receive u01 which is stored in utrc from -1
c*** left neighbor.

        time2 = dclock()
        if (my_col.gt.1) call msgwait (msgid_lf1)
        time3 = dclock()-time2
        
        time_xch  = time_xch + time3
        time_wait = time_wait + time3
        txchng1   = txchng1 + time3
        
        if ((my_col.gt.1).and.(my_col.lt.node_col)) then
           
           do 210 k = kbeg_loop, kend_loop

              krc = k - kk_pointer

              do 230 i = 2, nxm1

                 bwfi2 = utrc(i,krc,6)*utrc(i,krc,2)
                 bwfi3 = utrc(i,krc,6)*utrc(i,krc,3)
                 bwfi4 = utrc(i,krc,6)*utrc(i,krc,4)
                 bwfi5 = utrc(i,krc,6)*utrc(i,krc,5)
                 
                 cwfi1 = bwfi3*bwfi3
                 bwfi1 = bwfi2*bwfi2+cwfi1+bwfi4*bwfi4

                 dufi2 = uiv(i,k)*u(i,k,2)
                 dufi3 = uiv(i,k)*u(i,k,3)
                 dufi4 = uiv(i,k)*u(i,k,4)
                 dufi5 = uiv(i,k)*u(i,k,5)
                 
                 cufi1 = dufi3*dufi3
                 dufi1 = dufi2*dufi2+cufi1+dufi4*dufi4

                 qm1 = 0.5d0*(bwfi2*utrc(i,krc,2)+
     &                bwfi3*utrc(i,krc,3)+bwfi4*utrc(i,krc,4))
                 
                 rsd(i,k,2) = rsd(i,k,2)-ty2*(
     &                -utrc(i,krc,2)*bwfi3)+
     &                yycon2*(-2.d0*dufi2+bwfi2)
                 
                 rsd(i,k,3) = rsd(i,k,3)-ty2*(-
     &                (utrc(i,krc,3)*bwfi3+c2*(utrc(i,krc,5)-qm1)))+
     &                yycon1*(-2.d0*dufi3+bwfi3)
                 
                 rsd(i,k,4) = rsd(i,k,4)-ty2*(
     &                -utrc(i,krc,4)*bwfi3)+
     &                yycon2*(-2.d0*dufi4+bwfi4)
                 
                 rsd(i,k,5) = rsd(i,k,5)-ty2*(-
     &                (bwfi3*(c1*utrc(i,krc,5)-c2*qm1)))+
     &                yycon3*(-2.d0*dufi1+bwfi1)+
     &                yycon4*(-2.d0*cufi1+cwfi1)+
     &                yycon5*(-2.d0*dufi5+bwfi5)
                 
 230          continue

 210       continue

           do 215 k = kbeg_loop, kend_loop

              krc = k - kk_pointer

              do 235 i = 2, nxm1

                 rsd(i,k,1) = rsd(i,k,1)-ty2*(
     &                -utrc(i,krc,3))+
     &                dy1ty1*(utrc(i,krc,1)-2.d0*u(i,k,1))
                 
                 rsd(i,k,2) = rsd(i,k,2)+
     &                dy2ty1*(utrc(i,krc,2)-2.d0*u(i,k,2))
                 
                 rsd(i,k,3) = rsd(i,k,3)+
     &                dy3ty1*(utrc(i,krc,3)-2.d0*u(i,k,3))
                 
                 rsd(i,k,4) = rsd(i,k,4)+
     &                dy4ty1*(utrc(i,krc,4)-2.d0*u(i,k,4))
                 
                 rsd(i,k,5) = rsd(i,k,5)+
     &                dy5ty1*(utrc(i,krc,5)-2.d0*u(i,k,5))
                 
 235          continue
 215       continue

        endif

c***fourth-order dissipation

        if (my_col.eq.2) then

           do 250 k = kbeg_loop, kend_loop
              do 260 i = 2, nxm1

                 rsd(i,k,1) = rsd(i,k,1)-c5dssp*u(i,k,1)
                 rsd(i,k,2) = rsd(i,k,2)-c5dssp*u(i,k,2)
                 rsd(i,k,3) = rsd(i,k,3)-c5dssp*u(i,k,3)
                 rsd(i,k,4) = rsd(i,k,4)-c5dssp*u(i,k,4)
                 rsd(i,k,5) = rsd(i,k,5)-c5dssp*u(i,k,5)

 260          continue
 250       continue

        elseif (my_col.eq.ndcolm1) then
           
           do 370 k = kbeg_loop, kend_loop

              krc = k - kk_pointer

              do 380 i = 2,nxm1

                 rsd(i,k,1) = rsd(i,k,1)-dssp*(-4.d0*utrc(i,krc,1)+
     &                5.d0*u(i,k,1))
                 rsd(i,k,2) = rsd(i,k,2)-dssp*(-4.d0*utrc(i,krc,2)+
     &                5.d0*u(i,k,2))
                 rsd(i,k,3) = rsd(i,k,3)-dssp*(-4.d0*utrc(i,krc,3)+
     &                5.d0*u(i,k,3))
                 rsd(i,k,4) = rsd(i,k,4)-dssp*(-4.d0*utrc(i,krc,4)+
     &                5.d0*u(i,k,4))
                 rsd(i,k,5) = rsd(i,k,5)-dssp*(-4.d0*utrc(i,krc,5)+
     &                5.d0*u(i,k,5))

 380          continue

 370       continue
           
        elseif ((my_col.gt.2).and.(my_col.lt.ndcolm1)) then

           do 310 k = kbeg_loop, kend_loop

              krc = k - kk_pointer

              do 320 i = 2,nxm1

                 rsd(i,k,1) = rsd(i,k,1)-dssp*(-4.d0*utrc(i,krc,1)+
     &                6.d0*u(i,k,1))
                 rsd(i,k,2) = rsd(i,k,2)-dssp*(-4.d0*utrc(i,krc,2)+
     &                6.d0*u(i,k,2))
                 rsd(i,k,3) = rsd(i,k,3)-dssp*(-4.d0*utrc(i,krc,3)+
     &                6.d0*u(i,k,3))
                 rsd(i,k,4) = rsd(i,k,4)-dssp*(-4.d0*utrc(i,krc,4)+
     &                6.d0*u(i,k,4))
                 rsd(i,k,5) = rsd(i,k,5)-dssp*(-4.d0*utrc(i,krc,5)+
     &                6.d0*u(i,k,5))

 320          continue

 310       continue

        endif

C******************************************************************************C
C******************************************************************************C
c*** Phase 2 of 4: send u to -1 left neighbor and post a receive for utrc which
c*** is stored in utrc from +1 right neighbor.

        time4 = dclock()

        msgtp_rt1 = numtype2 + 20
        
        if (my_col.lt.node_col) then
           msgid_rt1 = irecv (msgtp_rt1,utrc,nbytes_plane6)
        endif
        
        call gsync ()
        
        if (my_col.gt.1) then
           call csend (msgtp_rt1,utsd,nbytes_plane6,node_minus1,my_pid)
        endif

        time5 = dclock()-time4
        
        time_xch = time_xch + time5
        txchng1  = txchng1 + time5

C******************************************************************************C
C******************************************************************************C
C***                     zeta-direction flux differences                    ***C
C******************************************************************************C
C******************************************************************************C

        if ((my_col.gt.1).and.(my_col.lt.node_col)) then

           do 400 i = 2, nxm1

              do 410 k = kbegm1, kenda1
                 
                 buff1(k) = 2.d0*uiv(i,k)*qu(i,k)
                 buff2(k) = uiv(i,k)*u(i,k,2)
                 buff3(k) = uiv(i,k)*u(i,k,3)
                 buff4(k) = uiv(i,k)*u(i,k,4)
                 buff5(k) = uiv(i,k)*u(i,k,5)
                 
                 cuff(k)  = buff4(k)*buff4(k)

                 save2(k) = buff4(k)*u(i,k,2)
                 save3(k) = buff4(k)*u(i,k,3)
                 save4(k) = buff4(k)*u(i,k,4)+c2*(u(i,k,5)-qu(i,k))
                 save5(k) = buff4(k)*(c1*u(i,k,5)-c2*qu(i,k))

 410          continue

              do 420 k = kbeg_loop, kend_loop
                 
                 km1 = k-1
                 kp1 = k+1

                 rsd(i,k,2) = rsd(i,k,2)-tz2*(
     &                save2(kp1)-save2(km1))+
     &                zzcon2*(buff2(kp1)-2.d0*buff2(k)+buff2(km1))

                 rsd(i,k,3) = rsd(i,k,3)-tz2*(
     &                save3(kp1)-save3(km1))+
     &                zzcon2*(buff3(kp1)-2.d0*buff3(k)+buff3(km1))

                 rsd(i,k,4) = rsd(i,k,4)-tz2*(
     &                save4(kp1)-save4(km1))+
     &                zzcon1*(buff4(kp1)-2.d0*buff4(k)+buff4(km1))

                 rsd(i,k,5) = rsd(i,k,5)-tz2*(
     &                save5(kp1)-save5(km1))+
     &                zzcon3*(buff1(kp1)-2.d0*buff1(k)+buff1(km1))+
     &                zzcon4*(cuff(kp1)-2.d0*cuff(k)+cuff(km1))+
     &                zzcon5*(buff5(kp1)-2.d0*buff5(k)+buff5(km1))
                 
 420          continue
              
 400       continue

c*** new loop with interchanging indices.

           do 425 k = kbeg_loop, kend_loop

              km1 = k-1
              kp1 = k+1

              do 405 i = 2, nxm1

                 rsd(i,k,1) = rsd(i,k,1)-tz2*(
     &                u(i,kp1,4)-u(i,km1,4))+
     &                dz1tz1*(u(i,km1,1)-2.d0*u(i,k,1)+u(i,kp1,1))

                 rsd(i,k,2) = rsd(i,k,2)+
     &                dz2tz1*(u(i,km1,2)-2.d0*u(i,k,2)+u(i,kp1,2))

                 rsd(i,k,3) = rsd(i,k,3)+
     &                dz3tz1*(u(i,km1,3)-2.d0*u(i,k,3)+u(i,kp1,3))

                 rsd(i,k,4) = rsd(i,k,4)+
     &                dz4tz1*(u(i,km1,4)-2.d0*u(i,k,4)+u(i,kp1,4))

                 rsd(i,k,5) = rsd(i,k,5)+
     &                dz5tz1*(u(i,km1,5)-2.d0*u(i,k,5)+u(i,kp1,5))
                 
 405          continue

 425       continue

        endif

C******************************************************************************C
C******************************************************************************C
C***                      eta-direction flux differences                    ***C
C******************************************************************************C
C******************************************************************************C
c*** End phase 2 of 4: receive utrc which is stored in utrc from +1
c*** right neighbor:

        time6 = dclock()
        if (my_col.lt.node_col) call msgwait (msgid_rt1)
        time7 = dclock()-time6
        
        time_xch  = time_xch + time7
        time_wait = time_wait + time7
        txchng1   = txchng1 + time7
        
        if ((my_col.gt.1).and.(my_col.lt.node_col)) then
           
           do 1210 k = kbeg_loop, kend_loop
              
              krc = k - kk_pointer

              do 1230 i = 2, nxm1

                 bzfi2 = utrc(i,krc,6)*utrc(i,krc,2)
                 bzfi3 = utrc(i,krc,6)*utrc(i,krc,3)
                 bzfi4 = utrc(i,krc,6)*utrc(i,krc,4)
                 bzfi5 = utrc(i,krc,6)*utrc(i,krc,5)

                 czfi  = bzfi3*bzfi3
                 bzfi1 = bzfi2*bzfi2+czfi+bzfi4*bzfi4
                 
                 qp1 = 0.5d0*(bzfi2*utrc(i,krc,2)+
     &                bzfi3*utrc(i,krc,3)+bzfi4*utrc(i,krc,4))
                 
                 rsd(i,k,1) = rsd(i,k,1)-ty2*utrc(i,krc,3)+
     &                dy1ty1*utrc(i,krc,1)
                 
                 rsd(i,k,2) = rsd(i,k,2)-ty2*utrc(i,krc,2)*bzfi3+
     &                yycon2*bzfi2+dy2ty1*utrc(i,krc,2)
                 
                 rsd(i,k,3) = rsd(i,k,3)-ty2*(
     &                (utrc(i,krc,3)*bzfi3+c2*(utrc(i,krc,5)-qp1)))+
     &                yycon1*bzfi3+dy3ty1*utrc(i,krc,3)
                 
                 rsd(i,k,4) = rsd(i,k,4)-ty2*utrc(i,krc,4)*bzfi3+
     &                yycon2*bzfi4+dy4ty1*utrc(i,krc,4)
                 
                 rsd(i,k,5) = rsd(i,k,5)-ty2*
     &                (bzfi3*(c1*utrc(i,krc,5)-c2*qp1))+
     &                yycon3*bzfi1+yycon4*czfi+yycon5*bzfi5+
     &                dy5ty1*utrc(i,krc,5)
                 
 1230         continue

 1210      continue

        endif

c***fourth-order dissipation

        if ((my_col.gt.1).and.(my_col.lt.ndcolm1)) then

           do 1310 k = kbeg_loop, kend_loop

              krc = k - kk_pointer

              do 1320 i = 2,nxm1

                 rsd(i,k,1) = rsd(i,k,1)+c4dssp*utrc(i,krc,1)
                 rsd(i,k,2) = rsd(i,k,2)+c4dssp*utrc(i,krc,2)
                 rsd(i,k,3) = rsd(i,k,3)+c4dssp*utrc(i,krc,3)
                 rsd(i,k,4) = rsd(i,k,4)+c4dssp*utrc(i,krc,4)
                 rsd(i,k,5) = rsd(i,k,5)+c4dssp*utrc(i,krc,5)

 1320         continue

 1310      continue

        endif

C******************************************************************************C
C******************************************************************************C
c*** phase 3 of 4: send u to +2 right neighbor and post a receive for u02
c*** which is stored in utrc from -2 left neighbor:

        time8 = dclock()

        msgtp_lf2 = numtype2 + 30

        if (my_col.gt.2) then
           msgid_lf2 = irecv (msgtp_lf2,utrc,nbytes_plane5)
        endif

        call gsync ()

        if (my_col.lt.ndcolm1) then
           call csend (msgtp_lf2,utsd,nbytes_plane5,node_plus2,my_pid)
        endif

        time9 = dclock()-time8
        
        time_xch = time_xch + time9
        txchng1  = txchng1 + time9
        
C******************************************************************************C
C******************************************************************************C
C***                     zeta-direction flux differences                    ***C
C******************************************************************************C
C******************************************************************************C
c***fourth-order dissipation
           
        if ((my_col.gt.1).and.(my_col.lt.node_col)) then

           if (my_row.eq.1) then 

              do 440 i = 2, nxm1

                 rsd(i,2,1) = rsd(i,2,1)-dssp*(5.d0*u(i,2,1)-
     &                4.d0*u(i,3,1)+u(i,4,1))
                 rsd(i,2,2) = rsd(i,2,2)-dssp*(5.d0*u(i,2,2)-
     &                4.d0*u(i,3,2)+u(i,4,2))
                 rsd(i,2,3) = rsd(i,2,3)-dssp*(5.d0*u(i,2,3)-
     &                4.d0*u(i,3,3)+u(i,4,3))
                 rsd(i,2,4) = rsd(i,2,4)-dssp*(5.d0*u(i,2,4)-
     &                4.d0*u(i,3,4)+u(i,4,4))
                 rsd(i,2,5) = rsd(i,2,5)-dssp*(5.d0*u(i,2,5)-
     &                4.d0*u(i,3,5)+u(i,4,5))

                 rsd(i,3,1) = rsd(i,3,1)-dssp*(-4.d0*u(i,2,1)+
     &                6.d0*u(i,3,1)-4.d0*u(i,4,1)+u(i,5,1))
                 rsd(i,3,2) = rsd(i,3,2)-dssp*(-4.d0*u(i,2,2)+
     &                6.d0*u(i,3,2)-4.d0*u(i,4,2)+u(i,5,2))
                 rsd(i,3,3) = rsd(i,3,3)-dssp*(-4.d0*u(i,2,3)+
     &                6.d0*u(i,3,3)-4.d0*u(i,4,3)+u(i,5,3))
                 rsd(i,3,4) = rsd(i,3,4)-dssp*(-4.d0*u(i,2,4)+
     &                6.d0*u(i,3,4)-4.d0*u(i,4,4)+u(i,5,4))
                 rsd(i,3,5) = rsd(i,3,5)-dssp*(-4.d0*u(i,2,5)+
     &                6.d0*u(i,3,5)-4.d0*u(i,4,5)+u(i,5,5))

 440          continue
              
           endif

           if (my_row.eq.node_row) then

              do 490 i = 2, nxm1
                 
                 rsd(i,nzm2,1) = rsd(i,nzm2,1)-dssp*(
     &                u(i,nzm4,1)-4.d0*u(i,nzm3,1)+
     &                6.d0*u(i,nzm2,1)-4.d0*u(i,nzm1,1))
                 rsd(i,nzm2,2) = rsd(i,nzm2,2)-dssp*(
     &                u(i,nzm4,2)-4.d0*u(i,nzm3,2)+
     &                6.d0*u(i,nzm2,2)-4.d0*u(i,nzm1,2))
                 rsd(i,nzm2,3) = rsd(i,nzm2,3)-dssp*(
     &                u(i,nzm4,3)-4.d0*u(i,nzm3,3)+
     &                6.d0*u(i,nzm2,3)-4.d0*u(i,nzm1,3))
                 rsd(i,nzm2,4) = rsd(i,nzm2,4)-dssp*(
     &                u(i,nzm4,4)-4.d0*u(i,nzm3,4)+
     &                6.d0*u(i,nzm2,4)-4.d0*u(i,nzm1,4))
                 rsd(i,nzm2,5) = rsd(i,nzm2,5)-dssp*(
     &                u(i,nzm4,5)-4.d0*u(i,nzm3,5)+
     &                6.d0*u(i,nzm2,5)-4.d0*u(i,nzm1,5))
                 
                 rsd(i,nzm1,1) = rsd(i,nzm1,1)-dssp*(
     &                u(i,nzm3,1)-4.d0*u(i,nzm2,1)+
     &                5.d0*u(i,nzm1,1))
                 rsd(i,nzm1,2) = rsd(i,nzm1,2)-dssp*(
     &                u(i,nzm3,2)-4.d0*u(i,nzm2,2)+
     &                5.d0*u(i,nzm1,2))
                 rsd(i,nzm1,3) = rsd(i,nzm1,3)-dssp*(
     &                u(i,nzm3,3)-4.d0*u(i,nzm2,3)+
     &                5.d0*u(i,nzm1,3))
                 rsd(i,nzm1,4) = rsd(i,nzm1,4)-dssp*(
     &                u(i,nzm3,4)-4.d0*u(i,nzm2,4)+
     &                5.d0*u(i,nzm1,4))
                 rsd(i,nzm1,5) = rsd(i,nzm1,5)-dssp*(
     &                u(i,nzm3,5)-4.d0*u(i,nzm2,5)+
     &                5.d0*u(i,nzm1,5))
                 
 490          continue
              
           endif

           do 470 k = kstart, kstop

              km1 = k-1
              km2 = k-2
              kp1 = k+1
              kp2 = k+2

              do 460 i = 2, nxm1

                 rsd(i,k,1) = rsd(i,k,1)-dssp*(u(i,km2,1)-
     &                4.d0*u(i,km1,1)+6.d0*u(i,k,1)-
     &                4.d0*u(i,kp1,1)+u(i,kp2,1))
                 rsd(i,k,2) = rsd(i,k,2)-dssp*(u(i,km2,2)-
     &                4.d0*u(i,km1,2)+6.d0*u(i,k,2)-
     &                4.d0*u(i,kp1,2)+u(i,kp2,2))
                 rsd(i,k,3) = rsd(i,k,3)-dssp*(u(i,km2,3)-
     &                4.d0*u(i,km1,3)+6.d0*u(i,k,3)-
     &                4.d0*u(i,kp1,3)+u(i,kp2,3))
                 rsd(i,k,4) = rsd(i,k,4)-dssp*(u(i,km2,4)-
     &                4.d0*u(i,km1,4)+6.d0*u(i,k,4)-
     &                4.d0*u(i,kp1,4)+u(i,kp2,4))
                 rsd(i,k,5) = rsd(i,k,5)-dssp*(u(i,km2,5)-
     &                4.d0*u(i,km1,5)+6.d0*u(i,k,5)-
     &                4.d0*u(i,kp1,5)+u(i,kp2,5))

 460          continue
              
 470       continue
           
        endif

C******************************************************************************C
C******************************************************************************C
C***                      eta-direction flux differences                    ***C
C******************************************************************************C
C******************************************************************************C
c*** End phase 3 of 4: receive u02 which is stored in utrc from -2 left neighbor:

        time10 = dclock()
        if (my_col.gt.2) call msgwait (msgid_lf2)
        time11 = dclock()-time10
        
        time_xch  = time_xch + time11
        time_wait = time_wait + time11
        txchng1   = txchng1 + time11
        
c***fourth-order dissipation

        if ((my_col.gt.3).and.(my_col.lt.node_col)) then

           do 870 k = kbeg_loop, kend_loop

              krc = k - kk_pointer

              do 880 i = 2, nxm1

                 rsd(i,k,1) = rsd(i,k,1)-dssp*utrc(i,krc,1)
                 rsd(i,k,2) = rsd(i,k,2)-dssp*utrc(i,krc,2)
                 rsd(i,k,3) = rsd(i,k,3)-dssp*utrc(i,krc,3)
                 rsd(i,k,4) = rsd(i,k,4)-dssp*utrc(i,krc,4)
                 rsd(i,k,5) = rsd(i,k,5)-dssp*utrc(i,krc,5)

 880          continue

 870       continue

        endif

C******************************************************************************C
C******************************************************************************C
c*** phase 4 of 4: send u to -2 left neighbor and post a receive for u2 which
c*** is stored in utrc from +2 right neighbor:

        time12 = dclock()

        msgtp_rt2 = numtype2 + 40

        if (my_col.lt.ndcolm1) then
           msgid_rt2 = irecv (msgtp_rt2,utrc,nbytes_plane5)
        endif

        call gsync ()
        
        if (my_col.gt.2) then
           call csend (msgtp_rt2,utsd,nbytes_plane5,node_minus2,my_pid)
        endif
        
        time13 = dclock()-time12
        
        time_xch = time_xch + time13
        txchng1  = txchng1 + time13
        
C******************************************************************************C
C******************************************************************************C
C***                      eta-direction flux differences                    ***C
C******************************************************************************C
C******************************************************************************C
c*** End phase 4 of 4: receive u2 which is stored in utrc from +2 right neighbor.

        time14 = dclock()
        if (my_col.lt.ndcolm1) call msgwait (msgid_rt2)
        time15 = dclock()-time14
        
        time_xch  = time_xch + time15
        time_wait = time_wait + time15
        txchng1   = txchng1 + time15
        
c***fourth-order dissipation

        if ((my_col.gt.1).and.(my_col.lt.ndcolm2)) then

           do 1870 k = kbeg_loop, kend_loop

              krc = k - kk_pointer

              do 1880 i = 2, nxm1

                 rsd(i,k,1) = rsd(i,k,1)-dssp*utrc(i,krc,1)
                 rsd(i,k,2) = rsd(i,k,2)-dssp*utrc(i,krc,2)
                 rsd(i,k,3) = rsd(i,k,3)-dssp*utrc(i,krc,3)
                 rsd(i,k,4) = rsd(i,k,4)-dssp*utrc(i,krc,4)
                 rsd(i,k,5) = rsd(i,k,5)-dssp*utrc(i,krc,5)

 1880         continue

 1870      continue

        endif

        return

        end

C******************************************************************************C
C******************************************************************************C
