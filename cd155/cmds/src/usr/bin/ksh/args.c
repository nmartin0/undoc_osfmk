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
static char rcsid[] = "@(#)$RCSfile: args.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:27:03 $";
#endif
/*
static char sccsid[] = "@(#)args.c      1.14  com/cmd/ksh/sh,3.1 4/6/90 09:34:14";
*/
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
 * UNIX shell
 *
 * S. R. Bourne
 * Rewritten by David Korn
 * AT&T Bell Laboratories
 *
 */

#include	"defs.h"
#ifdef DEVFD
#   include	"jobs.h"
#endif	/* DEVFD */
#include	"sym.h"
#include	"builtins.h"


#ifdef DEVFD
    void	close_pipes();
#endif	/* DEVFD */

#if (defined(NLS) || defined(KJI))
#	include	<NLchar.h>
#endif

extern void	gsort();
extern int	strcmp();

#ifdef  KSH_88D
static int              arg_expand(struct argnod *);
#else
static int              arg_expand();
#endif /* KSH_88D */
static struct dolnod*	copyargs();
static void		print_opts();
static int		split();

static char		*null;
static struct dolnod	*argfor; /* linked list of blocks to be cleaned up */
static struct dolnod	*dolh;
static char flagadr[12];
static const char flagchar[] =
{
	'i',	'n',	'v',	't',	's',	'x',	'e',	'r',	'k',
	'u',	'f',	'a',	'm',	'h',	'p',	'c',	0
};
static const optflag flagval[]  =
{
	INTFLG,	NOEXEC,	READPR,	ONEFLG, STDFLG,	EXECPR,	ERRFLG,	RSHFLG,	KEYFLG,
	NOSET,	NOGLOB,	ALLEXP,	MONITOR, HASHALL, PRIVM, CFLAG, 0
};

#ifdef MSG
#include "ksh_msg.h" 
extern nl_catd catd;
#define MSGSTR(n,s) NLcatgets(catd,MS_KSH,n,s) 
#else
#define MSGSTR(n,s) s
#endif

/* ======== option handling	======== */

/*
 *  This routine turns options on and off
 *  The options "sicr" are illegal from set command.
 *  The -o option is used to set option by name
 *  This routine returns the number of non-option arguments
 */

int arg_opts(int argc, char **argv)
{
	register char *cp;
	register int c;
	register char *flagc;
	register optflag newflags=opt_flags;
	register optflag opt;
	char minus;
	struct namnod *np = (struct namnod*)0;
	char sort = 0;
	char minmin = 0;
	int setflag = (st.states&BUILTIN);
	while((cp= *++argv) && ((c= *cp) == '-' || c=='+'))
	{
		minus = (c == '-');
		argc--;
		if((c= *++cp)==0)
		{
			newflags &= ~(EXECPR|READPR);
			argv++;
			break;
		}
		else if(c == '-')
		{
			minmin = 1;
			argv++;
			break;
		}
		while(c= *cp++)
		{
			if(setflag)
			{
				if(c=='s')
				{
					sort = 1;
					continue;
				}
				else if(c=='A')
				{
#ifdef  KSH_88D
                                        if (argv[1] == 0)
                                                sh_fail (*argv, e_argexp);
#endif /* KSH_88D */
					np = env_namset(*++argv,sh.var_tree,P_FLAG|V_FLAG);
					argc--;
					if(minus)
						nam_free(np);
					continue;
				}
				else if(strchr("icr",c))
					sh_fail(*argv, MSGSTR(E_OPTION, (char *)e_option)); /*MSG*/
			}
				/*	PTM 20825
				*	ksh -c read from stdin when argc
				*	was less than 2; changed to exit
				*	shell is argc < 2
				*/
			if(c=='c' && minus)
				if ( argc < 2 )
					sh_done (0);
				else if ( sh.comdiv==0 )
				{
					sh.comdiv= *++argv;
					argc--;
					newflags |= CFLAG;
					continue;
				}
			if(flagc=strchr(flagchar,c))
				opt = flagval[flagc-flagchar];
			else if(c != 'o')
				sh_fail(*argv,MSGSTR(E_OPTION, (char *)e_option)); /*MSG*/
			else
			{
				argv++;
				if(*argv==NIL)
				{
					print_opts(newflags);
					argv--;
					continue;
				}
				else
				{
					argc--;
					c=sh_lookup(*argv,tab_options);
					opt = 1L<<c;
					if(opt&(1|INTFLG|RSHFLG))
						sh_fail(*argv,MSGSTR(E_OPTION, (char *)e_option)); /*MSG*/
				}
			}
			if(minus)
			{
#if ESH || VSH
				if(opt&(EDITVI|EMACS|GMACS))
					newflags &= ~ (EDITVI|EMACS|GMACS);

#endif /* ESH || VSH */
				newflags |= opt;
			}
			else
				newflags &= ~opt;
		}
	}
	/* cannot set -n for interactive shells since there is no way out */
	if(is_option(INTFLG))
		newflags &= ~NOEXEC;
#if defined(RAWONLY)
	if( is_option(EDITVI))
		newflags |= VIRAW;
#endif	/* RAWONLY */
	if(!setflag)
		goto skip;
	if(sort)
	{
#if (defined(NLS) || defined(KJI))
		if(argc>1)
			gsort(argv,argc-1,NLstrcmp);
		else
			gsort(st.dolv+1,st.dolc,NLstrcmp);
#else
		if(argc>1)
			gsort(argv,argc-1,strcmp);
		else
			gsort(st.dolv+1,st.dolc,strcmp);
#endif
	}
	if((newflags&PRIVM) && !is_option(PRIVM))
	{
		if((sh.userid!=sh.euserid && setuid(sh.euserid)<0) ||
			(sh.groupid!=sh.egroupid && setgid(sh.egroupid)<0) ||
			(sh.userid==sh.euserid && sh.groupid==sh.egroupid))
			newflags &= ~PRIVM;
	}
	else if(!(newflags&PRIVM) && is_option(PRIVM))
	{
		setuid(sh.userid);
		setgid(sh.groupid);
		if(sh.euserid==0)
		{
			sh.euserid = sh.userid;
			sh.egroupid = sh.groupid;
		}
	}
skip:
	opt_flags = newflags;
	if(setflag)
	{
		argv--;
		if(np)
			env_arrayset(np,argc,argv);
#ifdef  KSH_88D
                else if(argc>1 || minmin)
#else
                if(argc>1 || minmin)
#endif /* KSH_88D */
			arg_set(argv);
	}
	return(argc);
}

