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
static char rcsid[] = "@(#)$RCSfile: name.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:27:25 $";
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

#include	"defs.h"
#include	"sym.h"
#include	"builtins.h"
#include	"history.h"
#include	"timeout.h"
#ifdef _locale_
#   include	<locale.h>
#   ifdef MULTIBYTE
#	include	"national.h"
	extern unsigned char _ctype[];
#   endif /* MULTIBYTE */
#endif /* _locale_ */

#ifdef ECHOPRINT
#   undef ECHO_RAW
#   undef ECHO_N
#endif /* ECHOPRINT */

#ifdef ECHO_RAW
char		*echo_mode();
#endif	/* ECHO_RAW */
void		name_unscope();

/* This module references the following external routines */
extern char	**environ;
extern struct Amemory *gettree();
extern void	gsort();
extern int	gscan_all();
extern int	gscan_some();
extern int	rand();
extern int	scan_all();
extern void	srand();
extern int	strcmp();
extern char	*utos();

#ifdef ECHO_RAW
static char	*echo_arg;
#endif	/* ECHO_RAW */
static void	attstore();
static long	get_rand();
static long	get_second();
static struct Amemory *inittree();
static void	no_export();
static void	pushnam();
static void	pushnid();
static void	rehash();
static void	rm_node();
static int	set_second();
static int	set_rand();
static long	get_lineno();
static int	set_lineno();
static char	*staknam();


static int rsflag; /* used to see if "SHELL" has been set in the environment */
static int initenv;
#ifdef  KSH_88D
static int      pwdset;
#endif /* KSH_88D */
static char	**argnam;
static struct Amemory	*namebase;
static int	attsize;
static char	*attval;

struct Bfunction sh_seconds = { get_second, set_second};
struct Bfunction sh_randnum = { get_rand, set_rand};
struct Bfunction line_numbers = { get_lineno, set_lineno};

#ifdef MSG
#include "ksh_msg.h" 
extern nl_catd catd;
#define MSGSTR(n,s) NLcatgets(catd,MS_KSH,n,s) 
#else
#define MSGSTR(n,s) s
#endif

/* ========	variable and string handling	======== */

/*
 *  Table lookup routine
 *  The table <syswds> is searched for string <w> and corresponding value is returned
 */
sh_lookup(const char *w, SYSTAB syswds)
{
	register int first;
	register const struct sysnod	*syscan;
	register int c;
	if(w==0 || (first= *w)==0)
		return(0);
	syscan=syswds;
	while((c= *syscan->sysnam) && c <= first)
	{
                if(first == c && eq(w,syscan->sysnam)) 
                        return(syscan->sysval);
		syscan++;
	}
	return(0);
}

/*
 * perform parameter assignment on an argument list
 */

void env_setlist(struct argnod	*arg, int xp)
{
	register char *s;
	int traceon;
	if(is_option(ALLEXP))
		xp |= N_EXPORT;
	if(arg)
		traceon = sh_trace((char**)0,0);
	while(arg)
	{
		if(arg->argflag&A_MAC)
			s=mac_trim(arg->argval,0);
		else
			s = arg->argval;
		env_namset(s,sh.var_tree,xp);
		arg=arg->argnxt.ap;
		if(traceon)
		{
			p_setout(ERRIO);
			p_str(s,arg?SP:NL);
		}
	}
}

/*
 * Put <arg> into associative memory.
 * If <xp> & V_FLAG then subscript is not allowed
 * If <xp> & G_FLAG then use the current scope only
 * If <xp> & P_FLAG then assignment is not allowed
 */


