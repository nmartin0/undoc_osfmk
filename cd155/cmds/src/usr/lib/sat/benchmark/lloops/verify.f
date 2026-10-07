C
C***********************************************
      SUBROUTINE VERIFY( iou )
C***********************************************************************
C                                                                      *
C      VERIFY     auxiliary test routine to check-out function SECOND  *
C                 and to verify that sufficiently long Loop sizes are  *
C                 defined in Subr. SIZES for accurate CPU timing.      *
C                                                                      *
C       iou    -  Logical Output Device Number                         *
C                                                                      *
C***********************************************************************
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,O-Z)
C
CLOX  REAL*8 SECOND
C
C/C/      PARAMETER( l1=   1001, l2=   101, l1d= 2*1001 )
C/C/      PARAMETER( l13= 64, l13h= 64/2, l213= 64+32, l813= 8*64 )
C/C/      PARAMETER( l14= 2048, l16= 75, l416= 4*75 , l21= 25)
C/C/      PARAMETER( kn= 47, kn2= 95, np= 3, ls= 3*47, krs= 24)
C
      parameter( ntmp= 100 )
C
      COMMON /SPACE1/ U(1001), V(1001), W(1001),
     1  X(1001), Y(1001), Z(1001), G(1001),
     2  DU1(101), DU2(101), DU3(101), GRD(1001), DEX(1001),
     3  XI(1001), EX(1001), EX1(1001), DEX1(1001),
     4  VX(1001), XX(1001), RX(1001), RH(2048),
     5  VSP(101), VSTP(101), VXNE(101), VXND(101),
     6  VE3(101), VLR(101), VLIN(101), B5(101),
     7  PLAN(300), D(300), SA(101), SB(101)
C
      COMMON /SPACE2/ P(4,512), PX(25,101), CX(25,101),
     1  VY(101,25), VH(101,7), VF(101,7), VG(101,7), VS(101,7),
     2  ZA(101,7)  , ZP(101,7), ZQ(101,7), ZR(101,7), ZM(101,7),
     3  ZB(101,7)  , ZU(101,7), ZV(101,7), ZZ(101,7),
     4  B(64,64), C(64,64), H(64,64),
     5  U1(5,101,2),  U2(5,101,2),  U3(5,101,2)
C
      COMMON /ALPHA/ mk,ik,im,ml,il,Mruns,Nruns,jr,iovec,NPFS(8,3,47)
      COMMON /TAU/   tclock, tsecov, testov, cumtim(4)
C
      COMMON /BETA / tic, TIMES(8,3,47), SEE(5,3,8,3),
     1              TERRS(8,3,47), CSUMS(8,3,47),
     2              FOPN(8,3,47), DOS(8,3,47)
C
      COMMON /SPACES/ ion,j5,k2,k3,MULTI,laps,Loop,m,kr,LP,n13h,ibuf,nx,
     1 L,npass,nfail,n,n1,n2,n13,n213,n813,n14,n16,n416,n21,nt1,nt2,
     2 last,idebug,mpy,Loops2,mucho,mpylim, intbuf(16)
C
      COMMON /SPACEI/ WTP(3), MUL(3), ISPAN(47,3), IPASS(47,3)
C
C
      DIMENSION  TIM(ntmp), TUM(ntmp), TAV(ntmp), TER(ntmp)
      DIMENSION  TMX(ntmp), SIG(ntmp), LEN(ntmp)
C
C
C     CALL TRACE ('VERIFY  ')
c
      DO  1 k = 1,101
          X(k)= 0.0d0
          Y(k)= 0.0d0
    1  CX(1,k)= 0.0d0
           nzd= 0
C
C***********************************************************************
C     Measure tsecov:  Overhead time for calling SECOND
C***********************************************************************
C
      tsecov = SECOVT( iou)
         tic = tsecov
C
C***********************************************************************
C     Measure time resolution of cpu-timer;  tclock= MIN t
C***********************************************************************
C
        fuzz= 1.00d-12
      nticks= INT( 1.00d0/( tsecov + fuzz ))
          IF( nticks.LT.1000 ) nticks= 1000
          dt= 0.00d0
          t1= SECOND( cum)
           m= 0
