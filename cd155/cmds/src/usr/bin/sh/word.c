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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.2
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: word.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 01:39:29 $";
#endif
/*
 * COMPONENT_NAME: (CMDSH) Bourne shell and related commands
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

#include	"defs.h"
#include	"sym.h"
#include        <fcntl.h>
#ifdef KJI
#include	<NLchar.h>
#endif

static	unsigned int	readb ();

/* ========	char handling for command lines	========*/


word()
{
	register uchar_t	c, d;
	struct argnod	*arg = (struct argnod *)locstak();
	register uchar_t	*argp = arg->argval;
	needmem(argp);
	*argp++ = FNLS;   /* mark word as NLS-encoded */

	wdset = 0;
	wdnum = 0;

	while (1)
	{
#ifdef KJI
		c = skipc();
#else
		while (c = nextc(0), space(c))		/* skipc() */
			;
#endif

		if (c == COMCHAR)
		{
			while ((c = readc()) != NL && c != EOF);
			peekc = c;
		}
		else
		{
			break;	/* out of comment - white space loop */
		}
	}
	if (!eofmeta(c))
	{
		do
		{
#ifdef KJI
			if (NLSfontshift(c)) {
				register unsigned int cc;
				cc = readwc((unsigned int)c);
				unreadwc(cc);
				if (isjspace(cc)) 
					break;
			}
#endif
			if (c == LITERAL)
			{
				needmem(argp);
				*argp++ = (DQUOTE);
				while ((c = readc()) && c != LITERAL)
				{
					needmem(argp);
					*argp++ = (c | QUOTE);
					if (c == NL)
						chkpr();
				}
				needmem(argp);
				*argp++ = (DQUOTE);
			}
			else
			{
				needmem(argp);
				*argp++ = (c);
				if (qotchar(c))
				{
					d = c;
					while(1) {
						needmem(argp);

						if(!(*argp++ = c = nextc(d))
						 || c == d)
							break;
						if (c == NL)
							chkpr();
					}
				}
			}
		} while ((c = nextc(0), !eofmeta(c)));
		argp = endstak(argp);
		wdset = (scanset(arg->argval)!=0);
		peekn = c | MARK;
		/* arg->argval[0] is FNLS */
		if (arg->argval[2] == 0 &&
		    (d = arg->argval[1], digit(d)) &&
		    (c == '>' || c == '<'))
		{
			word();
			wdnum = d - '0';
		}
		else		/*check for reserved words*/
		{
			if (reserv == FALSE || (wdval = syslook(arg->argval,reserved, no_reserved)) == 0)
			{
				wdarg = arg;
				wdval = 0;
			}
		}
	}
	else if (dipchar(c))
	{
		if ((d = nextc(0)) == c)
		{
			wdval = c | SYMREP;
			if (c == '<')
			{
				if ((d = nextc(0)) == '-')
					wdnum |= IOSTRIP;
				else
					peekn = d | MARK;
			}
		}
		else
		{
			peekn = d | MARK;
			wdval = c;
		}
	}
	else
	{
		if ((wdval = c) == EOF)
			wdval = EOFSYM;
		if (iopend && eolchar(c))
		{
			copy(iopend);
			iopend = 0;
		}
	}
	reserv = FALSE;
#ifdef NLSDEBUG
{  uchar_t buf[50];
   sprintf(buf,"word: %d ",wdval);
   debug(buf,wdarg->argval);
}
#endif
	return(wdval);
}

skipc()
{
	register uchar_t c;

#ifdef KJI
	register unsigned int  cc;
	/*  Check for double-wide spaces also. */
	while (c = nextc(0)) {
		if (space(c)) continue;
		if (NLSfontshift(c)) {
			cc = readwc((int)c);
			if (isjspace(cc)) 
				continue;
			else
				unreadwc(cc);
		} 
		break;
	}
#else
	while (c = nextc(0), space(c))
		;
#endif
	return(c);
}

uchar_t	readvarQuoted ;		/**	from apar 4138 of 5A code	**/

nextc(quote)
uchar_t	quote;
{
	register uchar_t	c, d;
#ifdef NLS
	/* If you read an ESCAPE (backslash) char, under most */
	/* circumstances the next one gets quoted.  This may be    */
	/* a font-shift, but it will never be the char after  */
	/* the font shift.  Everything should work right here...   */

	/* The ESCAPE is sometimes thrown away, but when it isn't  */
	/* the peek'd char is MARKed so it will be handled    */
	/* right by readc() later.  MARK is a kind of quote not in */
	/* the 8 bits, which causes char to compare unequal   */
	/* and which is automtically stripped when the char   */
	/* is stored back later.  Yuch!                            */
#endif

retry:
	if ((d = readc()) == ESCAPE)
	{
		if ((c = readc()) == NL)
		{
			chkpr();
			goto retry;
		}
		else if (quote && c != quote && !escchar(c))
			peekc = c | MARK;
		else
		{
			d = c | QUOTE;
				/**
				*	Here is some more code to go Yuch about.
				*	apar 4138
				**/
			if ( c != d )
				readvarQuoted = 1 ;
		}
	}
	return(d);
}

