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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: sigops.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 02:08:17 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTIONS: sigaddset, sigdelset, sigfillset, sigismember,
 *	      sigemptyset (POSIX)
 *
 * ORIGINS: 27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * sigops.c	1.10  com/lib/c/gen,3.1,9013 10/23/89 13:38:39
 */

#include <signal.h>
#include <errno.h>

#ifdef  _THREAD_SAFE
#define SETERR(err)     seterrno(err)
#else
#define SETERR(err)     errno = err
#endif

/*
 * NAME: sigemptyset		(POSIX)
 *
 * FUNCTION: clears all bits in signal set, i.e. all signals excluded
 *
 * NOTES: This subroutine does not affect a processes signal mask,
 *	  it just initializes the specified mask data structure for
 *	  use in subsequent calls such as sigprocmask() that WOULD
 *	  affect a processes signal mask.
 *
 * RETURN VALUES: 
 * 	always return successfully with a value of 0
 *	upon return the specifies signal set is cleared
 *
 */
int
sigemptyset(sigset_t *setp)
{
	*setp = 0;
	return (0);
}

/*
 * NAME: sigfillset		(POSIX)
 *
 * FUNCTION: sets all bits in signal set, i.e. ALL signals blocked
 *
 * NOTES: This subroutine does not affect a processes signal mask,
 *	  it just initializes the specified mask data structure for
 *	  use in subsequent calls such as sigprocmask() that WOULD
 *	  affect a processes signal mask.
 *
 * RETURN VALUES: 
 * 	always return successfully with a value of 0
 *	upon return all signals in the specifies signal set are set
 */
/* setp is the pointer to signal set structure to be set */
int sigfillset(sigset_t *setp)
{
	*setp = ~0;
	return (0);
}
/*
 * NAME: sigaddset		(POSIX)
 *
 * FUNCTION: adds a signal to the specified set of signals to block
 *
 * NOTES: This subroutine does not affect a processes signal mask,
 *	  it just initializes the specified mask data structure for
 *	  use in subsequent calls such as sigprocmask() that WOULD
 *	  affect a processes signal mask.
 *
 * RETURN VALUES: 
 * 	0 => successfully altered signal set
 *	-1 => failed, did not alter signal set 
 *	errno = EINVAL if a -1 is returned => invalid signal number
 *	errno is unchanged if a 0 is returned
 */
/* setp is the pointer to signal set that will be changed */
/* signo is the signal to be added to "*setp" signal set */
  
int sigaddset(sigset_t *setp, int signo)
{

	/* check for invalid signal numbers - POSIX requires this */
	if ((signo <= 0) || (signo > SIGMAX))
	{	SETERR(EINVAL);
		return (-1);
	}
	/* set appropriate mask bit for valid signal number */
	SIGADDSET(*setp,signo);
	return (0);
}
/*
 * NAME: sigdelset		(POSIX)
 *
 * FUNCTION: deletes a signal from the specified set of signals to block
 *
 * NOTES: This subroutine does not affect a processes signal mask,
 *	  it just initializes the specified mask data structure for
 *	  use in subsequent calls such as sigprocmask() that WOULD
 *	  affect a processes signal mask.
 *
 * RETURN VALUES: 
 * 	0 => successfully deleted signal from the signal set
 *	-1 => failed, did not alter signal set 
 *	errno = EINVAL if a -1 is returned => invalid signal number
 *	errno is unchanged if a 0 is returned
 */
int sigdelset(sigset_t *setp, int signo)
{
	/* check for invalid signal numbers - POSIX requires this */
	if ((signo <= 0) || (signo > SIGMAX))
	{	SETERR(EINVAL);
		return (-1);
	}
	/* clear appropriate mask bit for valid signal number */ 
	SIGDELSET(*setp,signo);
	return (0);
}
/*
 * NAME: sigismember		(POSIX)
 *
 * FUNCTION: tests to see if a signal is in the specified set of signals
 *
 * NOTES: This subroutine does not affect a processes signal mask,
 *	  it just initializes the specified mask data structure for
 *	  use in subsequent calls such as sigprocmask() that WOULD
 *	  affect a processes signal mask.
 *
 * RETURN VALUES: 
 * 	0 => signal is NOT in the signal set
 *	1 => signal IS in the signal set
 *     -1 => failed, invalid signal number could not be tested
 *	errno = EINVAL if a -1 is returned => invalid signal number
 *	errno is unchanged if a 0 is returned
 */ 
int sigismember(sigset_t *setp, int signo)
{
	/* check for invalid signal number */
	if ( (signo <= 0) || (signo > SIGMAX) )
	{	SETERR(EINVAL);
		return (-1);
	}
	/* test bit corresponding to signal number; 1 => set */
	else
		return (SIGISMEMBER(*setp,signo));
}
