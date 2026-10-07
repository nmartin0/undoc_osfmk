/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
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
 *	iod.h 11.1 94/03/25 15:51:56
 */
/** static char	iodh_ver[] = "@(#) iod.h 11.1 94/03/25 15:51:56";
/**/

/*
 * iod.h
 *
 * This module defines I/O descriptors
 * 
 */

#define NIOD 100

typedef
struct _iod {
	struct _iod	*link;
	char		busy;
	char		async;
	char		try;
	char		rw;
	int			fid;
	esize_t		position;
	void		(*_sync_handler)();
	int			_sync_handler_count;
	char		*bufsave;
	long		countsave;
	char		*buf;
	long		count;
	long		actual;
	long		errno;
} _iod_t;

extern _iod_t _iod[NIOD];

extern _iod_t *free_iod_q;
extern _iod_t *fio_iod_qf, *fio_iod_ql;
extern _iod_t *pipe_iod_qf, *pipe_iod_ql;
extern _iod_t *cfs_iod_qf, *cfs_iod_ql;

extern int _iod_sync_level;

extern _iod_t *new_iod();