C
      DO 2 k= 1,nticks
          t2= SECOND( cum)
          IF( t2 .NE. t1 ) THEN
                  m= m + 1
                 dt= dt + ( t2 - t1 )
                 t1= t2
                 IF( m .GE. 200 ) GO TO 3
          ENDIF
    2 continue
C
    3     IF( m.LE.2 ) THEN
              tclock= 1.00d0
              WRITE(   *,163)
              WRITE( iou,163)
          ELSE
              tclock= dt/( REAL(m) + fuzz )
          ENDIF
C
       WRITE(   *,164) m, tclock
       WRITE( iou,164) m, tclock
  163 FORMAT(1X,'WARNING(VERIFY): POOR Cpu-timer resolution; REPLACE?')
  164 FORMAT('VERIFY:',I10,E12.4,' =  Time Resolution of Cpu-timer')
C
C****************************************************************************
C         VERIFY ADEQUATE Loop SIZE VERSUS CPU CLOCK ACCURACY
C****************************************************************************
C
C         VERIFY produced the following output on CRAY-XMP4 in a
C         fully loaded, multi-processing, multi-programming system:
C
C
C         VERIFY ADEQUATE Loop SIZE VERSUS CPU CLOCK ACCURACY
C         -----     -------     -------    -------   --------
C         EXTRA     MAXIMUM     DIGITAL    DYNAMIC   RELATIVE
C         Loop      CPUTIME     CLOCK      CLOCK     TIMING
C         SIZE      SECONDS     ERROR      ERROR     ERROR
C         -----     -------     -------    -------   --------
C             1  5.0000e-06      10.00%     17.63%     14.26%
C             2  7.0000e-06       7.14%      6.93%      4.79%
C             4  1.6000e-05       3.12%      6.56%      7.59%
C             8  2.8000e-05       1.79%      2.90%      2.35%
C            16  6.1000e-05       0.82%      6.72%      4.50%
C            32  1.1700e-04       0.43%      4.21%      4.62%
C            64  2.2700e-04       0.22%      3.13%      2.41%
C           128  4.4900e-04       0.11%      3.14%      0.96%
C           256  8.8900e-04       0.06%      2.06%      2.50%
C           512  1.7740e-03       0.03%      1.92%      1.59%
C          1024  3.4780e-03       0.01%      0.70%      1.63%
C          1360              Current Run:    MULTI=   10.000
C          2048  7.0050e-03       0.01%      0.74%      1.28%
C          4096  1.3823e-02       0.00%      1.35%      0.78%
C         -----     -------     -------    -------   --------
C
C          Approximate Serial Job Time=   2.5e+01 Sec.    ( Nruns= 7 RUNS)
C
C****************************************************************************
C
                WRITE( iou,45)
                WRITE( iou,49)
                WRITE( iou,46)
                WRITE( iou,47)
                WRITE( iou,48)
                WRITE( iou,49)
   45 FORMAT(/,8X,'VERIFY ADEQUATE Loop SIZE VERSUS CPU CLOCK ACCURACY')
   46 FORMAT(8X,'EXTRA     MAXIMUM     DIGITAL    DYNAMIC   RELATIVE')
   47 FORMAT(8X,'Loop      CPUTIME     CLOCK      CLOCK     TIMING  ')
   48 FORMAT(8X,'SIZE      SECONDS     ERROR      ERROR     ERROR   ')
   49 FORMAT(8X,'-----     -------     -------    -------   --------')
C
C
C****************************************************************************
C     Measure Cpu Clock Timing Errors As A Function Of Loop Size(lo)
C****************************************************************************
C
         ttest= 100.00d0 * tclock
        ilimit= 30
            nj= 5
            lo= 128
             i= 0
C
   10        i= i + 1
            lo= lo + lo
      DO 53  j= 1,nj
             n= 100
      cumtim(1)= 0.0d0
             t0= SECOND( cumtim(1))
c                                    Time Kernel 12
      DO 12 m = 1,lo
      DO 12 k = 1,n
   12     X(k)= X(k+1) - X(k)
