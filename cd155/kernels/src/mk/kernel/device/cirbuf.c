/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: cirbuf.c,v $
 * Revision 1.5  1994/11/18  20:30:33  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:21:34  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:19:50  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:19:28  dleslie
 * First R1_0 release
 *
 * Revision 2.5.3.1  92/03/28  10:04:26  jeffreyh
 * 	04-Mar-92  emcmanus at gr.osf.org
 * 		Added new cb_space function to report how much more space there is
 * 		in a buffer.
 * 		Fixed a bug where getc could return negative characters if char is
 * 		signed.
 * 	[92/03/10  07:56:00  bernadat]
 * 
 * Revision 2.5  91/07/31  17:32:09  dbg
 * 	Put CB_CHECK under debugging switch.
 * 	[91/07/30  16:45:34  dbg]
 * 
 * Revision 2.4  91/05/14  15:39:25  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:08:01  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  17:26:33  mrt]
 * 
 * Revision 2.2  90/08/27  21:54:30  dbg
 * 	q_to_b was always emptying the circular buffer; fix it.
 * 	[90/08/06            dbg]
 * 	Created.
 * 	[90/07/09            dbg]
 * 
 */
/*
 *	Author: David B. Golub, Carnegie Mellon University
 *	Date: 	7/90
 *
 * 	Circular buffers for TTY
 */

#include <device/cirbuf.h>
#include <kern/kalloc.h>

/* read at c_cf, write at c_cl */
/* if c_cf == c_cl, buffer is empty */
/* if c_cl == c_cf - 1, buffer is full */

#if	DEBUG
#define	CB_CHECK(cb) cb_check(cb)

cb_check(cb)
	register struct cirbuf *cb;
{
	if (!(cb->c_cf >= cb->c_start && cb->c_cf < cb->c_end))
	    panic("cf %x out of range [%x..%x)",
		cb->c_cf, cb->c_start, cb->c_end);
	if (!(cb->c_cl >= cb->c_start && cb->c_cl < cb->c_end))
	    panic("cl %x out of range [%x..%x)",
		cb->c_cl, cb->c_start, cb->c_end);
	if (cb->c_cf <= cb->c_cl) {
	    if (!(cb->c_cc == cb->c_cl - cb->c_cf))
		panic("cc %x should be %x",
			cb->c_cc,
			cb->c_cl - cb->c_cf);
	}
	else {
	    if (!(cb->c_cc == cb->c_end - cb->c_cf
			    + cb->c_cl - cb->c_start))
		panic("cc %x should be %x",
			cb->c_cc,
			cb->c_end - cb->c_cf +
			cb->c_cl - cb->c_start);
	}
}
#else	/* DEBUG */
#define	CB_CHECK(cb)
#endif	/* DEBUG */

/*
 * Put one character in circular buffer.
 */
int
putc(c, cb)
	int	c;
	register struct cirbuf *cb;
{
	register char *ow, *nw;

	ow = cb->c_cl;
	nw = ow+1;
	if (nw == cb->c_end)
	    nw = cb->c_start;
	if (nw == cb->c_cf)
	    return (1);		/* not entered */
	*ow = c;
	cb->c_cl = nw;

	cb->c_cc++;

	CB_CHECK(cb);

	return (0);
}

/*
 * Return number of successful putc()s we can do before buffer is full.
 */
int
cb_space(cb)
	register struct cirbuf *cb;
{
	int space = cb->c_cf - cb->c_cl - 1;
	if (space < 0)
		space += cb->c_end - cb->c_start;
	return space;
	/*
	 * Reasoning: if cl < cf, the space is the number of intervening
	 * characters (cf - cl - 1); otherwise it is the space from cl to the
	 * end plus the space from the start to cf
	 * (end - cl + cf - start - 1  ==  end - start + (cf - cl - 1)).
	 */
}

/*
 * Get one character from circular buffer.
 */
