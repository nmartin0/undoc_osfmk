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
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/* @(#)jctype0.h	1.1  com/inc/sys,3.1,8943 10/11/89 12:58:26 */
/*
 *  COMPONENT_NAME: (INCSYS) definition of _jctype0_ Kanji table
 *
 *  FUNCTIONS: _jctype0_
 *
 *  ORIGINS: 10
 *
 *  (C) COPYRIGHT International Business Machines Corp.  1986, 1989
 *  All Rights Reserved
 *  Licensed Materials - Property of IBM
 *
 *  US Government Users Restricted Rights - Use, duplication or
 *  disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 */
/*
 * OSF/1 Release 1.0
 */

#ifndef _SYS_JCTYPE_H_
#define _SYS_JCTYPE_H_

	/* Upper-byte table, used to select a row of the main table.
	 * NOTE:  DO NOT MOVE subtable 7--it must remain the table which
	 * indicates valid lower bytes.  (See jctype.h). */
unsigned char _jctype0_[256] = {
	/* one-byte codes, followed by 127 illegal upper bytes */
/*0x*/	1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
/*1x*/	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
/*2x*/	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
/*3x*/	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
/*4x*/	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
/*5x*/	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
/*6x*/	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
/*7x*/	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
/*8x*/	0,					/* reserved (won't be used) */
	   2,
	      3,
		 4,
		     5,
		       12,12,12,		/* unused wards */
	/* Kanji Level 1 */
				  6, 7, 7, 7,  7, 7, 7, 7,
/*9x*/	7, 7, 7, 7,  7, 7, 7, 7,  8,
	/* Kanji Level 2 */
				     7, 7, 7,  7, 7, 7, 7,
	/* "hole" corresponding to one-byte katakana (a0-df) */
/*ax*/	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
/*bx*/	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
/*cx*/	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
/*dx*/	0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
/*ex*/	7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 9,
	/* reserved for two byte expansion */
					  12, 12,12,12,12,
/*fx*/ 12,12,12,12, 12,12,12,12, 12,12,
	/* IBM-defined Kanji */
				       10, 7, 11,
	/* reserved (won't be used) */
						  0, 0, 0
};
#endif /* _SYS_JCTYPE_H_ */
