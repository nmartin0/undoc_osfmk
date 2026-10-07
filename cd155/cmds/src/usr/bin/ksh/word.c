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
static char rcsid[] = "@(#)$RCSfile: word.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:27:39 $";
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
 * UNIX shell
 *
 * S. R. Bourne
 * Rewritten by David Korn
 * AT&T Bell Laboratories
 *
 */

#include	"defs.h"
#include	"sym.h"
#include	"builtins.h"
#ifdef	NEWTEST
#   include	"test.h"
#endif	/* NEWTEST */



static struct argnod *letflg = 0;
static void setupalias();
static int here_copy();
static int here_tmp();
static int qtrim();

/* This module defines the following routines */
char	*match_paren();
int	sh_lex(void);

#if (defined(NLS) || defined(KJI))
#define alph(c)		NLSletter(c)
#define alphnum(c) 	NLSalphanum(c)
#else
#define alph(c)		isalpha(c)
#define alphnum(c) 	isalnum(c)
#endif /* NLS | KJI */

/* This module references these external routines */
extern char	*sh_tilde();

/* ========	character handling for command lines	========*/

/*
 * Get the next word and put it on the top of the stak
 * Determine the type of word and set sh.wdnum and sh.wdset accordingly
 * Returns the token type
 */

sh_lex(void)
{
	register int c;
	register int d;
	register char *argp;
	register char *tildp;
	char chk_keywd;
	int 	alpha = 0;
	sh.wdnum=0;
	sh.wdval = 0;
	/* condition needed to check for keywords, name=value */
	chk_keywd = (sh.reserv!=0 && !(sh.wdset&IN_CASE)) || (sh.wdset&KEYFLG);
	sh.wdset &= ~KEYFLG;
	if(letflg)
	{
		/* return argument inside ((...)) */
		sh.wdarg = letflg;
		letflg = 0;
		return(0);
	}
	sh.wdarg = (struct argnod*)stak_begin();
#ifdef  KSH_88D
        sh.wdarg->argnxt.ap = 0;
#endif /* KSH_88D */
	argp = sh.wdarg->argval;
	tildp = NULL;
	while(1)
	{
		while((c=io_nextc(), isblank(c)));
		if(c==COMCHAR)
		{
			while((c=io_readc()) != NL && c != ENDOF);
			io_unreadc(c);
		}
		else	 /* out of comment - white space loop */
			break;
	}
	if(c=='~')
		tildp = argp;
	if(!ismeta(c))
	{
		do
		{
			if(c==LITERAL)
			{
				argp = match_paren(argp,c,c);
				alpha = -1;
			}
			else
			{
				if(argp==sh.wdarg->argval&&chk_keywd&&alph(c))
				{
					alpha++;
				}
				if(argp >= (char*)sh.brkend)
					sh_addmem(BRKINCR);
#if (defined(NLS) || defined(KJI))
				if (c&~STRIP)
					*argp++ = c>>8;
				*argp++=(c&STRIP);
				if(c == ESCAPE)
				{
					int d;
					d = io_readc();
					if (d&~STRIP)
						*argp++ = d>>8;
					*argp++ = (d&STRIP);
				}
#else
				*argp++=(c);
				if(c == ESCAPE)
					*argp++ = io_readc();
#endif	/* NLS|KJI */
				if(alpha>0)
				{
					if(c == '[')
					{
						argp = match_paren(argp,'[',']');
					}
					else if(c=='=')
					{
						sh.wdset |= KEYFLG;
						tildp = argp;
						alpha = 0;
					}
					else if(!alphnum(c))
						alpha = 0;
				}
				if(qotchar(c))
				{
					argp = match_paren(argp,c,c);
				}
			}
			d = c;
			c = io_nextc();
			if(d==DOLLAR && c==LBRACE)
			{
				*argp++ = c;
				argp = match_paren(argp, LBRACE, RBRACE);
				c = io_nextc();
			}
			else if(c==LPAREN && patchar(d))
			{
				if(d==DOLLAR)
					sh.nested_sub++;
				*argp++ = c;
				argp = match_paren(argp, LPAREN, RPAREN);
				c = io_nextc();
			}
			else if(tildp!=NULL &&  (c == '/'  || c==':' || ismeta(c)))
			{
				/* check for tilde expansion */
				register char *dir;
				*argp = 0;
				sh.staktop = argp;
				dir=sh_tilde(tildp);
				/* This check needed if sh_tilde() uses malloc() */
#ifndef INT16
				if(sh.stakbot != (STKPTR)sh.wdarg)
				{
					tildp += ((char*)sh.staktop-argp);
					argp = sh.staktop;
					sh.wdarg = (struct argnod*)sh.stakbot;
				}
#endif /*INT16 */
				if(dir)
				{
					argp=tildp;
					argp = sh_copy(dir,argp);
				}
				else
					tildp = NULL;
			}
			/* tilde substitution after : in variable assignment */
			/* left in as unadvertised compatibility feature */
			if(c==':' && (sh.wdset&KEYFLG))
				tildp = argp+1;
		}
		while(!ismeta(c));
		argp=stak_end(argp);
		io_unreadc(c);
#ifdef	NEWTEST
		if(sh.wdset&IN_TEST)
		{
			tildp = ((struct argnod*)argp)->argval;
			if(sh.wdset&TEST_OP1)
			{
				if(tildp[0]=='-' && tildp[2]==0 &&
					strchr(test_unops,(int)tildp[1])) /*NOTX*/
				{
					sh.wdnum = tildp[1];
					sh.wdval = TESTUNOP;
				}
				else if(tildp[0]=='!' && tildp[1]==0)
				{
					sh.wdval = '!';
				}
				else
					sh.wdval = 0;
				sh.wdset &= ~TEST_OP1;
				return(sh.wdval);
			}
			c = sh_lookup(tildp, test_optable);
			switch(c)
			{
			case TEST_END:
				return(sh.wdval=ETSTSYM);

			default:
				if(sh.wdset&TEST_OP2)
				{
					sh.wdset &= ~TEST_OP2;
					sh.wdnum = c;
					return(sh.wdval=TESTBINOP);	
				}

			case TEST_OR: case TEST_AND:
			case 0:
				return(sh.wdval = 0);
			}
		}
#endif	/*NEWTEST */
		if(((struct argnod*)argp)->argval[1]==0 &&
			(d=((struct argnod*)argp)->argval[0],isdigit(d)) &&
			(c=='>' || c=='<'))
		{
			sh_lex();
#ifdef  KSH_88D
                        sh.wdnum |= (d-'0');
#else
                        sh.wdnum=d-'0';
#endif /* KSH_88D */
		}
		else
		{
			/*check for reserved words and aliases */
			sh.wdval = (sh.reserv!=0?sh_lookup(((struct argnod*)argp)->argval,tab_reserved):0);
			/* for unity database software, allow select to be aliased */
			if((sh.reserv!=0 && (sh.wdval==0||sh.wdval==SELSYM)) || (sh.wdset&CAN_ALIAS))
			{
				/* check for aliases */
				struct namnod* np;
				argp = ((struct argnod*)argp)->argval;
				if((sh.wdset&(IN_CASE|KEYFLG))==0 &&
					(np=nam_search(argp,sh.alias_tree,N_NOSCOPE))
					&& !nam_istype(np,M_FLAG)
					&& (argp=nam_strval(np)))
				{
					setupalias(argp,np);
					st.peekn = 0;
					nam_ontype(np,M_FLAG);
					sh.wdset |= KEYFLG;
					return(sh_lex());
				}
			}
		}
	}
	else if(dipchar(c))
	{
		sh.wdval = c;
#ifdef  KSH_88D
                d = io_nextc();
                if(d==c)
#else
                if((d=io_nextc())==c)
#endif /* KSH_88D */
		{
			sh.wdval = c|SYMREP;
			if(c=='<')
			{
				if((d=io_nextc())=='-')
					sh.wdnum |= IOSTRIP;
				else
					io_unreadc(d);
			}
			/* arithmetic evaluation ((expr)) */
			else if(c == LPAREN && sh.reserv != 0)
			{
				*argp++ =(DQUOTE);
				argp = match_paren(argp, LPAREN, RPAREN);
				*(argp-1)=(DQUOTE);
				c = io_nextc();
				if(c != ')')
				{
					/*
					 * process as nested () command
					 * for backward compatibility
					 */
					*argp++ = ')';
#if (defined(NLS) || defined(KJI))
					if (c&~STRIP)
						*argp++ = c>>8;
#endif
					*argp++ = c&STRIP;
					stak_end(argp);
					qtrim(argp = sh.wdarg->argval);
					setupalias(argp,(struct namnod*)0);
					sh.wdval = st.peekn = '(';
				}
				else
				{
#ifdef  KSH_88D
                                        stak_end(argp);
#else
                                        stak_end(argp-1);
#endif /* KSH_88D */
					letflg = sh.wdarg;
					sh.wdarg = (struct argnod*)stak_begin();
					stak_end(sh_copy(e_let,sh.wdarg->argval)); /*NOTX*/
					sh.wdval = 0;
				}
			}
		}
		else if(c=='|')
		{
			if(d=='&')
				sh.wdval = COOPSYM;
			else
				io_unreadc(d);
		}
#ifdef DEVFD
		else if(d==LPAREN && iochar(c))
			sh.wdval = (c=='>'?OPROC:IPROC);
#endif	/* DEVFD */
#ifdef  KSH_88D
                else if(c==';' && d=='&')
                        sh.wdval = ECASYM;
#endif /* KSH_88D */
		else
			io_unreadc(d);
	}
	else
	{
		if((sh.wdval=c)==ENDOF)
		{
			sh.wdval=EOFSYM;
			if(st.standin->ftype==F_ISALIAS)
				io_pop(1);
		}
		if(st.iopend && eolchar(c))
		{
			if(sh.owdval || is_option(NOEXEC))
				c = getlineno(1);
			if(here_copy(st.iopend)<=0 && sh.owdval)
			{
				sh.owdval = ('<'|SYMREP);
				sh.wdval = EOFSYM;
				sh.olineno = c;
				sh_syntax();
			}
			st.iopend=0;
		}
	}
	sh.reserv=0;
	return(sh.wdval);
}

