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
static char rcsid[] = "@(#)$RCSfile: cannon.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:28:20 $";
#endif
/*
 * COMPONENT_NAME: (CMDKSH) Korn shell
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 26, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * Copyright 1976, Bell Telephone Laboratories, Inc.
 */

/*

 *      Copyright (c) 1984, 1985, 1986, 1987, 
 *                  1988, 1989   AT&T
 *      All Rights Reserved

 *      THIS IS UNPUBLISHED PROPRIETARY SOURCE 
 *      CODE OF AT&T.
 *      The copyright notice above does not 
 *      evidence any actual or intended
 *      publication of such source code.

 */
/*
 *  pathcanon - Generate canonical pathname from given pathname.
 *  This routine works with both relative and absolute paths.
 *  Relative paths can contain any number of leading ../ .
 *  The length of the canonical path is returned, zero for ..
 *  The operator ... is also expanded by this routine when LIB_3D is defined
 *  In this case length of final path may be larger than orignal length
 *
 *   David Korn
 *   AT&T Bell Laboratories
 *   Room 3C-526B
 *   Murray Hill, N. J. 07974
 *   Tel. x7975
 *
 */

#include	"sh_config.h"

char	*pathcanon(path)
char *path;
{
	register char *dp=path;
	register int c = '/';
	register char *sp;
	register char *begin=dp;
#ifdef LIB_3D
        extern char *pathnext();
#endif /* LIB_3D */
#ifdef PDU
	/* Take out special case for /../ as */
	/* Portable Distributed Unix allows it */
	if ((*dp == '/') && (*++dp == '.') &&
	    (*++dp == '.') && (*++dp == '/') &&
	    (*++dp != 0))
		begin = dp = path + 3;
	else
		dp = path;
#endif /* PDU */

	if(*dp != '/')
		dp--;
	sp = dp;
	while(1)
	{
		sp++;
		if(c=='/')
		{
			if(*sp == '/')
				/* eliminate redundant / */
				continue;
			else if(*sp == '.')
			{
				c = *++sp;
				if(c == '/')
					continue;
				if(c==0)
					break;
				if(c== '.')
				{
					if((c= *++sp) && c!='/')
					{
#ifdef LIB_3D
						if(c=='.')
						{
							char *savedp;
							int savec;
							if((c= *++sp) && c!='/')
								goto dotdotdot;
							/* handle ... */
							savec = *dp;
							*dp = 0;
							savedp = dp;
#ifdef  KSH_88D
                                                        dp = pathnext(path,sp);
#else
							dp = next_vpath(path,sp);
#endif /* KSH_88D */
							if(dp)
							{
								*dp = savec;
								sp = dp;
								if(c==0)
									break;
								continue;
							}
							dp = savedp;
							*dp = savec;
						dotdotdot:
							*++dp = '.';
						}
#endif /* LIB_3D */
					dotdot:
						*++dp = '.';
					}
					else /* .. */
					{
						if(dp>begin)
						{
							while(*--dp!='/')
								if(dp<begin)
									break;
						}
						else if(dp < begin)
						{
							begin += 3;
							goto dotdot;
						}
						if(c==0)
							break;
						continue;
					}
				}
				*++dp = '.';
			}
		}
		if((c= *sp)==0)
			break;
		*++dp = c;
	}
#ifdef LIB_3D
	*++dp= 0;
#else
	/* remove all trailing '/' */
	if(*dp!='/' || dp<=path)
		dp++;
	*dp= 0;
#endif /* LIB_3D */
	return(dp);
}

