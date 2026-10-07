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
 *	Author: Stan Smith
 *	Date: 	9-30-92
 *
 *	based on ideas from device/device_master.h
 *
 * 	Temporarily bind a thread to the master CPU.
 *
 * $Id: cpu_master.h,v 1.4 1994/11/18 20:38:29 mtm Exp $
 *
 * HISTORY:
 * $Log: cpu_master.h,v $
 * Revision 1.4  1994/11/18  20:38:29  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/03/19  01:00:50  lenb
 *  Reviewer: sean
 *  Risk: low
 *  Benefit: if (NCPUS > 1 && !ASMP) then #define FORCE_MASTER
 * decouples FORCE_MASTER feature from the type of MP kernel
 *  Testing: SAT
 *
 * Revision 1.2  1993/06/30  22:30:14  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.1  1992/11/30  17:56:04  stans
 * Initial revision
 *
 */

#include <cpus.h>

#if	NCPUS > 1
#ifdef	ASMP
#else	ASMP
#define	FORCE_MASTER 1
#endif	ASMP
#endif	/* NCPUS > 1 && !ASMP */

#ifdef	FORCE_MASTER

#define	FORCE_MASTER_CPU() force_master_cpu()
#define	RELEASE_MASTER_CPU() release_master_cpu()

#if	MACH_ASSERT
#define	MUST_BE_MASTER(string, caller) must_be_master(string, caller)
#else	/* MACH_ASSERT */
#define	MUST_BE_MASTER(string, caller)
#endif	/* MACH_ASSERT */

#else	/* FORCE_MASTER */

#define	FORCE_MASTER_CPU()
#define	RELEASE_MASTER_CPU()
#define	MUST_BE_MASTER(string, caller)

#endif	/* FORCE_MASTER */

