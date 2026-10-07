C******************************************************************************C
C******************************************************************************C
C***                      compute the right hand sides                      ***C
C******************************************************************************C
C******************************************************************************C
c*** Try to overlap communication with computation by posting a receive, send,
c*** compute and receive by msgwait.

        subroutine rhs

        include 'appbt.incl'

C******************************************************************************C
C******************************************************************************C
C*** First prepare u to be sent to +/-1 and +/-2 neighbors.

        do 1020 j = 1, nynriv

           jsd = jj_pointer + j

           do 1030 i = 1, nx

              utsd(i,j,1) = u(i,jsd,1)
              utsd(i,j,2) = u(i,jsd,2)
              utsd(i,j,3) = u(i,jsd,3)
              utsd(i,j,4) = u(i,jsd,4)
              utsd(i,j,5) = u(i,jsd,5)
              utsd(i,j,6) = uiv(i,jsd)

 1030      continue

 1020   continue

C******************************************************************************C
C******************************************************************************C
c*** Phase 1 of 4: Each node broadcasts u to +1 right neighbor and post a
c*** receive for u01 which is stored in utrc from left -1 neighbor.

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
c*** After posting a receive for u01, each node can now perform their own
c*** tasks.

        do 20 j = jj_begin, jj_end
           do 30 i = 1, nx

              rsd(i,j,1) = -frct(i,j,1)
              rsd(i,j,2) = -frct(i,j,2)
              rsd(i,j,3) = -frct(i,j,3)
              rsd(i,j,4) = -frct(i,j,4)
              rsd(i,j,5) = -frct(i,j,5)

 30        continue
 20     continue

C******************************************************************************C
C******************************************************************************C
C***                      xi-direction flux differences                     ***C
C******************************************************************************C
C******************************************************************************C
           
        if ((my_col.gt.1).and.(my_col.lt.node_col)) then

           do 110 j = jbeg_loop, jend_loop
              
              do 120 i = 1, nx
                 
                 buff1(i) = 2.d0*uiv(i,j)*qu(i,j)
                 buff2(i) = uiv(i,j)*u(i,j,2)
                 buff3(i) = uiv(i,j)*u(i,j,3)
                 buff4(i) = uiv(i,j)*u(i,j,4)
                 buff5(i) = uiv(i,j)*u(i,j,5)
                 
                 cuff(i)  = buff2(i)*buff2(i)

                 save2(i) = buff2(i)*u(i,j,2)+c2*(u(i,j,5)-qu(i,j))
                 save3(i) = buff2(i)*u(i,j,3)
                 save4(i) = buff2(i)*u(i,j,4)
                 save5(i) = buff2(i)*(c1*u(i,j,5)-c2*qu(i,j))
                 
 120          continue
              
              do 130 i = 2, nxm1

                 im1 = i-1
                 ip1 = i+1

                 rsd(i,j,2) = rsd(i,j,2)-tx2*(
     &                save2(ip1)-save2(im1))+
     &                xxcon1*(buff2(ip1)-2.d0*buff2(i)+buff2(im1))
                 
                 rsd(i,j,3) = rsd(i,j,3)-tx2*(
     &                save3(ip1)-save3(im1))+
     &                xxcon2*(buff3(ip1)-2.d0*buff3(i)+buff3(im1))
                 
                 rsd(i,j,4) = rsd(i,j,4)-tx2*(
     &                save4(ip1)-save4(im1))+
     &                xxcon2*(buff4(ip1)-2.d0*buff4(i)+buff4(im1))
                 
                 rsd(i,j,5) = rsd(i,j,5)-tx2*(
     &                save5(ip1)-save5(im1))+
     &                xxcon3*(buff1(ip1)-2.d0*buff1(i)+buff1(im1))+
     &                xxcon4*(cuff(ip1)-2.d0*cuff(i)+cuff(im1))+
     &                xxcon5*(buff5(ip1)-2.d0*buff5(i)+buff5(im1))
                 
 130          continue
              
 110       continue

           do 115 j = jbeg_loop, jend_loop
              do 135 i = 2, nxm1

                 im1 = i-1
                 ip1 = i+1

                 rsd(i,j,1) = rsd(i,j,1)-tx2*(
     &                u(ip1,j,2)-u(im1,j,2))+
     &                dx1tx1*(u(im1,j,1)-2.d0*u(i,j,1)+u(ip1,j,1))

                 rsd(i,j,2) = rsd(i,j,2)+
     &                dx2tx1*(u(im1,j,2)-2.d0*u(i,j,2)+u(ip1,j,2))
                 
                 rsd(i,j,3) = rsd(i,j,3)+
     &                dx3tx1*(u(im1,j,3)-2.d0*u(i,j,3)+u(ip1,j,3))
                 
                 rsd(i,j,4) = rsd(i,j,4)+
     &                dx4tx1*(u(im1,j,4)-2.d0*u(i,j,4)+u(ip1,j,4))
                 
                 rsd(i,j,5) = rsd(i,j,5)+
     &                dx5tx1*(u(im1,j,5)-2.d0*u(i,j,5)+u(ip1,j,5))
                 
 135          continue
 115       continue

