C
C
C**********************************************
      FUNCTION   SECOND( OLDSEC)
C***********************************************
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,O-Z)
CLOX  REAL*8 SECOND
C
C     SECOND= Cumulative CPU time for job in seconds.  MKS unit is seconds.
C             Clock resolution should be less than 2% of Kernel 11 run-time.
C             ONLY CPU time should be measured, NO system or I/O time included.
C             In VM systems, page-fault time must be avoided (Direction 8).
C             SECOND accuracy may be tested by calling: CALIBR test.
C
C     IF your system provides a timing routine that satisfies
C     the definition above; THEN simply delete this function.
C
C     ELSE this function must be programmed using some
C     timing routine available in your system.
C     Timing routines with CPU-clock resolution are always  sufficient.
C     Timing routines with microsec. resolution are usually sufficient.
C
C     Timing routines with much less resolution have required the use
C     of multiple-pass loops around each kernel to make the run time
C     at least 50 times the tick-period of the timing routine.
C     Function SECOVT measures the overhead time for a call to SECOND.
C
C     If no CPU timer is available, then you can time each kernel by
C     the wall clock using the PAUSE statement at the end of func. TEST.
C
C     An independent calibration of the running time may be wise.
C     Compare the Total Job Cpu Time printout at end of the LFK output file
C     with the job Cpu time charged by your operating system.
C
C     Default, uni-processor tests measure job  Cpu-time in SECOND (TSS mode).
C     Parallel processing tests should measure Real-time in stand-alone mode.
C
C     The following statement is deliberately incomplete:
C
c      SECOND=                                                            sdef
C               USE THE HIGHEST RESOLUTION CPU-TIMER FUNCTION AVAILABLE
C*******************************************************************************
C
C     The following statements were used on  UNIX 4.2bsd systems, e.g.  SUN
C     Time Resolution of ETIME is poor= 0.01 Sec.
C
cintel   REAL*4 CPUTYM(4), ETIME                                          unix
cintel   XT= ETIME( CPUTYM)                                               unix
cintel   SECOND=    CPUTYM(1)                                             unix
         real*8 cputym,dclock
	 cputym = dclock()
	 second = cputym
