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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Id: autoinit.c,v 1.4 1994/11/19 02:10:31 mtm Exp $
 *
 * HISTORY
 * $Log: autoinit.c,v $
 * Revision 1.4  1994/11/19  02:10:31  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1992/06/10  16:42:00  stans
 * try again, stub out nxlib routines.
 *
 * Revision 1.2  92/06/09  11:47:12  stans
 * temp fix until Nandine fixes crt0.c correctly!
 * 
 *
 */

/*
 * This is dummy routine which will be replaced with the one
 * in libnx when linking for parallel application.
 *
 */

int
autoinit( argc, argv, envp ) 
int	*argc;
char	*argv[];
char	*envp[];
{
	return;
}
