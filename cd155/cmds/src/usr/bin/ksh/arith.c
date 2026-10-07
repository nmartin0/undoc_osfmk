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
static char rcsid[] = "@(#)$RCSfile: arith.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:27:04 $";
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
 * shell arithmetic - uses streval library
 */

#include	"defs.h"
#include	"streval.h"

#ifdef MSG
#include "ksh_msg.h" 
extern nl_catd catd;
#define MSGSTR(n,s) NLcatgets(catd,MS_KSH,n,s) 
#else
#define MSGSTR(n,s) s
#endif

extern int	sh_lastbase;

#ifdef FLOAT
    extern double atof();
#endif /* FLOAT */

static number arith(ptr, lvalue, type, n)
char **ptr;
struct lval *lvalue;
number n;
{
	register number r= 0;
	char *str = *ptr;
	switch(type)
	{
	case ASSIGN:
	{
		register struct namnod *np = (struct namnod*)(lvalue->value);
		if(nam_istype(np, N_ARRAY))
			array_ptr(np)->cur[0] = lvalue->flag;
		nam_longput(np, n);
		break;
	}
	case LOOKUP:
	{
#if (defined(NLS) || defined(KJI))
		wchar_t c; 
		int clen;
#else
		register int c;
#endif
		lvalue->value = (char*)0;
#if (defined(NLS) || defined(KJI))
		clen = NCdec(str,&c);
		if(NLSletter(c))
		{
			char savc;

			register struct namnod *np;
			str += clen;
			while((clen=NCdec(str,&c))>0, NLSalphanum(c))
				str += clen;
			savc = *str;
			*str = 0;
			np = nam_search(*ptr,sh.var_tree,N_ADD);
			*str = savc;
#else
		c = *str;
		if(isalpha(c))
		{
			register struct namnod *np;
			while(c= *++str, isalnum(c));
			*str = 0;
			np = nam_search(*ptr,sh.var_tree,N_ADD);
			*str = c;
#endif
			if(c=='(')
			{
				lvalue->value = (char*)MSGSTR(E_FUNCTION, (char *)e_function); /*MSG*/
				str = *ptr;
				break;
			}
			else if(c=='[')
			{
#ifndef KSH_88D
				array_dotset(np,0);
#endif /* KSH_88D */
				str =array_subscript(np,str);
			}
			else if(nam_istype(np,N_ARRAY))
				array_dotset(np,ARRAY_UNDEF);
			lvalue->value = (char*)np;
			if(nam_istype(np,N_ARRAY))
				lvalue->flag = array_ptr(np)->cur[0];
		}
		else
		{
#ifndef KSG_88D
			register int base = 10;
#endif /* KSH_88D */
#ifdef FLOAT
			char isfloat = 0;
#endif /* FLOAT */
#ifdef  KSH_88D
                        sh_lastbase = 10;
#endif /* KSH_88D */
#ifdef KJI
			while(str += NCdec(str,&c), c>0) switch(c)
#else
			while(c= *str++)  switch(c)
#endif
			{
			case '#':
#ifdef  KSH_88D
                                sh_lastbase = r;
#else
				sh_lastbase = base = r;
#endif /* KSH_88D */
				r = 0;
				break;
#ifdef  KSH_88D
                        case '.':
                        {
                                /* skip past digits */
                                if(sh_lastbase!=10)
                                        goto badnumber;
                                while(c= *str++,isdigit(c));
#ifdef FLOAT
                                isfloat = 1;
                                if(c=='e' || c == 'E')
                                {
                                dofloat:
                                        c = *str;
                                        if(c=='-'||c=='+')
                                                c= *++str;
                                        if(!isdigit(c))
                                                goto badnumber;
                                        while(c= *str++,isdigit(c));
                                }
                                else if(!isfloat)
                                        goto badnumber;
                                set_float();
                                r = atof(*ptr);
#endif /* FLOAT */
                                goto breakloop;
                        }
#else
			case ':':
			{
#ifdef FLOAT
				char *val = (str-1);
				isfloat = 1;
				while(c= *str,isdigit(c))
					str++;
				if(c=='e' || c == 'E')
				{
				dofloat:
					c = *str++;
					if(c=='-')
						c = *str++;
					while(isdigit(c))
						c = *str++;
				}
				else if(!isfloat)
					breakloop;
				set_float();
				r += atof(val);
#endif /* FLOAT */
				goto endloop;
			}
#endif /* KSH_88D */
			default:
				if(isdigit(c))
					c -= '0';
				else if(isupper(c))
					c -= ('A'-10); 
				else if(islower(c))
					c -= ('a'-10); 
				else
#ifdef  KSH_88D
                                        goto breakloop;
                                if( c < sh_lastbase )
                                        r = sh_lastbase*r + c;
#else
					c = 100;
				if( c < base )
					r = base*r + c;
#endif /* KSH_88D */
				else
				{
#ifdef FLOAT
#ifdef  KSH_88D
                                        if(c == 0xe && sh_lastbase == 10)
#else
					if(c == 0xe)
#endif /* KSH_88D */
						goto dofloat;
#endif /* FLOAT */
#ifdef  KSH_88D
                                        goto breakloop;
#else
                                        goto endloop;
#endif /* KSH_88D */
				}
			}
#ifdef  KSH_88D
                        breakloop:
#else
                        endloop:
#endif /* KSH_88D */
				str--;
		}
		break;
#ifdef  KSH_88D
        badnumber:
                lvalue->value = (char *) e_number; /* make into MSG */
                return (r);

#endif /* KSH_88D */
	
	}
	case VALUE:
	{
		register union Namval *up;
		register struct namnod *np;
		np = (struct namnod*)(lvalue->value);
               	if (nam_istype (np, N_INTGER))
		{
#ifdef NAME_SCOPE
			if (nam_istype (np,N_CWRITE))
				np = nam_copy(np,1);
#endif
			if(nam_istype (np, N_ARRAY))
				up = &(array_find(np,A_ASSIGN)->namval);
			else
				up= &np->value.namval;
			if(nam_istype(np,N_INDIRECT))
				up = up->up;
			if(nam_istype (np, (N_BLTNOD)))
				r = (long)((*up->fp->f_vp)());
			else if(up->lp==NULL)
				r = 0;
#ifdef FLOAT
			else if(nam_istype (np, N_DOUBLE))
			{
				set_float();
       	                	r = *up->dp;
			}
#endif /* FLOAT */
			else
       	                	r = *up->lp;
		}
		else
		{
			if((str=nam_strval(np))==0 || *str==0)
				*ptr = 0;
			else
				r = streval(str, &str, arith);
		}
		return(r);
	}
	case ERRMSG:
		sh_fail(*ptr,lvalue->value);
	}

	*ptr = str;
	return(r);
}

number sh_arith(char *str)
{
	return(streval(str,&str, arith));
}

