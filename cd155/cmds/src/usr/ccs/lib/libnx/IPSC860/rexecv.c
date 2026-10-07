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
 * Copyright (c) 1991, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: rexecv.c,v $
 * Revision 3.3  1994/11/19  02:33:28  mtm
 * Copyright additions/changes
 *
 * Revision 3.2  1993/07/13  15:20:17  shala
 * New version from LOCUS.
 *
 * Revision 1.3  1993/05/06  19:30:42  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  23:02:54  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:13:21  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:04:11  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:58:29  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 3.1  1992/04/14  15:00:00  klh
 * Fix RCS Log.
 *
 * $EndLog$
 */
extern char **environ;

int rexecv(
	char	*path,
	char	*argv[],
	int	node)
{
	return( rexecve(path, argv, environ, node));
}
