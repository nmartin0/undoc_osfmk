C*******************************************************************************C
C*******************************************************************************C
C***            Form the block tridiagonal system for xi-direction sweep     ***C
C*******************************************************************************C
C*******************************************************************************C

        subroutine jacx2

        include 'appbt.incl'
        include 'appbt.incl2'

C*******************************************************************************C
C*******************************************************************************C
        
        call dirichlet (1,nx,1,nznriv)

C*** compute the xi-direction flux jacobian

        do 100 k = kk_begin, kk_end
           
           krc = k - kk_pointer

           do 200 i = 1, nx
              
              xvel = uiv(i,k)*u(i,k,2)
              yvel = uiv(i,k)*u(i,k,3)
              zvel = uiv(i,k)*u(i,k,4)
              
              xvelsq = xvel*xvel
              yvelsq = yvel*yvel
              zvelsq = zvel*zvel
              
              yzvelsq = yvelsq+zvelsq
              velsq   = xvelsq+yvelsq+zvelsq
                 
              xyvel  = xvel*yvel
              xzvel  = xvel*zvel
              
              eterm  = uiv(i,k)*u(i,k,5)
              
              conjx1 = dttx1*c3c4*uiv(i,k)
              conjx2 = dttx1*c3c4a*uiv(i,k)
              conjx3 = dttx1*c3c4aa*uiv(i,k)
              conjx4 = dttx1*c3c4bb*uiv(i,k)
              conjx5 = dttx1*c1345*uiv(i,k)

              fjac2(i,1) =  dttx2*(-xvelsq+c2half*velsq)
              fjac2(i,2) =  dttx2*c2m2*xvel
              fjac2(i,3) = -c2dttx2*yvel
              fjac2(i,4) = -c2dttx2*zvel

              fjac3(i,1) = -dttx2*xyvel
              fjac3(i,2) =  dttx2*yvel
              fjac3(i,3) =  dttx2*xvel

              fjac4(i,1) = -dttx2*xzvel
              fjac4(i,2) =  dttx2*zvel
              fjac4(i,4) =  dttx2*xvel

              fjac5(i,1) =  dttx2*xvel*(c2*velsq-c1*eterm)
              fjac5(i,2) =  dttx2*(c1*eterm-c2half*(
     &             3.d0*xvelsq+yzvelsq))
              fjac5(i,3) = -c2dttx2*xyvel
              fjac5(i,4) = -c2dttx2*xzvel
              fjac5(i,5) =  c1dttx2*xvel
              
              djac2(i,1) = -conjx2*xvel
              djac2(i,2) =  conjx2+dttx1dx2
                 
              djac3(i,1) = -conjx1*yvel
              djac3(i,3) =  conjx1+dttx1dx3
              
              djac4(i,1) = -conjx1*zvel
              djac4(i,4) =  conjx1+dttx1dx4
              
              djac5(i,1) = -(conjx4*xvelsq+conjx3*yzvelsq+conjx5*eterm)
              djac5(i,2) =  conjx4*xvel
              djac5(i,3) =  conjx3*yvel
              djac5(i,4) =  conjx3*zvel
              djac5(i,5) =  conjx5+dttx1dx5
              
 200       continue

           do 300 i = 2, nxm1
              
              im1 = i-1
              
              aa(2,1,i,krc) = -fjac2(im1,1)-djac2(im1,1)
              aa(2,2,i,krc) = -fjac2(im1,2)-djac2(im1,2)
              aa(2,3,i,krc) = -fjac2(im1,3)
              aa(2,4,i,krc) = -fjac2(im1,4)
                 
              aa(3,1,i,krc) = -fjac3(im1,1)-djac3(im1,1)
              aa(3,2,i,krc) = -fjac3(im1,2)
              aa(3,3,i,krc) = -fjac3(im1,3)-djac3(im1,3)
                 
              aa(4,1,i,krc) = -fjac4(im1,1)-djac4(im1,1)
              aa(4,2,i,krc) = -fjac4(im1,2)
              aa(4,4,i,krc) = -fjac4(im1,4)-djac4(im1,4)
                 
              aa(5,1,i,krc) = -fjac5(im1,1)-djac5(im1,1)
              aa(5,2,i,krc) = -fjac5(im1,2)-djac5(im1,2)
              aa(5,3,i,krc) = -fjac5(im1,3)-djac5(im1,3)
              aa(5,4,i,krc) = -fjac5(im1,4)-djac5(im1,4)
              aa(5,5,i,krc) = -fjac5(im1,5)-djac5(im1,5)
                 
 300       continue
              
           do 400 i = 2, nxm1
                 
              bb(2,1,i,krc) = 2.d0*djac2(i,1)
              bb(2,2,i,krc) = 1.d0+2.d0*djac2(i,2)
                 
              bb(3,1,i,krc) = 2.d0*djac3(i,1)
              bb(3,3,i,krc) = 1.d0+2.d0*djac3(i,3)
                 
              bb(4,1,i,krc) = 2.d0*djac4(i,1)
              bb(4,4,i,krc) = 1.d0+2.d0*djac4(i,4)
                 
              bb(5,1,i,krc) = 2.d0*djac5(i,1)
              bb(5,2,i,krc) = 2.d0*djac5(i,2)
              bb(5,3,i,krc) = 2.d0*djac5(i,3)
              bb(5,4,i,krc) = 2.d0*djac5(i,4)
              bb(5,5,i,krc) = 1.d0+2.d0*djac5(i,5)

 400       continue
              
           do 500 i = 2, nxm1

              ip1 = i+1
                 
              cc(2,1,i,krc) = fjac2(ip1,1)-djac2(ip1,1)
              cc(2,2,i,krc) = fjac2(ip1,2)-djac2(ip1,2)
              cc(2,3,i,krc) = fjac2(ip1,3)
              cc(2,4,i,krc) = fjac2(ip1,4)
              cc(2,5,i,krc) = fjac2(ip1,5)
                 
              cc(3,1,i,krc) = fjac3(ip1,1)-djac3(ip1,1)
              cc(3,2,i,krc) = fjac3(ip1,2)
              cc(3,3,i,krc) = fjac3(ip1,3)-djac3(ip1,3)
                 
              cc(4,1,i,krc) = fjac4(ip1,1)-djac4(ip1,1)
              cc(4,2,i,krc) = fjac4(ip1,2)
              cc(4,4,i,krc) = fjac4(ip1,4)-djac4(ip1,4)
                 
              cc(5,1,i,krc) = fjac5(ip1,1)-djac5(ip1,1)
              cc(5,2,i,krc) = fjac5(ip1,2)-djac5(ip1,2)
              cc(5,3,i,krc) = fjac5(ip1,3)-djac5(ip1,3)
              cc(5,4,i,krc) = fjac5(ip1,4)-djac5(ip1,4)
              cc(5,5,i,krc) = fjac5(ip1,5)-djac5(ip1,5)

 500       continue
              
 100    continue
         
        kpp1 = kbeg_loop - kk_pointer
        kpp2 = kend_loop - kk_pointer

        do 1000 k = kpp1, kpp2
           do 1500 i = 2, nxm1
              
              aa(1,1,i,k) = -dttx1dx1
              aa(1,2,i,k) = -dttx2
              aa(1,3,i,k) =  0.d0
              aa(1,4,i,k) =  0.d0
              aa(1,5,i,k) =  0.d0
                 
              aa(2,5,i,k) = -c2dttx2
                 
              aa(3,4,i,k) =  0.d0
              aa(3,5,i,k) =  0.d0
                 
              aa(4,3,i,k) =  0.d0
              aa(4,5,i,k) =  0.d0
                 
 1500      continue
 1000   continue
         
        cbb11 = 1.d0+2.d0*dttx1dx1

        do 2000 k = kpp1, kpp2
           do 2500 i = 2, nxm1

              bb(1,1,i,k) = cbb11
              bb(1,2,i,k) = 0.d0
              bb(1,3,i,k) = 0.d0
              bb(1,4,i,k) = 0.d0
              bb(1,5,i,k) = 0.d0
                 
              bb(2,3,i,k) = 0.d0
              bb(2,4,i,k) = 0.d0
              bb(2,5,i,k) = 0.d0
                 
              bb(3,2,i,k) = 0.d0
              bb(3,4,i,k) = 0.d0
              bb(3,5,i,k) = 0.d0
                 
              bb(4,2,i,k) = 0.d0
              bb(4,3,i,k) = 0.d0
              bb(4,5,i,k) = 0.d0
                 
 2500      continue
 2000   continue
         
        do 3000 k = kpp1, kpp2
           do 3500 i = 2, nxm1
                 
              cc(1,1,i,k) = -dttx1dx1
              cc(1,2,i,k) =  dttx2
              cc(1,3,i,k) =  0.d0
              cc(1,4,i,k) =  0.d0
              cc(1,5,i,k) =  0.d0
                 
              cc(2,5,i,k) = c2dttx2
                 
              cc(3,4,i,k) = 0.d0
              cc(3,5,i,k) = 0.d0
                 
              cc(4,3,i,k) = 0.d0
              cc(4,5,i,k) = 0.d0
                 
 3500      continue
 3000   continue
         
        return

        end

C*******************************************************************************C
C*******************************************************************************C
