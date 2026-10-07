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
/* @(#)rt_control.h   1.3 2/6/90 15:50:29 */
/*
 * COMPONENT_NAME: TCPIP rt_control.h
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
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/gated/rt_control.h,v 1.2 1994/11/19 03:06:48 mtm Exp $
 *
 */

#define MAXINTERFACE	25		/* Maximum number of interfaces */

struct restricthash {
	struct	restrictlist *rt_forw;
	struct	restrictlist *rt_back;
};

struct restrictlist {
	struct	restrictlist *rt_forw;
	struct	restrictlist *rt_back;
	u_long	rhash;
	struct	sockaddr_in rdst;
	int	rproto;
	int	regpmetric;
	int	flags;
	u_long  rintf[MAXINTERFACE];
};

#define RT_ANNOUNCE	0x1		/* announce control restriction */
#define RT_NOLISTEN	0x2		/* listen control restriction */
#define RT_SRCLISTEN	0x4		/* listen from source */
#define RT_NOANNOUNCE	0x8		/* noannounce control restriction */

struct as_entry {
	struct as_entry *next;
	u_short as;
	u_short flags;
};

struct as_list {
	struct as_list *next;
	u_short as;
	u_short flags;
	struct as_entry *as_ptr;
};

#define	AS_SEND		0x1		/* Can send to this AS */
#define AS_DONOTSEND	0x2		/* Can not send to this AS */
#define	AS_RESTRICT	0x4		/* Announcement restrictions apply */

struct as_valid {
	struct as_valid *next;
	struct in_addr dst;
	u_short as;
	u_short metric;
};

#define MARTIAN_NETS	static char *martian_nets[7] = {\
	"127.0.0.0",\
	"128.0.0.0",\
	"191.255.0.0",\
	"192.0.0.0",\
	"223.255.255.0",\
	"224.0.0.0",\
	(char *)0 }		/* This is the end of the table, not default net */
