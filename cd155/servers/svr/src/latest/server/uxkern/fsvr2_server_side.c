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
 * $Log: fsvr2_server_side.c,v $
 * Revision 1.12  1995/02/01  22:19:21  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.11  1994/11/18  20:47:53  mtm
 * Copyright additions/changes
 *
 * Revision 1.10  1994/05/07  22:09:02  yazz
 * Merged R1.2 revision 1.7.4.2 into main stem.
 *
 * Revision 1.9  1994/05/04  21:55:25  mjl
 * TNC select rewrite.  The fsvr_file_unref_svr() RPC now returns the
 * file port send right to the primary socket's node.
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
 * Revision 1.7.4.2  1994/04/27  22:10:45  yazz
 *  Reviewer: Charlie Johnson
 *  Risk: Lo
 *  Benefit or PTS #: GUBT
 *  Testing: VSX, EATS
 *  Module(s): server/uxkern/fsvr2_server_side.c
 *
 * Init MIG OUT params for deallocatable Out-Of-Line (OOL) memory to null,
 * lest randomly specified pages of VM be accidentally transmitted in the
 * reply and unintentionally deallocated out from under the server.
 *
 * Revision 1.8  1994/01/05  17:18:03  brad
 * Fixed lint warnings in PFS-related code.
 *
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: Some PFS source now passes lint
 *  Testing: Ran PFS EATs
 *  Module(s): emulator/emul_callback.c
 *             emulator/fsvr_user_side.c
 *             emulator/pfs2_user_side.c
 *             emulator/pfs_emath.c
 *             emulator/pfs_fdt.h
 *             emulator/pfs_iomode.c
 *             emulator/pfs_tokenmgt.c
 *             emulator/pfs_user_side.c
 *             server/uxkern/fsvr.defs
 *             server/uxkern/fsvr2.defs
 *             server/uxkern/fsvr2_server_side.c
 *             server/uxkern/fsvr_types.defs
 *             server/uxkern/pfs2.defs
 *
 * Revision 1.7  1993/09/01  01:38:13  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 2.5  93/08/10  15:42:37  slk
 * Moved S_fsvr_file_unref_svr from fsvr_server_side.c to fsvr2_server_side.c
 * because fsvr_server_side.c is not built with lite servers, and file_unref_svr
 * is needed for sockets.
 * 
 * Revision 1.6  1993/07/19  23:00:18  robboy
 * Integrate OSF/Locus Lite server changes
 *
 * Revision 1.5  1993/07/14  18:41:30  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:01:25  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/06/17  01:01:22  brad
 * Added comment explaining VOP_DATAIN semantics.
 *
 * Revision 1.3  1993/05/06  19:27:34  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:54:11  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:43:01  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:52:12  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1992/07/28  19:59:52  rabii
 *
 * Revision 2.4  1993/03/30  16:08:04  roy
 * 	Rewrote.
 * 	[93/02/16            roy]
 *
 * Revision 2.3  92/07/28  19:59:52  rabii
 * 	Add S_fsvr_write2 (pjg).
 * 
 * Revision 2.2  92/06/08  18:29:21  pjg
 * 	Created.
 * 
 */

/*
 * Server-side operations for fast path IO.
 */

#include <uxkern/syscall_subr.h>
#include <uxkern/import_mach.h>
#include <uxkern/fsvr.h>
#include <uxkern/syscalltrace.h>
#include <uxkern/bsd_msg.h>
#include <sys/vnode.h>
#include <sys/errno.h>
#include <sys/file.h>
#include <fullserver.h>

#if	FULLSERVER