c
      cumtim(1)= 0.0d0
         TIM(j)= SECOND( cumtim(1)) - t0 - tsecov
   53 continue
c                                    Compute Dynamic Clock Error
c
          CALL  STATS( TUM, TIM, nj)
         rterr= 100.0*( TUM(2)/( TUM(1) + fuzz ))
            IF( TUM(1).LE. 0.00d0)  rterr= 100.00d0
c
c                                    Compute Digital Clock Error
c
          CALL  TDIGIT( SIG(i), nzd, TUM(4))
C
        TAV(i)= TUM(1)
        TMX(i)= TUM(4)
        TER(i)= rterr
        LEN(i)= lo
      IF( i.GT.ilimit .AND. ( TUM(1).LT.fuzz )) THEN
      WRITE(  *,146)  lo, TUM(1)
  146 FORMAT('VERIFY:',I12,' Repetitions.  Bad Timer=',E14.5,' sec.')
      ENDIF
      IF( i.LE.8 .OR.  ( TUM(1).LT.ttest .AND. i.LT.ntmp )) GO TO 10
            nn= i
C
C****************************************************************************
C     Compute Multiple-Pass Loop Counters MULTI and Loops2
C     Such that:  each Kernel is run at least 100 ticks of Cpu-timer.
C****************************************************************************
C
          i2= 2
       MULTI= 1
       mucho= 1
        CALL  SIZES(12)
      loop12= IPASS(12,2) * MUL(2)
C
      MULTI= INT( (REAL(lo)/(REAL(loop12)+fuzz))*(ttest/(TUM(1)+fuzz)))
      mucho= MULTI
C
C     If timing errors are too large, you must increase MULTI...
C     When MULTI.GE.100 each kernel is executed over a million times
C     and the time used to re-intialize overstored input variables
C     is negligible.  Thus each kernel may be run arbitrarily many times
C     (MULTI >> 100) without overflow and produce verifiable checksums.
c
C     Each kernel's results are automatically checksummed for  MULTI :=
c
C     MULTI=   1      clock resolution << 0.01 SEC,  or Cpu << 1 Mflops
C     MULTI=  10      clock resolution << 0.01 SEC,  or Cpu <  2 Mflops
C     MULTI=  50      clock resolution <= 0.01 SEC,  or Cpu <  2 Mflops
C     MULTI= 100      clock resolution <= 0.01 SEC,  or Cpu <  5 Mflops
C     MULTI= 200      clock resolution <= 0.01 SEC,  or Cpu < 10 Mflops
C
c     MULTI=   1
c     MULTI=  10
c     MULTI=  50
c     MULTI= 100
c     MULTI= 200
C
                 mpy= 1
              Loops2= 1
              mpylim= Loops2
          IF( MULTI.LE.  1 ) THEN
              MULTI =    1
      ELSEIF( MULTI.LE. 10 ) THEN
              MULTI =   10
      ELSEIF( MULTI.LE. 50 ) THEN
              MULTI =   50
      ELSEIF( MULTI.LE.100 ) THEN
              MULTI =  100
      ELSE
              Loops2= (MULTI + 50)/100
              mpylim= Loops2
              MULTI =  100
      ENDIF
C
C
         mucho= MULTI
        loops0= loop12 * MULTI * Loops2
        repeat= REAL(    MULTI * Loops2 )
            IF( Loop.EQ.1 ) repeat= 1.00d0/( REAL( loop12) + fuzz)
C
C****************************************************************************
C     Estimate Timing Error By Comparing Time Of Each Run With Longest Run
C****************************************************************************
C
             m= 0
           tnn= ( TAV(nn) + 2.00d0* TAV(nn-1))* 0.500d0
          fuzz= 1.0d-12
            IF( tnn.LT.fuzz)  tnn= fuzz
      DO 69  i= 1,nn
         rterr= TER(i)
            lo= LEN(i)
c                                    Compute Relative Clock Error
c
            rt= 0.0d0
            IF( LEN(i).GE. 0)     rt= LEN(nn)/LEN(i)
         rperr= 100.00d0
            IF( tnn.GT.fuzz) rperr= 100.00d0*(ABS( tnn - rt*TAV(i))/tnn)
         WRITE( iou,64) lo, TMX(i), SIG(i),rterr, rperr
   64   FORMAT(6X,I7,E12.4,F11.2,1H%,F10.2,1H%,F10.2,1H%)
