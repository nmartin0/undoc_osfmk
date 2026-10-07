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
#ifndef lint
#ifndef _NOIDENT
static char rcsid[] = "@(#) $RCSfile: nmf.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:56:10 $";
#endif
#endif
/* static char sccsid[] = "@(#)nmf.c	1.4  com/cmd/prog/cflow,3.1,9021 3/19/90 09:16:01"; */

/*
 * COMPONENT_NAME: (CMDPROG) Programming Utilites
 *
 * FUNCTIONS: main
 *
 * ORIGINS: 00 03 10 27 32
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1988, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

#include <stdio.h>
#include <locale.h>

#define SYMSTART 11
#define SYMCLASS 9

extern void exit();
extern char *strcpy(), *strncpy();

main(argc, argv)
char	*argv[];
{
	char name[BUFSIZ], buf[BUFSIZ], *fname = NULL, *pty;
	register char *p;
	int nsize, tysize, lineno;

	setlocale(LC_ALL, "");
	strcpy(name, argc > 1? argv[1] : "???");
	if (argc > 2)
		fname = argv[2];
	else
		fname = "???";
	while (gets(buf))
	{
		p = buf;
		while (*p != ' ' && *p != '|')
			++p;
		nsize = p - buf;
		do ; while (*p++ != '|');		/* skip rem of name */
		do ; while (*p++ != '|');		/* skip value */
		do ; while (*p++ != '|');		/* skip class */
		while (*p == ' ')
			++p;
		if (*p != '|')
		{
			pty = p++;
			while (*p != '|')
				++p;
			tysize = p - pty;
		}
		else
			pty = (char *) NULL;
		++p;
		do ; while (*p++ != '|');		/* skip size */
		while (*p == ' ')
			++p;
		lineno = atoi(p);
		do ; while (*p++ != '|');		/* and xlated line */
		while (*p == ' ')
			++p;
		if (!strncmp(p, ".text", 5) || !strncmp(p, ".data", 5))
		{					/* it's defined */
			strncpy(name, buf, nsize);
			name[nsize] = '\0';
			printf("%s = ", name);
			if (pty)
				printf("%.*s", tysize, pty);
			else
			{
				fputs("???", stdout);
				if (!strncmp(p, ".text", 5))
					fputs("()", stdout);
			}
			printf(", <%s %d>\n", fname, lineno);
		}
		else
			printf("%s : %.*s\n", name, nsize, buf);
	}
	exit(0);
	/*NOTREACHED*/
}
