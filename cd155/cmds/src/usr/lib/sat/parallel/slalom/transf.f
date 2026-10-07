
      real*8 coef_buf(npatch, *)
      integer iadd_max, send_node
*
*  Transform data into 1d decomposition
*
 	messtype = 1000
 	do 1010 k = 0, nxproc-1
 	  iadd = 1
 	  l = (non0 - 2 - iyproc + nyproc) / nyproc
 	  do 1020 i = non0-1, npatch-1, nxproc
 	    jstart = (i + k - 1 - ixproc + nxproc) / nxproc
 	    call dcopy(jstart, buf(iadd), 1, scratch((l+k)*idim), 1)
 	    iadd = iadd + jstart
 	    l = l + nxproc
 1020	  continue
 	  iadd_max = iadd + npatch/nxproc
 	  call csend(messtype, buf, 8*iadd, k*ixproc, pid)
 	  if(iproc .ge. k*nproc .and. iproc .lt. (k+1)*nproc) then
 	    do 1030 i = 1, nxproc
 	      send_node = infonode()
 	      call crecv(messtype, buf, 8*iadd_max)
 	      do 1040 j = 0, npatch-1, nxproc
 	        iadd = 1
 		jstart = (j - 2 - ixproc + nxproc) / nxproc
 		call dcopy(jstart, coef_buf(1+send_node, iadd), nxproc,
     &		  buf, 1)
 		iadd = iadd + 1
 1040	      continue
 1030	    continue
 	  endif
 1010	continue
