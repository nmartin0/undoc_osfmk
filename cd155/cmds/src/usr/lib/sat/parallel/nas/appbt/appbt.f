C******************************************************************************C
C******************************************************************************C
C***         driver for the performance evaluation of the solver for        ***C
C***         five coupled, nonlinear partial differential equations.        ***C
C******************************************************************************C
C******************************************************************************C

        program appbt

        include 'appbt.incl'

        dimension ndata(14)
        dimension ddata(7)

C******************************************************************************C
C******************************************************************************C
C*** my processor id info.

        my_node  = mynode()
        my_pid   = mypid()
        num_node = nnodes
        
        my_col = mod (my_node,node_col) + 1
        my_row = my_node / node_col + 1

        myids3 =  0
        myids4 =  0

        if (my_row.eq.1) then
           myids3 = 1
        endif
        
        if (my_row.eq.node_row) then 
           myids4 = -1
        endif

        my_colm1 = my_col - 1
        my_rowm1 = my_row - 1

        if (mod(nnodes,node_col).ne.0) then
           write (6,*) 'numnodes node_col mod = ',
     &          nnodes,node_col,mod(nnodes,node_col)
        endif

C******************************************************************************C
C******************************************************************************C
C***  node 0 opens file for input data, read and send to all other nodes.

        if (my_node.eq.0) then

           open (unit=5,file='appbt.inp',status='old')

C*** read the unit number for output data

           read (5,901)
           read (5,901)
           read (5,*) iout

           write (6,*) 'iout = ',iout

C*** flag that controls printing of the progress of iterations

           read (5,901)
           read (5,901)
           read (5,*) ipr, inorm
           
           write (6,*) 'ipr inorm = ',ipr, inorm

C*** set the maximum number of pseudo-time steps to be taken

           read (5,901)
           read (5,901)
           read (5,*) itmax

           write (6,*) 'itmax = ',itmax

C*** set the magnitude of the time step

           read (5,901)
           read (5,901)
           read (5,*) dt

           write (6,*) 'dt = ',dt

C*** set the method of inverting the jacobian martix
c   (invert = 1 : use Block approximate factorization method,
c    invert = 2 : use Diagonalized approximate factorization method,
c    invert = 3 : use SSOR methd)

           read (5,901)
           read (5,901)
           read (5,*) invert

           write (6,*) 'invert = ',invert

C*** set the value of over-relaxation factor for SSOR iterations

           read (5,901)
           read (5,901)
           read (5,*) omega

           write (6,*) 'omega = ',omega

C*** set the steady-state residual tolerance levels

           read (5,901)
           read (5,901)
           read (5,*) (tolrsd(m),m=1,5)

           write (6,*) 'tolrsd = ',(tolrsd(i),i=1,5)

C***  read problem specification parameters
c*** specify the number of grid points in xi, eta and zeta directions
           
           read (5,901)
           read (5,901)
           read (5,*) nx, ny, nz
           
           write (6,*) 'nx nyn nz = ',nx,ny,nz

C***  read specification for forced or unforced message types.
C***  if mforce=1, then use forced-type messages; otherwise, if
C***  mforce=0, use regular messages.

           read (5,901)
           read (5,901)
           read (5,*) mforce1,mforce2,mforce3
           
           write (6,*) 'mforce1 mforce2 mforce3 = ',
     &          mforce1,mforce2,mforce3

C***  read the node which will write the output

           read (5,901)
           read (5,901)
           read (5,*) node_wr,idowr

           write (6,*) 'node_wr idowr = ',node_wr,idowr

C***  read the node which will write the output

           read (5,901)
           read (5,901)
           read (5,*) numruns

           write (6,*) 'numruns = ',numruns

C***

           write (6,*) 'node_row node_col nnodes = ',
     &          node_row,node_col,nnodes

C***  prepare and send input data to all other nodes.

           ndata(1)  = iout
           ndata(2)  = ipr
           ndata(3)  = inorm
           ndata(4)  = itmax
           ndata(5)  = invert
           ndata(6)  = nx
           ndata(7)  = ny
           ndata(8)  = nz
           ndata(9)  = mforce1
           ndata(10) = mforce2
           ndata(11) = node_wr
           ndata(12) = idowr
           ndata(13) = mforce3
           ndata(14) = numruns

           nbytes = 4*14
           call csend (10011,ndata,nbytes,-1,my_pid)

           ddata(1) = dt
           ddata(2) = omega

           do 3001 i=1,5
              ddata(i+2) = tolrsd(i)
 3001      continue

           nbytes = 8*7
           call csend (10012,ddata,nbytes,-1,my_pid)

           close (unit=5, status='keep')

