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
 * $Log: if_dl.h,v $
 * Revision 1.5  1994/11/18  20:33:36  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:07:15  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:27:34  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  18:59:30  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:26:27  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:25:03  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:18:43  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:32:27  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1991/08/31  13:39:25  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:32:53  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.3  90/10/07  14:31:37  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  16:05:38  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:09:51  gm]
 * 
 * Revision 1.2  90/07/27  08:58:44  devrcs
 * 	Created, from BSD Reno release.
 * 	[90/07/19  16:31:17  tmt]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1990 Regents of the University of California.
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
 *	Base:	if_dl.h	7.1 (Berkeley) 7/1/90
 */

/* 
 * A Link-Level Sockaddr may specify the interface in one of two
 * ways: either by means of a system-provided index number (computed
 * anew and possibly differently on every reboot), or by a human-readable
 * string such as "il0" (for managerial convenience).
 * 
 * Census taking actions, such as something akin to SIOCGCONF would return
 * both the index and the human name.
 * 
 * High volume transactions (such as giving a link-level ``from'' address
 * in a recvfrom or recvmsg call) may be likely only to provide the indexed
 * form, (which requires fewer copy operations and less space).
 * 
 * The form and interpretation  of the link-level address is purely a matter
 * of convention between the device driver and its consumers; however, it is
 * expected that all drivers for an interface of a given if_type will agree.
 */

/*
 * Structure of a Link-Level sockaddr:
 */
struct sockaddr_dl {
	u_char	sdl_len;	/* Total length of sockaddr */
	u_char	sdl_family;	/* AF_DLI */
	u_short	sdl_index;	/* if != 0, system given index for interface */
	u_char	sdl_type;	/* interface type */
	u_char	sdl_nlen;	/* interface name length, no trailing 0 reqd. */
	u_char	sdl_alen;	/* link level address length */
	u_char	sdl_slen;	/* link layer selector length */
	char	sdl_data[12];	/* minimum work area, can be larger;
				   contains both if name and ll address */
};

#define LLADDR(s) ((caddr_t)((s)->sdl_data + (s)->sdl_nlen))

