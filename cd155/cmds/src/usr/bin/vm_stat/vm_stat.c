/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: vm_stat.c,v $
 * Revision 1.6.4.1  1995/06/11  18:42:35  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.6  1994/11/19  01:47:10  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/05/13  18:53:13  shala
 *  Reviewer: NONE
 *  Risk: Low
 *  Benefit or PTS #: Bug #8447
 *  Testing: Build a new vm_stat and check on the exit status.
 *  Module(s): cmds_libs/src/usr/bin/vm_stat/vm_stat.c
 *
 * Needed to add exit(0) in the main module when it is successful.
 *
 * Revision 1.4  1993/09/16  15:49:55  stans
 *    Added NORMA multicomputer support in the form of a "-n node" cmd-line
 *    switch. If the "-nnode" switch is seen a remote task is created, Mach
 *    syscalls invoked using the remote task port instead of my local task
 *    port.
 *
 * Revision 1.3  1993/07/16  20:36:41  stans
 *    Revert back to older Mach 2.5 interface version as Mach 3.0 version
 *    does not correctly function. Looks like machID server does not do NORMA
 *    correctly.
 *
 * Revision 1.1.10.1  1993/06/18  17:27:49  dleslie
 * Adding copyright notice required by legal
 *
 * Revision 1.1  1992/05/20  15:13:01  shala
 * Initial revision
 *
 * Revision 1.2  89/05/05  18:26:25  mrt
 * 	Cleanup for Mach 2.5
 * 
 *
 *  6-Jun-86  Avadis Tevanian, Jr. (avie) at Carnegie-Mellon University
 *	Use official Mach interface.
 *
 */
/*
 *	File:	vm_stat.c
 *	Author:	Avadis Tevanian, Jr.
 *
 *	Copyright (C) 1986, Avadis Tevanian, Jr.
 *
 *	Display Mach VM statistics at local or specific node.
 *
 *	**** WARNING ****
 *		Mach 2.5 compatibility MK interfaces are used here. MachID
 *		server needs to be adapted for NORMA multicomputer operation
 *		before the Mach 3.0 style of vm_stat can be utilized.
 */


#include <stdio.h>
#include <mach.h>
#include <mach_init.h>
#include <signal.h>

typedef	int	node_t;

struct vm_statistics	vm_stat, last;

int		percent;
char		*pgmname;
mach_port_t	task_port=MACH_PORT_NULL;
mach_port_t	mytask;

/*
 * make sure we remove any remotely created tasks
 */
task_destroy( tport )
	mach_port_t     tport;
{
        kern_return_t	kr;

	if ((kr=task_terminate( tport )) != KERN_SUCCESS) {
		mach_error("task_terminate(%d)",kr);
		exit( 1 );
	}
}

/*
 * remove remote tasks on SIGINT.
 */
void sig_handler()
{
	if ( task_port != MACH_PORT_NULL && task_port != mytask )
		task_destroy( task_port );
	exit(0);
}


main(argc, argv)
	int	argc;
	char	*argv[];
{
	int		delay=0;
	node_t		mynode, target_node;
	kern_return_t	kr;

	pgmname = argv[0];	/* remember who we are */
	
	mytask = mach_task_self();

	/* retrieve my local node number */
	if ((kr = norma_node_self( mytask, &mynode)) != KERN_SUCCESS)
		quit(1,"vm_stat: norma_node_self():%s\n",mach_error_string(kr));

	/* assume local VM stats */
	task_port = mytask;
	target_node = mynode;

	if ( argc >= 2 ) {
		if ( strncmp(argv[1], "-n", 2) == 0 ) {
			register char	*cp;

			/* -n node or "-nnode" ? */
			if ( argv[1][2] == '\0' ) {
				argv++; argc--;
				cp = argv[1];
			}
			else
				cp = &argv[1][2];
			target_node = atoi( cp );
			/*
			 * we need a task port for the vm_stat() Mach syscall.
			 * here we create a remote task with no address space.
			 */
			if ((kr=norma_task_create( mytask, FALSE, target_node,
		   		&task_port )) != KERN_SUCCESS) {
				mach_error("norma_task_create()",kr);
				exit( 1 );
			}
			/* skip over any cmd-line switches */
			argv++; argc--;

			/* don't forget to remove remote tasks */
			signal(SIGINT, sig_handler);
		}
	}

	/*
	 * was a delay (in seconds) cmd-line specified?
	 */
	if (argc == 2) {
		if (sscanf(argv[1], "%d", &delay) != 1)
			usage();
		if (delay < 0)
			usage();
	}

	/*
	 * retrieve VM statistics from a micro-kernel
	 */

	if (delay == 0) {
		snapshot( task_port, target_node );
	}
	else {
		while (1) {
			print_stats( task_port, target_node );
			sleep(delay);
		}
	}

	/*
	 * do we need to remove a remote task?
	 */
	if ( task_port != mytask )
		task_destroy( task_port );
	exit(0);
}

