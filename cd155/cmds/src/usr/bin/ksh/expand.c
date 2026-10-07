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
static char rcsid[] = "@(#)$RCSfile: expand.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:27:17 $";
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
 *	File name expansion
 *
 *	David Korn
 *	AT&T Bell Laboratories
 *
 */

#include	"sh_config.h"
#ifdef KSHELL
#   include	"defs.h"
#else
#   include	<sys/stat.h>
#   include	<setjmp.h>
#   ifdef _unistd_
#	include	<unistd.h>
#   endif /* _unistd_ */
#endif /* KSHELL */

#if (defined(KJI) || defined(NLS))
#include	<NLchar.h>
#endif

/* now for the directory reading routines */
#ifdef  FS_3D
#       undef   _ndir_
#       define  _dirent_        1
#endif /* FS_3D */
#ifdef _ndir_
#   undef	direct
#   define direct dirent
#   include	<ndir.h>
#else
/*
#   undef       dirent
#   ifndef      FS_3D
#       define  dirent direct
#   endif  * FS_3D */
#   ifdef _dirent_
#	include	<dirent.h>
#   else
#	include	<sys/dir.h>
#	ifndef rewinddir	/* old system V */
#           define OLDSYS5 1
#	    define NDENTS	32
	    typedef struct 
	    {
		int		fd;
		struct dirent	*next;
		struct dirent	*last;
		struct dirent	entries[NDENTS];
		char		extra;
		ino_t		save;
	    } DIR;
	    DIR *opendir();
	    struct dirent *readdir();
#  	    define closedir(dir)	close(dir->fd)
#	endif /* rewinddir */
#   endif /* _dirent_ */
#endif


#ifdef KSHELL
#   define check_signal()	(sh.trapnote&SIGSET)
#    define argbegin	argnxt.cp
    int		path_expand();
    void	rm_files();
    int		f_complete();
    static	char	*sufstr;
    static	int	suflen;
#else
#   define check_signal()	(0)
#   define round(x,y)		(((int)(x)+(y)-1)&~((y)-1))
#   define stak_end(x)		(*(globptr()->last = (x))=0)
#   define sh_access		access
#   define suflen		0
    struct argnod
    {
	struct argnod	*argbegin;
	struct argnod	*argchn;
	char		argval[1];
    };
    static char		*sh_copy();
    static char		*stak_begin();
    static int		test_type();
#endif /* KSHELL */


/*
 * This routine builds a list of files that match a given pathname
 * Uses external routine strmatch() to match each component
 * A leading . must match explicitly
 *
 */

struct glob
{
	int		argn;
	char		**argv;
	int		flags;
	struct argnod	*rescan;
	struct argnod	*match;	
	DIR		*dirf;
#ifndef KSHELL
	char		*memlast;
	char		*last;
	struct argnod	*resume;
	jmp_buf		jmpbuf;
	char		begin[1];
#endif
};


#define GLOB_RESCAN 1
#define	argstart(ap)	((ap)->argbegin)
#define globptr()	((struct glob*)membase)

static struct glob	 *membase;
static void		addmatch();
static void		glob_dir();

int path_expand(pattern)
char *pattern;
{
	register struct argnod *ap;
	register struct glob *gp;
#ifdef KSHELL
	struct glob globdata;
	membase = &globdata;
#endif /* KSHELL */
	gp = globptr();
	ap = (struct argnod*)stak_alloc(strlen(pattern)+sizeof(struct argnod)+suflen);
	gp->rescan =  ap;
	gp->argn = 0;
#ifdef KSHELL
	gp->match = st.gchain;
#else
	gp->match = 0;
#endif /* KSHELL */
	ap->argbegin = ap->argval;
	ap->argchn = 0;
#ifdef KSHELL
	pattern = sh_copy(pattern,ap->argval);
	if(suflen)
		sh_copy(sufstr,pattern);
#else
	sh_copy(pattern,ap->argval);
#endif /* KSHELL */
	suflen = 0;
	do
	{
		gp->rescan = ap->argchn;
		glob_dir(ap);
	}
	while(ap = gp->rescan);
#ifdef KSHELL
	st.gchain = gp->match;
#endif /* KSHELL */
	return(gp->argn);
}

