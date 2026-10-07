C******************************************************************************C
C******************************************************************************C
C***       This routine transposes two planes of the res matrix             ***C
C******************************************************************************C
C******************************************************************************C

        subroutine xzTyzH_wait

        include 'appbt.incl'

C******************************************************************************C
C******************************************************************************C
c*** restore the message buffer.

        do 210 icount = 1, ndcolm1
           
           irc = ndrec_col(icount)
           
           call msgwait (msidrec(icount))
           
           do 220 j = 1, nynriv
              
              jrc = jj_pointer + j

              rsd(irc,jrc,1) = bufrow(j,1,irc)
              rsd(irc,jrc,2) = bufrow(j,2,irc)
              rsd(irc,jrc,3) = bufrow(j,3,irc)
              rsd(irc,jrc,4) = bufrow(j,4,irc)
              rsd(irc,jrc,5) = bufrow(j,5,irc)
              
 220       continue
           
 210    continue

        return 
        
        end

C******************************************************************************C
C******************************************************************************C
