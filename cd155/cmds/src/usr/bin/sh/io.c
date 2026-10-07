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
 * OSF/1 Release 1.0.3
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: io.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:39:11 $";
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
#include	"dup.h"
#include	<fcntl.h>

short topfd;

/* ========	input output and file copying ======== */

initf(fd)
int	fd;
{
	register struct fileblk *f = standin;

	f->fdes = fd;
	f->fsiz = ((flags & oneflg) == 0 ? SH_BUFSIZ : 1);
	f->fnxt = f->fend = f->fbuf;
	f->feval = 0;
	f->flin = 1;
	f->sh_feof = FALSE;
#if defined(NLS) || defined(KJI)
	f->fraw = FALSE;
#endif
}

estabf(s)
register uchar_t *s;
{
	register struct fileblk *f;

	(f = standin)->fdes = -1;
	f->fend = length(s) + (f->fnxt = s);
	f->flin = 1;
#if defined(NLS) || defined(KJI)
	/* fraw flag says s is already encoded, otherwise readc() encodes */
	/* only macro() sets this to TRUE after calling estabf()         */
	f->fraw = FALSE;
#endif
	return(f->sh_feof = (s == 0));
}

push(af)
struct fileblk *af;
{
	register struct fileblk *f;

	(f = af)->fstak = standin;
	f->sh_feof = 0;
	f->feval = 0;
	standin = f;
}

pop()
{
	register struct fileblk *f;

	if ((f = standin)->fstak)
	{
		if (f->fdes >= 0)
			close(f->fdes);
		standin = f->fstak;
		return(TRUE);
	}
	else
		return(FALSE);
}

struct tempblk *tmpfptr;

pushtemp(fd,tb)
	int fd;
	struct tempblk *tb;
{
	tb->fdes = fd;
	tb->fstak = tmpfptr;
	tmpfptr = tb;
}

poptemp()
{
	if (tmpfptr)
	{
		close(tmpfptr->fdes);
		tmpfptr = tmpfptr->fstak;
		return(TRUE);
	}
	else
		return(FALSE);
}
	
chkpipe(pv)
int	*pv;
{
	if (pipe(pv) < 0 || pv[INPIPE] < 0 || pv[OTPIPE] < 0)
		error(MSGSTR(M_PIPERR,(char *)piperr));
}

chkopen(idf)
uchar_t *idf;
{
	register int	rc;

	if ((rc = open((char *)idf, 0)) < 0)
		failed(idf, MSGSTR(M_BADOPEN,(char *)badopen));
	else
		return(rc);
}

sh_rename(f1, f2)
register int	f1, f2;
{
	if (f1 != f2)
	{
#ifdef	i860
		dup2(f1, f2);
#else
		dup(f1 | DUPFLG, f2);
#endif
		close(f1);
		if (f2 == 0)
			ioset |= 1;
	}
}

create(s)
uchar_t *s;
{
	register int	rc;

	if ((rc = creat((char *)s, (mode_t)0666)) < 0)
		failed(s, MSGSTR(M_BADCREATE,(char *)badcreate));
	else
		return(rc);
}

tmpfil(tb)
	struct tempblk *tb;
{
	int fd;

	itos(serial++);
	movstr(numbuf, tempname);
	fd = create(tmpout);
	pushtemp(fd,tb);
	return(fd);
}

/*
 * set by trim
 */
extern BOOL		nosubst;
#define			CPYSIZ		512

copy(ioparg)
struct ionod	*ioparg;
{
	register uchar_t	*cline;
	register uchar_t	*clinep;
	register struct ionod	*iop;
	uchar_t	c;
	uchar_t	*ends;
	uchar_t	*start;
	int		fd;
	int		i;
	int		stripflg;
	
#ifdef NLS
/*
 *      Copy stdin to iop, a temporary file.
 *      "nosubst" is set if terminator (ends) is quoted e.g. cat<<"EOF"
 *      If nosubst is set, temp file is decoded and will be read directly
 *      by command it is being passed to.  If it is reset, temp file is
 *      encoded, and read later by subst() which does substition.
 *      (Why the shell doesn't do substitution now and be done with it
 *      is beyond me, but I'm not going to experiment...)
 */
#endif

	if (iop = ioparg)
	{
		struct tempblk tb;

		copy(iop->iolst);
		ends = dqmacro(iop->ioname);
		trim(ends);
#if defined(NLS) || defined(KJI)
		if (nosubst)
			NLSdecode(ends);
		else
			ends += 1;      /* skip FNLS uchar_tacter */
#endif
		stripflg = iop->iofile & IOSTRIP;
		if (nosubst)
			iop->iofile &= ~IODOC;
		fd = tmpfil(&tb);

	/*	code change for PTM 5667
	 *	allocate all iop's from the heap
	*/
		iop->ioname = make(tmpout);

		iop->iolst = iotemp;
		iotemp = iop;

		cline = clinep = start = locstak();
		if (stripflg)
		{
			iop->iofile &= ~IOSTRIP;
			while (*ends == '\t')
				ends++;
		}
		for (;;)
		{
			chkpr();
			if (nosubst)
			{
#if defined(NLS) || defined(KJI)
				/* mark new string as encoded */
				needmem(clinep);
				*clinep++ = FNLS;
#endif
				c = readc();
				if (stripflg)
					while (c == '\t')
						c = readc();

				while (!eolchar(c))
				{
					needmem(clinep);
					*clinep++ = c;
					c = readc();
				}
#if defined(NLS) || defined(KJI)
				needmem(clinep);
				*clinep = 0;
				NLSdecode(cline);
				clinep = cline + strlen(cline);
#endif
			}
			else
			{
				c = nextc(*ends);
				if (stripflg)
					while (c == '\t')
						c = nextc(*ends);
				
				while (!eolchar(c))
				{
					needmem(clinep);
					*clinep++ = c;
					c = nextc(*ends);
				}
			}

			needmem(clinep);
			*clinep = 0;

			if (eof || eq(cline, ends))
			{
				if ((i = cline - start) > 0)
					write(fd, start, i);
				break;
			}
			else {
				needmem(clinep);
				*clinep++ = NL;
			}
			if ((i = clinep - start) < CPYSIZ)
				cline = clinep;
			else
			{
				write(fd, start, i);
				cline = clinep = start;
			}
		}

		poptemp();		/* pushed in tmpfil -- bug fix for problem
					   deleting in-line scripts */
	}
}


link_iodocs(i)
	struct ionod	*i;
{
	while(i)
	{
		free(i->iolink);

		itos(serial++);
		movstr(numbuf, tempname);
		i->iolink = make(tmpout);
		link(i->ioname, i->iolink);

		i = i->iolst;
	}
}


swap_iodoc_nm(i)
	struct ionod	*i;
{
	while(i)
	{
		free(i->ioname);
		i->ioname = i->iolink;
		i->iolink = 0;

		i = i->iolst;
	}
}


savefd(fd)
	int fd;
{
	register int	f;

	f = fcntl(fd, F_DUPFD, 10);
	return(f);
}


restore(last)
	register int	last;
{
	register int 	i;
	register int	dupfd;

	for (i = topfd - 1; i >= last; i--)
	{
		if ((dupfd = fdmap[i].dup_fd) > 0)
			sh_rename(dupfd, fdmap[i].org_fd);
		else
			close(fdmap[i].org_fd);
	}
	topfd = last;
}
