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

#include <stdio.h>
#include <mach.h>

main(argc, argv)
	char *argv[];
{
	task_t		task;
	vm_address_t	address, end;
	vm_size_t	size;
	vm_prot_t	protection, max_protection;
	vm_inherit_t	inheritance;
	boolean_t	shared;
	port_t		object_name;
	vm_offset_t	offset;
	int		rc, first = 1;
	int		pid;

	if (argc < 2) {
		fprintf(stderr, "usage: %s <process-id>\n", argv[0]);
		exit(1);
	}

	pid = atoi(argv[1]);

	if ((rc = task_by_unix_pid(task_self(), pid, &task)) != KERN_SUCCESS) {
		fprintf(stderr, "%s: task_by_unix_pid(task_self(), %d, &task) failed: %s\n",
			argv[0], pid, mach_error_string(rc));
		exit(1);
	}

	for (address = (vm_address_t)0; 
	  vm_region(task, &address, &size, &protection, &max_protection,
	    &inheritance, &shared, &object_name, &offset) == KERN_SUCCESS;
	  address += size) {

		if (first)
			first = 0;
		else
			if (address != end)
				printf("*                     %#10x\n", address-end);
		end = address+size;
		printf("%#10x %#10x %#10x ", address, end, size);
		if (protection == (vm_prot_t)0)
			printf("none");
		else {
			if (protection & VM_PROT_READ)
				putchar('r');
			if (protection & VM_PROT_WRITE)
				putchar('w');
			if (protection & VM_PROT_EXECUTE)
				putchar('x');
		}
#ifdef	INHERIT
		putchar(' ');
		if (inheritance == VM_INHERIT_SHARE)
			printf("share");
		else if (inheritance == VM_INHERIT_COPY)
			printf("copy");
		else if (inheritance == VM_INHERIT_NONE)
			printf("none");
#endif
		putchar('\n');
	}
	exit(0);
}
