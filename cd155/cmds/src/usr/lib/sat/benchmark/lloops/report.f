C
C***********************************************************************
      SUBROUTINE REPORT( iou, ntk,nek,FLOPS,TR,RATES,LSPAN,WG,OSUM,ID)
C***********************************************************************
C                                                                      *
C     REPORT -  Prints Statistical Evaluation Of Fortran Kernel Timings*
C                                                                      *
C     iou    -  Logical Output Device Number                           *
C     ntk    -  Total number of Kernels to Edit in Report              *
C     nek    -  Number of Effective Kernels in each set to Edit        *
C     FLOPS  -  Array:  Number of Flops executed by each kernel        *
C     TR     -  Array:  Time of execution of each kernel(microsecs)    *
C     RATES  -  Array:  Rate of execution of each kernel(megaflops/sec)*
C     LSPAN  -  Array:  Span of inner DO loop in each kernel           *
C     WG     -  Array:  Weight assigned to each kernel for statistics  *
C     OSUM   -  Array:  Checksums of the results of each kernel        *
C***********************************************************************
c
c                                REFERENCES
c
c               F.H.McMahon,   The Livermore Fortran Kernels:
c               A Computer Test Of The Numerical Performance Range,
c               Lawrence Livermore National Laboratory,
c               Livermore, California, UCRL-53745, December 1986.
c
c        from:  National Technical Information Service
c               U.S. Department of Commerce
c               5285 Port Royal Road
c               Springfield, VA.  22161
c
c               J.T. Feo,  An Analysis Of The Computational And Parallel
c               Complexity Of The Livermore Loops, PARALLEL COMPUTING
c               (North Holland), Vol 7(2), 163-185, (1988).
c
c                                NOTICE
c
c               "This report was  prepared  as an account
c               of work  sponsored  by  the United States
c               Government.  Neither  the  United  States
c               nor the   United   States  Department  of
c               Energy, nor any  of  their employees, nor
c               any of their contractors, subcontractors,
c               or their employees,  makes  any warranty,
c               express or  implied, or assumes any legal
c               liability or   responsibility   for   the
c               accuracy, completeness  or  usefulness of
c               any information,  apparatus,  product  or
c               process disclosed, or represents that its
c               use would  not  infringe  privateiy-owned
c               rights."
c
c               Reference to  a  company  or product name
c               does not impiy approval or recommendation
c               of the   product  by  the  University  of
c               California or  the   U.S. Department   of
c               Energy to  the  exclusion  of others that
c               may be suitable.
c
c
c               Work performed under  the auspices of the
c               U.S. Department of Energy by the Lawrence
c               Livermore Laboratory    under    contract
c               number W-7405-ENG-48.
c
c***********************************************************************
c
c  Abstract
c
c  A computer performance  test that measures a realistic floating-point
c  performance range  for  Fortran applications is described.  A variety
c  of computer performance analyses may be easily carried out using this
c  small central  processing unit (cpu) test that would be infeasible or
c  too costly using complete applications as benchmarks, particularly in
c  the developmental  phase of an immature computer system.  The problem
c  of benchmarking numerical  applications  sufficiently,  especially on
c  new supercomputers,  is analyzed to identify several useful roles for
c  the Livermore Fortran  Kernal  (LFK) test.  The 24 LFK contain enough
c  samples of Fortran practice to expose many specific inefficiencies in
c  the formulation of the Fortran source, in the quality of compiled cpu
c  code, and   in   the  capability  of  the  instruction  architecture.
c  Examples show how the  LFK may be used to study compiled Fortran code
c  efficiency, to test the ability of compilers to vectorize Fortran, to
c  simulate mature coding  of  Fortran on new computers, and to estimate
c  the effective  subrange  of  supercomputer  performance  for  Fortran
c  applications.
c
c  Cpu performance  measurements   of  several  Fortran  benchmarks  and
c  numerical applications  that  correlate well with the cpu performance
c  range measured  by  the   LFK  test  are  presented.   The  numerical
c  performance metric  Mflops, first introduced in 1970 in this cpu test
c  to quantify the cpu  performance  range of numerical applications, is
c  discussed.  Analyses  of  the  LFK  performance results argue against
c  reducing the cpu performance  range  of  supercomputers  to  a single
c  number.  The  24  LFK  measured  rates  show  a realistic variance in
c  Fortran cpu  performance  that  is  essential  data  for  circumspect
c  computer evaluations.   Cpu performance data measured by the LFK test
c  on a number of recent computer systems are tabulated for reference.
c
c
c
c  I: FORTRAN CPU PERFORMANCE ANALYSIS
c
c
c     These kernels measure  Fortran numerical computation rates for a
c     spectrum of  CPU-limited computational structures or benchmarks.
c     The kernels benchmark  contains  extracts  or  kernels from more
c     than a score CPU-limited scientific application programs.  These
c     kernels are The most  important  CPU  time  components  from The
c     application programs.   This  benchmark  may  be easily extended
c     with important new kernels leaving performance statistics intact.
c
c     The time required  to  convert,  debug,  execute  and time many,
c     entire, large  programs  on  new  machines  each  having  a  new
c     implementation of  Fortran,   or   several   implementations  or
c     dialects rapidly  becomes  excessive.  Almost all The conversion
c     costs are in segments  of  The programs which are irrelevant for
c     evaluation of  The  CPU,  e.g.,  I/O, Fortran variations, memory
c     allocation, overlays,   job  control,   etc.    all   of   these
c     complexities are reduced to a single, small benchmark which uses
c     a minimum of I/O  and  a  single level of storage.  further, the
c     computation in  the  kernels  is  the  most  stable  part of the
c     Fortran language.
c
c     The kernels benchmark  is sufficient to determine a range of CPU
c     performance for  many  different  computational  structures in a
c     single computer run.   Since The range in performance is usually
c     large the  mean  has  a secondary significance.  To estimate the
c     performance of  a particular,  CPU-limited  application  program
c     select the  case(s) which are most similar to the application as
c     most relevent to the  estimate.   The  performance  ratio  of  a
c     kernel on  two  different  machines or compiled by two different
c     compilers on the same  machine  will  approximate  the  ratio of
c     through-puts for   an  application  which  is  very  similar  in
c     structure.
c
c     This set of kernels was chosen to measure lower and upper bounds
c     for scalar Fortran computation rates.  The upper bound on scalar
c     rates serves as a base  to  evaluate the effectiveness of vector
c     computation.  The  kind  of  Fortran  which  has the highest MIP
c     rates is pure arithmetic  in  DO-loops where complete local code
c     optimization by a Fortran compiler is possible.  All other kinds
c     of Fortran  operations  execute  at  much  lower  MIP  rates  on
c     multiple register machines (these ops may not be necessary).
c
c     Through-put is  measured  in  units of floating-point operations
c     executed per micro-second;  called  results  per micro-second or
c     mega-flops.  The  Mflop is a measure of the NECESSARY results in
c     a scientific application  program  regardless  of  the number or
c     kind of  operations  or processing.  The ratio of Mflops for two
c     different machines will approximate  the  ratio  of through-puts
c     for the  majority  of compute-limited scientific applications on
c     the two  machines.    The   kernels  measure  performance  scale
c     factors.
c
c
c II: FORTRAN PROGRAMMING SYSTEM MATURITY
c
c     Hardware performance   gains   depend   criticaly   on  compiler
c     maturity.  These  kernels   measure  the  joint  performance  of
c     hardware and  Fortran  compiler  software and may easily be used
c     for a comparative  analysis  of  all  the available compilers or
c     options on a given machine.  For a new or proposed machine where
c     no compiler is available  the  performance  may  be estimated by
c     simulating a  reasonable  compilation.  An example of simulation
c     rationale is given below.
c
c     Fortran compilers for new  types  of  machines require a lengthy
c     development cycle  to  achive  an  effective  level  of  machine
c     utilization.  A fully  mature  compiler  may not be completed in
c     the first  years  of  a  new machine.  Indeed, maturity is not a
c     stationary state   but  evolves   with   advances   in   program
c     optimization techniques.   Some  of  these  techniques depend on
c     special facilities in the  new  machines and serious development
c     and implementation cannot start much earlier than development of
c     the new machine.   Assumptions  on  the  maturity  of  available
c     Fortran compilers  are  crucial  to  the  evaluation  of Fortran
c     performance and  thus,   compiler   characteristics   should  be
c     explicit parameters of the performance analysis.
c
c
c -----------------------------------------------------------------------------
c III: A CPU Performance Metric For Computational Physics:    Mega-Flops/sec.
c -----------------------------------------------------------------------------
c
c
c A:  Floating-Point Instructions:  The Necessary Mathematics
c
c Computational physics applies  systems  of  PDEs from Mathematical physics to
c simulate the  evolution of physical systems.  The mathematical methods depend
c on real  valued  functions   and   the   algorithms  are  programmed,  almost
c exclusively, in  Fortran  Floating-point  computer operations (Flops).  These
c floating-point operations  are,  unquestionably,   the   NECESSARY   computer
c operations on  ANY  computer  and  the  total  number  is  INVARIANT.  Thus a
c meaningful computation rate can  always  be  measured  by  counting the total
c number of Flops and dividing by the total execution time of a program.
c
c B:   Procedural Machine  Instructions:   Artifices Of An Archetecture
c
c All of  the non-arithmetic instructions in a machine program are artifices of
c a particular hardware  architecture,  i.e.  machine dependant, as well as the
c result of  a  particular compiler's imperfect coding techniques.  How many of
c these procedural machine  instructions  are  strictly  necessary  can only be
c determined by further, tedious analysis which is ALWAYS machine dependant.  A
c famous example  of software  masking  hardware  capabilities  is  the  PASCAL
c compiler written  by  n.Wirth  which  used  only  50%  of  the command set to
c generate machine programs for the CDC-7600.
c
c Unless the next generation computer design is constrained for some reason, to
c closely resemble  its  obsolete  predecessor,  the  instruction  mix  used in
c current machines is not necessarily  relevent.   Furthermore, the instruction
c mix is  not  a  definitive  characterization  of the intrinsic physics or the
c mathematical algorithms.
c
c  1.  Primary Memory Access Instructions
c
c The number of memory  instructions  that  are necessary for a given algorithm
c depends strongly  on  the  number  and  kind of CPU registers and is a highly
c machine dependent number.   Operating registers, scratch-pad memories, vector
c buffers, short-stop  and  feed-back paths in the cpu are examples of hardware
c artifices which reduce the  number  of  primary memory operations.  Compilers
c and other  coders must make intelligent use of these particular cpu resources
c to minimize memory operations  and this is generally not the case, as is well
c known.
c
c  2.  Branching Instructions
c
c Branching instructions   are   the  slowest  and  most  expensive  procedural
c instructions and are very  often  unecessary.  Here the source programmer has
c primary responsibility  to  minimize  branching in the program by avoiding IF
c statements whenever possible  by  using MAX, MIN, or merge functions like
c CSMG.  Careful  logical  reduction  and  placement of IF tests is required to
c minimize the execution of branching operations.  Compilers can do very little
c to change or optimize the branch graph specified in the source program.
c
c On vector  computers ALL IF tests over mesh or array (state) variables can be
c eliminated.  Conditional computation can be vectorized by direct construction
c using explicit  sub-set mappings.  Vector relationals replace the IF clauses.
c Then sparse,  one-to-one   mappings  called  vector  Compress/Decompress  and
c one-to-many mappings   called   vector   Gather/Scatter   are  necessary  and
c sufficient to compose sub-vector operands for simple vector operations.
c
c
c
c
c
c IV: PERFORMANCE MEASUREMENTS
c
c
c     Through-put is measured in units of millions of floating-point
c     operations executed per second, called mflops.
c
c
c     Artificially long computer  runs do not have to be contrived for
c     timing on  machines  where  a cpu clock may be read in job mode.
c     Statistics on  the accuracy  of  the  timing  method  should  be
c     measured.
c
c     Net mflops is meaningful only if real run time of each kernel
c     is adjusted such that it weights the total time in proportion
c     to the actual usage of that catagory of computation in the
c     total workload.
c
c
c
c
c
c   1. Assignment Of Weights To Floating-Point Operations
c
c     Weights are assigned to different kinds of floating-point
c     operations to normalize their hardware execution time to
c     addition time so that the flop rates computed for various
c     Fortran Kernels will be commensurable.
c
c                           +,-,*   1
c                          /,SQRT   4
c                     EXP,SIN,ETC.  8
c                     IF(X.REL.Y)   1
c
c
c     Each Kernel flop-count is the weighted number of flops required for
c     serial execution.  The scalar version defines the NECESSARY computation
c     generally, in the absence of proof to the contrary.  The vector
c     or parallel executions are only credited with executing the same
c     necessary computation.  If the parallel methods do more computation
c     than is necessary then the extra flops are not counted as through-put.
c
c
c    2. SAMPLE OUTPUT:               CDC-7600/FTN-4.4
c
c                 KERNEL  FLOPS   TIME   MFLOPS
c                      1    500    94.4    5.30
c                      2    300    45.3    6.62
c                      3    100    21.9    4.57
c                      4    300   109.3    2.75
c                      5    100    25.6    3.91
c                      6    100    27.8    3.60
c                      7    640    88.2    7.25
c                      8   1440   249.0    5.78
c                      9    680   123.2    5.52
c                     10    360   102.8    3.50
c                     11     49    34.8    1.41
c                     12     49    18.3    2.68
c                     13    224   107.7    2.08
c                     14   3300   809.3    4.08
c                     15   3960  1769.5    2.24
c                     16    530   320.3    1.65
c                     17    405    92.2    4.39
c                     18   6600  1121.5    5.88
c                     19    540   105.8    5.11
c                     20   1300   266.0    4.89
c                     21   1250   370.9    3.37
c                     22   1700   601.9    2.82
c                     23   1650   362.4    4.55
c                     24    200   171.7    1.16
c
c                      AVERAGE  RATE =     3.96 MEGA-FLOPS/SEC.
c                      MEDIAN   RATE =     4.08 MEGA-FLOPS/SEC.
c                      HARMONIC MEAN =     3.15 MEGA-FLOPS/SEC.
c                      STANDARD DEV. =     1.61 MEGA-FLOPS/SEC.
c
c                                                    F.H.MCMAHON  1972
c
c
c
c
c
c
c    3. INTERPRETATION OF OUTPUT FILE FROM SUBROUTINE REPORT:
c
c
c
c  The highly instrumented LFK test program measures the effective cpu
c  performance range and has sufficient timed samples for many statisical
c  analyses thus avoiding the PERIL of a SINGLE performance "rating".
c  A COMPLETE REPORT OF LFK TEST RESULTS MUST QUOTE THE PERFORMANCE RANGE
c  STATISTICS BASED ON THE SUMMARY OF 72 TIMED SAMPLES:  the minium,
c  the equi-weighted harmonic, geometric, and arithmetic means and the maximum
c  rates.  The standard deviation must also be quoted to show the variance
c  in performance rates.  NO SINGLE RATE QUOTATION IS SUFFICIENT OR HONEST.
c
c  The LFK test (Livermore loops) outputs data for three benchmarking contexts
c  following print-outs of cpu clock checks and experimental timing errors:
c
c
c
c  1. Conventional "Balanced"  Cpus,  e.g. PCs, DEC-VAXs, IBM-370s.
c
c    1.1. [Refer to SUMMARY of 72 timings on pp.9-10 of LFK test OUTPUT file.
c         The bottom line is the set of nine performance range statistics
c         min thru max plus standard deviation listed after SUMMARY table.
c         These statistics may be used for computer comparisons as shown
c         in figure 11, p.24 of the LFK report UCRL-53745.  Ratios of the
c         range statistics from two computers show the range of speed-ups.]
c
c    1.2. An all-scalar coded LFK test (NOVECTOR) measures the basal scalar,
c         mono-processor computing capability.
c
c
c
c  2. Vector "Unbalanced"  Cpus,  e.g. CRAY, NEC, IBM-3090.
c
c    2.1. [Pages 2-8 of the LFK test OUTPUT file analyzes three different
c         runs of the 24 Livermore loops with short, medium, and long DO
c         loop spans (vector lengths).  The performance range statistics
c         for each of these three runs on vector computers should be compared
c         as shown in figure 12, p.25 of the LFK report UCRL-53745.]
c
c    2.2  The performance rates of most applications on vector computers are
c         observed in a sub-range from approximately the harmonic mean through
c         the mean rate of the 24 LFK samples (thru the two middle quartiles).
c
c         2.2.1  The equi-weighted arithmetic mean (AM) of 72 LFK rates
c                correlates with highly vectorised applications in the workload,
c                (80%-90% of flops) because the average is dominated by the high
c                vector rates.  Very highly vectorised applications (95%-99%+)
c                may run several times the average rate (figure 10, p21, ibid).
c
c         2.2.2  The equi-weighted harmonic mean (HM) of 72 LFK rates
c                correlates with poorly vectorised applications in the workload,
c                (30%-40% of flops) because the HM is dominated by the low
c                scalar rates.  An all-scalar coded LFK test (NOVECTOR)
c                measures the basal scalar, mono-processor computing capability.
c
c         2.2.3  The best central measure is the Geometric Mean(GM) of 72 rates,
c                because it is least biased by outliers.  CRAY hardware monitors
c                have demonstrated net Mflop rates for the LLNL and UCSD
c                workloads are closest to the 72 LFK test geometric mean rate.
c
c
c
c  3. Parallel "Unbalanced"  Cpus,  e.g. CRAY, NEC, IBM-3090.
c
c    3.1. The lower,   uni-processor bound of an MP system is given by 1.2.
c
c    3.2. The upper, multi-processor bound of an MP system is estimated by
c         multiplying the LFK performance statistics from 1.2 or from 2.2.
c         by N, the number of processors.
c
c
c
c
c      Comparision of two or more computers should make use of all the
c  performance range statistics in the tables below ( DO span= 167):
c  the extrema, the mean rates, and the standard deviation.
c  NO SINGLE MFLOPS RATE QUOTATION IS SUFFICIENT OR HONEST.
c  If the performance range is very large the causes and implications should
c  be fully explored.  Use of a single mean statistic is insufficient
c  but may be valid if the three mean rates are close in value and the
c  standard deviation is relatively small.  The geometric mean is a
c  better central measure than the median which depends on one value
c  in a small set.  The least biased central measure is the geometric
c  mean because it is less sensitive to outliers than either the average
c  or the harmonic mean.  When the computer performance range is very
c  large the net Mflops rates of many Fortran programs and workloads
c  have been observed to be in the sub-range between the equi-weighted
c  harmonic and arithmetic means depending on the degree of code
c  parallelism and optimization(Ref. 1).  Note that LFK mean Mflops rates
c  also imply the average efficiency of a computing system since
c  the peak rate is a well known constant.
c
c      The performance data shown for the computers below will be subject to
c  change with time.  Effective Cpu performance may improve as the programming
c  system software matures or effective performance may regress when the system
c  is oversubscribed.  We have observed degraded performance for the LFK test
c  in virtual storage systems when the working set size was too small, and in
c  multiprogramming or multiprocessing systems which were either immature or
c  very active.  In these active environments the LFK test measures a real
c  Cpu degradation in the effectiveness of caching data and data access
c  generally.  It is necessary to run the LFK test stand-alone to have
c  reproducable performance measurements.
c
c      The performance data sets tabulated below which have 72 sample
c  timings are a combination of three 24 sample sets produced by the
c  LFK test.  Statistics on the 72 sample data set are more significant
c  and these statistics should be quoted ( DO span= 167).
c
c
c
c
c
c
c                          REFERENCES
c
c         F.H.McMahon,   The Livermore Fortran Kernels:
c         A Computer Test Of The Numerical Performance Range,
c         Lawrence Livermore National Laboratory,
c         Livermore, California, UCRL-53745, December 1986.
c
c  from:  National Technical Information Service
c         U.S. Department of Commerce
c         5285 Port Royal Road
c         Springfield, VA.  22161
c
c
c         F.H.McMahon, "The Livermore Fortran Kernels Test of the Numerical
c         Performance Range", in Performance Evaluation of Supercomputers
c         (J.L.Martin, ed., North Holland, Amsterdam), 143-186(1988).
c
c
c         J.T. Feo,  An Analysis Of The Computational And Parallel
c         Complexity Of The Livermore Loops, PARALLEL COMPUTING
c         (North Holland), Vol 7(2), 163-185, (1988).
c
c
c         F.H.McMahon, "Measuring the Performance of Supercomputers",
c         in Energy and Technology Review (A.J.Poggio,ed.),
c         Lawrence Livermore National Laboratory, UCRL-52000-88-5, (1988).
c
c
c
c
c    The range of speed-ups shown below as ratios of the performance
c    statistics has a small variance compared to the enormous
c    performance ranges; the range of speed-ups are convergent estimates.
c    Report all nine performance range statistics on 72 samples, e.g.:
c
c
c
c
c D.117 LFK Test   117.1      117.2      117.3      117.4      117.5      117.6
c ------------- ---------- ---------- ---------- ---------- ---------- ---------
c   Vendor       CRAY RI    CRAY RI    CRAY RI    CDC        IBM        NEC
c   Model        XMP1 8.5   YMP1       2          ETA10-G    3090S180   SX-2
c   OSystem      COS 1.16   COS 1.16   UNICOS     EOS1.2J2   MVS2.2.0   SXOS1.21
c   Compiler     CFT771.2   CFT771.2   CFT771.3   F200 690   VSF2.3.0   F77/SX24
c   OptLevel     Vector     Vector     Vector     VAST2.25   Vector     Vector
c   NR.Procs          1          1          1          1          1          1
c   Samples          72         72         72         72         72         72
c   WordSize         64         64         64         64         64         64
c   DO Span         167        167        167        167        167        167
c   Year           1987       1988       1988       1988       1989       1986
c   Kernel/MFlops--------- ---------- ---------- ---------- ---------- ---------
c          1       183.57     258.64     160.17     405.57      56.03     800.05
c          2        42.49      67.09      21.61      12.55       8.88      49.94
c          3       173.19     236.67     111.93     233.09      53.66     528.67
c          4        65.68      95.05      47.45      59.48      40.72     164.18
c          5        15.89      18.69      13.01      11.86       8.83      11.26
c          6        12.91      20.58      13.07      13.13       8.57      29.30
c          7       207.28     295.48     228.00     488.07      62.08    1042.33
c          8       149.44     232.41     189.47     242.77      46.19     415.68
c          9       178.50     251.07     195.24     186.88      61.70     705.28
c         10        78.50     111.42      73.20      82.68       8.57     120.75
c         11        12.02      16.52      12.39       7.11       6.84       8.32
c         12        81.14     112.50      57.52     227.40      18.18     242.80
c         13         5.89       7.35       4.83       5.66       4.12      16.78
c         14        22.48      31.90      19.08      11.56      11.08      25.79
c         15         6.24       7.78       7.58      75.87       4.93       8.73
c         16         7.28       8.62       5.06       2.53       5.27       9.85
c         17        11.70      14.92      10.29       8.38      10.65      17.89
c         18       126.84     203.76     127.63     160.39      37.13     349.42
c         19        16.74      20.63      13.70       9.69      11.58      13.40
c         20        14.56      18.76      13.51       8.13       9.75      16.12
c         21       117.63     168.79      58.97     138.42      19.62     253.03
c         22        75.96     103.46      95.34      54.32      17.04     183.34
c         23        15.34      17.71      10.46      20.22      13.97      20.52
c         24         3.60       4.58       2.66      28.60       3.95       4.59
c -------------      ....       ....       ....       ....       ....       ....
c PM Correlation =   1.00       1.00       0.97       0.90       0.95       0.93
c Standard  Dev. =  59.92      86.75      61.18      89.09      16.32     219.72
c
c Maximum   Rate = 207.28     295.48     228.00     488.07      62.08    1042.33
c Quartile  Q3   =  78.59     111.42      73.20      78.61      19.20     156.56
c Average   Rate =  55.39      78.23      49.70      64.38      17.56     139.95
c Geometric Mean =  27.57      36.63      22.61      26.39      12.23      43.94
c Median    Q2   =  16.74      20.63      13.77      19.82      10.06      24.16
c Harmonic  Mean =  13.95      17.66      11.26      12.25       9.02      19.07
c Quartile  Q1   =  11.70      14.75       8.34       8.39       6.99      11.44
c Minimum   Rate =   2.20       2.85       2.01       2.25       2.43       4.47
c
c Maxima    Ratio=   1.00       1.43       1.10       2.35       0.30       5.03
c Average   Ratio=   1.00       1.41       0.90       1.16       0.32       2.53
c Geometric Ratio=   1.00       1.33       0.82       0.96       0.44       1.59
c Harmonic  Ratio=   1.00       1.27       0.81       0.88       0.65       1.37
c Minima    Ratio=   1.00       1.30       0.91       1.02       1.10       2.03
c
c           The range of speed-ups shown above as ratios of the performance
c           statistics has a small variance compared to the enormous
c           performance ranges; the range of speed-ups are convergent estimates.
c           More accurate projection of a cpu workload rate may be
c           computed by assigning appropriate weights for each kernel.
c
c           The upper bound for Fortran performance of a parallel
c           N-processor system is given by multiplying the seven range
c           statistics from a uni-processor LFK test (2.2) by N.
c
c D.118 LFK Test  118.1      118.2      118.3      118.4      118.5      118.6
c ------------- ---------- ---------- ---------- ---------- ---------- ---------
c   Vendor       CRAY RI    CRAY RI    CRAY RI    CRAY RI    CRAY RI    CRAY RI
c   Model        YMP1modY   YMP1modY   YMP/832    YMP/832    YMP/832    YMP/832
c   OSystem      NLTSS      NLTSS      UNICOS     UNICOS     UNICOS     UNICOS
c   Compiler     CFT77 3.   CFT77 3.   CF77 4.0   CF77 4.0   CF77 4.0   CF77 4.0
c   OptLevel     Scalar     Vector     vector     vector     vector     vector
c   NR.Procs            1          1          1          2          4          8
c   Samples            72         72         72         72         72         72
c   WordSize           64         64         64         64         64         64
c   DO Span           167        167        167        167        167        167
c   Year             1989       1989       1990       1990       1990       1990
c   Kernel/MFlops--------- ---------- ---------- ---------- ---------- ---------
c          1        23.33     258.08     188.23     364.86     535.99     581.75
c          2        14.26      68.12      64.45      64.86      65.59      64.08
c          3        25.05     232.20     236.81     236.93     233.45     236.86
c          4        22.92      92.14      89.72     110.24     160.77     156.70
c          5        19.44      19.59      19.30      19.64      19.59      19.65
c          6         9.24      21.15      20.76      21.07      20.93      20.86
c          7        32.83     291.31     274.07     521.69     896.68    1308.07
c          8        30.00     229.89     188.78     264.72     262.94     266.89
c          9        31.23     240.88     169.97     225.10     219.31     243.47
c         10        18.53     108.73     106.66     112.78     108.76     108.58
c         11        19.73      19.75      37.87      38.66      38.52      37.93
c         12        16.95     135.81     126.99     130.52     125.68     130.49
c         13         6.73       6.74      20.77      21.16      20.89      21.18
c         14         9.71      29.98      29.04      35.15      38.77      40.81
c         15         7.55       7.55      32.53      52.47      73.84     127.58
c         16         8.42       8.34       8.38       8.44       8.34       8.44
c         17        13.47      13.84      15.70      15.89      15.88      15.89
c         18        24.84     199.36     179.98     293.24     410.87     526.41
c         19        20.28      20.34      20.07      20.37      20.27      20.37
c         20        18.27      18.50      17.84      17.98      18.03      18.05
c         21        20.53     160.40     278.54     439.90     776.14    1268.94
c         22         8.74     106.25      86.52     132.58     131.62     129.79
c         23        19.53      20.16      36.35      36.65      36.75      36.79
c         24         3.85       3.93      38.04      38.82      38.58      38.80
c -------------      ....       ....       ....       ....       ....       ....
c Standard  Dev. =   7.93      85.18      78.37     113.05     171.34     246.63
c
c Maximum   Rate =  32.83     291.31     278.54     521.69     896.68    1308.07
c Quartile  Q3   =  20.53     109.15     116.42     120.27     125.70     128.95
c Average   Rate =  16.65      77.30      75.88      92.90     113.89     136.32
c Geometric Mean =  14.58      36.50      41.43      45.11      48.00      49.09
c Median    Q2   =  16.95      21.15      32.25      36.53      36.75      36.79
c Harmonic  Mean =  12.40      17.27      22.69      23.48      24.12      23.58
c Quartile  Q1   =   8.76      13.84      16.56      16.74      17.94      17.13
c Minimum   Rate =   3.73       2.90       2.82       2.87       2.86       2.83
c
c Maxima    Ratio=   1.00       8.87       8.48      15.89      27.31      39.84
c Average   Ratio=   1.00       4.64       4.56       5.58       6.84       8.19
c Geometric Ratio=   1.00       2.50       2.84       3.09       3.29       3.37
c Harmonic  Ratio=   1.00       1.39       1.83       1.89       1.95       1.90
c Minima    Ratio=   1.00       0.78       0.76       0.77       0.77       0.76
c
c
c
c
c           The parallel complexity and parallel techniques for the LFK
c           test are described in the following reference:
c
c           J.T.Feo,  An Analysis of the Computational and
c           Parallel Complexity of the Livermore Loops,
c           PARALLEL COMPUTING 7(2), 163-185(1988).
c
c
c
c
c
c
c
c    4. SAMPLE OUTPUT FILE FROM SUBROUTINe REPORT: (CRAY-YMP832/CFT77 Compiler)
c                                                                       aus
c
c         Output file for the Mono-processed Standard Benchmark Test:
c         The following output was uni-processed on CRAY-YMP832 in a
c         fully loaded, multi-processing, multi-programming system:
c
c
c  >>> USE 72 SAMPLES LFK TEST RESULTS SUMMARY (line 700+)
c  >>> USE ALL RANGE STATISTICS FOR OFFICIAL QUOTATIONS.
c SECOVT:     16000  5.0567e-05     1.0000
c SECOVT:     32000  5.0555e-05     0.0002
c VERIFY:       200  5.0885e-05 =  Time Resolution of Cpu-timer
c
c         VERIFY ADEQUATE Loop SIZE VERSUS CPU CLOCK ACCURACY
c         -----     -------     -------    -------   --------
c         EXTRA     MAXIMUM     DIGITAL    DYNAMIC   RELATIVE
c         Loop      CPUTIME     CLOCK      CLOCK     TIMING
c         SIZE      SECONDS     ERROR      ERROR     ERROR
c         -----     -------     -------    -------   --------
c           256  4.0944e-04       0.00%      0.84%      0.66%
c           512  8.1144e-04       0.00%      0.42%      0.66%
c          1024  1.6184e-03       0.00%      0.22%      0.54%
c          2048  3.2344e-03       0.00%      0.13%      0.66%
c          4096  6.4544e-03       0.00%      0.56%      0.00%
c          6800       Repetition Count = MULTI * Loops2 =       50.000
c          8192  1.2917e-02       0.00%      0.53%      0.09%
c         16384  2.5677e-02       0.00%      0.25%      0.25%
c         32768  5.1674e-02       0.00%      0.56%      0.11%
c         65536  1.0320e-01       0.00%      0.28%      0.11%
c         -----     -------     -------    -------   --------
c
c
c  CLOCK CALIBRATION TEST OF INTERNAL CPU-TIMER: SECOND
c  MONOPROCESS THIS TEST, STANDALONE, NO TIMESHARING.
c  VERIFY TIMED INTERVALS SHOWN BELOW USING EXTERNAL CLOCK
c  START YOUR STOPWATCH NOW !
c
c            Verify  T or DT  observe external clock:
c
c            -------     -------      ------      -----
c            Total T ?   Delta T ?    Mflops ?    Flops
c            -------     -------      ------      -----
c      1        9.70        9.70       65.84    6.38411e+08
c      2       19.38        9.68       65.89    1.27682e+09
c      3       29.19        9.82       65.60    1.91523e+09
c      4       38.98        9.78       65.52    2.55364e+09
c            -------     -------      ------      -----
c  END CALIBRATION TEST.
c
c
c  ESTIMATED TOTAL JOB CPU-TIME:=    98.917 sec.  ( Nruns=       7 Trials)
c
c
c Trial=      1             ChkSum=  908    Pass=      0     Fail=      0
c Trial=      2             ChkSum=  908    Pass=      1     Fail=      0
c Trial=      3             ChkSum=  908    Pass=      2     Fail=      0
c Trial=      4             ChkSum=  908    Pass=      3     Fail=      0
c Trial=      5             ChkSum=  908    Pass=      4     Fail=      0
c Trial=      6             ChkSum=  908    Pass=      5     Fail=      0
c Trial=      7             ChkSum=  908    Pass=      6     Fail=      0
c1
c
c
c time TEST overhead (t err):
c
c      RUN        AVERAGE        STANDEV        MINIMUM        MAXIMUM
c TICK   1   9.856583e-07   7.917129e-10
c TICK   2   9.827836e-07   3.459393e-10
c TICK   3   9.845331e-07   4.918561e-10
c TICK   4   9.858521e-07   1.222940e-09
c TICK   5   9.843086e-07   3.900581e-12
c TICK   6   9.848147e-07   6.017975e-10
c TICK   7   9.839837e-07   1.912898e-10
c DATA   7   9.998664e-02   5.433200e-07   9.998564e-02   9.998765e-02
c DATA   7   9.998599e-02   9.174233e-07   9.998434e-02   9.998765e-02
c TICK   7   9.845620e-07   9.630494e-10   9.827836e-07   9.858521e-07
c
c
c THE EXPERIMENTAL TIMING ERRORS FOR ALL  7 RUNS
c --  ---------  ---------  --------- -----  -----   ---
c  k   T min      T avg      T max    T err   tick   P-F
c --  ---------  ---------  --------- -----  -----   ---
c  1 2.7669e-02 2.7708e-02 2.7761e-02  0.13%  0.00%     0
c  2 1.0227e-01 1.0274e-01 1.0327e-01  0.31%  0.00%     0
c  3 1.5476e-02 1.5608e-02 1.5690e-02  0.40%  0.00%     0
c  4 4.1177e-02 4.1403e-02 4.1745e-02  0.52%  0.00%     0
c  5 3.6697e-01 3.6710e-01 3.6722e-01  0.02%  0.00%     0
c  6 1.1576e-01 1.1602e-01 1.1629e-01  0.18%  0.00%     0
c  7 4.3866e-02 4.3928e-02 4.4012e-02  0.11%  0.00%     0
c  8 6.5278e-02 6.5553e-02 6.6188e-02  0.45%  0.00%     0
c  9 5.4051e-02 5.4393e-02 5.4880e-02  0.65%  0.00%     0
c 10 5.7777e-02 5.8703e-02 5.9561e-02  1.20%  0.00%     0
c 11 2.8525e-01 2.8528e-01 2.8531e-01  0.01%  0.00%     0
c 12 2.2396e-02 2.2675e-02 2.3112e-02  0.96%  0.00%     0
c 13 1.5454e-01 1.5542e-01 1.5671e-01  0.53%  0.00%     0
c 14 1.4662e-01 1.4694e-01 1.4733e-01  0.17%  0.00%     0
c 15 4.4328e-01 4.4352e-01 4.4394e-01  0.05%  0.00%     0
c 16 3.3733e-01 3.3750e-01 3.3766e-01  0.03%  0.00%     0
c 17 4.6434e-01 4.6448e-01 4.6475e-01  0.03%  0.00%     0
c 18 4.6521e-02 4.6617e-02 4.6883e-02  0.26%  0.00%     0
c 19 5.6179e-01 5.6192e-01 5.6215e-01  0.02%  0.00%     0
c 20 3.1377e-01 3.1387e-01 3.1395e-01  0.02%  0.00%     0
c 21 1.9713e-01 1.9862e-01 1.9933e-01  0.36%  0.00%     0
c 22 3.7298e-02 3.7309e-02 3.7319e-02  0.02%  0.00%     0
c 23 4.8072e-01 4.8090e-01 4.8128e-01  0.04%  0.00%     0
c 24 2.5737e-02 2.5783e-02 2.5863e-02  0.14%  0.00%     0
c --  ---------  ---------  --------- -----  -----   ---
c
c
c NET CPU TIMING VARIANCE (T err);  A few % is ok:
c
c                 AVERAGE        STANDEV        MINIMUM        MAXIMUM
c     Terr          0.28%          0.31%          0.01%          1.20%
c
c
c
c
c
c
c
c
c
c
c
c
c1
c ********************************************
c THE LIVERMORE  FORTRAN KERNELS:  M F L O P S
c ********************************************
c
c              Computer : CRAY-YMP (6.0ns)
c              System   : NLTSS   fully loaded
c              Compiler : CFT77 4.0.3.4
c              Date     : 91.07.14
c         Mean DO Span  =   471
c         When the computer performance range is very large
c         the net Mflops rate of many Fortran programs and
c         workloads will be in the sub-range between the equi-
c         weighted Harmonic and Arithmetic means depending
c         on the degree of code parallelism and optimization.
c         The least biased central measure is the Geometric
c         Mean of 72 rates,  quoted +- a standard deviation.
c         Mean Mflops rates imply the average efficiency of a
c         computing system since the peak rate is well known.
c         LFK test measures a lower bound for a Multi-processor
c         and N * LFK rates project an upper bound for N-procs.
c
c KERNEL  FLOPS   MICROSEC   MFLOP/SEC SPAN WEIGHT  CHECK-SUMS             OK
c ------  -----   --------   --------- ---- ------  ---------------------- --
c  1 7.0070e+06 2.7708e+04    252.8850 1001   1.00  3.5802568852572702e+05 14
c  2 5.1992e+06 1.0274e+05     50.6078  101   1.00  1.0778052681679546e+04 13
c  3 3.6036e+06 1.5608e+04    230.8880 1001   1.00  7.0052001814661253e+01 13
c  4 3.3600e+06 4.1403e+04     81.1541 1001   1.00  4.1994754168318025e+00 14
c  5 4.0000e+06 3.6710e+05     10.8963 1001   1.00  3.1842101496719639e+04 13
c  6 2.3808e+06 1.1602e+05     20.5205   64   1.00  3.0625814413198270e+04 12
c  7 1.2736e+07 4.3928e+04    289.9310  995   1.00  4.2729757526224293e+05 14
c  8 1.4256e+07 6.5553e+04    217.4721  100   1.00  1.0508876039390042e+06 12
c  9 1.2362e+07 5.4393e+04    227.2807  101   1.00  8.3261052698255703e+05 13
c 10 6.1812e+06 5.8703e+04    105.2963  101   1.00  5.1172588490281440e+05 13
c 11 2.2000e+06 2.8528e+05      7.7117 1001   1.00  2.3400376808530903e+08 12
c 12 2.4000e+06 2.2675e+04    105.8436 1000   1.00  2.0349990127721540e-04  7
c 13 3.2256e+06 1.5542e+05     20.7546   64   1.00  8.4177377140212891e+11  9
c 14 4.4044e+06 1.4694e+05     29.9742 1001   1.00  2.2158871308010742e+10 12
c 15 3.3000e+06 4.4352e+05      7.4405  101   1.00  2.7606716832465865e+05 13
c 16 2.6500e+06 3.3750e+05      7.8518   75   1.00  3.9555320000000000e+06 16
c 17 6.3630e+06 4.6448e+05     13.6991  101   1.00  7.8024924103214871e+03 12
c 18 8.7120e+06 4.6617e+04    186.8842  100   1.00  7.1100892625153065e+05 13
c 19 4.7268e+06 5.6192e+05      8.4119  101   1.00  3.7952718721052370e+03 12
c 20 5.2000e+06 3.1387e+05     16.5676 1000   1.00  2.1284510375197506e+08 11
c 21 2.5250e+07 1.9862e+05    127.1262  101   1.00  1.1181157964903107e+09 11
c 22 3.7774e+06 3.7309e+04    101.2451  101   1.00  2.0570230635969638e+03 13
c 23 8.7120e+06 4.8090e+05     18.1160  100   1.00  2.4849303510962054e+05 12
c 24 1.0000e+06 2.5783e+04     38.7847 1001   1.00  3.5000000000000000e+03 16
c ------  -----   --------   --------- ---- ------  ---------------------- --
c 24 1.5301e+08 4.4140e+06     34.6643  471                               300
c
c         MFLOPS    RANGE:             REPORT ALL RANGE STATISTICS:
c         Maximum   Rate =    289.9310 Mega-Flops/Sec.
c         Quartile  Q3   =    157.0052 Mega-Flops/Sec.
c         Average   Rate =     90.7226 Mega-Flops/Sec.
c         Geometric Mean =     45.9242 Mega-Flops/Sec.
c         Median    Q2   =     44.6962 Mega-Flops/Sec.
c         Harmonic  Mean =     23.0425 Mega-Flops/Sec.
c         Quartile  Q1   =     15.1334 Mega-Flops/Sec.
c         Minimum   Rate =      7.4405 Mega-Flops/Sec.
c
c
c         Standard  Dev. =     91.2693 Mega-Flops/Sec.
c         Avg Efficiency =     15.84%  Program & Processor
c         Mean Precision =     12.50   Decimal Digits
c1
c
c
c
c
c
c
c                    SENSITIVITY ANALYSIS
c
c
c         The sensitivity of the harmonic mean rate (Mflops)
c         to various weightings is shown in the table below.
c         Seven work distributions are generated by assigning
c         two distinct weights to ranked kernels by quartiles.
c         Forty nine possible cpu workloads are then evaluated
c         using seven sets of values for the total weights:
c
c
c             ------ ------ ------ ------ ------ ------ ------
c   1st QT:       O      O      O      O      O      X      X
c   2nd QT:       O      O      O      X      X      X      O
c   3rd QT:       O      X      X      X      O      O      O
c   4th QT:       X      X      O      O      O      O      O
c             ------ ------ ------ ------ ------ ------ ------
c   Total
c   Weights                    Net Mflops:
c    X    O
c  ---- ----
c
c  1.00 0.00    8.89  12.68  22.09  35.25  87.25 126.51 230.03
c
c  0.95 0.05    9.27  13.27  22.15  33.48  73.58  87.31 143.87
c
c  0.90 0.10    9.68  13.93  22.21  31.88  63.61  66.65 104.67
c
c  0.80 0.20   10.63  15.46  22.34  29.09  50.05  45.24  67.75
c
c  0.70 0.30   11.78  17.36  22.46  26.75  41.26  34.25  50.08
c
c  0.60 0.40   13.22  19.80  22.59  24.76  35.09  27.55  39.72
c
c  0.50 0.50   15.05  23.04  22.72  23.04  30.53  23.04  32.92
c  ---- ----
c             ------ ------ ------ ------ ------ ------ ------
c
c
c
c
c
c
c SENSITIVITY OF NET MFLOPS RATE TO USE OF OPTIMAL FORTRAN CODE(SISD/SIMD MODEL)
c
c   12.68  15.63  20.38  29.27  37.44   51.93   84.73  123.85  230.03
c
c    0.00   0.20   0.40   0.60   0.70    0.80    0.90    0.95    1.00
c    Fraction Of Operations Run At Optimal Fortran Rates
c
c
c1
c
c
c time TEST overhead (t err):
c
c      RUN        AVERAGE        STANDEV        MINIMUM        MAXIMUM
c TICK   1   9.856583e-07   7.917129e-10
c TICK   2   9.827836e-07   3.459393e-10
c TICK   3   9.845331e-07   4.918561e-10
c TICK   4   9.858521e-07   1.222940e-09
c TICK   5   9.843086e-07   3.900581e-12
c TICK   6   9.848147e-07   6.017975e-10
c TICK   7   9.839837e-07   1.912898e-10
c DATA   7   9.998664e-02   5.433200e-07   9.998564e-02   9.998765e-02
c DATA   7   9.998599e-02   9.174233e-07   9.998434e-02   9.998765e-02
c TICK   7   9.845620e-07   9.630494e-10   9.827836e-07   9.858521e-07
c
c
c THE EXPERIMENTAL TIMING ERRORS FOR ALL  7 RUNS
c --  ---------  ---------  --------- -----  -----   ---
c  k   T min      T avg      T max    T err   tick   P-F
c --  ---------  ---------  --------- -----  -----   ---
c  1 4.0964e-02 4.1146e-02 4.1528e-02  0.43%  0.00%     0
c  2 1.2248e-01 1.2274e-01 1.2312e-01  0.16%  0.00%     0
c  3 6.0446e-02 6.0540e-02 6.0602e-02  0.10%  0.00%     0
c  4 2.5181e-01 2.5194e-01 2.5210e-01  0.04%  0.00%     0
c  5 4.1136e-01 4.1142e-01 4.1148e-01  0.01%  0.00%     0
c  6 2.2587e-01 2.2615e-01 2.2647e-01  0.08%  0.00%     0
c  7 5.2859e-02 5.3120e-02 5.3446e-02  0.43%  0.00%     0
c  8 7.8265e-02 7.8555e-02 7.9045e-02  0.35%  0.00%     0
c  9 6.2854e-02 6.3375e-02 6.3858e-02  0.55%  0.00%     0
c 10 6.3650e-02 6.4887e-02 6.6192e-02  1.29%  0.00%     0
c 11 3.4608e-01 3.4615e-01 3.4623e-01  0.01%  0.00%     0
c 12 3.3683e-02 3.3878e-02 3.4230e-02  0.47%  0.00%     0
c 13 2.0237e-01 2.0332e-01 2.0461e-01  0.35%  0.00%     0
c 14 1.5414e-01 1.5473e-01 1.5509e-01  0.20%  0.00%     0
c 15 8.8648e-01 8.8713e-01 8.8768e-01  0.04%  0.00%     0
c 16 3.9403e-01 3.9427e-01 3.9466e-01  0.05%  0.00%     0
c 17 5.3069e-01 5.3093e-01 5.3158e-01  0.05%  0.00%     0
c 18 4.6512e-02 4.6655e-02 4.7000e-02  0.33%  0.00%     0
c 19 6.6284e-01 6.6303e-01 6.6350e-01  0.03%  0.00%     0
c 20 5.0282e-01 5.0312e-01 5.0355e-01  0.04%  0.00%     0
c 21 2.6824e-01 2.7087e-01 2.7489e-01  0.97%  0.00%     0
c 22 4.7450e-02 4.7480e-02 4.7510e-02  0.04%  0.00%     0
c 23 6.0078e-01 6.0143e-01 6.0233e-01  0.10%  0.00%     0
c 24 8.3666e-02 8.3784e-02 8.4103e-02  0.18%  0.00%     0
c --  ---------  ---------  --------- -----  -----   ---
c
c
c NET CPU TIMING VARIANCE (T err);  A few % is ok:
c
c                 AVERAGE        STANDEV        MINIMUM        MAXIMUM
c     Terr          0.26%          0.31%          0.01%          1.29%
c
c
c
c
c
c
c
c
c
c
c
c
c1
c ********************************************
c THE LIVERMORE  FORTRAN KERNELS:  M F L O P S
c ********************************************
c
c              Computer : CRAY-YMP (6.0ns)
c              System   : NLTSS   fully loaded
c              Compiler : CFT77 4.0.3.4
c              Date     : 91.07.14
c         Mean DO Span  =    90
c         When the computer performance range is very large
c         the net Mflops rate of many Fortran programs and
c         workloads will be in the sub-range between the equi-
c         weighted Harmonic and Arithmetic means depending
c         on the degree of code parallelism and optimization.
c         The least biased central measure is the Geometric
c         Mean of 72 rates,  quoted +- a standard deviation.
c         Mean Mflops rates imply the average efficiency of a
c         computing system since the peak rate is well known.
c         LFK test measures a lower bound for a Multi-processor
c         and N * LFK rates project an upper bound for N-procs.
c
c KERNEL  FLOPS   MICROSEC   MFLOP/SEC SPAN WEIGHT  CHECK-SUMS             OK
c ------  -----   --------   --------- ---- ------  ---------------------- --
c  1 8.0800e+06 4.1146e+04    196.3722  101   2.00  3.6773413452565728e+03 16
c  2 6.2080e+06 1.2274e+05     50.5788  101   2.00  1.0778052681679546e+04 13
c  3 4.2824e+06 6.0540e+04     70.7366  101   2.00  7.0681900560541635e+00 13
c  4 3.3600e+06 2.5194e+05     13.3367  101   2.00  4.1994754168318025e+00 14
c  5 4.4000e+06 4.1142e+05     10.6948  101   2.00  3.2123223577214776e+02 13
c  6 2.6880e+06 2.2615e+05     11.8860   32   2.00  6.0421729517419226e+02 12
c  7 1.4221e+07 5.3120e+04    267.7098  101   2.00  4.4419104210488440e+03 14
c  8 1.7107e+07 7.8555e+04    217.7736  100   2.00  1.0508876039390042e+06 12
c  9 1.4423e+07 6.3375e+04    227.5796  101   2.00  8.3261052698255703e+05 13
c 10 6.9084e+06 6.4887e+04    106.4682  101   2.00  5.1172588490281440e+05 13
c 11 2.5600e+06 3.4615e+05      7.3957  101   2.00  2.4034922852330748e+05 14
c 12 2.7200e+06 3.3878e+04     80.2871  100   2.00  4.9892984137045460e-05  8
c 13 3.6736e+06 2.0332e+05     18.0678   32   2.00  6.8714714682226172e+11 10
c 14 4.4440e+06 1.5473e+05     28.7211  101   2.00  2.1279884256031895e+08 12
c 15 6.6000e+06 8.8713e+05      7.4397  101   2.00  2.7606716832465865e+05 13
c 16 3.0240e+06 3.9427e+05      7.6699   40   2.00  4.5362870000000000e+06 16
c 17 7.2720e+06 5.3093e+05     13.6968  101   2.00  7.8024924103214871e+03 12
c 18 8.7120e+06 4.6655e+04    186.7309  100   2.00  7.1100892625153065e+05 13
c 19 5.5752e+06 6.6303e+05      8.4086  101   2.00  3.7952718721052370e+03 12
c 20 8.3200e+06 5.0312e+05     16.5369  100   2.00  2.1883436251674779e+05 12
c 21 2.5000e+07 2.7087e+05     92.2937   50   2.00  5.4771674139930725e+08 11
c 22 4.8076e+06 4.7480e+04    101.2550  101   2.00  2.0570230635969638e+03 13
c 23 1.0890e+07 6.0143e+05     18.1067  100   2.00  2.4849303510962054e+05 12
c 24 1.2400e+06 8.3784e+04     14.7999  101   2.00  3.5000000000000000e+02 16
c ------  -----   --------   --------- ---- ------  ---------------------- --
c 24 1.7652e+08 6.1406e+06     28.7455   90                               307
c
c         MFLOPS    RANGE:             REPORT ALL RANGE STATISTICS:
c         Maximum   Rate =    267.7098 Mega-Flops/Sec.
c         Quartile  Q3   =    103.8616 Mega-Flops/Sec.
c         Average   Rate =     73.9394 Mega-Flops/Sec.
c         Geometric Mean =     36.2408 Mega-Flops/Sec.
c         Median    Q2   =     23.4139 Mega-Flops/Sec.
c         Harmonic  Mean =     19.7105 Mega-Flops/Sec.
c         Quartile  Q1   =     12.6114 Mega-Flops/Sec.
c         Minimum   Rate =      7.3957 Mega-Flops/Sec.
c
c
c         Standard  Dev. =     81.6635 Mega-Flops/Sec.
c         Avg Efficiency =     12.50%  Program & Processor
c         Mean Precision =     12.79   Decimal Digits
c1
c
c
c
c
c
c
c                    SENSITIVITY ANALYSIS
c
c
c         The sensitivity of the harmonic mean rate (Mflops)
c         to various weightings is shown in the table below.
c         Seven work distributions are generated by assigning
c         two distinct weights to ranked kernels by quartiles.
c         Forty nine possible cpu workloads are then evaluated
c         using seven sets of values for the total weights:
c
c
c             ------ ------ ------ ------ ------ ------ ------
c   1st QT:       O      O      O      O      O      X      X
c   2nd QT:       O      O      O      X      X      X      O
c   3rd QT:       O      X      X      X      O      O      O
c   4th QT:       X      X      O      O      O      O      O
c             ------ ------ ------ ------ ------ ------ ------
c   Total
c   Weights                    Net Mflops:
c    X    O
c  ---- ----
c
c  1.00 0.00    8.61  11.08  15.52  24.57  58.89  89.25 184.27
c
c  0.95 0.05    8.95  11.59  15.74  23.97  52.00  65.97 118.38
c
c  0.90 0.10    9.31  12.14  15.97  23.41  46.55  52.33  87.20
c
c  0.80 0.20   10.14  13.43  16.45  22.36  38.49  37.01  57.11
c
c  0.70 0.30   11.12  15.03  16.96  21.40  32.81  28.63  42.46
c
c  0.60 0.40   12.31  17.05  17.50  20.52  28.59  23.35  33.79
c
c  0.50 0.50   13.79  19.71  18.08  19.71  25.33  19.71  28.06
c  ---- ----
c             ------ ------ ------ ------ ------ ------ ------
c
c
c
c
c
c
c SENSITIVITY OF NET MFLOPS RATE TO USE OF OPTIMAL FORTRAN CODE(SISD/SIMD MODEL)
c
c   11.08  13.64  17.75  25.41  32.39   44.65   71.89  103.43  184.27
c
c    0.00   0.20   0.40   0.60   0.70    0.80    0.90    0.95    1.00
c    Fraction Of Operations Run At Optimal Fortran Rates
c
c
c1
c
c
c time TEST overhead (t err):
c
c      RUN        AVERAGE        STANDEV        MINIMUM        MAXIMUM
c TICK   1   9.856583e-07   7.917129e-10
c TICK   2   9.827836e-07   3.459393e-10
c TICK   3   9.845331e-07   4.918561e-10
c TICK   4   9.858521e-07   1.222940e-09
c TICK   5   9.843086e-07   3.900581e-12
c TICK   6   9.848147e-07   6.017975e-10
c TICK   7   9.839837e-07   1.912898e-10
c DATA   7   9.998664e-02   5.433200e-07   9.998564e-02   9.998765e-02
c DATA   7   9.998599e-02   9.174233e-07   9.998434e-02   9.998765e-02
c TICK   7   9.845620e-07   9.630494e-10   9.827836e-07   9.858521e-07
c
c
c THE EXPERIMENTAL TIMING ERRORS FOR ALL  7 RUNS
c --  ---------  ---------  --------- -----  -----   ---
c  k   T min      T avg      T max    T err   tick   P-F
c --  ---------  ---------  --------- -----  -----   ---
c  1 4.6552e-02 4.6885e-02 4.7277e-02  0.44%  0.00%     0
c  2 2.3215e-01 2.3247e-01 2.3281e-01  0.10%  0.00%     0
c  3 1.3283e-01 1.3295e-01 1.3311e-01  0.07%  0.00%     0
c  4 5.4677e-01 5.4712e-01 5.4839e-01  0.10%  0.00%     0
c  5 3.2937e-01 3.2952e-01 3.2969e-01  0.03%  0.00%     0
c  6 5.7700e-01 5.7735e-01 5.7791e-01  0.06%  0.00%     0
c  7 5.7375e-02 5.7444e-02 5.7533e-02  0.10%  0.00%     0
c  8 1.1030e-01 1.1109e-01 1.1179e-01  0.55%  0.00%     0
c  9 8.8652e-02 8.9477e-02 9.0447e-02  0.64%  0.00%     0
c 10 8.1429e-02 8.1862e-02 8.2501e-02  0.42%  0.00%     0
c 11 2.7512e-01 2.7518e-01 2.7528e-01  0.02%  0.00%     0
c 12 4.5972e-02 4.6355e-02 4.6689e-02  0.47%  0.00%     0
c 13 2.9003e-01 2.9097e-01 2.9194e-01  0.19%  0.00%     0
c 14 1.5044e-01 1.5086e-01 1.5181e-01  0.29%  0.00%     0
c 15 4.9302e-01 4.9311e-01 4.9326e-01  0.02%  0.00%     0
c 16 3.2175e-01 3.2196e-01 3.2236e-01  0.06%  0.00%     0
c 17 3.9370e-01 3.9382e-01 3.9397e-01  0.03%  0.00%     0
c 18 1.0431e-01 1.0454e-01 1.0478e-01  0.17%  0.00%     0
c 19 4.9471e-01 4.9496e-01 4.9519e-01  0.03%  0.00%     0
c 20 4.5972e-01 4.5992e-01 4.6005e-01  0.03%  0.00%     0
c 21 8.4632e-01 8.5030e-01 8.5329e-01  0.29%  0.00%     0
c 22 5.1496e-02 5.1531e-02 5.1561e-02  0.05%  0.00%     0
c 23 4.6724e-01 4.6751e-01 4.6771e-01  0.04%  0.00%     0
c 24 1.4363e-01 1.4376e-01 1.4387e-01  0.05%  0.00%     0
c --  ---------  ---------  --------- -----  -----   ---
c
c
c NET CPU TIMING VARIANCE (T err);  A few % is ok:
c
c                 AVERAGE        STANDEV        MINIMUM        MAXIMUM
c     Terr          0.18%          0.19%          0.02%          0.64%
c
c
c
c
c
c
c
c
c
c
c
c
c1
c ********************************************
c THE LIVERMORE  FORTRAN KERNELS:  M F L O P S
c ********************************************
c
c              Computer : CRAY-YMP (6.0ns)
c              System   : NLTSS   fully loaded
c              Compiler : CFT77 4.0.3.4
c              Date     : 91.07.14
c         Mean DO Span  =    19
c         When the computer performance range is very large
c         the net Mflops rate of many Fortran programs and
c         workloads will be in the sub-range between the equi-
c         weighted Harmonic and Arithmetic means depending
c         on the degree of code parallelism and optimization.
c         The least biased central measure is the Geometric
c         Mean of 72 rates,  quoted +- a standard deviation.
c         Mean Mflops rates imply the average efficiency of a
c         computing system since the peak rate is well known.
c         LFK test measures a lower bound for a Multi-processor
c         and N * LFK rates project an upper bound for N-procs.
c
c KERNEL  FLOPS   MICROSEC   MFLOP/SEC SPAN WEIGHT  CHECK-SUMS             OK
c ------  -----   --------   --------- ---- ------  ---------------------- --
c  1 6.0480e+06 4.6885e+04    128.9966   27   1.00  2.6985731517464592e+02 16
c  2 3.2384e+06 2.3247e+05     13.9304   15   1.00  2.7673078908323623e+02 13
c  3 3.1968e+06 1.3295e+05     24.0453   27   1.00  1.8895163625248799e+00 13
c  4 1.8240e+06 5.4712e+05      3.3338   27   1.00  4.1994754168318025e+00 14
c  5 3.3280e+06 3.2952e+05     10.0996   27   1.00  2.2278306739137747e+01 13
c  6 1.6128e+06 5.7735e+05      2.7934    8   1.00  7.8421657542730827e+00 12
c  7 1.0752e+07 5.7444e+04    187.1752   21   1.00  1.9920041523508189e+02 14
c  8 1.3478e+07 1.1109e+05    121.3250   14   1.00  2.0723805675143376e+04 12
c  9 1.0608e+07 8.9477e+04    118.5558   15   1.00  1.8367779226120794e+04 13
c 10 5.4000e+06 8.1862e+04     65.9647   15   1.00  1.1559038593888574e+04 13
c 11 1.9136e+06 2.7518e+05      6.9540   27   1.00  4.5858129350924282e+03 13
c 12 1.9968e+06 4.6355e+04     43.0764   26   1.00  1.3604051882776247e-05  9
c 13 2.7776e+06 2.9097e+05      9.5460    8   1.00  2.6929869385100195e+11  9
c 14 3.8016e+06 1.5086e+05     25.1997   27   1.00  2.0464784190878510e+07 11
c 15 3.6960e+06 4.9311e+05      7.4953   15   1.00  7.7629810169453849e+03 13
c 16 2.4640e+06 3.2196e+05      7.6531   15   1.00  3.6065120000000000e+06 16
c 17 5.6160e+06 3.9382e+05     14.2605   15   1.00  2.0631580330134784e+02 12
c 18 9.1520e+06 1.0454e+05     87.5496   14   1.00  6.7904523486389953e+03 12
c 19 4.0320e+06 4.9496e+05      8.1461   15   1.00  8.8776148863621074e+01 12
c 20 7.5712e+06 4.5992e+05     16.4620   26   1.00  4.1913992746301519e+03 12
c 21 4.0000e+07 8.5030e+05     47.0421   20   1.00  3.5069619696601486e+08 10
c 22 3.2640e+06 5.1531e+04     63.3400   15   1.00  4.2769781097853183e+01 13
c 23 8.0080e+06 4.6751e+05     17.1291   14   1.00  3.3952384219261585e+03 12
c 24 9.5680e+05 1.4376e+05      6.6557   27   1.00  9.1000000000000000e+01 16
c ------  -----   --------   --------- ---- ------  ---------------------- --
c 24 1.5474e+08 6.7509e+06     22.9207   19                               303
c
c         MFLOPS    RANGE:             REPORT ALL RANGE STATISTICS:
c         Maximum   Rate =    187.1752 Mega-Flops/Sec.
c         Quartile  Q3   =     64.6524 Mega-Flops/Sec.
c         Average   Rate =     43.1971 Mega-Flops/Sec.
c         Geometric Mean =     22.0242 Mega-Flops/Sec.
c         Median    Q2   =     16.7955 Mega-Flops/Sec.
c         Harmonic  Mean =     11.9909 Mega-Flops/Sec.
c         Quartile  Q1   =      7.8996 Mega-Flops/Sec.
c         Minimum   Rate =      2.7934 Mega-Flops/Sec.
c
c
c         Standard  Dev. =     49.3389 Mega-Flops/Sec.
c         Avg Efficiency =      7.60%  Program & Processor
c         Mean Precision =     12.62   Decimal Digits
c1
c
c
c
c
c
c
c                    SENSITIVITY ANALYSIS
c
c
c         The sensitivity of the harmonic mean rate (Mflops)
c         to various weightings is shown in the table below.
c         Seven work distributions are generated by assigning
c         two distinct weights to ranked kernels by quartiles.
c         Forty nine possible cpu workloads are then evaluated
c         using seven sets of values for the total weights:
c
c
c             ------ ------ ------ ------ ------ ------ ------
c   1st QT:       O      O      O      O      O      X      X
c   2nd QT:       O      O      O      X      X      X      O
c   3rd QT:       O      X      X      X      O      O      O
c   4th QT:       X      X      O      O      O      O      O
c             ------ ------ ------ ------ ------ ------ ------
c   Total
c   Weights                    Net Mflops:
c    X    O
c  ---- ----
c
c  1.00 0.00    4.93   6.88  11.34  16.46  30.01  46.83 106.47
c
c  0.95 0.05    5.14   7.18  11.38  15.87  27.28  36.29  69.80
c
c  0.90 0.10    5.35   7.52  11.42  15.32  25.00  29.62  51.92
c
c  0.80 0.20    5.85   8.29  11.51  14.32  21.43  21.66  34.33
c
c  0.70 0.30    6.45   9.24  11.59  13.45  18.74  17.07  25.64
c
c  0.60 0.40    7.19  10.44  11.68  12.68  16.66  14.09  20.47
c
c  0.50 0.50    8.12  11.99  11.77  11.99  14.99  11.99  17.03
c  ---- ----
c             ------ ------ ------ ------ ------ ------ ------
c
c
c
c
c
c
c SENSITIVITY OF NET MFLOPS RATE TO USE OF OPTIMAL FORTRAN CODE(SISD/SIMD MODEL)
c
c    6.88   8.46  10.99  15.67  19.92   27.32   43.48   61.75  106.47
c
c    0.00   0.20   0.40   0.60   0.70    0.80    0.90    0.95    1.00
c    Fraction Of Operations Run At Optimal Fortran Rates
c
c
c
c
c
c
c
c
c
c
c
c
c
c
c1
c ********************************************
c THE LIVERMORE  FORTRAN KERNELS:  * SUMMARY *
c ********************************************
c
c              Computer : CRAY-YMP (6.0ns)
c              System   : NLTSS   fully loaded
c              Compiler : CFT77 4.0.3.4
c              Date     : 91.07.14
c         Mean DO Span  =   167
c         When the computer performance range is very large
c         the net Mflops rate of many Fortran programs and
c         workloads will be in the sub-range between the equi-
c         weighted Harmonic and Arithmetic means depending
c         on the degree of code parallelism and optimization.
c         The least biased central measure is the Geometric
c         Mean of 72 rates,  quoted +- a standard deviation.
c         Mean Mflops rates imply the average efficiency of a
c         computing system since the peak rate is well known.
c         LFK test measures a lower bound for a Multi-processor
c         and N * LFK rates project an upper bound for N-procs.
c
c KERNEL  FLOPS   MICROSEC   MFLOP/SEC SPAN WEIGHT  CHECK-SUMS             OK
c ------  -----   --------   --------- ---- ------  ---------------------- --
c  1 6.0480e+06 4.6885e+04    128.9966   27   1.00  2.6985731517464592e+02 16
c  2 3.2384e+06 2.3247e+05     13.9304   15   1.00  2.7673078908323623e+02 13
c  3 3.1968e+06 1.3295e+05     24.0453   27   1.00  1.8895163625248799e+00 13
c  4 1.8240e+06 5.4712e+05      3.3338   27   1.00  4.1994754168318025e+00 14
c  5 3.3280e+06 3.2952e+05     10.0996   27   1.00  2.2278306739137747e+01 13
c  6 1.6128e+06 5.7735e+05      2.7934    8   1.00  7.8421657542730827e+00 12
c  7 1.0752e+07 5.7444e+04    187.1752   21   1.00  1.9920041523508189e+02 14
c  8 1.3478e+07 1.1109e+05    121.3250   14   1.00  2.0723805675143376e+04 12
c  9 1.0608e+07 8.9477e+04    118.5558   15   1.00  1.8367779226120794e+04 13
c 10 5.4000e+06 8.1862e+04     65.9647   15   1.00  1.1559038593888574e+04 13
c 11 1.9136e+06 2.7518e+05      6.9540   27   1.00  4.5858129350924282e+03 13
c 12 1.9968e+06 4.6355e+04     43.0764   26   1.00  1.3604051882776247e-05  9
c 13 2.7776e+06 2.9097e+05      9.5460    8   1.00  2.6929869385100195e+11  9
c 14 3.8016e+06 1.5086e+05     25.1997   27   1.00  2.0464784190878510e+07 11
c 15 3.6960e+06 4.9311e+05      7.4953   15   1.00  7.7629810169453849e+03 13
c 16 2.4640e+06 3.2196e+05      7.6531   15   1.00  3.6065120000000000e+06 16
c 17 5.6160e+06 3.9382e+05     14.2605   15   1.00  2.0631580330134784e+02 12
c 18 9.1520e+06 1.0454e+05     87.5496   14   1.00  6.7904523486389953e+03 12
c 19 4.0320e+06 4.9496e+05      8.1461   15   1.00  8.8776148863621074e+01 12
c 20 7.5712e+06 4.5992e+05     16.4620   26   1.00  4.1913992746301519e+03 12
c 21 4.0000e+07 8.5030e+05     47.0421   20   1.00  3.5069619696601486e+08 10
c 22 3.2640e+06 5.1531e+04     63.3400   15   1.00  4.2769781097853183e+01 13
c 23 8.0080e+06 4.6751e+05     17.1291   14   1.00  3.3952384219261585e+03 12
c 24 9.5680e+05 1.4376e+05      6.6557   27   1.00  9.1000000000000000e+01 16
c  1 8.0800e+06 4.1146e+04    196.3722  101   2.00  3.6773413452565728e+03 16
c  2 6.2080e+06 1.2274e+05     50.5788  101   2.00  1.0778052681679546e+04 13
c  3 4.2824e+06 6.0540e+04     70.7366  101   2.00  7.0681900560541635e+00 13
c  4 3.3600e+06 2.5194e+05     13.3367  101   2.00  4.1994754168318025e+00 14
c  5 4.4000e+06 4.1142e+05     10.6948  101   2.00  3.2123223577214776e+02 13
c  6 2.6880e+06 2.2615e+05     11.8860   32   2.00  6.0421729517419226e+02 12
c  7 1.4221e+07 5.3120e+04    267.7098  101   2.00  4.4419104210488440e+03 14
c  8 1.7107e+07 7.8555e+04    217.7736  100   2.00  1.0508876039390042e+06 12
c  9 1.4423e+07 6.3375e+04    227.5796  101   2.00  8.3261052698255703e+05 13
c 10 6.9084e+06 6.4887e+04    106.4682  101   2.00  5.1172588490281440e+05 13
c 11 2.5600e+06 3.4615e+05      7.3957  101   2.00  2.4034922852330748e+05 14
c 12 2.7200e+06 3.3878e+04     80.2871  100   2.00  4.9892984137045460e-05  8
c 13 3.6736e+06 2.0332e+05     18.0678   32   2.00  6.8714714682226172e+11 10
c 14 4.4440e+06 1.5473e+05     28.7211  101   2.00  2.1279884256031895e+08 12
c 15 6.6000e+06 8.8713e+05      7.4397  101   2.00  2.7606716832465865e+05 13
c 16 3.0240e+06 3.9427e+05      7.6699   40   2.00  4.5362870000000000e+06 16
c 17 7.2720e+06 5.3093e+05     13.6968  101   2.00  7.8024924103214871e+03 12
c 18 8.7120e+06 4.6655e+04    186.7309  100   2.00  7.1100892625153065e+05 13
c 19 5.5752e+06 6.6303e+05      8.4086  101   2.00  3.7952718721052370e+03 12
c 20 8.3200e+06 5.0312e+05     16.5369  100   2.00  2.1883436251674779e+05 12
c 21 2.5000e+07 2.7087e+05     92.2937   50   2.00  5.4771674139930725e+08 11
c 22 4.8076e+06 4.7480e+04    101.2550  101   2.00  2.0570230635969638e+03 13
c 23 1.0890e+07 6.0143e+05     18.1067  100   2.00  2.4849303510962054e+05 12
c 24 1.2400e+06 8.3784e+04     14.7999  101   2.00  3.5000000000000000e+02 16
c  1 7.0070e+06 2.7708e+04    252.8850 1001   1.00  3.5802568852572702e+05 14
c  2 5.1992e+06 1.0274e+05     50.6078  101   1.00  1.0778052681679546e+04 13
c  3 3.6036e+06 1.5608e+04    230.8880 1001   1.00  7.0052001814661253e+01 13
c  4 3.3600e+06 4.1403e+04     81.1541 1001   1.00  4.1994754168318025e+00 14
c  5 4.0000e+06 3.6710e+05     10.8963 1001   1.00  3.1842101496719639e+04 13
c  6 2.3808e+06 1.1602e+05     20.5205   64   1.00  3.0625814413198270e+04 12
c  7 1.2736e+07 4.3928e+04    289.9310  995   1.00  4.2729757526224293e+05 14
c  8 1.4256e+07 6.5553e+04    217.4721  100   1.00  1.0508876039390042e+06 12
c  9 1.2362e+07 5.4393e+04    227.2807  101   1.00  8.3261052698255703e+05 13
c 10 6.1812e+06 5.8703e+04    105.2963  101   1.00  5.1172588490281440e+05 13
c 11 2.2000e+06 2.8528e+05      7.7117 1001   1.00  2.3400376808530903e+08 12
c 12 2.4000e+06 2.2675e+04    105.8436 1000   1.00  2.0349990127721540e-04  7
c 13 3.2256e+06 1.5542e+05     20.7546   64   1.00  8.4177377140212891e+11  9
c 14 4.4044e+06 1.4694e+05     29.9742 1001   1.00  2.2158871308010742e+10 12
c 15 3.3000e+06 4.4352e+05      7.4405  101   1.00  2.7606716832465865e+05 13
c 16 2.6500e+06 3.3750e+05      7.8518   75   1.00  3.9555320000000000e+06 16
c 17 6.3630e+06 4.6448e+05     13.6991  101   1.00  7.8024924103214871e+03 12
c 18 8.7120e+06 4.6617e+04    186.8842  100   1.00  7.1100892625153065e+05 13
c 19 4.7268e+06 5.6192e+05      8.4119  101   1.00  3.7952718721052370e+03 12
c 20 5.2000e+06 3.1387e+05     16.5676 1000   1.00  2.1284510375197506e+08 11
c 21 2.5250e+07 1.9862e+05    127.1262  101   1.00  1.1181157964903107e+09 11
c 22 3.7774e+06 3.7309e+04    101.2451  101   1.00  2.0570230635969638e+03 13
c 23 8.7120e+06 4.8090e+05     18.1160  100   1.00  2.4849303510962054e+05 12
c 24 1.0000e+06 2.5783e+04     38.7847 1001   1.00  3.5000000000000000e+03 16
c ------  -----   --------   --------- ---- ------  ---------------------- --
c 72 4.8426e+08 1.7306e+07     27.9829  167                               910
c
c         MFLOPS    RANGE:             REPORT ALL RANGE STATISTICS:
c         Maximum   Rate =    289.9310 Mega-Flops/Sec.
c         Quartile  Q3   =    105.2963 Mega-Flops/Sec.
c         Average   Rate =     70.4496 Mega-Flops/Sec.
c         Geometric Mean =     33.9496 Mega-Flops/Sec.
c         Median    Q2   =     24.0453 Mega-Flops/Sec.
c         Harmonic  Mean =     17.5236 Mega-Flops/Sec.
c         Quartile  Q1   =     10.8963 Mega-Flops/Sec.
c         Minimum   Rate =      2.7934 Mega-Flops/Sec.
c
c
c         Standard  Dev. =     79.4989 Mega-Flops/Sec.
c         Avg Efficiency =     11.71%  Program & Processor
c         Mean Precision =     12.64   Decimal Digits
c <<<<<<<<<<<<<<<<<<<<<<<<<<<*>>>>>>>>>>>>>>>>>>>>>>>>>>>
c < BOTTOM-LINE:   72 SAMPLES LFK TEST RESULTS SUMMARY. >
c < USE RANGE STATISTICS ABOVE FOR OFFICIAL QUOTATIONS. >
c <<<<<<<<<<<<<<<<<<<<<<<<<<<*>>>>>>>>>>>>>>>>>>>>>>>>>>>
c1
c
c
c TOP QUARTILE: BEST ARCHITECTURE/APPLICATION MATCH
c
c
c
c KERNEL  FLOPS   MICROSEC   MFLOP/SEC SPAN WEIGHT
c ------  -----   --------   --------- ---- ------
c  7 1.2736e+07 4.3928e+04    289.9310  995   1.00
c  7 1.4221e+07 5.3120e+04    267.7098  101   2.00
c  1 7.0070e+06 2.7708e+04    252.8850 1001   1.00
c  3 3.6036e+06 1.5608e+04    230.8880 1001   1.00
c  9 1.4423e+07 6.3375e+04    227.5796  101   2.00
c  9 1.2362e+07 5.4393e+04    227.2807  101   1.00
c  8 1.7107e+07 7.8555e+04    217.7736  100   2.00
c  8 1.4256e+07 6.5553e+04    217.4721  100   1.00
c  1 8.0800e+06 4.1146e+04    196.3722  101   2.00
c  7 1.0752e+07 5.7444e+04    187.1752   21   1.00
c 18 8.7120e+06 4.6617e+04    186.8842  100   1.00
c 18 8.7120e+06 4.6655e+04    186.7309  100   2.00
c  1 6.0480e+06 4.6885e+04    128.9966   27   1.00
c 21 2.5250e+07 1.9862e+05    127.1262  101   1.00
c  8 1.3478e+07 1.1109e+05    121.3250   14   1.00
c  9 1.0608e+07 8.9477e+04    118.5558   15   1.00
c 10 6.9084e+06 6.4887e+04    106.4682  101   2.00
c 12 2.4000e+06 2.2675e+04    105.8436 1000   1.00
c ------  -----   --------   --------- ---- ------
c
c         Frac.  Weights =      0.2500
c         Average   Rate =    191.6513 Mega-Flops/Sec.
c         Harmonic  Mean =    173.5450 Mega-Flops/Sec.
c         Standard  Dev. =     55.0716 Mega-Flops/Sec.
c
c
c
c KERNEL  FLOPS   MICROSEC   MFLOP/SEC SPAN WEIGHT
c ------  -----   --------   --------- ---- ------
c 10 6.1812e+06 5.8703e+04    105.2963  101   1.00
c 22 4.8076e+06 4.7480e+04    101.2550  101   2.00
c 22 3.7774e+06 3.7309e+04    101.2451  101   1.00
c 21 2.5000e+07 2.7087e+05     92.2937   50   2.00
c 18 9.1520e+06 1.0454e+05     87.5496   14   1.00
c  4 3.3600e+06 4.1403e+04     81.1541 1001   1.00
c 12 2.7200e+06 3.3878e+04     80.2871  100   2.00
c  3 4.2824e+06 6.0540e+04     70.7366  101   2.00
c 10 5.4000e+06 8.1862e+04     65.9647   15   1.00
c 22 3.2640e+06 5.1531e+04     63.3400   15   1.00
c  2 5.1992e+06 1.0274e+05     50.6078  101   1.00
c  2 6.2080e+06 1.2274e+05     50.5788  101   2.00
c 21 4.0000e+07 8.5030e+05     47.0421   20   1.00
c 12 1.9968e+06 4.6355e+04     43.0764   26   1.00
c 24 1.0000e+06 2.5783e+04     38.7847 1001   1.00
c 14 4.4044e+06 1.4694e+05     29.9742 1001   1.00
c 14 4.4440e+06 1.5473e+05     28.7211  101   2.00
c 14 3.8016e+06 1.5086e+05     25.1997   27   1.00
c  3 3.1968e+06 1.3295e+05     24.0453   27   1.00
c 13 3.2256e+06 1.5542e+05     20.7546   64   1.00
c  6 2.3808e+06 1.1602e+05     20.5205   64   1.00
c 23 8.7120e+06 4.8090e+05     18.1160  100   1.00
c 23 1.0890e+07 6.0143e+05     18.1067  100   2.00
c 13 3.6736e+06 2.0332e+05     18.0678   32   2.00
c 23 8.0080e+06 4.6751e+05     17.1291   14   1.00
c 20 5.2000e+06 3.1387e+05     16.5676 1000   1.00
c 20 8.3200e+06 5.0312e+05     16.5369  100   2.00
c 20 7.5712e+06 4.5992e+05     16.4620   26   1.00
c 24 1.2400e+06 8.3784e+04     14.7999  101   2.00
c 17 5.6160e+06 3.9382e+05     14.2605   15   1.00
c  2 3.2384e+06 2.3247e+05     13.9304   15   1.00
c 17 6.3630e+06 4.6448e+05     13.6991  101   1.00
c 17 7.2720e+06 5.3093e+05     13.6968  101   2.00
c  4 3.3600e+06 2.5194e+05     13.3367  101   2.00
c  6 2.6880e+06 2.2615e+05     11.8860   32   2.00
c  5 4.0000e+06 3.6710e+05     10.8963 1001   1.00
c ------  -----   --------   --------- ---- ------
c
c         Frac.  Weights =      0.5104
c         Average   Rate =     40.5352 Mega-Flops/Sec.
c         Harmonic  Mean =     23.7604 Mega-Flops/Sec.
c         Standard  Dev. =     30.9452 Mega-Flops/Sec.
c
c
c
c KERNEL  FLOPS   MICROSEC   MFLOP/SEC SPAN WEIGHT
c ------  -----   --------   --------- ---- ------
c  5 4.4000e+06 4.1142e+05     10.6948  101   2.00
c  5 3.3280e+06 3.2952e+05     10.0996   27   1.00
c 13 2.7776e+06 2.9097e+05      9.5460    8   1.00
c 19 4.7268e+06 5.6192e+05      8.4119  101   1.00
c 19 5.5752e+06 6.6303e+05      8.4086  101   2.00
c 19 4.0320e+06 4.9496e+05      8.1461   15   1.00
c 16 2.6500e+06 3.3750e+05      7.8518   75   1.00
c 11 2.2000e+06 2.8528e+05      7.7117 1001   1.00
c 16 3.0240e+06 3.9427e+05      7.6699   40   2.00
c 16 2.4640e+06 3.2196e+05      7.6531   15   1.00
c 15 3.6960e+06 4.9311e+05      7.4953   15   1.00
c 15 3.3000e+06 4.4352e+05      7.4405  101   1.00
c 15 6.6000e+06 8.8713e+05      7.4397  101   2.00
c 11 2.5600e+06 3.4615e+05      7.3957  101   2.00
c 11 1.9136e+06 2.7518e+05      6.9540   27   1.00
c 24 9.5680e+05 1.4376e+05      6.6557   27   1.00
c  4 1.8240e+06 5.4712e+05      3.3338   27   1.00
c  6 1.6128e+06 5.7735e+05      2.7934    8   1.00
c ------  -----   --------   --------- ---- ------
c
c         Frac.  Weights =      0.2396
c         Average   Rate =      7.7092 Mega-Flops/Sec.
c         Harmonic  Mean =      7.0169 Mega-Flops/Sec.
c         Standard  Dev. =      1.7944 Mega-Flops/Sec.
c1
c
c
c
c
c
c
c                    SENSITIVITY ANALYSIS
c
c
c         The sensitivity of the harmonic mean rate (Mflops)
c         to various weightings is shown in the table below.
c         Seven work distributions are generated by assigning
c         two distinct weights to ranked kernels by quartiles.
c         Forty nine possible cpu workloads are then evaluated
c         using seven sets of values for the total weights:
c
c
c             ------ ------ ------ ------ ------ ------ ------
c   1st QT:       O      O      O      O      O      X      X
c   2nd QT:       O      O      O      X      X      X      O
c   3rd QT:       O      X      X      X      O      O      O
c   4th QT:       X      X      O      O      O      O      O
c             ------ ------ ------ ------ ------ ------ ------
c   Total
c   Weights                    Net Mflops:
c    X    O
c  ---- ----
c
c  1.00 0.00    6.75   9.43  15.64  24.17  53.09  80.94 170.24
c
c  0.95 0.05    7.03   9.87  15.72  23.17  46.46  58.70 106.07
c
c  0.90 0.10    7.34  10.35  15.80  22.25  41.30  46.04  77.04
c
c  0.80 0.20    8.04  11.46  15.96  20.62  33.79  32.17  49.78
c
c  0.70 0.30    8.89  12.84  16.12  19.21  28.59  24.72  36.77
c
c  0.60 0.40    9.93  14.59  16.29  17.98  24.78  20.08  29.15
c
c  0.50 0.50   11.26  16.90  16.46  16.90  21.87  16.90  24.15
c  ---- ----
c             ------ ------ ------ ------ ------ ------ ------
c
c
c
c
c
c
c SENSITIVITY OF NET MFLOPS RATE TO USE OF OPTIMAL FORTRAN CODE(SISD/SIMD MODEL)
c
c    9.80  12.08  15.74  22.58  28.86   39.96   64.97   94.54  173.54
c
c    0.00   0.20   0.40   0.60   0.70    0.80    0.90    0.95    1.00
c    Fraction Of Operations Run At Optimal Fortran Rates
c
c
c1
c
c
c  Cumulative Checksums:  RUN=    1
c
c  k    VL=  471                      90                      19
c  1  5.1146526932246750e+04  5.2533447789379716e+02  3.8551045024949644e+01
c  2  1.5397218116685108e+03  1.5397218116685108e+03  3.9532969869033877e+01
c  3  1.0007428830665901e+01  1.0097414365791693e+00  2.6993090893212646e-01
c  4  5.9992505954740238e-01  5.9992505954740238e-01  5.9992505954740238e-01
c  5  4.5488716423885198e+03  4.5890319396021141e+01  3.1826152484482577e+00
c  6  4.3751163447426225e+03  8.6316756453456492e+01  1.1203093934675863e+00
c  7  6.1042510751749156e+04  6.3455863157840940e+02  2.8457202176440319e+01
c  8  1.5012680056271516e+05  1.5012680056271516e+05  2.9605436678776314e+03
c  9  1.1894436099750828e+05  1.1894436099750828e+05  2.6239684608743992e+03
c 10  7.3103697843259200e+04  7.3103697843259200e+04  1.6512912276983698e+03
c 11  3.3429109726472735e+07  3.4335604074758245e+04  6.5511613358463364e+02
c 12  2.9071414468173629e-05  7.1275691624350657e-06  1.9434359832537496e-06
c 13  1.2025339591459033e+11  9.8163878117466309e+10  3.8471241978714600e+10
c 14  3.1655530440015411e+09  3.0399834651474237e+07  2.9235405986969322e+06
c 15  3.9438166903522797e+04  3.9438166903522797e+04  1.1089972881350550e+03
c 16  5.6507600000000000e+05  6.4804100000000000e+05  5.1521600000000000e+05
c 17  1.1146417729030727e+03  1.1146417729030727e+03  2.9473686185906899e+01
c 18  1.0157270375021873e+05  1.0157270375021873e+05  9.7006462123414531e+02
c 19  5.4218169601503541e+02  5.4218169601503541e+02  1.2682306980517353e+01
c 20  3.0406443393139362e+07  3.1262051788106910e+04  5.9877132494716716e+02
c 21  1.5973082807004452e+08  7.8245248771329880e+07  5.0099456709430933e+07
c 22  2.9386043765670911e+02  2.9386043765670911e+02  6.1099687282647608e+00
c 23  3.5499005015660077e+04  3.5499005015660077e+04  4.8503406027516576e+02
c 24  5.0000000000000000e+02  5.0000000000000000e+01  1.3000000000000000e+01
c
c
c  Cumulative Checksums:  RUN=    7
c
c  k    VL=  471                      90                      19
c  1  3.5802568852572702e+05  3.6773413452565728e+03  2.6985731517464592e+02
c  2  1.0778052681679546e+04  1.0778052681679546e+04  2.7673078908323623e+02
c  3  7.0052001814661253e+01  7.0681900560541635e+00  1.8895163625248799e+00
c  4  4.1994754168318025e+00  4.1994754168318025e+00  4.1994754168318025e+00
c  5  3.1842101496719639e+04  3.2123223577214776e+02  2.2278306739137747e+01
c  6  3.0625814413198270e+04  6.0421729517419226e+02  7.8421657542730827e+00
c  7  4.2729757526224293e+05  4.4419104210488440e+03  1.9920041523508189e+02
c  8  1.0508876039390042e+06  1.0508876039390042e+06  2.0723805675143376e+04
c  9  8.3261052698255703e+05  8.3261052698255703e+05  1.8367779226120794e+04
c 10  5.1172588490281440e+05  5.1172588490281440e+05  1.1559038593888574e+04
c 11  2.3400376808530903e+08  2.4034922852330748e+05  4.5858129350924282e+03
c 12  2.0349990127721540e-04  4.9892984137045460e-05  1.3604051882776247e-05
c 13  8.4177377140212891e+11  6.8714714682226172e+11  2.6929869385100195e+11
c 14  2.2158871308010742e+10  2.1279884256031895e+08  2.0464784190878510e+07
c 15  2.7606716832465865e+05  2.7606716832465865e+05  7.7629810169453849e+03
c 16  3.9555320000000000e+06  4.5362870000000000e+06  3.6065120000000000e+06
c 17  7.8024924103214871e+03  7.8024924103214871e+03  2.0631580330134784e+02
c 18  7.1100892625153065e+05  7.1100892625153065e+05  6.7904523486389953e+03
c 19  3.7952718721052370e+03  3.7952718721052370e+03  8.8776148863621074e+01
c 20  2.1284510375197506e+08  2.1883436251674779e+05  4.1913992746301519e+03
c 21  1.1181157964903107e+09  5.4771674139930725e+08  3.5069619696601486e+08
c 22  2.0570230635969638e+03  2.0570230635969638e+03  4.2769781097853183e+01
c 23  2.4849303510962054e+05  2.4849303510962054e+05  3.3952384219261585e+03
c 24  3.5000000000000000e+03  3.5000000000000000e+02  9.1000000000000000e+01
c1
c
c
c                          TABLE OF SPEED-UP RATIOS OF MEAN RATES (72 Samples)
c
c                          Arithmetic, Geometric, Harmonic Means (AM,GM,HM)
c                          The Geometric Mean is the least biased statistic.
c
c --------  ----  ------   -------- -------- -------- -------- -------- --------
c SYSTEM    MEAN  MFLOPS   SX-3/14  CRAY-YMP 3090s180 9000/720 6000/540 i486/25
c --------  ----  ------   -------- -------- -------- -------- -------- --------
c
c
c NEC       AM=  311.820 :    1.000    4.426   17.757   22.678   22.006  271.148
c SX-3/14   GM=   95.590 :    1.000    2.816    7.816    8.334    8.909   91.038
c F77v.012  HM=   38.730 :    1.000    2.210    4.294    4.242    5.199   42.098
c           SD=  499.780
c
c
c CRAY-YMP  AM=   70.450 :    0.226    1.000    4.012    5.124    4.972   61.261
c CRAY-YMP  GM=   33.950 :    0.355    1.000    2.776    2.960    3.164   32.333
c CFT77 4.  HM=   17.524 :    0.452    1.000    1.943    1.919    2.352   19.047
c           SD=   79.499
c
c
c IBM       AM=   17.560 :    0.056    0.249    1.000    1.277    1.239   15.270
c 3090s180  GM=   12.230 :    0.128    0.360    1.000    1.066    1.140   11.648
c VSF2.2.0  HM=    9.020 :    0.233    0.515    1.000    0.988    1.211    9.804
c           SD=   16.320
c
c
c HP        AM=   13.750 :    0.044    0.195    0.783    1.000    0.970   11.957
c 9000/720  GM=   11.470 :    0.120    0.338    0.938    1.000    1.069   10.924
c f77 8.05  HM=    9.130 :    0.236    0.521    1.012    1.000    1.226    9.924
c           SD=    7.510
c
c
c IBM       AM=   14.170 :    0.045    0.201    0.807    1.031    1.000   12.322
c 6000/540  GM=   10.730 :    0.112    0.316    0.877    0.935    1.000   10.219
c XL v0.90  HM=    7.450 :    0.192    0.425    0.826    0.816    1.000    8.098
c           SD=    9.590
c
c
c COMPAQ    AM=    1.150 :    0.004    0.016    0.065    0.084    0.081    1.000
c i486/25   GM=    1.050 :    0.011    0.031    0.086    0.092    0.098    1.000
c           HM=    0.920 :    0.024    0.053    0.102    0.101    0.123    1.000
c           SD=    0.480
c1
c
c Version: 22/DEC/86  MF508
c CHECK FOR CLOCK CALIBRATION ONLY:
c Total Job    Cpu Time =     1.57890e+02 Sec.
c Total 24 Kernels Time =     1.21139e+02 Sec.
c Total 24 Kernels Flops=     3.38982e+09 Flops
C                                                    F.H.MCMAHON  1991
C**********************************************************************
c
c
c
c    5. SAMPLE OUTPUT FILE FROM SUBROUTINe REPORT:  SUN SPARC station 1+
c
C
c >>> USE 72 SAMPLES LFK TEST RESULTS SUMMARY (line 330+)
c >>> USE ALL RANGE STATISTICS FOR OFFICIAL QUOTATIONS.
cSECOVT:     16000  0.2500E-04     1.0000
cSECOVT:     32000  0.1969E-04     0.2125
cSECOVT:     64000  0.1344E-04     0.3175
cSECOVT:    128000  0.1570E-04     0.1443
cSECOVT:    256000  0.1527E-04     0.0274
cSECOVT:    512000  0.1420E-04     0.0703
cSECOVT:   1024000  0.1488E-04     0.0459
c
c        VERIFY ADEQUATE Loop SIZE VERSUS CPU CLOCK ACCURACY
c        -----     -------     -------    -------   --------
c        EXTRA     MAXIMUM     DIGITAL    DYNAMIC   RELATIVE
c        Loop      CPUTIME     CLOCK      CLOCK     TIMING
c        SIZE      SECONDS     ERROR      ERROR     ERROR
c        -----     -------     -------    -------   --------
c            1 -0.1527E-04       0.00%    100.00%    115.28%
c            2 -0.1527E-04       0.00%    100.00%    107.64%
c            4 -0.1527E-04       0.00%    100.00%    103.82%
c            8  0.9985E-02       0.00%    201.54%    148.20%
c           16  0.9985E-02       0.00%    201.54%     24.10%
c           32  0.9985E-02       0.00%    201.54%     37.95%
c           64  0.9985E-02       0.00%     50.10%     24.81%
c          128  0.1999E-01       0.00%     33.38%      6.33%
c          256  0.2999E-01       0.00%     18.85%      1.54%
c          512  0.5998E-01       0.00%      7.69%      1.57%
c         1024  0.1100E+00       0.00%      3.92%      0.37%
c         2048  0.2100E+00       0.00%      2.38%      0.62%
c         4096  0.4100E+00       0.00%      0.00%      0.13%
c         8192  0.8200E+00       0.00%      0.49%      0.11%
c        13600              Current Run:   MULTI=  200.000
c        16384  0.1640E+01       0.00%      0.30%      0.11%
c        32768  0.3280E+01       0.00%      0.15%      0.02%
c        -----     -------     -------    -------   --------
c
c Estimated Total Job Cpu-Time=  2593.310 sec.  ( Nruns=       7 Trials)
c
c Trial=      1             ChkSum= 1146    Pass=      0     Fail=      0
c  Time=          452.71sec
c Trial=      2             ChkSum= 1146    Pass=      1     Fail=      0
c  Time=          782.49sec
c Trial=      3             ChkSum= 1146    Pass=      2     Fail=      0
c  Time=         1110.76sec
c Trial=      4             ChkSum= 1146    Pass=      3     Fail=      0
c  Time=         1437.19sec
c Trial=      5             ChkSum= 1146    Pass=      4     Fail=      0
c  Time=         1794.01sec
c Trial=      6             ChkSum= 1146    Pass=      5     Fail=      0
c  Time=         2150.69sec
c Trial=      7             ChkSum= 1146    Pass=      6     Fail=      0
c  Time=         2477.55sec
c1
c
c
c time TEST overhead (t err):
c
c      RUN        AVERAGE        STANDEV        MINIMUM        MAXIMUM
c TICK   1   0.249996E-05   0.451660E-10
c TICK   2   0.246863E-05   0.311374E-07
c TICK   3   0.248439E-05   0.154251E-07
c TICK   4   0.250008E-05   0.839114E-09
c TICK   5   0.250005E-05   0.868416E-09
c TICK   6   0.250005E-05   0.861573E-09
c TICK   7   0.250010E-05   0.818610E-09
c DATA   7   0.999866E-01   0.543320E-06   0.999856E-01   0.999877E-01
c DATA   7   0.999859E-01   0.862081E-06   0.999843E-01   0.999875E-01
c TICK   7   0.249332E-05   0.114356E-07   0.246863E-05   0.250010E-05
c
c
c THE EXPERIMENTAL TIMING ERRORS FOR ALL  7 RUNS
c --  ---------  ---------  --------- -----  -----   ---
c  k   T min      T avg      T max    T err   tick   P-F
c --  ---------  ---------  --------- -----  -----   ---
c  1 0.3606E+01 0.3609E+01 0.3627E+01  0.19%  0.00%     0
c  2 0.2726E+01 0.2735E+01 0.2737E+01  0.13%  0.00%     0
c  3 0.1135E+01 0.1141E+01 0.1146E+01  0.44%  0.00%     0
c  4 0.1063E+01 0.1069E+01 0.1073E+01  0.46%  0.00%     0
c  5 0.3065E+01 0.3066E+01 0.3075E+01  0.11%  0.00%     0
c  6 0.1768E+01 0.1769E+01 0.1769E+01  0.00%  0.00%     0
c  7 0.4608E+01 0.4619E+01 0.4628E+01  0.14%  0.00%     0
c  8 0.7105E+01 0.7109E+01 0.7115E+01  0.07%  0.00%     0
c  9 0.4332E+01 0.4338E+01 0.4342E+01  0.11%  0.00%     0
c 10 0.7303E+01 0.7307E+01 0.7313E+01  0.07%  0.00%     0
c 11 0.2404E+01 0.2409E+01 0.2415E+01  0.20%  0.00%     0
c 12 0.2404E+01 0.2408E+01 0.2414E+01  0.21%  0.00%     0
c 13 0.5062E+01 0.5066E+01 0.5072E+01  0.10%  0.00%     0
c 14 0.6379E+01 0.6382E+01 0.6389E+01  0.07%  0.00%     0
c 15 0.5790E+01 0.5797E+01 0.5800E+01  0.08%  0.00%     0
c 16 0.2017E+01 0.2020E+01 0.2028E+01  0.23%  0.00%     0
c 17 0.2712E+01 0.2718E+01 0.2722E+01  0.18%  0.00%     0
c 18 0.4549E+01 0.4553E+01 0.4559E+01  0.11%  0.00%     0
c 19 0.3850E+01 0.3856E+01 0.3861E+01  0.13%  0.00%     0
c 20 0.3530E+01 0.3535E+01 0.3540E+01  0.14%  0.00%     0
c 21 0.1274E+02 0.1276E+02 0.1281E+02  0.18%  0.00%     0
c 22 0.3884E+01 0.3892E+01 0.3895E+01  0.12%  0.00%     0
c 23 0.3886E+01 0.3889E+01 0.3896E+01  0.12%  0.00%     0
c 24 0.9975E+00 0.1003E+01 0.1008E+01  0.49%  0.00%     0
c --  ---------  ---------  --------- -----  -----   ---
c
c
c NET CPU TIMING VARIANCE (T err);  A few % is ok:
c
c                 AVERAGE        STANDEV        MINIMUM        MAXIMUM
c     Terr          0.17%          0.12%          0.00%          0.49%
c
c
c
c
c
c ********************************************
c THE LIVERMORE  FORTRAN KERNELS:  * SUMMARY *
c ********************************************
c
c              Computer : SUN SPARC 1+
c              System   : UNIX BSD 4
c              Compiler : f77 V1.4
c              Date     : 91.08.14
c         Mean DO Span  =   167
c         When the computer performance range is very large
c         the net Mflops rate of many Fortran programs and
c         workloads will be in the sub-range between the equi-
c         weighted Harmonic and Arithmetic means depending
c         on the degree of code parallelism and optimization.
c         The least biased central measure is the Geometric
c         Mean of 72 rates,  quoted +- a standard deviation.
c         Mean Mflops rates imply the average efficiency of a
c         computing system since the peak rate is well known.
c         LFK test measures a lower bound for a Multi-processor
c         and N * LFK rates project an upper bound for N-procs.
c
c KERNEL  FLOPS   MICROSEC   MFLOP/SEC SPAN WEIGHT  CHECK-SUMS             OK
c ------  -----   --------   --------- ---- ------  ---------------------- --
c  1 6.0480E+06 3.1826E+06      1.9003   27   1.00  2.6985731517464728E+02 16
c  2 3.2384E+06 1.9908E+06      1.6267   15   1.00  2.7673078908321412E+02 16
c  3 3.1968E+06 1.0995E+06      2.9074   27   1.00  1.8895163625244704E+00 16
c  4 1.8240E+06 1.1913E+06      1.5311   27   1.00  4.1994754168317234E+00 16
c  5 3.3280E+06 2.5375E+06      1.3115   27   1.00  2.2278306739132379E+01 16
c  6 1.6128E+06 1.7233E+06      0.9359    8   1.00  7.8421657542696117E+00 15
c  7 1.0752E+07 3.8688E+06      2.7792   21   1.00  1.9920041523508166E+02 16
c  8 1.3478E+07 6.4283E+06      2.0967   14   1.00  2.0723805675125019E+04 16
c  9 1.0608E+07 3.6977E+06      2.8688   15   1.00  1.8367779226119754E+04 16
c 10 5.4000E+06 6.3503E+06      0.8504   15   1.00  1.1559038593887853E+04 16
c 11 1.9136E+06 2.2822E+06      0.8385   27   1.00  4.5858129350920399E+03 16
c 12 1.9968E+06 2.1599E+06      0.9245   26   1.00  1.3604051867913136E-05 16
c 13 2.7776E+06 4.4606E+06      0.6227    8   1.00  8.1274474685488098E+10 15
c 14 3.8016E+06 4.7952E+06      0.7928   27   1.00  1.8264361844941873E+07 16
c 15 3.6960E+06 6.3375E+06      0.5832   15   1.00  7.7629810169434950E+03 16
c 16 2.4640E+06 1.9713E+06      1.2499   15   1.00  1.8033120000000000E+06 16
c 17 5.6160E+06 2.3734E+06      2.3662   15   1.00  2.0631580330125522E+02 16
c 18 9.1520E+06 4.9492E+06      1.8492   14   1.00  6.7904523486359285E+03 16
c 19 4.0320E+06 3.4454E+06      1.1703   15   1.00  8.8776148863570242E+01 15
c 20 7.5712E+06 4.8563E+06      1.5590   26   1.00  4.1913992746327112E+03 16
c 21 4.0000E+07 2.0945E+07      1.9098   20   1.00  1.7539193044904393E+08 16
c 22 3.2640E+06 3.3795E+06      0.9658   15   1.00  4.2769781097847805E+01 16
c 23 8.0080E+06 3.6221E+06      2.2109   14   1.00  3.3952384219249789E+03 16
c 24 9.5680E+05 9.7110E+05      0.9853   27   1.00  9.1000000000000000E+01 16
c  1 8.0800E+06 4.0887E+06      1.9762  101   2.00  3.6773413452565801E+03 16
c  2 6.2080E+06 3.2629E+06      1.9026  101   2.00  1.0778052681678693E+04 16
c  3 4.2824E+06 1.7771E+06      2.4097  101   2.00  7.0681900560526625E+00 16
c  4 3.3600E+06 1.3273E+06      2.5314  101   2.00  4.1994754168317234E+00 16
c  5 4.4000E+06 3.2965E+06      1.3347  101   2.00  3.2123223577206875E+02 16
c  6 2.6880E+06 2.0630E+06      1.3030   32   2.00  6.0421729517332471E+02 16
c  7 1.4221E+07 5.0709E+06      2.8044  101   2.00  4.4419104210488395E+03 16
c  8 1.7107E+07 8.5283E+06      2.0059  100   2.00  1.0508876039380585E+06 16
c  9 1.4423E+07 5.0662E+06      2.8469  101   2.00  8.3261052698248636E+05 15
c 10 6.9084E+06 8.1696E+06      0.8456  101   2.00  5.1172588490277075E+05 16
c 11 2.5600E+06 2.8390E+06      0.9017  101   2.00  2.4034922852330303E+05 16
c 12 2.7200E+06 2.7422E+06      0.9919  100   2.00  4.9892983915750255E-05 16
c 13 3.6736E+06 5.8005E+06      0.6333   32   2.00  2.4799100425819748E+11 16
c 14 4.4440E+06 5.7100E+06      0.7783  101   2.00  2.1111605770897460E+08 15
c 15 6.6000E+06 1.1588E+07      0.5696  101   2.00  2.7606716832464293E+05 16
c 16 3.0240E+06 2.4488E+06      1.2349   40   2.00  2.2682870000000000E+06 16
c 17 7.2720E+06 3.1101E+06      2.3382  101   2.00  7.8024924103174017E+03 16
c 18 8.7120E+06 4.5518E+06      1.9140  100   2.00  5.2551705030583183E+05 16
c 19 5.5752E+06 4.5499E+06      1.2254  101   2.00  3.7952718721030451E+03 16
c 20 8.3200E+06 5.2934E+06      1.5718  100   2.00  2.1883436251708021E+05 16
c 21 2.5000E+07 1.2970E+07      1.9275   50   2.00  2.7413199222149867E+08 16
c 22 4.8076E+06 4.9544E+06      0.9704  101   2.00  2.0570230635966882E+03 16
c 23 1.0890E+07 4.8564E+06      2.2424  100   2.00  2.4849303510945366E+05 16
c 24 1.2400E+06 1.2491E+06      0.9927  101   2.00  3.5000000000000000E+02 16
c  1 7.0070E+06 3.6093E+06      1.9414 1001   1.00  3.5802568852572696E+05 16
c  2 5.1992E+06 2.7352E+06      1.9009  101   1.00  1.0778052681678693E+04 16
c  3 3.6036E+06 1.1412E+06      3.1576 1001   1.00  7.0052001814645450E+01 15
c  4 3.3600E+06 1.0687E+06      3.1439 1001   1.00  4.1994754168317234E+00 16
c  5 4.0000E+06 3.0664E+06      1.3045 1001   1.00  3.1842101496710871E+04 16
c  6 2.3808E+06 1.7685E+06      1.3462   64   1.00  3.0625814413109896E+04 16
c  7 1.2736E+07 4.6194E+06      2.7571  995   1.00  4.2729757526223321E+05 16
c  8 1.4256E+07 7.1093E+06      2.0053  100   1.00  1.0508876039380585E+06 16
c  9 1.2362E+07 4.3377E+06      2.8500  101   1.00  8.3261052698248636E+05 15
c 10 6.1812E+06 7.3073E+06      0.8459  101   1.00  5.1172588490277075E+05 16
c 11 2.2000E+06 2.4088E+06      0.9133 1001   1.00  2.3400376808550769E+08 16
c 12 2.4000E+06 2.4083E+06      0.9966 1000   1.00  2.0349989059170737E-04 16
c 13 3.2256E+06 5.0663E+06      0.6367   64   1.00  3.4706712065081335E+11 16
c 14 4.4044E+06 6.3818E+06      0.6901 1001   1.00  2.2156947925784698E+10 16
c 15 3.3000E+06 5.7966E+06      0.5693  101   1.00  2.7606716832464293E+05 16
c 16 2.6500E+06 2.0204E+06      1.3116   75   1.00  1.9780320000000000E+06 16
c 17 6.3630E+06 2.7183E+06      2.3408  101   1.00  7.8024924103174017E+03 16
c 18 8.7120E+06 4.5532E+06      1.9134  100   1.00  5.2551705030583183E+05 16
c 19 4.7268E+06 3.8562E+06      1.2258  101   1.00  3.7952718721030451E+03 16
c 20 5.2000E+06 3.5352E+06      1.4709 1000   1.00  2.1284510375458673E+08 16
c 21 2.5250E+07 1.2755E+07      1.9796  101   1.00  5.6017393194626498E+08 16
c 22 3.7774E+06 3.8916E+06      0.9706  101   1.00  2.0570230635966882E+03 16
c 23 8.7120E+06 3.8889E+06      2.2402  100   1.00  2.4849303510945366E+05 16
c 24 1.0000E+06 1.0032E+06      0.9968 1001   1.00  3.5000000000000000E+03 16
c ------  -----   --------   --------- ---- ------  ---------------------- --
c 72 0.4843E+09 0.3110E+09      1.5572  167                              1145
c
c         MFLOPS    RANGE:             REPORT ALL RANGE STATISTICS:
c         Maximum   Rate =      3.1576 Mega-Flops/Sec.
c         Quartile  Q3   =      2.0059 Mega-Flops/Sec.
c         Average   Rate =      1.5922 Mega-Flops/Sec.
c         Geometric Mean =      1.4275 Mega-Flops/Sec.
c         Median    Q2   =      1.3462 Mega-Flops/Sec.
c         Harmonic  Mean =      1.2712 Mega-Flops/Sec.
c         Quartile  Q1   =      0.9681 Mega-Flops/Sec.
c         Minimum   Rate =      0.5693 Mega-Flops/Sec.
c
c
c         Standard  Dev. =      0.7177 Mega-Flops/Sec.
c         Avg Efficiency =     45.21%  Program & Processor
c         Mean Precision =     15.90   Decimal Digits
c <<<<<<<<<<<<<<<<<<<<<<<<<<<*>>>>>>>>>>>>>>>>>>>>>>>>>>>
c < BOTTOM-LINE:   72 SAMPLES LFK TEST RESULTS SUMMARY. >
c < USE RANGE STATISTICS ABOVE FOR OFFICIAL QUOTATIONS. >
c <<<<<<<<<<<<<<<<<<<<<<<<<<<*>>>>>>>>>>>>>>>>>>>>>>>>>>>
c1
c1
c
c
c
c
c
c
c                    SENSITIVITY ANALYSIS
c
c
c         The sensitivity of the harmonic mean rate (Mflops)
c         to various weightings is shown in the table below.
c         Seven work distributions are generated by assigning
c         two distinct weights to ranked kernels by quartiles.
c         Forty nine possible cpu workloads are then evaluated
c         using seven sets of values for the total weights:
c
c
c             ------ ------ ------ ------ ------ ------ ------
c   1st QT:       O      O      O      O      O      X      X
c   2nd QT:       O      O      O      X      X      X      O
c   3rd QT:       O      X      X      X      O      O      O
c   4th QT:       X      X      O      O      O      O      O
c             ------ ------ ------ ------ ------ ------ ------
c   Total
c   Weights                    Net Mflops:
c    X    O
c  ---- ----
c
c  1.00 0.00    0.75   0.90   1.14   1.40   1.81   2.12   2.56
c
c  0.95 0.05    0.77   0.93   1.15   1.39   1.76   1.99   2.40
c
c  0.90 0.10    0.79   0.96   1.16   1.37   1.71   1.87   2.26
c
c  0.80 0.20    0.84   1.02   1.17   1.34   1.62   1.67   2.01
c
c  0.70 0.30    0.89   1.09   1.19   1.32   1.54   1.51   1.82
c
c  0.60 0.40    0.96   1.17   1.21   1.29   1.47   1.38   1.66
c
c  0.50 0.50    1.03   1.27   1.22   1.27   1.41   1.27   1.52
c  ---- ----
c             ------ ------ ------ ------ ------ ------ ------
c
c
c
c
c
c
c SENSITIVITY OF NET MFLOPS RATE TO USE OF OPTIMAL FORTRAN CODE(SISD/SIMD MODEL)
c
c    0.91   1.04   1.22   1.48   1.65    1.87    2.16    2.34    2.55
c
c    0.00   0.20   0.40   0.60   0.70    0.80    0.90    0.95    1.00
c    Fraction Of Operations Run At Optimal Fortran Rates
c
c
c1
c
c
c                          TABLE OF SPEED-UP RATIOS OF MEAN RATES (72 Samples)
c
c                          Arithmetic, Geometric, Harmonic Means (AM,GM,HM)
c                          The Geometric Mean is the least biased statistic.
c
c --------  ----  ------   -------- -------- -------- -------- -------- --------
c SYSTEM    MEAN  MFLOPS   SX-3/14  YMP/1    9000/720 6000/540 CRAY-YMP i486/25
c --------  ----  ------   -------- -------- -------- -------- -------- --------
c
c
c NEC       AM=  311.820 :    1.000    3.986   22.678   22.006  195.844  271.148
c SX-3/14   GM=   95.590 :    1.000    2.610    8.334    8.909   66.965   91.038
c F77v.012  HM=   38.730 :    1.000    2.193    4.242    5.199   30.466   42.098
c           SD=  499.780
c
c
c CRAY      AM=   78.230 :    0.251    1.000    5.689    5.521   49.134   68.026
c YMP/1     GM=   36.630 :    0.383    1.000    3.194    3.414   25.661   34.886
c CFT771.2  HM=   17.660 :    0.456    1.000    1.934    2.370   13.892   19.196
c           SD=   86.750
c
c
c HP        AM=   13.750 :    0.044    0.176    1.000    0.970    8.636   11.957
c 9000/720  GM=   11.470 :    0.120    0.313    1.000    1.069    8.035   10.924
c f77 8.05  HM=    9.130 :    0.236    0.517    1.000    1.226    7.182    9.924
c           SD=    7.510
c
c
c IBM       AM=   14.170 :    0.045    0.181    1.031    1.000    8.900   12.322
c 6000/540  GM=   10.730 :    0.112    0.293    0.935    1.000    7.517   10.219
c XL v0.90  HM=    7.450 :    0.192    0.422    0.816    1.000    5.860    8.098
c           SD=    9.590
c
c
c SUN       AM=    1.592 :    0.005    0.020    0.116    0.112    1.000    1.385
c SPARC 1+  GM=    1.427 :    0.015    0.039    0.124    0.133    1.000    1.359
c f77 V1.4  HM=    1.271 :    0.033    0.072    0.139    0.171    1.000    1.382
c           SD=    0.718
c
c
c COMPAQ    AM=    1.150 :    0.004    0.015    0.084    0.081    0.722    1.000
c i486/25   GM=    1.050 :    0.011    0.029    0.092    0.098    0.736    1.000
c           HM=    0.920 :    0.024    0.052    0.101    0.123    0.724    1.000
c           SD=    0.480
c
c
c Version: 22/DEC/86  MF508
c CHECK FOR CLOCK CALIBRATION ONLY:
c Total Job    Cpu Time =     2.47860E+03 Sec.
c Total 24 Kernels Time =     2.17686E+03 Sec.
c Total 24 Kernels Flops=     3.38982E+09 Flops
C
C                                                    F.H.MCMAHON  1991
C**********************************************************************
c
C
      IMPLICIT  DOUBLE PRECISION (A-H,O-Z)
