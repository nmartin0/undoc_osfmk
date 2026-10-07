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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_boot.h,v 1.5 1995/03/07 02:36:01 lenb Exp $
 */

/*
 * mcmsg_boot.c
 *
 * boot message support
 */


#define	BOOT_DELAY	BUMPERS

int	mcmsg_boot_count[4];

struct {
	unsigned long		node;
	unsigned long		flags;
	unsigned long		control;
	unsigned long		count;
#if	BOOT_DELAY
	double			when;
#endif	BOOT_DELAY
	unsigned char		*bufp;
	unsigned char		buf[4096];
} mcmsg_boot_request;

