*******************************************************************************
*  This routine measures time required on a revised LINPACK-type benchmark,   *
*  including input, matrix generation, solution, and output.                  *
*                                                                             *
*  John Gustafson, Diane Rover, and Steve Elbert; Ames Laboratory, 2/27/90.   *
*                                                                             *
*  Calls: Reader  Reads the problem description from secondary storage.       *
*         Region  Partitions box surface into rectangular regions (patches).  *
*         SetUp1  Sets up equations from patch geometries-parallel faces.     *
*         SetUp2  Sets up equations from patch geometries-orthogonal faces.   *
*         SetUp3  Sets up equations-row normalization and radiant properties. *
*         Solver  Solves the equations by LDL factorization.                  *
*         Writer  Stores solution (patch radiosities) on secondary storage.   *
*         When    Returns wall-clock time, in seconds.                        *
*******************************************************************************
*
      SUBROUTINE Meter (npatch, timing, work, ok, pxarea,coeff,scratch,
     &	pxdiag, pxplace, pyplace, ptemp, pxrhs, pxsize, pysize, px, py,
     &	pxans)
      include 'sizes'
*
*  Passed variables:
*    npatch  In, problem size, here the number of equations (1 per patch).
*    timing  Out, number of seconds measured.
*    work    Out, work done, here the number of floating point operations.
*    ok      Out; .TRUE. if no errors occur, else .FALSE.
*    area    Vector, areas of patches * 8 * pi.
*    coeff   Matrix, the coefficients of the equations to solve.
*    diag    Vectors, diagonal terms of the equations to solve, (R-G-B).
*    place   Vectors, width-height-depth position of patches.
*    rhs     Vectors, right-hand sides of equations to solve (R-G-B).
*    size    Vectors, width-height sizes of patches.
*
      INTEGER npatch
      REAL*8 timing, work
      LOGICAL ok
      REAL*8 pxarea(nxmax), coeff( *), scratch(*)
      REAL*8 pxdiag(nxmax, 3), ptemp(nymax)
      REAL*8 pxplace(nxmax, 3), pyplace(nymax, 3), pxrhs(nxmax, 3)
      real*8 pxsize(nxmax, 2), pysize(nymax, 2)
      real*8 px(nxmax), py(nymax + 1), pxans(nxmax, 3)
*
*  Local variables:
*    When    Function to return wall clock time in seconds.
*    box     Vector, dimensions of box in x, y, z directions.
*    emiss   Vector, emissivities of patches in R-G-B components.
*    ops     Vector, floating-point operation counts for routines.
*    p       Vector, number of patches in faces (for operation count).
*    ans  Vectors, answer radiosities (R-G-B)
*    rho     Vector, reflectivities of patches in R-G-B components.
*    sec     Vector, times for routines, in seconds.
*    tmp1-2  REAL*8 temporary variables.
*    i       General loop counter.
*    itmp1   Integer temporary variable.
*    loop    Vectors, patch number ranges for faces.
*    non0    Index of the first nonzero off-diagonal matrix element.
*    tasks   Vector, list of names of routines measured.
*
      REAL*8 When, box(7)
      REAL*8 emiss(6, 3), ops(8)
      REAL*8 rho(6, 3)
      REAL*8 sec(8), tmp1, tmp2
      INTEGER i, itmp1, loop(6, 2), non0, iofile
      CHARACTER*6 tasks(7)
      DATA iofile /9/
      include 'isc.h'
      include 'fnx.h'
      integer dsize, ipatch, jpatch, icount, mess_size, j, fsize
      integer nxproc, nyproc, nxtop, nytop, nx, ny
      integer ixproc, iyproc, netw, nete, netn, nets, info(16),
     &	idim, jdim
      data dsize /8/
      DATA tasks/'Reader', 'Region', 'SetUp1', 'SetUp2',
     &           'SetUp3', 'Solver', 'Storer'/
*
*  First check that npatch lies between 6 and nmax:
*
c     if(me .eq. 0) write(6,*) 'meter begun'
      nxproc = node_dim
      nyproc = node_dim
      num_node = nxproc * nyproc
      nxtop = nxproc - 1
      nytop = nyproc - 1
      ixproc = mod(me,nxproc)
      iyproc = me / nxproc
      nx = (npatch - ixproc + nxtop) / nxproc
      jdim = nx + 1
      ny = (npatch - iyproc + nytop) / nyproc
      idim = (npatch - iyproc + nyproc) / nyproc
      IF (npatch .LT. 6) THEN
 	if(me.eq.0) WRITE (*, *) ' Must be at least 6, the number of faces.'
        ok = .FALSE.
        RETURN
      ELSE IF (nx .GT. nxmax .or. ny .gt. nymax) THEN

	print *,'nx= ',nx,'nxmax=',nxmax,'ny=',ny,'nymax=',nymax
	print *,'nyproc=',nyproc,'nytop=',nytop,'iyproc=',iyproc

 	if(me.eq.0) WRITE (*, *) ' Exceeds', nxmax*nxproc, ' = maximum 
     &		for this system. nx = ',nx
        ok = .FALSE.
        RETURN
      END IF
*
*  Ensure that previous 'answer' files are deleted:
*
      if(me.eq.0) then
 	OPEN (iofile, FILE = 'answer', STATUS = 'UNKNOWN')
 	CLOSE (iofile, STATUS = 'DELETE')
      endif