cIBM  IMPLICIT  REAL*8           (A-H,O-Z)
Cout  DOUBLE  PRECISION  sum                                            REDUNDNT
C
C/      PARAMETER( kn= 47, kn2= 95, np= 3, ls= 3*47, krs= 24)
C/      PARAMETER( nk= 47, nl= 3, nr= 8 )
      parameter(  nt= 4 )
C
      CHARACTER  NAME*8
      CHARACTER  Komput*24, Kontrl*24, Kompil*24, Kalend*24, Identy*24
C
      COMMON /SYSID/ Komput, Kontrl, Kompil, Kalend, Identy
      COMMON /ALPHA/ mk,ik,im,ml,il,Mruns,Nruns,jr,iovec,NPFS(8,3,47)
      COMMON /BETA / tic, TIMES(8,3,47), SEE(5,3,8,3),
     1              TERRS(8,3,47), CSUMS(8,3,47),
     2              FOPN(8,3,47), DOS(8,3,47)
C
      COMMON /SPACE0/ TIME(47), CSUM(47), WW(47), WT(47), ticks,
     1                FR(9), TERR1(47), SUMW(7), START,
     2              SKALE(47), BIAS(47), WS(95), TOTAL(47), FLOPN(47),
     3                IQ(7), NPF, NPFS1(47)
