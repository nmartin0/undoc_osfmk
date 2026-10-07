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
static char sccsid[] = "@(#)macro.c	1.8  com/cmd/ksh/sh,3.1,9013 3/7/90 15:44:19";
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
 * UNIX shell
 *
 * S. R. Bourne
 * AT&T Bell Laboratories
 * Rewritten by David Korn
 *
 */

#include	"defs.h"
#include	"sym.h"
#include	"builtins.h"
#ifdef MULTIBYTE
#   include	"national.h"
#endif /* MULTIBYTE */

/* These external routines are referenced by this module */

extern char		*ltos();
extern char		*match_paren();

#if (defined(MULTIBYTE) || defined(NLS) || defined(KJI))
    static int	charlen();
#endif /* MULTIBYTE|NLS|KJI */

#if (defined(NLS) || defined(KJI))
#define alph(c)	NLSletter(c)
#define alphnum(c) NLSalphanum(c)
#else
#define alph(c)	isalpha(c)
#define alphnum(c)  isalnum(c)
#endif /* NLS | KJI */

static void	copyto();
static char	*substring();
static void	skipto();
static int	getch();
static int	comsubst();
static void	mac_error();
#ifdef  KSH_88D
#ifdef KJI
static void     mac_copy (unsigned char *);
#else
static void     mac_copy (char *);
#endif
#endif /* KSH_88D */

static char	quote;	/* used locally */
static char	quoted;	/* used locally */
static char	mflag;	/* 0 for $x, 1 for here docs */
#ifdef  KSH_88D
static const char *ifs;
#endif /* KSH_88D */
static int	w_fd = -1;
static int mactry;
static char *mac_current;
static jmp_buf mac_buf;
static char	idb[2];
#ifdef FLOAT
static double numb;
#else
static long numb;
#endif /* FLOAT */

#ifdef MSG
#include "ksh_msg.h" 
extern nl_catd catd;
#define MSGSTR(n,s) NLcatgets(catd,MS_KSH,n,s) 
#else
#define MSGSTR(n,s) s
#endif

static void copyto(endch,newquote)
register char	endch;
{
	register int	c;
	register int count = 1;
	int saveq = quote;

	quote = newquote;
	while(c=getch(endch))
	{
#ifdef  KSH_88D
                if((c==endch) && (saveq || !quote) && --count<=0)
#else
                if(c==endch && --count<=0)
#endif /* KSH_88D */
			break;
		if(quote || c==ESCAPE)
		{
			if(c==ESCAPE)
			{
				c = io_readc();
				if(quote && !escchar(c) && c!= '"')
				{
					stak_push(ESCAPE);
					stak_push(ESCAPE);
				}
			}
			if(!mflag || !escchar(c))
				stak_push(ESCAPE);
		}
		if(sh.staktop >= sh.brkend)
			sh_addmem(BRKINCR);
		stak_push(c);
		if(c=='[' && endch==']')
			count++;
	}
	quote = saveq;
	stak_zero();
	if(c!=endch)
		mac_error();
}

/* skip chars up to } */

static void skipto(endch)
register char endch;
{
#if (defined(NLS) || defined(KJI))
	register int	c;
#else
	register char	c;
#endif /* NLS|KJI */
	while((c=io_readc()) && c!=endch)
	{
		switch(c)
		{
			case ESCAPE:
				io_readc();
				break;

			case SQUOTE:	case DQUOTE:
				skipto(c);
				break;

			case DOLLAR:
				if((c=io_readc()) == LBRACE)
					skipto(RBRACE);
				else if(!dolchar(c))
					io_unreadc(c);
		}
	}
	if(c!=endch)
		mac_error();
}

#if (defined(NLS) || defined(KJI))
/*
 *	getch returns either a 1- or 2-byte character (see readc() in word.c)
 */
