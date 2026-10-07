/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright (c) 1992-1995, Locus Computing Corporation
 * All rights reserved
 */
/* 
 * HISTORY
 * $Log: un_dg.h,v $
 * Revision 1.6  1995/02/01  22:02:19  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.5  1994/11/18  20:44:42  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:35:37  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:49:25  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/17  19:07:07  cfj
 * 05-06-93 MI driver drop from Locus.
 *
 * Revision 1.2  1993/05/06  19:27:11  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:47:51  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 3.1  93/05/07  19:07:29  mjl
 * Make UN_DG_DEBUG conditional on MACH_ASSERT.
 * 
 * Revision 3.0  1992/12/10  16:59:04  mjl
 * Definitions for TNC Unix domain datagram sockets.
 *
 */

#ifndef	_TNC_UN_DG_H
#define _TNC_UN_DG_H

#include <mach_assert.h>

/*
 *  TNC definitions for AF_UNIX SOCK_DGRAM code.
 */

#if	MACH_ASSERT
#define UN_DG_DEBUG	1
#endif

#ifdef	UN_DG_DEBUG
extern int	un_dg_disable;
extern int	undgdebug;

#define UNDGDEBUG(mask,args)	if ((undebug|undgdebug) & (mask)) printf args

#define UNDGPORTINFO(port,str)				\
MACRO_BEGIN						\
	if ((undebug|undgdebug) & U_PORT)		\
		print_port_info( (port), (str));	\
MACRO_END

struct {
	int	xrefwait;	/* had to wait for non-file-port vnode op */
	int	halfopen;	/* couldn't kill rss due to half opens */
	int	portcreate;	/* # of ports created by set_redirect */
	int	portdestroywait;/* # of waits on nms prior to portdestroy */
	int	portdestroy;	/* # of ports destroyed by clr_redirect */
	int	enable_redirect;	/* # of calls to set_redirect */
	int	disable_redirect;	/* # of calls to clr_redirect */
} un_dg_statistics;
#define UNDGSTAT(field)	un_dg_statistics.field += 1

#else	/* ! UN_DG_DEBUG */

#define UNDGDEBUG(mask,args)
#define UNDGPORTINFO(port,str)
#define UNDGSTAT(field)

#endif	/* ! UN_DG_DEBUG */

#endif	/* ! _TNC_UN_DG_H */

