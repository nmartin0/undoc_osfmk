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
 * $Log: igmp.h,v $
 * Revision 1.4  1994/11/18  20:34:41  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/05/06  20:25:44  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:33:27  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:28:24  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:26:47  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:20:26  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:42:40  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:34:55  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/10/07  14:33:59  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  16:05:52  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:13:30  gm]
 * 
 * Revision 1.3  90/07/27  09:00:30  devrcs
 * 	IGMP driver for multicast group maintenance.
 * 	[90/07/19  17:26:43  tmt]
 * 
 * $EndLog$
 */
/*
 * Internet Group Management Protocol (IGMP) definitions.
 *
 * Written by Steve Deering, Stanford, May 1988.
 *
 * MULTICAST 1.1
 */

/*
 * IGMP packet format.
 */
struct igmp {
	u_char		igmp_type;	/* version & type of IGMP message  */
	u_char		igmp_code;	/* unused, should be zero          */
	u_short		igmp_cksum;	/* IP-style checksum               */
	struct in_addr	igmp_group;	/* group address being reported    */
};					/*  (zero for queries)             */

#define IGMP_MINLEN		     8

#define IGMP_HOST_MEMBERSHIP_QUERY   0x11  /* message types, incl. version */
#define IGMP_HOST_MEMBERSHIP_REPORT  0x12
#define IGMP_DVMRP		     0x13  /* for experimental multicast   */
					   /*  routing protocol            */

#define IGMP_MAX_HOST_REPORT_DELAY   10    /* max delay for response to    */
					   /*  query (in seconds)          */