int
getc(cb)
	register struct cirbuf *cb;
{
	register char *nr;
	register int	c;

	nr = cb->c_cf;
	if (nr == cb->c_cl) {
	    CB_CHECK(cb);
	    return (-1);	/* empty */
	}
	c = *(unsigned char *)nr;
	nr++;
	if (nr == cb->c_end)
	    nr = cb->c_start;
	cb->c_cf = nr;

	cb->c_cc--;

	CB_CHECK(cb);

	return (c);
}

/*
 * Get lots of characters.
 * Return number moved.
 */
int
q_to_b(cb, cp, count)
	register struct cirbuf *cb;
	register char	*cp;
	register int	count;
{
	char *		ocp = cp;
	register int	i;

	while (count != 0) {
	    if (cb->c_cl == cb->c_cf)
		break;		/* empty */
	    if (cb->c_cl < cb->c_cf)
		i = cb->c_end - cb->c_cf;
	    else
		i = cb->c_cl - cb->c_cf;
	    if (i > count)
		i = count;
	    bcopy(cb->c_cf, cp, i);
	    cp += i;
	    count -= i;
	    cb->c_cf += i;
	    cb->c_cc -= i;
	    if (cb->c_cf == cb->c_end)
		cb->c_cf = cb->c_start;

	    CB_CHECK(cb);
	}
	CB_CHECK(cb);

	return (cp - ocp);
}

/*
 * Add character array to buffer and return number of characters
 * NOT entered.
 */
int
b_to_q(cp, count, cb)
	register char *	cp;
	int	count;
	register struct cirbuf *cb;
{
	register int	i;
	register char	*lim;

	while (count != 0) {
	    lim = cb->c_cf - 1;
	    if (lim < cb->c_start)
		lim = cb->c_end - 1;

	    if (cb->c_cl == lim)
		break;
	    if (cb->c_cl < lim)
		i = lim - cb->c_cl;
	    else
		i = cb->c_end - cb->c_cl;

	    if (i > count)
		i = count;
	    bcopy(cp, cb->c_cl, i);
	    cp += i;
	    count -= i;
	    cb->c_cc += i;
	    cb->c_cl += i;
	    if (cb->c_cl == cb->c_end)
		cb->c_cl = cb->c_start;

	    CB_CHECK(cb);
	}
	CB_CHECK(cb);
	return (count);
}

/*
 * Return number of contiguous characters up to a character
 * that matches the mask.
 */
int
ndqb(cb, mask)
	register struct cirbuf *cb;
	register int	mask;
{
	register char *cp, *lim;

	if (cb->c_cl < cb->c_cf)
	    lim = cb->c_end;
	else
	    lim = cb->c_cl;
	if (mask == 0)
	    return (lim - cb->c_cf);
	cp = cb->c_cf;
	while (cp < lim) {
	    if (*cp & mask)
		break;
	    cp++;
	}
	return (cp - cb->c_cf);
}

/*
 * Flush characters from circular buffer.
 */
void
ndflush(cb, count)
	register struct cirbuf *cb;
	register int	count;
{
	register int	i;

	while (count != 0) {
	    if (cb->c_cl == cb->c_cf)
		break;		/* empty */
	    if (cb->c_cl < cb->c_cf)
		i = cb->c_end - cb->c_cf;
	    else
		i = cb->c_cl - cb->c_cf;
	    if (i > count)
		i = count;
	    count -= i;
	    cb->c_cf += i;
	    cb->c_cc -= i;
	    if (cb->c_cf == cb->c_end)
		cb->c_cf = cb->c_start;
	    CB_CHECK(cb);
	}

	CB_CHECK(cb);
}

/*
 * Allocate character space for a circular buffer.
 */
void
cb_alloc(cb, buf_size)
	register struct cirbuf *cb;
	int		buf_size;
{
	register char *buf;

	buf = (char *)kalloc(buf_size);

	cb->c_start = buf;
	cb->c_end = buf + buf_size;
	cb->c_cf = buf;
	cb->c_cl = buf;
	cb->c_cc = 0;

	CB_CHECK(cb);
}

/*
 * Free character space for a circular buffer.
 */
void
cb_free(cb)
	register struct cirbuf *cb;
{
	int		size;

	size = cb->c_end - cb->c_start;
	kfree((vm_offset_t)cb->c_start, size);
}

