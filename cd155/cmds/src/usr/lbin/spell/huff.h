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
/* "@(#)$RCSfile: huff.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:48:17 $"; */
/*
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */ 
/*  huff.h	1.2  11/18/89 17:29:01"  */
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
extern struct huff {
	long xn;
	int xw;
	long xc;
	long xcq;	/* (c,0) */
	long xcs;	/* c left justified */
	long xqcs;	/* (q-1,c,q) left justified */
	long xv0;
} huffcode;
#define n huffcode.xn
#define w huffcode.xw
#define c huffcode.xc
#define cq huffcode.xcq
#define cs huffcode.xcs
#define qcs huffcode.xqcs
#define v0 huffcode.xv0
