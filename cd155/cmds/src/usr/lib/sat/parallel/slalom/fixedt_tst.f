
*                               S L A L O M
*
*    Scalable Language-independent Ames Laboratory One-minute Measurement
*
*     The following program is the first benchmark based on fixed time rather
*  than fixed problem comparison.  Not only is fixed time more representative
*  of the way people use computers, it also greatly increases the scope and
*  longevity of the benchmark.  SLALOM is very scalable, and can be used to
*  compare computers as slow as 148 floating-point operations per second
*  (FLOPS) to computers running at over 10**15 FLOPS.  The scalability can
*  be used to compare single processors to massively-parallel collections
*  of processors, and to study the space of problem size vs. ensemble size
*  in fine detail.  It resembles the LINPACK benchmark since it involves
*  factoring and backsolving a (nearly) dense 64-bit matrix, but incorporates
*  a number of improvements to that benchmark that we hope will make SLALOM
*  a better reflection of general system performance.
*
*     The SLALOM benchmark solves a complete, real problem (optical radiosity
*  on the interior of a box), not a contrived kernel or a synthetic mixture of
*  sample operations.  SLALOM is unusual since it times input, problem setup,
*  solution, and output, not just the solution.  For slower computers, the
*  problem setup will take the majority of the time; it grows as the square of
*  the problem size.  The solver grows as the cube of the problem size, and
*  dominates the time for large values of n.
*
*     While the following is Fortran 77, you are free to translate it into any
*  language you like, including assembly language specific to one computer.
*  You may use compiler directives, hand-tuned library calls, loop unrolling,
*  and even change the algorithm, if you can provide a convincing argument
*  that the program still works for the full range of possible inputs.  For
*  example, if you replace the direct solver with an iterative one, you must
*  make sure your method is correct even when the geometry is quite eccentric
*  and the box faces are highly reflective.
*
*     The "FixedT" driver should be used with the value of 60 seconds for the
*  SLALOM benchmark.  The work done for a particular problem size is figured
*  after timing has ceased, so there is no overhead for work assessment.  Two
*  computers may be compared either by their problem size n, or by their MFLOPS
*  rate, never by the ratio of execution times.  Times will always be near one
*  minute in SLALOM.  We have used the following weights for floating-point
*  operation counting, based on the weights used by Lawrence Livermore National
*  Laboratory:
*
*                        OPERATION                       WEIGHT
*                    a=b, a=(constant)                      0
*       a.LT.0, a.LE.0, a.EQ.0, a.NE.0, a.GT.0, a.GE.0      0
*                 -a, ABS(a), SIGN(a, b)                    0
*                   a+b, a-b, a*b, a**2                     1
*       a.LT.b, a.LE.b, a.EQ.b, a.NE.b, a.GT.b, a.GE.b      1
*           INT(a), FLOAT(i) (even when implicit)           1
*                         NINT(a)                           2
*                        1/a, -1/a                          3
*                           a/b                             4
*                          SQRT(a)                          4
*               Format to or from ASCII string              6
*       SIN(a), COS(a), TAN(a), LOG(a), ATN(a), EXP(a)      8
*
*     We invite you to share with us the results of any measurements that you
*  make with SLALOM.  We do NOT accept anonymous data; machine timings will be
*  referenced and dated.  For any one machine, we are interested in knowing
*  two extremes: performance with as little alteration as possible from the
*  following specification or similar high-level language, and the performance
*  with every tuning you can think of.  The former rewards clever optimizing
*  compilers, while the latter allows advanced architectures to show what they
*  can achieve.  We call the ratio of the two performances the "ease of use
*  index" since it represents the gap between automatically- and manually-tuned
*  performance.
*
*     The least you need to do to adapt SLALOM to your computer is:
*
*        1.  In the "Measure" routine, set nmax to a value large enough to keep
*            the computer working for a minute.  Vary it slightly if it helps
*            (for reasons of cache size, interleaving, etc.)
*
*        2.  Replace the timer call in "When" with the most accurate wall-clock
*            timer at your disposal.  If only CPU time is available, try to run
*            the job standalone or at high priority, since we are ultimately
*            interested in the top of the statistical range of performance.
*
*        3.  Edit in the information specific to your test in the "What"
*            routine, so that final output will be automatically annotated.
*
*        4.  Compile, link, and run the program, interacting to select values
*            of n that bracket a time of one minute.  Once everything is
*            running, run it as a batch job so as to record the session.
*
*     Examples of ways you may optimize performance:
*
*        1.  Unroll the loops in SetUp1 and SetUp2; it is possible to
*            vectorize both SetUp1 and SetUp2 at the cost of some extra
*            operations, program complexity, and storage.
*
*        2.  Replace the innermost loops of Solver with calls to well-tuned
*            libraries of linear algebra routines, such as DDOT from the
*            Basic Linear Algebra Subroutines (level 1 BLAS).  Better still,
*            use a tuned library routine for all of Solver; the sparsity
*            exploited in Solver is only a few percent, so you will usually
*            gain more than you lose by applying a dense symmetric solver.
*
*        3.  Parallelize the SetUp and Solver routines; all are highly
*            parallel.  Each element of the matrix can be constructed
*            independently, once each processor knows the geometry and part of
*            the partitioning into regions.  A substantial body of literature
*            now exists for performing the types of operations in Solver in
*            parallel.
*
*        4.  Overlap computation with output.  Once the Region routine is done,
*            the first part of the output file (patch geometry) can be written
*            while the radiosities are being calculated.
*
*     Examples of what you may NOT do:
*
*        1.  The tuning must not be made specific to the particular input
*            provided.  For example, you may not eliminate IF tests simply
*            because they always come out the same way for this input; you
*            may not use precomputed answers or table look-up unless those
*            answers and tables cover the full range of possible inputs; and
*            you may not exploit symmetry for even values of the problem size.
*
*        2.  You may not disable the test in SetUp3 that ensures that the
*            row sums are close to unity, nor alter its tolerance constant.
*
*        3.  You may not change the input or output files to unformatted
*            binary or other format that would render them difficult to create
*            or read for humans.
*
*        4.  You may not eliminate the reading of the "geom" file by putting
*            its data directly into the compiled program.
*
*        5.  You may not change any of the work assessments in Meter.  If you
*            use more floating-point operations than indicated, you must still
*            use the assessments provided.  If you find a way to use fewer
*            operations and still get the job done for arbitrary input
*            parameters, please tell us!
*
*                          -John Gustafson, Diane Rover, and Steve Elbert
*                           Ames Laboratory, Ames, Iowa 50011
*
*******************************************************************************
*  The following program finds a value n such that a problem of size n        *
*  takes just under "goal" seconds to execute.                                *
*                                                                             *
*  John Gustafson, Diane Rover, and Steve Elbert; Ames Laboratory, 5/17/90.   *
*                                                                             *
*  Calls:  Meter   Measures execution time for some application.              *
*          What    Prints work-timing statistics and system information.      *
*******************************************************************************
*
      PROGRAM FixedT
