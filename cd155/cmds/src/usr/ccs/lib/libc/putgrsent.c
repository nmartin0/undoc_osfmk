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
static char rcsid[] = "@(#)$RCSfile: putgrsent.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:31 $";
#endif
/*
 * COMPONENT_NAME: (LIBCADM) Standard C Library System Admin Functions 
 *
 * FUNCTIONS: putgrsent 
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
 * putgrsent.c	1.3  com/lib/c/adm,3.1,8943 9/13/89 10:44:20
 */

#include <stdio.h>
#include <grp.h>

/*                                                                    
 * FUNCTION: format a security/group file entry in stanza format
 *
 * RETURN VALUE DESCRIPTIONS:
 *	     - 0 on success
 *	     - nonzero on failure
 */

int
putgrsent(g, fp)
register struct group  *g;
FILE		       *fp;
{

	(void) fprintf (fp,"%s:\n", g->gr_name);
	if (*(g->gr_passwd) != NULL)
		(void) fprintf (fp,"\tpassword = %s\n", g->gr_passwd);

	/* end the stanza with a blank line */
	(void) putc('\n', fp);

	return (ferror (fp));
}
