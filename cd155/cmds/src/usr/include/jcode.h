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
/* @(#)jcode.h	1.4  com/inc,3.1,9013 12/20/89 13:23:33 */
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Library Support 
 *
 * FUNCTIONS: 
 *
 * ORIGINS: 27
 *
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*
 *   jcode.h: definitions for jis/sj/uj conversion routines
 */

#ifndef _JCODE_H_
#define _JCODE_H_

extern unsigned char
		*cjistosj(),	/* character conversion */
		*cjistouj(),
		*csjtojis(),
		*csjtouj(),
		*cujtojis(),
		*cujtosj(),

		*jistosj(),	/* string conversion */
		*jistouj(),
		*sjtojis(),
		*sjtouj(),
		*ujtojis(),
		*ujtosj();

#endif /* _JCODE_H_ */
