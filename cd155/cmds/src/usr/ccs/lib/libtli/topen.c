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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.2
 */

/** Copyright (c) 1989  Mentat Inc.
 ** topen.c 1.1, last change 8/9/89
 **/

#include <tli/common.h>
#include <stropts.h>
#include <errno.h>
#ifdef XTI
#include <fcntl.h>
#include <xti.h>
#include <tli/timod.h>
#else
#include <tiuser.h>
#endif

#ifdef XTIDBG
#include <tli/tdbg.h>
#endif

int
t_open (name, oflag, tinfo)
	char	* name;
	int	oflag;
	struct t_info	* tinfo;
{
	int	fd;
        int     code;

        code = -1;
#ifdef XTI
	if (oflag & ~(O_RDWR | O_NONBLOCK)) {
		t_errno = TBADFLAG;
		goto rtn;
	}
#endif
	if ((fd = stream_open(name, oflag)) != -1) {
		if ( stream_ioctl(fd, I_PUSH, "timod") != -1 ) {
#ifdef XTI
			if (tli_ioctl(fd, TI_XTI_HELLO, nilp(char), 0) == -1) {
				t_close(fd);
				goto rtn;
			}
#endif
			if (tinfo  &&  t_getinfo(fd, tinfo) == -1) {
				t_close(fd);
				goto rtn;
			}
			(void)t_sync(fd);
			code = fd;
			goto rtn;
		}
		stream_close(fd);
	}
#ifdef XTI
	t_errno = ((errno == ENOENT) ? TBADNAME : TSYSERR);
#else
	t_errno = TSYSERR;
#endif

rtn:
#ifdef XTIDBG
        tr_open (name, oflag, tinfo, code);
#endif
	return code;
}
