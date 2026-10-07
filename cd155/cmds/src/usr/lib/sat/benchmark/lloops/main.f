*                                            1/6/92
*
*   This past year I have enhanced the LFK test program to automatically
*   increase sample run-timings in proportion to the cpu-clock resolution.
*   The poor resolution of ETIME in UNIX systems had required increasing
*   the run-time limit manually as the speed of workstations increased.
*   Now this LFK test will run dependably, hands-off. 
*
*		     Frank McMahon
*
*
C     PROGRAM DPMFLOPS(TAPE6=OUTPUT)              Double Precision Test
C                    LATEST KERNEL MODIFICATION DATE: 22/DEC/86
C                    LATEST FILE   MODIFICATION DATE: 30/SEP/91 version mf523
C****************************************************************************
C MEASURES CPU PERFORMANCE RANGE OF THE COMPUTATION/COMPILER/COMPUTER COMPLEX
C****************************************************************************
C                                                                           *
C     L. L. N. L.   F O R T R A N   K E R N E L S  T E S T:   M F L O P S   *
C                                                                           *
C                                  Our little systems have their day;       *
C                                  They have their day and cease to be:     *
C                                  They are but broken parts of Thee,       *
C                                  And Thou, O Lord, are more than they.    *
C                                           Alfred, Lord Tennyson (1850)    *
C                                                                           *
C                                                                           *
C     These kernels measure  Fortran  numerical  computation rates for a    *
C     spectrum of  CPU-limited  computational  structures.  Mathematical    *
C     through-put is measured  in  units  of  millions of floating-point    *
C     operations executed per Second, called Mega-Flops/Sec.                *
C                                                                           *
C     The experimental  design  of some traditional  benchmark tests  is    *
C     defective when  applied  to computers employing vector or parallel    *
C     processing because the range of cpu performance is 10 to 100 times    *
C     the range  of conventional, serial processors.  In particular, the    *
C     effective Cpu performance  of supercomputers now ranges from a few    *
C     megaflops to a few thousand megaflops. Attempts by some marketeers    *
C     and decision makers to reduce this three orders of magnitude range    *
C     of cpu  performance  to  a  single  number is unscientific and has    *
C     produced much confusion.   The  LFK  test  also has been abused by    *
C     some analysts who quote only a single, average performance number.    *
C                                                                           *
C     The Livermore  Fortran  Kernels (LFK) test contains a broad sample    *
C     of generic Fortran computations which have been used to measure an    *
C     effective numerical  performance range, thus avoiding the peril of    *
C     a single performance  "rating".   A complete report of 72 LFK test    *
C     results must  quote  six performance range statistics(rates):  the    *
C     minimum, the  harmonic,   geometric,  and  arithmetic  means,  the    *
C     maximum and  the  standard deviation.  No single rate quotation is    *
C     sufficient or  honest.    These   measurements  show  a  realistic    *
C     variance in  Fortran  cpu  performance  that has stood the test of    *
C     time and that is vital data for circumspect computer evaluations.     *
C     Quote statistics from the SUMMARY table of 72 timings (DO Span= 167). *
C                                                                           *
C     This LFK test may be used as a standard performance test, as a test   *
C     of compiler accuracy (checksums), or as a hardware endurance test.    *
C     The LFK methodology is discussed in subroutine REPORT with references.*
C     The glossary and module hierarchy are documented in subroutine INDEX. *
C                                                                           *
C     Use of this program is granted with the request that a copy of the    *
C     results be sent to  the  author  at the address shown below, to be    *
C     added to  our studies of  computer performance.   Please send your    *
C     complete LFK test output file on 5" DOS floppy-disk, or by E-mail.    *
C     Your timing results  may be held as proprietary data, if so marked.   *
C     Otherwise your results will be quoted in published reports and will   *
C     be disseminated through a publicly accessable computer network.       *
C     Most computer vendors have run the LFK test(akas Livermore Loops test)*
C     and can provide LFK test results to prospective customers on request. *
C                                                                           *
C                                                                           *
C          F.H. McMahon     L-35                                            *
C          Lawrence Livermore National Laboratory                           *
C          P.0. Box 808                                                     *
C          Livermore, CA.   94550                                           *
C                                                                           *
C          (510) 422-1647                                                   *
C          mcmahon@ocfmail.ocf.llnl.gov                                     *
C          MCMAHON3@LLNL.GOV                                                *
C                                                                           *
C                                                                           *
C                    (C) Copyright 1983 the Regents of the                  *
C                University of California. All Rights Reserved.             *
C                                                                           *
C               This work was produced under the sponsorship of             *
C                the U.S. Department of Energy. The Government              *
C                       retains certain rights therein.                     *
C****************************************************************************
C
C
C                             DIRECTIONS
C
C  1. We REQUIRE one test-run of the Fortran kernels as is, that is, with
C     no reprogramming.  Standard product compiler directives may be used
C     for optimization as these do not constitute reprogramming. Use of
C     special compiler coding used only for specific LFK kernels is PROHIBITED.
C     We REQUIRE one mono-processed run (1 cpu) of this unaltered test.
C
C     The performance of the standard, "as is" LFK test (no modifications)
C     correlates well with the performance of the majority of cpu-bound,
C     Fortran applications and hence of diverse workloads.  These measured
C     correlations show the LFK to be a good sampling of the existing
C     inventory of Fortran coding practice in general.  The extrema in
C     the Fortran inventory are represented from serial recurrences on
C     small arrays to global-parallel computation on large arrays.
C
C  2. In addition, the vendor may, if so desired, reprogram the kernels to
C     demonstrate high performance hardware features.  Kernels 13,14,23
C     are partially vectorisable and kernels 15,16,24 are vectorisable if
C     re-written. Kernels 5,6,11,17,19,20,23 are implicit computations that
C     must NOT be explicitly vectorised using compiler directives to
C     ignore dependencies.  In any case, compiler listings of the codes
C     actually used should be returned along with the timing results.
C
C     We permit the LFK kernels to be reprogrammed ONLY as a partial
C     demonstration of the performance of innovative, high performance
C     architectures.  We may then infer from the reprogramming work
C     the kind and degree of optimisations which are necessary to achive
C     high performance as well as the cost in time and effort.
C     Only if it can be shown that this reprogramming can be automated
C     could we establish a correlation with the existing Fortran inventory.
C     These non-standard tests using the LFK samples are intended to explore
C     programming requirements and should not be correlated with standard
C     LFK test results (as in 1 above).
C
C  3. For vector processors, we REQUIRE an ALL-scalar compilation test-run
C     to measure the basic scalar performance range of the processor.
C
C  4. On computers where default single precision is REAL*4 we REQUIRE an
C     additional test-run with all mantissas.ge.47 .  Declare all REAL*8 using:
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
c
c     To change REAL*4 (MFLOPS) to REAL*8 Double Precision:
c
c      vi... :1,$s/cANSI/     /g
c      vi... :1,$s/      DOUBLE  PRE/Cout  DOUBLE  PRE/g
c     ( some redundance in IQRANF,REPORT,RESULT,SEQDIG,TALLY,TRIAL,VALUES)
c
c     To reverse REAL*8 (DPMFLOPS) to REAL*4 Single Precision:
c
c      vi... :1,$s/      IMPLICIT  DOUBLE PRE/cANSI IMPLICIT  DOUBLE PRE/g
c      vi... :1,$s/Cout  DOUBLE  PRE/      DOUBLE  PRE/g
C
C  5. Installation includes verifying or changing the following:
C
C      First :  the definition of function SECOND for CPU time only, and
C      Second:  the definition of function MOD2N in KERNEL
C      Third :  the system names Komput, Kontrl, and Kompil in MAIN.
C     During check-out run-time can be reduced by setting:    Nruns= 1 in SIZES.
C     For Standard LFK Benchmark Test verify:                 Nruns= 7 in SIZES.
C
C  6. Each kernel's computation is check-summed for easy validation.
C     Your checksums should compare to the precision used, within round-off.
C     The number of correct, significant digits in your check-sums is printed
C     in the OK column next to each check-sum.  Single precision should produce
C     6 to 8 OK digits and double precision should produce 11 to 16 OK digits.
C     Try REAL*16 in subr SIGNEL and SUMO to improve accuracy of DP checksums.
C
C  7. Verify CPU Time measurements from function SECOND by comparing the clock
C     calibration printout of total CPU time with system or real-time measures.
C     The accuracy of SECOND is also tested using subr VERIFY and CALIBR.
C     Each kernel's execution may be repeated arbitrarily many times
C     (MULTI >> 100) without overflow and produce verifiable checksums.
C
C     Default, uni-processor tests measure job  Cpu-time in SECOND (TSS mode).
C     Parallel processing tests should measure Real-time in stand-alone mode.
C
C  8. On computers with Virtual Storage Systems assure a working-set space
C     larger than the entire program so that page faults are negligible,
C     because we must measure the CPU-limited computation rates.
C     IT IS ALSO NECESSARY to run this test stand-alone, i.e. NO timesharing.
C     In VS Systems a series of runs are needed to show stable CPU timings.
C
C  9. On computers with Cache memories and high resolution CPU clocks we
C     need, if feasible, another ALL-scalar test-run setting Loop= 1
C     in SIZES to test un-primed cache (as well as encached) cpu rates.
C     Increase the size of array CACHE(in subr. VALUES) from 8192 to cache size.
C
C 10. On parallel computer systems which compile parallel Multi-tasking
C     at the Do-loop level (Micro-tasking) parallelisation of each
C     kernel is encouraged, but the number of processors used must be
C     reported.  Parallelisation of, or invarient code hoisting outside of
C     the outermost, repetition loop around each kernel (including TEST)
C     is PROHIBITED.  You may NOT declare NO-SIDE-EFFECTS function TEST.
C
C 11. A long endurance test can be set-up by redefining "laps" in SIZES.
C
C
C
C
C
C
C
C 12. Quote statistics from the SUMMARY table of 72 timings (DO Span= 167)
C     located near line 700+ in the output file and terminated with a banner>>>
C
C     ********************************************
C     THE LIVERMORE  FORTRAN KERNELS:  * SUMMARY *
C     ********************************************
C
C                  Computer : CRAY Y-MP1
C                  System   : UNICOS 5.1
C                  Compiler : CF77 4.0
C                  Date     : 06/03/90
C           .
C           .
C           .
C             MFLOPS    RANGE:             REPORT ALL RANGE STATISTICS:
C             Mean DO Span   =   167
C             Code Samples   =    72
C
C             Maximum   Rate =    294.34   Mega-Flops/Sec.
C             Quartile  Q3   =    123.27   Mega-Flops/Sec.
C             Average   Rate =     82.71   Mega-Flops/Sec.
C             Geometric Mean =     43.42   Mega-Flops/Sec.
C             Median    Q2   =     31.14   Mega-Flops/Sec.
C             Harmonic  Mean =     23.20   Mega-Flops/Sec.
C             Quartile  Q1   =     17.16   Mega-Flops/Sec.
C             Minimum   Rate =      2.74   Mega-Flops/Sec.
C             <<<<<<<<<<<<<<<<<<<<<<<<<<<*>>>>>>>>>>>>>>>>>>>>>>>>>>>
C             < BOTTOM-LINE:   72 SAMPLES LFK TEST RESULTS SUMMARY. >
C             < USE RANGE STATISTICS ABOVE FOR OFFICIAL QUOTATIONS. >
C             <<<<<<<<<<<<<<<<<<<<<<<<<<<*>>>>>>>>>>>>>>>>>>>>>>>>>>>
C
C     Sadly some analysts quote only the long vector(DO span=471) LFK statistics
C     because they are the most impressive but they are not the best guide to
C     the performance of a large, diverse workload; the SUMMARY statistics are.
C
C     A complete LFK perform-range report must include the minimum, the Harmonic
C     Geometric, and Arithmetic means, the maximum and the standard deviation.
C     The best central measure is the Geometric Mean(GM) of 72 rates because the
C     GM is less biased by outliers than the Harmonic(HM) or Arithemetic(AM).
C     CRAY hardware monitors have demonstrated that net Mflop rates for the
C     LLNL and UCSD tuned workloads are closest to the 72 LFK test GM rate.
C
C
C        CORRELATION OF LFK TEST PERFORMANCE MEANS WITH LARGE WORKLOAD TUNING
C
C        -------      --------      ----------     -----------------------
C        Type of      CRAY-YMP1     Fraction       Tuning of Workload
C        Mean         (VL=167)      Flops in       Correlated with
C                     (MFlops)      Vector Ops     LFK Mean Performance
C        -------      --------      ----------     -----------------------
C
C         2*AM          165.0           .97        Best applications
C
C           AM           82.7           .89        Optimized applications
C
C           GM           43.4           .74        Tuned workload
C
C           HM           23.2           .45        Untuned workload
C
C           HM(scalar)   12.4           .0         All-scalar applications
C        -------      --------      ----------     -----------------------
C        (AM,GM,HM  stand for Arithmetic, Geometric, Harmonic Mean Rates)
C
C     Interpretation of LFK performance rates is discussed in Subr REPORT and:
C
C              F.H. McMahon,   The Livermore Fortran Kernels:
C              A Computer Test Of The Numerical Performance Range,
C              Lawrence Livermore National Laboratory,
C              Livermore, California, UCRL-53745, December 1986.
C
C****************************************************************************
C
C
C
C     DEVELOPMENT HISTORY OF THE LIVERMORE LOOPS TEST PROGRAM
C
C     The first version of the LFK Test (a.k.a. the Livermore Loops, circa
C     1970) consisting of 12 numerical Fortran kernels  was developed
C     and enhanced by F.H. McMahon unless noted otherwise below.
C     The author is grateful for the constructive criticism of colleagues:
C     J.Owens, H.Nelson, L.Berdahl, D.Fuss, L.Sloan, T.Rudy, M.Seager.
C     Since mainframe computers in that era all provided cpu-timers
C     with micro-second time resolution, each kernal was executed just
C     once and timed with negligible experimental timing errors.
C
C     In 1980 the number of Fortran samples was doubled to 24 kernels
C     to represent a broad range of computational structures that would
C     challenge a comiler's capability to generate optimal machine code.
C
C     In 1983 the LFK test driver was extended to execute all 24 kernels
C     three times using three sets of DO loop limits (Avg: 18, 89, 468)
C     since parallel computer performace depends on scale or granularity.
C     These 72 sample statistics are more robust and definitive.
C
C     In 1985 a repetition loop was placed around each kernel to execute
C     them long enough for accurate timing using the standard UNIX
C     timer ETIME which has a crude time resolution of 0.01 seconds.
C
C     In 1986 the LFK test driver was extended to run the entire test
C     seven times so that experimental timing errors for each of the
C     72 samples could be measured.  Reports of these timing errors
C     are necessary for honest scientific experiments. See App. B, C:
C
C           F.H.McMahon,   The Livermore Fortran Kernels:
C           A Computer Test Of The Numerical Performance Range,
C           Lawrence Livermore National Laboratory,
C           Livermore, California, UCRL-53745, December 1986.
C
C     In 1986 Greg Astfalk (AT&T) reprogrammed subroutine KERNEL containing
C     the 24 samples in the C language.  This C module can then be linked
C     with the standard Fortran LFK Test driver-program for testing under
C     identical benchmark conditions as the Fortran samples benchmark.
C     This C module was refined at LLNL by K.O'Hair, C.Rasbold, and M.Seager.
C  
C     In 1990 the repetition loops around each kernel were modified
C     following reports of some code-hoisting by global optimization.
C     These repetition loops were submerged into function TEST beyond
C     the scope of optimizers so the 72 samples are now bullet-proof.
C     New, highly accurate, convergent methods to measure overhead time
C     were implemented ( in VERIFY, SECOVT, TICK ).
C
C     In 1991 the LFK test runtime control MULTI was increased twenty fold
C     for accurate timing when crude UNIX timers having poor time resolution
C     (Tmin= 0.01 sec) were used on very fast computers.  This was only a
C     temporary fix since under UNIX each kernel must always be run
C     at least 1 sec for 1% accuracy despite ever increasing cpu speeds.
C     Thus new algorithms were implemented that automatically determine
C     appropriate values for MULTI which are sufficiently large for
C     accurate timing of the kernels in any system.  A new method
C     of repetition is used that allows MULTI to be increased indefinately
C     (MULTI >> 100) in future without causing overflow and still compute
C     verifiable checksums.  New checksums were generated using IEEE 754
C     standard floating-point hardware on SUN, SGI, and HP workstations.
C     Operational accuracy of the test program is assured in future.
C
C****************************************************************************
C
C
C
C
C/      PARAMETER( kn= 47, kn2= 95, np= 3, ls= 3*47, krs= 24)
C/      PARAMETER( nk= 47, nl= 3, nr= 8 )
      parameter( ntimes= 18 )