C******************************************************************************C
C******************************************************************************C
C***  other nodes receive input data from node node_wr.

        else

           nbytes = 4*14
           call crecv (10011,ndata,nbytes)

           nbytes = 8*7
           call crecv (10012,ddata,nbytes)

           iout    = ndata(1)
           ipr     = ndata(2)
           inorm   = ndata(3)
           itmax   = ndata(4)
           invert  = ndata(5)
           nx      = ndata(6)
           ny      = ndata(7)
           nz      = ndata(8)
           mforce1 = ndata(9)
           mforce2 = ndata(10)
           node_wr = ndata(11)
           idowr   = ndata(12)
           mforce3 = ndata(13)
           numruns = ndata(14)

           dt     = ddata(1)
           omega  = ddata(2)

           do 3002 i=1,5
              tolrsd(i) = ddata(i+2)
 3002      continue

        endif

C******************************************************************************C
C******************************************************************************C
C*** open the file for output data

        if ((nx.lt.5).or.(ny.lt.5).or.(nz.lt.5)) then
           write (6,2001)
           stop
        endif

        if ((nx.ne.isiz1).or.(ny.ne.isiz2).or.(nz.ne.isiz3)) then

           write (6,*) 'my_node nx ny nz isiz1 isiz2 isiz3 = ',
     &          my_node,nx,ny,nz,isiz1,isiz2,isiz3
           stop

        endif

C******************************************************************************C
C******************************************************************************C
C***  specify forced or unforced-type messages.

        if (mforce1.eq.1) then
           numtype1 = num30
        elseif (mforce1.eq.0) then
           numtype1 = num300
        endif

        if (mforce2.eq.1) then
           numtype2 = num30
        elseif (mforce2.eq.0) then
           numtype2 = num300
        endif

        if (mforce3.eq.1) then
           numtype3 = num30
        elseif (mforce3.eq.0) then
           numtype3 = num300
        endif

C******************************************************************************C
C******************************************************************************C
C*** locate neighbor nodes.

        node_plus1 = my_node + 1
        node_plus2 = my_node + 2

        node_minus1 = my_node - 1
        node_minus2 = my_node - 2

C******************************************************************************C
C******************************************************************************C
C***  set constants for all subroutines.

        nxm1 = nx-1
        nxm2 = nx-2
        nxm3 = nx-3
        nxm4 = nx-4

        nym1 = ny-1
        nym2 = ny-2
        nym3 = ny-3
        nym4 = ny-4

        nzm1 = nz-1
        nzm2 = nz-2
        nzm3 = nz-3
        nzm4 = nz-4
        
        dnxm1  = 1.d0/dble(nxm1)
        dnym1  = 1.d0/dble(nym1)
        dnzm1  = 1.d0/dble(nzm1)

        dtotal = 1.d0/dsqrt(dble(nxm2*nym2*nzm2))

        nxnriv = nx/node_row
        nynriv = ny/node_row
        nznriv = nz/node_row

        ii_pointer = my_rowm1*nxnriv
        jj_pointer = my_rowm1*nynriv
        kk_pointer = my_rowm1*nznriv

        ii_begin = 1+ii_pointer
        ii_end   = nxnriv+ii_pointer

        jj_begin = 1+jj_pointer
        jj_end   = nynriv+jj_pointer

        kk_begin = 1+kk_pointer
        kk_end   = nznriv+kk_pointer

        ibeg_loop = ii_begin + myids3
        iend_loop = ii_end   + myids4

        jbeg_loop = jj_begin + myids3
        jend_loop = jj_end   + myids4

        kbeg_loop = kk_begin + myids3
        kend_loop = kk_end   + myids4

        jbegm1 = jbeg_loop - 1
        jenda1 = jend_loop + 1
        
        kbegm1 = kbeg_loop - 1
        kenda1 = kend_loop + 1
        
        ilength = iend_loop-ibeg_loop+1
        jlength = jend_loop-jbeg_loop+1
        klength = kend_loop-kbeg_loop+1

        jstart = jj_begin + myids3*3
        jstop  = jj_end   + myids4*3
           
        kstart = kk_begin + myids3*3
        kstop  = kk_end   + myids4*3
           
C******************************************************************************C
C******************************************************************************C

        nbytes_line   = isiz1*5*8
        nbytes_plane5 = isiz1*isiz2_row*5*8
        nbytes_plane6 = isiz1*isiz2_row*6*8

        nbytes_row1 = nynriv*nxnriv*5*8
        nbytes_row2 = nznriv*nxnriv*5*8
        nbytes_row3 = nxnriv*nz*5*8
        nbytes_row4 = nynriv*nz*5*8

        nbytes_lrow = nxnriv*5*8

