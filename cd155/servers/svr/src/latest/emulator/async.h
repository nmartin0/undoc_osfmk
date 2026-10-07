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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *      Copyright 1992 Intel Corporation.
 */

/*
 * HISTORY
 * $Log: async.h,v $
 * Revision 1.7  1994/11/18  20:22:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1993/12/22  21:27:16  rlg
 * Changed the value of the MAX_ASYNC_REQUESTS define (from 128 to 256)
 * in response to PTS #7017.
 *
 * Revision 1.5  1993/08/03  16:55:04  wunder
 * Reduced initial structure count used based on performance results.
 *
 * Revision 1.4  1993/05/26  23:27:07  wunder
 * Added thread field to request structure needed for thread_switch useage.
 *
 * Revision 1.3  1993/05/25  18:31:55  dbm
 * Added interrupt support for asyncronous calls.
 *
 * Revision 1.2  1993/04/03  03:17:04  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1  1993/03/25  23:45:45  wunder
 * Removed type field from async_req structure and changed literal definitions
 * for ISYNC and IREADV, etc to repair conflict.
 *
 * Revision 1.1  1993/03/16  01:45:54  wunder
 * Initial version for asynchronous I/O support.
 *
 *
 */

/* Asynchronous request tracking structure */
typedef struct async_request {
	long			async_id;	/* unique id for structure */
	spin_lock_t		async_lock;	/* state_flag access lock */
	unsigned long		state;	/* state of asynchronous request */
	struct async_request	*next;	/* pointer used for queuing */
	long			actual;	/* number of bytes transferred */
	long			async_errno;	/* request error */
	mach_port_t		proc_port;
	mach_port_t		async_thread;
	int			*interrupt;
	char			*buffer;
	unsigned int		nbytes;
	unsigned int		fdte;
} async_req;

/* definitions for async_req state_flag */
#define IREAD	0x0001
#define IREADV	0x0002
#define	IWRITE	0x0003
#define	IWRITEV	0x0004
#define	ISYNC	0x0005
#define IDONE	0x8000

/* definition for the initial number of fdte queue structures to create,
   also used for the size to expand the list when more are needed */
#define PFS_INITIAL_ASYNC_OPS 2

/* definition for the maximum number of outstanding asynchronous requests
   that can be in progress at the same time.  Any attempt to exceed this
   number will result in an error being returned to the user */
#define MAX_ASYNC_REQUESTS 256 

/* state values for fdte lock entries */
#define ASYNC_FREE	0
#define ASYNC_BUSY	1

/* Asynchronous fdte lock structure */
typedef struct async_fdt_entry_queue {
	spin_lock_t	lock;		/* locks protecting queue pointers */
	unsigned short	index;
	unsigned short	state;
	emul_stack_t	stack;
	mach_port_t	thread;
	int		interrupt;	/* Indicates interrupt occurred. */
	async_req	*head;		/* head of fdte wait queue, owner of fdte */
	async_req	*tail;		/* end of fdte wait queue */
} async_fdte_queue;

