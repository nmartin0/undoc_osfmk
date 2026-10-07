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
static char rcsid[] = "@(#)$RCSfile: echo.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:27:13 $";
#endif
/*
 * COMPONENT_NAME: (CMDKSH) Korn shell
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 27
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
 * This is the code for the echo and print command
 */

#ifdef KSHELL
#   include	"defs.h"
#endif	/* KSHELL */

#ifdef KJI
#	include <NLchar.h>
#endif

#ifdef  KSH_88D
#ifdef __STDC__
#   define ALERT        '\a'
#else
#   define ALERT        07
#endif /* __STDC__ */
#else
#define ALERT   07
#endif /* ALERT */

/*
 * echo the argument list
 * if raw is non-zero then \ is not a special character.
 * returns 0 for \c otherwise 1.
 */

int echo_list(raw,com)
int raw;
char *com[];
{
#ifndef KJI
	register
#endif
	wchar_t outc;
	register char *cp;
	while(cp= *com++)
	{
#ifdef KJI
		if(!raw) for(; *cp; cp += NLchrlen(cp))
#else
		if(!raw) for(; *cp; cp++)
#endif
		{
#ifdef KJI
			NCdec(cp, &outc);
#else
			outc = *cp;
#endif
			if(outc == '\\')
			{
				switch(*++cp)
				{
					case 'a':
						outc = ALERT;
						break;
					case 'b':
						outc = '\b';
						break;
					case 'c':
						return(0);
					case 'f':
						outc = '\f';
						break;
					case 'n':
						outc = '\n';
						break;
					case 'r':
						outc = '\r';
						break;
					case 'v':
						outc = '\v';
						break;
					case 't':
						outc = '\t';
						break;
					case '\\':
						outc = '\\';
						break;
					case '0':
					{
						register char *cpmax;
						outc = 0;
						cpmax = cp + 4;
						while(++cp<cpmax && *cp>='0' && 
							*cp<='7')
						{
							outc <<= 3;
							outc |= (*cp-'0');
						}
						cp--;
						break;
					}
					default:
					cp--;
				}
			}
			p_char(outc);
		}
#ifdef  KSH_88D
#ifdef POSIX
                else if(raw>1)
                        p_qstr(cp,0);
#endif /* POSIX */
#endif /* KSH_88D */
		else
			p_str(cp,0);
		if(*com)
			p_char(' ');
#ifdef KSHELL
		if(sh.trapnote&SIGSET)
			sh_exit(SIGFAIL);
#endif	/* KSHELL */
	}
	return(1);
}

