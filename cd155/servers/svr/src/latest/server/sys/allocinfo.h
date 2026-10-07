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
 * Definition of the Paragon OSF/1 allocinfo file format
 *
 * $Id: allocinfo.h,v 1.3 1994/11/18 20:39:59 mtm Exp $
 *
 * HISTORY
 * $Log: allocinfo.h,v $
 * Revision 1.3  1994/11/18  20:39:59  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/07/27  15:27:24  johannes
 * changed header
 *
 */

#ifndef __ALLOCINFO_H__
#define __ALLOCINFO_H__

#ifdef _KERNEL
#include <nx/nx_types.h>
#else
#include <allocsys.h>
#endif

#define ALLOC_MAGIC	0xdeadaabb	/* allocinfo file magic number */
#define ALLOC_SWAP	0xff00		/* byte swap field */
#define ALLOC_VERSION   1		/* initial allocinfo file version */

struct allocinfo_hdr {
    int			a_magic;	/* allocinfo file magic number */
    unsigned short	a_swap;		/* byte swap field */
    short		a_version;	/* allocinfo file version */
    nx_part_info_t	a_partition;	/* NX partition info */	
    nx_app_info_t	a_application;	/* NX application info */
 };
 
#endif
