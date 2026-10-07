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
static char rcsid[] = "@(#)$RCSfile: exec.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:21:28 $";
#endif
/*
 * COMPONENT_NAME: CMDCSH  c shell(csh)
 *
 * FUNCTIONS: doexec pexerr texec getpath rmwhite execash xechoit dohash 
 *             dounhash hash
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

#include <dirent.h>

#ifdef KJI
#include <NLctype.h>
#endif
#include "sh.h"

/*
 * C shell
 */

/*
 * System level search and execute of a command.
 * We look in each directory for the specified command name.
 * If the name contains a '/' then we execute only the full path name.
 * If there is no search path then we execute only full path names.
 */

/* 
 * As we search for the command we note the first non-trivial error
 * message for presentation to the user.  This allows us often
 * to show that a file has the wrong mode/no access when the file
 * is not in the last component of the search path, so we must
 * go on after first detecting the error.
 */
uchar_t	*exerr;			/* Execution error message */
uchar_t	*expath;		/* Path for exerr */

/*
 * Xhash is an array of HSHSIZ uchar_ts, which are used to hash execs.
 * If it is allocated, then to tell whether ``name'' is (possibly)
 * present in the i'th component of the variable path, you look at
 * the i'th bit of xhash[hash("name")].  This is setup automatically
 * after .login is executed, and recomputed whenever ``path'' is
 * changed.
 */
int	havhash;
#define	HSHSIZ	511
uchar_t	xhash[HSHSIZ];

/* Dummy search path for just absolute search when no path */
uchar_t	*justabs[] =	{ (uchar_t *)"", 0 };

doexec(t)
	register struct command *t;
{
	uchar_t *sav;
	register uchar_t *dp, **pv, **av;
	register struct varent *v;
	bool slash = any('/', t->t_dcom[0]);
	int hashval, i;
	uchar_t *blk[2];

	/*
	 * Glob the command name.  If this does anything, then we
	 * will execute the command only relative to ".".  One special
	 * case: if there is no PATH, then we execute only commands
	 * which start with '/'.
	 */
	sav = t->t_dcom[0];
	dp = globone(t->t_dcom[0]);
	exerr = 0; expath = t->t_dcom[0] = dp;
	xfree(sav);
	v = adrof("path");
	if (v == 0 && expath[0] != '/')
		pexerr();
	slash |= gflag;

	/*
	 * Glob the argument list, if necessary.
	 * Otherwise trim off the quote bits.
	 */
	gflag = 0; av = &t->t_dcom[1];
	rscan(av, tglob);
	if (gflag) {
		av = glob(av);
		if (av == 0)
			error(MSGSTR(M_NOMATCH, "No match"));
	}
#ifndef KJI
	/* Do not need to scan this twice. */
	blk[0] = t->t_dcom[0];
	blk[1] = 0;
	av = blkspl(blk, av);
#endif
	scan(av, trim);
#ifdef KJI
	blk[0] = t->t_dcom[0];
	blk[1] = 0;
	av = blkspl(blk, av);
#endif

	xechoit(av);		/* Echo command if -x */
	closech();		/* Close random fd's */

	/*
	 * We must do this after any possible forking (like `foo`
	 * in glob) so that this shell can still do subprocesses.
	 */
	sigblock(0);

	/*
	 * If no path, no words in path, or a / in the filename
	 * then restrict the command search.
	 */
	if (v == 0 || v->vec[0] == 0 || slash)
		pv = justabs;
	else
{
#if defined(DEBUG) && defined(debugc)
uchar_t **P;
#endif
		pv = v->vec;
#if defined(DEBUG) && defined(debugc)
for (P=pv; P&&*P&&**P;P++)
	printf("doexec: pv=%s\n", *P);
#endif
}
	sav = strspl("/", *av);		/* / command name for postpending */
	if (havhash)
{
		hashval = xhash[hash(*av)];
#if defined(DEBUG) && defined(debugc)
printf("doexec: *av=%s, hash(*av)=%d, xhash[hash(*av)]=%d\n",
	*av, hash(*av), xhash[hash(*av)]);
#endif
}
	i = 0;
	do {
#if defined(DEBUG) && defined(debugc)
printf("doexec: slash=%d, havhash=%d, *pv=%s, hashval=%d, i=%d\n",
        slash, havhash, *pv, hashval, i);
#endif

		if (!slash && pv[0][0] == '/' && havhash && (hashval & (1 << (i % 8))) == 0)
			goto cont;
		if (pv[0][0] == 0 || eq(pv[0], "."))	/* don't make ./xxx */
{
#if defined(DEBUG) && defined(debugc)
printf("doexec: *av=%s\n", *av);
#endif
			texec(*av, av);
}
		else {
			dp = strspl(*pv, sav);
			texec(dp, av);
			xfree(dp);
		}
cont:
		pv++;
		i++;
	} while (*pv);
	xfree(sav);
	xfree(av);
	pexerr();
}