*
*  Parameters:
*    nmax    Parameter, largest npatch for your computer; adjust as needed.
*
      include 'sizes'
      PARAMETER (nmax_coef = 510000, nmax_coef1 = 300000)
*
*  Variables used:
*    goal    User input, fixed-time benchmark goal, in seconds.
*    timing  Elapsed time returned by Meter routine, in seconds.
*    work    In this case, number of floating-point operations performed.
*    mean    Average between upper and lower bounds for bisection method.
*    n       The problem size.  Time and work should increase with n.
*    nupper  Upper bound on problem size, used in iterating toward goal.
*    ok      Flag indicating computational success of Meter.
*
      REAL*8 pxarea(nxmax), coeff(nmax_coef), scratch(nmax_coef1)
      REAL*8 pxdiag(nxmax, 3)
      REAL*8 pxplace(nxmax, 3), pyplace(nymax, 3), pxrhs(nxmax, 3)
      REAL*8 pxsize(nxmax, 2), pysize(nymax, 2)
      real*8 px(nxmax), py(nymax + 1), pxans(nxmax, 3)
      real*8 ans(nmax, 3), place(nmax, 3), size(nmax, 2)
      REAL*8 goal, timing, work
      INTEGER mean, n, nupper
      LOGICAL ok
      include 'isc.h'
      include 'fcube.h'
      data pid /1/
      me = mynode()
      num_node = numnodes()
      node_dim = nodedim()
*
*  Get desired number of seconds:
*
      messtype = 1
      if(me.eq.0) then
        WRITE (*, *) ' Enter the number of seconds that is the goal:'
        READ (*, *) goal
 	call csend(messtype, goal, 8, -1, pid)
      else
 	call crecv(messtype, goal, 8)
      endif
