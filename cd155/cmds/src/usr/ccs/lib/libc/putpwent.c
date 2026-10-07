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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.1
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: putpwent.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:33 $";
#endif
/*
 * COMPONENT_NAME: (LIBCADM) Standard C Library System Admin Functions 
 *
 * FUNCTIONS: putpwent 
 *
 * ORIGINS: 3, 27 
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * putpwent.c	1.5  com/lib/c/adm,3.1,8943 9/13/89 10:49:03
 */

/*LINTLIBRARY*/
#include <stdio.h>
#include <pwd.h>

/*                                                                    
 * FUNCTION: Update a user description in the files /etc/passwd
 *		and /etc/security/passwd.
 *
 * PARAMETERS: pw - struct passwd *pw;  the description of the users
 *					password entry.
 *
 * RETURN VALUE DESCRIPTIONS: Upon successful conpletion, PUTPWENT returns
 *				a value of 0. If PUTPWENT fails, a nonzero
 *				value is returned.
 *
 */
/*
 *
 * format a password file entry
 */

int
putpwent(p, f)
register struct passwd *p;
register FILE *f;
{
	(void) fprintf(f, "%s:%s", p->pw_name, p->pw_passwd);


	(void) fprintf(f, ":%u:%u:%s:%s:%s",
		p->pw_uid,
		p->pw_gid,
		p->pw_gecos,
		p->pw_dir,
		p->pw_shell);
	(void) putc('\n', f);
	return(ferror(f));
}
