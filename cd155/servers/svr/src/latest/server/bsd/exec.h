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
 * @OSF_COPYRIGHT@
 */
/* 
 * HISTORY
 * $Log: exec.h,v $
 * Revision 1.4  1994/11/18  20:26:42  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  17:46:29  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:45:09  cfj
 * Adding new code from vendor
 * Revision 1.2  1992/11/30  22:15:02  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:05:24  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:05:49  cfj
 * Bump major revision number.
 *
 * Revision 2.1  1991/10/14  11:38:53  sjs
 * Created.
 *
 * Revision 3.0  91/10/04  16:34:40  sp
 * New file to share exec structures between kern_exec.c and ldr_exec.c
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986, 1989 Regents of the University of California.
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
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef	_BSD_EXEC_H_
#define	_BSD_EXEC_H_

/*
 * exec system call, with and without environments.
 */
struct execa {
	char	*fname;
	char	**argp;
	char	**envp;
};

struct execr {				/* exec return arguments */
	char		*cfname;	/* shell file name */
	char		*cfarg;		/* shell args */
	int		*entry;		/* pointer to pc entry points */
	unsigned int	*entry_count;	/* number of entries */
};

#endif	/* _BSD_EXEC_H_ */