/*
 * returns the value of $-
 */

char *arg_dolminus(void)
{
	register const char *flagc=flagchar;
	register char *flagp=flagadr;
	while(*flagc)
	{
		if(opt_flags&flagval[flagc-flagchar])
			*flagp++ = *flagc;
		flagc++;
	}
	*flagp = 0;
	return(flagadr);
}

/*
 * set up positional parameters 
 */

void arg_set(char *argi[])
{
	register char **argp=argi;
	register int size = 0; /* count number of bytes needed for strings */
	register char *cp;
	register int 	argn;
	/* count args and number of bytes of arglist */
	while((cp=(char*)*argp++) != ENDARGS)
	{
		size += strlen(cp);
	}
	/* free old ones unless on for loop chain */
	argn = argp - argi;
	arg_free(dolh,0);
	dolh=copyargs(argi, --argn, size);
	st.dolc=argn-1;
}

/*
 * free the argument list if the use count is 1
 * If count is greater than 1 decrement count and return same blk
 * Free the argument list if the use count is 1 and return next blk
 * Delete the blk from the argfor chain
 * If flag is set, then the block dolh is not freed
 */

struct dolnod *arg_free(struct dolnod *	blk, int flag)
{
	register struct dolnod*	argr=blk;
	register struct dolnod*	argblk;
	if(argblk=argr)
	{
		if((--argblk->doluse)==0)
		{
			if(flag && argblk==dolh)
				dolh->doluse = 1;
			else
			{
				/* delete from chain */
				if(argfor == argblk)
					argfor = argblk->dolnxt;
				else
				{
					for(argr=argfor;argr;argr=argr->dolnxt)
						if(argr->dolnxt==argblk)
							break;
					if(argr==0)
					{
						return(NULL);
					}
					argr->dolnxt = argblk->dolnxt;
				}
				free((void *)argblk);
			}
			argr = argblk->dolnxt;
		}
	}
	return(argr);
}

/*
 * grab space for arglist and link argblock for cleanup
 * The strings are copied after the argment vector
 */

static struct dolnod *copyargs(from, n, size)
char *from[];
{
	register struct dolnod *dp=new_of(struct dolnod,n*sizeof(char*)+size+n);
	register char **pp;
	register char *sp;
	dp->doluse=1;	/* use count */
	/* link into chain */
	dp->dolnxt = argfor;
	argfor = dp;
	pp= dp->dolarg;
	st.dolv=pp;
	sp = (char*)dp + sizeof(struct dolnod) + n*sizeof(char*);
	while(n--)
	{
		*pp++ = sp;
		sp = sh_copy(*from++,sp) + 1;
	}
	*pp = ENDARGS;
	return(dp);
}

/*
 *  used to set new argument chain for functions
 */

struct dolnod *arg_new(char *argi[], struct dolnod **savargfor)
{
	register struct dolnod *olddolh = dolh;
	*savargfor = argfor;
	dolh = NULL;
	argfor = NULL;
	arg_set(argi);
	return(olddolh);
}

