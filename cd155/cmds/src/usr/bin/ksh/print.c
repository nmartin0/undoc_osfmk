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
static char rcsid[] = "@(#)$RCSfile: print.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:27:26 $";
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
#include	"builtins.h"
#include	<limits.h>

/* This module references the following external */
extern void	nam_rjust();

/* printing and io conversion */

#define TIC_SEC	CLOCKS_PER_SEC	/* number of ticks per second */


/*
 *  flush the output queue and reset the output stream
 */

void	p_setout(int fd)
{
	register struct fileblk *fp;
	register int count;
	if(!(fp=io_ftable[fd]))
		fp = io_ftable[fd] = &io_stdout;
	else if(fp->flag&IOREAD)
	{
		if(count=fp->last-fp->ptr)
			lseek(fd,-((off_t)count),SEEK_CUR);
		fp->fseek = 0;
		fp->ptr = fp->base;
	}
	fp->last = fp->base + IOBSIZE;
	fp->flag &= ~(IOREAD|IOERR|IOEOF);
	fp->flag |= IOWRT;
	if(output==fd)
		return;
	if(fp = io_ftable[output])
		if(io_ftable[fd]==fp || (fp->flag&IOSLOW))
			p_flush();
	output = fd;
}

/*
 * flush the output if necessary and null terminate the buffer
 */

void p_flush(void)
{
	register struct fileblk *fp = io_ftable[output];
	register unsigned count;
	if(fp)
	{
		if(count=fp->ptr-fp->base)
		{
			if(write(output,fp->base,count) < 0)
				fp->flag |= IOERR;
			if(sh.heretrace)
				write(ERRIO,fp->base,count);
			fp->ptr = fp->base;
		}
		/* leave buffer as a null terminated string */
		*fp->ptr = 0;
	}
}

/*
 * print a given character
 */

void	p_char(int c)
{
	register struct fileblk *fp = io_ftable[output];
	if(fp->ptr >= fp->last)
		p_flush();
#if (defined(NLS) || defined(KJI))
	if (c&~STRIP)
		*fp->ptr++ = c>>8;
	*fp->ptr++ = c&STRIP;
#else
	*fp->ptr++ = c;
#endif
}

/*
 * print a string optionally followed by a character
 * The buffer is always terminated with a zero byte.
 */

void	p_str(const char *string, int c)
{
	register struct fileblk *fp = io_ftable[output];
	register int cc;
	while(1)
	{
		if((cc= *string)==0) {
#if (defined(NLS) || defined(KJI))
			if (c&~STRIP) {
				cc = c>>8;
				c = c&STRIP;
			}
			else
#endif
			cc = c,c = 0;
		}
		else
			string++;
		if(fp->ptr >= fp->last)
			p_flush();
		*fp->ptr = cc;
		if(cc==0)
			break;
		fp->ptr++;
	}
}

/*
 * print a given character a given number of times
 */

void	p_nchr(int c,int n)
{
	register struct fileblk *fp = io_ftable[output];
	while(n-- > 0)
	{
		if(fp->ptr >= fp->last)
			p_flush();
#if (defined(NLS) || defined(KJI))
		if (c&~STRIP)
			*fp->ptr++ = c>>8;
		*fp->ptr++ = c&STRIP;
#else
		*fp->ptr++ = c;
#endif
	}
}
/*
 * print a message preceded by the command name
 */

