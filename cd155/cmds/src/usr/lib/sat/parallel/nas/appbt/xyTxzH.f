C******************************************************************************C
C******************************************************************************C
C***       This routine transposes two planes of the res matrix             ***C
C******************************************************************************C
C******************************************************************************C

        subroutine xyTxzH

        include 'appbt.incl'

        dimension bufsend(isiz1_row,5)

C******************************************************************************C
C******************************************************************************C
c*** each node posts num_node-1 recv() messages.

        do 50 icount = 1, ndcolm1
           
           jrc    = ndrec_col(icount)
           msgtp1 = icount + numtype1
           
           msidrec(icount) = irecv (msgtp1,bufrow(1,1,jrc),nbytes_lrow)
              
 50     continue

        call gsync ()

c*** each node now is ready to send num_node-1 messages.

        do 100 icount = 1, ndcolm1
           
           jsd = ndsen_col(icount,2)

           do 120 i = 1, nxnriv

              bufsend(i,1) = temp(1,jsd,i)
              bufsend(i,2) = temp(2,jsd,i)
              bufsend(i,3) = temp(3,jsd,i)
              bufsend(i,4) = temp(4,jsd,i)
              bufsend(i,5) = temp(5,jsd,i)
              
 120       continue
              
           msgtp1 = icount + numtype1
           
           call csend (msgtp1,bufsend,nbytes_lrow,
     &          ndsen_col(icount,1),my_pid)
           
 100    continue
        
        return 

        end

C******************************************************************************C
C******************************************************************************C
