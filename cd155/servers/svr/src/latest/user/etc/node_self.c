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
 *		Copyright (c) Locus Computing, 1991-93
 * 		This is UNPUBLISHED source code that is
 * 		the property of Locus Computing, containing
 *		proprietary secrets of LCC.  Any disclosure
 *		is strictly prohibited.  Locus makes no warantee,
 *		explicit or implicit, on the functionality of this code.
 */
#include <stdio.h>

int node_self();	/* from the TNC library libtnc.a */
/*	Print out which node the process is currently running on.  
 *	useful for debugging and testing.		
 */
main()
{
	register int thisnode = node_self();
	if (thisnode >= 0)  {
		printf("%d\n", thisnode);
		exit(0);
	}
	else 
		exit (-1);

}
