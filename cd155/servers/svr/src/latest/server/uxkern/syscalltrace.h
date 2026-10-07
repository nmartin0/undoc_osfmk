/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * System call tracing
 */
#define	SYSCALLTRACE	1

#ifdef	SYSCALLTRACE
extern int	syscalltrace;

#define	SC_TRACE(arglist)	{ \
	if (syscalltrace == -1 || syscalltrace == u.u_procp->p_pid) { \
	    cprintf arglist ; \
	} \
}
#else
#define SC_TRACE(arglist)

#endif

