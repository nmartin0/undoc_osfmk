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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_appl.h,v 1.14 1994/11/18 20:43:49 mtm Exp $
 */

/*
 * mcmsg_appl.h
 *
 * Application info data types.
 */

#ifndef MCMSG_APPL_H
#define MCMSG_APPL_H

/*
 * Logical to physical node map
 */

 typedef long           LP_MAP_ENTRY_T;
/*
 *	conditionally compile LP_MAP_T so server can build.
 */
 typedef LP_MAP_ENTRY_T  *LP_MAP_T;

/*
 * Application information 
 */

 typedef struct applinfo {
	unsigned long	app;		/* application id */
	unsigned long	process_lock;	/* lock process data in memory */
	unsigned long	pkt_size;	/* message packet size */
	unsigned long	memory_buffer;	/* total message buffer to allocate */
	unsigned long	memory_export;	/* total buffer for other nodes */
	unsigned long	memory_each;	/* buffer available for each node */
	unsigned long	send_threshold;	/* send multiple packet threshold */
	unsigned long	send_count;	/* pkts to send when send_threshold */
	unsigned long	give_threshold;	/* send give message threshold */
	unsigned long   noc;		/* number of correspondents */
	unsigned long   rows;		/* number of rows in application */
	unsigned long   columns;	/* number of columns in application */
	unsigned long   unused[12];
 } APPLINFO_T;

#define PROVIDE_THRESHOLD (128 * 1024)  /* used in nx_initve.c and rklib.c */

#endif /* MCMSG_APPL_H */
