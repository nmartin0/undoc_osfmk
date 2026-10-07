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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: rexecv.c,v $
 * Revision 1.6  1995/02/02  00:15:28  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.5  1994/11/18  20:56:16  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:52:04  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.3  1993/05/06  19:30:42  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.3  1993/07/01  21:19:03  cfj
 * Adding new code from vendor
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