static void setupalias(string,np)
char *string;
struct namnod *np;
{
	register struct fileblk *f;
	register int line;
	f = new_of(struct fileblk,0);
	line = st.standin->flin;
	io_push(f);
	io_sopen(string);
	f->flin = line-1;
	f->ftype = F_ISALIAS;
	f->feval = (char**)np;
	f->flast = st.peekn;
}

/*
 * read until matching <close>
 */

char *match_paren(argp,open,close)
register char *argp;
register int open;
{
	register int c;
	register int count = 1;
	register int quoted = 0;
	int was_dollar=0;
#ifndef KSH_88D
	char *oldargp = argp;
#endif /* !KSH_88D */
	int line = st.standin->flin;
#ifdef  KSH_88D
        if(open==LITERAL)
                *argp++ = '"';
#endif /* KSH_88D */
	while(count)
	{
		/* check for unmatched <open> */
		if(quoted || open==LITERAL)
			c = io_readc();
		else
			c = io_nextc();
		if(c==0)
		{
			/* eof before matching quote */
			/* This keeps old shell scripts running */
			if(filenum(st.standin)!=F_STRING || is_option(NOEXEC))
			{
				sh.olineno = line;
				sh.owdval = open;
				sh.wdval = EOFSYM;
				sh_syntax();
			}	
			io_unreadc(0);
			c = close;
		}
		if(c == NL)
		{
			if(open=='[')
			{
				io_unreadc(c);
				break;
			}
			sh_prompt(0);
		}
		else if(c == close)
		{
			if(!quoted)
				count--;
		}
		else if(c == open && !quoted)
			count++;
#ifdef  KSH_88D
                if(open==LITERAL && (escchar(c) || c=='"'))
#else
                if(open==LITERAL)
#endif /* KSH_88D */
			/* preceed each character with a '\' */
			*argp++ = ESCAPE;
		if(argp >= (char*)sh.brkend)
			sh_addmem(BRKINCR);
#if (defined(NLS) || defined(KJI))
		if (c&~STRIP)
			*argp++ = c>>8;
#endif
		*argp++ = c&STRIP;
		if(open==LITERAL)
			continue;
		if(!quoted)
		{
#ifdef  KSH_88D
                        if(open==LBRACE && (c=='<'||c=='>') )
#else
			if(open==LBRACE && dipchar(c) && c!=LPAREN && c!=RPAREN)
#endif /* KSH_88D */
			{
				/* reserved for future use */
				sh.wdval = c;
				sh_syntax();
			}
			/* check for nested '', "", and `` within $() */
			if(open!=close) 
			{
				if(c==LITERAL)
					argp--;
				else if(!qotchar(c))
					goto skip;
				argp = match_paren(argp,c,c);
			}
			/* check for $() within (), '', "", and `` */
			else if(was_dollar && c==LPAREN)
			{
				if(open==c)
					count--;
				argp = match_paren(argp,LPAREN,RPAREN);
			}
		skip:
			was_dollar = (c==DOLLAR);
		}
		if(c == ESCAPE)
			quoted = 1 - quoted;
		else
			quoted = 0;
	}
	if(open==LITERAL)
#ifdef  KSH_88D
                argp[-1] = '"';
#else
	{
		argp -= 2;
		if(argp==oldargp)
		{
			/* handle null argument specially */
			*argp++ = '"';
			*argp++ = '"';
		}
	}
#endif /* KSH_88D */
	return(argp);
}

