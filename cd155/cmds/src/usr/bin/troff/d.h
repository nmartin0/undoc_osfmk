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
 *
 *  $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/troff/d.h,v 1.2 1994/11/19 01:43:55 mtm Exp $
 */
/*	d.h	4.2	83/08/11	*/

struct d {filep op; int dnl,dimac,ditrap,ditf,alss,blss,nls,mkline,
		maxl,hnl,curd;} d[NDI], *dip;
