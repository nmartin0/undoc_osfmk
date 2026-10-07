/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
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
 *      Copyright 1993  Intel Corporation.
 *
 *	$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/gopen.c,v 1.4 1994/11/19 02:29:58 mtm Exp $
 *
 * Perform a global open on a pathname and set the PFS I/O mode.
 *
 *      HISTORY:
 *      $Log: gopen.c,v $
 * Revision 1.4  1994/11/19  02:29:58  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/06/13  15:47:24  rlg
 * Added the M_ASYNC I/O mode for shared files.  This mode is characterized by:
 *     o	each node has a unique file pointer,
 *     o	nodes are not synchronized
 *     o	file access is unrestricted
 *     o	standard UNIX file sharing semantics requiring atomicity of I/O
 * 	are not preserved.
 *
 *  Reviewer:  Brad Rullman
 *  Risk:  medium
 *  Benefit or PTS #:  7480
 *  Testing:  I/O mode unit test; 132 Eval I/O tests; rw performance test;
 *  Module(s):  emulator/fsvr_user_side.c		libnx/_gopen.c
 * 		      pfs2_user_side.c		      _pfs_setio.c
 * 		      pfs_iomode.c		      _setiomode.c
 * 		      pfs_iomode.h		      gopen.c
 * 		      pfs_tokenmgt.c		      gopen_.c
 * 		      pfs_user_side.c		      pfs_iomode.h
 * 						      setiomode.c
 *
 * Revision 1.2  1994/03/29  21:38:06  bks
 * Changed fourth argument from type int to type mode_t to match nx.h, and
 * specification for gopen.
 *
 *  Reviewer: Cris Derr
 *  Risk: L
 *  Benefit or PTS #: 8766
 *  Testing: Compiled libnx, showfs, ls, tar, mount
 *  Module(s): gopen()
 *
 * Revision 1.1  1993/02/12  17:34:30  dbm
 * Initial revision
 *
 *
 *
 */
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>

/*
 * gopen.c
 *
 * Description:
 *		This function is used to open a shared file and 
 *		set the PFS I/O mode of the file. At the same time, 
 *		a global synchronizing operation is performed.
 *		This function will will return on success.
 *		On failure, the error message will be written to 
 *		standard error and the calling process will be 
 *		terminated with a non zero exit code.
 *	
 * Parameters:	
 *		path	Pathname of the file to open.
 *
 *		oflag	Access flag.
 *	
 *		iomode	An integer from 0 through 5 that sets the I/O
 *			mode of the designated file:
 *
 *			0	Individual file pointer.
 *
 *			1	Common file pointer.
 *
 *			2	Synchronized, common file pointer,
 *				variabgle length requests.
 *
 *			3	Synchronized, common file pointer,
 *				fixed length requests.
 *
 *			4	Synchronized, common file pointer,
 *				synchronized requests.
 *
 *			5	Individual file pointer, no token
 *				synchronization on reads *or* writes.
 *
 *		mode	Permissions of the file, (used when creating
 *			the file).
 *
 * Returns:
 *		The file descriptor handle to the file that was
 *		opened.
 */
int
gopen(path, oflag, iomode, mode)
char	*path;
int	oflag;
int	iomode;
mode_t	mode;
{
	int status;


	status = _gopen(path, oflag, iomode, mode);
	if (status == -1) {
		nx_perror("gopen");
		exit(1);
	}
	return status;
}
