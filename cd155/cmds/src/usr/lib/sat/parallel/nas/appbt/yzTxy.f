C******************************************************************************C
C******************************************************************************C
C***         This routine transposes two planes of the res matrix.          ***C
C******************************************************************************C
C******************************************************************************C

        subroutine yzTxy

        include 'appbt.incl'

        dimension bufsend(isiz2,5)

C******************************************************************************C
C******************************************************************************C
c*** each node posts num_node-1 recv() messages.

        do 50 icount = 1, ndcolm1
           
           isd    = ndrec_col(icount)
           msgtp1 = icount + numtype1
           
           msidrec(icount) = irecv (msgtp1,bufrec(1,1,isd),nbytes_line)
           
 50     continue

c*** each node now is ready to send num_node-1 messages.

        call gsync ()

        do 100 icount = 1, ndcolm1
           
           node_sen = ndsen_col(icount,1)
           jrc      = ndsen_col(icount,2)

           msgtp1   = icount + numtype1
           
           do 120 i = 1, nx
              
              bufsend(i,1) = u(i,jrc,1)
              bufsend(i,2) = u(i,jrc,2)
              bufsend(i,3) = u(i,jrc,3)
              bufsend(i,4) = u(i,jrc,4)
              bufsend(i,5) = u(i,jrc,5)
              
 120       continue
           
           call csend (msgtp1,bufsend,nbytes_line,node_sen,my_pid)
              
 100    continue
           
c***  get own data.
           
        jcol = my_col
        irow = my_col
        
        do 160 i = 1, nx
           
           bufsend(i,1) = u(i,jcol,1)
           bufsend(i,2) = u(i,jcol,2)
           bufsend(i,3) = u(i,jcol,3)
           bufsend(i,4) = u(i,jcol,4)
           bufsend(i,5) = u(i,jcol,5)
              
 160    continue
           
        do 165 i = 1, nx
           
           u(irow,i,1) = bufsend(i,1)
           u(irow,i,2) = bufsend(i,2)
           u(irow,i,3) = bufsend(i,3)
           u(irow,i,4) = bufsend(i,4)
           u(irow,i,5) = bufsend(i,5)
              
 165    continue
           
c***  restore the message buffer.

        do 210 icount = 1, ndcolm1
           
           irc = ndrec_col(icount)
           
           call msgwait (msidrec(icount))
              
           do 220 j = 1, ny
              
              u(irc,j,1) = bufrec(j,1,irc)
              u(irc,j,2) = bufrec(j,2,irc)
              u(irc,j,3) = bufrec(j,3,irc)
              u(irc,j,4) = bufrec(j,4,irc)
              u(irc,j,5) = bufrec(j,5,irc)
              
 220       continue
              
 210    continue

        return 
        
        end

C******************************************************************************C
C******************************************************************************C
