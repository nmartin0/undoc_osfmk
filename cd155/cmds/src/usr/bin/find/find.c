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
static char rcsid[] = "@(#) $RCSfile: find.c,v $ $Revision: 1.4 $ (OSF) $Date: 1994/11/19 01:24:51 $";
#endif
/*
 * COMPONENT_NAME: (CMDSCAN) commands that scan files
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
#ifndef lint
	static char sccsid[] = "find.c	1.28  com/cmd/scan,3.1,9021D 5/24/90 14:15:25";
#endif /* lint */

#include        <sys/types.h>
#include        <sys/param.h>
#include	<stdio.h>
#include        <dirent.h>
#include	<locale.h>
#include	<pwd.h>
#include	<grp.h>
#include 	<utmp.h>
#include        <sys/limits.h>
#include        <sys/mount.h>
#include        <sys/stat.h>
#include        <sys/errno.h>
#include        <sys/param.h>
#include	<NLctype.h>
#include        <NLchar.h>

#include "find_msg.h"
char *NLgetenv();
char *strncpy();
nl_catd catd;
#define MSGSTR(Num, Str) catgets(catd,MS_FIND, Num, Str)

#define A_DAY	86400L /* a day full of seconds */
#define EQ(x, y)	(strcmp(x, y)==0)
#define BUFSIZE	512
#define CPIOBSZ	4096
#define Bufsize	5120

#define UUID     1
#define GGID     2

int	Randlast;
char	Pathname[PATH_MAX];
char	*ystr = "y";
extern errno;
struct anode {
	int (*F)();
	struct anode *L, *R;
} Node[100];
int Nn;  /* number of nodes */
char	*Fname;
char    fstyped;         /* -fstype option specified */
int     Fstype;		 /* type # of current fs */
long	Now;
int	Argc,
	Ai,
	Pi;
char	**Argv;
/* cpio stuff */
int	Cpio;
short	*SBuf, *Dbuf, *Wp;
char	*Buf, *Cbuf, *Cp;
char	Strhdr[500],
	*Chdr = Strhdr;
int	Wct = Bufsize / 2, Cct = Bufsize;
int	Cflag;
int	depthf = 0;
long	Newer;
int     pruned = 0;     /* terminate search in this directory */

int	Xdev = 1;		/* true if SHOULD cross devices (filesystems) */
struct	stat	Devstat;	/* stats of each argument path's file system */
int	Sizefactor = 512;	/* block size for "-size" option */

/* struct stat Statb;   */
struct stat Statb;
/* statfs struct Statfsb */
struct statfs Statfsb;

struct glob_str { int f; char *pat; }; 
struct mtime_str { int f, t, s; }; 
struct atime_str { int f, t, s; }; 
struct lctime_str { int f, t, s; }; 
struct user_str { int f, u, s; }; 
struct ino_str { int f, u, s; };
struct group_str { int f, u; }; 
struct links_str { int f, link, s; }; 
struct size_str { int f, sz, s; }; 
struct perm_str { int f, per, s; }; 
struct type_str { int f, per, s; };
struct fstype_str { int f; char *fsname; };
struct prune_str { int f, per, s; };
struct exeq_str { int f, com; };
struct ok_str { int f, com; };

static struct anode	*exp(),
			*e1(),
			*e2(),
			*e3(),
			*mk();
static char	*nxtarg();
static char 	*getname();
static char	*getgroup();

char	Home[PATH_MAX];
long	Blocks;
char *strrchr();
char *sbrk();

static int	and(),	or(),	not(),	glob(),	print();
static int	mtime(),atime(),lctime();
static int	user(),	nouser(),ino(), group(),nogroup();
static int	links(),size(),	perm(), type(),	fstype();
static int	prune(),exeq(),	ok();
static int	list(),	crossdev(), depth(), cpio();
static int	bintochar(), ls(), newer();
static int	scomp(), doex(), getunum(), descend();
static int	gmatch(), umatch(), bwrite(), writehdr(); 
static int	chgreel(), chdir_access();
static long	mklong();
static unsigned long	getnid();
static wchar_t	colval();

/*********************
 These are the strings/fstype numbers for the -fstype type option
*********************/
char	*fstype_strngs[MOUNT_MAXTYPE + 1] = {
	"NONE",			/* type zero */
	"ufs",			/* type 1 */
	"nfs",			/* type 2 */
	"mfs",			/* type 3 */
	"pc",			/* type 4 */
	"s5fs",			/* type 5 */
	"afs",			/* type 6 */
#ifdef	PFS
	"pfs"			/* type 7 */
#endif
};

struct header {
	short	h_magic,
		h_dev;
	ushort	h_ino,
		h_mode,
		h_uid,
		h_gid;
	short	h_nlink,
		h_rdev,
		h_mtime[2],
		h_namesize,
		h_filesize[2];
	char	h_name[256];
} hdr;


struct  utmp utmp;
#define NMAX    (sizeof (utmp.ut_name))
#define SCPYN(a, b)     (void)strncpy(a, b, NMAX)

#define NUID    64
#define NGID    300

#ifdef OBSOLETTE
struct ncache {
        int     uid;
        char    name[NMAX+1];
} nc[NUID];
#endif
char    names[NUID][NMAX+1];
char    outrangename[NMAX+1];
int     outrangeuid = -1;
char    groups[NGID][NMAX+1];
char    outrangegroup[NMAX+1];
int     outrangegid = -1;