c***Fourth-order dissipation
           
           do 140 j = jbeg_loop, jend_loop

              rsd(2,j,1) = rsd(2,j,1)-dssp*(5.d0*u(2,j,1)-
     &             4.d0*u(3,j,1)+u(4,j,1))
              rsd(2,j,2) = rsd(2,j,2)-dssp*(5.d0*u(2,j,2)-
     &             4.d0*u(3,j,2)+u(4,j,2))
              rsd(2,j,3) = rsd(2,j,3)-dssp*(5.d0*u(2,j,3)-
     &             4.d0*u(3,j,3)+u(4,j,3))
              rsd(2,j,4) = rsd(2,j,4)-dssp*(5.d0*u(2,j,4)-
     &             4.d0*u(3,j,4)+u(4,j,4))
              rsd(2,j,5) = rsd(2,j,5)-dssp*(5.d0*u(2,j,5)-
     &             4.d0*u(3,j,5)+u(4,j,5))

              rsd(3,j,1) = rsd(3,j,1)-dssp*(-4.d0*u(2,j,1)+
     &             6.d0*u(3,j,1)-4.d0*u(4,j,1)+u(5,j,1))
              rsd(3,j,2) = rsd(3,j,2)-dssp*(-4.d0*u(2,j,2)+
     &             6.d0*u(3,j,2)-4.d0*u(4,j,2)+u(5,j,2))
              rsd(3,j,3) = rsd(3,j,3)-dssp*(-4.d0*u(2,j,3)+
     &             6.d0*u(3,j,3)-4.d0*u(4,j,3)+u(5,j,3))
              rsd(3,j,4) = rsd(3,j,4)-dssp*(-4.d0*u(2,j,4)+
     &             6.d0*u(3,j,4)-4.d0*u(4,j,4)+u(5,j,4))
              rsd(3,j,5) = rsd(3,j,5)-dssp*(-4.d0*u(2,j,5)+
     &             6.d0*u(3,j,5)-4.d0*u(4,j,5)+u(5,j,5))
              
 140       continue

           do 150 j = jbeg_loop, jend_loop
              
              rsd(nxm2,j,1) = rsd(nxm2,j,1)-dssp*(u(nxm4,j,1)-
     &             4.d0*u(nxm3,j,1)+6.d0*u(nxm2,j,1)-
     &             4.d0*u(nxm1,j,1))
              rsd(nxm2,j,2) = rsd(nxm2,j,2)-dssp*(u(nxm4,j,2)-
     &             4.d0*u(nxm3,j,2)+6.d0*u(nxm2,j,2)-
     &             4.d0*u(nxm1,j,2))
              rsd(nxm2,j,3) = rsd(nxm2,j,3)-dssp*(u(nxm4,j,3)-
     &             4.d0*u(nxm3,j,3)+6.d0*u(nxm2,j,3)-
     &             4.d0*u(nxm1,j,3))
              rsd(nxm2,j,4) = rsd(nxm2,j,4)-dssp*(u(nxm4,j,4)-
     &             4.d0*u(nxm3,j,4)+6.d0*u(nxm2,j,4)-
     &             4.d0*u(nxm1,j,4))
              rsd(nxm2,j,5) = rsd(nxm2,j,5)-dssp*(u(nxm4,j,5)-
     &             4.d0*u(nxm3,j,5)+6.d0*u(nxm2,j,5)-
     &             4.d0*u(nxm1,j,5))

              rsd(nxm1,j,1) = rsd(nxm1,j,1)-dssp*(u(nxm3,j,1)-
     &             4.d0*u(nxm2,j,1)+5.d0*u(nxm1,j,1))
              rsd(nxm1,j,2) = rsd(nxm1,j,2)-dssp*(u(nxm3,j,2)-
     &             4.d0*u(nxm2,j,2)+5.d0*u(nxm1,j,2))
              rsd(nxm1,j,3) = rsd(nxm1,j,3)-dssp*(u(nxm3,j,3)-
     &             4.d0*u(nxm2,j,3)+5.d0*u(nxm1,j,3))
              rsd(nxm1,j,4) = rsd(nxm1,j,4)-dssp*(u(nxm3,j,4)-
     &             4.d0*u(nxm2,j,4)+5.d0*u(nxm1,j,4))
              rsd(nxm1,j,5) = rsd(nxm1,j,5)-dssp*(u(nxm3,j,5)-
     &             4.d0*u(nxm2,j,5)+5.d0*u(nxm1,j,5))
              
 150       continue

           do 170 j = jbeg_loop, jend_loop
              do 180 i = 4, nxm3

                 im1 = i-1
                 im2 = i-2
                 ip1 = i+1
                 ip2 = i+2

                 rsd(i,j,1) = rsd(i,j,1)-dssp*(u(im2,j,1)-
     &                4.d0*u(im1,j,1)+6.d0*u(i,j,1)-
     &                4.d0*u(ip1,j,1)+u(ip2,j,1))
                 rsd(i,j,2) = rsd(i,j,2)-dssp*(u(im2,j,2)-
     &                4.d0*u(im1,j,2)+6.d0*u(i,j,2)-
     &                4.d0*u(ip1,j,2)+u(ip2,j,2))
                 rsd(i,j,3) = rsd(i,j,3)-dssp*(u(im2,j,3)-
     &                4.d0*u(im1,j,3)+6.d0*u(i,j,3)-
     &                4.d0*u(ip1,j,3)+u(ip2,j,3))
                 rsd(i,j,4) = rsd(i,j,4)-dssp*(u(im2,j,4)-
     &                4.d0*u(im1,j,4)+6.d0*u(i,j,4)-
     &                4.d0*u(ip1,j,4)+u(ip2,j,4))
                 rsd(i,j,5) = rsd(i,j,5)-dssp*(u(im2,j,5)-
     &                4.d0*u(im1,j,5)+6.d0*u(i,j,5)-
     &                4.d0*u(ip1,j,5)+u(ip2,j,5))

 180          continue
 170       continue

        endif

