C******************************************************************************C
C******************************************************************************C
C***       This routine transposes two planes of the res matrix             ***C
C******************************************************************************C
C******************************************************************************C

        subroutine xzTyz

        include 'appbt.incl'

        dimension bufsend(isiz1,5)

C******************************************************************************C
C******************************************************************************C
c*** each node posts num_node-1 recv() messages.

        do 50 icount = 1, ndcolm1
           
           isd    = ndrec_col(icount)
           msgtp1 = icount + numtype1
           
           msidrec(icount) = irecv (msgtp1,bufrec(1,1,isd),nbytes_line)
              
 50     continue

        call gsync ()

c*** each node now is ready to send num_node-1 messages.

        do 100 icount = 1, ndcolm1
           
           node_sen = ndsen_col(icount,1)
           irc      = ndsen_col(icount,2)

           msgtp1   = icount + numtype1
           
           do 120 j = 1, ny
              
              bufsend(j,1) = u(irc,j,1)
              bufsend(j,2) = u(irc,j,2)
              bufsend(j,3) = u(irc,j,3)
              bufsend(j,4) = u(irc,j,4)
              bufsend(j,5) = u(irc,j,5)
              
 120       continue
           
           call csend (msgtp1,bufsend,nbytes_line,node_sen,my_pid)
           
 100    continue
        
        return 
        
        end

C******************************************************************************C
C******************************************************************************C
