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
static char rcsid[] = "@(#)$RCSfile: assign.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:28:19 $";
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
 *   ASSIGN.C
 *
 *   Programmer:  D. G. Korn
 *
 *        Owner:  D. A. Lambeth
 *
 *         Date:  April 17, 1980
 *
 *
 *   NAM_PUTVAL (NP, STRING)
 *
 *        Assign STRING to NP.
 *
 *   NAM_FPUTVAL (NP, STRING)
 *
 *        Assign STRING to NP even if readonly.
 *
 *
 *   See Also:  nam_longput(III), nam_free(III), nam_strval(III)
 */

#include	<string.h>
#include	"sh_config.h"
#include	"name.h"
#ifdef MULTIBYTE
#   include	"national.h"
#endif /* MULTIBYTE */

#if (defined(NLS) || defined(KJI))
#	include	<NLchar.h>
#	include	<NLctype.h>
#endif (KJI | NLS)

extern void	utol(),ltou();
extern void	nam_rjust();

#if (defined(MULTIBYTE) || defined(KJI) || defined(NLS))
static unsigned char *savep;
#	ifdef MULTIBYTE
		static unsigned char savechars[ESS_MAXCHAR+1];
#	else
		static unsigned char savechars[3];
#	endif /* MULTIBYTE */
static int ja_size();
#else
#define size	 np->value.namsz
#endif /* INTL */

#ifdef MSG
#include "ksh_msg.h" 
extern nl_catd catd;
#define MSGSTR(n,s) NLcatgets(catd,MS_KSH,n,s) 
#else
#define MSGSTR(n,s) s
#endif

/*
 *   NAM_PUTVAL (NP, STRING)
 *
 *        struct namnod *NP;
 *     
 *        char *STRING;
 *
 *   Assign the string given by STRING to the namnod given by
 *   NP.  STRING is converted according to the namflg field
 *   of NP before assignment.  
 *
 *   If NP is an array, then the element given by the
 *   current index is assigned to.
 *   
 *   Any freeable space associated with the old value of NP
 *   is released.
 *
 *   If the copy on write,N_CWRITE flag is set then the assignment
 *   is made on a copy of the np created on the last shell tree.
 * 
 */

static char forced = 0;

void nam_fputval(struct namnod *np, char *string)
{
	forced++;
	nam_putval(np,string);
	forced = 0;
#ifdef apollo
	if(nam_istype(np,N_EXPORT))
	{
		short namlen, vallen;
		char *vp = nam_strval(np);
		namlen =strlen(np->namid);
		vallen = strlen(vp);
		ev_$set_var(np->namid,&namlen,vp,&vallen);
	}
#endif /* apollo */
}

