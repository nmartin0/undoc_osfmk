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
 * (c) Copyright 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
 */
/*
 * OSF/1 Release 1.0.4
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: exit.c,v $ $Revision: 1.7 $ (OSF) $Date: 1994/11/19 02:04:33 $";
#endif

#include <sys/types.h>  /* for mon.h */
#include <mon.h>        /* for _mondata.proftype definition */
#include <limits.h>	/* for ATEXIT_MAX */

#ifdef _THREAD_SAFE
#include "rec_mutex.h"

extern struct rec_mutex _exit_rmutex;
#endif

/*
 * NAME: atexit
 *
 * FUNCTION: Special program termination sequence.  atexit() may be used
 *	     to register up to 32 functions that are to be executed before
 *           normal program termination
 *
 * PARAMETERS: 
 *  	     *func() - pointer to function to be executed before normal
 *	   	       program termination
 *
 * RETURN VALUE DESCRIPTIONS:
 *	     - 0 if registration succeeds
 *	     - 1 if registration does not succeed
 *
 */


static struct atexitarray {
	void (*func)(void);	
} ex_actions[ATEXIT_MAX];

static int action_cnt=-1;


int	
atexit(void (*func)(void))
{
	register int retval;

#ifdef _THREAD_SAFE
	rec_mutex_lock(&_exit_rmutex);
#endif
	if (action_cnt == -1)
		action_cnt = 0;
	if (action_cnt < ATEXIT_MAX) {
		ex_actions[action_cnt].func = func;
		action_cnt++;
		retval = 0;
	} else
		retval = 1;
#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_exit_rmutex);
#endif
	return(retval);
}

/*
 * NAME: exit
 *
 * FUNCTION: Normal program termination
 *
 * PARAMETERS: 
 *	     int code - status of exit
 *
 */

void
exit(int status)
{

/*** Leave out until profiling works. ****
	extern struct monglobal _mondata; 
	static struct monglobal *z=&_mondata; 

	if ( z->prof_type != 0 ){    
		monitor((caddr_t)0); 
	}
***/

#ifdef _THREAD_SAFE
	/*
	 * Don't bother unlocking as this would cause a race to get to the
	 * exit.
	 */
	rec_mutex_lock(&_exit_rmutex);
#endif
	while (action_cnt > 0)
		(*ex_actions[--action_cnt].func)();
	ldr_atexit();	/* last-minute loader/obj file cleanup */
	_cleanup();
	_exit(status);
}
