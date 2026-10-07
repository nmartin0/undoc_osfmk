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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: xpr.c,v $
 * Revision 1.5  1994/11/18  20:32:57  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:02:20  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:22:06  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:17:33  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:31:06  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:23:35  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:22:29  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.2  1992/03/09  16:06:10  durriya
 * 	initial check-in from grV3.5
 *
 *
 */
/*
 * xpr silent tracing circular buffer.
 */

#include <kern/xpr.h>
#include <kern/lock.h>
#include <sys/user.h>

decl_simple_lock_data(,xprlock)

int nxprbufs = 0;	/* Number of contiguous xprbufs allocated */
unsigned int xprflags = 0;	/* Bit mask of xpr flags enabled */
struct xprbuf *xprbase;	/* Pointer to circular buffer nxprbufs*sizeof(xprbuf)*/
struct xprbuf *xprptr;	/* Currently allocated xprbuf */
struct xprbuf *xprlast;	/* Pointer to end of circular buffer */

/*VARARGS1*/
xpr(msg, arg1, arg2, arg3, arg4, arg5, arg6)
	char *msg;
{
	register s;
	register struct xprbuf *x;

#ifdef	lint
	arg6++;
#endif	lint

	/* If we aren't initialized, ignore trace request */
	if (!nxprbufs || !xprbase)
		return;
	/* Gaurd against all interrupts and allocate next buffer */
	s = splhigh();
	simple_lock(&xprlock);
	x = xprptr++;
	if (xprptr >= xprlast) {
		/* wrap around */
		xprptr = xprbase;
	}
	simple_unlock(&xprlock);
	splx(s);
	x->msg = msg;
	x->arg1 = arg1;
	x->arg2 = arg2;
	x->arg3 = arg3;
	x->arg4 = arg4;
	x->arg5 = arg5;
	x->thread = (int)current_thread();
}

xprbootstrap()
{
	simple_lock_init(&xprlock);
	if (nxprbufs == 0)
		return;	/* assume XPR support not desired */
	xprbase = (struct xprbuf *)kalloc(nxprbufs * sizeof(struct xprbuf));
	xprlast = &xprbase[nxprbufs];
	xprptr = xprbase;
}

int		xprinitial = 0;

xprinit()
{
	xprflags |= xprinitial;
}

/*
 *	Save 'number' of xpr buffers to the area provided by buffer.
 */

xpr_save(buffer,number)
struct xprbuf *buffer;
int number;
{
    int i,s;
    struct xprbuf *x;
    s = splhigh();
    simple_lock(&xprlock);
    if (number > nxprbufs) number = nxprbufs;
    x = xprptr;
    
    for (i = number - 1; i >= 0 ; i-- ) {
	if (--x < xprbase) {
	    /* wrap around */
	    x = xprlast - 1;
	}
	bcopy(x,&buffer[i],sizeof(struct xprbuf));
    }
    simple_unlock(&xprlock);
    splx(s);
}

/*
 *	Print current content of xpr buffers (KDB's sake)
 *	Use stack order to make it understandable
 */
xpr_dump(nbufs)
{
	register struct xprbuf *x;
	int             s,i;

	if (!nxprbufs || !xprbase)
		return;
	s = splhigh();
	simple_lock(&xprlock);

	if (nbufs <= 0 || nbufs > nxprbufs)
		nbufs = nxprbufs;
	for (x = xprptr, i = nbufs; i > 0; i--) {
		if (x->msg == 0)
			break;
		printf("%d:<%x> ", nbufs-i, x->thread);
		printf(x->msg, x->arg1,x->arg2,x->arg3,x->arg4,x->arg5);
		if (--x < xprbase)
			x = xprlast - 1;
	} 

	simple_unlock(&xprlock);
	splx(s);
}












