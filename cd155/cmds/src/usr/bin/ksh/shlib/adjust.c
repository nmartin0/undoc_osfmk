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
static char rcsid[] = "@(#)$RCSfile: adjust.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:28:15 $";
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
 *   ADJUST.C
 *
 *   Programmer:  D. A. Lambeth
 *
 *        Owner:  D. A. Lambeth
 *
 *         Date:  April 17, 1980
 *
 *
 *
 *   CHATTRIB (NODE, NEWATTS)
 *
 *        Give NODE the attribute(s) NEWATTS, and change its
 *        value to conform to the new attributes.
 *
 *
 *
 *   See Also:  TYPESET(I)
 */


#include	<locale.h>
#include	<stdlib.h>
#include	"sh_config.h"
#include	"defs.h"
#include	"builtins.h"

#ifdef MSG
#include "ksh_msg.h" 
nl_catd catd;
#define MSGSTR(n,s) NLcatgets(catd,MS_KSH,n,s) 
#else
#define MSGSTR(n,s) s
#endif

#ifdef apollo
    extern void	ev_$delete_var();
    extern void	ev_$set_var();
#endif /* apollo */


/*
 *   NAM_NEWYTPE (NODE, NEWATTS)
 *
 *        struct namnod *NODE;
 *
 *        int NEWATTS;
 *
 *	  int size;
 *
 *   Give NODE the attributes NEWATTS, and change its current
 *   value to conform to NEWATTS.  The SIZE of left and right
 *   justified fields may be given.
 */
void	env_check_nls_and_locale ( struct namnod *, int );

void nam_newtype (struct namnod *node, unsigned int newatts, int size)
{
	register char *sp;
	register char *cp = NULL;
	register struct namnod *np = node;
	register unsigned int n;
	struct Namaray *ap = 0;
	int oldsize,oldatts,savedot;

#ifdef NAME_SCOPE
	if(np->value.namflg&N_CWRITE)
		np = nam_copy(np,1);
#endif /* NAME_SCOPE */
	/* handle attributes that do not change data separately */
	n = np->value.namflg;
#ifdef apollo
	if(((n^newatts)&N_EXPORT))
	/* record changes to the environment */
	{
		short namlen = strlen(np->namid);
		if(n&N_EXPORT)
			ev_$delete_var(np->namid,&namlen);
		else
		{
			char *vp = nam_strval(np);
			short vallen = strlen(vp);
			ev_$set_var(np->namid,&namlen,vp,&vallen);
		}
	}
#endif /* apollo */
	if((size==0||(n&N_INTGER)) && ((n^newatts)&~NO_CHANGE)==0)
	{
		if(size)
			np->value.namsz = size;
		np->value.namflg = newatts;
		if ( nam_istype ( np, N_EXPORT ))
			env_check_nls_and_locale ( np , 0);
		return;
	}
	/* for an array, change all the elements */
	if(nam_istype(np,N_ARRAY))
	{
		ap = array_ptr(np);
		savedot = ap->cur[0];
		ap->cur[0] = 0;
		oldsize = np->value.namsz;
		oldatts = np->value.namflg;
	}
again:
	if (sp = nam_strval (np))
 	{
		cp = malloc ((size_t)((n=strlen (sp)) + 1));
		strcpy (cp, sp);
		nam_free(np);
		if(size==0 && (newatts&(N_LJUST|N_RJUST|N_ZFILL)))
			size = n;
	}
	np->value.namsz = size;
	np->value.namflg &= ~(N_LJUST|N_RJUST|N_ZFILL|N_LTOU|N_UTOL|N_HOST|N_EXPORT|N_RDONLY);
	np->value.namflg |= newatts;
	if (cp != NULL)
	{
		nam_fputval (np, cp);
		free((void *)cp);
	}
	if(ap)
	{
		while((n= ++ap->cur[0]) < ap->maxi)
		{
			if(ap->val[n])
			{
				np->value.namsz = oldsize;
				np->value.namflg = oldatts;
				goto again;
			}
		}
		ap->cur[0] = savedot;
	}
	return;
}


static int call_putenv (struct namnod *n)
{
	char	*env, *oldenv;
	int	env_off;

	oldenv = getenv ( n->namid );
	if ( oldenv && n->value.namval.cp && !strcmp ( oldenv, n->value.namval.cp ))
		return (TRUE);
	env_off = (int) strlen (n->namid);
	env = malloc (env_off + (int) strlen(n->value.namval.cp) + 2);
	strcpy (env, n->namid);
	env[env_off] = '=';
	env[env_off+1] = (char) 0;
	if ( n->value.namval.cp )
		strcat (env, n->value.namval.cp);
	putenv (env);
	if (oldenv)
		free (oldenv);
	return (FALSE);
}


void env_check_nls_and_locale ( struct namnod *n , int init_flag )
{

#if	(defined(NLS) || defined(KJI))
	if ( n==NLSPATHNOD || n==LANGNOD) {
				/*	if already set then return	*/
		if ( call_putenv (n) || init_flag )
			return;
		catclose (catd);
                open_msg_catalog ();
		setlocale ( LC_ALL, "" );
		return;
	}
#endif /* NLS || KJI */

#ifdef _locale_
	if(n==LCTYPENOD || n==LCOLLTNOD || n==LCMONTNOD ||
		n==LCNUMNOD || n==LCTIMENOD || n==LCMESSNOD)
	{
		int	category;

				/*	if already set then return	*/
		if ( call_putenv (n) || init_flag )
			return;

		if ( n == LCTYPENOD )
			category = LC_CTYPE;
		else if ( n == LCOLLTNOD )
			category = LC_COLLATE;
		else if ( n == LCMONTNOD )
			category = LC_MONETARY;
		else if ( n == LCTIMENOD )
			category = LC_TIME;
		else if ( n == LCMESSNOD )
			category = LC_MESSAGES;
		else if ( n == LCNUMNOD )
			category = LC_NUMERIC;
		else
			return;

			/*	only valid setlocale entries get here	*/
		setlocale ( category, n->value.namval.cp );
	}
#endif /* _locale_ */

	return;
}
