C*******************************************************************************C
C*******************************************************************************C
C***          Form the block tridiagonal system for zeta-direction sweep     ***C
C*******************************************************************************C
C*******************************************************************************C

        subroutine jacz2

        include 'appbt.incl'
        include 'appbt.incl2'

C*******************************************************************************C
C*******************************************************************************C
C*** Set the Dirichlet boundary conditions.

        call dirichlet (1,nz,1,nynriv)

        call dtran (nynriv,nz,uiv(ii_begin,1),nx,utrc(1,1,1),nz)
        
        call dtran3 (nynriv,nz,4,u(ii_begin,1,2),nx,
     &       utrc(1,1,2),nz,1,2,info)
        
C*** Compute the zeta-direction flux jacobians

        do 100 j = jj_begin, jj_end
           
           jrc = j - jj_pointer

           do 200 k = 1, nz
                 
              xvel = utrc(k,jrc,1)*utrc(k,jrc,2)
              yvel = utrc(k,jrc,1)*utrc(k,jrc,3)
              zvel = utrc(k,jrc,1)*utrc(k,jrc,4)
              
              xvelsq = xvel*xvel
              yvelsq = yvel*yvel
              zvelsq = zvel*zvel
              
              xyvelsq = xvelsq+yvelsq
              velsq   = xyvelsq+zvelsq
              
              xzvel  = xvel*zvel
              yzvel  = yvel*zvel
              
              eterm  = utrc(k,jrc,1)*utrc(k,jrc,5)
              
              dttz1uiv = dttz1*utrc(k,jrc,1)

              conjz1 = dttz1uiv*c3c4
              conjz2 = dttz1uiv*c3c4a
              conjz3 = dttz1uiv*c3c4aa
              conjz4 = dttz1uiv*c3c4bb
              conjz5 = dttz1uiv*c1345

              fjac2(k,1) = -dttz2*xzvel
              fjac2(k,2) =  dttz2*zvel
              fjac2(k,4) =  dttz2*xvel
              
              fjac3(k,1) = -dttz2*yzvel
              fjac3(k,3) =  dttz2*zvel
              fjac3(k,4) =  dttz2*yvel
              
              fjac4(k,1) =  dttz2*(-zvelsq+c2half*velsq)
              fjac4(k,2) = -c2dttz2*xvel
              fjac4(k,3) = -c2dttz2*yvel
              fjac4(k,4) =  dttz2*c2m2*zvel
              
              fjac5(k,1) =  dttz2*zvel*(c2*velsq-c1*eterm)
              fjac5(k,2) = -c2dttz2*xzvel
              fjac5(k,3) = -c2dttz2*yzvel
              fjac5(k,4) =  dttz2*(c1*eterm-c2half*(
     &             xyvelsq+3.d0*zvelsq))
              fjac5(k,5) =  c1dttz2*zvel
              
              djac2(k,1) = -conjz1*xvel
              djac2(k,2) =  conjz1+dttz1dz2
              
              djac3(k,1) = -conjz1*yvel
              djac3(k,3) =  conjz1+dttz1dz3
              
              djac4(k,1) = -conjz2*zvel
              djac4(k,4) =  conjz2+dttz1dz4
                 
              djac5(k,1) = -(conjz3*xyvelsq+conjz4*zvelsq+conjz5*eterm)
              djac5(k,2) =  conjz3*xvel
              djac5(k,3) =  conjz3*yvel
              djac5(k,4) =  conjz4*zvel
              djac5(k,5) =  conjz5+dttz1dz5
              
 200       continue