main(argc, argv) 
int argc;
char *argv[];
{
	struct anode *exlist;
	int paths;
	register char *cp, *sp = 0;
	int i,typno;

	(void) setlocale (LC_ALL,"");
	catd = catopen(MF_FIND,0);
	if (ystr = NLgetenv("YESSTR")) {
		i = strcspn(ystr, ":");
		ystr = strncpy(calloc(i+1,1), ystr, i);
	} else	ystr = "y";

	(void)time(&Now);
	if( getwd(Home) == NULL) {
                (void)fprintf(stderr, MSGSTR( BADPWD, "Cannot execute pwd\n"));
		exit(2);
	}
	Argc = argc; Argv = argv;
	if(argc<3) {
usage:		(void)fprintf(stderr, MSGSTR( USAGE, "Usage: find path-list expression-list\n"));
		exit(1);
	}
	for(Ai = paths = 1; Ai < (argc-1); ++Ai, ++paths)
		if(*Argv[Ai] == '-' || *Argv[Ai] == '(' || *Argv[Ai] == '!')
			break;
	if(paths == 1) /* no path-list */
		goto usage;
	if(!(exlist = exp())) { /* parse and compile the arguments */
		(void)fprintf(stderr,MSGSTR( PARSERR, "find: parsing error\n"));
		exit(1);
	}
	if(Ai<argc) {
		(void)fprintf(stderr, MSGSTR( CONJ, "find: missing conjunction\n"));
		exit(1);
	}
	for(Pi = 1; Pi < paths; ++Pi) {
		sp = NULL;
		(void)strcpy(Pathname, Argv[Pi]);
		if(*Pathname != '/')
			(void)chdir_access(Home);
		if(cp = strrchr(Pathname, '/')) {
			sp = cp + 1;
			*cp = '\0';
			if(chdir_access(*Pathname? Pathname: "/") == -1) {
				(void)fprintf(stderr,MSGSTR( BADSTART, "find: bad starting directory\n"));
				exit(2);
			}
			*cp = '/'; 
		}
		Fname = sp ? sp: Pathname;
		if ( '\0' == *Fname )
			Fname = ".";
		if (fstyped) {
			if( statfs(Fname, &Statfsb) < 0) {
				(void)fprintf(stderr,MSGSTR( BADSTAT, 
				     "find: bad status-- %s\n"), Fname);
				return(0);
			}
				Fstype = Statfsb.f_type;
		}

		if( lstat(Fname,  &Devstat)<0) {
			(void)fprintf(stderr,MSGSTR( BADSTAT, "find: bad status-- %s\n"), Pathname);
			exit(2);
		}

		/* to find files that match  */
		(void)descend(Pathname, Fname, Fstype, 0, exlist);
	}
	if(Cpio) {
		(void)strcpy(Pathname, "TRAILER!!!");
		Statb.st_size = 0;
		(void)cpio();
	}
	return(0);
}

/* compile time functions:  priority is  exp()<e1()<e2()<e3()  */

static struct anode *
exp() /* parse ALTERNATION (-o)  */
{
	int or();
	register struct anode * p1;

	p1 = e1() /* get left operand */ ;
	if(EQ(nxtarg(), "-o")) {
		Randlast--;
		return(mk(or, p1, exp()));
	}
	else if(Ai <= Argc) --Ai;
	return(p1);
}

static struct anode *
e1()  /* parse CONCATENATION (formerly -a) */
{
	int and();
	register struct anode * p1;
	register char *a;

	p1 = e2();
	a = nxtarg();
	if(EQ(a, "-a")) {
And:
		Randlast--;
		return(mk(and, p1, e1()));
	} else if(EQ(a, "(") || EQ(a, "!") || (*a=='-' && !EQ(a, "-o"))) {
		--Ai;
		goto And;
	} else if(Ai <= Argc) --Ai;
	return(p1);
}

static struct anode *
e2()  /* parse NOT (!) */
{
	int not();

	if(Randlast) {
		(void)fprintf(stderr,MSGSTR( OPFOP, "find: operand follows operand\n"));
		exit(1);
	}
	Randlast++;
	if(EQ(nxtarg(), "!"))
		return(mk(not, e3(), (struct anode *)0));
	else if(Ai <= Argc) --Ai;
	return(e3());
}