C******************************************************************************C
C******************************************************************************C

        c1c2  = c1*c2
        c1c5  = c1*c5
        c3c4  = c3*c4
        c1345 = c1c5*c3c4

        conz1 = 0.5d0*(1.d0-c1c5)

        c3c4a = con43*c3c4
        
        c3c4aa = c3c4  - c1345
        c3c4bb = c3c4a - c1345
        
C******************************************************************************C
C******************************************************************************C

        dxi   = dnxm1
        deta  = dnym1
        dzeta = dnzm1

        tx1 = 1.d0 / (dxi * dxi)
        tx2 = 1.d0 / (2.d0 * dxi)
        tx3 = 1.d0 / dxi

        ty1 = 1.d0 / (deta * deta)
        ty2 = 1.d0 / (2.d0 * deta)
        ty3 = 1.d0 / deta

        tz1 = 1.d0 / (dzeta * dzeta)
        tz2 = 1.d0 / (2.d0 * dzeta)
        tz3 = 1.d0 / dzeta

C******************************************************************************C
C******************************************************************************C
C*** diffusion coefficients

        dx1 = 0.75d0
        dx2 = dx1
        dx3 = dx1
        dx4 = dx1
        dx5 = dx1

        dy1 = 0.75d0
        dy2 = dy1
        dy3 = dy1
        dy4 = dy1
        dy5 = dy1

        dz1 = 1.0d0
        dz2 = dz1
        dz3 = dz1
        dz4 = dz1 
        dz5 = dz1

C******************************************************************************C
C******************************************************************************C
C*** fourth difference dissipation 

        dssp = (max (dx1, dy1, dz1)) / 4.d0

        c4dssp = 4.d0*dssp
        c5dssp = 5.d0*dssp

C******************************************************************************C
C******************************************************************************C
C***  my new constants for sp code.

        dttx1 = dt*tx1
        dttx2 = dt*tx2

        dtty1 = dt*ty1
        dtty2 = dt*ty2
           
        dttz1 = dt*tz1
        dttz2 = dt*tz2

        c2dttx2 = c2*dttx2
        c2dtty2 = c2*dtty2
        c2dttz2 = c2*dttz2
           
        c1dttx2 = c1*dttx2
        c1dtty2 = c1*dtty2
        c1dttz2 = c1*dttz2
           
        dttx1dx1 = dttx1*dx1
        dttx1dx2 = dttx1*dx2
        dttx1dx3 = dttx1*dx3
        dttx1dx4 = dttx1*dx4
        dttx1dx5 = dttx1*dx5

        dtty1dy1 = dtty1*dy1
        dtty1dy2 = dtty1*dy2
        dtty1dy3 = dtty1*dy3
        dtty1dy4 = dtty1*dy4
        dtty1dy5 = dtty1*dy5

        dttz1dz1 = dttz1*dz1
        dttz1dz2 = dttz1*dz2
        dttz1dz3 = dttz1*dz3
        dttz1dz4 = dttz1*dz4
        dttz1dz5 = dttz1*dz5

        dtdssp = dt*dssp
           
        comz1  = dtdssp
        comz4  = 4.d0*dtdssp
        comz5  = 5.d0*dtdssp
        comz6  = 6.d0*dtdssp
           
        c3c4tx3 = c3c4*tx3
        c3c4ty3 = c3c4*ty3
        c3c4tz3 = c3c4*tz3

        dx1tx1 = dx1*tx1
        dx2tx1 = dx2*tx1
        dx3tx1 = dx3*tx1
        dx4tx1 = dx4*tx1
        dx5tx1 = dx5*tx1
        
        dy1ty1 = dy1*ty1
        dy2ty1 = dy2*ty1
        dy3ty1 = dy3*ty1
        dy4ty1 = dy4*ty1
        dy5ty1 = dy5*ty1
        
        dz1tz1 = dz1*tz1
        dz2tz1 = dz2*tz1
        dz3tz1 = dz3*tz1
        dz4tz1 = dz4*tz1
        dz5tz1 = dz5*tz1
        
        xxcon1 = c3c4tx3*con43*tx3
        xxcon2 = c3c4tx3*tx3
        xxcon3 = c3c4tx3*conz1*tx3
        xxcon4 = c3c4tx3*con16*tx3
        xxcon5 = c3c4tx3*c1c5*tx3

        yycon1 = c3c4ty3*con43*ty3
        yycon2 = c3c4ty3*ty3
        yycon3 = c3c4ty3*conz1*ty3
        yycon4 = c3c4ty3*con16*ty3
        yycon5 = c3c4ty3*c1c5*ty3

        zzcon1 = c3c4tz3*con43*tz3
        zzcon2 = c3c4tz3*tz3
        zzcon3 = c3c4tz3*conz1*tz3
        zzcon4 = c3c4tz3*con16*tz3
        zzcon5 = c3c4tz3*c1c5*tz3