*
*  Time the tasks, individually and collectively (see header descriptions).
*
      sec(1) = When ()
      CALL Reader (box, rho, emiss, ok)
      IF (.NOT. ok) RETURN
      itmp1 = (me .and. .not. nxtop)
      netw = me +1
      if(mod(me+1,nxproc) .eq. 0) netw = netw - nxproc
      nete = me -1
      if(nete .lt. (me/nxproc) *nxproc) nete = nete + nxproc
      itmp1 = me .and. nxtop
      netn = me - nxproc
      if(netn .lt. 0) netn = netn + num_node
      nets = me + nxproc
      if(nets .ge. num_node) nets = nets - num_node
      info(1) = me
      info(3) = num_node
      info(4) = nxproc
      info(5) = nyproc
      info(6) = ixproc
      info(7) = iyproc
      info(8) = nete
      info(9) = netn
      info(10) = nets
      info(11) = netw
      info(12) = idim
      info(13) = jdim
c     info(14) = idim * jdim
      info(14) = (idim+1) * jdim
c     write(6,*) 'info1',info(1),(info(i),i=3,6)
c     write(6,*) 'info2',info(1),(info(i),i=7,11)
      DO 11 i = 1, (idim+1) * (jdim+1)
  	coeff(i) = 0.
 	scratch(i) = 0.
 11   CONTINUE
c
c......Find transpose neighbor
c
      i = mod(me,nyproc)
      j = iyproc + (ixproc / nyproc) * nyproc
      info(15) = j + nxproc * i
      info(16) = npatch
      sec(2) = When ()
      CALL Region (box, pxarea, pxplace, pxsize, pyplace, pysize,
     &		   info, loop, npatch, nx, ny, ok)
      IF (.NOT. ok) STOP
      sec(3) = When ()
      CALL SetUp1 (box, coeff, px, pxplace, pxsize, py, pyplace,
     &		   pysize, info, loop, nx, ny)
      if(me.eq.0) write(6,*) 'set1 done'
      sec(4) = When ()
      CALL SetUp2 (coeff, px, pxplace, pxsize, py, pyplace,
     &		   pysize, info, loop, nx, ny)
      if(me.eq.0) write(6,*) 'set2 done'
      sec(5) = When ()
      CALL SetUp3 (coeff, emiss, px, pxarea, pxdiag, pxrhs, py,
     &		   rho, info, loop, nx, ny, ok)
      IF (.NOT. ok) STOP
      if(me.eq.0) write(6,*) 'set3 done'
      IF (.NOT. ok) RETURN
      sec(6) = When ()
      non0 = loop(2, 1)
      CALL Solver (coeff, scratch, ptemp, px, pxans, pxdiag, pxrhs, py,
     &		 info, non0, npatch, nx, ny)
      if(mynode .eq. 0) write(6,*) 'solver done'
      sec(7) = When ()
      CALL Storer (pxans, pxplace, pxsize, info, loop, npatch, nx)
      sec(8) = When ()
      timing = sec(8) - sec(1)
      DO 12 i = 1, 7
        sec(i) = sec(i + 1) - sec(i)
 12   CONTINUE
      CALL Verify (coeff, ptemp, px, pxans, pxdiag, pxrhs, py,
     &		   info, npatch, nx, ny)
*
*  Assess floating-point work done by each routine called, and total:
*
      itmp1 = 0
      tmp1 = 0.0D0
      DO 14 i = 1, 6
        px(i) = loop(i, 2) - loop(i, 1) + 1
        tmp1 = tmp1 + px(i) ** 2
        itmp1 = itmp1 + INT(SQRT(px(i) * box(i) / box(i + 1)) + .5D0)
 14   CONTINUE
      tmp2 = px(1) * px(4) + px(2) * px(5) + px(3) * px(6)
      ops(1) = 258.
      ops(2) = 154. + 8. * itmp1 + npatch
      ops(3) = 6. + 532. * tmp2
      ops(4) = 8. * npatch + 370 * (npatch ** 2 - tmp1 - 2 * tmp2) / 2
      ops(5) = 72. + 9. * npatch + npatch ** 2 - tmp1
      ops(6) = npatch * (npatch * (npatch + 7.5) - 2.5) - 21.
     &       + non0 * (non0 * (2. * non0 - 16.5) + 35.5)
     &       + non0 * npatch * (9. - 3. * non0)
      ops(7) = 48. * npatch
      work = ops(1) + ops(2) + ops(3) + ops(4) + ops(5) + ops(6)
     &		+ ops(7)
*
*  Display timing-work-speed breakdown by routine.
*
      if(me.eq.0) then
	WRITE (*, *) npatch, ' patches:', timing
	WRITE (*, *) 
     &   '  Task  Seconds      Operations      MFLOPS    % of Time'
	DO 15 i = 1, 7
	  IF (sec(i) .EQ. 0.) sec(i) = .001
 	  IF (timing .EQ. 0) timing = .001
	  WRITE (*, 16) tasks(i), sec(i), ops(i),
     &         ops(i) / sec(i) /  1.D6, 100. * sec(i) / timing
 15     CONTINUE
	WRITE (*, 16) ' TOTALS: ', timing, work,
     &		 work / timing / 1.D6, 100.
 16     FORMAT (' ', A6, F8.3, F15.0, F14.6, F9.1, ' %')
      endif
      END
