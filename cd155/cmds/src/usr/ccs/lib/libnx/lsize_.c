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
 *	This software is supplied under the terms of a license
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance
 *	with the terms of that agreement.
 *
 *  Copyright 1992  Intel Corporation.
 *
 *  $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/lsize_.c,v 1.6 1995/01/24 17:03:06 rlg Exp $
 *
 * FORTRAN interface to lsize function.
 *
 *      HISTORY:
 *      $Log: lsize_.c,v $
 * Revision 1.6  1995/01/24  17:03:06  rlg
 * In the R1.3 environment, the libnx to libf interface caused a number
 * of problems, as reported by PTS #11569, 11605, 12108, 12127, 12161,
 * 12185, and 12190.  The solution to these problems was to develop a new
 * libf initialization interface for libnx to use.  The old interface was
 * not safe in a multi-processor environment.
 *
 *  Reviewer:   Bob Toelle
 *  Risk:       high (changed interface between libxn and libf)
 *  PTS #:      11569
 *  Testing:    pfs and fileio EATs, 37 EVAL FORTRAN tests
 *  Module(s):  libnx/cread_.c    libnx/fcntl_.c    libnx/iwritev_.c
 *                    creadv_.c         iomode_.c         lseek_.c
 *                    cwrite_.c         iread_.c          lsize_.c
 *                    cwritev_.c        ireadv_.c         setiomode_.c
 *                    eseek_.c          iseof_.c
 *                    esize_.c          iwrite_.c
 *
 * Revision 1.5  1994/11/19  02:31:10  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/10/19  00:35:11  brad
 * Fix for bug 6914: print error if file not opened with form=unformatted.
 *
 * Revision 1.3  1992/08/05  11:54:29  dbm
 * Added History.
 *
 * 
 */

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>

extern FILE  *PGI_init_io();

lsize_(unit, offset, whence)
int	*unit;
off_t	*offset;
int	*whence;
{
	long   newsize, fd;
	FILE  *stream;


	/*
	 * Initialize the FORTRAN runtime for access to the specified
	 * unit.  Note that the FORTRAN runtime will print an error
	 * message and terminate this process it it detects any errors.
	 */
	stream = PGI_init_io(*unit, "lsize");
	fd = fileno(stream);

	newsize = _lsize(fd, *offset, *whence);

	if (newsize == -1) {
		nx_perror("lsize");
		exit(1);
	}
	return(newsize);
}
