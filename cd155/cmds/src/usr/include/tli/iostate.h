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
 * (c) Copyright 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
 */
/*
 * OSF/1 Release 1.0.4
 */

/** Copyright (c) 1990  Mentat Inc.
 ** iostate.h 1.1, last change 4/14/90
 **/

#ifndef _IOSTATE_H
#define _IOSTATE_H

#ifndef	IOSTATE_LOCK
#define	IOSTATE_LOCK
#define	IOSTATE_UNLOCK
#endif

#define IOSTATE_LOOKUP          0x0001
#define IOSTATE_VERIFY  IOSTATE_LOOKUP
#define IOSTATE_CREATE          0x0011
#define IOSTATE_SETFLAG         0x0021
#define IOSTATE_CLEARFLAG       0x0041
#define IOSTATE_SET             0x0081
#define IOSTATE_USTATE          0x0101
#define IOSTATE_FREE            0x0100

#define IO_MORE_RUDATA		0x0020

#endif
