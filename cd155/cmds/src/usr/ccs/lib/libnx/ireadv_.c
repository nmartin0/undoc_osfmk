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
 *
 * FORTRAN interface to do an asynchronous readv.
 *
 * HISTORY:
 * $Log: ireadv_.c,v $
 * Revision 1.4  1995/01/24  17:00:30  rlg
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
 * Revision 1.3  1994/11/19  02:30:49  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1993/10/20  00:28:58  brad
 * Print error if file not opened with form=unformatted, rather than just
 * exiting silently.
 *
 * Revision 1.1  1993/03/20  23:44:35  brad
 * Initial revision
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <errno.h>

extern FILE  *PGI_init_io();

ireadv_(unit, iov, iovcount)
int		*unit;
struct iovec	*iov;
int		*iovcount;

{
	int    fd, mid;
	FILE  *stream;


	/*
	 * Initialize the FORTRAN runtime for access to the specified
	 * unit.  Note that the FORTRAN runtime will print an error
	 * message and terminate this process it it detects any errors.
	 */
	stream = PGI_init_io(*unit, "ireadv");
	fd = fileno(stream);

	if ((mid = _ireadv(fd, iov, *iovcount)) == -1) {
		nx_perror("ireadv");
		exit(1);
	}
	return(mid);
}