void nam_putval(struct namnod *np, char *string)
{
	register char *sp=string;
	register union Namval *up;
	register char *cp;
#if (defined(MULTIBYTE) || defined(KJI) || defined(NLS))
	register int size = 0;
#endif /* INTL */
	register int dot;
#ifdef apollo
	/* reserve space for UNIX to host file name translation */
	char pathname[256];
	short pathlen;
#endif	/* apollo */
#ifdef NAME_SCOPE
	if (nam_istype (np,N_CWRITE))
		np = nam_copy(np,1);
#endif	/* NAME_SCOPE */
#ifdef  KSH_88D
        if (forced==0 && nam_istype (np, N_RDONLY|N_RESTRICT))
        {
                if(nam_istype (np, N_RDONLY))
                        sh_fail(np->namid,MSGSTR(E_READONLY, (char *)e_readonly)); /*MSG*/
                else
                        sh_fail(np->namid,MSGSTR(E_RESTRICTED, (char *)e_restricted)); /*MSG*/
                /* NOTREACHED */
        }
#else
	if (forced==0 && nam_istype (np, N_RDONLY))
		sh_fail(np->namid,MSGSTR(E_READONLY, (char *)e_readonly)); /*MSG*/
	else if(forced==0 && nam_istype (np, N_RESTRICT))
		sh_fail(np->namid,MSGSTR(E_RESTRICTED, (char *)e_restricted)); /*MSG*/
#endif /* KSH_88D */
	if(nam_istype (np, N_ARRAY))
		up = &(array_find(np,A_ASSIGN)->namval);
	else
		up= &np->value.namval;
	if (nam_istype (np, N_INDIRECT))
		up = up->up;
#ifdef  KSH_88D
        nam_offtype(np,~N_IMPORT);
#endif /* KSH_88D */
	if (nam_istype (np, N_INTGER))
	{
		long l;
#ifdef FLOAT
		extern double sh_arith();
		if (nam_istype(np, N_DOUBLE))
		{
			if(up->dp==0)
				up->dp = new_of(double,0);
			*(up->dp) = sh_arith(sp);
			return;
		}
#else
		extern long sh_arith();
#endif /* FLOAT */
		if (nam_istype (np, N_CPOINTER))
		{
			up->cp = sp;
			return;
		}
		l = (sp? (long)sh_arith(sp) : (sh_lastbase=10,0));
		if(np->value.namsz == 0)
			np->value.namsz = sh_lastbase;
		if (nam_istype (np, N_BLTNOD))
		{
			(*up->fp->f_ap)(l);
			return;
		}
		if(up->lp==0)
			up->lp = new_of(long,0);
		*(up->lp) = l;
		if(l && *sp++ == '0')
			np->value.namflg |= N_UNSIGN;
		return;
	}
#ifndef KSH_88D
        nam_offtype(np,~N_IMPORT);
#endif /* KSH_88D */
	if ((nam_istype (np, N_RJUST|N_ZFILL|N_LJUST)) && sp)
	{
#if (defined(KJI) || defined(NLS))
		register int len;
		register char *p;
#endif
#ifdef KJI
		wchar_t c;
		for (;len = NCdec(sp,&c), NCisspace((int)c); sp += len);
#else
		for(;*sp == ' '|| *sp=='\t';sp++);
#endif	/* KJI */
        	if ((nam_istype (np, N_ZFILL)) && (nam_istype (np, N_LJUST)))
			for(;*sp=='0';sp++);
#if (defined(KJI) || defined(NLS))
		/* strip trailing spaces first since ja_size() may
		 * replace a trailing 2-byte character with spaces */
		size = strlen(sp);
		if (size > 0) {
#	ifdef NLS
			for (p = sp+size;size && *--p == ' ';size--);
			*(++p) = '\0';
#	else /* KJI */
			/* clipping trailing spaces requires multiple searchs 
			 * from the beginning of the string
			 */
			do {
				register char *q;
	
				p = q = sp;
				while ((p += NLchrlen(p)) < (sp+size)) 
					q = p;
				len = NCdec(q,&c);
				if (c != ' ' && !isjspace(c))
					break;
				size -= len;
			} while (size > 0);
			*(sp+size) = '\0';
#	endif /* KJI */
		}
#endif /* NLS | KJI */
#if (defined(MULTIBYTE) || defined(NLS) || defined(KJI))
		if(size = np->value.namsz)
			size = ja_size((unsigned char*)sp,size,nam_istype(np,N_RJUST|N_ZFILL));
#endif /* INTL */
	}
	if ((!nam_istype (np, N_FREE|N_ALLOC)) && (up->cp != NULL))
		free((void *)up->cp);
	if (nam_istype (np, N_ALLOC))
		cp = up->cp;
	else
	{
        	np->value.namflg &= ~N_FREE;
        	if (sp)
		{
			dot = strlen(sp);
			if(size==0 && nam_istype(np,N_LJUST|N_RJUST|N_ZFILL))
				np->value.namsz = size = dot;
			else if(size > dot)
				dot = size;
			cp = malloc(((size_t)dot+1));
		}
		else
			cp = NULL;
		up->cp = cp;
	}
	if (!sp)
		return;
	if (nam_istype (np, N_LTOU))
		ltou(sp,cp);
	else if (nam_istype (np, N_UTOL))
		utol(sp,cp);
	else
        	strcpy (cp, sp);
	if (nam_istype (np, N_RJUST) && nam_istype (np, N_ZFILL))
		nam_rjust(cp,size,'0');
	else if (nam_istype (np, N_RJUST))
		nam_rjust(cp,size,' ');
	else if (nam_istype (np, N_LJUST))
        {
#if (defined(NLS) || defined(KJI))
		/* this will never split a 2-byte character since 2-byte
		 * characters straddling a boundary have already been
		 * replaced with spaces in ja_size()
		 */
#endif
         	sp = strlen (cp) + cp;
		*(cp = (cp + size)) = 0;
		for (; sp < cp; *sp++ = ' ');
         }
#if (defined(MULTIBYTE) || defined(NLS) || defined(KJI))
	/* restore original string */
	if(savep)
		ja_restore();
#endif /* INTL */
	return;
}

