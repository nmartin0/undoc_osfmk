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
#ifndef lint
static char sccsid[] = "@(#)gcd.c	5.3 (Berkeley) 3/2/87";
#endif not lint

#include <mp.h>
gcd(a,b,c) MINT *a,*b,*c;
{	MINT x,y,z,w;
	x.len=y.len=z.len=w.len=0;
	move(a,&x);
	move(b,&y);
	while(y.len!=0)
	{	mdiv(&x,&y,&w,&z);
		move(&y,&x);
		move(&z,&y);
	}
	move(&x,c);
	xfree(&x);
	xfree(&y);
	xfree(&z);
	xfree(&w);
	return;
}
invert(a, b, c) MINT *a, *b, *c;
{	MINT x, y, z, w, Anew, Aold;
	int i = 0;
	x.len = y.len = z.len = w.len = Aold.len = 0;
	Anew.len = 1;
	Anew.val = xalloc(1, "invert");
	*Anew.val = 1;
	move(b, &x);
	move(a, &y);
	while(y.len != 0)
	{	mdiv(&x, &y, &w, &z);
		move(&Anew, &x);
		mult(&w, &Anew, &Anew);
		madd(&Anew, &Aold, &Anew);
		move(&x, &Aold);
		move(&y, &x);
		move(&z, &y);
		i++;
	}
	move(&Aold, c);
	if( (i&01) == 0) msub(b, c, c);
	xfree(&x);
	xfree(&y);
	xfree(&z);
	xfree(&w);
	xfree(&Aold);
	xfree(&Anew);
}