C******************************************************************************C
C******************************************************************************C
C***                      zeta-direction flux differences                   ***C
C******************************************************************************C
C******************************************************************************C
c*** End phase 1 of 4: receive u01 which is stored in utrc from left
c*** -1 neighbor.

        time2 = dclock()

        if (my_col.gt.1) call msgwait (msgid_lf1)

        time3 = dclock()-time2

        time_xch  = time_xch + time3
        time_wait = time_wait + time3
        txchng1   = txchng1 + time3
        
        if ((my_col.gt.1).and.(my_col.lt.node_col)) then

           do 500 j = jbeg_loop, jend_loop

              jrc = j - jj_pointer

              do 520 i = 2, nxm1

                 bwfi2 = utrc(i,jrc,6)*utrc(i,jrc,2)
                 bwfi3 = utrc(i,jrc,6)*utrc(i,jrc,3)
                 bwfi4 = utrc(i,jrc,6)*utrc(i,jrc,4)
                 bwfi5 = utrc(i,jrc,6)*utrc(i,jrc,5)
                 
                 cwfi1 = bwfi4*bwfi4
                 bwfi1 = bwfi2*bwfi2+bwfi3*bwfi3+cwfi1
                 
                 dufi2 = uiv(i,j)*u(i,j,2)
                 dufi3 = uiv(i,j)*u(i,j,3)
                 dufi4 = uiv(i,j)*u(i,j,4)
                 dufi5 = uiv(i,j)*u(i,j,5)
                 
                 cufi1 = dufi4*dufi4
                 dufi1 = dufi2*dufi2+dufi3*dufi3+cufi1
                 
                 qm1 = 0.5d0*(bwfi2*utrc(i,jrc,2)+
     &                bwfi3*utrc(i,jrc,3)+bwfi4*utrc(i,jrc,4))
                 
                 rsd(i,j,2) = rsd(i,j,2)-tz2*(
     &                -utrc(i,jrc,2)*bwfi4)+
     &                zzcon2*(-2.d0*dufi2+bwfi2)

                 rsd(i,j,3) = rsd(i,j,3)-tz2*(
     &                -utrc(i,jrc,3)*bwfi4)+
     &                zzcon2*(-2.d0*dufi3+bwfi3)
                 
                 rsd(i,j,4) = rsd(i,j,4)-tz2*(-
     &                (utrc(i,jrc,4)*bwfi4+c2*(utrc(i,jrc,5)-qm1)))+
     &                zzcon1*(-2.d0*dufi4+bwfi4)
                 
                 rsd(i,j,5) = rsd(i,j,5)-tz2*(-
     &                bwfi4*(c1*utrc(i,jrc,5)-c2*qm1))+
     &                zzcon3*(-2.d0*dufi1+bwfi1)+
     &                zzcon4*(-2.d0*cufi1+cwfi1)+
     &                zzcon5*(-2.d0*dufi5+bwfi5)
                 
 520          continue

 500       continue
           
           do 505 j = jbeg_loop, jend_loop

              jrc = j - jj_pointer

              do 525 i = 2, nxm1
                 
                 rsd(i,j,1) = rsd(i,j,1)-tz2*(
     &                -utrc(i,jrc,4))+
     &                dz1tz1*(utrc(i,jrc,1)-2.d0*u(i,j,1))

                 rsd(i,j,2) = rsd(i,j,2)+
     &                dz2tz1*(utrc(i,jrc,2)-2.d0*u(i,j,2))

                 rsd(i,j,3) = rsd(i,j,3)+
     &                dz3tz1*(utrc(i,jrc,3)-2.d0*u(i,j,3))
                 
                 rsd(i,j,4) = rsd(i,j,4)+
     &                dz4tz1*(utrc(i,jrc,4)-2.d0*u(i,j,4))

                 rsd(i,j,5) = rsd(i,j,5)+
     &                dz5tz1*(utrc(i,jrc,5)-2.d0*u(i,j,5))
                 
 525          continue

 505       continue
           
        endif

