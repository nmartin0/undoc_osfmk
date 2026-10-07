C******************************************************************************C
C******************************************************************************C
C***                           verification routine                         ***C
C******************************************************************************C
C******************************************************************************C

        subroutine btverify (xcr, xce, xci)

        include 'appbt.incl'

        dimension xcr(5), xce(5)

        dimension xcrref(5),xceref(5),xcrdif(5),xcedif(5)

C******************************************************************************C
C******************************************************************************C
c***tolerance level

        epsilon = 1.0d-08

        notest = 0
        icase  = 0

C******************************************************************************C
C******************************************************************************C
C*** Reference values of RMS-norms of residual, for the (12X12X12) grid,
C** after 60 time steps, with  DT = 1.0d-02

        if (((nx.eq.12).and.(ny.eq.12).and.(nz.eq.12)).and.
     &       (itmax.eq.60)) then

c*** Reference values of RMS-norms of residual.

           xcrref(1) = 1.7034283709541311d-01
           xcrref(2) = 1.2975252070034097d-02
           xcrref(3) = 3.2527926989486055d-02
           xcrref(4) = 2.6436421275166801d-02
           xcrref(5) = 1.9211784131744430d-01

c*** Reference values of RMS-norms of solution error.

           xceref(1) = 4.9976913345811579d-04
           xceref(2) = 4.5195666782961927d-05
           xceref(3) = 7.3973765172921357d-05
           xceref(4) = 7.3821238632439731d-05
           xceref(5) = 8.9269630987491446d-04

c*** Reference value of surface integral.

           xciref = 7.8414305521809488d0

           notest = 1
           icase  = 1

C******************************************************************************C
C******************************************************************************C
C*** Reference values of RMS-norms of residual, for the (64X64X64) grid,
C*** after 200 time steps, with  DT = 8.0d-04

        elseif (((nx.eq.64).and.(ny.eq.64).and.(nz.eq.64)).and.
     &          (itmax.eq.200)) then

c*** Reference values of RMS-norms of residual.

           xcrref(1) = 1.0806346714637264d+02
           xcrref(2) = 1.1319730901220813d+01
           xcrref(3) = 2.5974354511582465d+01
           xcrref(4) = 2.3665622544678910d+01
           xcrref(5) = 2.5278963211748344d+02

c*** Reference values of RMS-norms of solution error.

           xceref(1) = 4.2348416040525025d+00
           xceref(2) = 4.4390282496995698d-01
           xceref(3) = 9.6692480136345650d-01
           xceref(4) = 8.8302063039765474d-01
           xceref(5) = 9.7379901770829278d+00

c*** Reference value of surface integral.

           xciref = 1.4101360012200832d+01

           notest = 1
           icase  = 2

        endif

C******************************************************************************C
C******************************************************************************C
c*** verification test for residuals if gridsize is either 12X12X12 or 
c*** 64X64X64.

        if (notest.eq.1) then

c*** Compute the difference of solution values and the known reference values.

           do 100 m = 1, 5

              xcrdif(m) = dabs(xcr(m)-xcrref(m)) 
              xcedif(m) = dabs(xce(m)-xceref(m))

 100       continue

           xcidif = dabs(xci-xciref)

C******************************************************************************C
C******************************************************************************C
C*** Output the comparison of computed results to known cases.

           write (6,*) 'accuracy setting for epsilon = ',epsilon
                 
           write (6,*) 'Comparison of RMS-norms of residual'
              
           do 200 m = 1, 5
              write (6,6001) m,xcr(m),xcrref(m),xcrdif(m)
 200       continue
              
           write (6,*) 'Comparison of RMS-norms of solution error'
              
           do 250 m = 1, 5
              write (6,6001) m,xce(m),xceref(m),xcedif(m)
 250       continue
              
           write (6,*) 'Comparison of RMS-norms of surface integral'
              
           write (6,6002) xci,xciref,xcidif
              
c*** Now verify for the solution by comparing the differences with the
c*** setting tolerance epsilon.

           nogo  = 1
           
           nogo1 = 1
           nogo2 = 1
           nogo3 = 1
           
           do 120 m = 1, 5
              
              if (xcrdif(m).gt.epsilon) then
                 
                 write (6,*) 'entry m xcr xcrref xcrdif = ',
     &                m,xcr(m),xcrref(m),xcrdif(m)
                 
                 nogo  = 0
                 nogo1 = 0
                 
              elseif (xcedif(m).gt.epsilon) then
                 
                 write (6,*) 'entry m xce xceref xcedif = ',
     &                m,xcr(m),xceref(m),xcedif(m)
                 
                 nogo  = 0
                 nogo2 = 0
                 
              endif
                 
 120       continue
           
           if (xcidif.gt.epsilon) then
              
              write (6,*) 'xci xciref xcidif = ',
     &             xci,xciref,xcidif
              
              nogo  = 0
              nogo3 = 0
              
           endif
           
