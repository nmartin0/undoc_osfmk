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
 * $Log: poll.h,v $
 * Revision 1.4  1994/11/18  20:40:59  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:26:27  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:35:28  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:43:05  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:40:45  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:39:37  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:09:42  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:44:16  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/10/07  14:52:48  devrcs
 * 	added #include <standards.h>
 * 	[90/10/03  16:03:42  rabin]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:43:07  gm]
 * 
 * Revision 1.3  90/09/23  16:00:08  devrcs
 * 	added support for _AES_SOURCE
 * 	[90/09/07  14:36:43  rabin]
 * 
 * Revision 1.2  90/04/14  00:34:04  devrcs
 * 	Made to conform to the AES, and moved to normal directory.
 * 	[90/03/30  16:24:22  coren]
 * 
 * Revision 1.4  90/02/23  00:31:01  devrcs
 * 
 * 	No changes.
 * 	[90/02/18  19:21:42  nolting]
 * 
 * Revision 1.3  90/01/02  20:22:49  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  10:27:13  gm
 * 	Added streams support.
 * 	[89/12/25            gm]
 * 
 * $EndLog$
 */
/** Copyright (c) 1989  Mentat Inc.
 ** poll.h 1.1, last change 8/8/89
 **/

#include <standards.h>		/* define feature-test macros */

#ifdef _AES_SOURCE

/* Poll masks */
#define	POLLNORM	01	/* message available on read queue */
#define	POLLOUT		04	/* stream is writable */
#define	POLLERR		010	/* error message has arrived */
#define	POLLHUP		020	/* hangup has occurred */
#define	POLLNVAL	040	/* invalid fd */

#ifdef _OSF_SOURCE
#define POLLIN		POLLNORM
#define	POLLPRI		02	/* priority message available */
#endif /* _OSF_SOURCE */

/* array of streams to poll */
struct pollfd {
	int	fd;
	short	events;
	short	revents;
};

#endif /* _AES_SOURCE */

#ifdef _OSF_SOURCE

#define	NPOLLFILE	20

#endif /* _OSF_SOURCE */

