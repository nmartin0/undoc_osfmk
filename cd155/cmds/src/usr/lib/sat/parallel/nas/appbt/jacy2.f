C*******************************************************************************C
C*******************************************************************************C
C***           Form the block tridiagonal system for eta-direction sweep     ***C
C*******************************************************************************C
C*******************************************************************************C

        subroutine jacy2

        include 'appbt.incl'
        include 'appbt.incl2'

C*******************************************************************************C
C*******************************************************************************C
C*** Set the Dirichlet boundary conditions.

        call dirichlet (1,ny,1,nznriv)

C*** Compute the eta-direction flux jacobians

        do 100 k = kbeg_loop, kend_loop
           
           krc = k - kk_pointer

           do 200 j = 1, ny
              
              xvel = uiv(j,k)*u(j,k,2)
              yvel = uiv(j,k)*u(j,k,3)
              zvel = uiv(j,k)*u(j,k,4)
              
              xvelsq = xvel*xvel
              yvelsq = yvel*yvel
              zvelsq = zvel*zvel
              
              xzvelsq = xvelsq+zvelsq
              velsq   = xzvelsq+yvelsq
              
              xyvel  = xvel*yvel
              yzvel  = yvel*zvel
              
              eterm  = uiv(j,k)*u(j,k,5)
              
              dtty1uiv = dtty1*uiv(j,k)

              conjy1 = dtty1uiv*c3c4
              conjy2 = dtty1uiv*c3c4a
              conjy3 = dtty1uiv*c3c4aa
              conjy4 = dtty1uiv*c3c4bb
              conjy5 = dtty1uiv*c1345

              fjac2(j,1) = -dtty2*xyvel
              fjac2(j,2) =  dtty2*yvel
              fjac2(j,3) =  dtty2*xvel

              fjac3(j,1) =  dtty2*(-yvelsq+c2half*velsq)
              fjac3(j,2) = -c2dtty2*xvel
              fjac3(j,3) =  dtty2*c2m2*yvel
              fjac3(j,4) = -c2dtty2*zvel

              fjac4(j,1) = -dtty2*yzvel
              fjac4(j,3) =  dtty2*zvel
              fjac4(j,4) =  dtty2*yvel

              fjac5(j,1) =  dtty2*yvel*(c2*velsq-c1*eterm)
              fjac5(j,2) = -c2dtty2*xyvel
              fjac5(j,3) =  dtty2*(c1*eterm-c2half*(
     &             xzvelsq+3.d0*yvelsq))
              fjac5(j,4) = -c2dtty2*yzvel
              fjac5(j,5) =  c1dtty2*yvel

              djac2(j,1) = -conjy1*xvel
              djac2(j,2) =  conjy1+dtty1dy2
              
              djac3(j,1) = -conjy2*yvel
              djac3(j,3) =  conjy2+dtty1dy3
              
              djac4(j,1) = -conjy1*zvel
              djac4(j,4) =  conjy1+dtty1dy4
              
              djac5(j,1) = -(conjy3*xzvelsq+conjy4*yvelsq+conjy5*eterm)
              djac5(j,2) =  conjy3*xvel
              djac5(j,3) =  conjy4*yvel
              djac5(j,4) =  conjy3*zvel
              djac5(j,5) =  conjy5+dtty1dy5
                 
 200       continue
              
           do 300 j = 2, nym1
              
              jm1 = j-1
              
              aa(2,1,j,krc) = -fjac2(jm1,1)-djac2(jm1,1)
              aa(2,2,j,krc) = -fjac2(jm1,2)-djac2(jm1,2)
              aa(2,3,j,krc) = -fjac2(jm1,3)
                 
              aa(3,1,j,krc) = -fjac3(jm1,1)-djac3(jm1,1)
              aa(3,2,j,krc) = -fjac3(jm1,2)
              aa(3,3,j,krc) = -fjac3(jm1,3)-djac3(jm1,3)
              aa(3,4,j,krc) = -fjac3(jm1,4)

              aa(4,1,j,krc) = -fjac4(jm1,1)-djac4(jm1,1)
              aa(4,3,j,krc) = -fjac4(jm1,3)
              aa(4,4,j,krc) = -fjac4(jm1,4)-djac4(jm1,4)

              aa(5,1,j,krc) = -fjac5(jm1,1)-djac5(jm1,1)
              aa(5,2,j,krc) = -fjac5(jm1,2)-djac5(jm1,2)
              aa(5,3,j,krc) = -fjac5(jm1,3)-djac5(jm1,3)
              aa(5,4,j,krc) = -fjac5(jm1,4)-djac5(jm1,4)
              aa(5,5,j,krc) = -fjac5(jm1,5)-djac5(jm1,5)

 300       continue

           do 400 j = 2, nym1

              bb(2,1,j,krc) = 2.d0*djac2(j,1)
              bb(2,2,j,krc) = 1.d0+2.d0*djac2(j,2)

              bb(3,1,j,krc) = 2.d0*djac3(j,1)
              bb(3,3,j,krc) = 1.d0+2.d0*djac3(j,3)
                 
              bb(4,1,j,krc) = 2.d0*djac4(j,1)
              bb(4,4,j,krc) = 1.d0+2.d0*djac4(j,4)
                 
              bb(5,1,j,krc) = 2.d0*djac5(j,1)
              bb(5,2,j,krc) = 2.d0*djac5(j,2)
              bb(5,3,j,krc) = 2.d0*djac5(j,3)
              bb(5,4,j,krc) = 2.d0*djac5(j,4)
              bb(5,5,j,krc) = 1.d0+2.d0*djac5(j,5)
                 
 400       continue

           do 500 j = 2, nym1
              
              jp1 = j+1
                 
              cc(2,1,j,krc) =  fjac2(jp1,1)-djac2(jp1,1)
              cc(2,2,j,krc) =  fjac2(jp1,2)-djac2(jp1,2)
              cc(2,3,j,krc) =  fjac2(jp1,3)

              cc(3,1,j,krc) =  fjac3(jp1,1)-djac3(jp1,1)
              cc(3,2,j,krc) =  fjac3(jp1,2)
              cc(3,3,j,krc) =  fjac3(jp1,3)-djac3(jp1,3)
              cc(3,4,j,krc) =  fjac3(jp1,4)
                 
              cc(4,1,j,krc) =  fjac4(jp1,1)-djac4(jp1,1)
              cc(4,3,j,krc) =  fjac4(jp1,3)
              cc(4,4,j,krc) =  fjac4(jp1,4)-djac4(jp1,4)

              cc(5,1,j,krc) =  fjac5(jp1,1)-djac5(jp1,1)
              cc(5,2,j,krc) =  fjac5(jp1,2)-djac5(jp1,2)
              cc(5,3,j,krc) =  fjac5(jp1,3)-djac5(jp1,3)
              cc(5,4,j,krc) =  fjac5(jp1,4)-djac5(jp1,4)
              cc(5,5,j,krc) =  fjac5(jp1,5)-djac5(jp1,5)

 500       continue

 100    continue

        kpp1 = kbeg_loop - kk_pointer
        kpp2 = kend_loop - kk_pointer

        do 1000 k = kpp1, kpp2
           do 1500 j = 2, nym1
              
              aa(1,1,j,k) = -dtty1dy1
              aa(1,2,j,k) =  0.d0
              aa(1,3,j,k) = -dtty2
              aa(1,4,j,k) =  0.d0
              aa(1,5,j,k) =  0.d0
                 
              aa(2,4,j,k) =  0.d0
              aa(2,5,j,k) =  0.d0
                 
              aa(3,5,j,k) = -c2dtty2

              aa(4,2,j,k) =  0.d0
              aa(4,5,j,k) =  0.d0

 1500      continue
 1000   continue

        cbb11 = 1.d0+2.d0*dtty1dy1

        do 2000 k = kpp1, kpp2
           do 2500 j = 2, nym1
              
              bb(1,1,j,k) = cbb11
              bb(1,2,j,k) = 0.d0
              bb(1,3,j,k) = 0.d0
              bb(1,4,j,k) = 0.d0
              bb(1,5,j,k) = 0.d0

              bb(2,3,j,k) = 0.d0
              bb(2,4,j,k) = 0.d0
              bb(2,5,j,k) = 0.d0

              bb(3,2,j,k) = 0.d0
              bb(3,4,j,k) = 0.d0
              bb(3,5,j,k) = 0.d0
                 
              bb(4,2,j,k) = 0.d0
              bb(4,3,j,k) = 0.d0
              bb(4,5,j,k) = 0.d0
                 
 2500      continue
 2000   continue

        do 3000 k = kpp1, kpp2
           do 3500 j = 2, nym1
                 
              cc(1,1,j,k) = -dtty1dy1
              cc(1,2,j,k) =  0.d0
              cc(1,3,j,k) =  dtty2
              cc(1,4,j,k) =  0.d0
              cc(1,5,j,k) =  0.d0


              cc(2,4,j,k) =  0.d0
              cc(2,5,j,k) =  0.d0

              cc(3,5,j,k) =  c2dtty2
                 
              cc(4,2,j,k) =  0.d0
              cc(4,5,j,k) =  0.d0

 3500      continue
 3000   continue

        return

        end

C*******************************************************************************C
C*******************************************************************************C
