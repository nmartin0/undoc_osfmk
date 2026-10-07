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
 * Copyright (c) 1988 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * This source file was modified by the Center for High Performance
 * Computing (CHPC) on behalf of OSF.
 */
/*
 * HISTORY
 * $Log: bsd_msg.h,v $
 * Revision 1.6  1995/02/01  22:16:19  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.5  1994/11/18  20:47:03  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:39:35  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:58:25  cfj
 * Adding new code from vendor
 *
 * Revision 1.1.1.3  1993/07/01  20:58:25  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:52:47  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:41:51  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:49:57  cfj
 * Bump major revision number.
 *
 * Revision 2.11  93/06/02  12:33:45  yazz
 * For Sys V IPC under TNC define message skeletons for generic svipc
 * message types.  Also corrected some inaccurate numbers in comments.
 * 
 * 	Revision 2.11  93/01/25  22:56:06  durriya
 * 		remove root_port & start_port from struct vnode_request. vnode_request
 * 		struct has to match forw_request struct                     (durriya)
 *
 * 	Revision 2.10  93/01/06  10:36:49  loverso
 * 		Fix notice.
 *
 * Revision 2.9  92/08/08  01:50:18  jdh
 * added support for both send and receive rights relocation arrays -- jdh
 * 
 * Revision 2.8  92/06/11  16:29:54  mjl
 * Added definitions for pipe/FIFO/socket relocation message that uses
 * out-of-line memory.
 * 
 * Revision 2.7  92/03/01  18:35:30  pjg
 * 	Remove signature ports.  (loverso)
 * 
 * Revision 2.6  92/01/17  17:27:32  roy
 * 	Added transaction id to message formats (loverso).
 * 
 * Revision 2.5  92/01/05  20:24:39  roy
 * 	Added new message types for generic file system system calls and
 * 	generic vproc system calls.  Added new message type for server
 * 	replies to poll()/select() requests from the emulator.  (noemi)
 * 
 * 	1991/09/22  21:53:58  noemi
 * 	OSF1/AD update
 * 
 * Revision 2.4  91/10/04  15:08:14  chrisp
 * Add the Locus copyright notice.
 * 
 * Revision 2.3  91/09/17  09:40:12  sjs
 * integrate Locus changes	roman
 * Added new message types for generic file system system calls and
 * generic vproc system calls.  Added new message type for server
 * replies to poll()/select() requests from the emulator.
 * 
 * Revision 2.2  91/08/31  14:23:38  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.0  91/01/17  12:05:45  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.4  90/06/02  15:26:48  rpd
 * 	Converted to new IPC.
 * 	[90/03/26  20:08:07  rpd]
 * 
 * Revision 2.3  89/11/29  15:30:13  af
 * 	Added rval2, because some syscalls do not modify it and it
 * 	must be preserved.
 * 	[89/11/20            af]
 * 
 * Revision 2.2  89/10/17  11:26:51  rwd
 * 	Added interrupt return parameter.  Removed INTERRUPT
 * 	and ERROR macros.
 * 	[89/09/21            dbg]
 * 
 * $EndLog$
 */
/*
 * Request and reply message for generic BSD kernel call. Also used for
 * vproc system calls.
 */

#ifndef _UXKERN_BSD_MSG_H_
#define _UXKERN_BSD_MSG_H_
#include <mach/boolean.h>
#include <mach/message.h>
#include <uxkern/bsd_types.h>
#ifdef	TNC
#include <tnc/reloc.h>
#endif

struct	bsd_request {
    mach_msg_header_t	hdr;
    mach_msg_type_t	int_type;	/* int[8] */
    int			rval2;
    int			syscode;
    int			arg[6];
};

struct	bsd_reply {
    mach_msg_header_t	hdr;
    mach_msg_type_t	int_type;	/* int[4] */
    int			retcode;
    int			rval[2];
    boolean_t		interrupt;
};

union bsd_msg {
    struct bsd_request	req;
    struct bsd_reply	rep;
    char		msg[8192];
};

#define	BSD_REQ_MSG_ID		100000

#ifdef	OSF1_ADFS
#define	VPROC_REQ_MSG_ID	100001

/*
 * Request and reply message for generic file port related system call
 */

