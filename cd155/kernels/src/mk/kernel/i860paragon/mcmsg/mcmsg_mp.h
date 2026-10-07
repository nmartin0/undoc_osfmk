/*
 * 
 * $Copyright
 * Copyright 1993 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1991 Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_mp.h,v 1.5 1994/11/18 20:44:26 mtm Exp $
 */

/*
 * mcmsg_mp.h
 *
 * Declarations for shared memory page used
 * to post calls to the message processor.
 */

#define POST_TASK_MAX	100
/* The following must turn out to be a power of 2 */
#define	POST_PAGE_SLOTS	(INTEL_PGBYTES/sizeof(post_page_t))

/*
 * Structure for one entry in a post page.
 * A post page consists of as many of these entries as fits in a page.
 * The entries are used in a ring. Each entry conveys one cross-processor call.
 *
 * The caller (kernel or user) has an "in" index, which rotates through the
 * entries in order. The "in" index always points to the next available entry.
 * The caller first fills in up to 10 parameters to the call (arg1 - arg10)
 * then places the method number identifying the call in method. In addition,
 * when the kernel is the caller, it might set mcmsg_task before method.
 * The caller then increments the "in" index to the next entry modulo page size.
 *
 * The callee (message processor) has an "out" index which follows the caller's
 * "in" index around the entries. When it detects that the method in the entry
 * designated by the "out" index is different from POST_EMPTY, it loads up the
 * arguments and makes the requested call. When the call returns, it places
 * the return value in status and changes method back to POST_EMPTY.
 * It then increments the "out" index to the next entry modulo page size.
 *
 * The caller (especially the user) does not normally wait for the call to
 * return. If desired, the caller can do so by spinning until method changes
 * back to POST_EMPTY and then picking up status. Normally, the user supplies
 * a pointer to a local data structure into which the call places status
 * when the total requested operation completes.
 *
 * If the caller does not wait for the call to return, it is necessary to
 * check the method to make sure it is POST_EMPTY before using it for another
 * call. Since the entries are used strictly in order, it is possible to
 * batch these checks by checking an entry beyond the one pointed to by the
 * "in" index. When the method in the entry beyond is POST_EMPTY, all entries
 * from the "in" index to the entry beyond are free.
 *
 * Because it must fit in a page, the size of the post_page structure
 * must be a power of 2.
 */

typedef
struct {
	unsigned long	method;
	long		arg0;
	long		arg1;
	long		arg2;
	long		arg3;
	long		arg4;
	long		arg5;
	long		arg6;
	long		arg7;
	long		arg8;
	long		arg9;
	long		dummy1;
	long		dummy2;
	long		dummy3;
	mcmsg_task_t	*mcmsg_task;
	long		status;
} post_page_t;