static struct anode *
e3()  /* parse parens and predicates */
{
	/*
	int exeq(), ok(), glob(),  mtime(), atime(), lctime(), user(),
		group(), size(), perm(), links(), print(), prune(),
		type(), ino(), depth(), nnode(), cpio(), newer(), fstype(),
		ls(), crossdev(), nouser(), nogroup();
	*/
	struct anode *p1;
	int i;
	unsigned long n, getnid();
	register char *a, *b, c;
	register int  s;

	a = nxtarg();
	if(EQ(a, "(")) {
		Randlast--;
		p1 = exp();
		a = nxtarg();
		if(!EQ(a, ")")) goto err;
		return(p1);
	}
	else if(EQ(a, "-print")) {
		return(mk(print, (struct anode *)0, (struct anode *)0));
	}
        else if (EQ(a, "-nouser")) {
                return(mk(nouser, (struct anode *)0, (struct anode *)0));
        }
        else if (EQ(a, "-nogroup")) {
                return(mk(nogroup, (struct anode *)0, (struct anode *)0));
        }
        else if (EQ(a, "-ls")) {
                return(mk(ls, (struct anode *)0, (struct anode *)0));
        }
	else if(EQ(a, "-prune")) {
		return(mk(prune, (struct anode *)0, (struct anode *)0));
	}
	else if(EQ(a, "-xdev")) {
		Xdev = 0;
		return(mk(crossdev, (struct anode *)0, (struct anode *)0));
	}
	else if(EQ(a, "-depth")) {
		depthf = 1;
		return(mk(depth, (struct anode *)0, (struct anode *)0));
	}
	b = nxtarg();
	s = *b;
	if(s=='+') b++;
	if(EQ(a, "-name"))
		return(mk(glob, (struct anode *)b, (struct anode *)0));
	else if(EQ(a, "-mtime"))
		return(mk(mtime, (struct anode *)atoi(b), (struct anode *)s));
	else if(EQ(a, "-atime"))
		return(mk(atime, (struct anode *)atoi(b), (struct anode *)s));
	else if(EQ(a, "-ctime"))
		return(mk(lctime, (struct anode *)atoi(b), (struct anode *)s));
	else if(EQ(a, "-user")) {
		if((i=getunum(UUID, b)) == -1) {
			if(gmatch(b, "[0-9][0-9][0-9]*")
			|| gmatch(b, "[0-9][0-9]")
			|| gmatch(b, "[0-9]"))
				return(mk(user, (struct anode *)atoi(b), (struct anode *)s));
			(void)fprintf(stderr, MSGSTR( NOUSER, "find: cannot find -user name\n"));
			exit(1);
		}
		return(mk(user, (struct anode *)i, (struct anode *)s));
	}
	else if(EQ(a, "-inum") || EQ(a, "-i"))
		return(mk(ino, (struct anode *)atoi(b), (struct anode *)s));
	else if(EQ(a, "-group")) {
		if((i=getunum(GGID, b)) == -1) {
			if(gmatch(b, "[0-9][0-9][0-9]*")
			|| gmatch(b, "[0-9][0-9]")
			|| gmatch(b, "[0-9]"))
				return(mk(group, (struct anode *)atoi(b), (struct anode *)s));
			(void)fprintf(stderr,MSGSTR( NOGRP, "find: cannot find -group name\n"));
			exit(1);
		}
		return(mk(group, (struct anode *)i, (struct anode *)s));
	} else if(EQ(a, "-size")) {
		/*   -size number => number is 512-bytes block
		 *   -size numberK or -size numberk => number is 1K-bytes block
		 *   -size numberc => number is expressed in bytes unit
		 */
		c = *b ? b[strlen(b) - 1] : '\0';
		switch (c) {
			case 'k':
			case 'K':
				Sizefactor = 1024;
				break;
			case 'c':
				Sizefactor = 1;
				break;
		}
		return(mk(size, (struct anode *)atoi(b), (struct anode *)s));
	}
	else if(EQ(a, "-links"))
		return(mk(links, (struct anode *)atoi(b), (struct anode *)s));
	else if(EQ(a, "-perm")) {
		if(*b=='-') ++b;
		for(i=0; *b ; ++b) {
			c = *b;
			if (!isdigit(c) || c == '8' || c == '9') {
				/*   keep "-perm" apart,
				 *   so it doesn't get translated
				 */
				(void)fprintf(stderr,MSGSTR( BADPERM,
				"find: an octal number is required after %s.\n"),
				"-perm");
				exit(1);
			}
			i <<= 3;
			i = i + (c - '0');
		}
		return(mk(perm, (struct anode *)i, (struct anode *)s));
	}
	else if(EQ(a, "-type")) {
		i = s=='d' ? S_IFDIR :
		    s=='b' ? S_IFBLK :
		    s=='c' ? S_IFCHR :
		    s=='p' ? S_IFIFO :
		    s=='f' ? S_IFREG :
		    s=='l' ? S_IFLNK :
#ifdef S_IFSOCK
		    s=='s' ? S_IFSOCK :
#endif /* S_IFSOCK */
		    0;
		return(mk(type, (struct anode *)i, (struct anode *)0));
	}
	else if(EQ(a, "-fstype")) {
		fstyped = 1;
		return(mk(fstype, (struct anode *)b, (struct anode *)0));
	}
	else if (EQ(a, "-exec")) {
		i = Ai - 1;
		while(!EQ(nxtarg(), ";"));
		return(mk(exeq, (struct anode *)i, (struct anode *)0));
	}
	else if (EQ(a, "-ok")) {
		i = Ai - 1;
		while(!EQ(nxtarg(), ";"));
		return(mk(ok, (struct anode *)i, (struct anode *)0));
	}
	else if(EQ(a, "-cpio")) {
		if((Cpio = creat(b, 0666)) < 0) {
			(void)fprintf(stderr,MSGSTR( NOCREATE, "find: cannot create %s\n"), b);
			exit(1);
		}
		SBuf = (short *)sbrk(CPIOBSZ);
		Wp = Dbuf = (short *)sbrk(Bufsize);
		depthf = 1;
		return(mk(cpio, (struct anode *)0, (struct anode *)0));
	}
	else if(EQ(a, "-ncpio")) {
		if((Cpio = creat(b, 0666)) < 0) {
			(void)fprintf(stderr,MSGSTR( NOCREATE, "find: cannot create %s\n"), b);
			exit(1);
		}
		Buf = (char*)sbrk(CPIOBSZ);
		Cp = Cbuf = (char *)sbrk(Bufsize);
		Cflag++;
		depthf = 1;
		return(mk(cpio, (struct anode *)0, (struct anode *)0));
	}
	else if(EQ(a, "-newer")) {
		if(stat(b, (struct stat *)&Statb) < 0) {
			(void)fprintf(stderr,MSGSTR(NOACCES, "find: cannot access %s\n"), b);
			exit(1);
		}
		Newer = Statb.st_mtime;
		return(mk(newer, (struct anode *)0, (struct anode *)0));
	}
/*** NID info to be removed or REPLACED
	else if(EQ(a, "-node")) {
		if((n=getnid(b)) == -1) {
			(void)fprintf(stderr,MSGSTR( BADNODE, "find: invalid node id %s\n"), b);
			exit(1);
		}
		return(mk(nnode, (struct anode *)n, (struct anode *)0));
	}
***/
err:	(void)fprintf(stderr,MSGSTR( BADOPTION, "find: bad option %s\n"), a);
	exit(1);
	/*NOTREACHED*/
	return((struct anode *)0);
}

static struct anode *
mk(f, l, r)
int (*f)();
struct anode *l, *r;
{
	Node[Nn].F = f;
	Node[Nn].L = l;
	Node[Nn].R = r;
	return(&(Node[Nn++]));
}

