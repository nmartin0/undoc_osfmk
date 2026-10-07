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
 *		INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *	type.h 11.1 94/03/22 16:49:05
 */
static char	typeh_ver[] = "@(#) sourcefile type.h 11.1 94/03/22 16:49:05";

/*
 *
 *  ******************  ENGINEERING CHANGE HISTORY ***********************
 *
 *    Date      Engineer                  Description
 *  
 *  2/25/92     G. Haycox       Module creation release to BETA
 *
 *
 */

/* TYPE.H:
 *
 * This include file contains data type definitions
 *
 */


typedef unsigned char   ubit8;
/* typedef unsigned char	uchar; */

typedef unsigned short  ubit16;
typedef unsigned int    ubit32;

typedef char            bit8;
typedef short           bit16;
typedef int             bit32;


#ifndef	NULL
	#define NULL            0x0L
#endif


#ifndef FALSE
#define FALSE           0
#endif

#ifndef TRUE
#define TRUE            !FALSE
#endif

#define ROLLOVER        0x80000000