struct namnod	*env_namset(char *argi, struct Amemory *root, int xp)
{
	register char *argscan=argi;
	register struct namnod	*n;
        char *sim;
#ifdef  KSH_88D
#if (defined(NLS) || defined(KJI))
        wchar_t sep = *argscan;
        if(root==sh.alias_tree)
        {
                do {
                        argscan += NCdec(argscan,&sep);
                } while(sep != '=' && !expchar(sep) && sep != '/');
                argscan -= (sep > 0377) ? 2 : 1;
        }
        else {
                argscan += NCdec(argscan,&sep);
                if(NLSletter(sep)) {
                        do {
                                argscan += NCdec(argscan,&sep);
                        } while (NLSalphanum(sep));
                        argscan -= (sep > 0377) ? 2 : 1;
                }
#else
        register int sep = *argscan;
        if(root==sh.alias_tree)
        {
                while((sep= *argscan) != '=' && !expchar(sep) && sep!='/')
                        argscan++;
        }
        else if(isalpha(sep))
        {
                do
                {
                        sep = *++argscan;
                }
                while(isalnum(sep));
#endif /* NLS || KJI */
        }
        if(argscan!=argi)
        {
#else
#if (defined(NLS) || defined(KJI))
	wchar_t sep;
	argscan += NCdec(argscan,&sep);
	if(NLSletter(sep) || (root==sh.alias_tree && !expchar(sep)))
	{
		do
		{
			argscan += NCdec(argscan,&sep);
		} while (NLSalphanum(sep));
		argscan -= (sep > 0377) ? 2 : 1;
#else
	register wchar_t sep = *argscan;
	if(isalpha(sep) || (root==sh.alias_tree && !expchar(sep)))
	{
		do
		{
			sep = *++argscan;
		}
		while(isalnum(sep));
#endif /* NLS | KJI */
#endif /* KSH_88D */
		*argscan = 0;
		n = nam_search(argi,root,N_ADD|(xp&G_FLAG));
#if (defined(NLS) || defined(KJI))
		if (sep&~STRIP)
			*argscan = sep>>8;
		else
#endif
		*argscan = sep;
		/* check for subscript*/
		if(sep=='[' && !(xp&V_FLAG))
		{
#ifndef KSH_88D
                        array_dotset(n,0);
#endif
			argscan = array_subscript(n,argscan);
			sep = *argscan;
		}
		else if(nam_istype(n,N_ARRAY))
			array_dotset(n,ARRAY_UNDEF);
		if(sep && ((sep!='=')||(xp&P_FLAG)))
		{
			if(initenv)
				return(0);
#ifdef  KSH_88D
                        goto failed;
#else
                        sh_fail(argi,MSGSTR(E_IDENT, e_ident)); /*MSG*/
#endif /* KSH_88D */
		}
		if(root==sh.alias_tree)
		{
			if(nam_istype(n,T_FLAG|NO_ALIAS))
				    nam_offtype(n,~(NO_ALIAS|T_FLAG|N_EXPORT));
		}
		if(sep == '=')
		{
			argscan++;
			if(n==SHELLNOD)
			{
				sim = path_basename(argscan);
				if(strmatch(sim,"*r*sh"))
					rsflag = 0;	/* restricted shell */
			}
			if(initenv)
			{
				nam_fputval(n, argscan);
				if(n==PWDNOD)
#ifdef  KSH_88D
                                        pwdset++;
#else
                                        path_pwd(0);
#endif /* KSH_88D */
			}
			else
				nam_putval(n, argscan);
			nam_ontype(n, xp&~(G_FLAG|V_FLAG|P_FLAG));
			if ( nam_istype ( n, N_EXPORT ))
				env_check_nls_and_locale (n, initenv);
			if(n==PATHNOD)
			{
				sh.lastpath = 0;
                                gscan_some(rehash,sh.track_tree,T_FLAG,T_FLAG);


#ifdef ECHO_RAW
				echo_arg = NIL;
#endif	/* ECHO_RAW */
			}
			else if(n==VISINOD || ((n==EDITNOD)&&isnull(VISINOD)))
			{
				/* turn on vi or emacs option 
				   if editor name is either */
				argscan = path_basename(argscan);
				if(strmatch(argscan,"*vi"))
				{
					off_option(EDITVI|EMACS|GMACS);
					on_option(EDITVI);
				}
				if(strmatch(argscan,"*macs"))
				{
					off_option(EDITVI|EMACS|GMACS);
					if(*argscan=='g')
						on_option(GMACS);
					else
						on_option(EMACS);
				}
			}
#ifdef MULTIBYTE
			else if(n==CSWIDTHNOD)
			{
				if(ed_setwidth(argscan) && !initenv)
					sh_fail(n->namid,MSGSTR(E_FORMAT, (char *)e_format)); /*MSG*/
			}
#endif /* MULTIBYTE */
		}
		return(n);
	}
	if(initenv)
		return(0);
#ifdef  KSH_88D
failed:
        sh_fail (argi, (root == sh.alias_tree ?
                MSGSTR(E_ALINAME, (char *)e_aliname) :
                MSGSTR(E_IDENT, (char *)e_ident))); /*MSG*/
#else
        sh_fail (argi, MSGSTR(E_IDENT, (char *)e_ident)); /*MSG*/
#endif /* KSH_88D */
	/* NOTREACHED */
}

/*
 * Mark each node is invalid
 */

static void rehash(np)
register struct namnod *np;
{
	nam_ontype(np,NO_ALIAS);
}

/*
 * Assign values to an array
 */

void env_arrayset(struct namnod *np, int argn, char *argv[])
{
	while(--argn > 0)
	{
#ifdef  KSH_88D
                if (argn > 1 || nam_istype (np, N_ARRAY))
#endif /* KSH_88D */
		array_dotset(np,argn-1);
		nam_putval(np,argv[argn]);
	}
}

/*
 * This is the code to read a line and to split it into tokens
 */

void env_readline(char **names, int fd, int raw)
{
	register struct fileblk	*fp;
	register int c;
	int issep;
	int mask;
	int maxtry = 10;
	register struct namnod	*n;
	register char *name;
	int checksep = 1;		/* set when looking for separators */
	int	rel;
	int	escape;
	char *seps;
	char is_eol;
	int savstates = st.states;
	/* save in history file if S_FLAG is set */
	if((raw&S_FLAG) && hist_open())
		st.states |= (RWAIT|FIXFLG);
	if(raw&R_FLAG)
		escape = 0;
	else
		escape = ESCAPE;
	if(names && (name = *names))
	{
		if(seps=strchr(name,'?'))
			*seps = 0;
		n = env_namset(name,sh.var_tree,P_FLAG);
		name = *++names;
		if(seps)
			*seps = '?';
	}
	else
	{
		name = 0;
		n = REPLYNOD;
	}
	seps = nam_fstrval(IFSNOD);
	if(seps==NULL)
		seps = (char*)e_sptbnl; /*NOTX*/
#ifdef KJI
	mask = 0;
#else
	if(strcmp(seps,e_sptbnl)==0) /*NOTX*/
		mask =  ~(' '|'\t'|'\n');
	else
		mask = 0;
#endif
	fp = io_ftable[fd];
	if(!fp || fp == &io_stdout)
	{
		io_init(fd,(struct fileblk*)0,(char*)0);
		fp = io_ftable[fd];
	}
	if(fp->flag&IOWRT)
	{
		int saveout = output;
		output = fd;
		p_flush();
		output = saveout;
		fp->flag &= ~IOWRT;
		fp->last = fp->ptr;
	}
	if(fp->flag&IOSLOW)
		fp->flag |= IOEDIT;
	rel= stak_offset();
	while(1)
	{
		c = io_getc(fd);
		/* check for interrupt */
		if(c == escape)
		{
			int d;

			if((d=io_getc(fd))==NL)
			{
				sh_prompt(0);
				continue;
			}
			fp->ptr--;
#if (defined(NLS) || defined(KJI))
			if (d&~STRIP)
				fp->ptr--;
#endif
		}
		else if(c==EOF && !fiseof(fp) && io_intr(fp)<0)
		{
			if(--maxtry > 0)
				continue;
			fp->flag |= IOERR;
		}
#ifdef KJI
		issep = (NLstrchr(seps,c)!=0);
#else
		issep = !(c&mask) && (strchr(seps,c)!=0);
#endif /* KJI */
		is_eol = (c==NL || c==EOF);
		if(checksep && issep && !is_eol)
		{
                        /* visible adjacent separators signify null fields*/
#ifdef KJI
			if(NLstrchr(e_sptbnl,c)!=0) /*NOTX*/
#else
			if(mask || strchr(e_sptbnl,c)!=0) /*NOTX*/
#endif /* KJI */
				continue;
		}
		checksep = 0;
		if((issep && name) || is_eol)	/* from non-separator to separator */
		{
			if(name==NULL)
			{
				/* remove trailing separators */
#ifdef KJI
				do {
					register char *p, *q;
					wchar_t d;

					p = q = sh.stakbot;
					while ((p += NLchrlen(p)) < sh.staktop)
						q = p;
					NCdec(q,&d);
					if (!NLstrchr(seps,d))
						break;
					sh.staktop = q;
				} while (sh.staktop > sh.stakbot);
#else
				while(sh.staktop>sh.stakbot)
				{
					if(strchr(seps,(int)*--sh.staktop))
						continue;
					sh.staktop++;
					break;
				}
#endif /* KJI */
			}
			stak_zero();
			nam_putval(n,stak_address(rel)); stak_set(rel);
		again:
			if(is_option(ALLEXP))
				nam_ontype(n,N_EXPORT);
			if(name)
			{
				n = env_namset(name,sh.var_tree,P_FLAG);
				name = *++names;
			}
			else
				n = 0;
			if(is_eol)
			{
				if(n==0)
					break;
				nam_putval(n, NULLSTR);
				goto again;
				
			}
			checksep = 1;
		}
		else 		/* not a separator */
		{
			if(c==escape)
				c = io_getc(fd);
			if(sh.staktop >= sh.brkend)
				sh_addmem(BRKINCR);
			stak_push(c);
		}
	}
	if(st.states&FIXFLG)
		hist_flush();
	st.states = savstates;
}


/*
 * print out the name and value of a name-value pair <n>
 */

int env_prnamval(struct namnod	*n, int	flag)
{
	register char *s;
	if(sh.trapnote&SIGSET)
		sh_exit(SIGFAIL);
	if(flag)
		flag = NL;
	if(nam_istype(n,NO_PRINT)==NO_PRINT)
		return(0);
	if(is_afunction(n))
	{
		if(!flag)
			p_str(e_bltfn,0); /*NOTX*/
#ifdef  KSH_88D
                if(n->value.namval.ip==0 || n->value.namval.rp->hoffset <0)
#else
                if(n->value.namval.rp->hoffset <0)
#endif /* KSH_88D */
			flag = NL;
		p_str(n->namid,flag);
		if(!flag)
		{
			p_str(e_fnhdr,0); /*NOTX*/
			hist_list(n->value.namval.rp->hoffset,0,"\n");
		}
		return(n->value.namsz+1);
	}
	if(s=nam_strval(n))
	{
		char numbuf[30];
#ifdef MULTIDIM
		struct Nodval *nv= &n->value;
#endif /* MULTIDIM */
		p_str(n->namid,0);
		if(!flag)
			flag = '=';
	        if (nam_istype (n, N_ARRAY))
		{
#ifdef MULTIDIM
			int i = 0;
#endif /* MULTIDIM */
			if(nam_istype(n,N_INTGER))
			{
				/* copy to a save place */
				strcpy(numbuf,s);
				s = numbuf;
			}
#ifdef MULTIDIM
			do
			{
				int dot = array_ptr(n)->cur[i++]&ARRAY_MASK;
				p_sub(dot,0);
				nv = nv->namval.aray->val[dot];
			}
			while(nv && (nv->namflg&N_ARRAY));
#else
			p_sub(((int)array_ptr(n)->cur[0]&ARRAY_MASK)-1,0);
#endif /* MULTIDIM */
		}
		p_char(flag);
		if(flag != NL)
#ifdef  KSH_88D
#ifdef POSIX
                        p_qstr(s,NL);
#else
                        p_str(s,NL);
#endif /* POSIX */
#else
                        p_str(s,NL);
#endif /* KSH_88D */
		return(1);
	}
	return(0);
}

/*
 * print the name of a node followed by the character c
 */


static void pushnid(np)
struct namnod *np;
{
	*argnam++ = np->namid;
}

/*
 * print the nodes in tree <root> which have attributes <flag> set
 */

void env_scan(int file, int flag, struct Amemory *root, int option)
{
	register char **argv;
	register struct namnod *np;
	register int namec;
	struct namnod *onp = 0;
#if (defined(NLS) || defined(KJI))
	extern int NLstrcmp();
#endif

	p_setout(file);
	namec = gscan_some((void(*)())0,root,flag,flag);
	argv = argnam  = (char**)stak_alloc((namec+1)*BYTESPERWORD);
	namec = gscan_some(pushnid,root,flag,flag);
#if (defined(NLS) || defined(KJI))
	gsort(argv,namec,NLstrcmp);
#else
	gsort(argv,namec,strcmp);
#endif
	while(namec--)
	{
		if((np = nam_search(*argv++,root,N_NULL)) && np!=onp)
		{
			onp = np;
			if(nam_istype(np,N_ARRAY))
			{
				array_dotset(np,ARRAY_AT);
				do
				{
					env_prnamval(np,option);
				}
				while(array_next(np));
			}
			else
				env_prnamval(np,option);
		}
	}
}

static char *staknam(n,value)
char *	value;
register struct namnod	*n;
{
	register char *p,*q;
	q = stak_alloc((unsigned)(strlen(n->namid)+strlen(value)+2));
	p=sh_copy(n->namid,q);
	*p++ = '=';
	strcpy(p,value);
	return(q);
}


/*
 * print the attributes of name value pair give by <n>
 */

void	env_prattr(struct namnod *n)
{
	register const struct sysnod	*syscan;
	register unsigned val;
#ifdef FLOAT
	register unsigned mask;
#endif /* FLOAT */
	if (namflag(n) != N_DEFAULT)
	{
		syscan=tab_attributes;
		while(*syscan->sysnam)
		{
			val = syscan->sysval;
#ifdef FLOAT
			mask = val;
			if(val&N_INTGER)
				mask |= N_DOUBLE;
			if(nam_istype(n,mask)==val)
#else
			if(nam_istype(n,val)==val)
#endif /* FLOAT */
			{
				p_str(syscan->sysnam,SP);
				if(val == (N_BLTNOD|N_INTGER))
					break;
		                if (nam_istype (n, N_LJUST|N_RJUST|N_ZFILL))
					p_num((int)n->value.namsz,SP);
			}
		        if(val == N_INTGER && nam_istype(n,N_INTGER))
			{
				if(n->value.namsz != 10)
				{
#ifdef FLOAT
					if(nam_istype(n, N_DOUBLE))
						p_str(e_precision,SP); /*NOTX*/
					else
#endif /* FLOAT */
						p_str(e_intbase,SP); /*NOTX*/
					p_num((int)n->value.namsz,SP);
				}
				break;
			}
			syscan++;
		}
		p_str(n->namid,NL);
	}
}

/*
 * read in the process environment and set up name-value pairs
 * skip over items that are not name-value pairs
 */

int	env_init(void)
{
	register char *cp;
	register struct namnod	*n;
	register char **e=environ;
	char *next;
	rsflag = 1;
	initenv = 1;
	if(e)
	{
		while(cp= *e++)
		{
			if(*cp=='A' && cp[1]=='_' && cp[2]=='_' &&
				cp[3]=='z' && cp[4]=='=')
			{
				cp +=4;
				do
				{
					if(next = strchr(++cp,'='))
						*next = 0;
					n = nam_search(cp+2,sh.var_tree,N_ADD);
					nam_newtype(n,(unsigned)(cp[0]-' ')|N_IMPORT|N_EXPORT,(int)cp[1]-' ');
				}
				while(cp=next);
			}
			else if(n=env_namset(cp,sh.var_tree,(N_IMPORT|N_EXPORT))) 
				n->value.namenv = cp;
		}
	}
	initenv = 0;
#ifdef  KSH_88D
        if (pwdset)
                path_pwd (0);
#endif /* KSH_88D */
	return(rsflag);
}

/*
 * put the name and attribute into value of attributes variable
 */

static void attstore(n)
register struct namnod	*n;
{
	register int flag = namflag(n);
	if(!(flag&N_EXPORT) || ((flag&N_INTGER) && (flag&N_BLTNOD)))
		return;
	flag &= (N_RDONLY|N_UTOL|N_LTOU|N_RJUST|N_LJUST|N_ZFILL|N_INTGER);
	*attval++ = '=';
	*attval++ = ' '+flag;
	if(flag&N_INTGER)
		*attval = ' '+n->value.namsz;
	else
		*attval = ' ';
	attval = sh_copy(n->namid,++attval);
}

static void pushnam(n)
register struct namnod	*n;
{
	register char *value;
	if(nam_istype(n,N_IMPORT))
		*argnam++ = n->value.namenv;
	else if(value=nam_strval(n))
		*argnam++ = staknam(n,value);
	if(nam_istype(n,N_RDONLY|N_UTOL|N_LTOU|N_RJUST|N_LJUST|N_ZFILL|N_INTGER))
		attsize += (strlen(n->namid)+4);
}

/*
 * Generate the environment list for the child.
 */


char **env_gen(void)
{
	register char **er;
	register int namec;
	register char *cp;
	/* L_ARGNOD gets generated automatically as full path name of command */
	nam_offtype(L_ARGNOD,~N_EXPORT);
	attsize = 6;
	namec = gscan_some ((void(*)())0,sh.var_tree, N_EXPORT|N_IMPORT, N_EXPORT);
	er = (char**)stak_alloc((namec+3)*BYTESPERWORD);
	argnam = ++er;
	gscan_some (pushnam, sh.var_tree, N_EXPORT|N_IMPORT, N_EXPORT);
	*argnam = (char*)stak_alloc((unsigned)attsize);
	cp = attval = sh_copy(e_envmarker,*argnam); /*NOTX*/
	gscan_some(attstore,sh.var_tree,
		(N_RDONLY|N_UTOL|N_LTOU|N_RJUST|N_LJUST|N_ZFILL|N_INTGER),
		(N_RDONLY|N_UTOL|N_LTOU|N_RJUST|N_LJUST|N_ZFILL|N_INTGER));
	*attval = 0;
	if(cp!=attval)
		argnam++;
	*argnam = 0;
	return(er);
}

/*
 * Initialize the shell name and alias table
 */

void nam_init(void)
{
	namebase = sh.var_tree = inittree(node_names);
	/* set up the seconds clock */
	set_second(0L);
	sh.alias_tree = inittree(alias_names);
        sh.track_tree = inittree(tracked_names);

	/* for back compatibility scope alias in front of tracked pathnames */
	sh.alias_tree->nexttree = sh.track_tree;
	sh.fun_tree = gettree(MEMSIZE/4);
}

/*
 * initialize name-value pairs
 */

static struct Amemory *inittree(name_vals)
struct name_value *name_vals;
{
	register struct namnod *np;
	register struct name_value *nv;
	{
		register unsigned flag = 0;
		for(nv=name_vals;*nv->nv_name;nv++)
			flag++;
		np = (struct namnod*)malloc((size_t)(flag*sizeof(struct namnod)));
		if(sh.bltin_nodes==0)
			sh.bltin_nodes = np;
	}
	{
		register struct Amemory *treep;
		treep = gettree(MEMSIZE);
		for(nv=name_vals;*nv->nv_name;nv++,np++)
		{
			np->namid = (char*)nv->nv_name;
			np->value.namval.cp = (char*)nv->nv_value;
#ifdef apollo
			if(*nv->nv_value==0)
				np->value.namval.cp = 0;
#endif	/* apollo */
			nam_typeset(np,nv->nv_flags);
			if(nam_istype(np,N_INTGER))
				np->value.namsz = 10;
			else
				np->value.namsz = 0;
			nam_link(np, treep);
		}
		return(treep);
	}
}

/*
 * create a new environment scope
 */

void nam_scope(struct argnod *envlist)
{
	register struct Amemory *sav_var_tree = sh.var_tree;
	register struct Amemory *newscope;
	newscope = gettree(MEMSIZE/8);
	newscope->nexttree = sav_var_tree;
	sh.var_tree = newscope;
	env_setlist(envlist,N_EXPORT|G_FLAG);
	newscope->nexttree = NULL;
	sh.var_tree = sav_var_tree;
	scan_all(no_export,newscope);
	newscope->nexttree = sav_var_tree;
	sh.var_tree = newscope;
}

/*
 * Temporarily remove name from export list of previous scopes
 */

static void no_export(nnod)
register struct namnod *nnod;
{
	register struct namnod *np = nam_search(nnod->namid,sh.var_tree,N_NULL);
	if(np && nam_istype(np,N_EXPORT))
	{
		nam_offtype(np,~N_EXPORT);
		nam_ontype(np,E_FLAG);
	}
}

/*
 * free up top environment scope
 */

void nam_unscope(void)
{
	register struct Amemory *ap = sh.var_tree;
	if((sh.var_tree = ap->nexttree)==NULL)
		sh.var_tree = namebase;
	scan_all(rm_node,ap);
	free((void *)ap);
}

/*
 * free up all environment scopes except the first
 */

void name_unscope()
{
	while(sh.var_tree->nexttree)
		nam_unscope();
}

/*
 * Remove a node and free up all the space
 * Restate export attribute for hidden nodes if necessary
 */
static void rm_node(nnod)
register struct namnod *nnod;
{
	register struct namnod *np = nam_search(nnod->namid,sh.var_tree,N_NULL);
	if(np && nam_istype(np,E_FLAG))
	{
		nam_offtype(np,~E_FLAG);
		nam_ontype(np,N_EXPORT);
	}
	nam_offtype(nnod,~N_EXPORT);
	env_nolocal(nnod);
	free((void *)nnod);
}

/* 
 * Remove freeable local space associated with the namval field
 * of nnod. This includes any strings representing the value(s) of the
 * node, as well as its dope vector, if it is an array.
 */

void	env_nolocal (struct namnod *nnod)
{
	if (nam_istype(nnod, N_EXPORT|N_FREE))
		return;
	if (nam_istype(nnod, N_ARRAY))
		array_dotset(nnod,ARRAY_AT);
	nam_free(nnod);
	nam_typeset(nnod, N_DEFAULT);
}

/*
 * Get the value of a built-in node
 * A nam_search may not be necessary
 */

char *nam_fstrval(struct namnod	*n)
{
	if(sh.var_tree->nexttree)
		n = nam_search(node_names[(n-sh.bltin_nodes)].nv_name,sh.var_tree,N_ADD);
	return(nam_strval(n));
}

/*
 *  for the whence command
 */

void	sh_whence(char **com, int flag)
{
	register const char *a1;
	register struct namnod *np;
	register const char *cp;
	register int type;
	int notrack = 1;
	int pflag = (flag&P_FLAG);
	flag &= ~P_FLAG;
	while(a1 = *++com)
	{
		if(flag)
			p_str(a1,0);
		if(pflag)
			goto search;
		/* reserved words first */
		if(sh_lookup(a1,tab_reserved))
		{
			if(flag)
				a1 = MSGSTR(IS_RESERVED, (char *)is_reserved); /*MSG*/
		}
		/* non-tracked aliases */
		else if((np=nam_search(a1,sh.alias_tree,N_NULL))
			&& (notrack=(nam_istype(np,T_FLAG)==0))
			&& (a1=nam_strval(np))) 
		{
			if(flag)
			{
				if(nam_istype(np,N_EXPORT))
					cp = MSGSTR(IS_XALIAS, (char *)is_xalias); /*MSG*/
				else
					cp = MSGSTR(IS_ALIAS, (char *)is_alias); /*MSG*/
				p_str(cp,0);
			}
		}
                /* built-in commands next */
                else if(type = sh_lookup(a1,tab_builtins))
                {
                        /* check for builtins */
			if ( !( type == SYSSU             ||
				type == SYSSHELL          ||
				type == SYSLOGOUT         ||
				type == SYSSETSENV        ||
				type == SYSSETGROUPS) && flag) {
				a1 = MSGSTR(IS_BUILTIN, (char *)is_builtin); /*MSG*/
			}
			else
                                goto search;
                }
		/* functions next */
		else if((np=nam_search(a1,sh.fun_tree,0)) && np->value.namenv)
		{
			if(flag)
			{
				if(isnull(np))
				{
					if(!nam_istype(np,N_FUNCTION))
						goto search;
					a1 = MSGSTR(IS_UFUNCTION, (char *)is_ufunction); /*MSG*/
				}
				else if(nam_istype(np,N_EXPORT))
					a1 = MSGSTR(IS_XFUNCTION, (char *)is_xfunction); /*MSG*/
				else
					a1 = MSGSTR(IS_FUNCTION, (char *)is_function); /*MSG*/
			}
		}
		else
		{
		search:
			if(path_search(a1,2)==0)
				a1 = sh.lastpath;
			sh.lastpath = 0;
			if(a1)
			{
				if(flag)
				{
					if(*a1!= '/')
						a1 = MSGSTR(IS_UFUNCTION, (char *)is_ufunction); /*MSG*/
					else
					/* tracked aliases next */
					if(!notrack && *a1 == '/') {
						p_str(MSGSTR(IS_TALIAS, 
						    (char *)is_talias),0); /*MSG*/
					 } else
						p_str(MSGSTR(IS_,(char *)is_),0);/*MSG*/
				}
			}
			else 
			{
				sh.exitval |= 1;
				a1 = MSGSTR(E_FOUND, (char *)e_found); /*MSG*/
				if(!flag)
					return;
			}
		}
#ifdef  KSH_88D
#ifdef POSIX
                p_qstr(a1,NL);
#else
                p_str(a1,NL);
#endif /* POSIX */
#else
                p_str(a1,NL);
#endif /* KSH_88d */
	}
}

/*
 * these functions are used to get and set the SECONDS variable
 */

static time_t sec_offset;

static int set_second(n)
long n;
{
	sec_offset =  time((time_t*)0) - (time_t)n ;
}

static long get_second()
{
	return((long)(time((time_t*)0)-sec_offset));
}

/*
 * These functions are used to get and set the RANDOM variable
 */

#ifdef  KSH_88D
static unsigned last_rand;
static int set_rand(n)
long n;
{
        srand(n&0x7fff);
        last_rand = 0;
}
#else
static int set_rand(n)
long n;
{
        srand(n&0x7fff);
}
#endif /* KSH_88D */

/*
 * get random number in range of 0 - 2**15
 * never pick same number twice in a row
 */

static long get_rand()
{
#ifdef  KSH_88D
        register int cur;
        do
                cur = sh_rand();
        while(cur==last_rand);
        last_rand = cur;
#else
        static unsigned last;
        register int cur;
        do
                cur = sh_rand();
        while(cur==last);
        last = cur;
#endif /* KSH_88D */
        return((long)cur);
}

static long get_lineno()
{
	return((long)st.cmdline);
}

static int set_lineno(n)
{
        if (!initenv)
		st.standin->flin = n;
}

#ifdef ECHO_RAW
char *echo_mode()
{
	register char *cp;
	optflag savopts;
	if(echo_arg==0)
	{
		savopts = opt_flags;
		off_option(HASHALL);
		cp = path_absolute(e_echobin+5); /*NOTX*/
		opt_flags = savopts;
#ifdef  KSH_88D
                if(cp && eq(cp,e_echobin)) /*NOTX*/
#else
                if(eq(cp,e_echobin)) /*NOTX*/
#endif /* KSH_88D */
			echo_arg = (char*)e_echoflag; /*NOTX*/
		else
			echo_arg = (char*)e_minus; /*NOTX*/
	}
	return(echo_arg);
}
#endif	/* ECHO_RAW */
