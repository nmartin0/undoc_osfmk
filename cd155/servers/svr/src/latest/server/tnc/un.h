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
 * $Log: un.h,v $
 * Revision 1.11  1995/02/01  22:00:17  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.10  1994/11/18  20:44:37  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1993/09/01  01:37:00  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.8  1993/07/14  18:35:23  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:49:12  cfj
 * Adding new code from vendor
 *
 * Revision 1.7  1993/05/06  19:26:53  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:47:47  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.6  1993/04/03  03:09:49  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.5  1993/03/25  23:26:26  cfj
 * T9 Merge.
 *
 * Revision 1.4.4.1  1993/03/24  23:42:34  cfj
 * Locus 03-22-93 vsocket drop to fix select().
 *
 * Revision 1.1.2.1.2.2  1993/02/16  20:06:34  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.4  1993/01/15  02:03:10  cfj
 * Multiple service partition fixes from Locus.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  06:03:25  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 3.5  93/08/23  00:06:22  mjl
 * Use ANSI prototypes to get rid of i860 warnings in files including this.
 * 
 * Revision 3.4  93/03/03  17:14:03  mjl
 * Definition of INVALID_NODE moved to <sys/types.h>.
 *
 * Revision 3.3  92/12/10  17:15:50  mjl
 * Move debug stuff to un_debug.[ch] .  Move *_bindport_*() routines to
 * un_port_hash.c --- don't call routines from this file directly, ONLY use
 * macros in un_port_hash.h !!!
 * 
 * Revision 3.2  92/11/18  12:46:03  mjl
 * Prototypes for i860 compiler happiness.
 * 
 * Revision 3.1  92/09/28  13:28:16  klh
 * Moved relocation policy related definitions here from <tnc/reloc.h>.
 * Reorganized the file a bit, keeping related definitions together. (klh for mjl).
 * 
 * Revision 3.0  92/08/08  01:20:05  jdh
 * changes and new files for Unix domain stream sockets -- jdh
 * 
 * Revision 3.2  92/07/08  09:14:06  roman
 * Change node numbers to type node_t.
 * 
 * Revision 3.1  92/06/30  17:19:03  mjl
 * Reference count the sockinfo structure.
 * 
 * Revision 3.0  92/06/26  17:55:33  mjl
 * Rename ust_XXX to un_pp_XXX.
 * 
 * Revision 3.1  92/06/11  16:13:22  mjl
 * Use macro_help.h macros.  Define new debug bits.
 * 
 * Revision 3.0  92/04/06  10:38:57  mjl
 * Common definitions for AF_UNIX stream sockets.
 * 
 */

#ifndef	_TNC_UN_H
#define _TNC_UN_H

#include <kern/macro_help.h>
#include <tnc/un_port_hash.h>
#include <tnc/un_debug.h>

/*
 *  TNC definitions for AF_UNIX SOCK_STREAM code.
 */

/*
 *  Common relocation policy data and routines.
 *
 *  Right now there is only one policy.  The policy state structure
 *  should only be modified under some appropriate lock (which one
 *  depends on what the info is attached to).
 */
typedef struct un_policy_state {
	pid_t		ps_reader;	/* process to follow when relocating */
	int		ps_rdrnode;	/* current execution node of reader */
	int		ps_newnode;	/* node to relocate to, if any */
	int		ps_refcnt;
	int		ps_flag;
} policy_state_t;

/* policy flags */
#define PLCY_HEAP	0x01		/* was allocated from heap */

extern int	un_policy(policy_state_t *, pid_t, node_t);
extern int	un_policy_alloc(policy_state_t **);
extern int	un_policy_free(policy_state_t **);

extern node_t this_node;


/*
 *  Policy state definitions for backward compatibility with
 *  older usage style.
 */
#define si_reader	ps_reader
#define si_rdrnode	ps_rdrnode
#define si_newnode	ps_newnode
#define si_refcnt	ps_refcnt
typedef policy_state_t un_sock_t;

extern int	un_alloc_sockinfo();
extern int	un_free_sockinfo();

/* XXX Make these macros call "modern" versions of these routines. */
#define	ALLOCATE_UN_SOCKINFO(vs)	un_alloc_sockinfo(vs)
#define	GET_UN_SOCKINFO(vs)		((un_sock_t *)(vs)->vs_data)
#define	FREE_UN_SOCKINFO(vs)		un_free_sockinfo(vs)

/*
 *  Miscellaneous common definitions and routines.
 */

#define	KEEPLOCK	1
#define	DISCARDLOCK	0

#define	ENTER_UN_VSOP(s,k,err)			\
MACRO_BEGIN					\
	SOCKET_LOCK(s);				\
	err = 0;				\
	if ( (s)->vs_flags & VS_RESTART )	\
		(err) = ERESTART;		\
	if ( (k) == DISCARDLOCK )		\
		SOCKET_UNLOCK(s);		\
MACRO_END

#define LEAVE_UN_VSOP(s)	/*no-op*/

extern struct socket *sopartner(struct socket *);
extern int remote_append_dgram(struct socket *,
			       struct mbuf *,
			       struct mbuf *);
extern void un_dg_disconnect(struct unpcb *);

#endif	/* ! _TNC_UN_H */
