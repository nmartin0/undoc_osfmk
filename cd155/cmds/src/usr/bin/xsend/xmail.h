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
/*	xmail.h	4.1	83/03/30	*/

#include <stdio.h>
#include <mp.h>
extern MINT *x, *b, *one, *c64, *t45, *z, *q, *r, *two, *t15;
extern char buf[256];
#ifdef debug
#define nin(x, y) m_in(x, 8, y)
#define nout(x, y) m_out(x, 8, y)
#endif
