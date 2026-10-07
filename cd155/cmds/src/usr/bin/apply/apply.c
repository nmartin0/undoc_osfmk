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
static char rcsid[] = "@(#)$RCSfile: apply.c,v $ $Revision: 1.4 $ (OSF) $Date: 1994/11/19 01:18:42 $";
#endif
/*
 * COMPONENT_NAME: (CMDSH) Bourne shell and related commands
 *
 * FUNCTIONS:
 *
 * ORIGINS: 26, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */
/*
 * NAME:  apply - apply a command to a set of arguments
 *
 * FUNCTION: 
 *      apply [-ac] [-n] command args ...
 *      -ac    changes '%' to be the character of your choice c.
 *      -n     number of arguments to be passed to the commmand.
 * NOTE:
 *	apply echo * == ls
 *	apply -2 cmp A1 B1 A2 B2   compares A's with B's
 *	apply "ln %1 /usr/fred/dir" *  duplicates a directory
 *        If your are having problems getting apply to work quote the command.
 */
#include <stdio.h>
#include <unistd.h>
#include <locale.h>
#include <NLchar.h>

#include "apply_msg.h" 
nl_catd catd;
#define MSGSTR(n,s) catgets(catd,MS_APPLY,n,s) 

char	*cmdp;
#define	NCHARS 512

NLchar	cmd[NCHARS];

char	defargs=1;
#define	DEFARGCHAR	'%'

NLchar	argchar=DEFARGCHAR;

int	nchars;

main(argc, argv)
int argc;
char *argv[];
{
	register int n;

	(void) setlocale (LC_ALL, "");
	catd = catopen(MF_APPLY,0);

	while(argc>2 && argv[1][0]=='-'){
		if(argv[1][1]=='a'){

			if(NCisshift(argv[1][2]))
				NCdecode(&argv[1][2],argchar);
			else	argchar=(NLchar)argv[1][2];

			if(argchar=='\0')
				argchar=DEFARGCHAR;
		} else {
			defargs = atoi(&argv[1][1]);
			if((int)defargs < 0)
				defargs = 1;
		}
		--argc; ++argv;
	}
	if(argc<2){
		fprintf(stderr, MSGSTR(USAGE, "usage: apply [-14] [-a%] cmd arglist\n")); /*MSG*/
		exit(1);
	}
	argc -= 2;
	cmdp = argv[1];
	argv += 2;
	while(n=docmd(argc, argv)){
		argc -= n;
		argv += n;
	}
	exit(0);
}

/*
 * NAME: addc
 *
 * FUNCTION: add character to command
 */
NLchar addc(c)
char c;
{
	NLchar tmp;
	if(nchars++>=NCHARS){

		fprintf(stderr, "apply: command too long\n");
		exit(1);
	}

	NCdecode(&c,&tmp);
	return (tmp);
}

/*
 * NAME: addarg
 *
 * FUNCTION: check length of s and copy to t.
 */
NLchar *addarg(s, t)
	register char *s;
	register NLchar *t;
{
	while(*t = addc(*s++))
		*t++;
	return(t);
}

/*
 * NAME: docmd
 *
 * FUNCTION:  get command from argv
 */
docmd(argc, argv)
	char *argv[];
{
	register char *p;
	NLchar *q;
	register max, i;
	char gotit;
	if(argc<=0)
		return(0);
	nchars = 0;
	max = 0;
	gotit = 0;
	p = cmdp;
	q = cmd;
	while(*q = addc(*p++)){
		if(*q++!=argchar || *p<'1' || '9'<*p)
			continue;
		if((i= *p++-'1') > max)
			max = i;
		if(i>=argc){
	Toofew:
			fprintf(stderr, MSGSTR(TOOFEWARGS, "apply: expecting argument(s) after `%s'\n"), argv[argc-1]); /*MSG*/
			exit(1);
		}
		q = addarg(argv[i], q-1);
		gotit++;
	}
	if(defargs!=0 && gotit==0){
		if(defargs>argc)
			goto Toofew;
		for(i=0; i<defargs; i++){
			*q++ = addc(' ');
			q = addarg(argv[i], q);
		}
	}
	i = system(cmd);
	if(i == 127){
		fprintf(stderr, MSGSTR(NOSHELL, "apply: no shell!\n")); /*MSG*/
		exit(1);
	}
	return(max==0? (defargs==0? 1 : defargs) : max+1);
}

/*
 * NAME: system
 * FUNCTION: execute command s
 */
system(s)
NLchar *s;
{
        char tmpbuf[NCHARS];
	int status, pid, w;
	char *shell = getenv("SHELL");

	if ((pid = fork()) == 0) {

		NCencstr(s,tmpbuf,sizeof(tmpbuf));
		execl(shell ? shell : "/bin/sh", "sh", "-c", tmpbuf, 0);
		exit(1);
	}
	if(pid == -1){
		fprintf(stderr, MSGSTR(NOFORK, "apply: can't fork\n")); /*MSG*/
		exit(1);
	}
	while ((w = wait(&status)) != pid && w != -1)
		;
	if (w == -1)
		status = -1;
	return(status);
}
