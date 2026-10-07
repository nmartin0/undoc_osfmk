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
/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)dbm.h	5.1 (Berkeley) 3/27/86
 */

#ifndef _DBM_H_
#define _DBM_H_

#ifndef NULL
/*
 * this is lunacy, we no longer use it (and never should have
 * unconditionally defined it), but, this whole file is for
 * backwards compatability - someone may rely on this.
 */
#define	NULL	((char *) 0)
#endif

#include <ndbm.h>

#if _NO_PROTO
datum	fetch();
datum	firstkey();
datum	nextkey();
#else
extern datum	fetch(datum);
extern datum	firstkey(void);
extern datum	nextkey(datum);
#endif	/* _NO_PROTO */

#if 0
datum	makdatum();
datum	firsthash();
long	calchash();
long	hashinc();
#endif

#endif /* _DBM_H_ */
