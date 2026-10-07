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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.1
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: sighold.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:08:13 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTIONS: sighold, sigrelse, sigset, sigignore
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
 * sigblock.c	1.8  com/lib/c/gen,3.1,8943 10/16/89 14:05:23
 */

#include <signal.h>
#include <errno.h>


/*
XXX * NAME: sigblock()		(BSD/AIX)  
XXX *
XXX * FUNCTION: add a set of signals to the process's currently blocked signals
XXX *
XXX * NOTES: sigblock() allows only a subset of the function of sigprocmask().
XXX *	  It is provided for compatibility with old source code.  
XXX *	  Only signals of value 1-31 can be added to the already blocked
XXX *	  signals using sigblock(); sigprocmask() must be used if signal
XXX *	  values 32-63 need to be added to the set of blocked signals.
XXX *        The sigblock() subroutine uses the sigprocmask() system call.
XXX *
XXX * WARNING: This source code is dependent on the type of sigset_t
XXX *	since it does not use the sig...set functions to form the
XXX *	sigprocmask signal sets.
XXX *
XXX * RETURN VALUES: 
XXX *	-1 => failed, errno is set to specify the cause of failure
XXX *      errno = EPERM => calling process tryed to block SIGSAK w/o privilege
XXX *
XXX *	If a -1 is not returned, then sigblock() was successful, and the
XXX *	return value is a mask indicating which of the signals 1-31 were
XXX *	previously blocked.
XXX
XXXint sigblock(mask)
XXXint	mask;	/X mask whose bits correspond to signals 1-31 X/
XXX{
XXX	int	rc;		/X save return value from sigprocmask() X/
XXX
XXX	sigset_t nset;		/X new signal set for sigprocmask() X/
XXX	sigset_t oset;		/X structure to return old signal set X/
XXX
XXX	nset.losigs = (ulong)mask;
XXX	nset.hisigs = 0;
XXX	rc = sigprocmask(SIG_BLOCK, &nset, &oset);
XXX	if ( rc == 0 )
XXX		rc = oset.losigs;
XXX	return (rc);
XXX}
*/ 

/*
XXX * NAME: sigsetmask()		(BSD/AIX)  
XXX *
XXX * FUNCTION: set the process's currently blocked signals
XXX *
XXX * NOTES: sigsetmask() allows only a subset of the function of sigprocmask().
XXX *	  It is provided for compatibility with old source code.  
XXX *	  Only signals of value 1-31 can be blocked using sigsetmask();
XXX *	  sigprocmask() must be used to block signal values 32-63.
XXX *        The sigsetmask() subroutine uses the sigprocmask() system call.
XXX *
XXX * WARNING: This source code is dependent on the type of sigset_t
XXX *	since it does not use the sig...set functions to form the
XXX *	sigprocmask signal sets.
XXX *
XXX * RETURN VALUES: 
XXX *	-1 => failed, errno is set to specify the cause of failure
XXX *      errno = EPERM => calling process tryed to block SIGSAK w/o privilege
XXX *
XXX *	If a -1 is not returned, then sigsetmask() was successful, and the
XXX *	return value is a mask indicating which of the signals 1-31 were
XXX *	previously blocked.  
XXXint sigsetmask(mask)
XXXint	mask;	/X mask whose bits correspond to signals 1-31 X/
XXX{
XXX	int	rc;		/X save return value from sigprocmask() X/
XXX
XXX	sigset_t nset;		/X new signal set for sigprocmask() X/
XXX	sigset_t oset;		/X structure to return old signal set X/
XXX
XXX	nset.losigs = (ulong)mask;
XXX	nset.hisigs = 0;
XXX	rc = sigprocmask(SIG_SETMASK, &nset, &oset);
XXX	if ( rc == 0 )
XXX		rc = oset.losigs;
XXX	return (rc);
XXX}
 */ 

/*
 * NAME: sighold()		(ATT/AIX)  
 *
 * FUNCTION: add a signal to the process's currently blocked signals
 *
 * NOTES: sighold() allows only a subset of the function of sigprocmask().
 *	  It is provided for compatibility with old source code.  
 *	  Only signals of value 1-31 can be added to the already blocked
 *	  signals using sigblock(); sigprocmask() must be used if signal
 *	  values 32-63 need to be added to the set of blocked signals.
 *        The sigblock() subroutine uses the sigprocmask() system call.
 *
 * RETURN VALUES: 
 *	-1 => failed, errno is set to specify the cause of failure
 *
 *	If a -1 is not returned, then sigblock() was successful, and the
 *	return value is a mask indicating which of the signals 1-31 were
 *	previously blocked.
 */ 