pexerr()
{

	/* Couldn't find the damn thing */
	setname(expath);
	/* xfree(expath); */
	if (exerr)
		bferr(exerr);
	bferr(MSGSTR(M_NOTFOUND, "Command not found"));
}

/* Last resort shell */
static uchar_t	*lastsh[] =	{ (uchar_t *)SHELLPATH, 0 };
static uchar_t bad_sys[45];   /* used to handle missing entries in sys_errlist */

/*
 * Execute command f, arg list t.
 * Record error message if not found.
 * Also do shell scripts here.
 */
texec(f, t)
	uchar_t *f;
	register uchar_t **t;
{
	register struct varent *v;
	register uchar_t **vp;
	extern char *sys_errlist[];
	extern int  sys_nerr;

#if defined(DEBUG) && defined(debugc)
printf("texec: %s\n", f);
#endif

	execv(f, t);
	switch (errno) {

	case ENOEXEC:
		/*
		 * If there is an alias for shell, then
		 * put the words of the alias in front of the
		 * argument list replacing the command name.
		 * Note no interpretation of the words at this point.
		 */
		v = adrof1("shell", &aliases);
		if (v == 0) {
#if defined(OTHERSH) || !defined(_BSD)
			register int ff = open(f, 0);
			uchar_t ch;

                        track_open(ff);  /* keep high water mark */
#endif
			vp = lastsh;
#if _BSD
			vp[0] = adrof("shell") ? value("shell") : (uchar_t *)SHELLPATH;
#ifdef OTHERSH
			if (ff != -1 && read(ff, &ch, 1) == 1 && ch != '#')
					vp[0] = (uchar_t *)OTHERSH;
#endif
#else
	/* execute OTHERSH unless #! present */
			if (       ff != -1
				&& read(ff, &ch, 1) == 1
				&& ch == '#'
				&& read(ff, &ch, 1) == 1
				&& ch == '!') {
					uchar_t **getpath();
					vp = getpath(ff);
			} else
#ifdef OTHERSH
					vp[0] = OTHERSH;
#else
					vp[0] = SHELLPATH;
#endif
#endif

#if defined(OTHERSH) || !defined(_BSD)
			close(ff);
#endif
		} else
			vp = v->vec;
		t[0] = f;
		t = blkspl(vp, t);		/* Splice up the new arglst */
		f = *t;
		execv(f, t);
		xfree((char *)t);
		/* The sky is falling, the sky is falling! */

	case ENOMEM:
		Perror(f);

	case ENOENT:
		break;

	default:
		if (exerr == 0) {
			if (errno > sys_nerr)
			{
			   sprintf( (char *)bad_sys, "errno %d not in sys_errlist in command ", errno);
			   exerr = bad_sys;
			}
			else exerr = (uchar_t *)sys_errlist[errno];
			expath = savestr(f);
		}
	}
}

