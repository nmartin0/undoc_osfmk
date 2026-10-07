C******************************************************************************C
C******************************************************************************C
C*** update rsd3 which is stored in rsd after z sweep.

        subroutine updz2

        include 'appbt.incl'

        dimension bufsd(isiz2_row,isiz3,5)
        dimension bufrc(isiz2_row,isiz3,5)

C******************************************************************************C
C******************************************************************************C
C*** Assemble to complete rsd.

        do 200 k = 1, nz
           do 250 j = 1, nynriv
              
              jsd = jj_pointer + j
           
              bufsd(j,k,1) = rsd(jsd,k,1)
              bufsd(j,k,2) = rsd(jsd,k,2)
              bufsd(j,k,3) = rsd(jsd,k,3)
              bufsd(j,k,4) = rsd(jsd,k,4)
              bufsd(j,k,5) = rsd(jsd,k,5)
              
 250       continue
 200    continue
        
        do 10 icount = 1, ndrowm1
           
           msgtp1 = numtype1 - icount
           
           msgid_row4 = irecv (msgtp1,bufrc,nbytes_row4)
           
           call csend (msgtp1,bufsd,nbytes_row4,
     &          ndsen_row(icount,1),my_pid)

           call msgwait (msgid_row4)
           
           jentry = ndrec_row(icount)*nynriv
           
           do 300 k = 1, nz

              do 350 j = 1, nynriv
                 
                 jrc = jentry + j
                 
                 rsd(jrc,k,1) = bufrc(j,k,1)
                 rsd(jrc,k,2) = bufrc(j,k,2)
                 rsd(jrc,k,3) = bufrc(j,k,3)
                 rsd(jrc,k,4) = bufrc(j,k,4)
                 rsd(jrc,k,5) = bufrc(j,k,5)
                 
 350          continue
           
 300       continue
              
 10     continue

        return
        
        end
           
C******************************************************************************C
C******************************************************************************C
