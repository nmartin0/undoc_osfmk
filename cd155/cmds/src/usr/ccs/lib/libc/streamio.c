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
 * OSF/1 Release 1.0.1
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: streamio.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:08:34 $";
#endif

/** Copyright (c) 1989  Mentat Inc. **/

#include <sys/ioctl.h>
#include <sys/errno.h>
#include <stropts.h>

int
getmsg (fd, ctlptr, dataptr, flags)
	int	fd;
	struct strbuf	* ctlptr;
	struct strbuf	* dataptr;
	int	* flags;
{
	struct strpeek	strp;
	int	i1;

	if (flags == 0) {
		errno = EINVAL;
		return -1;
	}
	memset(&strp, '\0', sizeof(struct strpeek));
	if (ctlptr)
		strp.ctlbuf = *ctlptr;
	else
		strp.ctlbuf.maxlen = -1;
	if (dataptr)
		strp.databuf = *dataptr;
	else
		strp.databuf.maxlen = -1;
	strp.flags = *flags;
	if ((i1 = ioctl(fd, I_GETMSG, &strp)) >= 0) {
		if (ctlptr)
			*ctlptr = strp.ctlbuf;
		if (dataptr)
			*dataptr = strp.databuf;
		*flags = strp.flags;
	} else if (errno == ENOTTY)
		errno = ENOSTR;
	return i1;
}

int
putmsg (fd, ctlptr, dataptr, flags)
	int	fd;
	struct strbuf	* ctlptr;
	struct strbuf	* dataptr;
	int	flags;
{
	struct strpeek	strp;
	int i1;

	memset(&strp, '\0', sizeof(struct strpeek));
	if (ctlptr)
		strp.ctlbuf = *ctlptr;
	else
		strp.ctlbuf.len = -1;
	if (dataptr)
		strp.databuf = *dataptr;
	else
		strp.databuf.len = -1;
	strp.flags = flags;
	if ((i1 = ioctl(fd, I_PUTMSG, &strp)) < 0 && errno == ENOTTY)
		errno = ENOSTR;
	return i1;
}