C
      CHARACTER  Komput*24, Kontrl*24, Kompil*24, Kalend*24, Identy*24 
      COMMON /SYSID/ Komput, Kontrl, Kompil, Kalend, Identy  
C
      COMMON /ALPHA/ mk,ik,im,ml,il,Mruns,Nruns,jr,iovec,NPFS(8,3,47)
      COMMON /ORDER/ inseq, match, NSTACK(20), isave, iret
      COMMON /TAU/   tclock, tsecov, testov, cumtim(4)
      DIMENSION  FLOPS(141), TR(141), RATES(141), ID(141)
      DIMENSION  LSPAN(141), WG(141), OSUM (141), TERR(141), TK(6)
CLOX  REAL*8 SECOND
CLLNL      CALL  DROPFILE (   '+MFLOPS' )
c                        Job start Cpu time
      cumtim(1)= 0.0d0
             ti= SECOND( cumtim(1))
C
c                                            Define your computer system:
       Komput  =  'CRAY-YMP (6.0ns)        '
       Kontrl  =  'UNICOS  fully loaded    '
       Kompil  =  'CFT77 4.0.3.4           '
       Kalend  =  '91.07.14                '
       Identy  =  'Frank McMahon, LLNL     '
c
c                        Initialize variables and Open Files
           CALL  INDATA( TK, iou)
