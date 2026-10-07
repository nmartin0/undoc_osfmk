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

/*
 * exec_with_loader - progam to call exec_with_loader()
 */

#include <stdio.h>

char usagestring[] =
"usage: exec_with_loader <flags> <loader> <file> argv[0] argv[1] ... argv[n]\n\
       exec_with_loader <flags> 0 <file> argv[0] argv[1] ... argv[n]\n\
\n\
       flags  = a decimal, hexadecimal or octal number\n\
       loader = file name of the loader to execute (0 means use default loader)\n\
       file   = file name of the file for the loader to load\n\
       argv   = arguments, including argv[0], to be passed to loader\n\
                and command\n\
\n\
                environment passed as is to the loader and command; use\n\
                env(1) to override environment\n";

extern char *environ;

int
main(argc, argv)
	char *argv[];
{
	char *loader, *file, **nargv;
	int flags;

	if (argc < 4) {
		(void)fprintf(stderr, "%s", usagestring);
		exit(1);
	}
	if (number(argv[1], &flags) == -1) {
		(void)fprintf(stderr, "exec_with_loader: bad number \"%s\"\n",
			argv[1]);
		exit(2);
	}
	loader = argv[2];
	if ((loader[0] == '0') && (loader[1] == '\0'))
		loader = (char *)0;

	file = argv[3];
	nargv = &argv[4];

	if (exec_with_loader(flags, loader, file, nargv, environ) == -1) {
		perror("exec_with_loader: exec_with_loader() failed");
		exit(3);
	}
	exit(0);
	return(0);	
}

/*
 * number() - Ascii (Hex, Octal, or Decimal) to Number
 */
int
number(cp, ip)
	char *cp;
	int *ip;
{
	int c, i;

	i = 0;
	if ((cp[0] == '0') && ((cp[1] == 'x') || (cp[1] == 'X')))
		for (cp = &cp[2]; c = *cp; cp++)
			if (('0' <= c) && (c <= '9'))
				i  = (i*16) + c - '0';
			else if (('a' <= c) && (c <= 'f'))
				i = (i*16) + c - 'a' + 10;
			else if (('A' <= c) && (c <= 'F'))
				i = (i*16) + c - 'A' + 10;
			else
				return(-1);
	else if (cp[0] == '0')
		for (cp = &cp[1]; c = *cp; cp++)
			if (('0' <= c) && (c <= '7'))
				i  = (i*8) + c - '0';
			else
				return(-1);
	else
		for (; c = *cp; cp++)
			if (('0' <= c) && (c <= '9'))
				i  = (i*10) + c - '0';
			else
				return(-1);
	*ip = i;
	return(0);
}
