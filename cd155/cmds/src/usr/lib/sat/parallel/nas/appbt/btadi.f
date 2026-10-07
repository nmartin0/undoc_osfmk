C*******************************************************************************C
C*******************************************************************************C
C** Perform pseudo-time stepping iterations for five coupled, nonlinear pde's **C
C*******************************************************************************C
C*******************************************************************************C

        subroutine btadi
      
        include 'appbt.incl'
        
        dimension delunm(5)

C*******************************************************************************C
C*******************************************************************************C

        iwcheck = 0
        if ((my_node.eq.node_wr).and.(idowr.eq.1)) iwcheck = 1
        
C*** compute and save the inverse of u.

        call compinv (1,nx,1,ny)
        
C*** compute the steady-state residuals by updating rsd using u.

        call rhs

        if (iwcheck.eq.1) write (6,*) 'passed rhs before adi'
        
C*******************************************************************************C
C*******************************************************************************C
        
        numsteps = itmax/2
        
        call gsync ()
        
        tstart = dclock ( )
        
        do 1000 istep = 1, numsteps
           
           if (my_node.eq.node_wr) then
              write(6,*) 'time step # = ',istep
           endif

           do 100 j = jj_begin, jj_end
              do 200 i = 1, nx
                 
                 rsd(i,j,1) = dt*rsd(i,j,1)
                 rsd(i,j,2) = dt*rsd(i,j,2)
                 rsd(i,j,3) = dt*rsd(i,j,3)
                 rsd(i,j,4) = dt*rsd(i,j,4)
                 rsd(i,j,5) = dt*rsd(i,j,5)
                 
 200          continue
 100       continue
           
C******************************************************************************C
C******************************************************************************C
C*** First exchange u to form uz: Send u and post a receive for uz.

           time10 = dclock()
           call xyTxz
           time11 = dclock()-time10
           
           tcomm = tcomm + time11
           
           if (iwcheck.eq.1) write (6,*) 'passed xyTxz'

C*******************************************************************************C
C*******************************************************************************C
C*** Perform 3-factor, Block ADI iterations
C*** perform the xi-direction sweep

           if ((my_col.gt.1).and.(my_col.lt.node_col)) then

              call jacx
           
              call btridxy1
           
           endif

C*******************************************************************************C
C*******************************************************************************C

           if (node_row.gt.1) then
              
              time_01 = dclock()
              call updx
              time_11 = dclock() - time_01
              txchng2 = txchng2 + time_11
              
              if (iwcheck.eq.1) write (6,*) 'passed updx'
              
           endif

C*******************************************************************************C
C*******************************************************************************C
C*** First transpose and copy rsd(nynriv,nx,5) into temp(5,ny,nxnriv).
        
           call dtran3 (nxnriv,ny,5,rsd(ii_begin,1,1),nx,
     &          temp,5,1,3,info)

C*******************************************************************************C
C*******************************************************************************C
C***  perform the eta-direction sweep

           if ((my_col.gt.1).and.(my_col.lt.node_col)) then

              call jacy
           
              call btridzy1

           endif
        
C*******************************************************************************C
C*******************************************************************************C
C*** Now receive uz which is stored in u.

           time12 = dclock()
           call xyTxz_wait
           time13 = dclock()-time12

           tcomm     = tcomm + time13
           time_wait = time_wait + time13
           
           if (iwcheck.eq.1) write (6,*) 'passed xyTxz_wait'

C*** Exchange rsd to form rsd2:  Send rsd and post a receive for rsd2.

           time14 = dclock()
           call xyTxzH
           time15 = dclock()-time14
           
           tcomm  = tcomm + time15
           
           if (iwcheck.eq.1) write (6,*) 'passed xyTxzH'

           call compinv (1,nx,1,ny)

           if (iwcheck.eq.1) write (6,*) 'passed compinv'
        
C*** Receive rsd2 which is stored in u.

           time16 = dclock()
           call xyTxzH_wait
           time17 = dclock()-time16
        
           tcomm      = tcomm + time17
           time_wait  = time_wait + time17
           
           if (iwcheck.eq.1) write (6,*) 'passed xyTxzH_wait'

C*******************************************************************************C
C*******************************************************************************C
        
           if ((my_col.gt.1).and.(my_col.lt.node_col)) then
           
              call jacz
              
              call btridzy2

           endif

C*******************************************************************************C
C*******************************************************************************C

           call dtran3 (5,nz,nxnriv,temp,5,rsd(ii_begin,1,1),
     &          nx,1,3,info)

C*******************************************************************************C
C*******************************************************************************C
        
           if (node_row.gt.1) then
              
              time_01 = dclock()
              call updz
              time_11 = dclock() - time_01
              txchng2 = txchng2 + time_11
              
              if (iwcheck.eq.1) write (6,*) 'passed updz'

           endif

C*******************************************************************************C
C*******************************************************************************C
C*** update the variables

           if ((my_col.gt.1).and.(my_col.lt.node_col)) then
              
              nlength = 5*nx*nz
              call daxpy (nlength,1.d0,rsd,1,u,1)

           endif
           
C*******************************************************************************C
C*******************************************************************************C
C*** compute and save the inverse of uz which is stored in u.

           call compinv (1,nx,1,nz)

           if (iwcheck.eq.1) write (6,*) 'passed compinv'
           
           time_xch = 0.d0
           
           time18 = dclock()
           call rhs2
           time19 = dclock()-time18
           
           tcalrhs = tcalrhs + time19 - time_xch
           
           if (iwcheck.eq.1) write (6,*) 'passed rhs2'
           
C*******************************************************************************C
C*******************************************************************************C
              
           nlength = 5*nx*ny
           call dscal (nlength,dt,rsd,1)

