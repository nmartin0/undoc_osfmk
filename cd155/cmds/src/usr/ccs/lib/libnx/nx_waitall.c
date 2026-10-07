/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 * HISTORY:
 * $Log: nx_waitall.c,v $
 * Revision 1.11  1994/11/19  02:32:01  mtm
 * Copyright additions/changes
 *
 * Revision 1.10  1994/07/21  13:49:05  johannes
 * In case of a bad signal with core dumping parallel termination is now done
 * in the server. This allows to continue non-faulting processes.
 *
 *  Reviewer: Stefan Tritscher
 *  Risk: L
 *  Benefit or PTS #: OS support for parallel core files
 *  Testing: special test cases, particulary with CORE_ACTION_OTHER=TRUE
 *  Module(s): libnx/nx_waitall.c
 *
 * Revision 1.9  1994/06/15  15:32:28  cfj
 * Use waitmulti() instead of waitpid().
 *
 *  Reviewer:shala
 *  Risk:M
 *  Benefit or PTS #:rforkmulti/waitmulti integration
 *  Testing:The following EATs:
 * 	 message
 * 	 controlc
 * 	 misc
 * 	 rmcall
 * 	 rmcmd
 *  Module(s):libnx/nx_waitall() libnx/IPSC860/rforkmulti.s
 *
 * Revision 1.8  1993/08/26  18:15:50  carbajal
 * Fixed PTS #6043.
 *
 * Revision 1.7  1993/08/05  19:24:15  stans
 *    use geterrno() instead of get_errno() which is really a macro, oops.
 *
 * Revision 1.6  1993/08/05  16:38:18  stans
 *    created a thread-safe version utilizing get_errno() function to
 *    retrieve errno values as they are thread specific values in a
 *    {p/c}threads pgm.
 *
 *
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#define	pid_t int
#define boolean_t int
#define FALSE 0
#define TRUE 1

/*
 * We have now parallel core dump support in the server
 * which handles parallel termination for bad signals.
 */
#define PARACORE

/* Forward reference */
int was_bad_signal(int status);

#ifdef	_THREAD_SAFE
/*
 * The thread safe version of nx_waitall() is used (found in libc_r.a) when
 * pthreads and libnx.a are linked together (a multi-threaded parallel program).
 * geterrno() retrieves an errno value from a thread specific structure.
 */
#define	errno geterrno()
#endif	/* _THREAD_SAFE */

/***************************** nx_waitall ***********************
 *
 *      Calling Sequence:
 *             nx_waitall(); 
 *
 *      Description:
 *		nx_waitall() waits for all the child processes.
 *
 *      Parameters:
 *              NONE
 *
 *      Returns:
 *              status
 *
 *
 */