c***fourth-order dissipation

        if (my_col.eq.2) then
                 
           do 540 j = jbeg_loop, jend_loop
              do 550 i = 2, nxm1

                 rsd(i,j,1) = rsd(i,j,1)-c5dssp*u(i,j,1)
                 rsd(i,j,2) = rsd(i,j,2)-c5dssp*u(i,j,2)
                 rsd(i,j,3) = rsd(i,j,3)-c5dssp*u(i,j,3)
                 rsd(i,j,4) = rsd(i,j,4)-c5dssp*u(i,j,4)
                 rsd(i,j,5) = rsd(i,j,5)-c5dssp*u(i,j,5)

 550          continue
 540       continue

        elseif (my_col.eq.ndcolm1) then
           
           do 660 j = jbeg_loop, jend_loop

              jrc = j - jj_pointer

              do 670 i = 2, nxm1

                 rsd(i,j,1) = rsd(i,j,1)-dssp*(-4.d0*utrc(i,jrc,1)+
     &                5.d0*u(i,j,1))
                 rsd(i,j,2) = rsd(i,j,2)-dssp*(-4.d0*utrc(i,jrc,2)+
     &                5.d0*u(i,j,2))
                 rsd(i,j,3) = rsd(i,j,3)-dssp*(-4.d0*utrc(i,jrc,3)+
     &                5.d0*u(i,j,3))
                 rsd(i,j,4) = rsd(i,j,4)-dssp*(-4.d0*utrc(i,jrc,4)+
     &                5.d0*u(i,j,4))
                 rsd(i,j,5) = rsd(i,j,5)-dssp*(-4.d0*utrc(i,jrc,5)+
     &                5.d0*u(i,j,5))

 670          continue
 660       continue
           
        elseif ((my_col.gt.2).and.(my_col.lt.ndcolm1)) then
                 
           do 600 j = jbeg_loop, jend_loop

              jrc = j - jj_pointer

              do 610 i = 2, nxm1

                 rsd(i,j,1) = rsd(i,j,1)-dssp*(-4.d0*utrc(i,jrc,1)+
     &                6.d0*u(i,j,1))
                 rsd(i,j,2) = rsd(i,j,2)-dssp*(-4.d0*utrc(i,jrc,2)+
     &                6.d0*u(i,j,2))
                 rsd(i,j,3) = rsd(i,j,3)-dssp*(-4.d0*utrc(i,jrc,3)+
     &                6.d0*u(i,j,3))
                 rsd(i,j,4) = rsd(i,j,4)-dssp*(-4.d0*utrc(i,jrc,4)+
     &                6.d0*u(i,j,4))
                 rsd(i,j,5) = rsd(i,j,5)-dssp*(-4.d0*utrc(i,jrc,5)+
     &                6.d0*u(i,j,5))

 610          continue
 600       continue

        endif
        