usage()
{
	fprintf(stderr, "usage: %s [ {-n} node repeat-interval ]\n", pgmname);
	exit(1);
}

banner( task, target_node )
	task_t	task;
	node_t	target_node;
{
	get_stats( task, &vm_stat );
	printf("Mach Virtual Memory Statistics @ node [%d]: ", target_node);
	printf("(VM page size %d Kb, cache hits %d%%)\n",
				(vm_stat.pagesize/1024), percent);
	printf("%6s %6s %4s %4s %8s %8s %8s %8s %8s %8s\n",
		"free",
		"active",
		"inac",
		"wire",
		"faults",
		"copy",
		"zerofill",
		"reactive",
		"pageins",
		"pageout");
	bzero(&last, sizeof(last));
}

snapshot( task, target_node )
	task_t	task;
	node_t	target_node;
{

	get_stats( task, &vm_stat );
	printf("Mach Virtual Memory Statistics @ node [%d]: VM page size %d Kb\n",
				 target_node, (vm_stat.pagesize/1024));
	pstat("Pages free:", vm_stat.free_count);
	pstat("Pages active:", vm_stat.active_count);
	pstat("Pages inactive:", vm_stat.inactive_count);
	pstat("Pages wired down:", vm_stat.wire_count);
	pstat("\"Translation faults\":", vm_stat.faults);
	pstat("Pages copy-on-write:", vm_stat.cow_faults);
	pstat("Pages zero filled:", vm_stat.zero_fill_count);
	pstat("Pages reactivated:", vm_stat.reactivations);
	pstat("Pageins:", vm_stat.pageins);
	pstat("Pageouts:", vm_stat.pageouts);
	printf("Object cache: %d hits of %d lookups (%d%% hit rate)\n",
			vm_stat.hits, vm_stat.lookups, percent);
}

pstat(str, n)
	char	*str;
	int	n;
{
	printf("%-25s %10d.\n", str, n);
}

print_stats( task, target_node )
	task_t	task;
	node_t	target_node;
{
	static count = 0;

	if (count++ == 0)
		banner( task, target_node );

	if (count > 20)
		count = 0;

	get_stats( task, &vm_stat );
	printf("%6d %6d %4d %4d %8d %8d %8d %8d %8d %8d\n",
		vm_stat.free_count,
		vm_stat.active_count,
		vm_stat.inactive_count,
		vm_stat.wire_count,
		vm_stat.faults - last.faults,
		vm_stat.cow_faults - last.cow_faults,
		vm_stat.zero_fill_count - last.zero_fill_count,
		vm_stat.reactivations - last.reactivations,
		vm_stat.pageins - last.pageins,
		vm_stat.pageouts - last.pageouts);
	last = vm_stat;
}

get_stats( task, stat )
	task_t			task;
	struct vm_statistics	*stat;
{
	if (vm_statistics( task, stat) != KERN_SUCCESS) {
		fprintf(stderr, "%s: failed to get statistics.\n", pgmname);
		exit(2);
	}
	if (stat->lookups == 0)
		percent = 0;
	else
		percent = (stat->hits*100)/stat->lookups;
}
