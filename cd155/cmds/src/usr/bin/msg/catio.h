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
/* @(#)catio.h	1.4  com/cmd/msg,3.1,9013 10/24/89 11:00:10 */
/*
 * COMPONENT_NAME: CMDMSG
 *
 * FUNCTIONS: catio.h
 *
 * ORIGINS: 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1988, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * (Copyright statements and/or associated legends of other
 * companies whose code appears in any part of this module must
 * be copied here.)
 */

/*
 * Note: There is a duplicate (sort of) of this file in libc.
 * 
 * If you change any values here, make sure the other also gets
 * changed.
 */

#ifndef _BLD	/* bootstrap indicator */
#include <mesg.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>

static void memmove();
static void substitute();
#else
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#endif /* _BLD */

#define ERR 		(-1)
#define TRUE 		1
#define FALSE 		0

#define QTSTR		"$quote"
#define SETSTR		"$set"

#define PATH_FORMAT	"/usr/lib/nls/msg/%L/%N:/etc/nls/msg/%L/%N"
#define DEFAULT_LANG	"C"

#define SETMIN		1
#define SETMAX  	65535

#define skip_to_white(s) 	while (*s && *s > ' ') s++
#define skip_to_nwhite(s) 	while (*s && (*s == ' ' || *s == '\t')) s++
#define die(s)			puts(s), exit(1)