struct	fs_request {
    mach_msg_header_t	hdr;
    mach_msg_type_t	port_type;	/* mach_port_t[1] */
    mach_port_t		cred_port;
    mach_msg_type_t	int_type;	/* int[9] */
    transaction_id_t	transid;
    int			rval2;
    int			syscode;
    int			arg[6];
};

union fs_msg {
    struct fs_request	req;
    struct bsd_reply	rep;		/* "interrupt" portion is unused */
    char		msg[8192];
};

#define	FS_REQ_MSG_ID		100002


/*
 * Delayed reply from server to emulator for poll/select messages.
 */

struct	ps_reply {
	mach_msg_header_t	hdr;
	mach_msg_type_t		int_type;	/* int[2] */
	short			index;		/* index into events/revents */
	short			revents;	/* events as returned */
};

#define POLL_SEL_REPLY_MSG_ID	100003

/*
 * Request and reply message for generic vnode port related system call
 */

struct	vnode_request {
    mach_msg_header_t	hdr;
    mach_msg_type_t	port_type;	/* mach_port_t[1] */
    mach_port_t		cred_port;
    mach_msg_type_t	int_type;	/* int[9] */
    transaction_id_t	transid;
    int			rval2;
    int			syscode;
    int			arg[6];
};

union vnode_msg {
    struct vnode_request	req;
    struct bsd_reply		rep;
    char			msg[8192];
};

#define	VNODE_REQ_MSG_ID	100004

struct forw_request {
    mach_msg_header_t	hdr;
    mach_msg_type_t	port_type;	/* mach_port_t[1] */
    mach_port_t		cred_port;
    mach_msg_type_t	int_type;	/* int[9] */
    transaction_id_t	transid;
    int			rval2;
    int			code;
    int			arg[6];
};

union forw_msg {
    struct forw_request		req;
    struct bsd_reply		rep;
    char			msg[8192];
};

#define	FORW_REQ_MSG_ID		100005

#ifdef	TNC

struct	outofline_region {
    mach_msg_type_long_t	oolr_type;
    vm_offset_t			oolr_offset;
};

struct	reloc_request {
    mach_msg_header_t		hdr;
    mach_msg_type_t		creds_type;
    mach_port_t			creds_port;
    mach_msg_type_t		rcvrts_type;
    mach_port_t			rcvrts[RELOC_PORT_ARRAY_SIZE];
    mach_msg_type_t		sndrts_type;
    mach_port_t			sndrts[RELOC_PORT_ARRAY_SIZE];
    mach_msg_type_t		region_count_type;
    int				region_count;
    struct outofline_region	regions[1];
};

struct	reloc_reply {
    mach_msg_header_t	hdr;
    mach_msg_type_t	int_type;	/* int[2] */
    int			retcode;
    int			status;
    mach_msg_type_t	ret_rcvrts_type;
    mach_port_t		ret_rcvrts[RELOC_PORT_ARRAY_SIZE];
    mach_msg_type_t	ret_sndrts_type;
    mach_port_t		ret_sndrts[RELOC_PORT_ARRAY_SIZE];
};

#define	MIN_RELOC_REPLY_SIZE \
    (sizeof(mach_msg_header_t) + sizeof(mach_msg_type_t) + (sizeof(int)<<1))

union	reloc_msg {
    struct reloc_request	req;
    struct reloc_reply		rep;
    char			msg[8192];
};

#define RELOC_MAXOUTOFLINE \
	((sizeof(union reloc_msg) - \
	  (sizeof(struct reloc_request) - sizeof(struct outofline_region))) \
	 / sizeof(struct outofline_region))

#define RELOC_OOL_REQ_MSG_ID	100006

/*
 * Request and reply message for System V IPC port related system calls.
 */

struct	svipc_request {
    mach_msg_header_t	hdr;
    mach_msg_type_t	port_type;	/* mach_port_t[1] */
    mach_port_t		cred_port;
    mach_msg_type_t	int_type;	/* int[9] */
    transaction_id_t	transid;
    int			rval2;
    int			syscode;
    int			arg[6];
};

union svipc_msg {
    struct svipc_request req;
    struct bsd_reply	rep;		/* "interrupt" portion is unused */
    char		msg[8192];
};

#define	SVIPC_REQ_MSG_ID	100007

#endif	/* TNC */
#endif	/* OSF1_ADFS */
#endif /* _UXKERN_BSD_MSG_H_ */
