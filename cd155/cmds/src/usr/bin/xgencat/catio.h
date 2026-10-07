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
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/* @(#)catio.h	1.4  com/cmd/msg,3.1,8943 10/24/89 11:00:10 */
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


#define ERR 		(-1)
#define TRUE 		1
#define FALSE 		0

#define QTSTR		"$quote"
#define SETSTR		"$set"

#define PATH_FORMAT	"%N::/usr/lpp/msg/%L/%N"
#define DEFAULT_LANG	"En_US"

#define MSGMIN		0
#define MSGMAX 		65535
#define SETMIN		1
#define SETMAX  	65535

#define skip_to_white(s) 	while (*s && *s > ' ') s++
#define skip_to_nwhite(s) 	while (*s && (*s == ' ' || *s == '\t')) s++
#define die(s)			puts(s), exit(1)



#include <mesg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <stdio.h>

/*______________________________________________________________________
	Function declarations for the message tools:
	(standard libc.a)

	malloc() is not declared because its type has to be 
	recast for each use.
  ______________________________________________________________________*/
static void memmove();
static void substitute();
