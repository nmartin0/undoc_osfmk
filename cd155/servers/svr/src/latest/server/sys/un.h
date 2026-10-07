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
 * $Log: un.h,v $
 * Revision 1.4  1994/11/18  20:42:03  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:30:25  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:39:29  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:45:16  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:42:53  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:41:57  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:16:06  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:47:08  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.8  90/10/07  14:56:11  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:48:20  gm]
 * 
 * Revision 1.7  90/07/27  09:08:28  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  15:51:39  tmt]
 * 
 * Revision 1.6  90/04/27  19:27:46  devrcs
 * 	Change name of 4.3/4.4 sockaddr compat token.
 * 	[90/04/20  12:09:10  tmt]
 * 
 * Revision 1.5  90/04/14  00:34:15  devrcs
 * 	Do new sockaddr with SOCKADDR_LEN || _KERNEL
 * 	[90/04/09  15:46:39  tmt]
 * 
 * Revision 1.4  90/01/18  08:49:08  gm
 * 	Make file re-entrant.
 * 	Drop path length field to match BSD4.4 update.
 * 	Add compat option for osockaddr like netinet/in.h.
 * 	[90/01/08  15:43:20  tmt]
 * 
 * 	OSF/1 "one" snapshot revision.
 * 	[90/01/02  12:00:00  tmt]
 * 
 * 	- Base is BSD 4.4 (Alpha) networking.
 * 	- Encore multiprocessing merged in with some structural
 * 	  modifications to support flexible configuration.
 * 	- Glue for compiling and running in MACH or Unix 4.4 environments,
 * 	  lock testing under Unix, thread or software interrupt netisr's,
 * 	  locking and/or spl synchronization, single or multiple CPUs.
 * 	[89/12/20  12:00:00  tmt]
 * 
 * Revision 1.3  90/01/02  20:16:13  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  10:42:35  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted provided
 * that: (1) source distributions retain this entire copyright notice and
 * comment, and (2) distributions including binaries display the following
 * acknowledgement:  ``This product includes software developed by the
 * University of California, Berkeley and its contributors'' in the
 * documentation or other materials provided with the distribution and in
 * all advertising materials mentioning features or use of this software.
 * Neither the name of the University nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	Base:	un.h	7.4 (Berkeley) 9/4/89
 *	Merged: un.h	7.7 (Berkeley) 6/28/90
 */

#ifndef	_SYS_UN_H_
#define	_SYS_UN_H_

/*
 * Definitions for UNIX IPC domain.
 */
#if	defined(_SOCKADDR_LEN) || defined(_KERNEL)
struct	sockaddr_un {
	u_char	sun_len;		/* sockaddr len including null */
	u_char	sun_family;		/* AF_UNIX */
	char	sun_path[104];		/* path name (gag) */
};
#else
struct	sockaddr_un {
	u_short	sun_family;		/* AF_UNIX */
	char	sun_path[104];		/* path name (gag) */
};
#endif

/* actual length of an initialized sockaddr_un */
#define SUN_LEN(su) \
	(sizeof(*(su)) - sizeof((su)->sun_path) + strlen((su)->sun_path))

#endif
