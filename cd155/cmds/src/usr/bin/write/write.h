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
/* @(#)$RCSfile: write.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:50:07 $ */
/*
 * COMPONENT_NAME: (CMDCOMM) user to user communication
 *
 * FUNCTIONS: 
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
 */
/* write.h	1.3  com/cmd/comm/write,3.1,9021 2/9/90 16:34:35 */

#define MAX_USERID_LEN 32 /* same as in struct utmp declared in utmp.h */
#define MAX_HOST_LEN 64  /* as defined in ping.c */
#define MAX_SER_LEN 10
#define DATE_LEN 30
#define MAX_TTY_LEN 32 /* same as in struct utmp declared in utmp.h */

#define RELAY  '0'
#define RWRITE '1'
#define HWRITE '2'
#define QUERY  '3'

#define OK 0
#define CANCEL 1
#define MQUERY 2

#define SOK "ok"
#define SCANCEL "cancel"
#define SQUERY "query"

/* common control characters */
#define WRT_BELL	7  
#define WRT_NEWLINE	10	

/* error codes from writesrv */
#define NOTLOG 0
#define NOTTY -1
#define NOPERM -2
#define NOOPEN -3
#define MALLOC -4
#define BADHAND -5
#define SNDRPLY -6
#define GETRPLY -7 
#define NOSERVICE -8

struct ttys {
	char tty[PATH_MAX];
	struct ttys *next;
};