int sighold(sig)
int	sig;	/* sig whose bit correspond to signals 1-31 */
{
	int	rc;		/* save return value from sigprocmask() */

	sigset_t nset;		/* new signal set for sigprocmask() */
	sigset_t oset;		/* structure to return old signal set */

	sigemptyset(&nset);
	if (sigaddset(&nset, sig) == -1)
		return(-1);
	if (sig == SIGKILL || sig == SIGSTOP) {
		errno = EINVAL;
		return(-1);
	}
	rc = sigprocmask(SIG_BLOCK, &nset, &oset);
	if ( rc == 0 )
		rc = (int)oset;
	return (rc);
}
/*
 * NAME: sigrelse()		(ATT/AIX)  
 *
 * FUNCTION: remove a signal to the process's currently blocked signals
 *
 * NOTES: sigrelse() allows only a subset of the function of sigprocmask().
 *	  It is provided for compatibility with old source code.  
 *
 * RETURN VALUES: 
 *	-1 => failed, errno is set to specify the cause of failure
 *
 *	 0 => succeeded
 */ 
int sigrelse(sig)
int	sig;	/* sig whose bit correspond to signals 1-31 */
{

	sigset_t nset;		/* new signal set for sigprocmask() */
	sigset_t oset;		/* structure to return old signal set */

	sigemptyset(&nset);
	if (sigaddset(&nset, sig) == -1)
		return(-1);
	if (sig == SIGKILL || sig == SIGSTOP) {
		errno = EINVAL;
		return(-1);
	}
	return (sigprocmask(SIG_UNBLOCK, &nset, &oset));
}

void
(*sigset(int sig, void (*func)(int)))(int)
{
	struct sigaction nact;	/* new signal action structure */
	struct sigaction oact;  /* returned signal action structure */ 
	struct sigaction *nactp;	/* pointer to new action, or NULL */
	sigset_t set,oset;	/* new and old signal masks */
	int	rc;	/* return value from sigaction */


	/* Error handling put in here to trap sigset(SIGKILL,SIG_DFL) 
	 * and also to avoid return value checking for each call to
         * sigaddset,sigprocmask,sigismember .. */

	if( sig <= 0 || sig > SIGMAX || sig == SIGKILL ) {
		errno=EINVAL;
		return(SIG_ERR);
	}
		
	sigprocmask(SIG_BLOCK,(sigset_t *) NULL,&oset);

	/* If function is SIG_HOLD mask this signal using sigprocmask
         * and clear the flag using sigaction to let the
	 * kernel know we are using the ATT sigset semantics        */

	if(func==SIG_HOLD) {
		sigemptyset(&set);
		sigaddset(&set,sig);
		sigprocmask(SIG_BLOCK,&set,(sigset_t *) NULL);
		rc = sigaction(sig,(struct sigaction *) NULL,&oact);
	}
	else {
        	/*  Set new handler,set mask to ignore this signal when 
                 *  handler is executing and clear flags */
		nact.sa_handler = func;
		sigemptyset(&(nact.sa_mask));
		sigaddset(&(nact.sa_mask), sig);
		nact.sa_flags = 0;
		rc=sigaction(sig,&nact,&oact);
	}

	if(rc == -1)
		return(SIG_ERR);

 	/* If previous action was SIG_HOLD return SIG_HOLD */
	if(sigismember(&oset,sig) == 1)  {
	        /* If new function installed clear the previous HOLD */
		if(func != SIG_HOLD) {
			sigemptyset(&set);
			sigaddset(&set,sig);
			sigprocmask(SIG_UNBLOCK,&set,(sigset_t *) NULL);
		}
		return(SIG_HOLD);
	}

	return (oact.sa_handler);
}


int
sigignore(int sig)
{
	struct sigaction nact;	/* new signal action structure */
	sigset_t set;		/* new signal mask */

	/* If the signal was set to SIG_HOLD release it */
	sigemptyset(&set);
	if( sigaddset(&set,sig) == -1)
		return(-1);
	sigprocmask(SIG_UNBLOCK,&set,(sigset_t *) NULL);
		
	nact.sa_handler = SIG_IGN;
	sigemptyset(&(nact.sa_mask));
	nact.sa_flags = 0;
	return (  sigaction(sig, &nact, (struct sigaction *)NULL)  ) ;

}