C******************************************************************************C
C******************************************************************************C
c*** Phase 2 of 4: Each node broadcasts u to -1 left neighbor and post a 
c*** receive for utrc from right +1 neighbor.

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
C***                      eta-direction flux differences                    ***C
C******************************************************************************C
C******************************************************************************C

        if ((my_col.gt.1).and.(my_col.lt.node_col)) then

           do 300 i = 2, nxm1

              do 310 j = jbegm1, jenda1
           
                 buff1(j) = 2.d0*uiv(i,j)*qu(i,j)
                 buff2(j) = uiv(i,j)*u(i,j,2)
                 buff3(j) = uiv(i,j)*u(i,j,3)
                 buff4(j) = uiv(i,j)*u(i,j,4)
                 buff5(j) = uiv(i,j)*u(i,j,5)
                 
                 cuff(j)  = buff3(j)*buff3(j)

                 save2(j) = buff3(j)*u(i,j,2)
                 save3(j) = buff3(j)*u(i,j,3)+c2*(u(i,j,5)-qu(i,j))
                 save4(j) = buff3(j)*u(i,j,4)
                 save5(j) = buff3(j)*(c1*u(i,j,5)-c2*qu(i,j))

 310          continue

              do 320 j = jbeg_loop, jend_loop
                    
                 jm1 = j-1
                 jp1 = j+1

                 rsd(i,j,2) = rsd(i,j,2)-ty2*(
     &                save2(jp1)-save2(jm1))+
     &                yycon2*(buff2(jp1)-2.d0*buff2(j)+buff2(jm1))

                 rsd(i,j,3) = rsd(i,j,3)-ty2*(
     &                save3(jp1)-save3(jm1))+
     &                yycon1*(buff3(jp1)-2.d0*buff3(j)+buff3(jm1))
                 
                 rsd(i,j,4) = rsd(i,j,4)-ty2*(
     &                save4(jp1)-save4(jm1))+
     &                yycon2*(buff4(jp1)-2.d0*buff4(j)+buff4(jm1))
                 
                 rsd(i,j,5) = rsd(i,j,5)-ty2*(
     &                save5(jp1)-save5(jm1))+
     &                yycon3*(buff1(jp1)-2.d0*buff1(j)+buff1(jm1))+
     &                yycon4*(cuff(jp1)-2.d0*cuff(j)+cuff(jm1))+
     &                yycon5*(buff5(jp1)-2.d0*buff5(j)+buff5(jm1))
                 
 320          continue
              
 300       continue

