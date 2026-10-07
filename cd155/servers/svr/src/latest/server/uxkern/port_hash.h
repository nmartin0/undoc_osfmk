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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * This source file was modified by the Center for High Performance
 * Computing (CHPC) on behalf of OSF.
 */
/*
 * HISTORY
 * $Log: port_hash.h,v $
 * Revision 1.8  1994/11/18  20:49:02  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/06/17  23:19:37  jlitvin
 * Remove embedded comment character to make lint happier.
 *
 * Revision 1.6  1994/05/04  22:07:00  mjl
 * TNC select rewrite.  Added port_hash_walk() routine to walk thru the
 * contents of a hash table.  Useful for debugging.
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
 * Revision 1.5  1993/07/14  18:43:32  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:04:56  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/05/06  19:31:48  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.3  1993/03/29  23:07:32  nandy
 * Merged from T9 branch
 *
 * Revision 1.2.8.1  1993/03/29  22:48:23  nandy
 * Changes from loverso to fix the OIP intr problem.
 *  Revision 2.9  93/03/24  15:38:41  loverso
 *  Reflect new and deleted functions in port_hash.c. (loverso)
 *
 * Revision 2.9  93/03/24  15:38:41  loverso
 * 	Reflect new and deleted functions in port_hash.c. (loverso)
 * 
 * Revision 2.8  93/01/06  10:37:03  loverso
 * 	Fix notice.
 * 
 * Revision 2.9  1993/03/24  15:38:41  loverso
 * 	Reflect new and deleted functions in port_hash.c. (loverso)
 *
 * Revision 2.8  93/01/06  10:37:03  loverso
 * 	Fix notice.
 * 
 * Revision 2.7  92/05/12  00:04:51  loverso
 * 	Add extern decl's to match new functions.  (loverso)
 * 
 * Revision 2.6  92/05/01  10:03:29  rabii
 * 	Add "subchain" to mpportid.(loverso)
 * 
 * Revision 2.5  92/03/15  14:30:51  roy
 * 	Added PORTID_HASH
 * 
 * Revision 2.4  92/03/03  13:51:19  pjg
 * 	Add support for mpportid_* routines (loverso).
 * 
 * Revision 2.3  92/01/05  20:16:40  roy
 * 	Revision 1.2  1991/10/14  20:53:13  noemi
 * 	Added data structures for port hash tables with locks and extern 
 * 	function declarations.
 * 
 * Revision 2.2  91/08/31  14:27:12  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/06/25  17:13:16  condict
 * Moved sys header files that were from OSF/1 kern dir, back to kern.
 * 
 * Revision 3.0  91/01/17  12:06:13  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.2  90/06/02  15:28:09  rpd
 * 	Converted to new IPC.
 * 	[90/03/26  20:21:33  rpd]
 * 
 */

#ifndef _UXKERN_PORT_HASH_H_
#define _UXKERN_PORT_HASH_H_
/*
 * Port-to-pointer hash routines.
 */
#include <kern/queue.h>
#include <uxkern/import_mach.h>

struct port_hash_entry {
	queue_chain_t	chain;
	mach_port_t	port;
	char *		value;
};
typedef struct port_hash_entry *port_hash_entry_t;

struct port_hash_table {
	int		length;
	queue_head_t	head[1];	/* variable size */
};
typedef struct port_hash_table *port_hash_table_t;

#define	PORT_HASH(port)	(port)

extern port_hash_table_t	port_hash_init();
extern boolean_t		port_hash_enter();
extern char *			port_hash_lookup();
extern boolean_t		port_hash_remove();
#if	MACH_ASSERT
extern int			port_hash_walk();
#endif

#ifdef	OSF1_ADFS
/*
 * Data structures for port hash tables with locks.
 */
struct mpport_hash_table {
	int		length;
	mpqueue_head_t	mphead[1];	/* variable size */
};
typedef struct mpport_hash_table *mpport_hash_table_t;

extern mpport_hash_table_t	mpport_hash_init();
extern boolean_t		mpport_hash_enter();
extern char *			mpport_hash_lookup();
extern char *			mpport_hash_lookup_lock();
extern void			mpport_hash_unlock();
extern boolean_t		mpport_hash_remove();


/*
 * Data structures for port hash tables with locks
 * and additional IDs.  Multiple threads can add the same port/id pair.
 *
 * Bucket is chosen by hash on port.  An additional id is used to
 * uniqify the entry.
 */

struct portid_hash_entry {
	queue_chain_t		chain;
	mach_port_t		port;
	unsigned long		id;
	char			*value;
	struct portid_hash_entry *subchain;
};
typedef struct portid_hash_entry *portid_hash_entry_t;
 
struct mpportid_hash_table {
	int		length;
	mpqueue_head_t	head[1];	/* variable size */
};
typedef struct mpportid_hash_table *mpportid_hash_table_t;

extern mpportid_hash_table_t	mpportid_hash_init();
extern boolean_t		mpportid_hash_enter_sub();
extern void			mpportid_hash_enter();
extern char *			mpportid_hash_lookup();
extern char *			mpportid_hash_lookup_func();
extern int			mpportid_hash_foreach_func();
extern boolean_t		mpportid_hash_remove();

#endif	/* OSF1_ADFS */
#endif _UXKERN_PORT_HASH_H_