C******************************************************************************C
C******************************************************************************C
C*** coefficients of the exact solution to the first pde

        ce(1,1)  = 2.d0
        ce(1,2)  = 0.d0
        ce(1,3)  = 0.d0
        ce(1,4)  = 4.d0
        ce(1,5)  = 5.d0
        ce(1,6)  = 3.d0
        ce(1,7)  = 5.0d-01
        ce(1,8)  = 2.0d-02
        ce(1,9)  = 1.0d-02
        ce(1,10) = 3.0d-02
        ce(1,11) = 5.0d-01
        ce(1,12) = 4.0d-01
        ce(1,13) = 3.0d-01

C*** coefficients of the exact solution to the second pde

        ce(2,1)  = 1.d0
        ce(2,2)  = 0.d0
        ce(2,3)  = 0.d0
        ce(2,4)  = 0.d0
        ce(2,5)  = 1.d0
        ce(2,6)  = 2.d0
        ce(2,7)  = 3.d0
        ce(2,8)  = 1.0d-02
        ce(2,9)  = 3.0d-02
        ce(2,10) = 2.0d-02
        ce(2,11) = 4.0d-01
        ce(2,12) = 3.0d-01
        ce(2,13) = 5.0d-01

C*** coefficients of the exact solution to the third pde

        ce(3,1)  = 2.d0
        ce(3,2)  = 2.d0
        ce(3,3)  = 0.d0
        ce(3,4)  = 0.d0
        ce(3,5)  = 0.d0
        ce(3,6)  = 2.d0
        ce(3,7)  = 3.d0
        ce(3,8)  = 4.0d-02
        ce(3,9)  = 3.0d-02
        ce(3,10) = 5.0d-02
        ce(3,11) = 3.0d-01
        ce(3,12) = 5.0d-01
        ce(3,13) = 4.0d-01

C*** coefficients of the exact solution to the fourth pde

        ce(4,1)  = 2.d0
        ce(4,2)  = 2.d0
        ce(4,3)  = 0.d0
        ce(4,4)  = 0.d0
        ce(4,5)  = 0.d0
        ce(4,6)  = 2.d0
        ce(4,7)  = 3.d0
        ce(4,8)  = 3.0d-02
        ce(4,9)  = 5.0d-02
        ce(4,10) = 4.0d-02
        ce(4,11) = 2.0d-01
        ce(4,12) = 1.0d-01
        ce(4,13) = 3.0d-01

C*** coefficients of the exact solution to the fifth pde

        ce(5,1)  = 5.d0
        ce(5,2)  = 4.d0
        ce(5,3)  = 3.d0
        ce(5,4)  = 2.d0
        ce(5,5)  = 1.0d-01
        ce(5,6)  = 4.0d-01
        ce(5,7)  = 3.0d-01
        ce(5,8)  = 5.0d-02
        ce(5,9)  = 4.0d-02
        ce(5,10) = 3.0d-02
        ce(5,11) = 1.0d-01
        ce(5,12) = 3.0d-01
        ce(5,13) = 2.0d-01

C******************************************************************************C
C******************************************************************************C

        iwcheck = 0
        if ((my_node.eq.node_wr).and.(idowr.eq.1)) iwcheck = 1

        do 20000 irun = 1, numruns

           if (my_node.eq.node_wr) write (6,*) 'run number = ',irun

           do 20100 m = 1, 5
              do 20200 j = 1, ny
                 do 20300 i = 1, nx
                    frct(i,j,m) = 0.d0
                    u(i,j,m)    = 0.d0
                    rsd(i,j,m)  = 0.d0
20300            continue
20200         continue
20100      continue

           call gsync ()

C*** timing.
           
           tcomm     = 0.d0
           tcalrhs   = 0.d0
           txchng1   = 0.d0
           txchng2   = 0.d0
           time_wait = 0.d0

C***  compile and save the list of exchanging nodes for my_node.

           call compnode

           if (iwcheck.eq.1) write (6,*) 'passed compnode'

C***  set the boundary and the initial values for dependent variables
           
           call setbviv

           if (iwcheck.eq.1) write (6,*) 'passed setbviv'