#endif
static int getch(endch)
int	endch;
{
	register int	c;
	register int	bra; /* {...} bra =1, {#...} bra=2 */
	int atflag;  /* set if $@ or ${array[@]} within double quotes */
retry:
	c = io_readc();
	if(c==DOLLAR)
	{
		register char *v;
#ifdef KJI
		register unsigned char *argp;
#else
		register char *argp;
#endif
		register struct namnod	*n=(struct namnod*)NULL;
		int 	dolg=0;
		int dolmax = st.dolc+1;
		int 	nulflg;
		char *id=idb;
		bra = 0;
		*id = 0;
	retry1:
		c = io_readc();
		switch(c)
		{
			case DOLLAR:
				v=sh_itos(sh.pid);
				break;

			case '!':
#ifdef  KSH_88D
                                if(sh.bckpid)
                                        v=sh_itos(sh.bckpid);
                                else
                                        v = "";
#else
                                v=sh_itos(sh.bckpid);
#endif /* KSH_88D */
				break;

			case LBRACE:
				if(bra++ ==0)
					goto retry1;

			case LPAREN:
				if(bra==0 && mactry==0)
				{
					if(comsubst(1))
						goto retry;
					v = ltos(numb,10);
				}
				else
					goto nosub;
				break;

			case RBRACE:
				if(bra!=2)
					goto nosub;
				bra = 0;
			case '#':
				if(bra ==1)
				{
					bra++;
					goto retry1;
				}
				v=sh_itos(st.dolc);
				break;

			case '?':
				v=sh_itos(sh.savexit&EXITMASK);
				break;

			case '-':
				v=arg_dolminus();
				break;
			
			default:
				if(alph(c))
				{
					int offset = stak_offset();
					while(alphnum(c))
					{
						if(sh.staktop >= sh.brkend)
							sh_addmem(BRKINCR);
						stak_push(c);
						c = io_readc();
					}
					while(c=='[' && bra)
					{
#ifdef KJI
						register int start;
						register char *p, *q;
						start = stak_offset();
#endif
						stak_push('[');
						copyto(']',0);
#ifdef KJI
						p = q = stak_address(start);
						while((p += NLchrlen(p)) > q &&
							p < sh.staktop)
							q = p;
						*id = *q;
						if (NCisshift(*q))
							*(id+1) = *(q+1);
#else
						*id = sh.staktop[-1];
#endif
						stak_push(']');
						c = io_readc();
					}
					io_unreadc(c);
					stak_zero();
					n=env_namset(stak_address(offset),sh.var_tree,P_FLAG);
					stak_set(offset);
					v = nam_strval(n);
#ifdef  KSH_88D
                                        c = (bra==2 && ((c= *id), astchar(c)));
#endif /* KSH_88D */
					if(nam_istype(n,N_ARRAY))
					{
#ifdef  KSH_88D
                                                if(c || (array_next(n) && v))
#else
                                                if((bra==2 && *id) || (array_next(n) && v))
#endif /* KSH_88D */
							dolg = -1;
						else
							dolg = 0;
					}
					else
#ifdef  KSH_88D
                                        {
                                                if(c)
                                                        dolmax = 0;
                                                id = n->namid;
                                        }
#else
                                                id = n->namid;
#endif /* KSH_88D */
					goto cont1;
				}
#ifdef KJI
				if (c&~STRIP) {
					*id = c>>8;
					*(id+1) = c&STRIP;
				}
				else
#endif
				*id = c;
				if(astchar(c))
				{
					dolg=1;
					c=1;
				}
				else if(isdigit(c))
				{
					c -= '0';
					if(bra)
					{
						int d;
						while((d=io_readc(),isdigit(d)))
							c = 10*c + (d-'0');
						io_unreadc(d);
					}
				}
				else
					goto nosub;
				if(c==0)
				{
					if((st.states&PROFILE) && !(st.states&FUNCTION))
						v = sh.shname;
					else
						v = st.cmdadr;
				}
				else if(c <= st.dolc)
					v = st.dolv[c];
				else
					dolg = 0, v = 0;
			}
	cont1:
		c = io_readc();
		if(bra==2)
		{
			if(c!=RBRACE)
				mac_error();
			if(dolg==0 && dolmax)
#if (defined(MULTIBYTE) || defined(NLS) || defined(KJI))
				c = (v?charlen(v):0);
#else
				c = (v?strlen(v):0);
#endif /* MULTIBYTE|NLS|KJI */
			else if(dolg>0)
				c = st.dolc;
			else if(dolg<0)
				c = array_elem(n);
			else
				c = (v!=0);
			v = sh_itos(c);
			dolg = 0;
			c = RBRACE;
		}
		/* check for quotes @ */
		if(idb[0]=='@' && quote && !atflag)
		{
			quoted--;
			atflag = 1;
		}
		if(c==':' && bra)	/* null and unset fix */
		{
			nulflg=1;
			c=io_readc();
		}
		else
			nulflg=0;
		if(!defchar(c) && bra)
			mac_error();
		argp = 0;
		if(bra)
		{
			if(c!=RBRACE)
			{
				bra = stak_offset();
				if(((v==0 || (nulflg && *v==0)) ^ (setchar(c)!=0))
					|| is_option(NOEXEC))
				{
					int newquote = quote;
					if(c=='#' || c == '%')
						newquote = 0;
					copyto(RBRACE,newquote);
				}
				else
					skipto(RBRACE);
				argp=stak_address(bra);
			}
		}
		else
		{
			io_unreadc(c);
			c=0;
		}
		/* check for substring operations */
		if(c == '#' || c == '%')
		{
			if(dolg != 0)
				mac_error();
			if(v && *v)
			{
#ifdef  KSH_88D
#ifdef POSIX
                                char *savarg = argp;
                                char *pat;
#endif /* POSIX */
#endif /* KSH_88D */
				/* allow room for escapes */
				bra = strlen(v);
				sh.staktop += bra;
				while(sh.staktop+bra >= sh.brkend)
					sh_addmem(BRKINCR);
#ifdef  KSH_88D
#ifdef POSIX
                                v = strcpy(sh.staktop,v);
                                do
                                {
                                        if(*argp==c)
                                        {
                                                c |= MARK;
                                                argp++;
                                        }
                                        pat = argp;
                                        while(1)
                                        {
                                                switch(*argp)
                                                {
                                                case '#': case '%':
                                                case 0:
                                                        goto endloop;

                                                case ESCAPE:
                                                        argp++;
                                                default:
                                                        argp++;
                                                }
                                        }
                                endloop:
                                        bra = *argp;
                                        *argp++ = 0;
                                        v = substring(v,pat,c);
                                        c = bra;
                                }
                                while(c);
                                argp = savarg;
#else
				strcpy(sh.staktop,v);
				if(*argp==c)
				{
					c |= MARK;
					argp++;
				}
				v = substring(sh.staktop,argp,c);
				if(c&MARK)
					argp--;
#endif /* POSIX */
#else
                                strcpy(sh.staktop,v);
                                if(*argp==c)
                                {
                                        c |= MARK;
                                        argp++;
                                }
                                v = substring(sh.staktop,argp,c);
                                if(c&MARK)
                                        argp--;
#endif /* KSH_88D */
                        }
			sh.staktop = argp;
		}
	retry2:
		if(v && (!nulflg || *v ) && c!='+')
		{
#ifndef KSH_88D
			int no_ifs = 0;
#endif /* !KSH_88D */
			int type = *id;
#define sep bra
#ifdef  KSH_88D
                        if(*ifs)
                                sep = *ifs;
                        else
                                sep = SP;
#else
                        sep = SP;
                        argp = nam_fstrval(IFSNOD);
                        if(argp==0 || *argp==0)
                                no_ifs++;
                        else
                                sep = *argp;
#endif /* KSH_88D */
			while(1)
			{
				/* quoted null strings have to be marked */
				if(*v==0 && quote)
				{
					stak_push(ESCAPE);
					stak_push(0);
				}
#ifdef  KSH_88D
                                mac_copy (v);
#else
				while(c = *v++)
				{
#if (defined(NLS) || defined(KJI))
					if (NCisshift(c))
						c = (c<<8) | *v++;
#endif
					if(sh.staktop >= sh.brkend)
						sh_addmem(BRKINCR);
					if(quote || (!mflag&&addescape(c))
						 || (no_ifs&&isspace(c)))
				 		stak_push(ESCAPE); 
					if(sh.staktop >= sh.brkend)
						sh_addmem(BRKINCR);
			 		stak_push(c); 
				}
#endif /* KSH_88D */
				if(dolg==0)
					 break;
				if(dolg>0)
				{
					if(++dolg >= dolmax)
						break;
					v = st.dolv[dolg];
				}
				else
				{
					if(type == 0)
						break;
					v = nam_strval(n);
					type = array_next(n);
				}
				if(quote && *id=='*')
				{
#ifdef  KSH_88D
                                        if (*ifs == 0)
#else
                                        if(no_ifs)
#endif /* KSH_88D */
						continue;
					stak_push(ESCAPE);
				}
				stak_push(sep);
#undef sep
			}
		}
		else if(argp)
		{
			if(c=='?' && !is_option(NOEXEC))
			{
				sh_trim(argp);
				sh_fail(id,*argp?argp:MSGSTR(E_NULLSET, (char *)e_nullset)); /*MSG*/
			}
			else if(c=='=')
			{
				if(n)
				{
					bra= argp-sh.stakbot;
					sh_trim(argp);
					nam_putval(n,argp);
					v = nam_strval(n);
					sh.staktop = argp = stak_address(bra);
					nulflg = 0;
					goto retry2;
				}
				else
					mac_error();
			}
		}
		else if(is_option(NOSET))
			sh_fail(id,MSGSTR(E_NOTSET, (char *)e_notset)); /*MSG*/
		goto retry;
	}
	else if(c==endch)
		return(c);
	else if(c==SQUOTE && mactry==0)
	{
		comsubst(0);
		goto retry;
	}
	else if(c==DQUOTE && !mflag)
	{
		if(quote ==0)
		{
			atflag = 0;
			quoted++;
		}
		quote ^= 1;
		goto retry;
	}
	return(c);
nosub:
	if(bra)
		mac_error();
	io_unreadc(c);
	return(DOLLAR);
}

	/* Strip "" and do $ substitution
	 * Leaves result on top of stack
	 */
