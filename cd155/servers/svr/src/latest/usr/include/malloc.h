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
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: malloc.h,v $
 * Revision 1.3  1994/11/18  21:10:20  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:15:47  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:58:34  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:17:05  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:19:41  nandy
 * Initial revision
 *
 * Revision 1.3  92/04/13  14:37:17  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.3  90/10/07  20:31:07  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:50:50  gm]
 * 
 * Revision 1.2  90/03/13  21:22:43  mbrown
 * 	AIX merge first cut - new file.
 * 	[90/02/12  18:23:23  tom]
 * 
 * $EndLog$
 */
/* @(#)malloc.h	1.8  com/inc,3.1,8943 4/11/89 17:15:35 */
/*
 * COMPONENT_NAME: malloc.h
 *                                                                    
 * ORIGIN: IBM
 *
 * Copyright International Business Machines Corp. 1985, 1988
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the Rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */                                                                   

#ifndef _MALLOC_H_
#define _MALLOC_H_
/*
	Constants defining mallopt operations
*/
#define M_MXFAST	1	/* set size of blocks to be fast */
#define M_NLBLKS	2	/* set number of block in a holding block */
#define M_GRAIN		3	/* set number of sizes mapped to one, for
				   small blocks */
#define M_KEEP		4	/* retain contents of block after a free until
				   another allocation */
/*
	structure filled by 
*/
struct mallinfo  {
	int arena;	/* total space in arena */
	int ordblks;	/* number of ordinary blocks */
	int smblks;	/* number of small blocks */
	int hblks;	/* number of holding blocks */
	int hblkhd;	/* space in holding block headers */
	int usmblks;	/* space in small blocks in use */
	int fsmblks;	/* space in free small blocks */
	int uordblks;	/* space in ordinary blocks in use */
	int fordblks;	/* space in free ordinary blocks */
	int keepcost;	/* cost of enabling keep option */
};	

#define valloc(s)	(char *)malloc(s)
#ifdef   _NO_PROTO
extern int mallopt();
extern struct mallinfo mallinfo();
#else  /*_NO_PROTO */
extern int mallopt(int, int);
extern struct mallinfo mallinfo(void);
#endif /*_NO_PROTO */

#endif /* _MALLOC_H_ */
