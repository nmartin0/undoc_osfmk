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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.2
 */ 
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: cmdtab.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:42:38 $";
#endif
/*
cmdtab.c	1.3  com/cmd/tip,3.1,9013 10/15/89 10:41:49";
 */
/* 
 * COMPONENT_NAME: UUCP cmdtab.c
 * 
 * FUNCTIONS: MSGSTR 
 *
 * ORIGINS: 10  26  27 
 *
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

/* static char sccsid[] = "cmdtab.c	5.3 (Berkeley) 5/5/86"; */

#include "tip.h"

extern	int shell(), getfl(), sendfile(), chdirectory();
extern	int finish(), help(), pipefile(), pipeout(), consh(), variable();
extern	int cu_take(), cu_put(), dollar(), genbrk(), suspend();

esctable_t etable[] = {
	{ '!',	NORM,		NULL, shell }, 
	{ '<',	NORM,		NULL, getfl }, 
	{ '>',	NORM,		NULL, sendfile }, 
	{ 't',	NORM,		NULL, cu_take }, 
	{ 'p',	NORM,		NULL, cu_put }, 
	{ '|',	NORM,		NULL, pipefile }, 
	{ '$',	NORM,		NULL, pipeout }, 
	{ 'C',  NORM,		NULL, consh }, 
	{ 'c',	NORM,		NULL, chdirectory }, 
	{ '.',	NORM,		NULL, finish }, 
	{CTRL('d'),NORM,	NULL, finish }, 
	{CTRL('y'),NORM,	NULL, suspend }, 
	{CTRL('z'),NORM,	NULL, suspend }, 
	{ 's',	NORM,		NULL, variable }, 
	{ '?',	NORM,		NULL, help }, 
	{ '#',	NORM,		NULL, genbrk }, 
	{ 0, 0, 0 }
};

char *etable_default[] = {
	"shell",
	"receive file from remote host",
	"send file to remote host",
	"take file from remote UNIX",
	"put file to remote UNIX",
	"pipe remote file",
	"pipe local command to remote host",
	"connect program to remote host",
	"change directory",
	"exit from tip",
	"exit from tip",
	"suspend tip (local+remote)",
	"suspend tip (local only)",
	"set variable",
	"get this summary",
	"send break",
	NULL
};