char *mac_expand(char *as)
{
	register int	savqu =quoted;
	register int	savq = quote;
	register int	savpeekn = st.peekn;
	struct fileblk	cb;
	mac_current = as;
	st.peekn = 0;
	io_push(&cb);
	io_sopen(as);
	stak_begin();
	mflag = 0;
	quote=0;
	quoted=0;
#ifdef  KSH_88D
        if(!(ifs = nam_fstrval(IFSNOD)))
                ifs = e_sptbnl;
#endif /* KSH_88D */
	copyto(0,0);
	io_pop(1);
	st.peekn = savpeekn;
	if(quoted && (sh.stakbot == sh.staktop))
	{
		stak_push(ESCAPE);
		stak_push(0);
	}
	/* above is the fix for *'.c' bug	*/
	quote=savq;
	quoted=savqu;
	return(stak_fix());
}

/*
 * command substitution
 * type==0 for ``
 * type==1 for $()
*/

static int comsubst(type)
int type;
{
	struct fileblk	cb;
	register int	fd;
#define d	fd
	register union anynode *t;
	register char *argc;
	struct ionod *saviotemp = st.iotemp;
	int savem = mflag;
	STKPTR savtop = sh.staktop;
	STKPTR savptr = stak_fix();
	char inbuff[IOBSIZE+1];
	int saveflags = (st.states&FIXFLG);
	register int waitflag = 0;
	stak_begin();
	if(type)
	{
		sh.staktop = (STKPTR)(match_paren((char*)sh.stakbot,LPAREN,RPAREN)-1);
		if(*sh.stakbot==LPAREN && *(sh.staktop-1)==RPAREN)
		{
			STKPTR savbot = sh.stakbot;
			argc = stak_end(sh.staktop-1);
			numb = sh_arith(mac_trim(argc+1,0));
			stak_reset(savtop);
			sh.stakbot = savptr;
			return(0);
		}
	}
	else
	{
		while((d=io_readc())!=SQUOTE && d)
		{
			if(d==ESCAPE)
			{
				d = io_readc();
				/*
				 * This is wrong but it preserves compatibility with
				 * the SVR2 shell
				 */
				if(!(escchar(d) || (d=='"' && quote)))
					stak_push(ESCAPE);
			}
			if(sh.staktop >= sh.brkend)
				sh_addmem(BRKINCR);
			stak_push(d);
		}
	}
	argc=stak_fix();
	st.states &= ~FIXFLG;		/* do not save command subs in fc file */
	if(w_fd>=0)
	{
		p_setout(w_fd);
		p_flush();	/* flush before executing command */
	}
	io_push(&cb);
	io_sopen(argc);
	sh.nested_sub = 0;
	st.exec_flag++;
	t = sh_parse(EOFSYM,MTFLG|NLFLG);
	st.exec_flag--;
	if(!t || is_option(NOEXEC))
		goto readit;
	if(!sh.nested_sub && !t->tre.treio && is_rbuiltin(t))
	{
		/* nested command subs not handled specially */
		/* handle command substitution of most builtins separately */
		/* exec, login, cd, ., eval and shift not handled this way */
		/* put output into tmpfile */
		int save1_out = st.standout;
		if((st.states&IS_TMP)==0)
		{
			char tmp_fname[TMPSIZ];
			/* create and keep open a /tmp file for command subs */
			fd = io_mktmp(tmp_fname);
			fd = io_renumber(fd,TMPIO);
			st.states |= IS_TMP;
			/* root cannot unlink because fsck could give bad ref count */
			if(sh.userid || !is_option(INTFLG))
				unlink(tmp_fname);
			else
				st.states |= RM_TMP;
		}
		else
			fd = TMPIO;
		st.standout = fd;
		/* this will only flush the buffer if output is fd already */
		p_setout(fd);
		p_char(0);
		st.subflag++;
		sh_funct(t,(char**)0,(int)(st.states&ERRFLG),(struct argnod*)0);
		st.subflag = 0;
		p_setout(fd);
		p_char(0);
		if(*_sobuf != 0)
		{
			/* file is larger than buffer, read from it */
			p_flush();
			io_seek(fd,(off_t)1,SEEK_SET);
			io_init(input=fd,st.standin,inbuff);
			waitflag = -1;
		}
		else
		{
			/* The file is all in the buffer */
			strcpy(inbuff,_sobuf+1);
			io_sopen(inbuff);
#ifdef  KSH_88D
                        io_ftable[fd]->ptr = io_ftable[fd]->base;
#endif /* KSH_88D */
		}
		st.standout = save1_out;
		goto readit;
	}
	else if(t->tre.tretyp==0 && t->com.comarg==0)
	{
#ifdef  KSH_88D
                if(t->tre.treio && (((t->tre.treio)->iofile)&IOUFD)==0)
#else
                if((((t->tre.treio)->iofile)&IOUFD) == 0)
#endif /* KSH_88D */
		{
			argc = (t->tre.treio)->ioname;
			if(!((t->tre.treio)->iofile&IORAW))
				argc=mac_trim(argc,1);
		}
		else
			argc = (char*)e_devnull; /*NOTX*/
		fd = io_fopen(argc);
	}
	else
	{
		int 	pv[2];
		int forkflag = FPOU|FCOMSUB;
		waitflag++;
		if(st.iotemp!=saviotemp)
			forkflag |= FTMP;
		t = sh_mkfork(forkflag,t);
		  /* this is done like this so that the pipe
		   * is open only when needed
		   */
		io_popen(pv);
		sh.inpipe = 0;
		sh.outpipe = pv;
		sh_exec(t, (int)(st.states&ERRFLG));
		fd = pv[INPIPE];
		io_fclose(pv[OTPIPE]);
	}
	io_init(input=fd,st.standin,inbuff);

readit:
	mflag = savem;
	stak_reset(savptr);
	d = savtop - savptr;
	while(d--)
		*sh.staktop++ = *savptr++;
#ifdef  KSH_88D
        mac_copy ((char *)0);
#else
	while(d=io_readc())
	{
		if(quote || (!mflag && addescape(d)))
			stak_push(ESCAPE);
		if(sh.staktop >= sh.brkend)
			sh_addmem(BRKINCR);
		stak_push(d);
	}
#endif /* KSH_88D */
	if(waitflag>0)
		job_wait(sh.subpid);
	while(sh.stakbot!=sh.staktop)
	{
		if(*--sh.staktop != NL)
		{
			++sh.staktop;
			break;
		}
		else if(quote)
			sh.staktop--;
	}
	io_pop(waitflag>=0?0:1);
	if(w_fd >=0)
		p_setout(w_fd);
	st.states |= saveflags;
	return(1);
#undef d
}