static void glob_dir(ap)
struct argnod *ap;
{
#if (defined(KJI) || defined(NLS))
	char		*lastc;
	char		*rescan;
	char		*prefix;
#else
	register char	*rescan;
	register char	*prefix;
#endif
	register char	*pat;
	DIR 		*dirf;
	char		quote = 0;
	char		savequote = 0;
	char		meta = 0;
	char		bracket = 0;
	char		first;
	char		*dirname;
	struct dirent	*dirp;
	if(check_signal())
		return;
	pat = rescan = argstart(ap);
	prefix = dirname = ap->argval;
	first = (rescan == prefix);
	/* check for special chars */
#if (defined(KJI) || defined(NLS))
	while(1) switch(lastc = rescan, rescan += NLchrlen(rescan), *lastc)
#else
	while(1) switch(*rescan++)
#endif
	{
		case 0:
			rescan = 0;
			if(meta)
				goto process;
			if(first)
				return;
			if(quote)
				sh_trim(argstart(ap));
			if(sh_access(prefix,F_OK)==0)
				addmatch((char*)0,prefix,(char*)0);
			return;

		case '/':
			if(meta)
				goto process;
			pat = rescan;
			bracket = 0;
			savequote = quote;
			break;

		case '[':
			bracket = 1;
			break;

		case ']':
			meta |= bracket;
			break;

		case '*':
		case '?':
		case '(':
			meta=1;
			break;

		case '\\':
			quote = 1;
#if (defined(KJI) || defined(NLS))
			rescan += NLchrlen(rescan);
#else
			rescan++;
#endif
	}
process:
	if(pat == prefix)
	{
		dirname = ".";
		prefix = 0;
	}
	else
	{
#if (defined(KJI) || defined(NLS))
		if(pat==(prefix + NLchrlen(prefix)))
#else
		if(pat==prefix+1)
#endif
			dirname = "/";
		*(pat-1) = 0;
		if(savequote)
			sh_trim(argstart(ap));
	}
	if(dirf=opendir(dirname))
	{
		/* check for rescan */
		if(rescan)
			*(rescan-1) = 0;
		while(dirp = readdir(dirf))
		{
			if(dirp->d_fileno==0 || (*dirp->d_name=='.' && *pat!='.'))
				continue;
			if(strmatch(dirp->d_name, pat))
				addmatch(prefix,dirp->d_name,rescan);
		}
		closedir(dirf);
	}
	return;
}

static  void addmatch(dir,pat,rescan)
char *dir, *pat, *rescan;
{
	register struct argnod *ap = (struct argnod*)stak_begin();
	register struct glob *gp = globptr();
	register char *cp = ap->argval;
#ifdef KSHELL
#ifdef  KSH_88D
        ap->argflag = A_RAW;
#endif /* KSH_88D */
#endif /* KSHELL */
	if(dir)
	{
		cp = sh_copy(dir,cp);
		*cp++ = '/';
	}
	cp = sh_copy(pat,cp);
	if(rescan)
	{
		if(test_type(ap->argval,S_IFMT,S_IFDIR)==0)
			return;
		*cp++ = '/';
		ap->argbegin = cp;
		cp = sh_copy(rescan,cp);
		ap->argchn = gp->rescan;
		gp->rescan = ap;
	}
	else
	{
#ifdef KSHELL
		if(is_option(MARKDIR) && test_type(ap->argval,S_IFMT,S_IFDIR))
			*cp++ = '/';
#endif /* KSHELL */
		ap->argchn = gp->match;
		gp->match = ap;
		gp->argn++;
	}
	stak_end(cp);
}


#ifdef KSHELL

/*
 * remove tmp files
 * template of the form /tmp/sh$$.???
 */

void	rm_files(template)
register char *template;
{
	register char *cp;
	struct argnod  *schain;
	cp = strrchr(template,'.');
	*(cp+1) = 0;
	f_complete(template,"*");
	schain = st.gchain;
	while(schain)
	{
		unlink(schain->argval);
		schain = schain->argchn;
	}
}

/*
 * file name completion
 * generate the list of files found by adding an suffix to end of name
 * The number of matches is returned
 */

f_complete(name,suffix)
char *name;
register char *suffix;
{
	st.gchain =  0;
	sufstr = suffix;
	suflen = strlen(suffix);
	return(path_expand(name));
}

#else

static char *sh_copy(sp,dp)
register char *sp;
register char *dp;
{
	register char *memlast = globptr()->memlast;
	while(dp < memlast)
	{
		if((*dp = *sp++)==0)
			return(dp);
		dp++;
	}
	longjmp(globptr()->jmpbuf);
}

static char * stak_begin()
{
	register struct argnod *ap;
	register struct glob *gp = globptr();
	ap = (struct argnod*)(round(gp->last,sizeof(struct argnod*)));
	gp->last = ap->argval;
	return((char*)ap);
}

/*
 * Return true if the mode bits of file <f> corresponding to <mask> have
 * the value equal to <field>.  If <f> is null, then the previous stat
 * buffer is used.
 */

static test_type(f,mask,field)
char *f;
int field;
{
	static struct stat statb;
	if(f && stat(f,&statb)<0)
		return(0);
	return((statb.st_mode&mask)==field);
}

/*
 * remove backslashes
 */

