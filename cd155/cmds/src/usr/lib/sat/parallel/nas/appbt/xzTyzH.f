C******************************************************************************C
C******************************************************************************C
C***       This routine transposes two planes of the res matrix             ***C
C******************************************************************************C
C******************************************************************************C

        subroutine xzTyzH

        include 'appbt.incl'

        dimension bufsend(isiz1_row,5)

C******************************************************************************C
C******************************************************************************C
c*** each node posts num_node-1 recv() messages.

        do 50 icount = 1, ndcolm1
           
           irc    = ndrec_col(icount)
           msgtp1 = icount + numtype1
           
           msidrec(icount) = irecv (msgtp1,bufrow(1,1,irc),nbytes_lrow)
              
 50     continue

        call gsync ()

c*** each node now is ready to send num_node-1 messages.

        do 100 icount = 1, ndcolm1
           
           node_sen = ndsen_col(icount,1)
           isd      = ndsen_col(icount,2)

           msgtp1   = icount + numtype1
           
           do 120 j = 1, nynriv
              
              jsd = jj_pointer + j

              bufsend(j,1) = rsd(isd,jsd,1)
              bufsend(j,2) = rsd(isd,jsd,2)
              bufsend(j,3) = rsd(isd,jsd,3)
              bufsend(j,4) = rsd(isd,jsd,4)
              bufsend(j,5) = rsd(isd,jsd,5)
              
 120       continue
           
           call csend (msgtp1,bufsend,nbytes_lrow,node_sen,my_pid)
           
 100    continue
        
        return 
        
        end

C******************************************************************************C
C******************************************************************************C
