/*
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
 * $Log: hostinfo.c,v $
 * Revision 1.6.4.1  1995/06/11  18:42:21  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.6  1994/11/21  16:41:22  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/09/16  16:49:58  stans
 * added Intel/SSD copyright notice
 *
 * Revision 1.4  1993/09/09  22:35:22  stans
 *    Added an optional command-line node parameter. 'node' specifies which
 *    micro-kernel the 'hostinfo' command collects information from.
 *
 * Revision 1.3  1993/05/18  20:14:36  stans
 *   Use the Mach 2.5 compat interfaces (xxx_*) from the Mach 3.0 version of
 *   'libmach.a'. To use the true Mach 3.0 host_info() and slot_info()
 *   interfaces, some assembly (C code rework) will be required.
 *
 * Revision 1.2  1993/05/07  19:55:50  stans
 * The Mach 2.5 compat. routine "xxx_slot_info()" is now in libmach.a built
 * from the micro-kernel sources.
 *
 * Revision 1.1  1992/10/31  23:39:09  shala
 * Display information about the host.
 *
 * Revision 1.2  89/05/05  18:22:18  mrt
 * 	Cleanup for Mach 2.5
 * 
 *
 * 28-Feb-87  Avadis Tevanian (avie) at Carnegie-Mellon University
 *	Created.
 *
 */
/*
 *	File:	hostinfo.c
 *	Author:	Avadis Tevanian, Jr.
 *
 *	Copyright (C) 1987, Avadis Tevanian, Jr.
 *
 *	Display information about the host this program is
 *	execting on.
 */


#include <stdio.h>
#include <mach.h>
#include <signal.h>
#include <errno.h>

struct machine_info	mi;
struct machine_slot	ms;
int			nslots;
mach_port_t		aTask=MACH_PORT_NULL;

#ifdef	_OSF_SOURCE
/* Mach 2.5 compat interfaces (xxx_*) within the Mach 3.0 libmach.a */
#define	SLOT_INFO xxx_slot_info
#define	HOST_INFO xxx_host_info
#else	/* to use the Mach 3.0 infterfaces requires a code rewrite. */
#define	HOST_INFO host_info
#define	SLOT_INFO slot_info
#endif

task_destroy( tport )
        mach_port_t     tport;
{
        kern_return_t   kr;

	if ( tport == MACH_PORT_NULL || tport == mach_task_self() )
		return;

        if ((kr=task_terminate( tport )) != KERN_SUCCESS) {
                mach_error("task_terminate(%d)",kr);
                exit( 1 );
        }
}

void
sig_handler()
{
	task_destroy( aTask );
	exit(0);
}

main(argc, argv)
	int	argc;
	char	*argv[];
{
	kern_return_t	kr;
	int		target_node;
	register int	i;

        /* remove any remotely created tasks */
        signal(SIGINT, sig_handler);

	aTask = mach_task_self();

	/*
	 * was a node number specified?
	 */
	if ( argc == 1 ) {
		norma_node_self(aTask, &target_node);
	}
	else {
		/*
		 * pickup specified node number and verify it.
		 */
		target_node = atoi(argv[1]);
		if ( meshnode_is_up( target_node ) == FALSE ) {
			fprintf(stderr,"%s: invalid node %d\n",
							argv[0], target_node);
			exit( EINVAL );
		}
		/*
		 * create a task on the remote node for the purpose of needing
		 * a remote task port for the {host/slot}_info() Mach system
		 * calls.
		 */
		kr = norma_task_create(aTask, FALSE, target_node, &aTask);
		if ( kr != KERN_SUCCESS ) {
			mach_error("norma_task_create()",kr);
			exit( 1 );
		}
	}

	if ((kr = HOST_INFO(aTask, &mi)) != KERN_SUCCESS) {
		mach_error(argv[0], kr);
		task_destroy( aTask );
		exit(1);
	}

	printf("node %d, Mach kernel version %d.%d.\n", target_node,
			mi.major_version, mi.minor_version);

	if ( (nslots = mi.max_cpus) > 1)
		printf("Kernel configured for up to %d processors.\n", nslots);
	else
		printf("Kernel configured for a single processor only.\n");

	printf("%d processor%s physically available.\n", mi.avail_cpus,
		(mi.avail_cpus > 1) ? "s are" : " is");
	printf("Primary memory available: %.2f megabytes.\n",
			(float)mi.memory_size/(1024.0*1024.0));

	for (i = 0; i < nslots; i++)
		do_slot( aTask, i );

	task_destroy( aTask );	/* cleanup */
}

do_slot( task_port, slot )
	mach_port_t	task_port;
	int		slot;
{
	kern_return_t	kr;

	kr = SLOT_INFO(task_port, slot, &ms);
	if (ms.is_cpu) {
#if 1
		/* slot is confusing terminology, just say CPU */
		printf("CPU %d:", slot);
#else
		printf("slot %d:", slot);
#endif
		do_cpu(&ms);
		printf("\n");
	}
}

do_cpu(ms)
	register struct machine_slot	*ms;
{
	char	*cpu_name, *cpu_subname;

	slot_name(ms->cpu_type, ms->cpu_subtype, &cpu_name, &cpu_subname);
	printf(" %s (%s) %s", cpu_name, cpu_subname,
			ms->running ? "UP" : "DOWN");
}