C
      COMMON /SPACEI/ WTP(3), MUL(3), ISPAN(47,3), IPASS(47,3)
C
      DIMENSION  NAME(nt), RATE(nt)
      DIMENSION  FLOPS(141), TR(141), RATES(141)
      DIMENSION  LSPAN(141), WG(141), OSUM (141), ID(141)
      DIMENSION  HM(12), LVL(10)
      DIMENSION  LQ(5), STAT1(20), STAT2(20)
      DIMENSION  IN(141), CSUM1(141), TV4(141), TV5(141)
      DIMENSION  MAP1(141), MAP2(141), MAP3(141), IN2(141), VL1(141)
      DIMENSION  MAP(141), VL(141), TV(141), TV1(141), TV2(141)
      DIMENSION  FLOPS1(141), RT1(141), ISPAN1(141), WT1(141)
      DIMENSION  FLOPS2(141), RT2(141), ISPAN2(141), WT2(141)
      SAVE    kall,  LVL
C
       MODI(i,mm)= (MOD( ABS(i)-1, mm) + 1)
C
      DATA  kall/0/
C
      CALL TRACE ('REPORT  ')
C
          IF( iou.LT.0) GO TO 73
C
            meff= 0
            neff= 0
            fuzz= 1.0d-9
       DO 1000 k= 1,ntk
           VL(k)= LSPAN(k)
 1000  CONTINUE