static char *
nxtarg()  /* get next arg from command line */
{
	static strikes = 0;

	if(strikes==3) {
		(void)fprintf(stderr,MSGSTR( INCSTATE, "find: incomplete statement\n"));
		exit(1);
	}
	if(Ai>=Argc) {
		strikes++;
		Ai = Argc + 1;
		return("");
	}
	return(Argv[Ai++]);
}

/* execution time functions */

static int
and(p)
register struct anode *p;
{
	return(((*p->L->F)(p->L)) && ((*p->R->F)(p->R))?1:0);
}

static int
or(p)
register struct anode *p;
{
	 return(((*p->L->F)(p->L)) || ((*p->R->F)(p->R))?1:0);
}

static int
not(p)
register struct anode *p;
{
	return( !((*p->L->F)(p->L)));
}

static int
glob(p)
register struct glob_str *p; 
{
	return(gmatch(Fname, p->pat));
}

static int
print()
{
	(void)puts(Pathname);
	return(1);
}

static int
mtime(p)
register struct mtime_str *p; 
{
	return(scomp((int)((Now - Statb.st_mtime) / A_DAY), p->t, p->s));
}

static int
atime(p)
register struct atime_str *p; 
{
	return(scomp((int)((Now - Statb.st_atime) / A_DAY), p->t, p->s));
}

static int
lctime(p)
register struct lctime_str *p; 
{
	return(scomp((int)((Now - Statb.st_ctime) / A_DAY), p->t, p->s));
}

static int
user(p)
register struct user_str *p; 
{
	return(scomp(Statb.st_uid, p->u, p->s));
}

static int
nouser(p)
struct anode *p;
{
        char *getname();

        return (getname(Statb.st_uid) == NULL);
}

static int
ino(p)
register struct ino_str *p;
{
	return(scomp((int)Statb.st_ino, p->u, p->s));
}

static int
group(p)
register struct group_str *p; 
{
	return(p->u == Statb.st_gid);
}

static int
nogroup(p)
struct anode *p;
{
        char *getgroup();

        return (getgroup(Statb.st_gid) == NULL);
}

static int
links(p)
register struct links_str *p; 
{
	return(scomp(Statb.st_nlink, p->link, p->s));
}

static int
size(p)
register struct size_str *p; 
{
	return(scomp((int)((Statb.st_size+(Sizefactor - 1))/Sizefactor), p->sz,
				p->s));
}

static int
perm(p)
register struct perm_str *p; 
{
	register i;
	i = (p->s=='-') ? p->per : 0740007777; /* '-' means only arg bits */
	return((Statb.st_mode & i & 0740007777) == p->per);
}

static int
type(p)
register struct type_str *p;
{
	return((Statb.st_mode&S_IFMT)==p->per);
}

static int
fstype(p)
register struct fstype_str *p;
{
  int typno = 0;
  int Fstype_arg = -1; /* type number of fs in option argument */
	
  while( typno <= MOUNT_MAXTYPE && strcmp(fstype_strngs[typno],p->fsname) )
    typno++;
  if(typno > MOUNT_MAXTYPE)
  {
    (void)fprintf(stderr,MSGSTR( BADFSTYPE, "find: %s is not a valid filesystem type.\n"), p->fsname);
    exit(2);
  }
  Fstype_arg = typno;
  return((Fstype == Fstype_arg));
}

static int
prune(p)
register struct prune_str *p;
{
	pruned = 1;
	return(1);
}

static int
exeq(p)
register struct exeq_str *p;
{
	(void)fflush(stdout); /* to flush possible `-print' */
	return(doex(p->com));
}

static int
ok(p)
struct ok_str *p;
{
	int yes=0;
	char c[35];

	(void)fflush(stdout); /* to flush possible `-print' */
	(void)fprintf(stderr,MSGSTR(OKPROMT,"< %s ... %s > (%s)?   "), 
					Argv[p->com], Pathname, ystr);
	(void)fflush(stderr);
	if (scanf("%20s", c) == EOF)
		exit(2);

	if (NLyesno(c) == 1) 
		yes = 1;
	return(yes? doex(p->com): 0);
}

#define MKSHORT(v, lv) {U.l=1L;if(U.c[0]) U.l=lv, v[0]=U.s[1], v[1]=U.s[0]; else U.l=lv, v[0]=U.s[0], v[1]=U.s[1];}
union { long l; short s[2]; char c[4]; } U;

static long
mklong(v)
short v[];
{
	U.l = 1;
	if(U.c[0] /* VAX */)
		U.s[0] = v[1], U.s[1] = v[0];
	else
		U.s[0] = v[0], U.s[1] = v[1];
	return U.l;
}

static int
crossdev()
{
	return(1);
}

static int
depth()
{
	return(1);
}

/*** NID info not available, to be REPLACEd or removed
static int
nnode(p)
register struct { int f, nid; } *p;
{
	return(p->nid == Statb.fst_nid);
}
***/