/*
 * read in here-document from script
 * small non-quoted here-documents are stored as strings 
 * quoted here documents, and here-documents without special chars are
 * treated like file redirection
 */

static int here_copy(ioparg)
struct ionod	*ioparg;
{
	register int	c;
	register char	*bufp;
	register struct ionod *iop;
	register char	*dp;
	int		fd = -1;
	int		match;
	int		savec = 0;
	int		special = 0;
	int		nosubst;
#ifndef KSH_88D
	char		*delim;
#endif /* !KSH_88D */
	char		obuff[IOBSIZE+1];
	if(iop=ioparg)
	{
		int stripflg = iop->iofile&IOSTRIP;
		register int nlflg;
		here_copy(iop->iolst);
#ifdef  KSH_88D
                iop->iodelim=iop->ioname;
                /* check for and strip quoted characters in ends */
                nosubst = qtrim(iop->iodelim);
                if(stripflg)
                        while(*iop->iodelim=='\t')
                                iop->iodelim++;
                dp = iop->iodelim;
#else
		delim=iop->ioname;
		/* check for and strip quoted characters in ends */
                nosubst = qtrim(delim);
		if(stripflg)
			while(*delim=='\t')
				delim++;
		dp = delim;
#endif /* KSH_88D */
		match = 0;
		nlflg = stripflg;
		bufp = obuff;
		sh_prompt(0);	
		do
		{
			if(nosubst || savec==ESCAPE)
				c = io_readc();
			else
				c = io_nextc();
#ifndef KSH_88D
			special |= escchar(c);
#endif /* KSH_88D */
			if((savec = c)<=0)
				c = '\n';
#ifdef  KSH_88D
                        else
                                special |= escchar(c);
#endif /* KSH_88D */
			if(c=='\n')
			{
#ifdef  KSH_88D
#if (defined(NLS) || defined(KJI))
				if(match>0 && NCdechr(dp)==0)
#else
                                if(match>0 && iop->iodelim[match]==0)
#endif
#else
#if (defined(NLS) || defined(KJI))
                                if(match>0 && NCdechr(dp)==0)
#else
				if(match>0 && delim[match]==0)
#endif
#endif /* KSH_88D */
				{
					savec =1;
					break;
				}
				if(match>0)
					goto trymatch;
						/*	PTM 16114
						*	moved sh_prompt after
						*	test for match
						*/
				sh_prompt(0);	
				nlflg = stripflg;
				match = 0;
				goto copy;
			}
			else if(c=='\t' && nlflg)
				continue;
			nlflg = 0;
			/* try matching delimiter when match>=0 */
			if(match>=0)
			{
#if (defined(NLS) || defined(KJI))
				int d, len;
			trymatch:

				if((len=NLchrlen(dp)) > 1)
					d = (*dp<<8) | *(dp+1);
				else
					d = *dp;
				if(d == c)
				{
					match++;
					dp += len;
					continue;
				}
				else if(--match>=0)
				{
					io_unreadc(c);
#ifdef  KSH_88D
                                        dp = iop->iodelim;
#else
                                        dp = delim;
#endif /* KSH_88D */
					c = *dp++;
					if (NCisshift(c))
						c = (c<<8) | *dp++;
				}
			}
		copy:
			do
			{
				if(c&~STRIP)
					*bufp++ = c>>8;
				*bufp++ = c&STRIP;
				if(bufp >= &obuff[IOBSIZE])
				{
					if(fd < 0)
						fd = here_tmp(iop);
					write(fd,bufp=obuff,(unsigned)IOBSIZE);
				}

					/*	PTM 16114
					*	reset dp = delim if only a
					*	partial match of eof is found
					*/
				if (c == '\n' || --match < 0) {
#ifdef  KSH_88D
                                        dp = iop->iodelim;
#else
                                        dp = delim;
#endif /* KSH_88D */
					break;
				}
				c = *dp++;
				if (NCisshift(c))
					c = (c<<8) | *dp++;
			} while(TRUE);

#else
#ifdef  KSH_88D
                        trymatch:
                                if(iop->iodelim[match]==c)
			{
                                        match++;
                                        continue;
                                }
                                else if(--match>=0)
                                {
                                        io_unreadc(c);
                                        dp = iop->iodelim;
                                        c = *dp++;
                                }
                        }
                copy:
                        do
                        {
                                *bufp++ = c;
                                if(bufp >= &obuff[IOBSIZE])
                                {
                                        if(fd < 0)
                                                fd = here_tmp(iop);
                                        write(fd,bufp=obuff,(unsigned)IOBSIZE);
                                }
                        }
                        while(c!='\n' && --match>=0 && (c= *dp++));
#else
			trymatch:

				if(delim[match]==c)
				{
					match++;
					continue;
				}
				else if(--match>=0)
				{
					io_unreadc(c);
					dp = delim;
					c = *dp++;
				}
			}
		copy:
			do
			{
				*bufp++ = c;
				if(bufp >= &obuff[IOBSIZE])
				{
					if(fd < 0)
						fd = here_tmp(iop);
					write(fd,bufp=obuff,(unsigned)IOBSIZE);
				}
			} while(c!='\n' && --match>=0 && (c= *dp++)); 
#endif /* KSH_88D */
#endif
		}
		while(savec>0);
		if(c = (nosubst|!special))
                        iop->iofile &= ~IODOC;
		if(fd < 0)
		{
	                if(c)
				fd = here_tmp(iop);
			else
			{
	                        iop->iofile |= IOSTRG;
				*bufp = 0;
#ifdef  KSH_88D
                                iop->ioname = stak_copy(obuff);
#else
                                iop->ioname = sh_heap(obuff);
#endif /* KSH_88D */
				return(savec);
			}
		}
		if(bufp > obuff)
			write(fd, obuff, (unsigned)(bufp-obuff));
		close(fd);
	}
	return(savec);
}

