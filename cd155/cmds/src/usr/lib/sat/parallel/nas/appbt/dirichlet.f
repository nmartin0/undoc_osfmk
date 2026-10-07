C*******************************************************************************C
C*******************************************************************************C
C***                          Form Dirichlet Boundary Conditions             ***C
C*******************************************************************************C
C*******************************************************************************C

        subroutine dirichlet (iset1,iset2,jpp1,jpp2)

        include 'appbt.incl'
        include 'appbt.incl2'

C*******************************************************************************C
C*******************************************************************************C
C*** Dirichlet boundary conditions

        do 10 j = jpp1, jpp2
           
           aa(1,1,iset1,j) = 0.d0
           aa(1,2,iset1,j) = 0.d0
           aa(1,3,iset1,j) = 0.d0
           aa(1,4,iset1,j) = 0.d0
           aa(1,5,iset1,j) = 0.d0
           
           aa(2,1,iset1,j) = 0.d0
           aa(2,2,iset1,j) = 0.d0
           aa(2,3,iset1,j) = 0.d0
           aa(2,4,iset1,j) = 0.d0
           aa(2,5,iset1,j) = 0.d0
           
           aa(3,1,iset1,j) = 0.d0
           aa(3,2,iset1,j) = 0.d0
           aa(3,3,iset1,j) = 0.d0
           aa(3,4,iset1,j) = 0.d0
           aa(3,5,iset1,j) = 0.d0
           
           aa(4,1,iset1,j) = 0.d0
           aa(4,2,iset1,j) = 0.d0
           aa(4,3,iset1,j) = 0.d0
           aa(4,4,iset1,j) = 0.d0
           aa(4,5,iset1,j) = 0.d0
           
           aa(5,1,iset1,j) = 0.d0
           aa(5,2,iset1,j) = 0.d0
           aa(5,3,iset1,j) = 0.d0
           aa(5,4,iset1,j) = 0.d0
           aa(5,5,iset1,j) = 0.d0
           
           aa(1,1,iset2,j) = 0.d0
           aa(1,2,iset2,j) = 0.d0
           aa(1,3,iset2,j) = 0.d0
           aa(1,4,iset2,j) = 0.d0
           aa(1,5,iset2,j) = 0.d0
              
           aa(2,1,iset2,j) = 0.d0
           aa(2,2,iset2,j) = 0.d0
           aa(2,3,iset2,j) = 0.d0
           aa(2,4,iset2,j) = 0.d0
           aa(2,5,iset2,j) = 0.d0
           
           aa(3,1,iset2,j) = 0.d0
           aa(3,2,iset2,j) = 0.d0
           aa(3,3,iset2,j) = 0.d0
           aa(3,4,iset2,j) = 0.d0
           aa(3,5,iset2,j) = 0.d0
           
           aa(4,1,iset2,j) = 0.d0
           aa(4,2,iset2,j) = 0.d0
           aa(4,3,iset2,j) = 0.d0
           aa(4,4,iset2,j) = 0.d0
           aa(4,5,iset2,j) = 0.d0
           
           aa(5,1,iset2,j) = 0.d0
           aa(5,2,iset2,j) = 0.d0
           aa(5,3,iset2,j) = 0.d0
           aa(5,4,iset2,j) = 0.d0
           aa(5,5,iset2,j) = 0.d0
              
 10     continue
           
        do 20 j = jpp1, jpp2
           
           bb(1,1,iset1,j) = 1.d0
           bb(1,2,iset1,j) = 0.d0
           bb(1,3,iset1,j) = 0.d0
           bb(1,4,iset1,j) = 0.d0
           bb(1,5,iset1,j) = 0.d0
           
           bb(2,1,iset1,j) = 0.d0
           bb(2,2,iset1,j) = 1.d0
           bb(2,3,iset1,j) = 0.d0
           bb(2,4,iset1,j) = 0.d0
           bb(2,5,iset1,j) = 0.d0
           
           bb(3,1,iset1,j) = 0.d0
           bb(3,2,iset1,j) = 0.d0
           bb(3,3,iset1,j) = 1.d0
           bb(3,4,iset1,j) = 0.d0
           bb(3,5,iset1,j) = 0.d0
           
           bb(4,1,iset1,j) = 0.d0
           bb(4,2,iset1,j) = 0.d0
           bb(4,3,iset1,j) = 0.d0
           bb(4,4,iset1,j) = 1.d0
           bb(4,5,iset1,j) = 0.d0
           
           bb(5,1,iset1,j) = 0.d0
           bb(5,2,iset1,j) = 0.d0
           bb(5,3,iset1,j) = 0.d0
           bb(5,4,iset1,j) = 0.d0
           bb(5,5,iset1,j) = 1.d0
           
           bb(1,1,iset2,j) = 1.d0
           bb(1,2,iset2,j) = 0.d0
           bb(1,3,iset2,j) = 0.d0
           bb(1,4,iset2,j) = 0.d0
           bb(1,5,iset2,j) = 0.d0
           
           bb(2,1,iset2,j) = 0.d0
           bb(2,2,iset2,j) = 1.d0
           bb(2,3,iset2,j) = 0.d0
           bb(2,4,iset2,j) = 0.d0
           bb(2,5,iset2,j) = 0.d0
           
           bb(3,1,iset2,j) = 0.d0
           bb(3,2,iset2,j) = 0.d0
           bb(3,3,iset2,j) = 1.d0
           bb(3,4,iset2,j) = 0.d0
           bb(3,5,iset2,j) = 0.d0
           
           bb(4,1,iset2,j) = 0.d0
           bb(4,2,iset2,j) = 0.d0
           bb(4,3,iset2,j) = 0.d0
           bb(4,4,iset2,j) = 1.d0
           bb(4,5,iset2,j) = 0.d0
           
           bb(5,1,iset2,j) = 0.d0
           bb(5,2,iset2,j) = 0.d0
           bb(5,3,iset2,j) = 0.d0
           bb(5,4,iset2,j) = 0.d0
           bb(5,5,iset2,j) = 1.d0
           
 20     continue

        do 30 j = jpp1, jpp2
           
           cc(1,1,iset1,j) = 0.d0
           cc(1,2,iset1,j) = 0.d0
           cc(1,3,iset1,j) = 0.d0
           cc(1,4,iset1,j) = 0.d0
           cc(1,5,iset1,j) = 0.d0
              
           cc(2,1,iset1,j) = 0.d0
           cc(2,2,iset1,j) = 0.d0
           cc(2,3,iset1,j) = 0.d0
           cc(2,4,iset1,j) = 0.d0
           cc(2,5,iset1,j) = 0.d0
           
           cc(3,1,iset1,j) = 0.d0
           cc(3,2,iset1,j) = 0.d0
           cc(3,3,iset1,j) = 0.d0
           cc(3,4,iset1,j) = 0.d0
           cc(3,5,iset1,j) = 0.d0
           
           cc(4,1,iset1,j) = 0.d0
           cc(4,2,iset1,j) = 0.d0
           cc(4,3,iset1,j) = 0.d0
           cc(4,4,iset1,j) = 0.d0
           cc(4,5,iset1,j) = 0.d0
           
           cc(5,1,iset1,j) = 0.d0
           cc(5,2,iset1,j) = 0.d0
           cc(5,3,iset1,j) = 0.d0
           cc(5,4,iset1,j) = 0.d0
           cc(5,5,iset1,j) = 0.d0
           
           cc(1,1,iset2,j) = 0.d0
           cc(1,2,iset2,j) = 0.d0
           cc(1,3,iset2,j) = 0.d0
           cc(1,4,iset2,j) = 0.d0
           cc(1,5,iset2,j) = 0.d0
           
           cc(2,1,iset2,j) = 0.d0
           cc(2,2,iset2,j) = 0.d0
           cc(2,3,iset2,j) = 0.d0
           cc(2,4,iset2,j) = 0.d0
           cc(2,5,iset2,j) = 0.d0
           
           cc(3,1,iset2,j) = 0.d0
           cc(3,2,iset2,j) = 0.d0
           cc(3,3,iset2,j) = 0.d0
           cc(3,4,iset2,j) = 0.d0
           cc(3,5,iset2,j) = 0.d0
           
           cc(4,1,iset2,j) = 0.d0
           cc(4,2,iset2,j) = 0.d0
           cc(4,3,iset2,j) = 0.d0
           cc(4,4,iset2,j) = 0.d0
           cc(4,5,iset2,j) = 0.d0
           
           cc(5,1,iset2,j) = 0.d0
           cc(5,2,iset2,j) = 0.d0
           cc(5,3,iset2,j) = 0.d0
           cc(5,4,iset2,j) = 0.d0
           cc(5,5,iset2,j) = 0.d0
              
 30     continue

        return
        
        end

C*******************************************************************************C
C*******************************************************************************C