C
C or
c        REAL*4 XTIME(4)                                                  unix
c        INTEGER    CLOCK                                                 unix
c        EXTERNAL   CLOCK                                                 unix
c        XT = REAL( CLOCK( XTIME)) * 1.00d-6                              unix
c        SECOND=  XT                                                      unix
C
C*******************************************************************************
C
C     The following statements were used on the DEC  VAX/780  VMS 3.0 .
C     Enable page-fault tallys in TEST by un-commenting LIB$STAT_TIMER calls.
C     Clock resolution is 0.01 Sec.
C
C       DATA  INITIA   /123/
C       IF(   INITIA.EQ.123 )  THEN
C             INITIA= 1
C             NSTAT = LIB$INIT_TIMER()
C       ELSE
C             NSTAT = LIB$STAT_TIMER(2,ISEC)
C             SECOND= REAL(ISEC)*0.01 - OLDSEC
C       ENDIF
C
C* OR less accurately:
C*        REAL    SECNDS
C*        SECOND= SECNDS( OLDSEC)
C
C*****************************************************************************
C
C     The following statements were used on the IBM RS/6000
C     Contrary to what the manual states, INTEGER FUNCTION MCLOCK()
C     returns the number of ticks with 100 ticks being one second.
C
CIBMRS          integer itemp, MCLOCK
CIBMRS          external MCLOCK
C
CIBMRS          itemp = MCLOCK()
CIBMRS          SECOND= REAL(itemp)/100.00d0
C
C*******************************************************************************
C     The following statements were used on the DEC PDP-11/23 RT-11 system.
C
C*       DIMENSION JT(2)
C*       CALL GTIM(JT)
C*       TIME1 = JT(1)
C*       TIME2 = JT(2)
C*       TIME = TIME1 * 65768. + TIME2
C*       SECOND=TIME/60. - OLDSEC
C*******************************************************************************
C
C     The following statements were used on the Hewlett-Packard HP 9000
C
C*       INTEGER*4 ITIME(4)
C*       CALL TIMES( ITIME(4))
C*       TIMEX= ITIME(1) + ITIME(2) + ITIME(3) + ITIME(4)
C*       SECOND= TIMEX/60. - OLDSEC
C
C*******************************************************************************
C
C     FOR THE GOULD 32/87 WITH MPX 3.2  (et seq. gratis D.Lindsay)
C
C     INTEGER*4 NSEC, NCLICK
C     REAL*8 CPUTIM
C
C      CALL M:CLOCK (NSEC, NCLICK)
C      CPUTIM = FLOAT(NSEC)
C      SECOND = CPUTIM + FLOAT(NCLICK)/60.
C
C*******************************************************************************
C
C  FOR THE HP 1000 RUNNING FORTRAN 77.
C  note that since the hp operating system has no facility for
C  returning cpu time, this routine only measures elapsed time.
C  therefore, the tests must be run stand-alone.
C
C     REAL*8 TOTIME
C     INTEGER*2 TIMEA(5)
C
C     CALL EXEC (11, TIMEA)
C     TOTIME = DBLE (TIMEA(1))/100.
C     TOTIME = TOTIME + DBLE (TIMEA(2))
C     TOTIME = TOTIME + DBLE (TIMEA(3)) * 60.
C     SECOND = TOTIME + DBLE (TIMEA(4)) * 3600.
C
C*******************************************************************************
C
C     FOR THE PR1ME SYSTEM UNDER PRIMOS
C
C     REAL*8 CPUTIM
C     INTEGER*2 TIMERS (28)
C
C     CALL TMDAT (TIMERS)
C     SECOND = DBLE (TIMERS(7))
C    .+ DBLE(TIMERS(8)) / DBLE(TIMERS(11))
C
C*******************************************************************************
C
C     The following statements were used on the Stellar
C
C      REAL DUMMY(8)
C      INTEGER*4 TIMES$
C      SAVE IOFSET
C      ITIME= TIMES$( DUMMY)
C      IF( IOFSET.EQ.0 )  IOFSET= ITIME
C      SECOND= (ITIME - IOFSET)/100.0  - OLDSEC
C*******************************************************************************
C
C     The following statements were used on the IBM 3090 VM system.
C     Clock resolution is 1 microsec.
C
C      SECOND= IOCPU(0.0d0)* 1.0d-6
C
C*******************************************************************************
C
C     The following statement was used on the IBM 3090  MVS
C
C**   CALL TODD( xtime)
c     TODD returns microsecs in REAL*8 form
c     TODD provides 1/16th of a microsecond precision
C**   xtime = xtime * 1.0D-6
C     SECOND= xtime - oldsec
C
C********************************
C     REAL*4 TIME(4)
C     xtime = 0.0D-6
C     CALL VCLOCK(time(1))
C     xtime = time(1)
C     SECOND= xtime - oldsec
C
C********************************
C     The following statement was used on the IBM 4381, 9370
C
C     real*8 elapsed(2),cpu(2)
C     call timer(elapsed,cpu)
C     second = cpu(1) - oldsec
C
C
C*******************************************************************************
C
C     The following statements were used on the IBM PC Professional Fortran.
C     Clock resolution is 0.01 Sec.
C
C      INTEGER*2 IHR,IMIN,ISEC,IS100
C      CALL GETTIM(IHR,IMIN,ISEC,IS100)
C      ISECT=(JFIX(IHR)*60+JFIX(IMIN))*60+JFIX(ISEC)
C      SECOND=FLOAT(ISECT)+FLOAT(IS100)/100.0
C
C*******************************************************************************
C
C     THE FOLLOWING STATEMENTS ARE USED ON IBM-PC WITH LAHEY COMPILER
C**   SECOND= REAL( MOD( ITICKS, 1000000)) * 1.0D-2
C
C**   INTEGER*4   ITICKS
C**   CALL TIMER( ITICKS)
C**   SECOND= REAL( ITICKS ) * 1.0D-2
C
C      INTEGER*4  I1, ITICK0, ITICKS
C      SAVE I1, ITICK0
C      DATA I1/-357/, ITICK0/0/
CC
C      IF(  I1.EQ.(-357)) THEN
C         CALL  TIMER( ITICK0)
C      ENDIF
C           I1 = 7
C         CALL  TIMER( ITICKS)
C       SECOND = REAL( ITICKS - ITICK0 ) * 1.0D-2
C
C
C*******************************************************************************
C
C  FOR THE IBM PC.
C  note that the pc's operating system has no facility for
C  returning cpu time; this routine only measures elapsed time.
C  also, the pc does not have real*8.  Remove all references to real*8
C
C      IMPLICIT INTEGER*4 (I-N)
C      LOGICAL FIRST
C      DATA FIRST /.TRUE./
C
C      CALL GETTIM (IYEAR, IMONTH, IDAY, IHOUR, IMIN, ISEC, IFRACT)
C
C  ifract is integer fractions of a second
C  in units of 1/32,768 seconds
C
C      IF (.NOT. FIRST) GO TO 10
C        FIRST = .FALSE.
C
C        LASTHR = IHOUR
C        BASETM = 0.
C10    CONTINUE
C
C  because of limited precision, do not include the time of day
C  in hours in the total time.  but correct for an hour change.
C
C      IF (LASTHR .EQ. IHOUR) GO TO 20
C        BASETM = BASETM + 3600.
C        LASTHR = IHOUR
C
C20    TOTIME = FLOAT(IMIN) * 60
C    . + FLOAT(ISEC)
C    . + FLOAT(IFRACT)/32768.
C      SECOND = TOTIME + BASETM
C
C
      RETURN
      END
