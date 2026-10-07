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
 *  $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/lseek_.c,v 1.6 1995/01/24 17:02:29 rlg Exp $
 *
 * FORTRAN interface to do a lseek.
 *
 *      HISTORY:
 *      $Log: lseek_.c,v $
 * Revision 1.6  1995/01/24  17:02:29  rlg
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
 * Revision 1.5  1994/11/19  02:31:07  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/10/20  00:29:11  brad
 * Print error if file not opened with form=unformatted, rather than just
 * exiting silently.
 *
 * Revision 1.3  1992/08/05  11:47:56  dbm
 * Added History; Added error checking on result of lseek.
 *
 *
 */

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>

extern FILE  *PGI_init_io();

int
lseek_(unit, offset, whence)
int	*unit;
int	*offset;
int	*whence;
{
	int    new_offset;
	int    fd;
	FILE  *stream;


	/*
	 * Initialize the FORTRAN runtime for access to the specified
	 * unit.  Note that the FORTRAN runtime will print an error
	 * message and terminate this process it it detects any errors.
	 */
	stream = PGI_init_io(*unit, "lseek");
	fd = fileno(stream);

	new_offset = lseek(fd, *offset, *whence);

	if (new_offset < 0) {
		nx_perror("lseek:");
		exit(1);
	}

	return new_offset;
}
