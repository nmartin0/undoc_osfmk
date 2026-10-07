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
 *		INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *  Copyright 1992  Intel Corporation.
 *
 *  $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/gopen_.c,v 1.5 1994/11/19 02:29:59 mtm Exp $
 *
 * FORTRAN interface to do a global open.
 *
 *      HISTORY:
 *      $Log: gopen_.c,v $
 * Revision 1.5  1994/11/19  02:29:59  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/06/13  15:49:24  rlg
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
 * Revision 1.3  1993/06/23  23:17:45  dbm
 * Modified to use the automatic file name size parameter provided by
 * Fortran instead of an explicit parameter.
 *
 * Revision 1.2  1993/06/08  20:17:19  dbm
 * Added reference to fgopen() to allow linking of program to work correctly.
 * The fgopen() function is called by the PGI __fio_gopen() function.  By
 * bringing it in the linker does not have to re-search the libnx library
 * after __fio_gopen is brought in.
 *
 * Revision 1.1  1993/02/13  01:02:51  dbm
 * Initial revision
 *
 *
 */
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include "pfs_iomode.h"

extern FILE *fgopen();
extern void __fio_gopen();

void
gopen_(unit, name, iomode, namelen)
int		*unit;
char		*name;
int		*iomode;
int		namelen;
{
	int dummy = 0;

	if ((*iomode < M_UNIX) || (*iomode > M_ASYNC)) {
    		fprintf(stderr, "gopen: Invalid iomode\n");
		exit(1);
	}

	__fio_gopen(*unit, *iomode, name, namelen);

	/*
	 * The following call to fgopen() is never made, the only
	 * purpose of the the following statements is to make the
 	 * reference so that the linker pulls the fgopen() function
	 * in when this function is linked.
	 */
	if (dummy) {	/* Never make the call. */
		fgopen();
	}
}
