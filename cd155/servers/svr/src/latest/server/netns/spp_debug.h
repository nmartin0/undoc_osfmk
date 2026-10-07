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
 * $Log: spp_debug.h,v $
 * Revision 1.5  1994/11/18  20:36:38  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:14:27  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:36:33  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:02:45  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:31:30  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:29:25  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:23:47  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:35:15  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1991/08/31  13:49:26  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:39:16  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.5  90/10/07  14:38:22  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:20:08  gm]
 * 
 * Revision 1.4  90/07/27  09:03:56  devrcs
 * 	XNS protocols, passthrough from BSD Reno release.
 * 	Operating under uniprocessor compat in OSF/1.
 * 	[90/07/19  17:19:04  tmt]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1984, 1985, 1986, 1987 Regents of the University of California.
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
 *	Base:	spp_debug.h	7.4 (Berkeley) 6/28/90
 */

struct	spp_debug {
	u_long	sd_time;
	short	sd_act;
	short	sd_ostate;
	caddr_t	sd_cb;
	short	sd_req;
	struct	spidp sd_si;
	struct	sppcb sd_sp;
};

#define	SA_INPUT 	0
#define	SA_OUTPUT	1
#define	SA_USER		2
#define	SA_RESPOND	3
#define	SA_DROP		4

#ifndef	CONST
#define CONST
#endif

#ifdef SANAMES
CONST	char	*sanames[] =
    { "input", "output", "user", "respond", "drop" };
#endif

#define	SPP_NDEBUG 100
extern	struct	spp_debug spp_debug[SPP_NDEBUG];
extern	int	spp_debx;
