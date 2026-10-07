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
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/qcmplx/qcmplx.c,v $
 *
 * DESCRIPTION:
 *
 *	Display queue status information.
 *	This program must run as a setuid program.
 *
 *	qstat, qwatch, and qcomplex are very similiar commands
 *	the real difference between these commands are the 
 *	target item.  Qstat targets the queues.  Qwatch 
 *	targets the requests and qcomplex targets the complexes.
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
 *	At one time a single binary with links for qstat, qwatch, and
 *	qcomplex was thought about.  We felt that this may constrain the
 *	code at some time in the future.  It was also felt that it may
 *	be more confusing to find the source code if it was the first
 *	time one had dove into the source.
 *					Terrie A. Carver  
 *					Computer Science Corperation
 *					Version 2.0 
 *					November 3, 1989
 */
/*
 * HISTORY
 * $Log: qcmplx.c,v $
 * Revision 1.5  1994/11/19  01:35:21  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/11/02  01:01:36  mwan
 * 1.2 mods
 *
 * Revision 1.2  1992/10/09  22:29:41  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  19:26:09  rkl
 * Initial revision
 *
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)qcmplx.c	1.2 (qcmplx.c OSF/1 NQS2.0 GJK) 6/11/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#include "nqs.h"

/*** main
 *
 *
 *	qcmplx -Q
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
	flags = parse_args(&argc, argv,"nh:Q",&selector,
#else
	flags = parse_args(&argc, argv,"n:h:Q",&selector,
#endif
		&tname,&hname);

	flags |= SHO_CMPLX;


	/* step 4. display the information */
	main_dsp(argc,argv,flags,selector,tname,hname);
	printf("\n");

}

