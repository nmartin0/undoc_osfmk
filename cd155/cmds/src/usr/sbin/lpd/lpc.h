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
/* @(#)$RCSfile: lpc.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:08:21 $ */
/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	lpc.h	5.3 (Berkeley) 6/30/88
 * 	lpc.h	4.1 15:58:57 7/19/90 SecureWare 
 */

/*
 * Line printer control program.
 */
struct	cmd {
	char	*c_name;		/* command name */
	int	c_help;			/* help message number, see lpc.msg */
	int	(*c_handler)();		/* routine to do the work */
#if SEC_BASE
	char	*c_priv;		/* authorization to check */
	int	c_hasauth;		/* result of auth check */
#else
	int	c_priv;			/* privileged command */
#endif
};
