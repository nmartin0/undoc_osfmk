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
 * $Log: nfsnode.h,v $
 * Revision 1.5  1994/11/18  20:37:22  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:16:57  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:39:08  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:29:50  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:36:04  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:32:54  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:30:53  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:25:08  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:54:08  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:41:04  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.8  90/10/07  14:40:03  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:22:49  gm]
 * 
 * Revision 1.7  90/07/27  09:05:04  devrcs
 * 	NFS parallelization.
 * 
 * 	Condensed history, reverse chronology:
 * 	Parallelized for OSF/1					nags@encore.com
 * 	Add stuff for buffer cache code. Use _KERNEL.		tmt@osf.org
 * 	eof fix; add n_direof* to nfsnode.			gmf@osf.org
 * 	add n_ctime for name cache consistency (resync w/Rick)	gmf@osf.org
 * 	Got rid of #ifdef VOP_PRINT (compatibility)		gmf@osf.org
 * 	New networking code from BSD.				tmt@osf.org
 * 	[90/07/20  17:04:48  nags]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Rick Macklem at The University of Guelph.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)nfsnode.h	7.5 (Berkeley) 10/29/89
 */

/*
 * The nfsnode is the nfs equivalent to ufs's inode. Any similarity
 * is purely coincidental.
 * There is a unique nfsnode allocated for each active file,
 * each current directory, each mounted-on file, text file, and the root.
 * An nfsnode is 'named' by its file handle. (nget/nfs_node.c)
 */

struct nfsnode {
	struct	nfsnode *n_chain[2];	/* must be first */
	nfsv2fh_t n_fh;			/* NFS File Handle */
	long	n_flag;			/* Flag for locking.. */
	struct	vnode *n_vnode;	/* vnode associated with this nfsnode */
	time_t	n_attrstamp;	/* Time stamp (sec) for attributes */
	struct	vattr n_vattr;	/* Vnode attribute cache */
	struct	sillyrename *n_sillyrename;	/* Ptr to silly rename struct */
	daddr_t	n_lastr;	/* Last block read for read ahead */
	u_long	n_size;		/* Current size of file */
	time_t	n_mtime;	/* Prev modify time to maintain data cache consistency*/
	time_t	n_ctime;	/* Prev create time for name cache consistency*/
	int	n_error;	/* Save write error value */
	u_long  n_direofoffset; /* Dir. EOF offset cache */
#ifdef  _KERNEL
	lock_data_t     n_io_lock;      /* reading/writing file's contents */
	udecl_simple_lock_data(,n_incore_lock)
#endif

};

#define	n_forw		n_chain[0]
#define	n_back		n_chain[1]

#ifdef _KERNEL
/*
 * Convert between nfsnode pointers and vnode pointers
 */
#define VTONFS(vp)	((struct nfsnode *)(vp)->v_data)
#define NFSTOV(np)	((struct vnode *)(np)->n_vnode)
#endif
/*
 * Flags for n_flag
 */
#define	NMODIFIED	0x4	/* Might have a modified buffer in bio */
#define	NWRITEERR	0x20	/* Flag write errors so close will know */

/*
 * Locking for the nfsnode:
 *	Field			Locking
 *	-----			-------
 *	n_chain			Hash chain lock
 *	n_fh			incore lock 
 *	n_flag			incore lock
 *	n_vnode			read-only
 *	n_attrstamp		incore lock
 *	n_vattr			incore lock
 *	n_sillyrename		incore lock
 *	n_lastr			incore lock
 *	n_size			I/O lock
 *	n_mtime			incore lock
 *	n_ctime			incore lock
 *	n_error			incore lock
 *	n_direofoffset		incore lock
 */
#define NP_LOCK(np)		usimple_lock(&(np)->n_incore_lock)
#define NP_UNLOCK(np)		usimple_unlock(&(np)->n_incore_lock)
#define NP_LOCK_INIT(np)	usimple_lock_init(&(np)->n_incore_lock)

#define NP_READ_LOCK(np)	lock_read(&(np)->n_io_lock)
#define NP_WRITE_LOCK(np)	lock_write(&(np)->n_io_lock)
#define NP_READ_UNLOCK(np)	lock_read_done(&(np)->n_io_lock)
#define NP_WRITE_UNLOCK(np)	lock_write_done(&(np)->n_io_lock)
#define NP_IO_LOCK_INIT(np)	lock_init2(&(np)->n_io_lock, TRUE, \
					   LTYPE_NFSNODE_IO)