unsigned int fshift;
static unsigned int peekfshift = 0;
static unsigned int peekchar = 0;

readc()
{
	register uchar_t	c;
	register unsigned int	len;
	register struct fileblk *f;

	if (peekn)
	{
		peekc = peekn;
		peekn = 0;
	 }
	if (peekc)
	{
		c = peekc;
		peekc = 0;
		return(c);
	}
	if (peekfshift) {
		c = peekfshift;
		peekfshift = 0;
		return(c);
	}
#ifdef KJI
	if (peekchar) {
		c = peekchar;
		peekchar = 0;
		return(c);
	}
#endif
	f = standin;
retry:
	if (f->fnxt != f->fend)
	{
		if ((c = *f->fnxt++) == 0)
		{
			if (f->feval)
			{
				/* temp hack till estabf takes raw as arg */
				int i = f->fraw;
				NLSskiphdr(*f->feval);
				if (estabf(*f->feval++))
					c = EOF;
				else
					c = SP;
				f->fraw = i;
			}
			else
				goto retry;	/* = c = readc(); */
		}
		if (flags & readpr && standin->fstak == 0)
			prc(c);
		if (c == NL)
			f->flin++;
	}
	else if (f->sh_feof || f->fdes < 0)
	{
		c = EOF;
		f->sh_feof++;
	}
	else if ((int)(len = readb()) <= 0)
	{
		close(f->fdes);
		f->fdes = -1;
		c = EOF;
		f->sh_feof++;
	}
	else
	{
		f->fend = (f->fnxt = f->fbuf) + len;
		goto retry;
	}
#ifdef NLS
	if (!f->fraw) {
		if (fshift)
			fshift = 0;
		else if (NLSneedesc(c)) {
			/* save it but output FSH0 first */
			peekfshift = c;
			c = FSH0;
		}
		else
			fshift = fontshift(c);
	}
#endif
#ifdef KJI
	if (!f->fraw) {
		if (fshift) {
			peekchar = c;
			c = (peekchar & 0x80) ? FSH21 : 
				(peekchar |= QUOTE, FSH20);
			fshift = 0;
		} else if (NLSneedesc(c)) {
			if (NCisshift (c)) {
				fshift++;
				peekfshift = c;
				goto retry;
			} else {
				peekchar = c;
				c = FSH0;
			}
		}
	}
#endif
	return(c);
}

#ifdef KJI

/*	Read a char from a stream which has been previously encoded.
 *	The first byte has already been read and is passed in as "ch".
 *	If the char is ASCII, it will consist of a single byte.
 *	Otherwise, it will consist of a magic font shift followed by
 *	1 or 2 bytes; convert these to an NLchar char.  Return 
 *	this char as the function value and return in "ch" the first
 *	byte read (this will be the char itself for an ASCII char
 *	and the magic font shift for a non-ASCII char).
 */
unsigned int
readwc (ch)
register unsigned int ch;
{
    register unsigned int c;
    register unsigned int peekf;

    if (NLSfontshift (ch))  {
	peekf = readc();
	if (ch == FSH20) 
	    c = readc() & STRIP;
	if (ch == FSH21) 
	    c = readc();
	return (NCisshift (peekf) ? _NCd2 (peekf, c) : peekf);
    }
    return (ch);
}

/*
 *	Put back the 1- or 2-byte char which we "peeked."
 *	If this is a 1-byte char, peekfshift will be zero and
 *	readc() will return peekchar on the next call.
*/
unreadwc (c)
register NLchar c;
{
    peekfshift = _NCtop (c);
    peekchar = _NCbot (c) | QUOTE;
}
#endif /* KJI */

static unsigned int
readb()
{
	register struct fileblk *f = standin;
	register int	len;
		 int    flags;

	do
	{
		if (trapnote & SIGSET)
		{
			newline();
			sigchk();
		}
		else if ((trapnote & TRAPSET) && (rwait > 0))
		{
			newline();
			chktrap();
			clearup();
		}
	mailalarm = 0;
	} while ((len = read(f->fdes, f->fbuf, f->fsiz)) < 0
	 && (trapnote || mailalarm));

	/* if O_NDELAY is on, 0 bytes read is not an EOF. */
	/* instead, retry the read with O_NDELAY off.     */

	if ((len == 0) && ((flags = fcntl(f->fdes, F_GETFL, 0)) & O_NDELAY))
	{
		fcntl(f->fdes, F_SETFL, flags & ~O_NDELAY);
		return(readb());
	}
	else    return(len);
}