/*
 * reset arguments as they were before function
 */

void arg_reset(struct dolnod *blk, struct dolnod *afor)
{
	while(argfor=arg_free(argfor,0));
	dolh = blk;
	argfor = afor;
}

void arg_clear(void)
{
	/* force `for' $* lists to go away */
	while(argfor=arg_free(argfor,1));
	argfor = dolh;
#ifdef DEVFD
	close_pipes();
#endif	/* DEVFD */
}

/*
 * increase the use count so that an arg_set will not make it go away
 */

struct dolnod *arg_use(void)
{
	register struct dolnod *dh;
	if(dh=dolh)
		dh->doluse++;
	return(dh);
}

/*
 *  Print option settings on standard output
 */

static void print_opts(oflags)
#ifndef pdp11
register
#endif	/* pdp11 */
optflag oflags;
{
	register const struct sysnod *syscan = tab_options;
#ifndef pdp11
	register
#endif	/* pdp11 */
	optflag value;
	p_setout(st.standout);
	p_str(MSGSTR(E_HEADING, (char *)e_heading),NL); /*MSG*/
	while(value=syscan->sysval)
	{
		value = 1<<value;
		p_str(syscan->sysnam,SP);
		p_nchr(SP,16-strlen(syscan->sysnam));
		if(oflags&value)
			p_str(MSGSTR(E_ON, (char *)e_on),NL); /*MSG*/
		else
			p_str(MSGSTR(E_OFF, (char *)e_off),NL); /*MSG*/
		syscan++;
	}
}

#ifdef VPIX
#   define EXTRA 2
#else
#   define EXTRA 1
#endif /* VPIX */

/*
 * build an argument list
 */

char **arg_build(int *nargs, struct comnod *comptr)
{
	register struct argnod	*argp;
	{
		register struct comnod	*ac = comptr;
		register struct argnod	*schain;
		/* see if the arguments have already been expanded */
		if(ac->comarg==NULL)
		{
			*nargs = 0;
			return(&null);
		}
		else if((ac->comtyp&COMSCAN)==0)
		{
			*nargs = ((struct dolnod*)ac->comarg)->doluse;
			return(((struct dolnod*)ac->comarg)->dolarg+EXTRA);
		}
		schain = st.gchain;
		st.gchain = NULL;
#ifdef  KSH_88D
#ifdef  DEVFD
                close_pipes ();
#endif /* DEVFD */
                *nargs = 0;
                if (ac) {
                        argp = ac->comarg;
                        while (argp) {
                                *nargs += arg_expand (argp);
                                argp = argp->argnxt.ap;
                        }
                }
#else
		*nargs = arg_expand(ac);
#endif /* KSH_88D */
		argp = st.gchain;
		st.gchain = schain;
	}
	{
		register char	**comargn;
		register int	argn;
		register char	**comargm;
		argn = *nargs;
		argn += EXTRA;	/* allow room to prepend args */
		comargn=(char**)stak_alloc((unsigned)(argn+1)*sizeof(char*));
		comargm = comargn += argn;
		*comargn = ENDARGS;
		while(argp)
		{
#ifdef  KSH_88D
                        struct argnod   *nextarg = argp->argchn;
                        argp->argchn = 0;
#endif /* KSH_88D */
			*--comargn = argp->argval;
			if((argp->argflag&A_RAW)==0)
				sh_trim(*comargn);
#ifdef  KSH_88D
                        if((argp=nextarg)==0 || (argp->argflag&A_MAKE))
#else
                        if((argp=argp->argchn)==0 || (argp->argflag&A_MAKE))
#endif
			{
				if((argn=comargm-comargn)>1)
#if (defined(NLS) || defined(KJI))
					gsort(comargn,argn,NLstrcmp);
#else
					gsort(comargn,argn,strcmp);
#endif
				comargm = comargn;
			}
		}
		return(comargn);
	}
}

#ifdef DEVFD
static int to_close[15];
#ifdef  KSH_88D
static  int     indx;
#endif /* KSH_88D */

void close_pipes()
{
	register int *fd = to_close;
	while(*fd)
	{
		close(*fd);
		*fd++ = -1;
	}
#ifdef  KSH_88D
        indx = 0;
#endif /* KSH_88D */
}
#endif	/* DEVFD */