S_fsvr_read_at_offset(file_port, creds_port, response_id, offset, count, 
		      data, amount)
	mach_port_t		file_port;
	mach_port_t		creds_port;
	unsigned int		*response_id;
	off_t			offset;
	unsigned int		count;
	char			**data;		/* out, dealloc (ool memory) */
	int			*amount;	/* out, size of the above */
{
	register int	error;
	struct file 	*fp;
	struct uio	uio;
	struct iovec	iov;
	int 		syscode = SYS_read;
	int 		serial = !sysent[syscode].sy_parallel;

	/*
	 * Always init MIG's OUT params for ports and ool memory to null,
	 * lest random values be accidentally transmitted in the reply.
	 */
	*data = 0;
	*amount = 0;

	error = start_fileserver_op(&fp, file_port, creds_port, 0,
				    syscode, serial);
	if (error)
		return(error);
	/*
	 * Note: synchronization of multiple clients must be done at 
	 * a higher layer.
	 */
	if ((int)count < 0) {
		error = EINVAL;
		*data = NULL;
		goto out;
	}

	uio.uio_segflg = UIO_SYSSPACE;
	uio.uio_iov = &iov;
	uio.uio_iovcnt = 1;
	uio.uio_offset = offset;
	uio.uio_rw = UIO_READ;
	uio.uio_resid = count;

	iov.iov_len = count;
	iov.iov_base = (caddr_t) NULL;    /* expecting a buffer in return */

	VOP_DATAIN((struct vnode *)fp->f_data, &uio, 0, fp->f_cred, error);

	/*
	 * Note that this routine assumes that the contents of iov are valid
	 * *after* the read is complete.  This is not the case for standard
	 * VOP_READ operations, where the buffer pointed to by iov_base is
	 * first allocated and then handed to VOP_READ.  Here, the semantics
	 * are that a buffer is allocated by the routines implementing
	 * VOP_DATAIN, and a pointer to the data is left in iov_base.
	 */
	*data = iov.iov_base;
	*amount = count - uio.uio_resid;
 out:
	error = end_fileserver_op(fp, error, serial);
	if (*amount)
		/* return success if any bytes moved */
		error = ESUCCESS;
	return(error);
}


S_fsvr_write_at_offset(file_port, creds_port, response_id, offset, data, 
		       count, amount)
	mach_port_t		file_port;
	mach_port_t		creds_port;
	unsigned int		*response_id;
	off_t			offset;
	char 			*data;
	unsigned int		count;
	int			*amount;		/* out */
{
	register int	error;
	struct file 	*fp;
	struct uio	uio;
	struct iovec	iov;
	unsigned 	file_limit;
	int 		syscode = SYS_write;
	int 		serial = !sysent[syscode].sy_parallel;

	*amount = 0;
	error = start_fileserver_op(&fp, file_port, creds_port, 0,
				    syscode, serial);
	if (error)
		return(error);
	/*
	 * Note: synchronization of multiple clients must be done at 
	 * a higher layer.
	 */
	if ((int)count < 0) {
		error = EINVAL;
		goto out;
	}

	file_limit = u.u_rlimit[RLIMIT_FSIZE].rlim_cur;
	if (offset >= file_limit) {
		/* higher layer responsible for generating SIGXFSZ */
		error = EFBIG;	
		goto out;
	} 
	if (offset + count > file_limit) 
		count = file_limit - offset;

	uio.uio_segflg = UIO_SYSSPACE;
	uio.uio_iov = &iov;
	uio.uio_iovcnt = 1;
	uio.uio_offset = offset;
	uio.uio_rw = UIO_WRITE;
	uio.uio_resid = count;

	iov.iov_len = count;
	iov.iov_base = (caddr_t)data;	/* expecting buffer to be consumed */
	
	VOP_DATAOUT((struct vnode *)fp->f_data, &uio, IO_CONSUME, fp->f_cred, 
		    error);

	*amount = count - uio.uio_resid;
out:
	error = end_fileserver_op(fp, error, serial);
	if (*amount)
		/* return success if any bytes moved */
		error = ESUCCESS;
	return(error);
}

#endif	FULLSERVER

#if	defined(TNC) || defined(FULLSERVER)

/*
 *  Called from select_cleanup() and select_wakeup_poll() to clean up
 *  an "extra" file_port send-right and file struct reference that was
 *  taken on the primary socket when this select was enqueued.  The
 *  extra ref is returned in file_port_send; one such "extra" ref was
 *  taken for each secondary socket (less one that was taken by the
 *  base OSF code).  See the enqueue ("scanning") loop in vsoo_select().
 */
int
S_fsvr_file_unref_svr(
	mach_port_t	file_port,
	mach_port_t	creds_port,
	mach_port_t	file_port_send,
	int		increment)
{
	int		error;
	struct file	*fp;
#if	MACH_ASSERT
	extern int vsdebug; /*zzz*/
#endif

	error = start_fileserver_op(&fp, file_port, creds_port, NULL,
								NULL, NULL);
	if (error)
		return(error);

	/* Should only be calling this for f_svrsend-type refs. */
	ASSERT(increment < 0);
	ASSERT(file_port == file_port_send);

#if	MACH_ASSERT
	if (vsdebug & 0x20000) /*zzz*/
		vs_frefs(fp, "before remote unref");
#endif
	error = fp_unref_port(fp, increment);
#if	MACH_ASSERT
	if (vsdebug & 0x20000) /*zzz*/
		vs_frefs(fp, " after remote unref");
#endif

	return(end_fileserver_op(fp, error, NULL));
}

#endif	/* TNC || FULLSERVER */
