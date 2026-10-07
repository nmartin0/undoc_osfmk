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
static char rcsid[] = "@(#)$RCSfile: sid_ba.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:38:24 $";
#endif
/*
 * COMPONENT_NAME: CMDSCCS      Source Code Control System (sccs)
 *
 * FUNCTIONS: sid_ba
 *
 * ORIGINS: 3, 10, 27
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
 */
/*   sid_ba.c 1.4 com/cmd/sccs/lib/comobj,3.1,9013 9/15/89 14:16:01"; */


# include	"defines.h"

char *
sid_ba(sp,p)
register struct sid *sp;
register char *p;
{
	sprintf(p,"%u.%u",sp->s_rel,sp->s_lev);
	while (*p++)
		;
	--p;
	if (sp->s_br) {
		sprintf(p,".%u.%u",sp->s_br,sp->s_seq);
		while (*p++)
			;
		--p;
	}
	return(p);
}