#ifndef _BSD
uchar_t **
getpath(ff)
register int ff;
{
	register uchar_t **vp;
	uchar_t tmp[PATH_MAX+1];
#ifdef KJI
	uchar_t ch2;
	int jspace=0;
	register uchar_t *tmptr = tmp;
#else
	register int i=0;
#endif
	register int j=0;
	uchar_t ch;

	/* max. number of arg.s is 10 */
	vp = (char **) calloc(11, sizeof(char *));
	
#ifdef OTHERSH
	vp[0] = OTHERSH;
#else
	vp[0] = SHELLPATH;
#endif	
	ch = '\n';	/* in case of "#!" only */
	if (rmwhite(ff, &ch) < 0)
		return(vp);
	while (ch != '\n') {
#ifdef KJI
		do {
			if (ch == '\n' || ch == '\t' || ch == ' '|| 
				/* need room for 1- or 2-byte character + NULL*/
				((tmptr + 2) > (tmp + sizeof(tmp) - 2))) 
				break;
			if (NCisshift(ch)) {
				if (read(ff, &ch2, 1) != 1)
					break;
				if (isjspace (_NCd2(ch, ch2))) {
					++jspace;
					break;
				}
				*tmptr++ = ch;
				*tmptr++ = ch2;
			} else
				*tmptr++ = ch;
		} while (read(ff, &ch, 1) == 1);
		*tmptr = '\0';
		if ((tmptr == tmp) && jspace) {
			/* this is how we "rmwhite" double-wide spaces */
			jspace = 0;
		} else {
		vp[j] = (char *) malloc(strlen(tmp) + 1);
		strcpy(vp[j], tmp);
		j++;
		tmptr = tmp;
		}
#else
		do {
			tmp[i++] = ch;
		} while (read(ff, &ch, 1) == 1 &&
		       (ch != '\n' && ch != '\t' && ch != ' '));
		tmp[i] = '\0';
		vp[j] = (char *) malloc(strlen(tmp) + 1);
		strcpy(vp[j], tmp);
		j++;
		i = 0;
#endif
		if (ch != '\n')
			if (rmwhite(ff, &ch) < 0)
				break;
	}
	vp[j] = 0;
	return(vp);
}

rmwhite(ff, cptr)
int ff;
uchar_t *cptr;
{
	while (read(ff, cptr, 1) == 1)
		if (*cptr != '\n' && *cptr != '\t' && *cptr != ' ')
			return(0);
	return(-1);
}
#endif

execash(t, kp)
	register struct command *kp;
{
	struct sigvec nsv;

	didcch++;
	nsv.sv_mask = 0;
	nsv.sv_onstack = 0;
	nsv.sv_handler = parintr;
	(void)sigvec(SIGINT, &nsv, (struct sigvec *)0);
	(void)sigvec(SIGQUIT, &nsv, (struct sigvec *)0);
	 /* if doexec loses, screw */
	nsv.sv_handler = parterm;
	(void)sigvec(SIGTERM, &nsv, (struct sigvec *)0);
	lshift(kp->t_dcom, 1);
	exiterr++;
#if defined(DEBUG) && defined(debugc)
printf("debug: execash calls doexec\n");
#endif
	doexec(kp);
	/*NOTREACHED*/
}

xechoit(t)
	uchar_t **t;
{

	if (adrof("echo")) {
		flush();
		haderr = 1;
		blkpr(t), printf("\n");
		haderr = 0;
	}
}

dohash()
{
	register int cnt;
	register struct dirent *dirbuf;
	register DIR *dirf;
	uchar_t d_name[NAME_MAX + 1];
	struct stat stb;
	int i = 0;
	struct varent *v = adrof("path");
	uchar_t **pv;

#if defined(DEBUG) && defined(debugp)
printf("entering dohash\n");
#endif
	havhash = 1;
	for (cnt = 0; cnt < HSHSIZ; cnt++)
		xhash[cnt] = 0;
	if (v == 0)
		return;
	for (pv = v->vec; *pv; pv++, i = (i + 1) % 8) {
#if defined(DEBUG) && defined(debugp)
printf("for loop\n");
#endif
		if (pv[0][0] != '/')
			continue;
		dirf = opendir((char *)*pv);
		if (dirf ==  NULL)
			continue;
		if (stat((char *)*pv, &stb) < 0 || !isdir(stb)) {
			closedir(dirf);
			continue;
		}
		while ((dirbuf = readdir(dirf)) != NULL ) {
			copdent(d_name, dirbuf->d_name);
			xhash[hash(d_name)] |= (1 << i);
		}
		closedir(dirf);
	}
#if defined(DEBUG) && defined(debugp)
printf("d_name=%s, hash(d_name)=%d, xhash[hash(d_name)]=%d\n",
	d_name, hash(d_name), xhash[hash(d_name)]);
#endif
}

dounhash()
{

	havhash = 0;
}

hash(cp)
	register uchar_t *cp;
{
	register long hash = 0;
	int retval;

	while (*cp)
		hash += hash + *cp++;
	if (hash < 0)
		hash = -hash;
	retval = hash % HSHSIZ;
	return (retval);
}