c***  Passing the verification if pass all of the tests above.
           
           if (nogo.eq.1) then
              
              write (6,*) 'accuracy setting for epsilon = ',epsilon
              
              write (6,1002)
              
              write (6,1004)
              
              write (6,1006)
              
              if (icase.eq.1) then
                 
                 write (6,1007)
                    
              elseif (icase.eq.2) then
                 
                 write (6,1008)
                    
              endif

c*** Failed if fail either one or all of the above tests.

           elseif (nogo.eq.0) then
              
              if (nogo1.eq.0) write (6,1001)
              
              if (nogo2.eq.0) write (6,1003)
              
              if (nogo3.eq.0) write (6,1005)
              
           endif
           
        elseif (notest.eq.0) then
           
           write (6,1009)
           
           write (6,*) 'accuracy of RMS-norms of residual'
              
           do 300 m = 1, 5
              write (6,6003) m,xcr(m)
 300       continue
           
           write (6,*) 'accuracy of RMS-norms of solution error'
           
           do 350 m = 1, 5
              write (6,6003) m,xce(m)
 350       continue
           
           write (6,*) 'accuracy of RMS-norms of surface integral'
           
           write (6,6004) xci
           
        end if
        
        return

C******************************************************************************C
C******************************************************************************C

 1001   format(/5x,'VERIFICATION TEST FOR RESIDUALS FAILED',
     +     1pe20.13)
 1003   format(/5x,'VERIFICATION TEST FOR SOLUTION ',
     $       'ERRORS FAILED',1pe20.13)
 1005   format (/5x,'VERIFICATION TEST FOR SURFACE INTEGRAL', 
     +       ' FAILED xci =', 1pe20.13)
 1002   format (/5x,'VERIFICATION TEST FOR RESIDUALS ',
     $       'IS SUCCESSFUL')
 1004   format (/5x,'VERIFICATION TEST FOR SOLUTION ERRORS ',
     $       'IS SUCCESSFUL')
 1006   format (/5x,'VERIFICATION TEST FOR SURFACE INTEGRAL ',
     $       'IS SUCCESSFUL')
 1007   format(//10x,'CAUTION',
     $ //5x,'REFERENCE VALUES CURRENTLY IN THIS VERIFICATION ',
     $     'ROUTINE ',
     $ /5x,'ARE VALID ONLY FOR RUNS WITH THE FOLLOWING PARAMETER ',
     $     'VALUES:',
     $ //5x,'NX = 12;  NY = 12;  NZ = 12 ',
     $ //5x,'ITMAX = 100',
     $ //5x,'DT = 1.5d-02',
     $ //5x,'CHANGE IN ANY OF THE ABOVE VALUES RENDER THE REFERENCE ',
     $     'VALUES ',
     $ /5x,'INVALID AND CAUSES A FAILURE OF THE VERIFICATION TEST.')
 1008   format(//10x,'CAUTION',
     $ //5x,'REFERENCE VALUES CURRENTLY IN THIS VERIFICATION ',
     $     'ROUTINE ',
     $ /5x,'ARE VALID ONLY FOR RUNS WITH THE FOLLOWING PARAMETER ',
     $     'VALUES:',
     $ //5x,'NX = 64;  NY = 64;  NZ = 64 ',
     $ //5x,'ITMAX = 400',
     $ //5x,'DT = 1.5d-03',
     $ //5x,'CHANGE IN ANY OF THE ABOVE VALUES RENDER THE REFERENCE ',
     $     'VALUES ',
     $ /5x,'INVALID AND CAUSES A FAILURE OF THE VERIFICATION TEST.')
 1009   format (//1x,'FOR THE PROBLEM PARAMETERS IN USE ',
     $       'NO REFERENCE VALUES ARE PROVIDED'
     $       /1x,'IN THE CURRENT VERIFICATION ROUTINE - ',
     $       'NO VERIFIACTION TEST WAS PERFORMED')
        
 6001   format (i2,3(2x,e20.13))
 6002   format (2x,3(2x,e20.13))

 6003   format (i2,5x,e20.13)
 6004   format (7x,e20.13)

        end

C******************************************************************************C
C******************************************************************************C
