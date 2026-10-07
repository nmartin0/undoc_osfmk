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
#if !defined( lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#) $RCSfile: hashuser.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:03:46 $";
#endif
/*
 * IBM CONFIDENTIAL
 * Copyright International Business Machines Corp. 1989
 * Unpublished Work
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
#if !lint
static char sccsid[] = "hashuser.c	1.1  com/cmd/acct/lib,3.1,8935 5/15/89 17:17:49";
#endif

#include "acctdef.h"

unsigned
hashuser(uid, name)
unsigned short uid;
register char *name;
{       register unsigned uh;
	register int i = NSZ;

	for (uh = *name++; --i > 0; ) {
		uh *= 7;
		if (*name == 0) break;
		uh += *name++;
	}
	return((uh+uid) % UHASH);
}