C*** compute the forcing term based on prescribed exact solution

           call erhs

           if (iwcheck.eq.1) write (6,*) 'passed erhs'

           call xyTxz2

           if (iwcheck.eq.1) then
              write (6,*) 'passed xytxz2 before adi'
           endif

C*** perform scalar approximate factorization iterations

           call btadi

           if (iwcheck.eq.1) write (6,*) 'passed btadi'

C*** compute the solution error

           call error

           if (iwcheck.eq.1) write (6,*) 'passed error'
        
C*** compute the surface integral

           call pintgr

           if (iwcheck.eq.1) write (6,*) 'passed pintgr'

C*** verification test

           if (my_node.eq.node_wr) then
              call  btverify (rsdnm,errnm,frc)
           endif

           if (iwcheck.eq.1) write (6,*) 'passed pintgr'

           ttalk = tcomm + txchng1 + txchng2
           
           if (my_node.eq.node_wr) then
              
              write (6,*) 'my_node number of steps = ',
     &             my_node,istep*2
              write (6,*) 'ttotal ttalk tcalrhs = ',
     &             ttotal,ttalk,tcalrhs
              write (6,*) 'tcomm txchng1 txchng2 = ',
     &             tcomm,txchng1,txchng2

           endif
           
           ttotal_min = ttotal
           ttotal_max = ttotal
           
           call gdlow  (ttotal_min,1,timetmp)
           call gdhigh (ttotal_max,1,timetmp)
           
           tcomm_min = tcomm
           tcomm_max = tcomm
           
           call gdlow  (tcomm_min,1,timetmp)
           call gdhigh (tcomm_max,1,timetmp)
           
           txchng1_min = txchng1
           txchng1_max = txchng1
           
           call gdlow  (txchng1_min,1,timetmp)
           call gdhigh (txchng1_max,1,timetmp)
           
           txchng2_min = txchng2
           txchng2_max = txchng2
           
           call gdlow  (txchng2_min,1,timetmp)
           call gdhigh (txchng2_max,1,timetmp)
           
           ttwait_min = time_wait
           ttwait_max = time_wait
           
           call gdlow  (ttwait_min,1,timetmp)
           call gdhigh (ttwait_max,1,timetmp)
           
           tcalrhs_min = tcalrhs
           tcalrhs_max = tcalrhs
           
           call gdlow  (tcalrhs_min,1,timetmp)
           call gdhigh (tcalrhs_max,1,timetmp)
           
           ttalk_min = ttalk
           ttalk_max = ttalk
           
           call gdlow  (ttalk_min,1,timetmp)
           call gdhigh (ttalk_max,1,timetmp)

           if (my_node.eq.node_wr) then
              
              write (6,7001) ttotal_min,ttotal_max
              write (6,7002) tcalrhs_min,tcalrhs_max
              write (6,7003) tcomm_min,tcomm_max
              write (6,7004) txchng1_min,txchng1_max
              write (6,7005) txchng2_min,txchng2_max
              write (6,7006) ttwait_min,ttwait_max
              write (6,7007) ttalk_min,ttalk_max

           endif

20000   continue
        
	stop

C******************************************************************************C
C******************************************************************************C

 1001   format (//5x,'Total CPU time = ',1pe12.4,' Sec. ')
 2001   format (5x,'PROBLEM SIZE IS TOO SMALL - ',
     $       /5x,'SET EACH OF NX, NY AND NZ AT LEAST EQUAL TO 5')
 2002   format (5x,'PROBLEM SIZE IS TOO LARGE - ',
     $       /5x,'NX, NY AND NZ = ',3(i5,2x),
     &       /5x,'SHOULD BE LESS THAN OR EQUAL TO ',
     $       /5x,'ISIZ1, ISIZ2 AND ISIZ3 RESPECTIVELY = '
     &       /5x,3(i5,2x))
 901    format (1x)

 7001   format(5x,'min and max total time               = ',2(e12.5,2x))
 7002   format(5x,'min and max time for rhs             = ',2(e12.5,2x))
 7003   format(5x,'min and max line communication time  = ',2(e12.5,2x))
 7004   format(5x,'min and max plane comm. time for rhs = ',2(e12.5,2x))
 7005   format(5x,'min and max plane comm. time for row = ',2(e12.5,2x))
 7006   format(5x,'min and max wait time for comm.      = ',2(e12.5,2x))
 7007   format(5x,'min and max total communication time = ',2(e12.5,2x))

        end

C******************************************************************************C
C******************************************************************************C