static int
cpio()
{
#define MAGIC 070707
#define HDRSIZE	(sizeof hdr - 256)
#define CHARS	76
	register ifile, ct;
	static long fsz;
	register i;

	(void)strcpy(hdr.h_name, !strncmp(Pathname, "./", 2)? Pathname+2: Pathname);
	hdr.h_magic = MAGIC;
	hdr.h_namesize = strlen(hdr.h_name) + 1;
	hdr.h_uid = Statb.st_uid;
	hdr.h_gid = Statb.st_gid;
	hdr.h_dev = Statb.st_dev;
	hdr.h_ino = Statb.st_ino;
	hdr.h_mode = Statb.st_mode;
	hdr.h_nlink = Statb.st_nlink;
	hdr.h_rdev = Statb.st_rdev;
	MKSHORT(hdr.h_mtime, Statb.st_mtime);
	fsz = (hdr.h_mode & S_IFMT) == S_IFREG? Statb.st_size: 0L;
	MKSHORT(hdr.h_filesize, fsz);

	if (Cflag)
		bintochar(fsz);

	if(EQ(hdr.h_name, "TRAILER!!!")) {
		Cflag? writehdr(Chdr, CHARS + hdr.h_namesize):
			bwrite((short *)&hdr, HDRSIZE + hdr.h_namesize);
		for (i = 0; i < 10; ++i)
			Cflag? writehdr(Buf, BUFSIZE): bwrite(SBuf, BUFSIZE);
		return;
	}
	if(!mklong(hdr.h_filesize)) {
		Cflag? writehdr(Chdr, CHARS + hdr.h_namesize):
			bwrite((short *)&hdr, HDRSIZE + hdr.h_namesize);
		return;
	}
	if((ifile = open(Fname, 0)) < 0) {
cerror:
		(void)fprintf(stderr,MSGSTR( NOCOPY, "find: Cannot copy %s.\n"), hdr.h_name);
		return;
	}
	Cflag? writehdr(Chdr, CHARS + hdr.h_namesize):
		bwrite((short *)&hdr, HDRSIZE+hdr.h_namesize);
	for(fsz = mklong(hdr.h_filesize); fsz > 0; fsz -= CPIOBSZ) {
		ct = fsz>CPIOBSZ? CPIOBSZ: fsz;
		if(read(ifile, Cflag? Buf: (char *)SBuf, ct) < 0)  {
			(void)fprintf(stderr,MSGSTR( NOREAD, "Cannot read %s.\n"), hdr.h_name);
			continue;
		}
		Cflag? writehdr(Buf, ct): bwrite(SBuf, ct);
	}
	(void)close(ifile);
	return;
}

static int
bintochar(t)
long t;
{
	(void)sprintf(Chdr, "%.6ho%.6ho%.6ho%.6ho%.6ho%.6ho%.6ho%.6ho%.11lo%.6ho%.11lo%s",
		MAGIC,Statb.st_dev,Statb.st_ino,Statb.st_mode,Statb.st_uid,
		Statb.st_gid,Statb.st_nlink,Statb.st_rdev & 00000177777,
		Statb.st_mtime,(short)strlen(hdr.h_name)+1,t,hdr.h_name);
}

static int
ls(p)
struct anode *p;
{
        (void)list(Pathname, &Statb);
        return (1);
}

static int
newer()
{
	return Statb.st_mtime > Newer;
}


static char *
getname(uid)
{
        register struct passwd *pw;
        static init;
        struct passwd *getpwent();

        if (uid >= 0 && uid < NGID && names[uid][0])
                return (&names[uid][0]);
        if (uid >= 0 && uid == outrangeuid)
                return (outrangename);
rescan:
        if (init == 2) {
                if (uid < NGID)
                        return (0);
                setpwent();
                while (pw = getpwent()) {
                        if (pw->pw_uid != uid)
                                continue;
                        outrangeuid = pw->pw_uid;
                        SCPYN(outrangename, pw->pw_name);
                        endpwent();
                        return (outrangename);
                }
                endpwent();
                return (0);
        }
        if (init == 0)
                setpwent(), init = 1;
        while (pw = getpwent()) {
                if ( pw->pw_uid >= NGID ) {
                        if (pw->pw_uid == uid) {
                                outrangeuid = pw->pw_uid;
                                SCPYN(outrangename, pw->pw_name);
                                return (outrangename);
                        }
                        continue;
                }
                if (names[pw->pw_uid][0])
                        continue;
                SCPYN(names[pw->pw_uid], pw->pw_name);
                if (pw->pw_uid == uid)
                        return (&names[uid][0]);
        }
        init = 2;
        goto rescan;
}


static char *
getgroup(gid)
{
        register struct group *gr;
        static init;
        struct group *getgrent();

        if (gid >= 0 && gid < NGID && groups[gid][0])
                return (&groups[gid][0]);
        if (gid >= 0 && gid == outrangegid)
                return (outrangegroup);
rescan:
        if (init == 2) {
                if (gid < NGID)
                        return (0);
                setgrent();
                while (gr = getgrent()) {
                        if (gr->gr_gid != gid)
                                continue;
                        outrangegid = gr->gr_gid;
                        SCPYN(outrangegroup, gr->gr_name);
                        endgrent();
                        return (outrangegroup);
                }
                endgrent();
                return (0);
        }
        if (init == 0)
                setgrent(), init = 1;
        while (gr = getgrent()) {
                if ( gr->gr_gid >= NGID ) {
                        if (gr->gr_gid == gid) {
                                outrangegid = gr->gr_gid;
                                SCPYN(outrangegroup, gr->gr_name);
                                return (outrangegroup);
                        }
                        continue;
                }
                if (groups[gr->gr_gid][0])
                        continue;
                SCPYN(groups[gr->gr_gid], gr->gr_name);
                if (gr->gr_gid == gid)
                        return (&groups[gid][0]);
        }
        init = 2;
        goto rescan;
}



/* support functions */
#define permoffset(who)         ((who) * 3)
#define permission(who, type)   ((type) >> permoffset(who))
#define kbytes(bytes)           (((bytes) + 1023) / 1024)

