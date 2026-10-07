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
 * FORTRAN interface to do a synchronous read.
 *
 * HISTORY:
 * $Log: cread_.c,v $
 * Revision 1.8  1995/01/24  16:53:16  rlg
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
 * Revision 1.7  1994/11/19  02:29:12  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1993/10/20  00:28:17  brad
 * Print error if file not opened with form=unformatted, rather than just
 * exiting silently.
 *
 * Revision 1.5  1993/04/02  22:41:46  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.4.2.1  1993/03/20  23:39:14  brad
 * Just call read()/write() directly instead of calling _cread()/_cwrite().
 *
 * Revision 1.4  1992/08/05  09:29:55  dbm
 * Matched parameter types with EPS.
 *
 * Revision 1.3  92/08/05  09:18:40  dbm
 * Added History.
 * Changed explicit error print to setting of errno and call to nx_perror();
 * 
 *
 */

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>

extern FILE  *PGI_init_io();

void
cread_(unit, buf, len)
int		*unit;
char		*buf;
unsigned int	*len;
{
	int    cnt, fd;
	FILE  *stream;


	/*
	 * Initialize the FORTRAN runtime for access to the specified
	 * unit.  Note that the FORTRAN runtime will print an error
	 * message and terminate this process it it detects any errors.
	 */
	stream = PGI_init_io(*unit, "cread");
	fd = fileno(stream);

	if ((cnt = read(fd, buf, *len)) == -1) {
		nx_perror("cread");
		exit(1);
	} else if (cnt < *len) {
		errno = ERDEOF;
		nx_perror("cread");
		exit(1);
	}
}
