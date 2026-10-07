C******************************************************************************C
C******************************************************************************C
C***       This routine transposes two planes of the res matrix             ***C
C******************************************************************************C
C******************************************************************************C

        subroutine xyTxz

        include 'appbt.incl'

        dimension bufsend(isiz1,5)

C******************************************************************************C
C******************************************************************************C
c*** each node posts num_node-1 recv() messages.

        do 50 icount = 1, ndcolm1
           
           jrc   = ndrec_col(icount)
           msgtp1 = icount + numtype1
           
           msidrec(icount) = irecv (msgtp1,bufrec(1,1,jrc),nbytes_line)
              
 50     continue

        call gsync ()

c*** each node now is ready to send num_node-1 messages.

        do 100 icount = 1, ndcolm1
           
           node_sen = ndsen_col(icount,1)
           jsd      = ndsen_col(icount,2)

           msgtp1   = icount + numtype1
           
           do 120 i = 1, nx
              
              bufsend(i,1) = u(i,jsd,1)
              bufsend(i,2) = u(i,jsd,2)
              bufsend(i,3) = u(i,jsd,3)
              bufsend(i,4) = u(i,jsd,4)
              bufsend(i,5) = u(i,jsd,5)
              
 120       continue
              
           call csend (msgtp1,bufsend,nbytes_line,node_sen,my_pid)
           
 100    continue
        
        return 

        end

C******************************************************************************C
C******************************************************************************C