c*** new loop with interchanging indices.

           do 325 j = jbeg_loop, jend_loop
              
              jm1 = j-1
              jp1 = j+1
              
              do 305 i = 2, nxm1
                 
                 rsd(i,j,1) = rsd(i,j,1)-ty2*(
     &                u(i,jp1,3)-u(i,jm1,3))+
     &                dy1ty1*(u(i,jm1,1)-2.d0*u(i,j,1)+u(i,jp1,1))
                 
                 rsd(i,j,2) = rsd(i,j,2)+
     &                dy2ty1*(u(i,jm1,2)-2.d0*u(i,j,2)+u(i,jp1,2))
                 
                 rsd(i,j,3) = rsd(i,j,3)+
     &                dy3ty1*(u(i,jm1,3)-2.d0*u(i,j,3)+u(i,jp1,3))
                 
                 rsd(i,j,4) = rsd(i,j,4)+
     &                dy4ty1*(u(i,jm1,4)-2.d0*u(i,j,4)+u(i,jp1,4))
                 
                 rsd(i,j,5) = rsd(i,j,5)+
     &                dy5ty1*(u(i,jm1,5)-2.d0*u(i,j,5)+u(i,jp1,5))
                 
 305          continue

 325       continue

        endif

C******************************************************************************C
C******************************************************************************C
C***                      zeta-direction flux differences                   ***C
C******************************************************************************C
C******************************************************************************C
c*** End phase 2 of 4: receive utrc from right +1 neighbor.

        time6 = dclock()
        if (my_col.lt.node_col) call msgwait (msgid_rt1)
        time7 = dclock()-time6

        time_xch  = time_xch + time7
        time_wait = time_wait + time7
        txchng1   = txchng1 + time7
        
        if ((my_col.gt.1).and.(my_col.lt.node_col)) then

           do 1500 j = jbeg_loop, jend_loop
              
              jrc = j - jj_pointer

              do 1520 i = 2, nxm1

                 bzfi2 = utrc(i,jrc,6)*utrc(i,jrc,2)
                 bzfi3 = utrc(i,jrc,6)*utrc(i,jrc,3)
                 bzfi4 = utrc(i,jrc,6)*utrc(i,jrc,4)
                 bzfi5 = utrc(i,jrc,6)*utrc(i,jrc,5)
                 
                 czfi1 = bzfi4*bzfi4
                 bzfi1 = bzfi2*bzfi2+bzfi3*bzfi3+czfi1
                 
                 qp1 = 0.5d0*(bzfi2*utrc(i,jrc,2)+
     &                bzfi3*utrc(i,jrc,3)+bzfi4*utrc(i,jrc,4))

                 rsd(i,j,1) = rsd(i,j,1)-tz2*utrc(i,jrc,4)+
     &                dz1tz1*utrc(i,jrc,1)

                 rsd(i,j,2) = rsd(i,j,2)-tz2*utrc(i,jrc,2)*bzfi4+
     &                zzcon2*bzfi2+dz2tz1*utrc(i,jrc,2)

                 rsd(i,j,3) = rsd(i,j,3)-tz2*utrc(i,jrc,3)*bzfi4+
     &                zzcon2*bzfi3+dz3tz1*utrc(i,jrc,3)
                 
                 rsd(i,j,4) = rsd(i,j,4)-tz2*(
     &                (utrc(i,jrc,4)*bzfi4+c2*(utrc(i,jrc,5)-qp1)))+
     &                zzcon1*bzfi4+dz4tz1*utrc(i,jrc,4)

                 rsd(i,j,5) = rsd(i,j,5)-tz2*(
     &                bzfi4*(c1*utrc(i,jrc,5)-c2*qp1))+
     &                zzcon3*bzfi1+zzcon4*czfi1+zzcon5*bzfi5+
     &                dz5tz1*utrc(i,jrc,5)
                 
 1520         continue

 1500      continue
           
        endif

