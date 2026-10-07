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
/* @(#)messages.h	1.4  com/cmd/prog/common,3.1,9013 9/15/89 19:34:53 */
/*
 * COMPONENT_NAME: (CMDPROG) messages.h
 *
 * FUNCTIONS: MESSAGE                                                        
 *
 * ORIGINS: 27 03 09 32 00 
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Changes for ANSI C were developed by HCR Corporation for IBM
 * Corporation under terms of a work made for hire contract.
 */

/* AIWS C compiler */

#ifndef UERROR
#    define	UERROR	uerror
#endif

#ifndef WERROR
#    define	WERROR	werror
#endif

#ifndef WARNING
#    define	WARNING	warning
#endif

#ifndef MESSAGE
#ifdef MSG
#define         MESSAGE(x) NLcatgets(catd, MS_CTOOLS, x+1, msgtext[ x ])
#else
#    define	MESSAGE(x)	msgtext[ x ]
#endif	/* MSG */
#endif

extern char	*msgtext[ ];

#define ALWAYS	1
