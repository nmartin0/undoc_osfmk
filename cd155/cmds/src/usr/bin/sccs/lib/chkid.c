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
static char rcsid[] = "@(#)$RCSfile: chkid.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:38:04 $";
#endif
/*
 * COMPONENT_NAME: CMDSCCS      Source Code Control System (sccs)
 *
 * FUNCTIONS: chkid
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
/*   chkid.c 1.4 com/cmd/sccs/lib/comobj,3.1,9013 9/15/89 13:48:32"; */


# include	"ctype.h"
# include	"defines.h"

char *strchr();

chkid(line,idstr)

char *line;
char *idstr;

{
	register char *lp;
	register char *p;
	extern int Did_id;

	if (!Did_id && any('%',line))
		if (!idstr || *idstr == 0)
			for(lp=line; *lp != 0; lp++) {
				if(lp[0] == '%' && lp[1] != 0 && lp[2] == '%')
					if (isupper(lp[1]))
						switch (lp[1]) {
						case 'J':
							break;
						case 'K':
							break;
						case 'N':
							break;
						case 'O':
							break;
						case 'V':
							break;
						case 'X':
							break;
						default:
							return(Did_id++);
						}
			}
		else
			{
			 p=idstr;
			 lp=line;
			 while(lp=strchr(lp,*p))
				if(!(strncmp(lp,p,strlen(p))))
					return(Did_id++);
				else
					++lp;
			}

	return(Did_id);
}
