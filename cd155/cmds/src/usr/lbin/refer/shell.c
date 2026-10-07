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
static char *sccsid = "@(#)shell.c	4.1 (Berkeley) 5/6/83";
#endif
/*
 * SORTS UP.
 * IF THERE ARE NO EXCHANGES (IEX=0) ON A SWEEP
 * THE COMPARISON GAP (IGAP) IS HALVED FOR THE NEXT SWEEP
 */
shell (n, comp, exch)
int (*comp)(), (*exch)();
{
	int igap, iplusg, iex, i, imax;
	igap=n;
	while (igap > 1)
	{
		igap /= 2;
		imax = n-igap;
		do
		    {
			iex=0;
			for(i=0; i<imax; i++)
			{
				iplusg = i + igap;
				if ((*comp) (i, iplusg) ) continue;
				(*exch) (i, iplusg);
				iex=1;
			}
		} 
		while (iex>0);
	}
}