#if     (defined(NLS) || defined(KJI) || defined(MULTIBYTE))
/*
 * handle left and right justified fields for multi-byte chars
 * given physical size, return a logical size which reflects the
 * screen width of multi-byte characters
 * Multi-width characters replaced by spaces if they cross the boundary
 * <type> is non-zero for right justified  fields
 */

static int ja_size(str,size,type)
unsigned char *str;
int size;
{
	register unsigned char *cp = str;
	register int c;
	register int n = size;
	int oldn;
	while(c = *cp++)
	{
		oldn = n;
#ifdef  MULTIBYTE
		/* find character set number */
		c = echarset(c);
		/* allow room for excess input bytes */
		if(c)
		{
			n += (in_csize(c)-out_csize(c)+(c>=2));
			cp += (in_csize(c)-(c==1));
		}
		size -= out_csize(c);
#else /* NLS || KJI */
                if (NCisshift(c)) {
#ifdef NLS
                        /* 2-byte characters in NLS display in 1 column */
                        n++;
#else /* KJI */
                        size--;
#endif
                        cp++;
                }
                size--;
#endif /* KSH_88D */
		if(size<=0 && type==0)
			break;
	}
	/* check for right justified fields that need truncating */
	if(size <0)
	{
		if(type==0)
		{
			/* left justified and character crosses field boundary */
			n = oldn;
			/* save boundary char and replace with spaces */
#ifdef  MULTIBYTE
                        size = in_csize(c)+(c>2);
#else /* NLS || KJI */
                        size = 2;
#endif /* MULTIBYTE */
			savechars[size] = 0;
			while(size--)
			{
				savechars[size] = *--cp;
				*cp = ' ';
			}
			savep = cp;
		}
		size = -size;
		if(type)
			n -= (ja_size(str,size,0)-size);
	}
	return(n);
}
#endif /* INTL */

#if 0
#if (defined(NLS) || defined(KJI))
/*
 * handle left and right justified fields for multi-byte chars
 * given physical size, return a logical size which reflects the
 * screen width of multi-byte characters
 * Multi-width characters replaced by spaces if they cross the boundary
 * <type> is non-zero for right justified  fields
 */

static int ja_size(str,size,type)
unsigned char *str;
int size;
{
	register unsigned char *cp = str;
	register int c;
	register int n = size;
	int oldn;
	while(c = *cp++)
	{
		oldn = n;
		if (NCisshift(c)) {
#ifdef NLS
			/* 2-byte characters in NLS display in 1 column */
			n++;
#else /* KJI */
			size--;
#endif
			cp++;
		}
		size--;
		if(size<=0 && type==0)
			break;
	}
	/* check for right justified fields that need truncating */
	if(size <0)
	{
		if(type==0)
		{
			/* left justified and char crosses field boundary */
			n = oldn;
			/* save boundary char and replace with spaces */
			size = 2;
			savechars[size] = 0;
			while(size--)
			{
				savechars[size] = *--cp;
				*cp = ' ';
			}
			savep = cp;
		}
		size = -size;
		if(type)
			n -= (ja_size(str,size,0)-size);
	}
	return(n);
}
#endif /* NLS | KJI */
#endif /* 0 */

#if (defined(MULTIBYTE) || defined(NLS) || defined(KJI))
int ja_restore()
{
	register unsigned char *cp = savechars;
	while(*cp)
		*savep++ = *cp++;
	savep = 0;
}
#endif /* MULTIBYTE | NLS | KJI */