C
              bl= 1.0d-5
              bu= 1.0d+5
            CALL  VALID( TV,MAP,neff,  bl, RATES, bu, ntk)
C
C      Compress valid data sets mapping on MAP.
C
              nd= 0
        DO  1  k= 1,neff
         MAP1(k)=  MODI( MAP(k),nek)
       FLOPS1(k)= FLOPS( MAP(k))
          RT1(k)=    TR( MAP(k))
          VL1(k)=    VL( MAP(k))
       ISPAN1(k)= LSPAN( MAP(k))
          WT1(k)=    WG( MAP(k))
          TV1(k)= RATES( MAP(k))
        CSUM1(k)=  OSUM( MAP(k))
              nd=    ID( MAP(k)) + nd
    1  continue
              IF( nd .LE. 8*neff )  nd= nd - 16*((neff-1+24)/24)
          precis= REAL(nd)/( REAL(neff) + fuzz)
C
             som= 0.00d0
             sum= 0.00d0
        DO 11  k= 1,neff
             som= som + FLOPS1(k)
             sum= sum + RT1(k)
   11  continue
           rneto= som/(sum + fuzz)
C
            CALL  STATW( STAT1,TV,IN, VL1,WT1,neff)
              lv= STAT1(1)
C
            CALL  STATW( STAT1,TV,IN, TV1,WT1,neff)
             twt= STAT1(6)
