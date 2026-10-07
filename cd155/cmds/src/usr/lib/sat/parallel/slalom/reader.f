*******************************************************************************
*  The following routine reads in the problem description from secondary      *
*  storage, and checks that numbers are in reasonable ranges.                 *
*                                                                             *
*  John Gustafson, Diane Rover, and Steve Elbert; Ames Laboratory, 2/27/90.   *
*******************************************************************************
*
      SUBROUTINE Reader (box, rho, emiss, ok)
*
*  Passed variables:
*    box     Out vector, dimensions of box in x, y, and z directions, repeated
*              to eliminate need for MOD function elsewhere in program.
*    rho     Out vector, reflectivities of interiors of faces, in R-G-B.
*    emiss   Out vector, emissivities of interiors of faces, in R-G-B.
*    ok      Out, .TRUE. if no errors occur, else .FALSE.
*
      REAL*8 box(7), rho(6, 3), emiss(6, 3)
      real*8 buf(39)
      LOGICAL ok
*
*  Local variables:
*    infile  Device number for input file.
*    i, j    Loop counters.
*    tmp1    Maximum emissivity, to check that emissivities are not all zero.
*
      INTEGER infile, i, j
      include 'isc.h'
      include 'fnx.h'
      DATA infile /9/
      REAL*8 tmp1
*
      if (mynode() .eq. 0) then
        OPEN (infile, FILE = 'geom', STATUS = 'OLD', ERR = 103)
        READ (infile, *, ERR = 103) (box(i),   i = 1, 3)
        READ (infile, *, ERR = 103) ((rho(i, j),   i = 1, 6), j = 1, 3)
        READ (infile, *, ERR = 103) ((emiss(i, j), i = 1, 6), j = 1, 3)
        CLOSE (infile)
 	call dcopy(3, box, 1, buf(1), 1)
 	call dcopy(18, rho, 1, buf(4), 1)
 	call dcopy(18, emiss, 1, buf(22),1)
 	do 10 i = 1, num_node-1
 	  call csend(5, buf, 312, i, 0)
 10	continue
      else
 	call crecv(5, buf, 312)
 	call dcopy(3, buf(1), 1, box, 1)
 	call dcopy(18, buf(4), 1, rho, 1)
 	call dcopy(18, buf(22), 1, emiss, 1)
      endif
*
      DO 102 j = 1, 3
        IF (box(j) .LT. 1. .OR. box(j) .GE. 100.) THEN
          if(me.eq.0) WRITE (*, *) ' Box dimensions must be between 1 
     &		and 100.'
          ok = .FALSE.
          RETURN
        END IF
        box(j + 3) = box(j)
*
        tmp1 = 0.
        DO 101 i = 1, 6
          IF (rho(i, j) .LT. 0.001D0 .OR. rho(i, j) .GT. 0.999D0) THEN
            if(me.eq.0) WRITE (*, *) ' Reflectivities must be between 
     &		.001 and .999'
            ok = .FALSE.
            RETURN
          END IF
          IF (emiss(i, j) .LT. 0.) THEN
            if(me.eq.0) WRITE (*, *) ' Emissivity cannot be negative.'
            ok = .FALSE.
            RETURN
          END IF
          IF (tmp1 .LT. emiss(i, j)) tmp1 = emiss(i, j)
 101    CONTINUE
        IF (tmp1 .EQ. 0.) THEN
          if(me.eq.0) WRITE (*, *) ' Emissivities are zero. 
     &		Problem is trivial.'
          ok = .FALSE.
          RETURN
        END IF
 102  CONTINUE
      box(7) = box(4)
*
      ok = .TRUE.
      RETURN
*
*  Trap if 'geom' file is missing or otherwise isn't read correctly:
*
 103  WRITE (*, *) ' Problem reading ''geom'' file.'
      ok = .FALSE.
      END
