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
 *  $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/fgopen.c,v 1.4 1994/11/19 02:29:32 mtm Exp $
 *
 *	FORTRAN interface support function used to perform the 
 *	global open functionality.  This function is only called 
 *	by the Fortran run-time library to do a globaly open a 
 *	the specified file. 
 *
 *      HISTORY:
 *      $Log: fgopen.c,v $
 * Revision 1.4  1994/11/19  02:29:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/09/29  23:34:31  dbm
 * Removed printf that was accidentally left in.
 *
 * Revision 1.2  1993/06/08  20:17:59  dbm
 * Added check for single node case.
 *
 * Revision 1.1  1993/02/13  01:02:38  dbm
 * Initial revision
 *
 *
 */
#include <stdio.h>
#include <unistd.h>
#ifdef  _THREAD_SAFE
#include "stdio_lock.h"
#endif
#include <fcntl.h>
#include <errno.h>
#include "pfs_iomode.h"

extern FILE *_findiop();


FILE *
fgopen(filename, iomode)
char		*filename;
int		iomode;
{
FILE	*stream;

struct open_info_t {
	int	oflag;
	int	flag;
	int 	retry;
}open_info = {0,0,0};
	
int	fd;
#ifdef  _THREAD_SAFE
        register filelock_t filelock;
#endif

	stream =  _findiop();

	if (stream == NULL || filename == NULL || filename[0] == '\0') {
#ifdef  _THREAD_SAFE
		seterrno(ENOENT);
#else
		errno = ENOENT;
#endif
		return (NULL);
	}
	/*
	 * Syncrhronize everyone up first:
	 */

	gsync();
	/*
	 * Figure out Fortran access type to compute the oflag value.
	 */
	if (mynode() == 0) {
		if (access(filename, F_OK) == 0) {
			/*
			 * File exits:
			 */
			open_info.oflag = O_RDWR;
			open_info.flag  = _IORW;
			open_info.retry = 1;
		} else {
			open_info.oflag = O_RDWR | O_TRUNC | O_CREAT; 	
			open_info.flag  = _IORW;
			open_info.retry = 0;
		}
		/*
		 * Send the access, retry, and stream flags to the other 
		 * nodes in the system.
		 */
		if (_numnodes() > 1) {
			if (_csend(PFS_FGOPEN_MSG_TYPE, &open_info, 
		    		   sizeof(open_info), -1, myptype()) < 0) {
					nx_perror("fgopen:");
					exit(1);
			}
		}
	} else {
		/* 
		 * Receive the access, retry, and stream flag from node 0.
		 */
		if (_crecv(PFS_FGOPEN_MSG_TYPE, &open_info, 
		     sizeof(open_info)) < 0) {
			nx_perror("fgopen");	
			exit(1);
		}
	}


        if ((fd = _gopen(filename, open_info.oflag, iomode, 0666)) < 0) {
		if (open_info.retry) {
			open_info.oflag = O_RDONLY;
			open_info.flag  = _IOREAD;
		
        		if ((fd = _gopen(filename, open_info.oflag, iomode, 
					0666)) < 0) {
                			return (NULL);
			}
		} else {
			return(NULL);
		}
	}

#ifdef  _THREAD_SAFE
        filelock = _flockfile(stream);
#endif
        stream->_cnt = 0;
        stream->_file = fd;
        stream->_flag = open_info.flag;
        stream->_bufendp = stream->_base = stream->_ptr = NULL;
#ifdef  _THREAD_SAFE
        _funlockfile(filelock);
#endif
        return (stream);
}
