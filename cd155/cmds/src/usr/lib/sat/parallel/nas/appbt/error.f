C******************************************************************************C
C******************************************************************************C
C***                          compute the solution error                    ***C
C******************************************************************************C
C******************************************************************************C

        subroutine error

        include 'appbt.incl'

        dimension dtemp(5)

C******************************************************************************C
C******************************************************************************C

        do 50 m = 1, 5
           errnm(m) = 0.d0
 50     continue

        if ((my_col.gt.1).and.(my_col.lt.node_col)) then
           
           knode = my_col
           
           zeta  = dble(knode-1)*dnzm1
           
           do 100 j = jbeg_loop, jend_loop
              
              eta = dble(j-1)*dnym1
              
              do 110 i = 2, nxm1
                 
                 xi = dble(i-1)*dnxm1
                 
                 call exact (xi,eta,zeta,dtemp)
                 
                 errnm(1) = errnm(1) + 
     &                (dtemp(1)-u(i,j,1))*(dtemp(1)-u(i,j,1))
                 errnm(2) = errnm(2) + 
     &                (dtemp(2)-u(i,j,2))*(dtemp(2)-u(i,j,2))
                 errnm(3) = errnm(3) + 
     &                (dtemp(3)-u(i,j,3))*(dtemp(3)-u(i,j,3))
                 errnm(4) = errnm(4) + 
     &                (dtemp(4)-u(i,j,4))*(dtemp(4)-u(i,j,4))
                 errnm(5) = errnm(5) + 
     &                (dtemp(5)-u(i,j,5))*(dtemp(5)-u(i,j,5))
                 
 110          continue
              
 100       continue
           
        endif
        
c***do a global sum on errnm to get contributions from other processors

        call gdsum (errnm, 5, dtemp)

        do 150 m = 1, 5
           errnm(m) = dtotal*dsqrt(errnm(m))
 150    continue

        if (my_node.eq.node_wr) then
           write (6,*) 'solution error'
           write (6,1002) (errnm(i), i=1,5)
        endif

        return

C******************************************************************************C
C******************************************************************************C

 1002   format (1x/1x,'RMS-norm of error in soln. to ',
     &       'first pde  = ',1pe20.13/,
     &       1x,'RMS-norm of error in soln. to ',
     &       'second pde = ',1pe20.13/,
     &       1x,'RMS-norm of error in soln. to ',
     &       'third pde  = ',1pe20.13/,
     &       1x,'RMS-norm of error in soln. to ',
     &       'fourth pde = ',1pe20.13/,
     &       1x,'RMS-norm of error in soln. to ',
     &       'fifth pde  = ',1pe20.13)

        end

C******************************************************************************C
C******************************************************************************C
