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
/***
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: local.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:18:18 $";
#endif
***/
/* 
 * COMPONENT_NAME: CMDMAILX local.h
 * 
 * FUNCTIONS: dup2, gethostname, gtty, stty, vfork 
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


#include <sys/utsname.h>
#include "v7.local.h"

#define vfork() fork()
#define dup2(a,b) close(b);dup(a)

#define sgttyb termio
#define gtty(a,b) ioctl(a, TCGETA, b)
#define stty(a,b) ioctl(a, TCSETA, b)
#define sg_ospeed c_cflag & CBAUD
#define sg_erase c_cc[2]
#define sg_kill c_cc[3]
