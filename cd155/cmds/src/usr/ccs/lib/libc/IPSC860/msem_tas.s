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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.1
 */

        .file   "msem_tas.s"

#include <i860/asm.h>

/* 
 * $Id: msem_tas.s,v 1.3 1994/11/19 02:12:45 mtm Exp $
 *
 * Mutex implementation for i860
 *	int
 *	_msem_tas(lock)
 *	int	*lock;
 * Returns the original value of the lock variable (i.e. before it is set).
 *
 * entry:
 *	r16 == slock adrs
 * exit
 *	r16 == original value of the lock.
 *
 * HISTORY
 * $Log: msem_tas.s,v $
 * Revision 1.3  1994/11/19  02:12:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/19  18:23:08  stans
 * [ Mon Oct 19 11:23:04 PDT 1992 ] stan
 * msem_tas.s now correctly returns previous value of lock.
 *
 *
 */

ENTRY(_msem_tas)

	lock
        ld.l    0(r16),r18      // get lock contents
	// we OWN the bus & soon the lock
	or	1,r0,r17	// set OWNED value for the lock
	unlock
        st.l    r17,0(r16)      // reset lock contents, make it so....

	bri	r1
	  or	r18,r0,r16	// set return value