static void sh_trim(sp)
register char *sp;
{
	register char *dp = sp;
	register int c;
	while(1)
	{
		if((c= *sp++) == '\\')
			c = *sp++;
		*dp++ = c;
		if(c==0)
			break;
	}
}
#endif /* KSHELL */

#ifdef OLDSYS5

static DIR dirbuff;

DIR *opendir(name)
char *name;
{
	register int fd;
	struct stat statb;
	if((fd = open(name,0)) < 0)
		return(0);
	if(fstat(fd,&statb) < 0 || (statb.st_mode&S_IFMT)!= S_IFDIR)
#ifdef  KSH_88D
        {
                close (fd);
                return (0);
        }
#else
		return(0);
#endif /* KSH_88D */
	dirbuff.fd = fd;
	dirbuff.next = dirbuff.last = dirbuff.entries + NDENTS;
	return(&dirbuff);
}

struct dirent *readdir(dir)
register DIR *dir;
{
	register int n;
	struct dirent *dp;
	if(dir->next >= dir->last)
	{
		n = read(dir->fd,(char*)dir->entries,NDENTS*sizeof(struct dirent));
		n /= sizeof(struct dirent);
		if(n <=0)
			return(0);
		dir->next = dir->entries;
		dir->last = dir->entries + n;
	}
	else
		dir->next->d_fileno =  dir->save;
	dp = (struct dirent*)dir->next++;
	dir->save = dir->next->d_fileno;
	dir->next->d_fileno = 0;
	return(dp);
}
#endif /* OLDSYS5 */

#ifdef BRACEPAT
int expbrace(todo)
struct argnod *todo;
/*@
	assume todo!=0;
	return count satisfying count>=1;
@*/
{
#if (defined(KJI) || defined(NLS))
	char *cp;
	char *lastcp;
#else
	register char *cp;
#endif
	register int brace;
	register struct argnod *ap;
	struct argnod *top = 0;
	struct argnod *apin;
	char *pat, *rescan;
	char *sp;
	char comma;
	int count = 0;
#ifdef  KSH_88D
        todo->argchn = 0;
#else
        ap->argchn = 0;
#endif /* KSH_88D */
again:
	apin = ap = todo;
	todo = ap->argchn;
	cp = ap->argval;
	comma = brace = 0;
	/* first search for {...,...} */
#if (defined(KJI) || defined(NLS))
	while(1) switch(lastcp = cp, cp += NLchrlen(cp), *lastcp)
#else
	while(1) switch(*cp++)
#endif
	{
		case '{':
			if(brace++==0)
				pat = cp;
			break;
		case '}':
			if(--brace>0)
				break;
			if(brace==0 && comma)
				goto endloop1;
			comma = brace = 0;
			break;
		case ',':
			if(brace==1)
				comma = 1;
			break;
		case '\\':
#if (defined(KJI) || defined(NLS))
			cp += NLchrlen(cp);
#else
			cp++;
#endif
			break;
		case 0:
			/* insert on stack */
			ap->argchn = top;
			top = ap;
			if(todo)
				goto again;
			for(; ap; ap=apin)
			{
				apin = ap->argchn;
				if((brace = path_expand(ap->argval)))
					count += brace;
				else
				{
					ap->argchn = st.gchain;
					st.gchain = ap;
					count++;
				}
				st.gchain->argflag |= A_MAKE;
			}
			return(count);
	}
endloop1:
	rescan = cp;
#if (defined(KJI) || defined(NLS))
	cp = lastcp;
#else
	cp = pat-1;
#endif
	*cp = 0;
	while(1)
	{
		brace = 0;
		/* generate each pattern and but on the todo list */
#if (defined(KJI) || defined(NLS))
		while(1) switch(cp += NLchrlen(cp), *cp)
#else
		while(1) switch(*++cp)
#endif
		{
			case '\\':
#if (defined(KJI) || defined(NLS))
			cp += NLchrlen(cp);
#else
				cp++;
#endif
				break;
			case '{':
				brace++;
				break;
			case ',':
				if(brace==0)
					goto endloop2;
				break;
			case '}':
				if(--brace<0)
					goto endloop2;
		}
	endloop2:
		/* check for match of '{' */
		if(*cp != '}')
			ap = (struct argnod*)stak_begin();
			ap->argflag = 0;
		else
			ap = apin;
		*cp = 0;
		sp = sh_copy(apin->argval,ap->argval);
		sp = sh_copy(pat,sp);
		sp = sh_copy(rescan,sp);
		ap->argchn = todo;
		todo = ap;
		if(ap == apin)
			break;
		stak_end(sp);
#if (defined(KJI) || defined(NLS))
		pat = cp + NLchrlen(cp);
#else
		pat = cp+1;
#endif
	}
	goto again;
}
#endif /* BRACEPAT */