C                             compute average efficiency= GM/Max
            kall= kall +  1
            peak= 0.00d0
              if( kall.LE.1 .OR. il.EQ.im ) then
                  peak= STAT1(4)
              endif
          avgeff= (100.0d0* STAT1(10))/( peak + fuzz)
C
          WRITE ( iou,7001)
          WRITE ( iou,7001)
          WRITE ( iou,7001)
          WRITE ( iou,7001)
          WRITE ( iou,7001)
          WRITE ( iou,7001)
       CALL PAGE( iou)
          WRITE ( iou,7002)
C
      IF( ntk .EQ. nek )  THEN
          WRITE ( iou,7003)
      ELSE
          WRITE ( iou,7090)
      ENDIF
C
          WRITE ( iou,7002)
          WRITE ( iou,7007)  Komput
          WRITE ( iou,7057)  Kontrl
          WRITE ( iou,7008)  Kompil
          WRITE ( iou,7038)  Kalend
          WRITE ( iou,7039)  Identy
          WRITE ( iou,7061)
          WRITE ( iou,7062)
          WRITE ( iou,7063)
          WRITE ( iou,7064)
          WRITE ( iou,7065)
          WRITE ( iou,7066)
          WRITE ( iou,7067)
          WRITE ( iou,7071)
          WRITE ( iou,7072)
          WRITE ( iou,7068)
          WRITE ( iou,7069)