c***fourth-order dissipation

        if ((my_col.gt.1).and.(my_col.lt.ndcolm1)) then
           
           do 1600 j = jbeg_loop, jend_loop

              jrc = j - jj_pointer

              do 1610 i = 2, nxm1
                 
                 rsd(i,j,1) = rsd(i,j,1)+c4dssp*utrc(i,jrc,1)
                 rsd(i,j,2) = rsd(i,j,2)+c4dssp*utrc(i,jrc,2)
                 rsd(i,j,3) = rsd(i,j,3)+c4dssp*utrc(i,jrc,3)
                 rsd(i,j,4) = rsd(i,j,4)+c4dssp*utrc(i,jrc,4)
                 rsd(i,j,5) = rsd(i,j,5)+c4dssp*utrc(i,jrc,5)

 1610         continue

 1600      continue

        endif
        
C******************************************************************************C
C******************************************************************************C
c*** Phase 3 of 4: Each node broadcasts u to +2 right neighbor and post a
c*** receive for u02 which is stored in utrc from left -2 neighbor.

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
C***                      eta-direction flux differences                    ***C
C******************************************************************************C
C******************************************************************************C
c*** Continuing fourth-order dissipation.

        if ((my_col.gt.1).and.(my_col.lt.node_col)) then

           if (my_row.eq.1) then
                 
              do 340 i = 2, nxm1
                    
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
                    
 340          continue
                 
           endif

c*** Continuing fourth order dissipation.

           if (my_row.eq.node_row) then

              do 380 i = 2, nxm1
                 
                 rsd(i,nym2,1) = rsd(i,nym2,1)-dssp*(u(i,nym4,1)-
     &                4.d0*u(i,nym3,1)+6.d0*u(i,nym2,1)-
     &                4.d0*u(i,nym1,1))
                 rsd(i,nym2,2) = rsd(i,nym2,2)-dssp*(u(i,nym4,2)-
     &                4.d0*u(i,nym3,2)+6.d0*u(i,nym2,2)-
     &                4.d0*u(i,nym1,2))
                 rsd(i,nym2,3) = rsd(i,nym2,3)-dssp*(u(i,nym4,3)-
     &                4.d0*u(i,nym3,3)+6.d0*u(i,nym2,3)-
     &                4.d0*u(i,nym1,3))
                 rsd(i,nym2,4) = rsd(i,nym2,4)-dssp*(u(i,nym4,4)-
     &                4.d0*u(i,nym3,4)+6.d0*u(i,nym2,4)-
     &                4.d0*u(i,nym1,4))
                 rsd(i,nym2,5) = rsd(i,nym2,5)-dssp*(u(i,nym4,5)-
     &                4.d0*u(i,nym3,5)+6.d0*u(i,nym2,5)-
     &                4.d0*u(i,nym1,5))
                 
                 rsd(i,nym1,1) = rsd(i,nym1,1)-dssp*(u(i,nym3,1)-
     &                4.d0*u(i,nym2,1)+5.d0*u(i,nym1,1))
                 rsd(i,nym1,2) = rsd(i,nym1,2)-dssp*(u(i,nym3,2)-
     &                4.d0*u(i,nym2,2)+5.d0*u(i,nym1,2))
                 rsd(i,nym1,3) = rsd(i,nym1,3)-dssp*(u(i,nym3,3)-
     &                4.d0*u(i,nym2,3)+5.d0*u(i,nym1,3))
                 rsd(i,nym1,4) = rsd(i,nym1,4)-dssp*(u(i,nym3,4)-
     &                4.d0*u(i,nym2,4)+5.d0*u(i,nym1,4))
                 rsd(i,nym1,5) = rsd(i,nym1,5)-dssp*(u(i,nym3,5)-
     &                4.d0*u(i,nym2,5)+5.d0*u(i,nym1,5))
                 
 380          continue
              
           endif
              
           do 350 j = jstart, jstop
              
              jm1 = j-1
              jm2 = j-2
              jp1 = j+1
              jp2 = j+2

              do 360 i = 2, nxm1

                 rsd(i,j,1) = rsd(i,j,1)-dssp*(u(i,jm2,1)-
     &                4.d0*u(i,jm1,1)+6.d0*u(i,j,1)-
     &                4.d0*u(i,jp1,1)+u(i,jp2,1))
                 rsd(i,j,2) = rsd(i,j,2)-dssp*(u(i,jm2,2)-
     &                4.d0*u(i,jm1,2)+6.d0*u(i,j,2)-
     &                4.d0*u(i,jp1,2)+u(i,jp2,2))
                 rsd(i,j,3) = rsd(i,j,3)-dssp*(u(i,jm2,3)-
     &                4.d0*u(i,jm1,3)+6.d0*u(i,j,3)-
     &                4.d0*u(i,jp1,3)+u(i,jp2,3))
                 rsd(i,j,4) = rsd(i,j,4)-dssp*(u(i,jm2,4)-
     &                4.d0*u(i,jm1,4)+6.d0*u(i,j,4)-
     &                4.d0*u(i,jp1,4)+u(i,jp2,4))
                 rsd(i,j,5) = rsd(i,j,5)-dssp*(u(i,jm2,5)-
     &                4.d0*u(i,jm1,5)+6.d0*u(i,j,5)-
     &                4.d0*u(i,jp1,5)+u(i,jp2,5))

 360          continue

 350       continue
           
        endif

