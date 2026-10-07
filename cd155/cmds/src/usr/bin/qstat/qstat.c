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
 
/*++ qstat.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/qstat/qstat.c,v $
 *
 * DESCRIPTION:
 *
 *	Display status information.
 *	This program must run as a setuid program.
 *
 *	Since the method of arriving at the target displays are
 *	the same they share the same major sections of code.
 *	The arguments and target is passed onto main_dsp().
 *	This is ease in maintainability and readability.
 *
 *	The shared code may be found in lib/main_dsp.  Each
 *	set of code that is specific to queues, requests, and
 *	complexs may be found in queues.c, requests.c and complexes.c
 *	
 */
/*
 * HISTORY
 * $Log: qstat.c,v $
 * Revision 1.5  1994/11/19  01:36:01  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/11/02  01:09:24  mwan
 * 1.2 mods
 *
 * Revision 1.2  1992/10/09  22:30:28  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  19:29:21  rkl
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include "nqs.h"

/*** main
 *
 *
 *	qstat [-b] [-p] [-d] [-f] [-L] [-n] [-x] [-A] [-g] [-u] [-U] [-T] [-h] 
 *		[ <queue-name(s)> ]
#ifdef SDSC
 *	added one more option [-t n_day] to list tennis court time slot.
 *      and [-v] verbose mode (132 characters per line).
#endif
 */
main (argc, argv)
int argc;
char *argv[];
{
	long flags=0;
	char * selector=NULL;
	char * tname=NULL;
 	char * hname=NULL;
	int	i;

	/* step 1. parse the command line */
#ifdef SDSC
	flags = parse_args(&argc, argv,"abdfg:h:lnpvs:u:A:Ut:",&selector,
		&tname,&hname);	/* CERN Boissat */
#else
	flags = parse_args(&argc, argv,"abdfg:h:lnps:u:A:U",&selector,
		&tname,&hname);	/* CERN Boissat */
#endif


	/* step 2. set SHO_QUE so we know the object to deal with  */

	/* If the options are -L, -b, -d, or -p, then we concerned */
	/* with the queues . . .                                   */

	if ((flags & SHO_LIMITS) || (flags & SHO_BATCH) ||
	    (flags & SHO_DEVICE) || (flags & SHO_PIPE))
	  flags |= SHO_QUE;

	/* If the options are -A acct, -T user, -U, -a, -g group,  */
	/* -s state, or -u user, . . . in other words, anything    */
	/* else (or nothing) . . . then we are concerned with      */
	/* requests.                                               */

#ifdef SDSC
	else if (flags & SHO_TSCHED) 
	  flags |= SHO_REQ;
#endif
	else
	  flags |= (SHO_BATCH | SHO_DEVICE | SHO_PIPE | SHO_REQ);	/* CERN Boissat */

	
	/* step 3. display the information */
	main_dsp(argc,argv,flags,selector,tname,hname);
	printf("\n");

}

