C******************************************************************************C
C******************************************************************************C
C*** update rsd2 which is stored in rsd after x sweep.

        subroutine updz

        include 'appbt.incl'

        dimension bufsd(isiz1_row,isiz3,5)
        dimension bufrc(isiz1_row,isiz3,5)

C******************************************************************************C
C******************************************************************************C
C*** Assemble to complete rsd.

        do 200 k = 1, nz
           do 250 i = 1, nxnriv
              
              isd = ii_pointer + i
           
              bufsd(i,k,1) = rsd(isd,k,1)
              bufsd(i,k,2) = rsd(isd,k,2)
              bufsd(i,k,3) = rsd(isd,k,3)
              bufsd(i,k,4) = rsd(isd,k,4)
              bufsd(i,k,5) = rsd(isd,k,5)
              
 250       continue
 200    continue

        do 10 icount = 1, ndrowm1

           msgtp1 = numtype1 - icount

           msgid_row3 = irecv (msgtp1,bufrc,nbytes_row3)

           call csend (msgtp1,bufsd,nbytes_row3,
     &          ndsen_row(icount,1),my_pid)
           
           call msgwait (msgid_row3)

           ientry = ndrec_row(icount)*nxnriv
           
           do 300 k = 1, nz
              do 350 i = 1, nxnriv
              
                 irc = ientry + i
                 
                 rsd(irc,k,1) = bufrc(i,k,1)
                 rsd(irc,k,2) = bufrc(i,k,2)
                 rsd(irc,k,3) = bufrc(i,k,3)
                 rsd(irc,k,4) = bufrc(i,k,4)
                 rsd(irc,k,5) = bufrc(i,k,5)
                 
 350          continue
 300       continue
              
 10     continue

        return

        end

C******************************************************************************C
C******************************************************************************C
