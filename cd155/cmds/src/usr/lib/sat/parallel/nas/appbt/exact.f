C******************************************************************************C
C******************************************************************************C
C***                  compute the exact solution at (i,j,k)                 ***C
C******************************************************************************C
C******************************************************************************C

        subroutine exact (xi,eta,zeta,dtemp)

        include 'appbt.incl'

        dimension dtemp(5)

C******************************************************************************C
C******************************************************************************C

        xisq   = xi*xi
        xicb   = xi*xisq
        xiqp   = xisq*xisq

        etasq  = eta*eta
        etacb  = eta*etasq
        etaqp  = etasq*etasq

        zetasq = zeta*zeta
        zetacb = zeta*zetasq
        zetaqp = zetasq*zetasq

        do 10 m = 1, 5

           dtemp(m) =  ce(m,1)
     $          + ce(m,2)*xi    + ce(m,3)*eta    + ce(m,4)*zeta
     $          + ce(m,5)*xisq  + ce(m,6)*etasq  + ce(m,7)*zetasq
     $          + ce(m,8)*xicb  + ce(m,9)*etacb  + ce(m,10)*zetacb
     $          + ce(m,11)*xiqp + ce(m,12)*etaqp + ce(m,13)*zetaqp

 10     continue

        if (dtemp(1).eq.0.d0) then
           write (6,*) 'my_node dtemp1 = ',my_node,dtemp(1)
           stop
        endif

        return

        end

C******************************************************************************C
C******************************************************************************C