static int
list(file, stp)
        char *file;
        register struct stat *stp;
{
        char pmode[32], uname[32], gname[32], fsize[32], ftime[32];
        char *getname(), *getgroup();
	struct tm *localtime();
	char timbuf[BUFSIZ];
        static long special[] = { S_ISUID, 's', S_ISGID, 's', S_ISVTX, 't' };
        static time_t sixmonthsago = -1;
#ifdef  S_IFLNK
        char flink[PATH_MAX + 1];
#endif
        register int who;
        register char *cp;
        time_t now;

        if (file == NULL || stp == NULL)
                return (-1);

        (void)time(&now);
        if (sixmonthsago == -1)
                sixmonthsago = now - 6L*30L*24L*60L*60L;

        switch (stp->st_mode & S_IFMT) {
#ifdef  S_IFDIR
        case S_IFDIR:   /* directory */
                pmode[0] = 'd';
                break;
#endif
#ifdef  S_IFCHR
        case S_IFCHR:   /* character special */
                pmode[0] = 'c';
                break;
#endif
#ifdef  S_IFBLK
        case S_IFBLK:   /* block special */
                pmode[0] = 'b';
                break;
#endif
#ifdef  S_IFLNK
        case S_IFLNK:   /* symbolic link */
                pmode[0] = 'l';
                break;
#endif
#ifdef  S_IFSOCK
        case S_IFSOCK:  /* socket */
                pmode[0] = 's';
                break;
#endif
#ifdef  S_IFREG
        case S_IFREG:   /* regular */
#endif
        default:
                pmode[0] = '-';
                break;
        }

        for (who = 0; who < 3; who++) {
                if (stp->st_mode & permission(who, S_IREAD))
                        pmode[permoffset(who) + 1] = 'r';
                else
                        pmode[permoffset(who) + 1] = '-';

                if (stp->st_mode & permission(who, S_IWRITE))
                        pmode[permoffset(who) + 2] = 'w';
                else
                        pmode[permoffset(who) + 2] = '-';

                if (stp->st_mode & special[who * 2])
                        pmode[permoffset(who) + 3] = special[who * 2 + 1];
                else if (stp->st_mode & permission(who, S_IEXEC))
                        pmode[permoffset(who) + 3] = 'x';
                else
                        pmode[permoffset(who) + 3] = '-';
        }
        pmode[permoffset(who) + 1] = '\0';

        cp = getname(stp->st_uid);
        if (cp != NULL)
                (void)sprintf(uname, "%-9.9s", cp);
        else
                (void)sprintf(uname, "%-9d", stp->st_uid);

        cp = getgroup(stp->st_gid);
        if (cp != NULL)
                (void)sprintf(gname, "%-9.9s", cp);
        else
                (void)sprintf(gname, "%-9d", stp->st_gid);

        if (pmode[0] == 'b' || pmode[0] == 'c')
                (void)sprintf(fsize, "%3d,%4d",
                        major(stp->st_rdev), minor(stp->st_rdev));
        else {
                (void)sprintf(fsize, "%8ld", stp->st_size);
#ifdef  S_IFLNK
                if (pmode[0] == 'l') {
                        /*
                         * Need to get the tail of the file name, since we have
                         * already chdir()ed into the directory of the file
                         */
                        cp = strrchr(file, '/');
                        if (cp == NULL)
                                cp = file;
                        else
                                cp++;
                        who = readlink(cp, flink, sizeof flink - 1);
                        if (who >= 0)
                                flink[who] = '\0';
                        else
                                flink[0] = '\0';
                }
#endif
        }

        if (stp->st_mtime < sixmonthsago || stp->st_mtime > now)
		(void)strftime(ftime, 12, "%b %d %Y", localtime(&stp->st_mtime));
        else
		(void)strftime(ftime, 12, "%b %d %H:%M", localtime(&stp->st_mtime));
	ftime[12] = '\0';

        (void)printf("%5lu %4ld %s %2d %s%s%s %s %s%s%s\n",
                stp->st_ino,                            /* inode #      */
#ifdef  S_IFSOCK
                (long) kbytes(dbtob(stp->st_blocks)),   /* kbytes       */
#else
                (long) kbytes(stp->st_size),            /* kbytes       */
#endif
                pmode,                                  /* protection   */
                stp->st_nlink,                          /* # of links   */
                uname,                                  /* owner        */
                gname,                                  /* group        */
                fsize,                                  /* # of bytes   */
                ftime,                                  /* modify time  */
                file,                                   /* name         */
#ifdef  S_IFLNK
                (pmode[0] == 'l') ? " -> " : "",
                (pmode[0] == 'l') ? flink  : ""         /* symlink      */
#else
                "",
                ""
#endif
        );

        return (0);
}


static int
scomp(a, b, s) /* funny signed compare */
register a, b;
register char s;
{
	if(s == '+')
		return(a > b);
	if(s == '-')
		return(a < (b * -1));
	return(a == b);
}

static int
doex(com)
{
	register np;
	register char *na;
	static char *nargv[50];
	static ccode;
	static pid;

	ccode = np = 0;
	while (na=Argv[com++]) {
		if(strcmp(na, ";")==0) break;
		if(strcmp(na, "{}")==0) nargv[np++] = Pathname;
		else nargv[np++] = na;
	}
	nargv[np] = 0;
	if (np==0) return(9);
	if(pid = fork())
		while(wait(&ccode) != pid);
	else { /*child*/
		(void)chdir_access(Home);
		(void)execvp(nargv[0], nargv, np);
		/* PTM # 34450 */
 		(void)fprintf(stderr, "Find: Can't execute %s", nargv[0]);
 		(void)fflush(stdout);
 		perror(":");
		exit(1);
	}
	return(ccode ? 0:1);
}

static int
getunum(t, s)
int	t;
char	*s;
{
	register i;
	struct	passwd	*pw;
	struct	group	*gr;
#ifndef NO_PW_STAYOPEN
        extern int _pw_stayopen;

        _pw_stayopen = 1;
#endif
	i = -1;
	if( t == UUID )
	{
	  if( ((pw = getpwnam( s )) != (struct passwd *)NULL) 
              && pw != (struct passwd *)EOF )
			i = pw->pw_uid;
	} 
	else 
	{
	  if( ((gr = getgrnam( s )) != (struct group *)NULL) 
              && gr != (struct group *)EOF )
			i = gr->gr_gid;
	}
	return(i);
}

