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
static char rcsid[] = "@(#)$RCSfile: signum.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:26:49 $";
#endif
/*
 * COMPONENT_NAME: (CMDCNTL) system control commands
 *
 * FUNCTIONS: signum,sigprt
 *
 * ORIGINS: 27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 * 
 * signum.c    1.5  com/cmd/cntl,3.1,9021 2/16/90 13:36:05
 */
#include <ctype.h>
#include <sys/signal.h>


/*
 * Table of signal names associated with signal codes.
 */

char *SGcodes[NSIG] = {
	"NULL",		"HUP",		"INT",		"QUIT", /* 0-3   */
	"ILL",		"TRAP",		"IOT",		"EMT",	/* 4-7   */
	"FPE",		"KILL",		"BUS",		"SEGV", /* 8-11  */
	"SYS",		"PIPE",		"ALRM",		"TERM", /* 12-15 */
	"URG",		"STOP",		"TSTP",		"CONT", /* 16-19 */
	"CHLD",		"TTIN",		"TTOU",		"IO",   /* 20-23 */
	"XCPU",		"XFSZ",		"VTALRM",	"PROF", /* 24-27 */
	"WINCH",	"", 	 	"USR1",		"USR2"  /* 28-31 */
};


/*
 * Table of descriptive signal names associated with signal codes.
 */

char *SGnames[NSIG] = {
	"Signal 0",		"Hangup",		"Interrupt",
	"Quit",			"Illegal instruction",	"Breakpoint",
	"Abort",		"EMT instruction",	"Floating exception",
	"Killed",		"Bus error",		"Memory fault",
	"Bad system call",	"Broken pipe",		"Alarm call",
	"Terminated",		"Urgent I/O",		"Stopped",
	"Suspended",		"Continue", 		"Child death",
	"Background read",	"Background write",	"I/O completed",
	"CPU time exceeded",	"File size exceeded", 	"Virtual time alarm",
	"Profiling alarm",	"Window size changed", "Signal 29",
	"User signal 1",	"User signal 2"
};

/*
 * NAME:  signum
 * FUNCTION: signum will convert a signal name to a signal number.
 *    if signal is a number that signum will convert it to an integer.
 */
int signum(char *signal)
{
	int i;
	char *s;

	if (*signal == NULL)
		return (-1);

	if (isdigit(signal[0])) {
		i = atoi(signal);
		if ((i > 0) && (i < NSIG))
			return(i);
		return(-1);
	}
	for (i=0; i < strlen(signal); ++i) 
		signal[i]=toupper((int)signal[i]);
	/* strip SIG off of string */
	if (signal[0] == 'S' && signal[1] == 'I' && signal [2] == 'G')
		s = signal+3;
	else
		s = signal;
	/* compare string to list of signals in SGcodes */
	for (i=0; i < NSIG; i++)
		if (!strcmp(s,SGcodes[i])) {
			return(i);		
		}
	return(-1);
}

/*
 * NAME: sigprt
 * FUNCTION: sigprt will print out a list of signal names without the prefix SIG
 */
void sigprt()
{
#define SIGCOLS		4		/* number of columns for '-l' */
	int i, j, sigi, rows, nsig = 0;
	int prsig[NSIG];

	/*
	 * There are holes in the signal numbers used.  Find which
	 * ones are in use to properly format them.
	 */
	for (i = 1; i < NSIG; i++)
		if(*SGcodes[i] != '\0')
			prsig[nsig++] = i;

	/*
	 * Print signal name list, sort vertically in columns.
	 */
	rows = (nsig+SIGCOLS-1)/SIGCOLS;
	for (i = 0; i < rows; i++) {
		for (j = 0; j < SIGCOLS; ++j) {
			sigi = (j*rows+i);
			if (sigi >= nsig)
				break;
			printf(" %2d: SIG%-10s", prsig[sigi],
					SGcodes[prsig[sigi]]);
		}
		printf("\n");
	}	
}