c         WRITE ( iou,7001)
          WRITE ( iou,7004)
          WRITE ( iou,7005)
          WRITE ( iou,7011) (MAP1(k),  FLOPS1(k), RT1(k), TV1(k),
     .                    ISPAN1(k), WT1(k), CSUM1(k), ID(k), k=1,neff)
          WRITE ( iou,7005)
C
          WRITE ( iou,7023)  neff, som, sum, rneto, lv, nd
          WRITE ( iou,7022)
          WRITE ( iou,7009)  lv
          WRITE ( iou,7010)  ntk
          WRITE ( iou,7041)  STAT1( 4)
          WRITE ( iou,7037)  STAT1(14)
          WRITE ( iou,7033)  STAT1( 1)
          WRITE ( iou,7043)  STAT1(10)
          WRITE ( iou,7030)  STAT1( 7)
          WRITE ( iou,7055)  STAT1( 5)
          WRITE ( iou,7036)  STAT1(13)
          WRITE ( iou,7042)  STAT1( 3)
          WRITE ( iou,7001)
          WRITE ( iou,7044)  STAT1( 2)
          WRITE ( iou,7091)  avgeff
          WRITE ( iou,7034)  precis
C
      IF( ntk .NE. nek )  THEN
          WRITE (   *,7001)
          WRITE (   *,7002)
          WRITE (   *,7090)
          WRITE (   *,7002)
          WRITE (   *,7007)  Komput
          WRITE (   *,7057)  Kontrl
          WRITE (   *,7008)  Kompil
          WRITE (   *,7038)  Kalend
          WRITE (   *,7039)  Identy
          WRITE (   *,7022)
          WRITE (   *,7009)  lv
          WRITE (   *,7010)  ntk
          WRITE (   *,7041)  STAT1( 4)
          WRITE (   *,7037)  STAT1(14)
          WRITE (   *,7033)  STAT1( 1)
          WRITE (   *,7043)  STAT1(10)
          WRITE (   *,7030)  STAT1( 7)
          WRITE (   *,7055)  STAT1( 5)
          WRITE (   *,7036)  STAT1(13)
          WRITE (   *,7042)  STAT1( 3)
          WRITE (   *,7001)
          WRITE (   *,7044)  STAT1( 2)
          WRITE (   *,7091)  avgeff
          WRITE (   *,7034)  precis
      ENDIF
