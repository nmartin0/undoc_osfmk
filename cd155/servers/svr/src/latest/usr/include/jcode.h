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
 * $Log: jcode.h,v $
 * Revision 1.3  1994/11/18  21:09:48  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:14:35  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:57:36  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:16:02  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:18:42  nandy
 * Initial revision
 *
 * Revision 1.3  92/04/13  14:35:49  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.3  90/10/07  20:28:11  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:46:29  gm]
 * 
 * Revision 1.2  90/04/27  23:06:29  devrcs
 * 	New file for KJI support.
 * 	[90/04/16  13:47:47  tom]
 * 
 * $EndLog$
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
