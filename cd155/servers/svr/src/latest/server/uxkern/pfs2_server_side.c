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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation 
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *      Copyright 1992 Intel Corporation.
 *
 *
 * Module Function:
 *	Server-side receive functions for PFS stripefile operations that are
 *	issued concurrently.
 *
 * HISTORY
 * $Log: pfs2_server_side.c,v $
 * Revision 1.9  1995/04/04  18:41:05  rlg
 * The gopen step (for the M_UNIX, M_RECORD, and M_ASYNC I/O modes) of
 * duplicating the file server's file table was redesigned.  This change
 * results in only one "many to one" messages (sent from N-1 compute nodes
 * to logical node 0 in the application's partition) and one set of "one to
 * many" messages (sent from each file server to those N-1 compute nodes).
 * The old code required each of the compute nodes in the application's
 * partition (minus one) to execute a sequence of four RPCs to the file
 * servers containing the header file and each stripe file.  This resulted
 * in four "many to one" messages sent to each of those file servers.
 *
 *  Reviewer:  Balaji Narasimhan, Stan Smith
 *  Risk:      Medium (number of modules changed, scope of redesign)
 *  PTS#       9637
 *  Testing:   pfs and fileio EATs, five Eval gopen tests,
 *             rw and iomode integration tests
 *  Module(s): cmds_libs/src/usr/ccs/lib/libnx/_gopen.c
 *             cmds_libs/src/usr/ccs/lib/libnx/pfs_iomode.h
 *             server/svr/src/svr/emulator/pfs_user_side.c
 *             server/svr/src/svr/server/conf/syscalls.master
 *             server/svr/src/svr/server/uxkern/fsvr.defs
 *             server/svr/src/svr/server/uxkern/fsvr_server_side.c
 *             server/svr/src/svr/server/uxkern/pfs2.defs
 *             server/svr/src/svr/server/uxkern/pfs2_server_side.c
 *
 * Revision 1.8  1995/03/02  21:35:00  stans
 *  Vnode caching support: pfs_host_init() syscall.
 *
 *  Reviewer:jlitvin,suri,cfj
 *  Risk:medium
 *  Benefit or PTS #:8129
 *  Testing: WW07 sats
 *
 * Revision 1.7  1994/11/18  20:48:55  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/04/20  20:48:51  rlg
 * merge of revision 1.4.4.2 from the R1.2 branch
 *
 * Revision 1.4.4.2  1994/04/20  18:50:37  rlg
 * The PFS close function was enhanced to close all stripe files in parallel.
 *
 *  Reviewer:  Brad Rullman
 *  Risk:  medium
 *  Benefit or PTS #:  PTS #8953
 *  Testing:  failing test case, pfs and fileio EATs
 *  Module(s):  emulator/fsvr_user_side.c
 * 	     emulator/pfs2_user_side.c
 * 	     server/uxkern/pfs2.defs
 * 	     server/uxkern/pfs2_server_side.c
 *
 * Revision 1.5  1994/03/11  20:01:10  rlg
 * Merged changes from the R1.2 branch to the R1.3 branch (1.4.4.1).
 *
 * Revision 1.4.4.1  1994/03/11  16:39:02  rlg
 * The utimes() function for PFS files was reimplemented following the model
 * of pfs_multi_stat(), so that the header file and all stripe files have the
 * same value set in the access and modification time fields.  The old code
 * only set these fields in the header file.
 *
 *  Reviewer:  Brad Rullnam
 *  Risk: medium
 *  Benefit or PTS #:  PTS #6870
 *  Module(s):  emulator/fsvr_user_side.c
 *              emulator/pfs2_user_side.c
 *              server/uxkern/fsvr.defs
 *              server/uxkern/pfs2.defs
 *              server/uxkern/fsvr_server_side.c
 *              server/uxkern/pfs2_server_side.c
 *              server/vfs/vfs_syscalls.c
 *
 * Revision 1.4  1993/06/16  18:48:41  brad
 * Removed mappable argument to S_fsvr_open ... it's no longer used.
 *
 * Revision 1.3  1993/05/27  02:03:19  brad
 * Removed temporary PFS read/write_at_offset interfaces ... PFS now uses
 * Fast Path read/write interfaces exclusively.
 *
 * Revision 1.2  1993/04/03  03:12:28  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.6  1993/03/10  05:55:35  brad
 * Added PFS support for chown, chmod, lsize, statfs, ... and others.
 * All async PFS operations now use the response ID parameter, and
 * the server side stubs call in to the fsvr_server_side.c routines
 * to do the work.  Opens of PFS stripefiles in different I/O modes
 * (e.g. VIO_MAPPED and VIO_STRIPED) are now disallowed.
 *
 * Revision 1.1.2.5  1993/02/23  04:46:26  brad
 * Added PFS support for access().
 *
 * Revision 1.1.2.4  1993/02/12  22:39:31  brad
 * Added support for disallowing simultaneous access to a PFS file
 * (VIO_PFS mode) and one of its stripefiles (VIO_STRIPED mode).
 *
 * Revision 1.1.2.3  1993/02/09  21:53:42  brad
 * Added stubs for asynchronous PFS RPC`s for stat, fstat, open, unlink,
 * and pfs_fdevstat.
 *
 * Revision 1.1.2.2  1993/02/04  00:32:49  brad
 * Changed pfs_read() and pfs_write() MiG interfaces to accept a file offset
 * and call directly down to ufs_read() and ufs_write().
 *
 * Revision 1.1.2.1  1993/01/05  22:44:29  brad
 * Added S_fsvr_pfs_write() for concurrent stripefile writes.
 *
 * Revision 1.1  1992/11/25  22:24:56  brad
 * Initial revision
 *
 * 
 */

#ifdef	PFS

#include <map_uarea.h>
#include <mach_nbc.h>
#include <mapped_files.h>

#include <uxkern/syscall_subr.h>
#include <uxkern/import_mach.h>
#include <uxkern/fsvr.h>
#include <sys/vnode.h>
#include <ufs/inode.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <sys/proc.h>
#include <sys/errno.h>
#include <sys/file.h>
#include <sys/un.h>

#include <sys/mount.h>
#include <uxkern/sthread.h>

#include <uxkern/proc_to_task.h>
#include <uxkern/syscalltrace.h>
#include <uxkern/bsd_msg.h>
#include <uxkern/mf.h>



int
S_fsvr_pfs_open(cwd_port, creds_port, transid, root_port, response_id,
		pathname, pathname_len, mode, crt_mode, file_port, iomode)
	mach_port_t		cwd_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	mach_port_t		root_port;
	uint_t			*response_id;
	char			*pathname;
	unsigned int		pathname_len;
	int			mode;
	int			crt_mode;
	mach_port_t		*file_port;	/* out */
	u_long			iomode;
{
	return(S_fsvr_open(cwd_port, creds_port, transid, root_port, pathname,
			   pathname_len, mode, crt_mode, file_port, &iomode));
}


int
S_fsvr_pfs_stat(cwd_port, creds_port, transid, root_port, response_id, 
		pathname, pathname_len, follow, stat)
	mach_port_t		cwd_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	mach_port_t		root_port;
	uint_t			*response_id;
	char			*pathname;
	unsigned int		pathname_len;
	boolean_t		follow;
	struct stat 		*stat;		/* out */
{
	u_long			dont_care;

	return(S_fsvr_stat(cwd_port, creds_port, transid, root_port,
			   pathname, pathname_len, follow, stat, &dont_care));
}


int
S_fsvr_pfs_fstat(file_port, creds_port, transid, response_id, stat)
	mach_port_t		file_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	uint_t			*response_id;
	struct stat	  	*stat;
{
	return(S_fsvr_fstat(file_port, creds_port, transid, stat));
}


int
S_fsvr_pfs_unlink(cwd_port, creds_port, transid, root_port, response_id,
		  pathname, pathname_len)
	mach_port_t		cwd_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	mach_port_t		root_port;
	uint_t			*response_id;
	char			*pathname;
	unsigned int		pathname_len;
{
	u_long			iomode = VIO_NONE;

	return(S_fsvr_unlink(cwd_port, creds_port, transid, root_port,
			     pathname, pathname_len, &iomode));
}


int
S_fsvr_pfs_access(cwd_port, creds_port, transid, root_port, response_id,
		  pathname, pathname_len, mode)
	mach_port_t		cwd_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	mach_port_t		root_port;
	uint_t			*response_id;
	char			*pathname;
	unsigned int		pathname_len;
	int			mode;
{
	u_long			dont_care;

	return(S_fsvr_access(cwd_port, creds_port, transid, root_port, 
			     pathname, pathname_len, mode, &dont_care));
}


int
S_fsvr_pfs__lsize(file_port, creds_port, transid, response_id,
		  off, sbase, newlen)
	mach_port_t		file_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	uint_t			*response_id;
	u_long			off;
	int			sbase;
	u_long			*newlen;	/* out */
{
	return(S_fsvr__lsize(file_port, creds_port, transid, 
			     off, sbase, newlen));
}


int
S_fsvr_pfs_ftruncate(file_port, creds_port, transid, response_id, len)
	mach_port_t		file_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	uint_t			*response_id;
	u_int			len;
{
	return(S_fsvr_ftruncate(file_port, creds_port, transid, len));
}


int
S_fsvr_pfs_chmod(cwd_port, creds_port, transid, root_port, response_id,
		 pathname, pathname_len, mode)
	mach_port_t		cwd_port;
	mach_port_t		creds_port;
 	transaction_id_t	transid;
	mach_port_t		root_port;
	uint_t			*response_id;
	char			*pathname;
	unsigned int		pathname_len;
	int			mode;
{
	u_long			dont_care;

	return(S_fsvr_chmod(cwd_port, creds_port, transid, root_port,
			    pathname, pathname_len, mode, &dont_care));
}


int
S_fsvr_pfs_fchmod(file_port, creds_port, transid, response_id, mode)
	mach_port_t		file_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	uint_t			*response_id;
	int			mode;
{
	return(S_fsvr_fchmod(file_port, creds_port, transid, mode));
}


S_fsvr_pfs_chown(cwd_port, creds_port, transid, root_port, response_id,
		 pathname, pathname_len, uid, gid)
	mach_port_t		cwd_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	mach_port_t		root_port;
	uint_t			*response_id;
	char			*pathname;
	unsigned int		pathname_len;
	int			uid;
	int			gid;
{
	u_long			dont_care;

	return(S_fsvr_chown(cwd_port, creds_port, transid, root_port,
			    pathname, pathname_len, uid, gid, &dont_care));
}


int
S_fsvr_pfs_fchown(file_port, creds_port, transid, response_id, uid, gid)
	mach_port_t		file_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	uint_t			*response_id;
	int			uid;
	int			gid;
{
	return(S_fsvr_fchown(file_port, creds_port, transid, uid, gid));
}


int
S_fsvr_pfs_fsync(file_port, creds_port, transid, response_id)
	mach_port_t		file_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	uint_t			*response_id;
{
	return(S_fsvr_fsync(file_port, creds_port, transid));
}


int
S_fsvr_pfs_file_unref(file_port, creds_port, response_id, move_file_right)
	mach_port_t		file_port;
	mach_port_t		creds_port;
	uint_t			*response_id;
	mach_port_t		move_file_right;
{
	return(S_fsvr_file_unref(file_port, creds_port, move_file_right));
}

int
S_fsvr_pfs_statfs(cwd_port, creds_port, transid, root_port, response_id,
		  pathname, pathname_len, statfsb)
	mach_port_t		cwd_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	mach_port_t		root_port;
	uint_t			*response_id;
	char			*pathname;
	unsigned int		pathname_len;
	struct statfs		*statfsb;	/* out */
{
	return(S_fsvr_statfs(cwd_port, creds_port, transid, root_port,
			     pathname, pathname_len, statfsb));
}

int
S_fsvr_pfs_utimes(cwd_port, creds_port, transid, root_port, response_id,
                  pathname, pathname_len, times, current_time)
	mach_port_t		cwd_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	mach_port_t		root_port;
	uint_t			*response_id;
	char			*pathname;
	unsigned int		pathname_len;
	struct timeval	        *times;
	boolean_t	        current_time;
{
        u_long                  iomode;		/* unused by this routine 
						   but needed by the call
						   to S_fsvr_utimes() */

	return(S_fsvr_utimes(cwd_port, creds_port, transid,
			     root_port, pathname, pathname_len,
			     times, current_time, &iomode));
}

int
S_fsvr_pfs_duplicate(oldfl_port, creds_port, transid,
		     count, response_id, buf, bufsize)
        mach_port_t     	oldfl_port;
        mach_port_t     	creds_port;
	transaction_id_t	transid;
	uint_t			response_id;	/* in, id needed by emulator  */
	mach_port_t		*buf;		/* in, dealloc (ool memory)   */
        int                     count;		/* in, number of ports in
						       remote_ports (and number
						       of file table entries
						       to duplicate)          */

{
	return(S_fsvr_duplicate(oldfl_port, creds_port, transid,
				count, response_id, buf, bufsize));
}


/*
 * MiG stub for RPC pre_parse of PFS pathname.
 * Support for pfs_host_init() syscall.
 */

int
S_fsvr_pfs_pre_parse(cwd_port, root_port, creds_port, transid, response_id,
		     pathname, pathname_len, new_vnode_port)
	mach_port_t		cwd_port;
	mach_port_t		root_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	uint_t			*response_id;
	char			*pathname;
	unsigned int		pathname_len;
	mach_port_t		*new_vnode_port;	/* out */
{
	register int    error;
	struct vnode	*vp;
	struct nameidata *ndp = &u.u_nd;
	kern_return_t	kr;


	*new_vnode_port = MACH_PORT_NULL;
	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				     creds_port, transid, pathname, 
				     pathname_len, NULL, NULL);

	if (error) 
		return(error);

	error =	pfs_pre_parse(u.u_procp, &pathname, 0);

	/*
	 * If a remote mount point was detected, forward the chown message.
	 */
	if (error == EREMOTE) {
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_pfs_pre_parse(ndp->ni_forwport, root_port,
					   creds_port, transid,
					   response_id,
					   ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
					   new_vnode_port);
		OIP_END_FORW(oipp);
	} else if (!error) {
		vp = ndp->ni_vp;
		get_vnode_port(vp, new_vnode_port);
		vrele(vp);

		/*
		 * boost the local port queue limit as we will be expecting
		 * a flood of fileop requests.
		 */
		kr = mach_port_set_qlimit(mach_task_self(),*new_vnode_port,20);
		if ( kr != KERN_SUCCESS ) {
			extern long	this_node;
			printf("[%d] mach_port_set_qlimit(port %x lim 20) kr %d\n",
				this_node, *new_vnode_port,kr);

		}
	}

	return(end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
			NULL));
}

#endif	/* PFS */
