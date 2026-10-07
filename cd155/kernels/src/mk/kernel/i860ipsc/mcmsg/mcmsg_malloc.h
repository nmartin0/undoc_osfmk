/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860ipsc/mcmsg/mcmsg_malloc.h,v 1.8 1994/11/18 20:41:37 mtm Exp $
 */

/*
 * mcmsg_malloc.h
 *
 * Message passing memory manager
 */

#ifndef MCMSG_MALLOC_H
#define MCMSG_MALLOC_H

#define	LOG2MMSIZE	18
#define MMSIZE		(1 << LOG2MMSIZE)

unsigned long		*mcmsg_l2malloc();

#endif MCMSG_MALLOC_H