void p_prp(const char *s1)
{
	register unsigned char *cp;
#if (defined(NLS) || defined(KJI))
	wchar_t c;
	int cl;		/* character length in bytes */
#else
	wchar_t c;
#endif	/* NLS|KJI */
	if(cp=(unsigned char *)st.cmdadr)
	{
		if(*cp=='-')
			cp++;
		c = ((st.cmdline>1)?0:':');
		p_str((char*)cp,c);
		if(c==0)
			p_sub(st.cmdline,':');
		p_char(SP);
	}
	if(cp = (unsigned char*)s1)
	{
#if (defined(NLS) || defined(KJI))
		for(; cl=NCdec(cp, &c), c > (wchar_t)0; cp += cl)
		{
			if(!NCisprint(c))
#else
		for(;c= *cp;cp++)
		{
			if(!isprint(c))
#endif	/* NLS|KJI */
			{
#if (defined(NLS) || defined(KJI))
				/* only makes sense with ascii */
#endif	/* NLS|KJI */
				p_char('^');
				c ^= TO_PRINT;
			}
#ifdef NLS
			/* at this point, c is a NLchar */
			if (cl > 1) {
				p_char((int)cp[0]);
				p_char((int)cp[1]);
			} else
				p_char(c);
#else
			p_char(c);
#endif /* NLS */
		}
	}
}

/*
 * print a time and a separator 
 */

void	p_time(clock_t t, int c)
{
	register int  min, sec, frac;
	register int hr;
	frac = t%TIC_SEC;
	frac = (frac*100)/TIC_SEC;
	t /= TIC_SEC;
	sec=t%60; t /= 60;
	min=t%60;
	if(hr=t/60)
	{
		p_num(hr,'h');
	}
	p_num(min,'m');
	p_num(sec,'.');
	if(frac<10)
		p_char('0');
	p_num(frac,'s');
	p_char(c);
}

/*
 * print a number optionally followed by a character
 */

void	p_num(int n, int c)
{
	p_str(sh_itos(n),c);
}


/* 
 * print a list of arguments in columns
 */
#define NROW	15	/* number of rows in output before going to multi-columns */
#define LBLSIZ	3	/* size of label field and interfield spacing */

void	p_list(int argn, const char *com[])
{
	register int i,j;
	register const char **arg;
	char a1[12];
	int nrow;
	int ncol = 1;
	int ndigits = 1;
	int fldsize;
#if ESH || VSH
	int wsize = ed_window();
#else
	int wsize = 80;
#endif
	char *cp = nam_fstrval(LINES);
	nrow = (cp?1+2*(atoi(cp)/3):NROW);
	for(i=argn;i >= 10;i /= 10)
		ndigits++;
	if(argn < nrow)
	{
		nrow = argn;
		goto skip;
	}
	i = 0;
	for(arg=com; *arg;arg++)
	{
#if (defined(NLS) || defined(KJI))
		i = max(i, NLstrdlen(*arg));	/* use display length */
#else
		i = max(i, strlen(*arg));
#endif /* NLS|KJI */
	}
	i += (ndigits+LBLSIZ);
	if(i < wsize)
		ncol = wsize/i;
	if(argn > nrow*ncol)
	{
		nrow = 1 + (argn-1)/ncol;
	}
	else
	{
		ncol = 1 + (argn-1)/nrow;
		nrow = 1 + (argn-1)/ncol;
	}
skip:
	fldsize = (wsize/ncol)-(ndigits+LBLSIZ);
	for(i=0;i<nrow;i++)
	{
		j = i;
		while(1)
		{
			arg = com+j;
			strcpy(a1,sh_itos(j+1));
			nam_rjust(a1,ndigits,' ');
			p_str(a1,')');
			p_char(SP);
			p_str(*arg,0);
			j += nrow;
			if(j >= argn)
				break;
			p_nchr(SP,fldsize-strlen(*arg));
		}
		newline();
	}
}

/*
 * Print a number enclosed in [] followed by a character
 */

void	p_sub(int n, int c)
{
	p_char('[');
	p_num(n,']');
	if(c)
		p_char(c);
}

#ifdef  KSH_88D
#ifdef POSIX
/*
 * print <str> qouting chars so that it can be read by the shell
 * terminate with the character <cc>
 */
void    p_qstr(str,cc)
char *str;
{
        register char *cp = str;
        register int c = *cp;
        register int state = (c==0);
        do
        {
                if(isalpha(c))
                {
                        while((c = *++cp),isalnum(c));
                        if(c=='=')
                        {
                                *cp = 0;
                                p_str(str,c);
                                *cp++ = c;
                                str = cp;
                                c = *cp;
                        }
                }
                if(c=='~')
                        state++;
                while((c = *cp++) && (c!= '\''))
                        state |= _ctype1[c];
                if(c || state)
                {
                        /* needs single quotes */
                        p_char('\'');
                        if(c)
                        {
                                /* string contains single quote */
                                c = *cp;
                                *cp = 0;
                                state = '\\';
                        }
                        else
                                state = '\'';
                        p_str(str,state);
                        if(c)
                                *cp = c;
                        str = (cp-1);
                }
        }
        while(c);
        p_str(str,cc);
}
#endif /* POSIX */
#endif /* KSH_88D */

