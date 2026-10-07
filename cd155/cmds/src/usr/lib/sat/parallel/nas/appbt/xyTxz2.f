C******************************************************************************C
C******************************************************************************C
C***       This routine transposes two planes of the res matrix             ***C
C******************************************************************************C
C******************************************************************************C

        subroutine xyTxz2

        include 'appbt.incl'

C******************************************************************************C
C******************************************************************************C

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
              
              bufsend(i,1) = frct(i,jsd,1)
              bufsend(i,2) = frct(i,jsd,2)
              bufsend(i,3) = frct(i,jsd,3)
              bufsend(i,4) = frct(i,jsd,4)
              bufsend(i,5) = frct(i,jsd,5)
              
 120       continue
              
           call csend (msgtp1,bufsend,nbytes_line,node_sen,my_pid)
           
 100    continue
        
C******************************************************************************C
C******************************************************************************C
c***  get own data.
        
        jcol = my_col
        
        do 160 i = 1, nx
           
           frct2(i,jcol,1) = frct(i,jcol,1)
           frct2(i,jcol,2) = frct(i,jcol,2)
           frct2(i,jcol,3) = frct(i,jcol,3)
           frct2(i,jcol,4) = frct(i,jcol,4)
           frct2(i,jcol,5) = frct(i,jcol,5)
           
 160    continue
           
c***  restore the message buffer.
           
        do 210 icount = 1, ndcolm1
           
           jrc = ndrec_col(icount)
           
           call msgwait (msidrec(icount))
           
           do 220 i = 1, nx
              
              frct2(i,jrc,1) = bufrec(i,1,jrc)
              frct2(i,jrc,2) = bufrec(i,2,jrc)
              frct2(i,jrc,3) = bufrec(i,3,jrc)
              frct2(i,jrc,4) = bufrec(i,4,jrc)
              frct2(i,jrc,5) = bufrec(i,5,jrc)
              
 220       continue
              
 210    continue

        return 

        end

C******************************************************************************C
C******************************************************************************C
