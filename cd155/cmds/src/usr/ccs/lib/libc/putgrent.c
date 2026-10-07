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
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: putgrent.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:31 $";
#endif
/*
 * COMPONENT_NAME: (LIBCADM) Standard C Library System Admin Functions 
 *
 * FUNCTIONS: putrent 
 *
 * ORIGINS: 27 
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * putgrent.c	1.4  com/lib/c/adm,3.1,8943 9/13/89 10:43:56
 */

#include <stdio.h>
#include <grp.h>

/*                                                                    
 * FUNCTION: Update a group description in the files /etc/group
 *		and /etc/security/group.
 *
 * RETURN VALUE DESCRIPTIONS: Upon successful conpletion, PUTGRENT returns
 *				a value of 0. If PUTGRENT fails a nonzero
 *				value is returned.
 *
 */
/*
 * format a group file entry
 */

int
putgrent(g, f)
register struct group *g;
register FILE *f;
{
	register char	**q;
	char		buf[BUFSIZ+1];
	/*
	 * AIX Version 2.2.1 security enhancement 
	 */
	/* build one comma separated string from pointer array */
	if (g == NULL)
	{
#ifdef DEBUGX
		fprintf(stderr, "putgrent called with null group ptr\n");
#endif
		return (-1);
	}
	for (buf[0] = NULL, q = g->gr_mem; *q;)
	{
		strcat (buf, *q);
		if (*(++q) != NULL)
			strcat (buf, ",");
	}
	(void) fprintf(f, "%s:%s:%d:%s",
			  g->gr_name, 	/* group name */
			  g->gr_passwd,	/* password default value */
			  g->gr_gid,  	/* group id */
			  buf);		/* list of comma separated grp names */

	(void) putc('\n', f);
	return(ferror(f));
	/* 
	 * TCSEC Division C Class C2 
	 */
}
