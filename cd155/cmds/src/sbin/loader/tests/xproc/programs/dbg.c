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
#include <errno.h>
#include <string.h>

#include <signal.h>
#include <sys/ptrace.h>

#include <sys/types.h>
#include <sys/wait.h>

#include <loader.h>

char *program;

void parent(pid_t);
void wait_on_child(pid_t);
void child(char *);

main(argc, argv)
	char *argv[];
{
	pid_t pid;

	program = argv[0];

	if (argc < 2) {
		fprintf(stderr, "usage: %s <program-filename>\n", program);
		exit(1);
	}
	
	if (!(pid = fork()))
		child(argv[1]);

	if (pid == -1) {
		fprintf(stderr, "main: fork() failed: %s\n", strerror(errno));
		exit(1);
	}

	parent(pid);

	exit(0);
}

void
child(program)
	char *program;
{
	if (ptrace(PT_TRACE_ME) == -1) {
		fprintf(stderr, "child: ptrace(PT_TRACE_ME) failed: %s\n",
			strerror(errno));
		exit(1);
	}
	execl(program, program, (char *)0);
	fprintf(stderr, "child: execl(\"%s\", \"%s\", (char *)0) failed: %s\n",
		program, program, strerror(errno));
	exit(1);
}

void
parent(pid)
	pid_t pid;
{
	ldr_process_t child;

	child = (ldr_process_t)pid;
	wait_on_child(pid);
	cmd_info(child);

	exit(0);
}

void
wait_on_child(pid)
	pid_t pid;
{
	int status;

	if (wait(&status) == -1) {
		fprintf(stderr, "wait_on_child: wait() failed: %s\n",
			strerror(errno));
		exit(1);
	}

	if (!(WIFSTOPPED(status) && (WSTOPSIG(status) == SIGTRAP))) {
		fprintf(stderr, "wait_on_child: child not stopped with SIGTRAP: status = %#x\n",
			status);
		exit(1);
	}

#ifndef	STOP_ONCE_ONLY
	if (ptrace(PT_CONTINUE, pid, 1, 0) == -1) {
		fprintf(stderr, "wait_on_child: ptrace(PT_CONTINUE) failed: %s\n",
			strerror(errno));
		exit(1);
	}

	if (wait(&status) == -1) {
		fprintf(stderr, "wait_on_child: wait() failed: %s\n",
			strerror(errno));
		exit(1);
	}

	if (!(WIFSTOPPED(status) && (WSTOPSIG(status) == SIGTRAP))) {
		fprintf(stderr, "wait_on_child: child not stopped with SIGTRAP: status = %#x\n",
			status);
		exit(1);
	}
#endif	/* STOP_ONCE_ONLY */
}