/*
 * Copy and expand a here-document
 */

int mac_here(struct ionod *iop)
{
	register int	c;
	register int 	in;
	register int	ot;
	struct fileblk 	fb;
	char inbuff[IOBSIZE+1];
	quote = 0;
#ifdef  KSH_88D
        ifs = e_nullstr;
#endif /* KSH_88D */
	mflag = 1;
	ot = io_mktmp(inbuff);
	unlink(inbuff);
	w_fd = ot;
	io_push(&fb);
	if(iop->iofile&IOSTRG)
	{
		io_sopen(iop->ioname);
		in = F_STRING;
	}
	else
	{
		in = io_fopen(iop->ioname);
		io_init(in,&fb,inbuff);
		}
	p_setout(ot);
	stak_begin();
	if(is_option(EXECPR))
		sh.heretrace=1;
	while(1)
	{
		c=getch(0);
		if(c==ESCAPE)
		{
			c = io_readc();
			if(!escchar(c))
				stak_push(ESCAPE);
		}
		if(sh.staktop!=sh.stakbot)
		{
			*sh.staktop = 0;
			p_str(sh.stakbot,(int)c);
			sh.staktop = sh.stakbot;
		}
		else if(c)
			p_char((int)c);
		if(c==0)
			break;
	}
	p_flush();
	sh.heretrace=0;
	mflag = 0;
	io_pop(0);
	w_fd = -1;
	io_ftable[ot] = 0;
	lseek(ot,(off_t)0,SEEK_SET);
	return(ot);
}

