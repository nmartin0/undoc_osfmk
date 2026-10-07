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
/* @(#)$RCSfile: diff.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:22:39 $ */
/*
 * COMPONENT_NAME: (CMDFILES) commands that manipulate files
 *
 * FUNCTIONS: 
 *
 * ORIGINS: 3, 26, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * diff.h	1.2  com/cmd/files/diff,3.1,9008 9/11/89 15:43:43 
 */

#include "diff_msg.h"
#define MSGSTR(Num,Str) catgets(catd,MS_DIFF,Num,Str)

#define	DI_NORMAL	0	/* Normal output */
#define	DI_EDIT		-1	/* Editor script out */
#define	DI_REVERSE	1	/* Reverse editor script */
#define	DI_CONTEXT	2	/* Diff with context */
#define	DI_IFDEF	3	/* Diff with merged #ifdef's */
#define	DI_NREVERSE	4	/* Reverse ed script with numbered */