int
nx_waitall() 
{
	int		status;
	int		*status_array;
	pid_t		*pid_array;
	long		waits = 0;
	pid_t		pgroup;
	boolean_t	use_waitmulti = FALSE;
	u_int		num_children;
	int		retval;

	/*
	 * First attempt to use waitmulti() to return the number of
	 * children. If this succeeds, allocate heap space for status
	 * and pid arrays. If all that succeeds, we're set to use
	 * waitmulti() rather then waitpid() in the following loop.
	 */
	num_children = waitmulti(&waits, NULL, NULL, 0);
	if (num_children > 0) {
		status_array = (int *)   malloc(num_children * sizeof(int));
		pid_array    = (pid_t *) malloc(num_children * sizeof(pid_t));
		if (status_array != NULL && pid_array != NULL)
			use_waitmulti = TRUE;
	}
	
	pgroup = -getpgrp();
	waits = 0;
	/* wait for all children to die */
	if (use_waitmulti) {
		/*
		 * Wait for all children to exit, but don't reap
		 * any until all have exited. This is achieved using the
		 * WNOWAIT option. This ploy is to suppress multiple
		 * SIGCHLDs and perform the reap in one swell foop by
		 * spanning tree.
		 */
#ifdef PARACORE
		long		last_waits = 0;
#else /* PARACORE */
		boolean_t	aborted = FALSE;
#endif /* PARACORE */

		waits = num_children;
		while ((waitmulti(&waits, pid_array,
				  status_array, WNOWAIT)) > 0) {
			int	i;
#ifdef PARACORE
			/*
			 * Check only newly terminated children for bad status,
			 * reporting is done in was_bad_signal().
			 */
			if (waits > last_waits)
				for (i = last_waits; i < waits; i++) {
					if (WIFSIGNALED(status_array[i]) &&
					    was_bad_signal(status_array[i])) {
						/*
						 * now handled by parallel
						 * termination in the Server
						 */
						;
					}
				}
			last_waits = waits;
#else /* PARACORE */
			/*
			 * If we haven't seen a bad status returned from a
			 * child, check for any and kill the entire pgrp to
			 * abort if necessary.
			 */
			if (!aborted)
				for (i=0; i < waits; i++) {
					if (WIFSIGNALED(status_array[i]) &&
					    was_bad_signal(status_array[i])) {
						/* bad stuff happened, kill
						* everyone
						*/
						kill(-pgroup,SIGKILL);
						aborted = TRUE;
						break;
					}
				}
#endif /* PARACORE */
			if (waits == num_children) {
				/*
				 * All children have exited - now reap them.
				 */
				(void) waitmulti(&waits, pid_array,
						 status_array, 0);
				retval = 0;
				goto out;
			} else if (waits < (num_children/2)) {
				/*
				 * Still most children to exit: sleep to
				 * give them time.
				 */
				sleep(1);
			}

			/*
			 * Try again.
			 */
			waits = num_children;
		}
	}
	for(;;){
					
		/* Exit out of this while loop only if wait returns -1
		 * If this happens we need to check errno.
		 */
		while( waitpid(pgroup,&status,0) != -1){	
			/* keep track of how many times we have issued
			 * wait
			*/
			 waits++;
			/* See why we exited */
			if ( WIFSIGNALED(status)){
				/* child died of an uncaught signal */
				if (was_bad_signal(status))
#ifdef PARACORE
					/*
					 * now handled by parallel
					 * termination in the Server
					 */
					;
#else /* PARACORE */
					/* bad stuff happened, kill
					* everyone
					*/
					kill(-pgroup,SIGKILL);
#endif /* PARACORE */
			}
		}

		if ((errno != EINTR) && (waits == 0)) {
			/* this is the first issue of wait, just return
			 * a bad status back to the user. This 
			 * preserves the unix wait semantics if nx_waitall
			 * is called when there are no children present
			 */
			retval = -1;
			goto out;
		}

		if (errno == ECHILD ) {
			/* No more children we have accomplished our task */
			retval = 0;
			goto out;
		}
		if (errno != EINTR)
			/* Some other error happened, return with bad 
			 * status
			*/
			break;
		/* The wait was interrupted so reissue it */
	}
	retval = -1;

out:
	if (use_waitmulti) {
		free(status_array);
		free(pid_array);
	}
	return retval;
}
	
/* Test the value of status to see if the child exited due to receiving
 * a "bad" signal
 *
 *	Parameters:
 *		status	integer returned from wait()
 *
 *	Returns:
 *		0 == no bad signal was received, child died by other means
 *		1 == bad signal was received, perror will be called
 *
*/
int
was_bad_signal(int status)
{
	int	signal_val;
	int	was_bad;
	char	*s;

	was_bad = 0;

	signal_val = WTERMSIG(status);

	switch (signal_val){
	case SIGSEGV:
		s = "Segmentation Violation";
		was_bad = 1;
		break;
	case SIGBUS:
		s = "Bus Error";
		was_bad = 1;
		break;
	case SIGILL:
		s = "Illegal Instruction";
		was_bad = 1;
		break;
	case SIGFPE:
		s = "Floating point exception";
		was_bad = 1;
		break;
	case SIGSYS:
		s = "Bad argument to system call";
		was_bad = 1;
		break;
	default:
		break;
	}

	if (was_bad){
		write(2,s,strlen(s));
		write(2,"\n",1);
	}
	return(was_bad);
}

 	
