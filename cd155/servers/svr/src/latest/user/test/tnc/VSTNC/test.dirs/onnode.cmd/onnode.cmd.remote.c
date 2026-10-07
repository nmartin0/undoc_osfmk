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
 *		Copyright (c) Locus Computing, 1991-92
 * 		This is UNPUBLISHED source code that is
 * 		the property of Locus Computing, containing
 *		proprietary secrets of LCC.  Any disclosure
 *		is strictly prohibited.  Locus makes no warantee,
 *		explicit or implicit, on the functionality of this code.
 * $Log: onnode.cmd.remote.c,v $
 * Revision 1.4  1994/11/18  21:07:40  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/02/04  01:46:23  slk
 *  Reviewer: Brent Olsen
 *  Risk: Low
 *  Benefit or PTS #: 7176	and update VSTNC.
 *  Testing: Built, Ran VSTNC
 *  Module(s):
 *
 * Revision 3.0  92/07/22  16:52:11  jpaul
 * Initial Checkin
 * 
 */
#include <stdio.h>

main(	int	argc,
	char	*argv[])
{
	int	i;

	printf("stdout: my node is %d\n", node_self());
	printf("stdout: argc is %d\n", argc);
	for( i=0; i<argc; ++i )
		{ printf("stdout: arg %d is %s\n", i, argv[i]); }

	fprintf(stderr, "stderr: my node is %d\n", node_self());
	fprintf(stderr, "stderr: argc is %d\n", argc);
	for( i=0; i<argc; ++i )
		{ fprintf(stderr, "stderr: arg %d is %s\n", i, argv[i]); }
			fflush(stderr);
	exit(0);
}
