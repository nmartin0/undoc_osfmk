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
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: mf.h,v $
 * Revision 1.7  1994/11/18  20:48:44  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1993/07/14  18:42:57  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:04:08  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  19:30:59  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.4  1993/04/03  03:12:19  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  06:05:32  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  03:06:33  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:55:18  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:44:01  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.7  92/11/05  17:27:41  roy
 * 	Added mscount to token info struct.
 * 	[92/10/29            roy]
 * 
 * Revision 2.7  1992/11/05  17:27:41  roy
 * 	Added mscount to token info struct.
 * 	[92/10/29            roy]
 *
 * Revision 2.6  92/10/05  12:09:09  rabii
 * 	token_info structure now has a lock for the whole structure,
 * 	not just for port-related fields.
 * 	[92/10/01            roy]
 * 
 * Revision 2.5  92/06/08  18:26:46  pjg
 * 	Moved definition of macro MF_MAPPABLE to vnode.h (pjg).
 * 
 * Revision 2.4  92/05/18  12:27:35  roy
 * 	Revision 2.3.1.2  92/05/08  12:07:48  roy
 * 	Added token states and new fields to the token info struct.
 * 	[92/05/08            roy]
 * 
 * 	Revision 2.3.1.1  92/04/22  10:04:09  roy
 * 	Added tok_released condition to token_info structure.
 * 	[92/03/31            roy]
 * 
 * Revision 2.3  92/03/15  14:30:09  roy
 * 	92/03/03  16:49:12  roy
 * 	Add vnode pointer to token structure.
 * 
 * 	92/02/19  10:45:05  roy
 * 	Define TOK_MAGIC.
 * 
 * Revision 2.2  91/12/10  22:17:17  roy
 * 	91/11/19  20:13:26  roy
 * 	Improved version.
 * 
 * Revision 2.1  91/11/16  17:27:08  roy
 * Created.
 * 
 * $EndLog$
 */

#ifndef	_SYS_MF_H_
#define _SYS_MF_H_

#include <mapped_files.h>
#include <sys/kernel.h>
#include <sys/file.h>
#include <sys/vnode.h>
#include <sys/unix_defs.h>
#include <kern/lock.h>
#include <kern/queue.h>

/*
 * Valid states for a token info structure.
 */
#define	T_FREE		0x0		/* free */
#define T_ACQ		0x0001		/* acquired */
#define T_REL		0x0002		/* released */
#define T_ACQ_AND_RIP	0x0004		/* acquired and revoke-in-progress */
#define T_REL_AND_RIP	0x0008		/* released and revoke-in-progress */

typedef struct token_info {
	long		tok_magic; 	/* must be first */
	struct mutex	lock;		/* lock protecting token_info fields */
	int		refcnt;		/* reference count */
	int		flags;		/* bits indicating tokens held */
	int		nms_waiting:1;	/* bits for synchronizing no-senders */
	u_long		seqno;		/* message sequence number */
	u_long		mscount;	/* count of send rights ever created */
	short		state;		/* state of the token info structure */
	short		send_count;	/* count of send rights outstanding */
	struct file	*fp;		/* pointer to file struct */
	struct vnode	*vp;		/* pointer to vnode */
	mach_port_t	revoke_port;    /* port used to revoke tokens */
	struct condition tok_released;  /* signalled when a token is released */
} token_info_t;


#define	TOK_MAGIC	0x080161	/* magic number for token_info struct */

/*
 * Token flag bits.  NOTE:  these values are also known by the emulator!
 */
#define	TOK_OFFSET	0x01
#define TOK_DATA_READ	0x02
#define TOK_DATA_WRITE	0x04

#define TOK_ALL         (TOK_OFFSET | TOK_DATA_READ | TOK_DATA_WRITE)

#define TINFO_TO_PORT(tip)	((mach_port_t) tip)
#define PORT_TO_TINFO(port)	(((token_info_t *)port)->tok_magic == TOK_MAGIC ? \
				 (token_info_t *)port : NULL)

#endif	/* _SYS_MF_H_ */
