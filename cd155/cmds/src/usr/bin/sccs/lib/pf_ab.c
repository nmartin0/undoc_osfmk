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
static char rcsid[] = "@(#)$RCSfile: pf_ab.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:38:21 $";
#endif
/*
 * COMPONENT_NAME: CMDSCCS      Source Code Control System (sccs)
 *
 * FUNCTIONS: pf_ab
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
/*   pf_ab.c 1.5 com/cmd/sccs/lib/comobj,3.1,9013 9/15/89 14:13:20";  */


# include	"defines.h"

pf_ab(s,pp,all)
char *s;
register struct pfile *pp;
int all;
{
	register char *p;
	register int i;
	extern char *Datep;
	char *xp, *sid_ab(), *fmalloc(), *strncpy();
	char stmp[MAXLINE];

	xp = p = stmp;
	copy(s,p);
	for (; *p; p++)
		if (*p == '\n') {
			*p = 0;
			break;
		}
	p = xp;
	p = sid_ab(p,&pp->pf_gsid);
	++p;
	p = sid_ab(p,&pp->pf_nsid);
	++p;
	i = strcspn(p, " ");
	pp->pf_user[0] = 0;
	if (((unsigned)i) < LOGSIZE) {
		strncpy(pp->pf_user,p,i);
		pp->pf_user[i] = 0;
	}
	else
		fatal("bad p-file format (co17)");
	p = p + i + 1;
	date_ab(p,&pp->pf_date);
	p = Datep;
	pp->pf_ilist = 0;
	pp->pf_elist = 0;
#ifdef CASSI
	pp->pf_cmrlist = 0;
#endif
	if (!all || !*p)
		return;
	p += 2;
	xp = fmalloc(size(p));
	copy(p,xp);
	p = xp;
	if (*p == 'i') {
		pp->pf_ilist = ++p;
		for (; *p; p++)
			if (*p == ' ') {
				*p++ = 0;
				p++;
				break;
			}
	}
	if (*p == 'x')
		{
		pp->pf_elist = ++p;
		for(;*p;p++)
			if(*p = ' ')
			{
				*p++ = 0;
				p++;
				break;
			}
	}
#ifdef CASSI
	if(*p == 'z')
		{
		pp->pf_cmrlist = ++p;
		}
#endif
}