#ifdef  KSH_88D
/*
 * copy value of string or file onto the stack inserting backslashes
 * as needed to prevent word splitting and file expansion
 */

#ifdef KJI
static void mac_copy(unsigned char *str)
#else
static void mac_copy(char *str)
#endif
{
        register int c;
        while(c = (str?*str++:io_readc()))
        {
#if (defined(NLS) || defined(KJI))
                if (NCisshift(c))
                        c = (c<<8) | *str++;
#endif
                if(sh.staktop >= sh.brkend)
                        sh_addmem(BRKINCR);
                /*@ assert (!mflag&&quote)==0; @*/
                if(quote || (!mflag&&addescape(c)&&(c==ESCAPE||!strchr(ifs,c))))
                        stak_push(ESCAPE);
                stak_push(c);
        }
}
#endif /* KSH_88D */

 
/*
 * Computes the substring of STRING using the expression PAT
 * depending on which FLAG is set.
 */

static char *substring(string,pat,flag)
char *string;
char *pat;
int flag;
{
	register char *sp = string;
	register char *cp;
	switch(flag)
	{
		case '#':
		case MARK|'#':
		{
			register int c;
			cp = sp;
			do
			{
#if (defined(NLS) || defined(KJI))
				sp += NLchrlen(sp);
				c = *sp;
#endif /* NLS | KJI */
#ifdef MULTIBYTE
				c = *sp;
				c = echarset(c);
				sp += (in_csize(c)+(c>=2));
				c = *sp;
#endif /* MULTIBYTE */
#if !(defined(MULTIBYTE) || defined(NLS) || defined(KJI))
				c= *++sp;
#endif /* MULTIBYTE|NLS|KJI */
				*sp=0;
				if(strmatch(string,pat))
				{
					cp = sp;
					if(flag=='#')
						break;
				}
				*sp = c;
			}
			while(c);
			*sp = c;
			return(cp);
		}

		case '%':
		case MARK|'%':
		{
			sp += strlen(sp);
			cp = sp;
			while(sp>=string)
			{
				if(strmatch(sp,pat))
				{
					cp = sp;
					if(flag=='%')
						break;
				}
				sp--;
#ifdef MULTIBYTE
				if(*sp&HIGHBIT)
				{
					if(*(sp-in_csize(3))==ESS3)
						sp -= in_csize(3);
					else if(*(sp-in_csize(2))==ESS2)
						sp -= in_csize(2);
					else
						sp -= (in_csize(1)-1);
				}
#endif /* MULTIBYTE */
#if (defined(NLS) || defined(KJI))
				{
				register char *q, *p;
				q = p = string;
				/*
				 * backing up by full characters requires
				 * searching from beginning of string 
				 * each time
				 */
				while(1) {
					if ((p += NLchrlen(p)) <= sp)
						q = p;
					else
						break;
				}
				if (sp > q)
					sp = q;
				}
#endif /* NLS | KJI */
			}
			*cp = 0;
			return(string);
		}
	}
	return(sp);
}


