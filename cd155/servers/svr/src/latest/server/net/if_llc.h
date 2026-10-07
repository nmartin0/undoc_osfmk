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
 * $Log: if_llc.h,v $
 * Revision 1.5  1994/11/18  20:33:41  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:07:30  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:27:46  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  18:59:33  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:26:32  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:25:10  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:18:48  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:32:30  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1991/08/31  13:39:40  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:33:09  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.7  90/10/07  14:31:46  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:10:05  gm]
 * 
 * Revision 1.6  90/07/27  08:58:51  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  16:33:31  tmt]
 * 
 * Revision 1.5  90/04/27  19:12:00  devrcs
 * 	Checkpoint.
 * 	[90/04/20  12:24:02  tmt]
 * 
 * Revision 1.4  90/01/18  08:43:25  gm
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
 * Revision 1.3  90/01/02  20:10:49  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  09:44:21  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1988 Regents of the University of California.
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
 *	Base:	if_llc.h	7.1 (Berkeley) 9/4/89
 *	Merged:	if_llc.h	7.2 (Berkeley) 6/28/90
 */

/*
 * IEEE 802.2 Link Level Control headers, for use in conjunction with
 * 802.{3,4,5} media access control methods.
 *
 * Headers here do not use bit fields due to shortcommings in many
 * compilers.
 */

struct llc {
	u_char	llc_dsap;
	u_char	llc_ssap;
	union {
	    struct {
		u_char control;
		u_char format_id;
		u_char class;
		u_char window_x2;
	    } type_u;
	    struct {
		u_char num_snd_x2;
		u_char num_rcv_x2;
	    } type_i;
	    struct {
		u_char control;
		u_char num_rcv_x2;
	    } type_s;
	    struct {
		u_char control;
		u_char org_code[3];
		u_short ether_type;
	    } type_snap;
	} llc_un;
};
#define llc_control llc_un.type_u.control
#define llc_fid llc_un.type_u.format_id
#define llc_class llc_un.type_u.class
#define llc_window llc_un.type_u.window_x2

#define llc_snap llc_un.type_snap

#define LLC_UI		0x3
#define LLC_UI_P	0x13
#define LLC_XID		0xaf
#define LLC_XID_P	0xbf
#define LLC_TEST	0xe3
#define LLC_TEST_P	0xf3

#define LLC_ISO_LSAP	0xfe
#define LLC_SNAP_LSAP	0xaa