C*** Note that I have changed aajk, bbjk and ccjk to aakj, bbkj and cckj.

           do 300 k = 2, nzm1
              
              km1 = k-1

              aa(2,1,k,jrc) = -fjac2(km1,1)-djac2(km1,1)
              aa(2,2,k,jrc) = -fjac2(km1,2)-djac2(km1,2)
              aa(2,4,k,jrc) = -fjac2(km1,4)
                 
              aa(3,1,k,jrc) = -fjac3(km1,1)-djac3(km1,1)
              aa(3,3,k,jrc) = -fjac3(km1,3)-djac3(km1,3)
              aa(3,4,k,jrc) = -fjac3(km1,4)
                 
              aa(4,1,k,jrc) = -fjac4(km1,1)-djac4(km1,1)
              aa(4,2,k,jrc) = -fjac4(km1,2)
              aa(4,3,k,jrc) = -fjac4(km1,3)
              aa(4,4,k,jrc) = -fjac4(km1,4)-djac4(km1,4)
                 
              aa(5,1,k,jrc) = -fjac5(km1,1)-djac5(km1,1)
              aa(5,2,k,jrc) = -fjac5(km1,2)-djac5(km1,2)
              aa(5,3,k,jrc) = -fjac5(km1,3)-djac5(km1,3)
              aa(5,4,k,jrc) = -fjac5(km1,4)-djac5(km1,4)
              aa(5,5,k,jrc) = -fjac5(km1,5)-djac5(km1,5)
                 
 300       continue
              
           do 400 k = 2, nzm1
                 
              bb(2,1,k,jrc) = 2.d0*djac2(k,1)
              bb(2,2,k,jrc) = 1.d0+2.d0*djac2(k,2)

              bb(3,1,k,jrc) = 2.d0*djac3(k,1)
              bb(3,3,k,jrc) = 1.d0+2.d0*djac3(k,3)
                 
              bb(4,1,k,jrc) = 2.d0*djac4(k,1)
              bb(4,4,k,jrc) = 1.d0+2.d0*djac4(k,4)
                 
              bb(5,1,k,jrc) = 2.d0*djac5(k,1)
              bb(5,2,k,jrc) = 2.d0*djac5(k,2)
              bb(5,3,k,jrc) = 2.d0*djac5(k,3)
              bb(5,4,k,jrc) = 2.d0*djac5(k,4)
              bb(5,5,k,jrc) = 1.d0+2.d0*djac5(k,5) 
                 
 400       continue
              
           do 500 k = 2, nzm1
                 
              kp1 = k+1

              cc(2,1,k,jrc) =  fjac2(kp1,1)-djac2(kp1,1)
              cc(2,2,k,jrc) =  fjac2(kp1,2)-djac2(kp1,2)
              cc(2,4,k,jrc) =  fjac2(kp1,4)

              cc(3,1,k,jrc) =  fjac3(kp1,1)-djac3(kp1,1)
              cc(3,3,k,jrc) =  fjac3(kp1,3)-djac3(kp1,3)
              cc(3,4,k,jrc) =  fjac3(kp1,4)
                 
              cc(4,1,k,jrc) =  fjac4(kp1,1)-djac4(kp1,1)
              cc(4,2,k,jrc) =  fjac4(kp1,2)
              cc(4,3,k,jrc) =  fjac4(kp1,3)
              cc(4,4,k,jrc) =  fjac4(kp1,4)-djac4(kp1,4)
                 
              cc(5,1,k,jrc) =  fjac5(kp1,1)-djac5(kp1,1)
              cc(5,2,k,jrc) =  fjac5(kp1,2)-djac5(kp1,2)
              cc(5,3,k,jrc) =  fjac5(kp1,3)-djac5(kp1,3)
              cc(5,4,k,jrc) =  fjac5(kp1,4)-djac5(kp1,4)
              cc(5,5,k,jrc) =  fjac5(kp1,5)-djac5(kp1,5)
                 
 500       continue
              
 100    continue

        jpp1 = jbeg_loop - jj_pointer
        jpp2 = jend_loop - jj_pointer

        do 1000 j = jpp1, jpp2
           do 1500 k = 2, nzm1
              
              aa(1,1,k,j) = -dttz1dz1
              aa(1,2,k,j) =  0.d0
              aa(1,3,k,j) =  0.d0
              aa(1,4,k,j) = -dttz2
              aa(1,5,k,j) =  0.d0
                 
              aa(2,3,k,j) =  0.d0
              aa(2,5,k,j) =  0.d0
                 
              aa(3,2,k,j) =  0.d0
              aa(3,5,k,j) =  0.d0
                 
              aa(4,5,k,j) = -c2dttz2
                 
 1500      continue
 1000   continue

        cbb11 = 1.d0+2.d0*dttz1dz1

        do 2000 j = jpp1, jpp2
           do 2500 k = 2, nzm1

              bb(1,1,k,j) = cbb11
              bb(1,2,k,j) = 0.d0
              bb(1,3,k,j) = 0.d0
              bb(1,4,k,j) = 0.d0
              bb(1,5,k,j) = 0.d0
                 
              bb(2,3,k,j) = 0.d0
              bb(2,4,k,j) = 0.d0
              bb(2,5,k,j) = 0.d0

              bb(3,2,k,j) = 0.d0
              bb(3,4,k,j) = 0.d0
              bb(3,5,k,j) = 0.d0
                 
              bb(4,2,k,j) = 0.d0
              bb(4,3,k,j) = 0.d0
              bb(4,5,k,j) = 0.d0
                 
 2500      continue
 2000   continue

        do 3000 j = jpp1, jpp2
           do 3500 k = 2, nzm1

              cc(1,1,k,j) = -dttz1dz1
              cc(1,2,k,j) =  0.d0
              cc(1,3,k,j) =  0.d0
              cc(1,4,k,j) =  dttz2
              cc(1,5,k,j) =  0.d0
                 
              cc(2,3,k,j) =  0.d0
              cc(2,5,k,j) =  0.d0

              cc(3,2,k,j) =  0.d0
              cc(3,5,k,j) =  0.d0
                 
              cc(4,5,k,j) =  c2dttz2
                 
 3500      continue
 3000   continue

        return

        end

C*******************************************************************************C
C*******************************************************************************C
