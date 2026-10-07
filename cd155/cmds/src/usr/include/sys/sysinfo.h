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
 * IBM CONFIDENTIAL
 * Copyright International Business Machines Corp. 1989
 * Unpublished Work
 * All Rights Reserved
 * Licensed Material - Property of IBM
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/* @(#)sysinfo.h	1.7  com/inc/sys,3.1,8943 8/14/89 14:11:22 */
/* sysinfo.h	5.2 87/01/09 18:25:51 */
/*
 * OSF/1 Release 1.0
 */

#ifndef _SYS_SYSINFO_H_
#define _SYS_SYSINFO_H_

/*  NOTE:  An assembly-language version of these structures exists
 *            in "sysinfo.m4".  BE SURE to update that file whenever
 *            <sys/sysinfo.h> is changed!!!
 */

#include <sys/types.h>

struct sysinfo {
#define	CPU_NTIMES	4 	/* number of cpu times */
	time_t	cpu[CPU_NTIMES];   /* this array is updated every clock tick,
			     and keys off the state of the current running
			     process */
#define	CPU_IDLE	0 /* slot incremented whenever the 'wait' process
			     is the current running process */
#define	CPU_USER	1 /* slot incremented whenever the current running
			     process is executing in user mode */
#define CPU_KERNEL	2 /* slot incremented whenever the current running 
			     process is executing in kernel mode */
#define	CPU_WAIT	3 /* slot is always zero. This slot used to be
			     incremented whenever the current running process
			     was waiting for a block i/o request to
			     complete. Currently, whenever a process becomes
			     blocked, it is put to sleep and a new process
			     is made the current running process (i.e. processes
			     no longer maintain control of the cpu when they 
			     become blocked). */
	time_t	wait[3];
#define	W_IO	0
#define	W_SWAP	1
#define	W_PIO	2
#define	sysfirst	bread	/* first sysinfo variable - used by sysrates()
			  	   sysfirst define should be maintained as 
				   first non-array sysinfo variable */
	long	bread;
	long	bwrite;
	long	lread;
	long	lwrite;
	long	phread;
	long	phwrite;
	long	pswitch;
	long	syscall;
	long	sysread;
	long	syswrite;
	long	sysfork;  /* field is incremented by one whenever a 'fork'
			     is done */
	long	sysexec;  /* field is incremented by one whenever a 'exec'
			     is done */
	long	runque;   /* every second the process table is scanned to
			     determine the number of processes that are
			     ready to run. If that count is > 0 the
			     current number of ready processes is added 
			     to 'runque' (i.e. 'runque' is a cummulative
			     total of ready processes at second intervals). */
	long	runocc;   /* whenever 'runque' is updated, 'runocc'
			     is incremented by 1 (can be used to compute
			     the simple average of ready processes). */
	long	swpque;   /* every second the process table is scanned to 
			     determine the number of processes that are
			     inactive because they are waiting to be paged
			     in. If that count is > 0 then the current number
			     of processes waiting to be paged in is added
			     to 'swpque' (i.e. 'swpque' is a cummulative
			     total of processes waiting to be swapped in
			     at second intervals). */
	long	swpocc;   /* whenever 'swpque' is updated, 'swpocc' is 
			     incremented by one (can be used to compute
			     the simple average of processes waiting to be 
			     paged in).*/
	long	iget;
	long	namei;
	long	dirblk;
	long	readch;
	long	writech;
	long	rcvint;
	long	xmtint;
	long	mdmint;
	long	rawch;
	long	canch;
	long	outch;
	long	msg;
	long	sema;
	long    ksched;   /* field is incremented by one whenever a kernel
                             process is created */
	long    koverf;   /* field is incremented by one whenever an  attempt
			     is made to create a kernel process and:
				- the user has forked to their maximum limit
					       - OR -
				- the configuration limit of processes has been
				  reached */
	long    kexit;    /* field is incremented by one immediately after the
			     kernel process becomes a zombie */
	long    rbread;         /** remote read requests       **/
	long    rcread;         /** reads from remote cache    **/
	long    rbwrt;          /** remote writes              **/
	long    rcwrt;          /** cached remote writes       **/
#define	syslast		rcwrt	/* last sysinfo variable - used by sysrates() 
				   syslast define should be maintained as 
				   last non-array sysinfo variable */
};

extern struct sysinfo sysinfo;
extern struct sysinfo sysrate;
extern struct sysinfo syshist;

struct syswait {
	short	iowait;
	short	physio;
};

extern struct syswait syswait;

struct syserr {
	long	inodeovf;
	long	fileovf;
	long	textovf;
	long	procovf;
	long	sbi[5];
#define	SBI_SILOC	0
#define	SBI_CRDRDS	1
#define	SBI_ALERT	2
#define	SBI_FAULT	3
#define	SBI_TIMEO	4
};

extern struct syserr syserr;
#endif /* _SYS_SYSINFO_H_ */
