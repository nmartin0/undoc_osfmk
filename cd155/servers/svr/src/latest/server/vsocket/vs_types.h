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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: vs_types.h,v $
 * Revision 1.15  1995/02/01  23:36:41  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.14  1994/11/18  20:52:49  mtm
 * Copyright additions/changes
 *
 * Revision 1.13  1994/06/15  17:28:04  mjl
 * Break race between close of remote virtual socket (MARK_VSOCK_CLOSING() macro)
 * and asynchronous status checking RPCs (PORT_TO_VSOCK_LOOKUP_ASYNC() macro).
 *
 *  Reviewer: Bob Yasi <yazz@locus.com>, Charlie Johnson <cfj@ssd.intel.com>
 *  Risk: Medium
 *  Benefit or PTS #: 9024
 *  Testing: NFS mounts on HiPPI configurations now succeed.
 *  Module(s):
 * 	server/sys/socketvar.h
 * 	server/vsocket/vs_types.h
 * 	server/vsocket/vs_ipc.c
 * 	server/vsocket/vs_subr.c
 * 	server/vsocket/vs_netops.c
 * 	server/vsocket/sys_vsocket.c
 *
 * Revision 1.12  1994/05/04  22:28:14  mjl
 * Merge revision 1.11.4.1 from R1_2 branch into main trunk.
 *
 * Revision 1.11.4.1  1994/04/27  23:11:07  yazz
 *  Reviewer: Charlie Johnson, Bob Yasi
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
 * TNC select rewrite.  Prefix "rvs_" to field names of vs_socket_t
 * structs.  Check magic number in port-to-socket-address translations.
 *
 * Revision 1.11  1993/09/01  01:42:10  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.10  1993/07/14  18:49:54  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:15:04  cfj
 * Adding new code from vendor
 *
 * Revision 1.9  1993/05/17  19:08:06  cfj
 * 05-06-93 MI driver drop from Locus.
 *
 * Revision 3.19  93/08/31  09:32:03  mjl
 * [LCC #0386] VS_MAXIOCLEN may be safely set to IOCPARM_MAX, since
 * 4k bytes will safely fit in the 8k message buffers allocated by
 * ux_server_loop().
 * 
 * Revision 3.18  93/08/26  11:11:55  mjl
 * Use better field names for option history entries.  Move constants from
 * obsoletel <vsocket/ins_var.h> here.
 * 
 * Revision 3.17  93/08/22  09:42:58  bhk
 * added a lenght field to the vs_opts structure [#358]
 * 
 * Revision 3.16  93/08/16  19:07:55  bhk
 * split getstate into synchronous and asynchrous routines [#346]
 * 
 * Revision 3.15  93/08/11  09:16:33  bhk
 * 
 * Moved the vs_index flag.  Added VS_READ_CHECK
 * 
 * Revision 3.14  93/08/06  18:00:28  bhk
 * Added readable flag to allow caching of ability to read remote sockets
 * 
 * Revision 3.13  93/05/07  15:39:18  nina
 * Added port_list_t to support extension of chouse_list() interface 
 * 
 * Revision 3.12  93/05/05  22:50:55  mjl
 * Delete obsolete typedefs.
 * 
 * Revision 3.11  93/05/04  16:49:24  bhk
 * Fixed RCS Comments
 * 
 * Revision 3.10  93/05/03  14:51:04  bhk
 * added vs_ostate and vs_flags for state sync bug(245),
 * added VS_USE constant  for multiserver bind/connect bug (242)
 * 
 * Revision 3.9  93/04/12  16:01:05  nina
 * Added node_list_t to support if_list RPC
 * 
 * Revision 3.8  93/02/25  17:56:45  nina
 * Add node field to vs_socket_t remote virtual socket list elements.  (mjl)
 * 
 * Revision 3.7  93/02/09  18:21:53  mjl
 * Move clearinghouse data definitions to vs_chouse.h .
 * 
 * Revision 3.6  93/01/04  20:45:54  bhk
 * Fixed type of vs_optstring_t to allow a clean compile
 * 
 * Revision 3.5  92/10/07  16:22:01  bhk
 * Fixed server_list_t typedef to reduce compiler warning messages
 * 
 * Revision 3.4  92/09/24  17:19:07  bhk
 * added field to hold node number for network server interface
 * registration
 * 
 * Revision 3.3  92/08/08  02:01:32  jdh
 * renamed a typedef -- sockaddr_in_t conflicts with a
 * declaration fsvr_types.h -- jdh
 * 
 * Revision 3.2  92/07/26  17:58:05  bhk
 * added internet sockaddr type
 * 
 * Revision 3.1  92/06/16  18:50:48  bhk
 * added state fields for multi-servers
 * 
 * Revision 3.0  92/04/20  17:23:16  bhk
 * Genesis
 * 
 * 
 */

#ifndef	_VS_TYPES_H_
#define	_VS_TYPES_H_

#include <sys/ioctl.h>
#include <kern/macro_help.h>

#define	NSRV_ADDR_EXISTS	-1992
#define	NSRV_ADDR_NOT_FOUND	-1993
#define	VS_NUM_ERRORS		200
#define	VS_SYSCALL_NUM		-1000

typedef struct vs_socket {
	struct vs_socket	*rvs_next;		/* list pointer */
	mach_port_t		rvs_server_port;	/* remote socket port */
	node_t			rvs_server_node;	/* remote socket node */
	struct socket		*rvs_remote_so;		/* remote socket addr */
	int			rvs_flags;
	int			rvs_state_index;
	int			rvs_read_index;
	int			rvs_state;
	int			rvs_ostate;
	int			rvs_soerror;
	int			rvs_qlen;
	struct socket		*rvs_so;		/* backptr to primary */
} vs_socket_t;

/*
 *  Flag values for rvs_flags
 */
#define	VS_USE			0x01	/* secondary socket is useable */
#define	VS_READABLE		0x02	/* the secondary socket is readable */
#define	VS_READ_CHECK		0x04	/* secondary has an outstanding */
					/*  check for readability */
#define	VS_STATE_CHECK		0x08	/* secondary has an outstanding */
					/*  state check */

/*
 *  Structure for remembering arguments to a setsockopt() or ioctl() call.
 */
typedef struct	vs_opts {
	struct vs_opts	*vo_next;
	int		vo_type;
	int		vo_level;
	int		vo_optname;
	char		*vo_value;	/* base addr of option data */
	int		vo_len;		/* length of option data */
} vs_opts_t;

/* Values for vo_type */
#define	VS_OPT_OPT		0x01
#define	VS_OPT_IOCTL		0x02

/*
 *  Constants and typedefs used by vs.defs MiG stubs.
 *
 *  These typedefs should never be used to allocate memory in
 *  the server; they are used solely to make MiG stubs happy.
 */
#define	VS_MAXOPTLEN	16
typedef	char		vs_optstring_t[VS_MAXOPTLEN];

#define VS_MAXIOCLEN	IOCPARM_MAX
typedef char		ioctl_data_t[VS_MAXIOCLEN];

#define	MAX_SERVERS	100
typedef node_t		node_list_t[MAX_SERVERS];

/*
 *  Miscellaneous
 */

typedef	int			(*match_func_t)();
typedef	struct sockaddr		*sockaddr_t;
typedef struct sockaddr_in	*sockaddr_in_p;
typedef mach_port_t		*server_list_t; 
typedef mach_port_t		*port_list_t;

/* ZZZ Won't someone please move all non-MIG stuff somewhere else? */

#define VSOCK_TO_PORT_LOOKUP(vs, port)				\
	(port) = ((mach_port_t)vs)

#define	PORT_TO_VSOCK_LOOKUP(p, vs)				\
MACRO_BEGIN							\
	(vs) = ((struct socket *)(p));				\
	if ((vs)->vs_magic != VS_MAGIC)				\
		panic("PORT_TO_VSOCK_LOOKUP: port %x", (p));	\
MACRO_END

/*
 *  This macro is called by the server sides of asynchronous RPCs
 *  (simpleroutines) sent from primary to secondary sockets.  It
 *  insures that a racing r_vs_soclose() RPC won't zero the
 *  so->so_lock pointer before the async RPC can take the SOCKET_LOCK.
 *  After this macro executes, `vs' will be either NULL or a locked
 *  socket pointer.
 */
#define PORT_TO_VSOCK_LOOKUP_ASYNC(p, vs)			\
MACRO_BEGIN							\
	(vs) = ((struct socket *)(p));				\
	if ((vs)->vs_magic != VS_MAGIC)	{			\
		/* Quick check: did we arrive *very* late? */	\
		(vs) = NULL;					\
	} else {						\
		mutex_lock(&(vs)->vs_close_mutex);		\
		if ((vs)->vs_magic != VS_MAGIC ||		\
		    (vs)->vs_is_closing) {			\
			mutex_unlock(&(vs)->vs_close_mutex);	\
			(vs) = NULL;				\
		} else {					\
			(vs)->vs_defer_close = TRUE;		\
			mutex_unlock(&(vs)->vs_close_mutex);	\
			SOCKET_LOCK(vs);			\
			(vs)->vs_defer_close = FALSE;		\
			condition_broadcast(&(vs)->vs_can_close); \
		}						\
	}							\
MACRO_END

/*
 *  Atomically mark this secondary socket as closing, possibly waiting
 *  for an asynchronous RPC to grab the SOCKET_LOCK first (see the
 *  PORT_TO_VSOCK_LOOKUP_ASYNC() macro above).  This is called by the
 *  close RPC prior to taking the SOCKET_LOCK itself.
 */
#define MARK_VSOCK_CLOSING(vs)					\
MACRO_BEGIN							\
	mutex_lock(&(vs)->vs_close_mutex);			\
	while ((vs)->vs_defer_close)				\
		condition_wait(&(vs)->vs_can_close,		\
			       &(vs)->vs_close_mutex);		\
	(vs)->vs_is_closing = TRUE;				\
	(vs)->vs_flags |= VS_CLOSING;				\
	mutex_unlock(&(vs)->vs_close_mutex);			\
MACRO_END


#define	VS_SEND_DATA	0x01
#define	VS_SEND_ADDR	0x02
#define	VS_SEND_CONTROL	0x04
#define VS_SEND_TOP	0x08

struct socket;
extern	void	enqueue_vs_opts(struct socket *, vs_opts_t *);

#endif	/* _VS_TYPES_H_ */
