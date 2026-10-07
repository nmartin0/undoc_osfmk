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
static char rcsid[] = "@(#)$RCSfile: misc.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:21:37 $";
#endif
/*
 * COMPONENT_NAME: CMDCSH  c shell(csh)
 *
 * FUNCTIONS: letter digit alnum any calloc cfree blkend blkpr blklen 
 *            blkcpy blkcat blkfree saveblk strspl blkspl lastchr closem 
 *            closech donefds dmove dcopy renum copy lshift number copyblk 
 *            strend strip udvar prefix
 *
 * ORIGINS: 10,26,27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * (Copyright statements and/or associated legends of other
 * companies whose code appears in any part of this module must
 * be copied here.)
 */

/*
 * IBM CONFIDENTIAL
 * Copyright International Business Machines Corp. 1989
 * Unpublished Work
 * All Rights Reserved
 * Licensed Material - Property of IBM
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 

#ifdef KJI
#include <NLctype.h>
#else
#include <ctype.h>
#endif
#include "local.h"
#include "sh.h"

/*
 * C Shell
 */

letter(c)
#ifdef KJI
	register unsigned int c;
{
	return (NCisalpha(c) || isjkanji(c) || isjkata(c) || isjhira(c)
		|| (c == '_'));
#else
	/*
	 * All non-ASCII character are considered letters.
	 * Two calls to this will pick up 2-byte NLS character.
	 * Sometimes this function gets passed -1 which is not a letter,
	 * though 0xff is a letter.
	 */
	register unsigned int c;
{
	return(NCisalpha(c) || c == '_');
#endif
}

digit(c)
#ifdef KJI
	register int c;
#else
	register uchar_t c;
#endif
{

	return (isascii(c) && (c >= '0' && c <= '9'));
}

alnum(c)
#if defined(NLS) || defined(KJI)
	register int c;
#else
	register uchar_t c;
#endif
{
#ifndef KJI
	return (letter(c) || digit(c));
#else
	return (letter(c) || digit(c) || isjdigit(c));
#endif
}

#ifdef KJI
/*
 *	Determine if there is an occurrence of the character "c" in the
 *	string "s", where "c" is a 1- or 2-byte character encoded as an 
 *	integer and "s" is a byte string consisting of 1- and 2-byte
 *	character.
 */
#endif

any(c, s)
	register int c;
	register uchar_t *s;
{

#ifdef KJI
	wchar_t	      nl;
	register wchar_t *nlc = &nl;
	while (*s) {
		s += NCdec (s, nlc);
		if (*nlc == c)
			return(1);
	}
#else
	while (*s)
		if (*s++ == c)
			return(1);
#endif
	return(0);
}

#ifdef KJI
/*
 *	Determine if there is an occurrence of the character "c" in the
 *	string "s".
 *	This function handles the case where "c" is a 1- or 2-byte character
 *	encoded as an integer and "s" is a string containing 1- and 2-byte
 *	character.  If an NLQUOTE is encountered in "s", no comparison
 *	is made with the character following it. 
 */

any_noquote(c, s)
register int c;
register uchar_t *s;
{
	register uchar_t *p = s;
	wchar_t nlc;

	while(*p) {
		p += NCdec (p, &nlc);
		if (nlc == c)
			return (1);
		else if (nlc == NLQUOTE)
			p++;
	}
	return (0);
}
#endif

uchar_t *
calloc(i, j)
	register unsigned i;
	unsigned j;
{
	register uchar_t *cp, *dp;
#ifdef debug
	static uchar_t *av[2] = {0, 0};
#endif

	i *= j;
	cp = (uchar_t *) malloc(i);
	if (cp == 0) {
		child++;
#ifndef debug
		error(MSGSTR(M_NOMEM, "Out of memory"));
#else
#ifdef ALLOC
		showall(av);
#endif
		printf("i=%d, j=%d: ", i/j, j);
		printf("Out of memory\n");
		chdir("/usr/tmp/cshcore");
		abort();
#endif
	}
	dp = cp;
	if (i != 0)
		do
			*dp++ = 0;
		while (--i);
	return (cp);
}

cfree(p)
	uchar_t *p;
{

	free(p);
}

uchar_t **
blkend(up)
	register uchar_t **up;
{

	while (*up)
		up++;
	return (up);
}
 
blkpr(av)
	register uchar_t **av;
{

	for (; *av; av++) {
		printf("%s", *av);
		if (av[1])
			printf(" ");
	}
}

blklen(av)
	register uchar_t **av;
{
	register int i = 0;

	while (*av++)
		i++;
	return (i);
}

uchar_t **
blkcpy(oav, bv)
	uchar_t **oav;
	register uchar_t **bv;
{
	register uchar_t **av = oav;

	while (*av++ = *bv++)
		continue;
	return (oav);
}

uchar_t **
blkcat(up, vp)
	uchar_t **up, **vp;
{

	blkcpy(blkend(up), vp);
	return (up);
}

blkfree(av0)
	uchar_t **av0;
{
	register uchar_t **av = av0;

	while (*av)
		xfree(*av++);
	xfree((char *)av0);
}

uchar_t **
saveblk(v)
	register uchar_t **v;
{
	register int len = blklen(v) + 1;
	register uchar_t **newv = (uchar_t **) calloc(len, sizeof (char **));
	uchar_t **onewv = newv;

	while (*v)
		*newv++ = savestr(*v++);
	return (onewv);
}

uchar_t *
strspl(cp, dp)
	register uchar_t *cp, *dp;
{
	register uchar_t *ep = calloc(1, strlen(cp) + strlen(dp) + 1);

	strcpy(ep, cp);
	strcat(ep, dp);
	return (ep);
}

uchar_t **
blkspl(up, vp)
	register uchar_t **up, **vp;
{
	register uchar_t **wp = (uchar_t **) calloc(blklen(up) + blklen(vp) + 1, sizeof (char **));

	blkcpy(wp, up);
	return (blkcat(wp, vp));
}

lastchr(cp)
	register uchar_t *cp;
{

	if (!*cp)
		return (0);
	while (cp[1])
		cp++;
	return (*cp);
}

/*
 * This routine is called after an error to close up
 * any units which may have been left open accidentally.
 */
closem()
{
	register int f;

#if defined(DEBUG) && defined(debugp)
printf("SHIN=%d, SHOUT=%d, SHDIAG=%d, OLDSTD=%d\n",
		SHIN, SHOUT, SHDIAG, OLDSTD);
#endif
	for (f = 0; f <= open_max; f++)
		if (f != SHIN && f != SHOUT && f != SHDIAG && f != OLDSTD &&
#ifdef MSG
		    f != FSHMSG &&
#endif
		    f != FSHTTY)
			close(f);
}

/*
 * Close files before executing a file.
 * We could be MUCH more intelligent, since (on a version 7 system)
 * we need only close files here during a source, the other
 * shell fd's being in units 16-19 which are closed automatically!
 */
closech()
{
	register int f;

	if (didcch)
		return;
	didcch = 1;
	SHIN = 0; SHOUT = 1; SHDIAG = 2; OLDSTD = 0;
	for (f = 3; f <= open_max; f++)
#ifdef MSG
		if (f != FSHMSG)
#endif
		close(f);
}

donefds()
{

	close(0), close(1), close(2);
	didfds = 0;
}

/*
 * Move descriptor i to j.
 * If j is -1 then we just want to get i to a safe place,
 * i.e. to a unit > 2.  This also happens in dcopy.
 */
dmove(i, j)
	register int i, j;
{

	if (i == j || i < 0)
		return (i);
/*#ifdef V7			we want to do this with dup2 emulation	*/
	if (j >= 0) {
		dup2(i, j);
                track_open(j);
		return (j);
	} else
/*#endif								*/
		j = dcopy(i, j);
	if (j != i)
		close(i);
	return (j);
}

dcopy(i, j)
	register int i, j;
{

	if (i == j || i < 0 || j < 0 && i > 2)
		return (i);
/*#ifdef V7			we want to do this with dup2 emulation	*/
	if (j >= 0) {
		dup2(i, j);
                track_open(j);
		return (j);
	}
/*#endif								*/
	close(j);
	return (renum(i, j));
}

renum(i, j)
	register int i, j;
{
	register int k = dup(i);

	if (k < 0)
		return (-1);
	if (j == -1 && k > 2) {
                track_open(k);
		return (k);
	}
	if (k != j) {
		j = renum(k, j);
		close(k);
                track_open(j);
		return (j);
	}
        track_open(k);
	return (k);
}

copy(to, from, size)
	register uchar_t *to, *from;
	register int size;
{

	if (size)
		do
			*to++ = *from++;
		while (--size != 0);
}

/*
 * Left shift a command argument list, discarding
 * the first c arguments.  Used in "shift" commands
 * as well as by commands like "repeat".
 */
lshift(v, c)
	register uchar_t **v;
	register int c;
{
	register uchar_t **u = v;

	while (*u && --c >= 0)
		xfree(*u++);
	blkcpy(v, u);
}

number(cp)
	uchar_t *cp;
{

	if (*cp == '-') {
		cp++;
		if (!digit(*cp++))
			return (0);
	}
	while (*cp && digit(*cp))
		cp++;
	return (*cp == 0);
}

uchar_t **
copyblk(v)
	register uchar_t **v;
{
	register uchar_t **nv = (uchar_t **) calloc((unsigned)(blklen(v) + 1), sizeof (uchar_t **));

	return (blkcpy(nv, v));
}

uchar_t *
strend(cp)
	register uchar_t *cp;
{

	while (*cp)
		cp++;
	return (cp);
}

uchar_t *
strip(cp)
	uchar_t *cp;
{
	register uchar_t *dp = cp;
	register uchar_t *xp = cp;
	while (*dp) {
#ifdef KJI
		if (*dp == NLQUOTE) 
			if (*++dp==0) break;
		PUTSTR(xp,dp);
#else
		if (*dp == NLQUOTE) {
			if (*++dp==0) break;
			*xp++ = *dp++ & TRIM;
		}
		else
			*xp++ = *dp++;
#endif
	}
	*xp = 0;
	return (cp);
}

udvar(name)
	uchar_t *name;
{

	setname(name);
	bferr(MSGSTR(M_UNDEF, "Undefined variable"));
}

prefix(sub, str)
	register uchar_t *sub, *str;
{

	for (;;) {
		if (*sub == 0)
			return (1);
		if (*str == 0)
			return (0);
		if (*sub++ != *str++)
			return (0);
	}
}