c
c                                    Find loops0 Size Used
c
            IF( (loops0.GE.lo) .AND. (loops0.LE.2*lo))  THEN
                     m= lo
                WRITE( iou,66)  loops0, repeat
                WRITE(   *,66)  loops0, repeat
                IF( rterr .GT. 10.00d0)  THEN
                  WRITE( iou, 67)
                  WRITE( iou, 68)
                  WRITE(   *, 67)
                  WRITE(   *, 68)
                ENDIF
   66 FORMAT(7X,i6,7X,'Repetition Count = MULTI * Loops2 = ',F12.3)
   67 FORMAT(34X,46HVERIFY: POOR TIMING OR ERROR. NEED LONGER RUN  )
   68 FORMAT(34X,34HINCREASE:   MULTI  IN SUBR. VERIFY     )
            ENDIF
C
   69 continue
            IF( m.LE.0 )  THEN
                WRITE( iou,66)  loops0, repeat
                WRITE(   *,66)  loops0, repeat
            ENDIF
                WRITE( iou,49)
C
C
      WRITE(   *,991)
      WRITE(   *,992)
  991 FORMAT(/,16X,'    (C) Copyright 1983 the Regents of the     ')
  992 FORMAT(  16X,'University of California. All Rights Reserved.',/)
C
C****************************************************************************
C     Clock Calibration Test of Internal Cpu-timer SECOND;
C           Verify 10 Internal SECOND Intervals using External Stopwatch
C****************************************************************************
C
C
  106 FORMAT(//,' CLOCK CALIBRATION TEST OF INTERNAL CPU-TIMER: SECOND')
  107 FORMAT(' MONOPROCESS THIS TEST, STANDALONE, NO TIMESHARING.')
  108 FORMAT(' VERIFY TIMED INTERVALS SHOWN BELOW USING EXTERNAL CLOCK')
  109 FORMAT(' START YOUR STOPWATCH NOW !')
  113 FORMAT(/,11X,'Verify  T or DT  observe external clock(sec):',/)
  114 FORMAT('           -------     -------      ------      -----')
  115 FORMAT('           Total T ?   Delta T ?    Mflops ?    Flops')
  119 FORMAT(4X,I2,3F12.2,2E15.5)
  120 FORMAT(' END CALIBRATION TEST.',/)
          WRITE( iou,106)
          WRITE( iou,107)
          WRITE( iou,108)
          WRITE( iou,109)
          WRITE( iou,113)
          WRITE( iou,114)
          WRITE( iou,115)
          WRITE( iou,114)
          WRITE(   *,106)
          WRITE(   *,107)
          WRITE(   *,108)
          WRITE(   *,109)
          WRITE(   *,113)
          WRITE(   *,114)
          WRITE(   *,115)
          WRITE(   *,114)
C
           task= 10.00d0
         passes= REAL(lo) * ( task/( tnn + fuzz))
         loiter= INT( passes )
          flops= 0.00d0
      cumtim(1)= 0.0d0
             t1= SECOND( cumtim(1))
             t2= 0.00d0
C
      DO 86   j= 1,4
              n= 100
             t0= t1
c                                    Time Kernel 12
      DO 82  m = 1,loiter
      DO 82  k = 1,n
   82      X(k)= X(k+1) - X(k)
c
      cumtim(1)= 0.0d0
             t1= SECOND( cumtim(1))
             td= t1 - t0 -tsecov
             t2= t2 + td
          flops= flops + passes*REAL(n)
         ratemf= ( 1.00d-6 * flops )/( t2 + fuzz )
          WRITE(   *,119)  j, t2, td, ratemf, flops
          WRITE( iou,119)  j, t2, td, ratemf, flops
   86 continue
          WRITE( iou,114)
          WRITE( iou,120)
          WRITE(   *,114)
          WRITE(   *,120)
C
C     CALL TRACK ('VERIFY  ')
      RETURN
      END
