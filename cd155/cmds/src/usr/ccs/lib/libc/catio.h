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
/* @(#)$RCSfile: catio.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:03:50 $ */
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
 * catio.h	1.6  com/lib/c/msg,3.1,8943 10/24/89 11:48:55
 */


#define ERR 		(-1)
#define TRUE 		1
#define FALSE 		0

#define QTSTR		"$quote"
#define SETSTR		"$set"

#define PATH_FORMAT	"/usr/lib/nls/msg/%L/%N:/etc/nls/msg/%L/%N"
#define DEFAULT_LANG	"C"

#define MSGMIN		0
#define MSGMAX 		65535
#define SETMIN		1
#define SETMAX  	65535

extern int errno;
#define	RETURN(s)	return((errno = errno_save), s)

#define skip_to_white(s) 	while (*s && *s > ' ') s++
#define skip_to_nwhite(s) 	while (*s && (*s == ' ' || *s == '\t')) s++
#define die(s)			puts(s), exit(1)



#include <mesg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
