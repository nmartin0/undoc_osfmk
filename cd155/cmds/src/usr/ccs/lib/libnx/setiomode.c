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
 *      Copyright 1992  Intel Corporation.
 *
 *	$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/setiomode.c,v 1.6 1994/11/19 02:32:32 mtm Exp $
 *
 * Set the I/O mode and perform a global syncronization.
 *
 *      HISTORY:
 *      $Log: setiomode.c,v $
 * Revision 1.6  1994/11/19  02:32:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/06/13  15:51:05  rlg
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
 * Revision 1.4  1993/11/04  18:02:39  shala
 *  Reviewer: regnier
 *  Risk: low
 *  Benefit or PTS #: Bug #7049.
 *  Testing: Build setiomode.c with no errors.
 *  Module(s): setiomode.c.
 *
 * Made parameter mode to be of type "int" to match "nx.h".
 *
 * Revision 1.3  1992/08/05  12:00:18  dbm
 * Added History; Matched parameter names and types to EPS.
 *
 *
 */
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>

/*
 * setiomode.c
 *
 * Description:
 *		This function is used to set the I/O mode of the file 
 *		specified by the 'fd' parameter.  At the same time, 
 *		a global synchronizing operation is performed.
 *		This function will will return on success.
 *		On failure, the error message will be written to 
 *		standard error and the calling process will be 
 *		terminated with a non zero exit code.
 *	
 * Parameters:	
 *		fildes	File descriptor.
 *		mode	An integer from 0 through 4 that sets the I/O
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
 * Returns:
 *		Nothing.
 */
void
setiomode(fildes, mode)
int	fildes;	/* File descriptor. */
int 	mode;	/* I/O mode. */
{
	int status;

	status = _setiomode(fildes, mode);
	if (status == -1) {
		nx_perror("setiomode");
		exit(1);
	}
}
