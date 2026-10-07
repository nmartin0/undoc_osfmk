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
 * $Log: inet_config.h,v $
 * Revision 1.4  1994/11/18  20:34:59  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/05/06  20:26:31  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:33:46  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:28:54  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:27:14  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:20:58  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:43:23  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:35:45  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/12/06  14:03:18  devrcs
 * 	Fixed up HISTORY marker.
 * 	[90/11/04  17:15:11  gm]
 * 
 * Revision 1.3  90/10/07  14:34:45  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:14:39  gm]
 * 
 * Revision 1.2  90/09/23  15:55:51  devrcs
 * 	Make config version different from INET.
 * 	[90/09/15  15:41:58  tmt]
 * 
 * 	Created, configuration structure for INET protocol family.
 * 	[90/09/15  14:04:37  tmt]
 * 
 * $EndLog$
 */

#define INET_CONFIG_VERSION_1	0x01091590

#define IN_DEFAULT_VALUE	-12345
#define IN_USEVALUE		0x01
#define IN_USEDEFAULTS		0x02

typedef struct inet_config {

	int	version;
	int	errcode;
	int	flags;

	int	inetprintfs;		/* If configured, enable printfs (0) */
	int	useloopback;		/* Use loopback for own packets (1) */

	int	ipgateway;		/* Configure as gateway (0) */
	int	ipforwarding;		/* Act as gateway (0) */
	int	ipsendredirects;	/* Send ICMP redirects (1) */
	int	ipdirected_broadcast;	/* Broadcasts accepted uniquely (0) */
	int	ipsrcroute;		/* Enable host source routing (1) */
	int	subnetsarelocal;	/* Subnets appear as connected (1) */
	int	ipqmaxlen;		/* Length of IP input queue (50) */

	/* All times in SECONDS */

	int	tcpttl;			/* Default time to live (60) */
	int	tcpmssdflt;		/* Default max segsize (512) */
	int	tcprttdflt;		/* Default initial rtt (3) */
	int	tcpkeepidle;		/* Keepalive idle timer (7200) */
	int	tcpkeepintvl;		/* Keepalive interval (75) */
	int	tcpcompat_42;		/* BSD4.2 compat keepalive/urg (1) */
	int	tcprexmtthresh;		/* Retransmit threshold (3) */
	int	tcpconsdebug;		/* If configured, debug printfs (0) */
	u_long	tcp_sendspace;		/* Default send queue (4096) */
	u_long	tcp_recvspace;		/* Default receive queue (4096) */

	int	udpttl;			/* Default time to live (60) */
	int	udpcksum;		/* Enable checksumming (1) */
	u_long	udp_sendspace;		/* Default send queue (4096) */
	u_long	udp_recvspace;		/* Default receive queue (4096) */

	int	arpkillc;		/* Time to remove completed (1200) */
	int	arpkilli;		/* Time to remove incomplete (180) */
	int	arprefresh;		/* Time to refresh entry (120) */
	int	arphold;		/* Time to hold packet (5) */
	int	arplost;		/* Count to broadcast refresh (3) */
	int	arpdead;		/* Count to assume dead (6) */
	int	arpqmaxlen;		/* Length of ARP input queue (50) */
	int	arptabbsiz;		/* Table bucket size (16/9 gw/!gw) */
	int	arptabnb;		/* Number of buckets (37/19 gw/!gw) */
} inet_config_t;