c                        Record name in active linkage chain in COMMON /DEBUG/
           CALL  TRACE (' MAIN.  ')
c
c                        Verify Sufficient Loop Size Versus Cpu Clock Accuracy
           CALL  VERIFY( iou )
             tj= SECOND( cumtim(1))
             nt= ntimes
c                        Define control limits:  Nruns(runs), Loop(time)
           CALL  SIZES(-1)
c
c                        Run test Mruns times Cpu-limited; I/O is deferred:
      DO 2    k= 1,Mruns
              i= k
             jr= MOD( i-1,7) + 1
           CALL  IQRAN0( 256)
c                        Run test using one of 3 sets of DO-Loop spans:
c                        Set iou Negative to supress all I/O during Cpu timing.
      DO 1    j= im,ml
             il= j
           tock= TICK( -iou, nt)
c
           CALL  KERNEL( TK)
    1 continue
           CALL  TRIAL( iou, i, ti, tj)
    2 continue
c
c                        Report timing errors, Mflops statistics:
      DO 3    j= im,ml
             il= j
           CALL  RESULT( iou,FLOPS,TR,RATES,LSPAN,WG,OSUM,TERR,ID)
c
c                Report  Mflops for Vector Cpus( short, medium, long vectors):
c
                 iovec= 0
        IF(      iovec.EQ.1 )  THEN
           CALL  REPORT( iou,   mk,mk,FLOPS,TR,RATES,LSPAN,WG,OSUM,ID)
        ENDIF
    3 continue
c                Report  Mflops SUMMARY Statistics: for Official Quotations
c
           CALL  REPORT( iou,3*mk,mk,FLOPS,TR,RATES,LSPAN,WG,OSUM,ID)
c
      cumtim(1)= 0.0d0
         totjob= SECOND( cumtim(1)) - ti - tsecov
          WRITE( iou,9)  inseq, totjob, TK(1), TK(2)
          WRITE(   *,9)  inseq, totjob, TK(1), TK(2)
    9    FORMAT( 1H1,//,27H Version: 22/DEC/86  mf523 ,2X,I12,/,1P,
     .                  35H CHECK FOR CLOCK CALIBRATION ONLY: ,/,
     .                  26H Total Job    Cpu Time =  ,e14.5, 5H Sec.,/,
     .                  26H Total 24 Kernels Time =  ,e14.5, 5H Sec.,/,
     .                  26H Total 24 Kernels Flops=  ,e14.5, 6H Flops)
C
C                        Optional Cpu Clock Calibration Test of SECOND:
c          CALL  CALIBR
      STOP
      END