C*******************************************************************************C
C*******************************************************************************C
C*** first exchange uz to form uy:  uz is already stored in u. Send uz
C*** which is in u and post a receive for uy.

           time20 = dclock()
           call xzTyz
           time21 = dclock()-time20

           tcomm  = tcomm + time21
           
           if (iwcheck.eq.1) write (6,*) 'passed xzTyz'

C*******************************************************************************C
C*******************************************************************************C
C*** perform the xi-direction sweep
        
           if ((my_col.gt.1).and.(my_col.lt.node_col)) then
           
              call jacx2
            
              call btridxy1
              
           endif

C*******************************************************************************C
C*******************************************************************************C
C*** Now receive uy in u.

           time22 = dclock()
           call xzTyz_wait
           time23 = dclock()-time22
           
           tcomm      = tcomm + time23
           time_wait  = time_wait + time23
           
           if (iwcheck.eq.1) write (6,*) 'passed xzTyz_wait'

C*** Now exchange rsd2 which is stored in rsd to form rsd3. Send rsd
C*** (which is rsd2) and post a receive for rsd3 which is stored in u.

           time24 = dclock()
           call xzTyzH
           time25 = dclock()-time24
           
           tcomm  = tcomm + time25
           
           if (iwcheck.eq.1) write (6,*) 'passed xzTyzH'
           
C***  compute and save the inverse of uy.

           call compinv (1,ny,1,nz)

           if (iwcheck.eq.1) write (6,*) 'passed compinv'

C*** Now receive rsd3 in u.

           time26 = dclock()
           call xzTyzH_wait
           time27 = dclock()-time26
           
           tcomm  = tcomm + time27
           time_wait  = time_wait + time27
           
           if (iwcheck.eq.1) write (6,*) 'passed xzTyzH_wait'

C*******************************************************************************C
C*******************************************************************************C
C*** perform the eta-direction sweep

           if ((my_col.gt.1).and.(my_col.lt.node_col)) then
           
              call jacy2
           
              call btridxy2
              
           endif

C*******************************************************************************C
C*******************************************************************************C

           if (node_row.gt.1) then
              
              time_01 = dclock()
              call updy2
              time_11 = dclock() - time_01
              txchng2 = txchng2 + time_11
              
              if (iwcheck.eq.1) write (6,*) 'passed updy2'
              
           endif
           
C*******************************************************************************C
C*******************************************************************************C
C***  perform the zeta-direction sweep
           
           if ((my_col.gt.1).and.(my_col.lt.node_col)) then
           
              call jacz2
              
              call btridzy3
              
           endif
              
C*******************************************************************************C
C*******************************************************************************C
           
           if (node_row.gt.1) then
              
              time_01 = dclock()
              call updz2
              time_11 = dclock() - time_01
              txchng2 = txchng2 + time_11
              
              if (iwcheck.eq.1) write (6,*) 'passed updz2'
              
           endif
           
C*******************************************************************************C
C*******************************************************************************C
C*** update the variables

           nlength = 5*ny*nz
           call daxpy (nlength,1.d0,rsd,1,u,1)

C*******************************************************************************C
C*******************************************************************************C
C*** transpose data to xy-plane and compute the steady-state residuals

           time29 = dclock()
           call yzTxy
           time30 = dclock()-time29
           
           tcomm = tcomm + time30
           
           if (iwcheck.eq.1) write (6,*) 'passed yzTxy'
        
C*** compute and save the inverse of u.

           call compinv (1,nx,1,ny)
           
           if (iwcheck.eq.1) write (6,*) 'passed compinv'
           
           time_xch = 0.d0

           time30 = dclock()
           
           call rhs
           
           time31 = dclock()-time30
           tcalrhs = tcalrhs + time31 - time_xch
           
           if (iwcheck.eq.1) write (6,*) 'passed rhs'
           
           if (istep.eq.numsteps) then
              
              call l2norm (rsd,rsdnm,nx,ny)
              
              if (my_node.eq.node_wr) then
                 write (6,1007) (rsdnm(m), m=1,5)
              endif
             
           endif
          
 1000   continue

C*******************************************************************************C
C*******************************************************************************C
         
        tend   = dclock ()
        ttotal = tend - tstart
       
        return

C*******************************************************************************C
C*******************************************************************************C

 1006   format (1x/1x,'RMS-norm of Block ADI-iteration correction ',
     &       'for first pde  = ',/5x,e20.13/,
     &       1x,'RMS-norm of Block ADI-iteration correction ',
     &       'for second pde = ',/5x,e20.13/,
     &       1x,'RMS-norm of Block ADI-iteration correction ',
     &       'for third pde  = ',/5x,e20.13/,
     &        1x,'RMS-norm of Block ADI-iteration correction ',
     &       'for fourth pde = ',/5x,e20.13/,
     &       1x,'RMS-norm of Block ADI-iteration correction ',
     &       'for fifth pde  = ',/5x,e20.13)
        
 1007   format (1x/1x,'RMS-norm of steady-state residual for ',
     &       'first pde  = ',/5x,e20.13/,
     &       1x,'RMS-norm of steady-state residual for ',
     &       'second pde = ',/5x,e20.13/,
     &       1x,'RMS-norm of steady-state residual for ',
     &       'third pde  = ',/5x,e20.13/,
     &       1x,'RMS-norm of steady-state residual for ',
     &       'fourth pde = ',/5x,e20.13/,
     &       1x,'RMS-norm of steady-state residual for ',
     &        'fifth pde  = ',/5x,e20.13)
        
        end

C*******************************************************************************C
C*******************************************************************************C