C
C         WRITE ( iou,7031)  STAT1( 9)
C         WRITE ( iou,7032)  STAT1(15)
C
 7001 FORMAT(/)
 7002 FORMAT(  45H ******************************************** )
 7003 FORMAT(  45H THE LIVERMORE  FORTRAN KERNELS:  M F L O P S  )
 7090 FORMAT(  45H THE LIVERMORE  FORTRAN KERNELS:  * SUMMARY *  )
 7004 FORMAT(/,53H KERNEL  FLOPS   MICROSEC   MFLOP/SEC SPAN WEIGHT  CH,
     X23HECK-SUMS             OK )
 7005 FORMAT(  53H ------  -----   --------   --------- ---- ------  --,
     X23H-------------------- -- )
 7007 FORMAT(/,9X,16H     Computer :  ,A )                               f77
 7057 FORMAT(  9X,16H     System   :  ,A )                               f77
 7008 FORMAT(  9X,16H     Compiler :  ,A )                               f77
 7038 FORMAT(  9X,16H     Date     :  ,A )                               f77
 7039 FORMAT(  9X,16H     Testor   :  ,A )                               f77
c7009 FORMAT(/,9X,16H     Computer :  ,A8)                               f66
c7057 FORMAT(  9X,16H     System   :  ,A8)                               f66
c7008 FORMAT(  9X,16H     Compiler :  ,A8)                               f66
c7038 FORMAT(  9X,16H     Date     :  ,A8)                               f66
 7009 FORMAT(  9X,16HMean DO Span   =  ,I5)
 7010 FORMAT(  9X,16HCode Samples   =  ,I5)
 7011 FORMAT(1X,i2,1PE11.4,E11.4,0PF12.4,1X,I4,1X,F6.2,1PE24.16,1X,I2)