C******************************************************************************C
C******************************************************************************C
C***                      zeta-direction flux differences                   ***C
C******************************************************************************C
C******************************************************************************C
c*** End phase 3 of 4: receive u02 which is stored in utrc from left -2 neighbor.

        time10 = dclock()
        if (my_col.gt.2) call msgwait (msgid_lf2)
        time11 = dclock()-time10

        time_xch  = time_xch + time11
        time_wait = time_wait + time11
        txchng1   = txchng1 + time11
        
c***fourth-order dissipation

        if ((my_col.gt.3).and.(my_col.lt.node_col)) then
                 
           do 770 j = jbeg_loop, jend_loop
              
              jrc = j - jj_pointer

              do 780 i = 2, nxm1

                 rsd(i,j,1) = rsd(i,j,1)-dssp*utrc(i,jrc,1)
                 rsd(i,j,2) = rsd(i,j,2)-dssp*utrc(i,jrc,2)
                 rsd(i,j,3) = rsd(i,j,3)-dssp*utrc(i,jrc,3)
                 rsd(i,j,4) = rsd(i,j,4)-dssp*utrc(i,jrc,4)
                 rsd(i,j,5) = rsd(i,j,5)-dssp*utrc(i,jrc,5)

 780          continue

 770       continue

        endif
        
C******************************************************************************C
C******************************************************************************C
c*** Phase 4 of 4: Each node broadcasts u to -2 left neighbor and post a 
c*** receive for u2 which is stored in utrc from right +2 neighbor.

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
        
        time_xch  = time_xch + time13
        txchng1   = txchng1 + time13
        
C******************************************************************************C
C******************************************************************************C
C***                      zeta-direction flux differences                   ***C
C******************************************************************************C
C******************************************************************************C
c*** End phase 4 of 4: receive u2 which is stored in utrc from right +2 neighbor.

        time14 = dclock()
        if (my_col.lt.ndcolm1) call msgwait (msgid_rt2)
        time15 = dclock()-time14
        
        time_xch  = time_xch + time15
        time_wait = time_wait + time15
        txchng1   = txchng1 + time15
        
c***fourth-order dissipation

        if ((my_col.gt.1).and.(my_col.lt.ndcolm2)) then
                 
           do 1770 j = jbeg_loop, jend_loop

              jrc = j - jj_pointer

              do 1780 i = 2, nxm1

                 rsd(i,j,1) = rsd(i,j,1)-dssp*utrc(i,jrc,1)
                 rsd(i,j,2) = rsd(i,j,2)-dssp*utrc(i,jrc,2)
                 rsd(i,j,3) = rsd(i,j,3)-dssp*utrc(i,jrc,3)
                 rsd(i,j,4) = rsd(i,j,4)-dssp*utrc(i,jrc,4)
                 rsd(i,j,5) = rsd(i,j,5)-dssp*utrc(i,jrc,5)

 1780         continue

 1770      continue

        endif
        
        return
        
        end

C******************************************************************************C
C******************************************************************************C