/*
 * create a temporary file for a here document
 */

static int here_tmp(iop)
register struct ionod *iop;
{
	register int fd = io_mktmp((char*)0);
	iop->ioname = stak_copy(io_tmpname);
	iop->iolst=st.iotemp;
	st.iotemp=iop;
	return(fd);
}


/*
 * trim quotes and the escapes
 * returns non-zero if string is quoted 0 otherwise
 */

#ifdef KJI

static int qtrim(string)
char *string;
{
	register char *sp = string;
	register char *dp = sp;
	wchar_t c;
	register int quote = 0;
	while(sp += NCdec(sp,&c), c > (wchar_t)0)
	{
		if(c == ESCAPE)
		{
			quote = 1;
			sp += NCdec(sp,&c);
		}
		else if(c == '"')
		{
			quote = 1;
			continue;
		}
		dp += NCenc(&c,dp);
	}
	*dp = 0;
	return(quote);
}

#else

static int qtrim(string)
char *string;
{
	register char *sp = string;
	register char *dp = sp;
	register int c;
	register int quote = 0;
	while(c= *sp++)
	{
		if(c == ESCAPE)
		{
			quote = 1;
			c = *sp++;
		}
		else if(c == '"')
		{
			quote = 1;
			continue;
		}
		*dp++ = c;
	}
	*dp = 0;
	return(quote);
}
#endif  /* KJI */
