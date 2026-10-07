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
static char rcsid[] = "@(#)$RCSfile: optget.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:28:34 $";
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
 * G. S. Fowler
 * AT&T Bell Laboratories
 *
 * commmand line option parse assist
 *
 *	-- or ++ terminates option list
 *
 *	return:
 *		0	no more options
 *		'?'	unknown option opt_option
 *		':'	option opt_option requires an argument
 *		'#'	option opt_option requires a numeric argument
 *
 *	conditional compilation:
 *
 *              KSHELL  opt_num and opt_cmd disabled
 */

#ifdef KSHELL
#include "sh_config.h"
#endif

#ifndef KSH_88D
static char     option[3];
#endif

int		opt_index;		/* argv index			*/
int		opt_char;		/* char pos in argv[opt_index]	*/
#ifndef KSHELL
long		opt_num;		/* # numeric argument		*/
#ifdef  KSH_88D
char*           opt_cmd;                /* argv[0]                      */
#endif /* KSH_88D */
#endif
char*		opt_arg;		/* {:,#} string argument	*/
#ifdef  KSH_88D
char            opt_option[3];          /* current flag {-,+} + option  */
#else
char*           opt_option = option;    /* current flag {-,+} + option  */
#endif /* KSH_88D */

extern char*	strchr();

#ifndef KSHELL
extern long	strtol();
#endif

int
optget(argv, opts)
register char**	argv;
char*		opts;
{
	register int	c;
	register char*	s;

	for (;;)
	{
		if (!opt_char)
		{
                        if (!opt_index) {
                                opt_index++;
#ifdef  KSH_88D
#ifndef KSHELL
                                opt_cmd = argv[0];
#endif
#endif /* KSH_88D */
                        }
			if (!(s = argv[opt_index]) || (opt_option[0] = *s++) != '-' && opt_option[0] != '+' || !*s) return(0);
			if (*s++ == opt_option[0] && !*s)
			{
				opt_index++;
				return(0);
			}
			opt_char++;
		}
		if (opt_option[1] = c = argv[opt_index][opt_char++]) break;
		opt_char = 0;
		opt_index++;
	}
	opt_arg = 0;
#ifndef KSHELL
	opt_num = 0;
#endif
	if (c == ':' || c == '#' || c == '?' || !(s = strchr(opts, c))) c = '?';
	else if (*++s == ':' || *s == '#')
	{
		if (argv[opt_index][opt_char]) opt_arg = &argv[opt_index++][opt_char];
		else if (!(opt_arg = argv[++opt_index])) c = ':';
		else opt_index++;
#ifndef KSHELL
		if (*s == '#' && opt_arg)
		{
			char*	e;

			opt_num = strtol(opt_arg, &e, 0);
			if (*e) c = '#';
		}
#endif
		opt_char = 0;
	}
	return(c);
}