/**used for NID info*****************************************************/
char    valid_chars[] = "0123456789abcdefABCDEF";

static unsigned long
getnid(nodename)
char *nodename;
{
	unsigned long nid;
	register char *testp;   /* used in nodename validation          */
	char * strchr();
	int length;             /* length of nodename                   */
	int i;                  /* counter and error returns            */

	/* is this nodename actually a nickname?                        */

	/* NOT AVAILABLE 
	i = drsname(nodename,&nid);
	*/
	return (-1);

	/*NOTREACHED*/
	if (i == 0)
		return(nid);      /* nodename translated into node id   */

	/* validate characters in nodename against allowable characters
	   in hex representation
         */
	testp = (char *) ~0;
	if ((length = strlen(nodename)) != 8)
		return(-1);
	for (i=0; (i<length) && (testp); i++)
		testp = strchr( valid_chars, nodename[i] );
	if (!testp)
		return(-1);
	(void)sscanf(nodename, "%x", &nid);
	return(nid);
}
/************************************************************************/

static int
descend(name, fname, pfstype, pfsno, exlist)
	struct anode *exlist;
	char *name, *fname;
	int pfstype;		/* fstype  of parent dir */
	dev_t pfsno;		/* device number of parent dir */
{
	DIR	*dir = NULL;
	register struct dirent	*dp;
	register char *c1;
	int rv = 0;
	char *endofname;
	dev_t cfsno;
	int cfstype;

	if( lstat(fname,  &Statb)<0) {
		(void)fprintf(stderr,MSGSTR( BADSTAT, "find: bad status-- %s\n"), name);
		return(0);
	}
	cfsno = Statb.st_dev;
	if (fstyped) {
		if (cfsno != pfsno) {
			if( statfs(Fname, &Statfsb) < 0) {
				(void)fprintf(stderr,MSGSTR( BADSTAT, 
					"find: bad status-- %s\n"), Fname);
				return(0);
			}
			Fstype = Statfsb.f_type;
		} else
			Fstype = pfstype;
		cfstype = Fstype;
	}

	if (!depthf)
		(*exlist->F)(exlist);

	if (pruned) {
		pruned = 0;
		return(1);
	}

	if((Statb.st_mode&S_IFMT)!=S_IFDIR ||
	   !Xdev && Devstat.st_dev != Statb.st_dev){    
		if (depthf)
			(*exlist->F)(exlist);
		return(1);
		}              

	for (c1 = name; *c1; ++c1);
	if (*(c1-1) == '/')
		--c1;
	endofname = c1;
	if (chdir_access(fname) == -1) {
                (void)fprintf(stderr,MSGSTR(NOCHDIR,"find: Cannot change directory to %s.\n"),fname);
                perror("");
		return(0);
	}
	if ((dir = opendir(".")) == NULL) {
		(void)fprintf(stderr, MSGSTR(NOOPEN, "find: Cannot open %s.\n"), name);
		rv = 0;
		goto ret;
	}
	for (dp = readdir(dir); dp != NULL; dp = readdir(dir)) {
		if ((dp->d_name[0]=='.' && dp->d_name[1]=='\0') ||
		    (dp->d_name[0]=='.' && dp->d_name[1]=='.' && dp->d_name[2]=='\0'))
			continue;
		c1 = endofname;
		*c1++ = '/';
		(void)strcpy(c1, dp->d_name);
		Fname = endofname+1;
		if(!descend(name, Fname, cfstype, cfsno, exlist)) {
			*endofname = '\0';
			(void)chdir_access(Home);
			if(chdir_access(Pathname) == -1) {
				(void)fprintf(stderr,MSGSTR( BADTREE, 
					"find: bad directory tree\n"));
				exit(1);
			}
		}
	}
	rv = 1;
ret:
	if(dir)
		closedir(dir);
	c1 = endofname;
        *c1 = '\0';
	if(chdir_access("..") == -1) {
		*endofname = '\0';
		(void)fprintf(stderr, MSGSTR(BADDIR, "find: bad directory <%s>\n"), name);
		rv = 1;
	}
	if(depthf){
		if( lstat(fname, &Statb)<0) {
			(void)fprintf(stderr, MSGSTR(NOSTAT, "find: Cannot get status on %s.\n"), fname);
		}
		(*exlist->F)(exlist);
	}
	return(rv);
}


