C
C***********************************************
      SUBROUTINE INDEX
C***********************************************
C       MODULE     PURPOSE
C       ------     -----------------------------------------------
C
C       CALIBR     cpu clock calibration tests accuracy of SECOND function
C
C       INDATA     initialize variables
C
C       IQRANF     computes a vector of pseudo-random indices
C       IQRAN0     define seed for new IQRANF sequence
C
C       KERNEL     executes 24 samples of Fortran computation
C
C       PFM        optional call to system hardware performance monitor
C
c       RELERR     relative error between  u,v  (0.,1.)
C
C       REPORT     prints timing results
C
C       RESULT     computes execution rates  into pushdown store
C
C       SECOND     cumulative CPU time for task in seconds (M.K.S. units)
C
C       SECOVT     measures the Overhead time for calling   SECOND
C
C       SENSIT     sensitivity analysis of harmonic mean to 49 workloads
C
C       SEQDIG     computes nr significant, equal digits in pairs of numbers
C
C       SIGNEL     generates a set of floating-point numbers near 1.0
C
C       SIMD       sensitivity analysis of harmonic mean to SISD/SIMD model
C
C       SIZES      test and set the loop controls before each kernel test
C
C       SORDID     simple sort
C
C       SPACE      sets memory pointers for array variables.  optional.
C
C       SPEDUP     computes Speed-ups: A circumspect method of comparison.
C
C       STATS      calculates unweighted statistics
C
C       STATW      calculates   weighted statistics
C
C       SUMO       check-sum with ordinal dependency
C
C       SUPPLY     initializes common blocks containing type real arrays.
C
C       TALLY      computes average and minimum Cpu timings and variances.
C
C       TDIGIT     counts lead digits followed by trailing zeroes
C
C       TEST       Repeats and times the execution of each kernel
C
C       TESTS      Checksums and initializes the data for each kernel test
C
C       TICK       measures timing overhead of subroutine test
C
C       TILE       computes  m-tile value and corresponding index
C
C       TRACE ,TRACK    push/pop caller's name and serial nr. in /DEBUG/
C
C       TRAP       checks that index-list values are in valid domain
C
C       TRIAL      validates checksums of current run for endurance trial
C
C       VALID      compresses valid timing results
C
C       VALUES     initializes special values
C
C       VERIFY     verifies sufficient Loop size versus cpu clock accuracy
C
C       WATCH      can continually test COMMON variables and localize bugs
c
c  ------------ -------- -------- -------- -------- -------- --------
c  ENTRY LEVELS:   1        2        3        4        5        6
c  ------------ -------- -------- -------- -------- -------- --------
c               MAIN.    SECOND
c                        INDATA
c                        VERIFY   SECOND
c                                 SIZES    IQRAN0
c                                 STATS    SQRT
c                                 TDIGIT   LOG10
c                        SIZES    IQRAN0
c
c                        TICK     TEST     TESTS    SECOND
c                                                   SIZES
c                                                   SUMO
c                                                   VALUES   SUPPLY   SIGNEL
c                                                            IQRANF   MOD
c                                          SECOND
c                                 VALID    TRAP              TRAP
c                                 STATS    SQRT
c                                 IQRANF   MOD
c                                          TRAP
c                        KERNEL   SPACE
c                                 SQRT
c                                 EXP
c                                 TEST     TESTS    SECOND
c                                                   SIZES
c                                                   SUMO
c                                                   VALUES   SUPPLY   SIGNEL
c                                                            IQRANF   MOD
c                                          SECOND
c                        TRIAL    SEQDIG   LOG10    TDIGIT
c                                 IQRAN0
c
c                        RESULT   TALLY    SIZES    IQRAN0   TRAP
c                                          PAGE
c                                          STATS    SQRT
c
c                                 SEQDIG   LOG10    TDIGIT
c
c                        REPORT   VALID    TRAP
c                                 MOD
c                                 STATW    SORDID   TRAP
c                                          TILE
c                                          SQRT
c                                          LOG10
c                                 PAGE
c                                 TRAP
c                                 SENSIT   VALID    TRAP
c                                          SORDID   TRAP
c                                          PAGE
c                                          STATW    SORDID   TRAP
c                                                   TILE
c                                 SIMD     VALID    TRAP
c                                          STATW    SORDID   TRAP
c                                                   TILE
c                                 SPEDUP
C                        STOP
C
C
C
C
C    All subroutines also call TRACE , TRACK , and WATCH to assist debugging.
C
C
C
C
C
C
C
c    ------ ---- ------     -----   ------------------------------------
c    BASE   TYPE CLASS      NAME    GLOSSARY
c    ------ ---- ------     -----   ------------------------------------
c    SPACE0    R Array      BIAS  - scale factors for SIGNEL data generator
c    SPACE0    R Array      CSUM  - checksums of KERNEL result arrays
c    BETA      R Array      CSUMS - sets of CSUM for all test runs
c    BETA      R Array      DOS   - sets of TOTAL flops for all test runs
c    SPACE0    R Array      FLOPN - flop counts for one execution pass
c    BETA      R Array      FOPN  - sets of FLOPN for all test runs
c    SPACE0    R Array      FR    - vectorisation fractions; abscissa for REPORT
c    SPACES    I scalar     ibuf  - flag enables one call to SIGNEL
c    ALPHA     I scalar     ik    - current number of executing kernel
c    ALPHA     I scalar     il    - selects one of three sets of loop spans
c    SPACES    I scalar     ion   - logical I/O unit number for output
c    SPACEI    I Array      IPASS - Loop control limits for multiple-pass loops
c    SPACE0    I Array      IQ    - set of workload weights for REPORT
c    SPACEI    I Array      ISPAN - loop control limits for each kernel
c    SPACES    I scalar     j5    - datum in kernel 16
c    ALPHA     I scalar     jr    - current test run number (1 thru 7)
c    SPACES    I scalar     k2    - counter in kernel 16
c    SPACES    I scalar     k3    - counter in kernel 16
c    SPACES    I scalar     kr    - a copy of mk
c    SPACES    I scalar     laps  - multiplies Nruns for long Endurance test
c    SPACES    I scalar     Loop  - current multiple-pass loop limit in KERNEL
c    SPACES    I scalar     m     - temp integer datum
c    ALPHA     I scalar     mk    - number of kernels to evaluate .LE.24
c    ALPHA     I scalar     ml    - maximum value of il=  3
c    SPACES    I scalar     mpy   - repetiton counter of MULTI pass loop
c    SPACES    I scalar     Loops2- repetiton loop limit
c    ALPHA     I scalar     Mruns - number of complete test runs .GE.Nruns
c    SPACEI    I Array      MUL   - multipliers * IPASS defines Loop
c    SPACES    I scalar     MULTI - Multiplier used to compute Loop in SIZES
c    SPACES    I scalar     n     - current DO loop limit in KERNEL
c    SPACES    I scalar     n1    - dimension of most 1-D arrays
c    SPACES    I scalar     n13   - dimension used in kernel 13
c    SPACES    I scalar     n13h  - dimension used in kernel 13
c    SPACES    I scalar     n14   - dimension used in kernel 14
c    SPACES    I scalar     n16   - dimension used in kernel 16
c    SPACES    I scalar     n2    - dimension of most 2-D arrays
c    SPACES    I scalar     n21   - dimension used in kernel 21
c    SPACES    I scalar     n213  - dimension used in kernel 21
c    SPACES    I scalar     n416  - dimension used in kernel 16
c    SPACES    I scalar     n813  - dimension used in kernel 13
c    SPACE0    I scalar     npf   - temp integer datum
c    ALPHA     I Array      NPFS  - sets of NPFS1 for all test runs
c    SPACE0    I Array      NPFS1 - number of page-faults for each kernel
c    ALPHA     I scalar     Nruns - number of complete test runs .LE.7
c    SPACES    I scalar     nt1   - total size of common -SPACE1- words
c    SPACES    I scalar     nt2   - total size of common -SPACE2- words
c    BETA      R Array      SEE   - (i,1,jr,il) sets of TEST overhead times
c    BETA      R Array      SEE   - (i,2,jr,il) sets of csums of SPACE1
c    BETA      R Array      SEE   - (i,3,jr,il) sets of csums of SPACE2
c    SPACE0    R Array      SKALE - scale factors for SIGNEL data generator
c    SPACE0    R scalar     start - temp start time of each kernel
c    PROOF     R Array      SUMS  - sets of verified checksums for all test runs
c    SPACE0    R Array      SUMW  - set of quartile weights for REPORT
c    TAU       R scalar     tclock- minimum cpu clock time= resolution
c    SPACE0    R Array      TERR1 - overhead-time errors for each kernel
c    BETA      R Array      TERRS - sets of TERR1 for all runs
c    TAU       R scalar     testov- average overhead time in TEST linkage
c    BETA      R scalar     tic   - average overhead time in SECOND (copy)
c    SPACE0    R scalar     ticks - average overhead time in TEST linkage(copy)
c    SPACE0    R Array      TIME  - net execution times for all kernels
c    BETA      R Array      TIMES - sets of TIME for all test runs
c    SPACE0    R Array      TOTAL - total flops computed by each kernel
c    TAU       R scalar     tsecov- average overhead time in SECOND
c    SPACE0    R Array      WS    - unused
c    SPACE0    R Array      WT    - weights for each kernel sample
c    SPACEI    R Array      WTP   - weights for the 3 span-varying passes
c    SPACE0    R Array      WW    - unused
C
C
c  --------- -----------------------------------------------------------------
c   COMMON   Usage
c  --------- -----------------------------------------------------------------
C
C   /ALPHA /
C            VERIFY    TICK      TALLY     SIZES     RESULT    REPORT    KERNEL
C            MAIN.
C   /BASE1 /
C            SUPPLY
C   /BASE2 /
C            SUPPLY
C   /BASER /
C            SUPPLY
C   /BETA  /
C            TICK      TALLY     SIZES     RESULT    REPORT    KERNEL
C   /DEBUG /
C            TRACE     TRACK     TRAP
C   /ORDER /
C            TRACE     TRACK     TRAP
C   /PROOF /
C            RESULT    BLOCKDATA
C   /SPACE0/
C            VALUES    TICK      TEST      TALLY     SUPPLY    SIZES     RESULT
C            REPORT    KERNEL    BLOCKDATA
C   /SPACE1/
C            VERIFY    VALUES    TICK      TEST      SUPPLY    SPACE     KERNEL
C   /SPACE2/
C            VERIFY    VALUES    TICK      TEST      SUPPLY    SPACE     KERNEL
C   /SPACE3/
C            VALUES
C   /SPACEI/
C            VERIFY    VALUES    TICK      TEST      SIZES     RESULT    REPORT
C            KERNEL    BLOCKDATA
C   /SPACER/
C            VALUES    TICK      TEST      SUPPLY    SIZES     KERNEL
C   /SPACES/
C            VERIFY    VALUES    TICK      TEST      SUPPLY    SIZES     KERNEL
C            BLOCKDATA
c  --------- -----------------------------------------------------------------
c
c
c           SubrouTine Timing on CRAY-XMP1:
c
c           Subroutine   Time(%) All Scalar
c
c           KERNEL       52.24%
c           SUPPLY       17.85%
c           VERIFY        8.76%
c           VALUES        6.15%
c           STATS         5.44%
c           DMPY          1.97%
c           DADD          1.53%
c           EXP           1.02%
c           SQRT           .99%
c           SORDID         .81%
c           DDIV           .38%
c           IQRANF         .25%
c           SUMO           .22%
c           TRACE          .19%
c           SIGNEL         .16%
c           TRAP           .10%
c           TRACK          .10%
c           STATW          .08%
c           TILE           .04%
c           SIZES          .03%
c           ALOG10         .03%
c
c           Subroutine   Time(%)  Auto Vector
c
c           KERNEL       56.28%
c           VALUES       10.33%
c           STATS         8.57%
c           DADD          4.34%
c           DMPY          3.86%
c           VERIFY        2.61%
c           SUPPLY        2.28%
c           SQRT          2.10%
c           SORDID        1.84%
c           SUMO           .80%
c           DDIV           .78%
c           SDOT           .67%
c           TRACE          .53%
c           IQRANF         .50%
c           SIGNEL         .36%
c           EXP            .32%
c           TRACK          .23%
c           TRAP           .20%
c           ALOG10         .18%
c           STATW          .16%
c
c
      RETURN
      END
