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
static char rcsid[] = "@(#)$RCSfile: tee.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:41:25 $";
#endif
/*
 * COMPONENT_NAME: (CMDSCAN) commands that scan files
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 26, 27
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
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * Copyright 1976, Bell Telephone Laboratories, Inc.
 * tee.c	1.4  com/cmd/scan,3.1,9008 12/21/89 12:53:09
 */

/*
 *
 * tee-- pipe fitting.	Displays the  output of  a
 *	program and  copies it into a file.
 */                                                                   

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <locale.h>
#include <stdlib.h>
#include "tee_msg.h"

/* there's only one message, so efficency isn't an issue */
#define MSGSTR(Num,Str) NLgetamsg("tee.cat",MS_TEE,Num,Str) 

static void stash();

int openf[20] = { 1 };
int n = 1;
int t = 0;
int aflag;

char in[BUFSIZ];
char out[BUFSIZ];

main(argc,argv)
char **argv;
{
	int r,w,p;
	struct stat buf;

	(void) setlocale (LC_ALL,"");
	while(argc>1&&argv[1][0]=='-') {
		switch(argv[1][1]) {
		case 'a':		/* append mode            */
			aflag++;
			break;
		case 'i':
		case 0:			/* ignore the kill signal */
			signal(SIGINT, SIG_IGN);
		}
		argv++;
		argc--;
	}
	fstat(1,&buf);
	t = (buf.st_mode&S_IFMT)==S_IFCHR;
	if(lseek(1,0L,SEEK_CUR)==-1&&errno==ESPIPE)
		t++;
	while(argc-->1) {
		openf[n++] = open(argv[1],O_WRONLY|O_CREAT|
			(aflag?O_APPEND:O_TRUNC), 0666);
		if(stat(argv[1],&buf)>=0) {
			if((buf.st_mode&S_IFMT)==S_IFCHR)
				t++;
		} else {
			printf(MSGSTR(NO_FILE,"tee: cannot open %s\n"), argv[1]);
			n--;
		}
		argv++;
	}
	r = w = 0;
	for(;;) {
		for(p=0;p<BUFSIZ;) {
			if(r>=w) {
				if(t>0&&p>0) break;
				w = read(0,in,BUFSIZ);
				r = 0;
				if(w<=0) {
					stash(p);
					exit(0);
				}
			}
			out[p++] = in[r++];
		}
		stash(p);
	}
}

/*
 * NAME: stash
 *                                                                    
 * FUNCTION: copy the buffer to each of the output files.
 *
 * RETURN VALUE DESCRIPTION: none
 */  
static void
stash(p)
{
	int k;
	int i;
	int d;
	d = t ? 16 : p;
	for(i=0; i<p; i+=d)
		for(k=0;k<n;k++)
			write(openf[k], out+i, d<p-i?d:p-i);
}