*
*  Get lower and upper bounds for n from the standard input device:
*
      open_flg = 0
 1    continue
      messtype = 2
      if(me.eq.0) then
        WRITE (*, *) ' Enter a lower bound for n:'
        READ (*, *, END = 4) n
 	call csend(messtype, n, 4, -1, pid)
      else
 	call crecv(messtype, n, 4)
      endif
      if(me .eq. 0) write(6,*) 'meter1'
      CALL Meter (n, timing, work, ok, pxarea, coeff, scratch, pxdiag,
     &	pxplace, pyplace, place, pxrhs, pxsize, pysize, px, py, pxans,
     &	size, ans)
      IF (.NOT. ok) GO TO 1
      IF (timing .GE. goal) THEN
        if(me.eq.0) WRITE(*, *) ' Must take less than', goal, 
     &		' sec. Took', timing
        GO TO 1
      END IF
*
 2    continue
      messtype = 3
      if(me.eq.0) then
        WRITE (*, *) ' Enter an upper bound for n:'
        READ (*, *, END = 4) nupper
 	call csend(messtype, nupper, 4, -1, pid)
      else
 	call crecv(messtype, nupper, 4)
      endif
      if(me .eq. 0) write(6,*) 'meter2'
      CALL Meter (nupper, timing, work, ok, pxarea, coeff, scratch,
     &	pxdiag,
     &	pxplace, pyplace, place, pxrhs, pxsize, pysize, px, py, pxans,
     &	size, ans)
      IF (.NOT. ok) GO TO 2
      IF (timing .LT. goal) THEN
 	if(me.eq.0) WRITE (*, *) ' Must take at least', goal, 
     &		' sec. Took', timing
        n = nupper
        GO TO 2
      END IF
*
*  While the [n, nupper] interval is larger than 1, bisect it and pick a half:
*
 3    IF (nupper - n .GT. 1) THEN
        mean = (n + nupper) / 2
      if(me .eq. 0) write(6,*) 'meter3'
        CALL Meter (mean, timing, work, ok, pxarea, coeff, scratch,
     &	 pxdiag,
     &	 pxplace, pyplace, place, pxrhs, pxsize, pysize, px, py, pxans,
     &	 size, ans)
        IF (timing .LT. goal) THEN
          n = mean
        ELSE
          nupper = mean
        END IF
 	if(me.eq.0) WRITE (*, *) ' New interval: [',n, ',', nupper, ']'
        GO TO 3
      END IF
*
*  Ensure that most recent run was for n, not nupper, and print statistics:
*
      if(me .eq. 0) write(6,*) 'meter4'
      CALL Meter (n, timing, work, ok, pxarea, coeff, scratch, pxdiag,
     &	pxplace, pyplace, place, pxrhs, pxsize, pysize, px, py, pxans,
     &	size, ans)
      if(me.eq.0) CALL What (n, timing, work)
 4    END
*
*******************************************************************************
*  This routine should be edited to contain information for your system.      *
*******************************************************************************
*
      SUBROUTINE What (n, timing, work)
      INTEGER n
      REAL*8 timing, work
*
      CHARACTER*64 info(18)
      DATA info /
     &   ' Machine:    Intel iPSC/860',
     &   ' Processor:  i860',
     &   ' Memory:     8 MB',
     &   ' # of procs: 64',
     &   ' Cache:      8 KB data, 4KB ins.',
     &   ' # used:     64',
     &   ' nmax:       600 * sqrt(# of procs)',
     &   ' Clock:      40 MHz',
     &   ' Disk:       CIOF',
     &   ' Node name:  Pacific Rim',
     &   ' OS:         NX/2 3.3',
     &   ' Timer:      Wall, dclock',
     &   ' Alone:      yes',
     &   ' Language:   Fortran 77',
     &   ' Compiler:   if77 2.0',
     &   ' Options:    -O3 -Knoieee',
     &   ' Run by:     E. Kushner',
     &   ' Date:       15 May 1991' /
*
      WRITE (*, *) ' '
      WRITE (*, '(A64)') info
      WRITE (*, *) ' M ops:  ',work * 1.D-6
      WRITE (*, *) ' Time:   ',timing, ' seconds'
      WRITE (*, *) ' Patches: ', n
      WRITE (*, *) 'MFLOPS: ', (work / timing) * 1.D-6
      END