C7011 FORMAT(1X,i2,E11.4,E11.4,F12.4,1X,I4,1X,F6.2,E35.25,1X,I2)
 7012 FORMAT(1X,i2,E11.4,E11.4,F12.4,1X,I4,1X,F6.2)
 7023 FORMAT(1X,i2,E11.4,E11.4,F12.4,1X,I4,30X,I4)
c7022 FORMAT(/,15H MFLOPS  RANGE:,23X,28HREPORT ALL RANGE STATISTICS: )  f66
 7022 FORMAT(/,9X,'MFLOPS    RANGE:',13X,'REPORT ALL RANGE STATISTICS:') f77
 7041 FORMAT(/,9X,16HMaximum   Rate =  ,F12.4,16H Mega-Flops/Sec. )
 7037 FORMAT(  9X,16HQuartile  Q3   =  ,F12.4,16H Mega-Flops/Sec. )
 7033 FORMAT(  9X,16HAverage   Rate =  ,F12.4,16H Mega-Flops/Sec. )
 7043 FORMAT(  9X,16HGeometric Mean =  ,F12.4,16H Mega-Flops/Sec. )
 7030 FORMAT(  9X,16HMedian    Q2   =  ,F12.4,16H Mega-Flops/Sec. )
 7055 FORMAT(  9X,16HHarmonic  Mean =  ,F12.4,16H Mega-Flops/Sec. )
 7036 FORMAT(  9X,16HQuartile  Q1   =  ,F12.4,16H Mega-Flops/Sec. )
 7042 FORMAT(  9X,16HMinimum   Rate =  ,F12.4,16H Mega-Flops/Sec. )
 7044 FORMAT(  9X,16HStandard  Dev. =  ,F12.4,16H Mega-Flops/Sec. )
c7031 FORMAT(  9X,16HMedian    Dev. =  ,F12.4,16H Mega-Flops/Sec. )
c7032 FORMAT(  9X,16HGeom.Mean Dev. =  ,F12.4,16H Mega-Flops/Sec. )
 7091 FORMAT(  9X,16HAvg Efficiency =  ,F10.2,22H%  Program & Processor)
 7034 FORMAT(  9X,16HMean Precision =  ,F10.2,17H   Decimal Digits )
 7053 FORMAT(/,9X,16HFrac.  Weights =  ,F12.4)
 7104 FORMAT(/,50H KERNEL  FLOPS   MICROSEC   MFLOP/SEC SPAN WEIGHT   )
 7105 FORMAT(  50H ------  -----   --------   --------- ---- ------   )
C
 7061 FORMAT(/,9X,50HWhen the computer performance range is very large )
 7062 FORMAT(9X,52Hthe net Mflops rate of many Fortran programs and    )
 7063 FORMAT(9X,52Hworkloads will be in the sub-range between the equi-)
 7064 FORMAT(9X,52Hweighted Harmonic and Arithmetic means depending    )
 7065 FORMAT(9X,52Hon the degree of code parallelism and optimization. )
c7066 FORMAT(9X,52HMore accurate estimates of cpu workload rates depend)
c7067 FORMAT(9X,52Hon assigning appropriate weights for each kernel.   )
c7066 FORMAT(9X,52HThe best central measure is the Geometric Mean of 72)
c7067 FORMAT(9X,52Hrates which must be quoted +- a standard deviation. )
 7066 FORMAT(9X,'The least biased central measure is the Geometric ')
 7067 FORMAT(9X,'Mean of 72 rates,  quoted +- a standard deviation.')
 7068 FORMAT(9X,'LFK test measures a lower bound for a Multi-processor')
 7069 FORMAT(9X,'and N * LFK rates project an upper bound for N-procs.')
 7071 FORMAT(9X,'Mean Mflops rates imply the average efficiency of a')
 7072 FORMAT(9X,'computing system since the peak rate is well known.')
C
      NAME(1)= Komput
      NAME(2)= Komput
      NAME(3)= Kompil
      RATE(1)= STAT1(1)
      RATE(2)= STAT1(10)
      RATE(3)= STAT1(5)
      RATE(4)= STAT1(2)
C
      IF( ntk .NE. nek )  THEN
      WRITE( iou,7099)
      WRITE( iou,7097)
      WRITE( iou,7098)
      WRITE( iou,7099)
 7097 FORMAT(' < BOTTOM-LINE:   72 SAMPLES LFK TEST RESULTS SUMMARY. >')
 7098 FORMAT(' < USE RANGE STATISTICS ABOVE FOR OFFICIAL QUOTATIONS. >')
 7099 FORMAT(' <<<<<<<<<<<<<<<<<<<<<<<<<<<*>>>>>>>>>>>>>>>>>>>>>>>>>>>')
       CALL PAGE( iou)
C
       IF( iovec.EQ.1 ) THEN
          WRITE ( iou,7070)
 7070 FORMAT(//,50H TOP QUARTILE: BEST ARCHITECTURE/APPLICATION MATCH )
C
C      Compute compression index-list MAP1:  Non-zero weights.
C
              bl= 1.0d-6
              bu= 1.0d+6
            CALL  VALID( TV,MAP1,meff,  bl, WT1, bu, neff)
C
C      Re-order data sets mapping on IN (descending order of MFlops).
C
        DO  2  k= 1,meff
         MAP3(k)=     IN( MAP1(k))
    2  continue
C
          IF( meff.GT.0 )  THEN
              CALL TRAP( MAP3, 8H REPORT  , 1, neff,meff)
          ENDIF
C
        DO  3  k= 1,meff
               i=   MAP3(k)
        FLOPS2(k)=  FLOPS1(i)
          RT2(k)=    RT1(i)
       ISPAN2(k)= ISPAN1(i)
          WT2(k)=    WT1(i)
          TV2(k)=    TV1(i)
         MAP2(k)=   MODI( MAP(i),nek)
    3  continue
C                             Sort kernels by performance into quartiles
              nq= meff/4
              lo= meff -4*nq
           LQ(1)= nq
           LQ(2)= nq + nq + lo
           LQ(3)= nq
              i2= 0
C
         DO 5  j= 1,3
              i1= i2 + 1
              i2= i2 + LQ(j)
              ll= i2 - i1 + 1
            CALL  STATW( STAT2,TV,IN2, TV2(i1),WT2(i1),ll)
            frac= STAT2(6)/( twt +fuzz)
C
          WRITE ( iou,7001)
          WRITE ( iou,7104)
          WRITE ( iou,7105)
          WRITE ( iou,7012) ( MAP2(k),  FLOPS2(k), RT2(k), TV2(k),
     .                         ISPAN2(k), WT2(k),  k=i1,i2 )
          WRITE ( iou,7105)
C
          WRITE ( iou,7053)  frac
          WRITE ( iou,7033)  STAT2(1)
          WRITE ( iou,7055)  STAT2(5)
          WRITE ( iou,7044)  STAT2(2)
    5 continue
C
       ENDIF
C
      ENDIF
C
C           Sensitivity analysis of harmonic mean rate to 49 workloads
C
      CALL  SENSIT(   iou,RATES,WG,IQ,SUMW, MAP,TV,TV4,TV2,TV5, ntk)
C
C
C           Sensitivity analysis of harmonic mean rate to SISD/SIMD model
C
      CALL  SIMD( HM, iou,RATES,WG,FR,9,    MAP,TV,TV4,TV2, ntk)
C
C
      IF( ntk .NE. nek )  THEN
        IF( iovec.EQ.1 )  THEN
               CALL  PAGE( iou)
               mrl= Nruns
                IF( Nruns.gt.8) mrl= 8
C
      DO  8      k= 1,mk
      DO  8      j= im,ml
               sum= 0.0d0
      DO  8      i= 1,mrl
               sum= sum + CSUMS(i,j,k)
      CSUMS(i,j,k)= sum
    8 continue
C
      DO  10     i= 1,mrl
                IF( (i.NE.1).AND.(i.NE.mrl))  GO TO 10
             WRITE( iou,76) i
             WRITE( iou,77)  ( LVL(j), j= 1,3 )
   76       FORMAT( //,29h  Cumulative Checksums:  RUN=,i5)
   77       FORMAT( /,10h  k    VL=,i5,3i24)
C
      DO  9      k= 1,mk
             WRITE( iou,78)  k, ( CSUMS(i,j,k), j= 1,3)
   78       FORMAT( 1X,I2,4E24.16)
    9 continue
   10 continue
        ENDIF
C
      CALL SPEDUP( iou, NAME, RATE )
      ENDIF
          LVL(il)= lv
   73 CONTINUE
      CALL TRACK ('REPORT  ')
      RETURN
C
      END
