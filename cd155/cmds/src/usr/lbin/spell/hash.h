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
/*    @(#)$RCSfile: hash.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:48:08 $ */
/*
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */ 
/*  hash.h	1.2  11/18/89 17:09:07"   */
/*
 * COMPONENT_NAME: (CMDTEXT) Text Formatting Services
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3
 *
 * OBJECT CODE ONLY SOURCE MATERIALS
 *
 */
#define HASHWIDTH 27
#define HASHSIZE 134217689L	/*prime under 2^HASHWIDTH*/
#define INDEXWIDTH 9
#define INDEXSIZE (1<<INDEXWIDTH)
#define NI (INDEXSIZE+1)
#define ND ((25750/2)*sizeof(*table))
#define BYTE 8

extern unsigned *table;
extern int index[];	/*into dif table based on hi hash bits*/

extern long hash();
