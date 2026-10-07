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
 * $Log: igmp_var.h,v $
 * Revision 1.4  1994/11/18  20:34:42  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/05/06  20:25:48  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:33:28  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:28:26  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:26:50  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:20:28  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:42:44  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:34:59  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/10/07  14:34:04  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  16:06:00  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:13:36  gm]
 * 
 * Revision 1.3  90/07/27  09:00:33  devrcs
 * 	IGMP driver for multicast group maintenance.
 * 	[90/07/19  17:27:04  tmt]
 * 
 * $EndLog$
 */
/*
 * Internet Group Management Protocol (IGMP),
 * implementation-specific definitions.
 *
 * Written by Steve Deering, Stanford, May 1988.
 *
 * MULTICAST 1.1
 */

struct igmpstat {
	u_int	igps_rcv_total;		/* total IGMP messages received    */
	u_int	igps_rcv_tooshort;	/* received with too few bytes     */
	u_int	igps_rcv_badsum;	/* received with bad checksum      */
	u_int	igps_rcv_queries;	/* received membership queries     */
	u_int	igps_rcv_badqueries;	/* received invalid queries        */
	u_int	igps_rcv_reports;	/* received membership reports     */
	u_int	igps_rcv_badreports;	/* received invalid reports        */
	u_int	igps_rcv_ourreports;	/* received reports for our groups */
	u_int	igps_snd_reports;	/* sent membership reports         */
#if	defined(_KERNEL) && LOCK_NETSTATS
	simple_lock_data_t igps_lock;	/* statistics lock */
#endif
};

#ifdef _KERNEL
extern struct igmpstat igmpstat;

/*
 * Macro to compute a random timer value between 1 and (IGMP_MAX_REPORT_DELAY
 * * countdown frequency).  We generate a "random" number by adding
 * the total number of IP packets received, our primary IP address, and the
 * multicast address being timed-out.  The 4.3 random() routine really
 * ought to be available in the kernel!
 */
#define IGMP_RANDOM_DELAY(multiaddr)					\
	/* struct in_addr multiaddr; */					\
	( (ipstat.ips_total +						\
	   ntohl(IA_SIN(in_ifaddr)->sin_addr.s_addr) +			\
	   ntohl((multiaddr).s_addr)					\
	  )								\
	  % (IGMP_MAX_HOST_REPORT_DELAY * PR_FASTHZ) + 1		\
	)

#endif
