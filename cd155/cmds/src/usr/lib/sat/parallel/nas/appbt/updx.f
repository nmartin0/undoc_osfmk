C******************************************************************************C
C******************************************************************************C
C***update rsd after x sweep

        subroutine updx

        include 'appbt.incl'

        dimension bufsd(isiz1_row,isiz2_row,5)
        dimension bufrc(isiz1_row,isiz2_row,5)

C******************************************************************************C
C******************************************************************************C
C*** Re-arrange rsd(nx,nynriv) to rsd(nxnriv,ny).

        do 10 icount = 1, ndrowm1
           
           msgtp1 = numtype1 - icount
           
           msgid_row1 = irecv (msgtp1,bufrc,nbytes_row1)
           
           ientry = ndsen_row(icount,2)*nxnriv

           do 200 j = 1, nynriv
           
              jsd = jj_pointer + j
              
              do 250 i = 1, nxnriv
                 
                 isd = ientry + i

                 bufsd(i,j,1) = rsd(isd,jsd,1)
                 bufsd(i,j,2) = rsd(isd,jsd,2)
                 bufsd(i,j,3) = rsd(isd,jsd,3)
                 bufsd(i,j,4) = rsd(isd,jsd,4)
                 bufsd(i,j,5) = rsd(isd,jsd,5)
                 
 250          continue
              
 200       continue

           call csend (msgtp1,bufsd,nbytes_row1,
     &          ndsen_row(icount,1),my_pid)
           
           call msgwait (msgid_row1)
           
           jentry = ndrec_row(icount)*nynriv
           
           do 300 j = 1, nynriv
              
              jrc = jentry + j
              
              do 350 i = 1, nxnriv

                 irc = ii_pointer + i
                 
                 rsd(irc,jrc,1) = bufrc(i,j,1)
                 rsd(irc,jrc,2) = bufrc(i,j,2)
                 rsd(irc,jrc,3) = bufrc(i,j,3)
                 rsd(irc,jrc,4) = bufrc(i,j,4)
                 rsd(irc,jrc,5) = bufrc(i,j,5)
                 
 350          continue
              
 300       continue
           
 10     continue

        return

        end

C******************************************************************************C
C******************************************************************************C
