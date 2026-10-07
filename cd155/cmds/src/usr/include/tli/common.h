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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */

 /** Copyright (c) 1988  Mentat Inc.
  ** common.h 1.2, last change 11/9/89
  **/

#ifndef	_TLI_COMMON_H
#define	_TLI_COMMON_H

#include <sys/types.h>
#include <sys/errno.h>

#define	staticf		static

#define	noop
#define	fallthru

typedef	int		boolean;
#define true		((boolean)1)
#define	false		((boolean)0)

#define A_CNT(arr)	(sizeof(arr)/sizeof(arr[0]))
#define	A_END(arr)	(&arr[A_CNT(arr)])
#define	A_LAST(arr)	(&arr[A_CNT(arr)-1])

#define getarg(ac,av)	(optind < ac ? av[optind++] : nilp(char))
#define newa(t,cnt)     ((t *)calloc(cnt, sizeof(t)))
#define	noshare
#define	MAX_STACK_BUF	512
#define ERR		(-1)

#define	nilp(t)		((t *)0)
#define	nil(t)		((t)0)

#define reg		register

#define	stream_open	open
#define	stream_close	close
#define	stream_ioctl	ioctl
#define	stream_read	read
#define	stream_write	write

#endif	/* _TLI_COMMON_H */