/* Argument list generation */
#ifdef  KSH_88D
static int arg_expand(register struct argnod *argp)
{
        register int    count=0;
#else
static int arg_expand(ac)
struct comnod		*ac;
{
	register struct argnod	*argp;
	register int 	count=0;
#ifdef DEVFD
	int indx = 0;
	close_pipes();
#endif	/* DEVFD */
	if(ac)
	{
		argp = ac->comarg;
		while(argp)
		{
#endif /* KSH_88D */
			argp->argflag &= ~A_MAKE;
#ifdef DEVFD
			if(*argp->argval==0 && (argp->argflag&A_EXP))
			{
				/* argument of the form (cmd) */
				register struct argnod *ap;
				char *cp;
				int pv[2];
				int fd;
				ap = (struct argnod*)stak_begin();
				ap->argflag |= A_MAKE;
				ap->argflag &= ~A_RAW;
				ap->argchn= st.gchain;
				st.gchain = ap;
				count++;
				cp = sh_copy(e_devfd,ap->argval); /*NOTX*/
				io_popen(pv);
				fd = argp->argflag&A_RAW;
				stak_end(sh_copy(sh_itos(pv[fd]),cp));
				sh.inpipe = sh.outpipe = 0;
				if(fd)
				{
					sh.inpipe = pv;
					sh_exec((union anynode*)argp->argchn,(int)(st.states&ERRFLG));
				}
				else
				{
					sh.outpipe = pv;
					sh_exec((union anynode*)argp->argchn,(int)(st.states&ERRFLG));
				}
#ifdef JOBS
				job.pipeflag++;
#endif	/* JOBS */
				close(pv[1-fd]);
				to_close[indx++] = pv[fd];
			}
			else
#endif	/* DEVFD */
			if((argp->argflag&A_RAW)==0)
			{
                                register char *ap = argp->argval;
				if(argp->argflag&A_MAC)
					ap = mac_expand(ap);
#ifdef  KSH_88D
                                count = split(ap,argp->argflag&(A_SPLIT|A_EXP));
#else
				count += split(ap,argp->argflag&(A_SPLIT|A_EXP));
#endif
			}
			else
			{
				argp->argchn= st.gchain;
				st.gchain = argp;
				argp->argflag |= A_MAKE;
				count++;
			}
#ifndef KSH_88D
			argp = argp->argnxt.ap;
		}
	}
#endif /* KSH_88D */
	return(count);
}

static int split(s,macflg) /* blank interpretation routine */
char *s;
{
	register char *argp;
#if !(defined(NLS) || defined(KJI))
	register
#endif
	wchar_t c;
	register struct argnod *ap;
	int 	count=0;
	int expflag = (!is_option(NOGLOB) && (macflg&A_EXP));
	const char *seps;
	if(macflg &= A_SPLIT)
		seps = nam_fstrval(IFSNOD);
	else
		seps = e_nullstr; /*NOTX*/
	if(seps==NULL)
		seps = e_sptbnl;  /*NOTX*/

        c = *s;
#if (defined(NLS) || defined(KJI))
        if ( c != (wchar_t)NULL && NLstrchr((unsigned char *)seps, c) )
                s += NCdec(s, &c);
#else
        if ( c != (wchar_t)NULL && strchr(seps, c) )
                s++;
#endif

	while(1)
	{
		if(sh.trapnote&SIGSET)
			sh_exit(SIGFAIL);
		ap = (struct argnod*)stak_begin();
		argp = ap->argval;
#if (defined(NLS) || defined(KJI))
		while(s += NCdec(s, &c), c > 0)
#else
		while(c = *s++)
#endif
		{
			if(c == ESCAPE)
			{
#if (defined(NLS) || defined(KJI))
				s += NCdec(s, &c);
#else
				c = *s++;
#endif
				if(c!='/')
					*argp++ = ESCAPE;
			}
#if (defined(NLS) || defined(KJI))
			else if(NLstrchr((unsigned char *)seps,c))
#else
			else if(strchr(seps,c))
#endif
				break;
			if(argp >= sh.brkend)
				sh_addmem(BRKINCR);
#if (defined(NLS) || defined(KJI))
			argp += NCenc(&c, argp);
#else
			*argp++ = c;
#endif
		}
	/* This allows contiguous visible delimiters to count as delimiters */
		if(argp==ap->argval)
		{
			if(c==0)
				return(count);
#if (defined(NLS) || defined(KJI))
			if(macflg==0 || NLstrchr((unsigned char *)e_sptbnl, c))
#else
			if(macflg==0 || strchr(e_sptbnl, c)) /*NOTX*/
#endif
				continue;
		}
		else if(c==0)
		{
			s--;
		}
		/* file name generation */
		stak_end(argp);
		ap->argflag &= ~(A_RAW|A_MAKE);
#ifdef BRACEPAT
		if(expflag)
			count += expbrace(ap);
#else
		if(expflag && (c=path_expand(ap->argval)))
			count += c;
#endif /* BRACEPAT */
		else
		{
			count++;
			ap->argchn= st.gchain;
			st.gchain = ap;
		}
		st.gchain->argflag |= A_MAKE;
	}
}
