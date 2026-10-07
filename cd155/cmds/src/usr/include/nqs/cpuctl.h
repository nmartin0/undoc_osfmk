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
 * Copyright (c) 1990 San Diego Supercomputer Center.
 * All rights reserved.  The SDSC software License Agreement
 * specifies the terms and conditions for redistribution.
 */
/*
 * HISTORY
 * $Log: cpuctl.h,v $
 * Revision 1.2  1994/11/19  02:43:52  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 *
 */

/* reply header fields */
#define reply_return 	(*(reply_intptr))
#define reply_errno	(*(reply_intptr+1))
#define reply_datasize	(*(reply_intptr+2))

/* request header fields */
#define reqst_reqnum	(*(reqst_intptr))
#define reqst_datasize	(*(reqst_intptr+1))

/* header size */
#define C_REQUEST_HDRLEN	2*sizeof(int)
#define C_REPLY_HDRLEN		3*sizeof(int)