static int
gmatch(s, p)
register char *s, *p;
{
	register int c;
	register cc, okc, lc, scc;
	register cc1, scc1, lc1;

	wchar_t colval();

	scc = *s;
	if (NCisshift (scc)) 
	{
		scc1 = *++s;
		if (_NCdec2 (scc, scc1, scc) == 1) s--;
	}
	lc = 0xffff;
	switch (c = *p) {

	case '[':
		okc = 0;
		while (cc = *++p) {
			if (NCisshift (cc))
			{
				cc1 = *++p;
				if (_NCdec2 (cc, cc1, cc) == 1) p--;
			}
			switch (cc) {

			case ']':
				if (okc)
					return(gmatch(++s, ++p));
				else
					return(0);

			case '-':
				cc = *++p;
				if (NCisshift (cc)) 
				{
					cc1 = *++p;
					if (_NCdec2 (cc, cc1, cc) == 1) p--; 
				}
#    ifndef OLD
				if ( ((scc1 = colval(scc)) > 0) &&
					((lc1 = colval(lc)) > 0) &&
						((cc1 = colval(cc)) > 0) ) {
				       okc |= ( (lc1 <= scc1) && (scc1 <= cc1) );
				       break;
				}
				okc |= ((lc == scc) || (scc == cc));
				break;
#    else
				scc1 = NCcolval(scc);
				okc |= ((NCcolval(lc) <= scc1) && (scc1 <= NCcolval(cc)));
				break;
#    endif
			case '[':
				if (p[1] == ':')
				{
					if (!strncmp(p,"[:alpha:]",9)) {
					   okc |= (isascii(scc) && isalpha(scc));
					   p += 8;
					   break;
					   }
					if (!strncmp(p,"[:upper:]",9)) {
					   okc |= (isascii(scc) && isupper(scc));
					   p += 8;
					   break;
					   }
					if (!strncmp(p,"[:lower:]",9)) {
					   okc |= (isascii(scc) && islower(scc));
					   p += 8;
					   break;
					   }
					if (!strncmp(p,"[:digit:]",9)) {
					   okc |= (isascii(scc) && isdigit(scc));
					   p += 8;
					   break;
					   }
					if (!strncmp(p,"[:alnum:]",9)) {
					   okc |= (isascii(scc) && isalnum(scc));
					   p += 8;
					   break;
					   }
					if (!strncmp(p,"[:print:]",9)) {
					   okc |= (isascii(scc) && isprint(scc));
					   p += 8;
					   break;
					   }
#ifdef KJI
					if (!strncmp(p,"[:jalpha:]",10)) {
					   okc |= isjalpha(scc);
					   p += 9;
					   break;
					   }
					if (!strncmp(p,"[:jdigit:]",10)) {
					   okc |= isjdigit(scc);
					   p += 9;
					   break;
					   }
					if (!strncmp(p,"[:jpunct:]",10)) {
					   okc |= isjpunct(scc);
					   p += 9;
					   break;
					   }
					if (!strncmp(p,"[:jparen:]",10)) {
					   okc |= isjparen(scc);
					   p += 9;
					   break;
					   }
					if (!strncmp(p,"[:jkanji:]",10)) {
					   okc |= isjkanji(scc);
					   p += 9;
					   break;
					   }
					if (!strncmp(p,"[:jhira:]",9)) {
					   okc |= isjhira(scc);
					   p += 8;
					   break;
					   }
					if (!strncmp(p,"[:jkata:]",9)) {
					   okc |= isjkata(scc);
					   p += 8;
					   break;
					   }
#endif
				}
			}
			if (scc==(lc=cc)) okc++;
		}
		return(0);

	case '?':
	caseq:
		if(scc) return(gmatch(++s, ++p));
		return(0);
	case '*':
		return(umatch(s, ++p));
	case 0:
		return(!scc);
	}
	if (NCisshift (c)) 
	{
		cc1 = *++p;
		if (_NCdec2 (c, cc1, c) == 1) p--;
	}
	if (c==scc) goto caseq;
	return(0);
}


static int
umatch(s, p)
register char *s, *p;
{
		register int scc;
	if(*p==0) return(1);
	while(*s)
		if (gmatch(s++,p))
		{
			scc = *s++;
			if (NCisshift (scc))
				if (_NCdec2 (scc, *s, scc) == 1) s--;
			return(1);
		}
	return(0);
}

static int
bwrite(rp, c)
register short *rp;
register c;
{
	register short *wp = Wp;

	c = (c+1) >> 1;
	while(c--) {
		if(!Wct) {
again:
			if(write(Cpio, (char *)Dbuf, Bufsize)<0) {
				Cpio = chgreel(1, Cpio);
				goto again;
			}
			Wct = Bufsize >> 1;
			wp = Dbuf;
			++Blocks;
		}
		*wp++ = *rp++;
		--Wct;
	}
	Wp = wp;
}

static int
writehdr(rp, c)
register char *rp;
register c;
{
	register char *cp = Cp;

	while (c--)  {
		if (!Cct)  {
again:
			if(write(Cpio, Cbuf, Bufsize) < 0)  {
				Cpio = chgreel(1, Cpio);
				goto again;
			}
			Cct = Bufsize;
			cp = Cbuf;
			++Blocks;
		}
		*cp++ = *rp++;
		--Cct;
	}
	Cp = cp;
}

static int
chgreel(x, fl)
{
	register f;
	char str[22];
	FILE *devtty;
	struct stat statb;

	(void)fprintf(stderr,( x ? MSGSTR( WRTOUT, "find: Cannot write output.\n") 
		      	   : MSGSTR( READIN, "find: Cannot read input.\n")));

	(void)fstat(fl, &statb);
	if((statb.st_mode&S_IFMT) != S_IFCHR)
		exit(1);
again:
	(void)fprintf(stderr,MSGSTR( ASKDEV, "If you want to go on, type device/file name when ready\n"));
	devtty = fopen("/dev/tty", "r");
	(void)fgets(str, 20, devtty);
	str[strlen(str) - 1] = '\0';
	if(!*str)
		exit(1);
	(void)close(fl);
	if((f = open(str, x? 1: 0)) < 0) {
		(void)fprintf(stderr,MSGSTR( NOGOOD, "That didn't work"));
		(void)fclose(devtty);
		goto again;
	}
	return f;
}

static wchar_t
colval(ch)
wchar_t ch;
{
	char ifbuf[16];
	char *ib;
	int cvalue, uvalue;

	ib = ifbuf;
	if (ch > 256)
		*ib++ = (ch >> 8);
	*ib++ = (ch & 0xff);	
	*ib = '\0';
	uvalue = NCcoluniq(ch);
	if ( ((cvalue = NCcollate(ch)) < 0) &&
	      (cvalue = _NLxcolu(cvalue, &ib, 0, &uvalue)) ) ;
	return (uvalue);
}

/*
 * Make sure the directory can be read before actually doing the cd
 */
static int
chdir_access(directory)
        char *directory;
{
        DIR *dir_desc;
        struct dirent *dir_pointer;
        int saved_errno;

        if ((dir_desc = opendir(directory)) == NULL) 
        	return (-1);

        dir_pointer = readdir(dir_desc);
       	saved_errno = errno;
        closedir(dir_desc);

        if (dir_pointer == NULL) {
                errno = saved_errno;
                return (-1);
        }
        return (chdir(directory));
}

