C******************************************************************************C
C******************************************************************************C
C*** update rsd3 which is stored in rsd after y sweep.

        subroutine updy2

        include 'appbt.incl'

        dimension bufsd(isiz1_row,isiz3_row,5)
        dimension bufrc(isiz1_row,isiz3_row,5)

C******************************************************************************C
C******************************************************************************C

        do 10 icount = 1, ndrowm1
           
           msgtp1 = numtype1 - icount
           
           msgid_row2 = irecv (msgtp1,bufrc,nbytes_row2)
           
           ientry = ndsen_row(icount,2)*nxnriv

           do 200 k = 1, nznriv
              
              ksd = kk_pointer + k
              
              do 250 i = 1, nxnriv
                 
                 isd = ientry + i

                 bufsd(i,k,1) = rsd(isd,ksd,1)
                 bufsd(i,k,2) = rsd(isd,ksd,2)
                 bufsd(i,k,3) = rsd(isd,ksd,3)
                 bufsd(i,k,4) = rsd(isd,ksd,4)
                 bufsd(i,k,5) = rsd(isd,ksd,5)
              
 250          continue
           
 200       continue

           call csend (msgtp1,bufsd,nbytes_row2,
     &          ndsen_row(icount,1),my_pid)
           
           call msgwait (msgid_row2)
           
           kentry = ndrec_row(icount)*nznriv
           
           do 300 k = 1, nznriv
              
              krc = kentry + k
              
              do 350 i = 1, nxnriv
                 
                 irc = ii_pointer + i

                 rsd(irc,krc,1) = bufrc(i,k,1)
                 rsd(irc,krc,2) = bufrc(i,k,2)
                 rsd(irc,krc,3) = bufrc(i,k,3)
                 rsd(irc,krc,4) = bufrc(i,k,4)
                 rsd(irc,krc,5) = bufrc(i,k,5)
                 
 350          continue
              
 300       continue
           
 10     continue

        return

        end

C******************************************************************************C
C******************************************************************************C
