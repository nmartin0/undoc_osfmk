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
 * $Log: un_debug.h,v $
 * Revision 1.7  1995/02/01  22:01:31  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.6  1994/11/18  20:44:41  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/05/04  21:46:56  mjl
 * TNC select rewrite.  Change name of a macro arg to avoid a compiler warning.
 *
 *  Reviewer: Charlie Johnson (Intel), Bob Yasi (Locus)
 *  Risk: Medium
 *  Benefit or PTS #: #7537 + select rewrite
 *  Testing: VSX, EATS, bobtest, Eval
 *  Module(s):
 * 	server/bsd/subr_select.c
 * 	server/sys/select.h
 * 	server/sys/socketvar.h
 * 	server/sys/user.h
 * 	server/tnc/un_debug.c
 * 	server/tnc/un_debug.h
 * 	server/uxkern/bsd_2.defs
 * 	server/uxkern/bsd_server_side.c
 * 	server/uxkern/fsvr.defs
 * 	server/uxkern/fsvr2_server_side.c
 * 	server/uxkern/fsvr_port.c
 * 	server/uxkern/fsvr_subr.c
 * 	server/uxkern/port_hash.c
 * 	server/uxkern/port_hash.h
 * 	server/vsocket/mi_config.c
 * 	server/vsocket/sys_vsocket.c
 * 	server/vsocket/two_way_hash.h
 * 	server/vsocket/vs.defs
 * 	server/vsocket/vs_chouse.c
 * 	server/vsocket/vs_debug.c
 * 	server/vsocket/vs_init.c
 * 	server/vsocket/vs_ipc.c
 * 	server/vsocket/vs_netops.c
 * 	server/vsocket/vs_subr.c
 * 	server/vsocket/vs_subr.h
 * 	server/vsocket/vs_types.h
 * 	server/vsocket/vsocket.h
 *
 * Revision 1.4  1993/07/14  18:35:34  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:49:22  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/17  19:07:05  cfj
 * 05-06-93 MI driver drop from Locus.
 *
 * Revision 1.2  1993/05/06  19:27:07  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:47:50  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 3.0  1992/12/10  16:58:40  mjl
 * Debugging macros and definitions for TNC Unix domain sockets.
 *
 */

#ifndef	_TNC_UN_DEBUG_H
#define	_TNC_UN_DEBUG_H

#include <uxkern/import_mach.h>
#include <sys/types.h>
#include <mach_assert.h>

/*
 *  Debugging and statistics gathering for TNC Unix domain socket code.
 */

#if	MACH_ASSERT
#define UN_DEBUG	1
#endif

#ifdef	UN_DEBUG
extern int	un_si_alloced;
extern int	un_si_freed;
extern int	un_enable_relocation;

/*
 *  These debug flags are used valid for *all* unXXXdebug patch
 *  variables.  When set in "undebug", all related UNxxxDEBUG()
 *  messages print; when set only in, e.g., "undgdebug", only
 *  UNDGDEBUG() messages print.  See, for example, <tnc/un_dg.h>.
 */
#define U_SEQNO		0x0001
#define U_RELOC		0x0002
#define U_NOTIFY	0x0004
#define U_POLICY	0x0008
#define U_SETERROR	0x0010
#define U_PORT		0x0020
#define U_BINDPORT	0x0040
#define U_HALFOPEN	0x0080
#define U_SGD		0x0100
#define U_INFO		0x0200
#define U_NAMEI		0x0400
#define U_RPC		0x0800
#define U_VECTOR	0x1000

extern node_t	this_node;
extern int	undebug;
extern void print_port_info(mach_port_t, char *);

#define UNDEBUG(mask,args)				\
MACRO_BEGIN						\
	if (undebug & (mask)) {				\
		printf("[node %d] ", this_node);	\
		printf args ;				\
	}						\
MACRO_END

#define UNPORTINFO(p,str)						\
MACRO_BEGIN								\
	if (undebug & U_PORT) {						\
		printf("[node %d, port 0x%x]\n", this_node, (p));	\
		print_port_info((p), (str));				\
	}								\
MACRO_END

#define PRINT_POLICY(ps,str)						    \
MACRO_BEGIN								    \
	if (undebug & U_POLICY)					    	    \
	    printf("policy @ 0x%x [%s]: pid %d, cur %d, new %d\n",	    \
		   ps, str, ps->ps_reader, ps->ps_rdrnode, ps->ps_newnode); \
MACRO_END

#else	/* ! UN_DEBUG */

#define UNDEBUG(mask,args)
#define UNPORTINFO(port,str)
#define PRINT_POLICY(ps,str)

#endif	/* ! UN_DEBUG */

#endif	/* ! _TNC_UN_DEBUG_H */
