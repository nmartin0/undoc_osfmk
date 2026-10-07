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
static char rcsid[] = "@(#)$RCSfile: uname.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:44:40 $";
#endif
/*	uname.c	1.2	*/

#include	<stdio.h>
#include	<sys/utsname.h>

struct utsname	unstr, *un;

main(argc, argv)
char **argv;
int argc;
{
	register i;
	int	sflg=1, nflg=0, rflg=0, vflg=0, mflg=0, errflg=0;
	int	optlet;

	un = &unstr;
	uname(un);

	while((optlet=getopt(argc, argv, "asnrvm")) != EOF) switch(optlet) {
	case 'a':
		sflg++; nflg++; rflg++; vflg++; mflg++;
		break;
	case 's':
		sflg++;
		break;
	case 'n':
		nflg++;
		break;
	case 'r':
		rflg++;
		break;
	case 'v':
		vflg++;
		break;
	case 'm':
		mflg++;
		break;
	case '?':
		errflg++;
	}
	if(errflg) {
		fprintf(stderr, "usage: uname [-snrvma]\n");
		exit(1);
	}
	if(nflg | rflg | vflg | mflg) sflg--;
	if(sflg)
		fprintf(stdout, "%.*s", SYS_NMLN, un->sysname);
	if(nflg) {
		if(sflg) putchar(' ');
		fprintf(stdout, "%.*s", SYS_NMLN, un->nodename);
	}
	if(rflg) {
		if(sflg | nflg) putchar(' ');
		fprintf(stdout, "%.*s", SYS_NMLN, un->release);
	}
	if(vflg) {
		if(sflg | nflg | rflg) putchar(' ');
		fprintf(stdout, "%.*s", SYS_NMLN, un->version);
	}
	if(mflg) {
		if(sflg | nflg | rflg | vflg) putchar(' ');
		fprintf(stdout, "%.*s", SYS_NMLN, un->machine);
	}
	putchar('\n');
	exit(0);
}
