/*
 *
 * $Copyright
 * Copyright 1992, 1993, 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 *
 */ 
#include <stdio.h>

/*
 *  Functions return values without worrying about possible allignment problems.
 *  Useful when accessing 486 or 680[2-4]0 data with 88k.
 */

unsigned long
cs_swap_4(l)
	register unsigned char *l;
{
	register unsigned long r;

	r = (unsigned long) l[3];
	r |= l[2] << 8;
	r |= l[1] << 16;
	r |= l[0] << 24;
	return(r);
}

unsigned short
cs_swap_2(s)
	register unsigned char *s;
{
	register unsigned short r;

	r = (unsigned short) s[1];
	r |= s[0] << 8;
	return(r);
}

swap_bytes(x)
	unsigned short x;
{
	return(((x<<8)&0xff00)|((x>>8)&0x00ff));
}
