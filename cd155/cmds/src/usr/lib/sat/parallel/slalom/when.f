*
*******************************************************************************
*  This function should return the actual, wall clock time (not CPU time),    *
*  as accurately as possible.  Change it to your system timer.                *
*******************************************************************************
*
      FUNCTION When ()
      REAL*8 When
	real*8 dclock, t_local, t_work
*
      t_local = dclock ()
      call gdhigh(t_local, 1, t_work)
c     after gdhigh there will be a sync and a uniform view of time
      When = t_local
      END
