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
/* @(#)hello.h   1.3 2/6/90 15:37:08 */
/*
 * COMPONENT_NAME: TCPIP hello.h
 *
 * FUNCTIONS: 
 *
 * ORIGINS: 10 26 27 39 36
 *
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 *   CENTER FOR THEORY AND SIMULATION IN SCIENCE AND ENGINEERING
 *			CORNELL UNIVERSITY
 *
 *      Portions of this software may fall under the following
 *      copyrights: 
 *
 *	Copyright (c) 1983 Regents of the University of California.
 *	All rights reserved.  The Berkeley software License Agreement
 *	specifies the terms and conditions for redistribution.
 *
 *  GATED - based on Kirton's EGP, UC Berkeley's routing daemon (routed),
 *	    and DCN's HELLO routing Protocol.
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/gated/hello.h,v 1.2 1994/11/19 03:06:37 mtm Exp $
 *
 */

#ifndef IPPROTO_HELLO
#define		IPPROTO_HELLO	63
#endif
#define		DELAY_INFINITY	30000		/* in ms */
#define		HELLO_TIMERRATE	15		/* in seconds */
#define		HELLO_HYST(s)	(int)(s*.25)	/* 25% of old route, in ms */

#define		HELLO_DEFAULT	0		/* net 0 as default */

#define		METRIC_DIFF(x,y)	(x > y ? x - y : y - x)

/*		Define the DCN HELLO protocol packet			*/

struct hellohdr {
		u_short	h_cksum;
		u_short h_date;
		u_long	h_time;
		u_short	h_tstp;
		} ;

/*
struct m_hdr {
		u_char	m_count ;
		u_char	m_type ;
		} ;
*/
struct type0pair {
		u_short d0_delay;
		u_short	d0_offset;
		} ;
			

struct type1pair {
		struct in_addr d1_dst;
		u_short	d1_delay ;
		short	d1_offset ;
		} ;