/*
 * do parameter and command substitution and strip of quotes
 * attempt file name expansion if <type> not zero
 */

char *mac_trim(char *s, int type)
{
	register char *t=mac_expand(s);
	struct argnod *schain = st.gchain;
	if(type && f_complete(t,e_nullstr)==1) /*NOTX*/
		t = st.gchain->argval;
	st.gchain = schain;
	sh_trim(t);
	return(t);
}

/*
 * perform only parameter substitution and catch failures
 */

char *mac_try(char *s)
{
	if(s)
	{
		mactry++;
		if(setjmp(mac_buf)==0)
			s = mac_trim(s,0);
		else
			io_pop(1);
		mactry = 0;
	}
	if(s==0)
		return(NULLSTR);
	return(s);
}

static void mac_error()
{
	sh_fail(mac_current,MSGSTR(E_SUBST, (char *)e_subst)); /*MSG*/
}

/*
 * check to see if the error occured while expanding prompt
 */

void mac_check(void)
{
	if(mactry)
		longjmp(mac_buf,1);
}



#ifdef MULTIBYTE
static int	charlen(str)
register char *str;
{
	register int n = 0;
	register int c;
	while(*str)
	{
		c = echarset(*str);		/* find character set */
		str += (in_csize(c)+(c>=2));	/* move to next char */
		n += out_csize(c);		/* add character size */
	}
	return(n);
}
#endif /* MULTIBYTE */

#if (defined(NLS) || defined(KJI))
static int charlen(str)
char *str;
{
	register int n = 0;
	register char *sp = str;

	while (*sp) {
		sp += NLchrlen(sp);
		n++;
	}
	return(n);
}
#endif
