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
static char rcsid[] = "@(#)$RCSfile: tar.c,v $ $Revision: 1.10 $ (OSF) $Date: 1994/11/19 01:41:15 $";
#endif

/*
 * COMPONENT_NAME: (CMDARCH) archive files
 *
 * FUNCTIONS: tar
 *
 * ORIGINS: 26, 27
 *
 * "tar.c	1.29  com/cmd/arch,3.1,9021 4/24/90 12:58:05";
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1990
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */
/* tar.c	5.1 15:44:05 8/16/90 SecureWare */

#include <sys/secdefines.h>
#if SEC_BASE
#include <sys/security.h>
#include <prot.h>
#endif

#define OSF1_ADFS	1
#include <stdio.h>
#include <sys/stat.h>
#include <sys/limits.h>
#include <tar.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mtio.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <ctype.h>
#include <locale.h>
#include <nl_types.h>
#include "tar_msg.h"

#ifdef PFS
#include <nx.h>
long _emin1();
#endif

#define MSGSTR(num,str)	catgets(catd,MS_TAR,num,str)
nl_catd	catd;		/*  message catalog descriptor  */

#define TBLOCK		512	/*  tar block size  */
#define NBLOCK		20	/*  default num of blocks  */
#define	TAR_PATH_LEN	257	/*  max size of tar archive path with \0  */
#define NAME_SIZE	100	/*  file name size in tar header  */
#define	PREFIX_SIZE	155	/*  path size in tar header  */
#define SPEC_MAGIC	"1234"


#define	writetape(b)	writetbuf(b, 1)
#define	min(a,b)  ((a) < (b) ? (a) : (b))
#define	max(a,b)  ((a) > (b) ? (a) : (b))

			/*  hblock modified for POSIX compliance  */
#define UNAMELEN 32
#define GNAMELEN 32

char *cur_wd;           /* Current working directory */

#ifdef	PFS
#define	PFS_CHKSUM_MAGIC	0x3e9	/* meaningless number for checksum */

/*
 * Macros for manipulating extended offsets and sizes.
 */

/*
 * LESS:
 *	Returns TRUE if extended number e1 is less than extended number e2.
 *
 */
#define LESS(e1, e2)	(_ecmp((e1), (e2)) < 0)

/*
 * GREATER:
 *	Returns TRUE if extended number e1 is greater than extended number e2.
 *
 */
#define GREATER(e1, e2)	(_ecmp((e1), (e2)) > 0)

/*
 * EQUAL:
 *	Returns TRUE if extended number e1 is equal to extended number e2.
 *
 */
#define EQUAL(e1, e2)	(_ecmp((e1), (e2)) == 0)

/*
 * EMIN:
 *	The extended minimum of the two given extended numbers.
 */
#define EMIN(e1, e2)	((LESS((e1), (e2))) ? (e1) : (e2))

/*
 * EMAX:
 *	The extended maximum of the two given extended numbers.
 */
#define EMAX(e1, e2)	((GREATER((e1), (e2))) ? (e1) : (e2))

#define stat(a,b)	_estat(a,b)
#define lstat(a,b)	_lestat(a,b)
#define fstat(a,b)	_festat(a,b)

esize_t	ezero		= { 0, 0 };		/* extended zero */
esize_t	eerror		= { -1, -1 };		/* extended -1 */
esize_t eint_max	= { INT_MAX, 0 };	/* extended 2G-1 */
esize_t etar_max	= { -1, 65535 };	/* determined by largest ASCII
						   hex string that can fit in
						   size field of tar header
						   (= ffffffffffff) */
int	extended_file	= 0;			/* is this file >2G-1 bytes? */

extern	esize_t	_xstoe();
extern	long	_etoxs();
#endif	PFS

union hblock {
	char dummy[TBLOCK];
	struct header {
		char name[NAME_SIZE];
		char mode[8];
		char uid[8];
		char gid[8];
		char size[12];
		char mtime[12];
		char chksum[8];
		char typeflag;
		char linkname[NAME_SIZE];
		char magic[6];
		char version[2];
		char uname[UNAMELEN];
		char gname[GNAMELEN];
		char devmajor[8];
		char devminor[8];
		char prefix[PREFIX_SIZE];
	} dbuf;
};

struct linkbuf {
	ino_t	inum;
	dev_t	devnum;
	int	count;
	char	pathname[NAME_SIZE+1];		/*  +1 to allow for \0  */
struct	linkbuf *nextp;
};

struct elem {
	struct elem *next;
	char *data;
};

struct list {
	struct elem *first;
	struct elem *last;
};

struct list except_list;
int	writingdir;
char	*stripstring;
char	*stripprefix();
char	*list_first();

union	hblock dblock;
int     device_node;
union	hblock *tbuf;
struct	linkbuf *ihead;
#ifdef PFS
struct	estat stbuf;
#else
struct	stat stbuf;
#endif
			/* tar functions:			*/
int	cflag;		/*	Write to archive - create	*/
int	rflag;		/*	Write to archive - append	*/
int	tflag;		/*	List contents of archive - table*/
int	xflag;		/*	Xtract from archive		*/
			/* tar options:				*/
int	bflag;		/*	blocking factor			*/
int	fflag;		/*	name of archive file		*/
int	hflag;		/*	follow symbolic links		*/
int	iflag;		/*	ignore checksum errors		*/
int	mflag;		/*	don't restore modification times*/
int	oflag;		/*	don't archive directory info	*/
			/*	 xtract: don't restore uid/gid 	*/
int	pflag;		/*	restore orig file modes		*/
int	sflag;		/*	strip leading "/" if present - xtract */
int	uflag;		/*  add files to archive if not there */
int	vflag;		/*	verbose info			*/
int	wflag;		/*	wait for user confirmation 	*/
int	Bflag;		/*	force blocking to blocking factor */
#ifdef	PFS
int	Eflag;		/*	allow extended ( >2G-1 bytes in size) files */
#endif
int	Lflag;		/*	create symlink if hard link fails - xtract */
int	Fflag;		/*	filter out certain file names 	*/
int	Sflag;		/*	size of tape			*/
int	Pflag;		/*	strip specified prefix		*/

int	mt;
int	term;
long	trecs;		/*  number of records written to current volume  */
int	recno;
int	first;
int	prtlinkerr;
int	freemem = 1;
int	nblock = 0;
char	startcwd[PATH_MAX];
char	full_name_buf[TAR_PATH_LEN+1];	/* extra char for trailing '/' for directories */
char	*full_name = full_name_buf;
size_t	name_size = NAME_SIZE-1;  /* Actual size of names w/o null at end */
int	dev_min, dev_maj;
int	multivol;

int	onintr(void);
int	onquit(void);
int	onhup(void);
int	onterm(void);

daddr_t	low;
daddr_t	high;
daddr_t	bsrch();

				/*  tape info defaults  */
#define GAP	7500          /* Interrecord gap, .0001 inch units */
#define	DENSITY	1200
#define	TAPE_FOOT	110000L		/*  11 inches  */
int	density = DENSITY ;
int	tapelen ;
int	bptape ;
int	rptape ;

FILE	*vfile = stdout;
FILE	*tfile;

char	tname[] = "/tmp/tarXXXXXX";
char	usefile [PATH_MAX+1] = "/dev/rmt8";	/*  +1 for \0 char  */

extern	void	*malloc();
extern	char	*getwd();
extern	long	time();
extern	char	*mktemp();

size_t		strftime();
struct tm	*localtime();

char		*getpwd();
char		*getmem();


main(argc, argv)
int	argc;
char	*argv[];
{
	char		*cp;

	(void) setlocale(LC_ALL, "");
	catd = catopen(MF_TAR,0);

	if (argc < 2)
		usage();

#if SEC_BASE
	ie_init(argc, argv, 0);
#endif
	list_empty(&except_list);
	tfile = NULL;
	argv[argc] = 0;
	argv++;

	/*
	 * Save the original working directory, before it gets changed.
	 */

	cur_wd=getcwd(NULL, 256);

	for (cp = *argv++; *cp; cp++) 
		switch(*cp) {

		case 'o':
			oflag++;
			name_size = NAME_SIZE - 1;
			break;

		case 'n':
			/*
			 * older versions of tar expect null-terminated strings
			 * in the name and linkname fields.  POSIX specifies
			 * that names & linknames will not be terminated by a
			 * null if the string fits exactly in the header array.
			 * This option allows archives to be created with this
			 * new POSIX "feature".  The 'n' & 'o' options are
			 * mutually exclusive.
			 */
			oflag = 0;
			name_size = NAME_SIZE;
			break;

		case 's':
			sflag++;
			break;

		case 'f':
			if (! *argv) {
				fprintf(stderr, MSGSTR(EFFLAG,
			"tar: file must be specified with 'f' option\n"));
				usage();
			}
			if (strlen(*argv) > PATH_MAX) {
				fprintf(stderr, MSGSTR(ELONGFN,
					"tar: %s: file name too long\n"),*argv);
				exit(1);
			}
			strcpy(usefile, *argv++);
			fflag++;
			break;

		case 'c':
			cflag++;
			rflag++;
#ifdef PFS		/* default # of blks a 90m DAT holds */
			bptape=3980400;
			Sflag++;
#endif
			break;

		case 'p':
			pflag++;
			break;
		
		case 'u':
			mktemp(tname);
			if ((tfile = fopen(tname, "w")) == NULL) {
				fprintf(stderr, MSGSTR(ETCRTMP,
				 "tar: cannot create temporary file (%s)\n"),
				 tname);
				done(1);
			}
			uflag++;
			fprintf(tfile, "!!!!!/!/!/!/!/!/!/! 000\n");
			/*FALL THRU*/

		case 'r':
			rflag++;
			break;

		case 'v':
			vflag++;
			break;

		case 'w':
			wflag++;
			break;

		case 'x':
			xflag++;
			break;

		case 't':
			tflag++;
			break;

		case 'm':
			mflag++;
			break;

		case '-':
			break;

		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			usefile[strlen(usefile)-1] = *cp;
			break;

		case 'b':
			if (! *argv) {
				fprintf(stderr, MSGSTR(ETNOBSIZE,
			"tar: blocksize must be specified with 'b' option\n"));
				usage();
			}
			nblock = atoi(*argv);
			if (nblock <= 0 || nblock > (UINT_MAX / TBLOCK)) {
				fprintf(stderr, MSGSTR ( EBSIZE,
				    "tar: invalid blocksize \"%s\"\n" ), *argv);
				done(1);
			}
		 	argv++;
			bflag++;
			break;

		case 'l':
			prtlinkerr++;
			break;

		case 'h':
			hflag++;
			break;

		case 'i':
			iflag++;
			break;

		case 'B':
			Bflag++;
			break;

		case 'F':
			Fflag++;
			break;

#ifdef	PFS
		case 'E':
			Eflag++;
			break;
#endif
			
		case 'L':
			Lflag++;
			break;

		case 'P':	
			if (! *argv) {
				fprintf(stderr, MSGSTR(ETNOPREFIX,
			"tar: prefix must be specified with 'P' option\n"));
				usage();
			}
			stripstring = *argv++;
			Pflag++;
			break;

		case 'S':
			if (! *argv) {
				fprintf(stderr, MSGSTR(ETNOTAPESIZE,
			"tar: tape size must be specified with 'S' option\n"));
				usage();
			}
			{
			int	len;
			char	*mark;
			len = strlen(*argv);
			if ((*argv)[--len] == 'b') {
				*argv[len] = '\0';
				bptape = atoi(*argv);
			}
			else {
				tapelen = atoi(*argv);
				mark = strchr(*argv, '@');
				if (mark)
					density = atoi(++mark);
			}
			}
			argv++;
			Sflag++;
			break;

		default:
			usage();
		}

	if (!rflag && !xflag && !tflag)
		usage();
	if (rflag + tflag + xflag > 1)
		usage();
	if ( mflag && tflag )
		usage();
	if ( (prtlinkerr && !cflag) && (prtlinkerr && !rflag)
			 && (prtlinkerr && !uflag) )
				usage();
	if (rflag) {
		if (cflag && tfile != NULL)
			usage();
		if (signal(SIGINT, SIG_IGN) != SIG_IGN)
			(void) signal(SIGINT, (void (*)(int))onintr);
		if (signal(SIGHUP, SIG_IGN) != SIG_IGN)
			(void) signal(SIGHUP, (void (*)(int))onhup);
		if (signal(SIGQUIT, SIG_IGN) != SIG_IGN)
			(void) signal(SIGQUIT, (void (*)(int))onquit);
		if (signal(SIGTERM, SIG_IGN) != SIG_IGN)
			(void) signal(SIGTERM, (void (*)(int))onterm);
		mt = openmt(usefile, 1);
		dorep(argv);
		done(0);
	}
	mt = openmt(usefile, 0);

	if ( xflag )
		doxtract(argv);
	else
		dotable(argv);

	/* read second null block if standard input */
	if (strcmp(usefile, "-") == 0)
		readtape((char *)&dblock);

	done(0);
}

usage()
{
	fprintf(stderr, MSGSTR(USAGE1,
	  "usage: tar [-]{crtux}[bfhilmnopsvwBLFPS0-9] [blocking] [tarfile]"));
	fprintf(stderr, MSGSTR(USAGE2,
	  " [prefix]\n\t\t\t\t\t    [feet[@density]|<blocks>b]\n"));
	fprintf(stderr, MSGSTR(USAGE3,
	  "\t\t\t\t\t    [-C directory] [-e except] file ...\n"));
	done(1);
}

int
openmt(tape, writing)
	char *tape;
	int writing;
{
#ifdef PFS
	struct estat sb;
#else
	struct stat sb;
#endif

	if (strcmp(tape, "-") == 0) {
		/*
		 * Read from standard input or write to standard output.
		 */
#if SEC_BASE
		if (writing)
		    fprintf(stderr, MSGSTR(ESECSTDOUT,
			"tar: cannot write archives to standard output\n"));
		else
		    fprintf(stderr, MSGSTR(ESECSTDIN,
			"tar: cannot read archives from standard input\n"));
		fprintf(stderr, MSGSTR(ESECUSEF,
			"\tplease use -f tapefile option\n"));
		done(1);
#else
		if (writing) {
			if (cflag == 0) {
				fprintf(stderr, MSGSTR( ESTDOUT,
			 "tar: can only create standard output archives\n"));
				done(1);
			}
			vfile = stderr;
			setlinebuf(vfile);
			mt = dup(1);
		} else {
			mt = dup(0);
			Bflag++;
		}
		multivol = 0;	/* Disable multi-vol processing */
#endif
	} else {
		/*
		 * Use file or tape on local machine.
		 */
#if SEC_BASE
		stopio(tape);
		ie_check_device(tape,
			writing ? AUTH_DEV_EXPORT : AUTH_DEV_IMPORT);
#endif
		if (writing) {
			if (cflag)
				mt = open(tape, O_RDWR|O_CREAT|O_TRUNC, 0666);
			else
				mt = open(tape, O_RDWR);
		} else
			mt = open(tape, O_RDONLY);
		if (mt < 0) {
			fprintf(stderr, MSGSTR(ETOPEN, "tar: cannot open "));
			perror(tape);
			done(1);
		}
		multivol = 1;
		if (*tape != '/')	/* Save cwd for multi-vol re-opens */
			(void) getpwd(startcwd);
	}
	if (multivol) {
		if (fstat(mt, &sb) != 0) {
			fprintf(stderr, MSGSTR(ESTAT,
				"tar: could not stat file = %s\n"), tape);
			done(1);
		}
		/* Only character devices are allowed multi-vol processing */
		multivol = S_ISCHR(sb.st_mode);
	}
	return(mt);
}


/*	dorep will create a new archive or append to an existing archive.
 */

dorep(argv)
	char *argv[];
{
	register char *cp2;
	char wdir[PATH_MAX], tempdir[PATH_MAX], *parent;
	int skip = 0;

	if (!cflag) {		/*  request is update or replace  */
		(void) getdir();
		do {
			passtape();
			if (term)
				done(0);
			(void) getdir();
		} while (!endtape());
		backtape();
		if (tfile != NULL) {
			char buf[200];

			sprintf(buf,
"sort +0 -1 +1nr %s -o %s; awk '$1 != prev {print; prev=$1}' %s >%sX; mv %sX %s",
				tname, tname, tname, tname, tname, tname);
			fflush(tfile);
			system(buf);
			freopen(tname, "r", tfile);
			fstat((int)fileno(tfile), &stbuf);
#ifdef	PFS
			high = stbuf.st_size.slow;
#else
			high = stbuf.st_size;
#endif
		}
	}

	(void) getpwd(wdir);
	while (*argv && ! term) {
		if (!strcmp(*argv, "-C") && argv[1]) {
			argv++;
			skip = 0;
			if (chdir(*argv) < 0) {
				fprintf(stderr, MSGSTR(ECHDIR,
					"tar: can't change directories to "));
				perror(*argv);
				skip++;
			} else
				(void) getpwd(wdir);
			argv++;
			continue;
		}
		if (!strcmp(*argv, "-e")) {
			argv++;
			if (*argv) {
				list_append(&except_list, *argv);
				argv++;
			}
			continue;
		}
		if (skip && **argv != '/') {
			/*
			 * This arg was meant to apply relative to a current
			 * directory set by "-C <dir>", but the chdir() failed.
			 */
			argv++;
			continue;
		}

		parent = wdir;
		cp2 = rindex(*argv, '/');
		if (!cp2)		/* pathname consists only of filename*/
			cp2 = *argv;
		else if (cp2 != *argv) {	/* there is a path prefix */
			*cp2 = '\0';
			if (chdir(*argv) < 0) {
				fprintf(stderr, MSGSTR(ECHDIR,
					"tar: can't change directories to "));
				perror(*argv);
				continue;
			}
			*cp2 = '/';
			cp2++;
			parent = getpwd(tempdir);
		}
		else {				/* path is / or /dir */
			if (chdir("/") < 0) {
				fprintf(stderr, MSGSTR(ECHDIR,
					"tar: can't change directories to "));
				perror("/");
				continue;
			}
			cp2++;
			parent = "/";
		}

		if (*cp2 == '\0')	/* Handle pathnames ending with '/' */
			cp2="." ;

		putfile(*argv++, cp2, parent);
		if (chdir(wdir) < 0) {
			fprintf(stderr, MSGSTR( ECHBACK,
				"tar: cannot change back?: "));
			perror(wdir);
		}
	}
	putempty();
	putempty();
	if (recno > 0)		/* flush any remaining buffers */
		bwrite((char *) tbuf);
	if (prtlinkerr == 0)
		return;
	for (; ihead != NULL; ihead = ihead->nextp) {
		if (ihead->count == 0)
			continue;
		fprintf(stderr, MSGSTR( ELINKS,
			"tar: missing links to %s\n" ), ihead->pathname);
	}
}

endtape()
{
	return (dblock.dbuf.name[0] == '\0');
}


	/*	The function getdir will read a header from the archive
	 *	file. A POSIX header will contain all needed information
	 *	in the correct fields. Non POSIX headers need to be
	 *	checked and completed (e.g. major and minor device # in
	 *	the mtime field).
	 */
getdir()
{
#ifdef PFS
	register struct estat	*sp = &stbuf;
#else
	register struct stat *sp = &stbuf;
#endif
	int i, ftype;
	int	chksum;

top:
	readtape((char *)&dblock);
	if (dblock.dbuf.name[0] == '\0')
		return;

#ifdef	PFS
	extended_file = 0;
#endif
	sscanf(dblock.dbuf.chksum, "%0o", &chksum);
	if (chksum != (i = checksum())) {
#ifdef	PFS
		/*
		 * If this header is using the PFS extended format, proceed.
		 */
		if (chksum == pfs_checksum())
			extended_file++;
		else {
#endif
		fprintf(stderr, MSGSTR( ECSUMC,
			"tar: directory checksum error (%d != %d)\n" ),
		    chksum, i);
		if (iflag)		/*  ignore checksum errors */
			goto top;
		done(2);
#ifdef	PFS
		}
#endif
	}

	sscanf(dblock.dbuf.mode, "%0o", &i);
	sp->st_mode = i;

	/* We need to do the following for POSIX which requires 'typeflag'
	   be used for setting the S_IFMT part of the mode bits */

	switch(dblock.dbuf.typeflag) {
		case DIRTYPE:
			sp->st_mode = ((sp->st_mode & 07777) | S_IFDIR);
			break;
		case FIFOTYPE:
			sp->st_mode = ((sp->st_mode & 07777) | S_IFIFO);
			break;
		case CHRTYPE:
			sp->st_mode = ((sp->st_mode & 07777) | S_IFCHR);
			break;
		case BLKTYPE:
			sp->st_mode = ((sp->st_mode & 07777) | S_IFBLK);
			break;
		case LNKTYPE:
			sp->st_mode = ((sp->st_mode & 07777) | S_IFREG);
			break;
		case SYMTYPE:
			sp->st_mode = ((sp->st_mode & 07777) | S_IFLNK);
			break;
		case REGTYPE:
			sp->st_mode = ((sp->st_mode & 07777) | S_IFREG);
			break;
		default:
			break;
	}

				/*  check for posix header  */
	bzero(full_name_buf, sizeof(full_name_buf));
	full_name = full_name_buf;
	if ( ! strncmp ( dblock.dbuf.magic , TMAGIC , (size_t)TMAGLEN )) {
		struct passwd	*pw_entry ;
		struct group	*gw_entry ;

		if (dblock.dbuf.prefix[0]) {	/*  use prefix + '/' + name  */
			strncpy(full_name, dblock.dbuf.prefix,
				(size_t)PREFIX_SIZE);
			i = strlen(full_name);
			full_name[i++] = '/';
		} else
			i = 0;
		strncpy(&full_name[i], dblock.dbuf.name, (size_t)NAME_SIZE);
		if (dblock.dbuf.typeflag == DIRTYPE) {
			i = strlen(full_name);
			if (full_name[i-1] != '/') {
				full_name[i++] = '/';
				full_name[i] = '\0';
			}
		}

			/*  set uid and gid based on uname and gname */
		if (pw_entry = getpwnam ( dblock.dbuf.uname )) {
			sp->st_uid = pw_entry->pw_uid ;
		}
		else {
			sscanf(dblock.dbuf.uid, "%0o", &i);
			sp->st_uid = i;
		}
		if (gw_entry = getgrnam ( dblock.dbuf.gname )) {
			sp->st_gid = gw_entry->gr_gid ;
		}
		else {
			sscanf(dblock.dbuf.gid, "%0o", &i);
			sp->st_gid = i;
		}
		sscanf(dblock.dbuf.mtime, "%0lo", &sp->st_mtime);
	}
	else {
				/*  older archive file  */
		full_name = dblock.dbuf.name;	/* already null-terminated */
		sscanf(dblock.dbuf.uid, "%0o", &i);
		sp->st_uid = i;
		sscanf(dblock.dbuf.gid, "%0o", &i);
		sp->st_gid = i;

				/*  if special file then decode major
				    and minor number in mtime  */
		sscanf(dblock.dbuf.mtime, "%0lo", &sp->st_mtime);
		ftype = sp->st_mode & S_IFMT ;
		switch ( ftype ) {
			case S_IFIFO :
			case S_IFBLK :
			case S_IFCHR : {
				int	val ;
					/*  place major and minor num
					 *  in dblock header
					 */
				val = ((sp->st_mtime >> 8) & 0xff00) |
					(sp->st_mtime & 0xff);
				sprintf(dblock.dbuf.devminor, "%06o " , val);
				val = (sp->st_mtime >> 8) & 0xff;
				sprintf(dblock.dbuf.devmajor, "%06o " , val);
				break ;
				}

			default :
				/* Required for POSIX */
				sprintf(dblock.dbuf.devminor, "%06o " , 0);
				sprintf(dblock.dbuf.devmajor, "%06o " , 0);
				break;
		}

	}

#ifdef PFS
	if (extended_file) {
		char	s[24];

		/* get a null-terminated copy of the file size string */
		strncpy(s, dblock.dbuf.size, sizeof(dblock.dbuf.size));
		s[sizeof(dblock.dbuf.size)] = '\0';

		/* convert from ASCII hex to an extended number */
		sp->st_size = _xstoe(s);
		if (EQUAL(sp->st_size, eerror)) {
			fprintf(stderr,
				MSGSTR(EMATH, "tar: extended math failure\n"));
			exit(2);
		}
	} else {
		sp->st_size.shigh = 0;
		sscanf(dblock.dbuf.size, "%0lo", &sp->st_size.slow);
	}
#else
	sscanf(dblock.dbuf.size, "%0lo", &sp->st_size);
#endif

	if (Pflag) {
		full_name = stripprefix(full_name);
		if (!*full_name)
			goto top;
	}
	/* strip off leading "/" if present */
	if (sflag && full_name[0] == '/') {
		while (*full_name && *full_name == '/')
			++full_name;
		if (!*full_name)
			goto top;
	}

	if (tfile != NULL)
		fprintf(tfile, "%s %s\n", full_name, dblock.dbuf.mtime);

	return (TRUE) ;
}

passtape()
{
#ifdef PFS
	esize_t blocks;
#else
	long blocks;
#endif
	char *bufp;

	switch (dblock.dbuf.typeflag) {
		case LNKTYPE:
		case SYMTYPE:
		case DIRTYPE:
		case CHRTYPE:
		case BLKTYPE:
		case FIFOTYPE:
			/* These types have no data blocks on archive */
			return;
	}
	blocks = stbuf.st_size;
#ifdef PFS
	blocks = _eadd1(ezero, _ediv(_eadd1(stbuf.st_size, TBLOCK-1), TBLOCK));

	while (GREATER(blocks, ezero)){
		blocks = _esub1(blocks,1);
		(void) readtbuf(&bufp, TBLOCK);
	}
#else
	blocks += TBLOCK-1;
	blocks /= TBLOCK;

	while (blocks-- > 0)
		(void) readtbuf(&bufp, TBLOCK);
#endif
}

putfile(longname, shortname, parent)
	char *longname;
	char *shortname;
	char *parent;
{
	int infile = 0;
#ifdef PFS
	esize_t blocks;
	esize_t	ename_size;
#else
	long blocks;
#endif
	char buf[TBLOCK];
	char *bigbuf;
	register char *cp;
	struct dirent *dp;
	DIR *dirp;
	register int i;
	long l;
	char newparent[PATH_MAX];
	extern int errno;
	int	maxread;
	int	hint;		/* amount to write to get "in sync" */
	int	update = 1;
	struct list list;
	char *elem;

	if (excepted(longname)) {
		if (vflag)
			fprintf(vfile, "except %s\n", longname);
		return;
	}
	if (Pflag && !writingdir)
		longname = stripprefix(longname);

	if (!hflag)
		i = lstat(shortname, &stbuf);
	else
		i = stat(shortname, &stbuf);
	if (i < 0) {
		fprintf(stderr, "tar: ");
		perror(longname);
		return;
	}
	if (tfile != NULL && checkupdate(longname) == 0)
		/* Even if longname is up-to-date, need to recurse if dir */
		if ((stbuf.st_mode & S_IFMT) == S_IFDIR)
			update = 0;
		else
			return;
	if (update && checkw('r', longname) == 0)
		return;
	if (Fflag && checkf(shortname, stbuf.st_mode) == 0)
		return;
#if SEC_ARCH
	if (!ie_sl_export(longname, (stbuf.st_mode & S_IFMT) != S_IFLNK))
		return;
#endif

	switch (stbuf.st_mode & S_IFMT) {
	case S_IFDIR:
		for (i = 0, cp = buf; *cp++ = longname[i++];)
			;
		*--cp = '/';
		*++cp = '\0';
		if (!oflag && update) {
#ifdef PFS
			stbuf.st_size.slow = stbuf.st_size.shigh = 0L; 
#else
			stbuf.st_size = 0;
#endif
			if (!tomodes(&stbuf, buf, shortname))
				return;
  			sprintf(dblock.dbuf.chksum, "%06o", checksum());
			(void) writetape((char *)&dblock);
			if ( vflag )
				fprintf(vfile, "a %s\n", longname);
		}

		sprintf(newparent, "%s/%s", parent, shortname);
		if (chdir(shortname) < 0) {
			perror(shortname);
			return;
		}
		if ((dirp = opendir(".")) == NULL) {
			fprintf(stderr, MSGSTR( EDIRREAD,
				"tar: %s: directory read error\n" ),
				longname);
			if (chdir(parent) < 0) {
				fprintf(stderr, MSGSTR( ECHBACK,
					"tar: cannot change back?: "));
				perror(parent);
			}
			return;
		}

		list_empty(&list);
		while ((dp = readdir(dirp)) != NULL && !term) {
			if (!strcmp(".", dp->d_name) ||
			    !strcmp("..", dp->d_name))
				continue;
			list_append(&list, dp->d_name);
		}
		closedir(dirp);

		while ((elem = list_first(&list)) != NULL && !term) {
			strcpy(cp, elem);
			free(elem);
			writingdir++;
			putfile(buf, cp, newparent);
		}

		if (chdir(parent) < 0) {
			fprintf(stderr, MSGSTR( ECHBACK,
				"tar: cannot change back?: "));
			perror(parent);
		}
		writingdir = 0;
		break;

	case S_IFLNK:
		if ( ! tomodes ( &stbuf , longname , shortname ))
			return;
#ifdef PFS
		ename_size = _eadd1(ezero, name_size);
		if (EQUAL(ename_size, eerror)) {
			fprintf(stderr,
				MSGSTR(EMATH, "tar: extended math failure\n"));
			exit(2);
		}
		if (GREATER(stbuf.st_size, ename_size)) {
#else
		if (stbuf.st_size > name_size) {
#endif
			fprintf(stderr, MSGSTR( ELONGSL,
				"tar: %s: symbolic link too long\n" ),
				longname);
#ifdef PFS
			ename_size = _eadd1(ezero, NAME_SIZE);
			if (EQUAL(ename_size, eerror)) {
				fprintf(stderr,
					MSGSTR(EMATH, "tar: extended math failure\n"));
				exit(2);
			}
			if (EQUAL(stbuf.st_size, ename_size))
#else
			if (stbuf.st_size == NAME_SIZE)
#endif
				fprintf(stderr, MSGSTR(ELONGFN2,
					"    (use 'n' option)\n"));
			return;
		}
		i = readlink(shortname, dblock.dbuf.linkname, name_size);
		if (i < 0) {
			fprintf(stderr, MSGSTR( EREADSL,
				"tar: can't read symbolic link "));
			perror(longname);
			return;
		}
		if (i < NAME_SIZE)
			dblock.dbuf.linkname[i] = '\0';
		if (vflag)
			fprintf(vfile, MSGSTR(SLINKTO,
				"a %s symbolic link to %.*s\n" ),
				longname, name_size, dblock.dbuf.linkname);
		sprintf(dblock.dbuf.size, "%011lo", 0L);
		sprintf(dblock.dbuf.chksum, "%06o", checksum());
		(void) writetape((char *)&dblock);
		break;

	case S_IFREG:
		if ( ! tomodes ( &stbuf , longname , shortname ))
			return;
		if ((infile = open(shortname, O_RDONLY)) < 0) {
			fprintf(stderr, "tar: ");
			perror(longname);
			return;
		}
		if ((i = link_file(longname)) < 0)
			return;
		else if (i) {
			sprintf(dblock.dbuf.chksum, "%06o", checksum());
			(void) writetape( (char *) &dblock);
			close(infile);
			return;
		}
#ifdef PFS
		blocks = _eadd1(ezero, _ediv(_eadd1(stbuf.st_size, TBLOCK-1),
					     TBLOCK));
		if (vflag) {
			char s[20];

			if (_etos(blocks, s) < 0) {
				fprintf(stderr,
					MSGSTR(EMATH, "tar: extended math failure\n"));
				exit(2);
			}
			fprintf(vfile,
				MSGSTR(BLKS, "a %s %s blocks\n" ), longname, s);
		}
		if (extended_file) {
			/*
			 * Extended file ... use pfs_checksum() so tars on
			 * other architectures will not attempt to interpret
			 * this header.  Instead, they should fail with
			 * "directory checksum error".
			 */
			sprintf(dblock.dbuf.chksum, "%06o", pfs_checksum());
		} else {
			sprintf(dblock.dbuf.chksum, "%06o", checksum());
		}
#else
		blocks = (stbuf.st_size + (TBLOCK-1)) / TBLOCK;
		if (vflag)
			fprintf(vfile, MSGSTR( BLKS,
				"a %s %ld blocks\n" ), longname, blocks);
		sprintf(dblock.dbuf.chksum, "%06o", checksum());
#endif
		hint = writetape( (char *) &dblock);
#ifdef PFS
		maxread = (nblock * TBLOCK);
#else
		maxread = max(stbuf.st_size, (nblock * TBLOCK));
#endif
		if ((bigbuf = (char *) malloc((size_t)maxread)) == 0) {
			maxread = TBLOCK;
			bigbuf = buf;
		}
#ifdef PFS
		while ((i = read(infile, bigbuf,
				 (unsigned)(min((hint*TBLOCK), maxread)))) > 0
		       && GREATER(blocks, ezero)) {
		  	register int nblks;

			nblks = ((i-1)/TBLOCK)+1;
		  	if ((nblks > blocks.slow) && (blocks.shigh == 0))
		  		nblks = blocks.slow;
			hint = writetbuf(bigbuf, nblks);
			blocks = _esub1(blocks, nblks);
#else
		while ((i = read(infile, bigbuf, (unsigned)(min((hint*TBLOCK), maxread)))) > 0
		  && blocks > 0) {
		  	register int nblks;

			nblks = ((i-1)/TBLOCK)+1;
		  	if (nblks > blocks)
		  		nblks = blocks;
			hint = writetbuf(bigbuf, nblks);
			blocks -= nblks;
#endif
		}
		close(infile);
		if (bigbuf != buf)
			free((void *)bigbuf);
		if (i < 0) {
			fprintf(stderr, MSGSTR( EREAD, "tar: Read error on "));
			perror(longname);
#ifdef PFS
		} else if (!EQUAL(blocks, ezero) || i != 0)
			fprintf(stderr, MSGSTR(EFSIZE,
				"tar: %s: file changed size\n"), longname);
		while (!LESS((blocks = _esub1(blocks, 1)), ezero))
#else
		} else if (blocks != 0 || i != 0)
			fprintf(stderr, MSGSTR(EFSIZE,
				"tar: %s: file changed size\n"), longname);
		while (--blocks >=  0)
#endif
			putempty();
		break;

	case S_IFBLK:
	case S_IFCHR:
	case S_IFIFO:
				/*  special files - write header  */
		if (!oflag) {
			if (! tomodes(&stbuf, longname, shortname))
				return;
			if ((i = link_file(longname)) < 0)
				return;
			if (vflag && !i)
				if (dblock.dbuf.typeflag == FIFOTYPE)
					fprintf(vfile,"a %s fifo\n", longname);
				else {
					(void) decode();
					fprintf(vfile,"a %s %s device %d: %d, %d\n",
						longname,
						dblock.dbuf.typeflag == BLKTYPE?
							"block" : "character",
						device_node, dev_maj, dev_min);
				}
			sprintf(dblock.dbuf.chksum, "%06o", checksum());
			(void) writetape( (char *) &dblock);
			break;
		}

	default:
		fprintf(stderr, MSGSTR( ENOARCH,
			"tar: %s could not be archived\n" ), longname);
		break;
	}
}


link_file (longname)
	char *longname;
{
	struct linkbuf *lp;
	int len, found = 0;

	if (stbuf.st_nlink > 1) {

		for (lp = ihead; lp != NULL; lp = lp->nextp)
			if (lp->inum == stbuf.st_ino &&
			    lp->devnum == stbuf.st_dev) {
				found++;
				break;
			}
		if (found) {
			if (lp->pathname[0] == '\0') {	/* link > NAME_SIZE */
				fprintf(stderr, MSGSTR(ELONGLN,
					"tar: %s skipped: linked to a pathname that is too long\n"),
					longname);
				if (lp->pathname[1] == 'n')
					fprintf(stderr, MSGSTR(ELONGFN2,
						"    (use 'n' option)\n"));
				return(-1);
			}
			strncpy(dblock.dbuf.linkname, lp->pathname, NAME_SIZE);
			dblock.dbuf.typeflag = LNKTYPE;
			sprintf(dblock.dbuf.size, "%011lo", 0L);
			if (vflag)
				fprintf(vfile, MSGSTR( LINKTO,
					"a %s linked to %s\n" ),
					longname, lp->pathname);
			lp->count--;
		}
		else {
			lp = (struct linkbuf *) getmem(sizeof(*lp));
			if (lp != NULL) {
				lp->nextp = ihead;
				ihead = lp;
				lp->inum = stbuf.st_ino;
				lp->devnum = stbuf.st_dev;
				lp->count = stbuf.st_nlink - 1;
				if ((len = strlen(longname)) <= name_size) 
					strcpy(lp->pathname, longname);
				else {	/* Can't link if name > NAME_SIZE */
					lp->pathname[0] = '\0';
					lp->pathname[1] = 
						len == NAME_SIZE ? 'n' : 'o';
				}
			}
		}
	}

	return(found);

}	/*  end link_file  */


doxtract(argv)
	char *argv[];
{
#ifdef PFS
	esize_t blocks, bytes;
#else
	long blocks, bytes;
#endif
	int  ofile, i, newdir;
	char ftype;

	for (;;) {
		if ((i = wantit(argv)) == 0)
			continue;
		if (i == -1)
			break;		/* end of tape */
		if (checkw('x', full_name) == 0) {
			passtape();
			continue;
		}
		if (Fflag) {
			char *s;

			if ((s = rindex(full_name, '/')) == 0)
				s = full_name;
			else
				s++;
			if (checkf(s, stbuf.st_mode) == 0) {
				passtape();
				continue;
			}
		}

		if (checkdir(&newdir)) {	/* have a directory */
			if (mflag == 0)
				dodirtimes();
			if (vflag && newdir)
				fprintf(vfile, "x %s\n", full_name);
			continue;
		}

		if (dblock.dbuf.typeflag == SYMTYPE) {	/* symlink */
			char link_name[NAME_SIZE+1];
			/*
			 * only unlink non directories or empty
			 * directories
			 */
			if (rmdir(full_name) < 0) {
				if (errno == ENOTDIR)
					unlink(full_name);
			}
			strncpy(link_name, dblock.dbuf.linkname, NAME_SIZE);
			link_name[NAME_SIZE] = '\0';
			if (symlink(link_name, full_name)<0) {
				fprintf(stderr, MSGSTR( ESYMFAIL,
					"tar: %s: symbolic link failed: " ),
					full_name);
				perror("");
				continue;
			}
			if (vflag)
				fprintf(vfile, MSGSTR( XSYMLINK,
					"x %s symbolic link to %s\n" ),
					full_name, link_name);
#if SEC_ARCH
			ie_sl_set_attributes(full_name);
#endif
			/* ignore alien orders */
			/* Commented out (as is BSD 4.3) since modifications
			   are to linked to file, not link itself. */
			/*
			if (!oflag)
				chown(full_name, stbuf.st_uid, stbuf.st_gid);
			if (mflag == 0)
				setimes(full_name, stbuf.st_mtime);
			if (pflag)
				chmod(full_name, stbuf.st_mode & 07777);
			*/
			continue;
		}

		if (dblock.dbuf.typeflag == LNKTYPE) {	/* regular link */
			char	link_type;
			char	link_buf[NAME_SIZE+1];
			char	*link_name;
			/*
			 * only unlink non directories or empty
			 * directories
			 */
			if (rmdir(full_name) < 0) {
				if (errno == ENOTDIR)
					unlink(full_name);
			}
			strncpy(link_buf, dblock.dbuf.linkname, NAME_SIZE);
			link_buf[NAME_SIZE] = '\0';
			link_name = Pflag ? stripprefix(link_buf) : link_buf;
			if (sflag && link_name[0] == '/') {
				while (*link_name && *link_name == '/')
					++link_name;
			}
			if (!*link_name) {
				fprintf(stderr, MSGSTR(EFLINK,
					"tar: can't link %s to %s: "),
					full_name, link_buf);
				fprintf(stderr, MSGSTR(ELPREFIX,
			"link no longer exists since prefix was stripped.\n"));
				continue;
			}
			if (link(link_name, full_name) < 0) {
				if ( !Lflag ) {
					fprintf(stderr, MSGSTR( EFLINK,
						"tar: can't link %s to %s: " ),
						full_name, link_name);
					perror("");
					continue;
				}
				else if (symlink(link_name, full_name) < 0) {
					fprintf(stderr, MSGSTR(EFLINK,
						"tar: can't link %s to %s: "),
				    		full_name, link_name);
					perror("");
					continue;
				}
				else
					link_type = SYMTYPE;
			}
			else
				link_type = LNKTYPE ;

			if (vflag)
				if ( link_type == LNKTYPE )
					fprintf(vfile, MSGSTR( LINKEDTO,
					"x %s linked to %s\n" ),
					full_name, link_name);
				else
					fprintf(vfile, MSGSTR( XSYMLINK,
					"x %s symbolic link to %s\n" ),
					full_name, link_name);
			continue;
		}

				/*  check for special files  */
		if ((ftype = dblock.dbuf.typeflag) == FIFOTYPE ||
			ftype == BLKTYPE || ftype == CHRTYPE) {
#if SEC_BASE
			if (ftype != FIFOTYPE && !hassysauth(SEC_MKNOD)) {
			    fprintf(stderr, MSGSTR(ESECMKNODAUTH,
				"tar: %s: need mknod kernel authorization\n"),
				full_name);
			    continue;
			}
#endif

			extract_node();
			if (rmdir(full_name) < 0 && errno == ENOTDIR)
				unlink(full_name);
			if (rmknod(full_name, stbuf.st_mode, decode(), device_node) < 0) {
				fprintf(stderr, MSGSTR(EMKNOD,
					"tar: %s: mknod failed: "), full_name);
				perror("");
				continue;
			}
#if SEC_ARCH
			ie_sl_set_attributes(full_name);
#endif
#if SEC_BASE
			if (getluid() == stbuf.st_uid || hassysauth(SEC_OWNER))
#endif
			if (!oflag)
				chown(full_name, stbuf.st_uid, stbuf.st_gid);
			if (mflag == 0)
				setimes(full_name, stbuf.st_mtime);
			if (pflag)
#if SEC_BASE
				ie_chmod(full_name, stbuf.st_mode & 07777, 0);
#else
				chmod(full_name, stbuf.st_mode & 07777);
#endif
			if (vflag)
				if (ftype == FIFOTYPE)
					fprintf(vfile,"x %s fifo\n", full_name);
				else
					fprintf(vfile,"x %s %s device %d: %d, %d\n",
						full_name,
						dblock.dbuf.typeflag == BLKTYPE?
							"block" : "character",
						device_node, dev_maj, dev_min);
			continue;
		}

		if ((ofile = creat(full_name,stbuf.st_mode&0xfff)) < 0) {
			fprintf(stderr, MSGSTR( ETCREATE,
				"tar: can't create %s: " ),
				full_name);
			perror("");
			passtape();
			continue;
		}
#if SEC_ARCH
		if (!ie_sl_set_attributes(full_name)) {
			close(ofile);
			unlink(full_name);
			passtape();
			continue;
		}
#endif
#if SEC_BASE
		if (getluid() == stbuf.st_uid || hassysauth(SEC_OWNER))
#endif
		if (!oflag)
			chown(full_name, stbuf.st_uid, stbuf.st_gid);
#ifdef PFS
		bytes.slow = stbuf.st_size.slow;
		bytes.shigh = stbuf.st_size.shigh;
		blocks = _eadd1(ezero, _ediv(_eadd1(bytes, TBLOCK-1), TBLOCK));
#else
		blocks = ((bytes = stbuf.st_size) + TBLOCK-1)/TBLOCK;
#endif
		if (vflag)
#ifdef PFS
		{
			char s[20], s1[20];

			if (_etos(bytes, s) < 0) {
				fprintf(stderr,
					MSGSTR(EMATH, "tar: extended math failure\n"));
				exit(2);
			}
			if (_etos(blocks, s1) < 0) {
				fprintf(stderr,
					MSGSTR(EMATH, "tar: extended math failure\n"));
				exit(2);
			}
			fprintf(vfile, MSGSTR( XSTAT,
				"x %s, %s bytes, %s tape blocks\n" ),
				full_name, s, s1);
		}
#else
			fprintf(vfile, MSGSTR( XSTAT,
				"x %s, %ld bytes, %ld tape blocks\n" ),
				full_name, bytes, blocks);
#endif
#ifdef PFS
		for (; GREATER(blocks, ezero);) {
#else
		for (; blocks > 0;) {
#endif
			register int nread;
			char	*bufp;
			register int nwant;
	
                        if (nblock == 0) {
                                nwant = NBLOCK*TBLOCK;
                        } else {
                                nwant = nblock*TBLOCK;
                        }
		
#ifdef PFS
			nwant = (int)_emin1(_emul(blocks, TBLOCK), (long)nwant);
#else
			if (nwant > (blocks*TBLOCK))
				nwant = (blocks*TBLOCK);
#endif
			nread = readtbuf(&bufp, nwant);
#ifdef PFS
			if (write(ofile, bufp,
				  (unsigned)_emin1(bytes, (long)nread)) < 0) {
#else
			if (write(ofile, bufp, (unsigned)min(nread, bytes)) < 0) {
#endif
				fprintf(stderr, MSGSTR( EXWRITE,
					"tar: %s: HELP - extract write error" ),
					full_name);
				perror("");
				done(2);
			}
#ifdef PFS
			bytes = _esub1(bytes, nread);
			blocks = _esub1(blocks, (((nread-1)/TBLOCK)+1));
#else
			bytes -= nread;
			blocks -= (((nread-1)/TBLOCK)+1);
#endif
		}
		close(ofile);
		if (mflag == 0)
			setimes(full_name, stbuf.st_mtime);
		if (pflag)
#if SEC_BASE
			ie_chmod(full_name, stbuf.st_mode & 07777, 0);
#else
			chmod(full_name, stbuf.st_mode & 07777);
#endif
	}
	if (mflag == 0) {
		full_name[0] = '\0';	/* process the whole stack */
		dodirtimes();
	}
}


	/*	The function decode will return a value which can be
	 *	used in the function call mknod. The value is extracted
	 *	from the dblock major and minor character strings.
	 */
decode ()
{
	sscanf(dblock.dbuf.devminor, "%0o", &dev_min);
	sscanf(dblock.dbuf.devmajor, "%0o", &dev_maj);
	return ( makedev ( dev_maj , dev_min )) ;
}	/*  end decode  */


/*
 * FUNCTION:	extract_node
 *
 * 	This function is used to extract the node number from a file header. If the
 *	maghic string is not found, the device node is set to 0. This is the default
 *	for devices.
 */
extract_node()
{
	char *ptr;
	int len;
	char tmp[10];

	/*
	 * Look for the magic string after the prefix. If it's there, get the
	 * node number from behind the NULL terminator.
	 */
	ptr = dblock.dbuf.prefix;
	len = strlen(ptr);
	device_node=0;
	if (len < (PREFIX_SIZE-9)) {
		ptr += len+1;
		strncpy(tmp, ptr, 8);
		tmp[8]=0;
		if (!strncmp(tmp, SPEC_MAGIC, 4)) {
			device_node=atoi(tmp+4);
		}
	}
}



dotable(argv)
	char *argv[];
{
	register int i;

	for (;;) {
		if ((i = wantit(argv)) == 0)
			continue;
		if (i == -1)
			break;		/* end of tape */
		if (vflag)
			longt(&stbuf);
		printf("%s ", full_name);
		switch (dblock.dbuf.typeflag) {
			case LNKTYPE:
				printf(MSGSTR(LINKED, "linked to %.*s"),
					NAME_SIZE, dblock.dbuf.linkname);
				break;
			case SYMTYPE:
				printf(MSGSTR(SLINKED, "symbolic link to %.*s"),
					NAME_SIZE, dblock.dbuf.linkname);
				break;
			case FIFOTYPE:
				printf("fifo");
				break;
			case CHRTYPE:
			case BLKTYPE:
				(void) decode();
				extract_node();
				printf("%d: %d, %d",
					device_node, dev_maj, dev_min);
/*
				printf("%s device %d: %d, %d",
					dblock.dbuf.typeflag == CHRTYPE ?
						"character" : "block",
					device_node, dev_maj, dev_min);
*/
				break;
		}
		printf("\n");
		passtape();
	}
}

putempty()
{
	char buf[TBLOCK];

	bzero(buf, sizeof (buf));
	(void) writetape(buf);
}

longt(st)
#ifdef PFS
	register struct estat *st;
#else
	register struct stat *st;
#endif
{
	register char *cp;
	char *ctime(), timbuf[26];

	pmode(st);
	printf("%3d/%-2d", st->st_uid, st->st_gid);
#ifdef PFS
	{
	char s[20];

	if (_etos(st->st_size, s) < 0) {
		fprintf(stderr, MSGSTR(EMATH, "tar: extended math failure\n"));
		exit(2);
	}
	printf(" %20s ", s);
	}
#else
	printf(" %7ld ", st->st_size);
#endif
	strftime(timbuf, (size_t)26,"%sD %T %Y", localtime(&st->st_mtime));
	printf("%s ",timbuf);
}

	/*	These constants are defined in tar.h
	 */
int	m1[] = { 1, TUREAD, 'r', '-' };
int	m2[] = { 1, TUWRITE, 'w', '-' };
int	m3[] = { 2, TSUID, 's', TUEXEC, 'x', '-' };
int	m4[] = { 1, TGREAD, 'r', '-' };
int	m5[] = { 1, TGWRITE, 'w', '-' };
int	m6[] = { 2, TSGID, 's', TGEXEC, 'x', '-' };
int	m7[] = { 1, TOREAD, 'r', '-' };
int	m8[] = { 1, TOWRITE, 'w', '-' };
int	m9[] = { 2, TSVTX, 't', TOEXEC, 'x', '-' };

int	*m[] = { m1, m2, m3, m4, m5, m6, m7, m8, m9};

pmode(st)
#ifdef PFS
	register struct estat *st;
#else
	register struct stat *st;
#endif
{
	char	c ;
	register int **mp;

	switch ( st->st_mode & S_IFMT ) {
		case S_IFREG :
			c = '-' ; break ;
		case S_IFDIR :
			c = 'd' ; break ;
		case S_IFLNK :
			c = 'l' ; break ;
		case S_IFCHR :
			c = 'c' ; break ;
		case S_IFBLK :
			c = 'b' ; break ;
		case S_IFIFO :
			c = 'p' ; break ;
		default :
			c = '?' ; break ;
	}
	putchar (c) ;
	for (mp = &m[0]; mp < &m[9];)
		selectbits(*mp++, st);
}

selectbits(pairp, st)
	int *pairp;
#ifdef PFS
	struct estat *st;
#else
	struct stat *st;
#endif
{
	register int n, *ap;

	ap = pairp;
	n = *ap++;
	while (--n>=0 && (st->st_mode&*ap++)==0)
		ap++;
	putchar(*ap);
}

/*
 * Make all directories needed by `name'.  If `name' is itself
 * a directory on the tar tape (indicated by a trailing '/'),
 * return 1; else 0.  The parameter allows the status of whether
 * the directory already existed to be returned.
 * The path name is created by concatenating dblock.dbuf.prefix
 * and dblock.dbuf.name together in the function getdir.
 */
checkdir(newdir)
int *newdir;
{
	register char *cp;

	*newdir = 0;
	/*
	 * Quick check for existence of directory.
	 */
	if ((cp = rindex(full_name, '/')) == 0)
		return (0);
	*cp = '\0';
	if (access(full_name, 0) == 0) {	/* already exists */
		*cp = '/';
		return (cp[1] == '\0');	/* return (lastchar == '/') */
	}
	*cp = '/';

	/*
	 * No luck, try to make all directories in path.
	 */
	cp = full_name;
	while (*cp == '/')
		cp++;
	for (; *cp; cp++) {
		if (*cp != '/')
			continue;
		*cp = '\0';
		if (access(full_name, 0) < 0) {
			if (mkdir(full_name, (mode_t)0777) < 0) {
				perror(full_name);
				*cp = '/';
				return (0);
			}
			*newdir = 1;
#if SEC_ARCH
			ie_sl_set_attributes(full_name);
#endif
#if SEC_BASE
			if (getluid() == stbuf.st_uid || hassysauth(SEC_OWNER))
#endif
			if (!oflag)
				chown(full_name, stbuf.st_uid, stbuf.st_gid);
			if (pflag && cp[1] == '\0')	/* dir on the tape */
#if SEC_BASE
				ie_chmod(full_name, stbuf.st_mode & 07777, 1);
#else
				chmod(full_name, stbuf.st_mode & 07777);
#endif
		}
		*cp = '/';
	}
	return (cp[-1]=='/');
}

onintr(void)
{
	(void) signal(SIGINT, SIG_IGN);
	term++;
}

onquit(void)
{
	(void) signal(SIGQUIT, SIG_IGN);
	term++;
}

onhup(void)
{
	(void) signal(SIGHUP, SIG_IGN);
	term++;
}

onterm(void)
{
	(void) signal(SIGTERM, SIG_IGN);
	term++;
}


	/*	The function to modes will fill in the header
	 *	information for a file.
	*/
tomodes(sp, longname , shortname )
#ifdef PFS
register struct estat *sp;
#else
register struct stat *sp;
#endif
	char	*longname ;
	char	*shortname ;
{
	register char	*cp;
	char		*slash ;
	int		ftype ;
	struct passwd	*pw_entry ;
	struct group	*gr_entry ;
	int nameLen;    /* length of longname */
	register char *nameBkpt; /* Name break point */
	char *ptr;
	int len;

	for (cp = dblock.dummy; cp < &dblock.dummy[TBLOCK]; cp++)
		*cp = '\0';

#ifdef	PFS
	extended_file = 0;
#endif
	nameLen = strlen(longname);
	if (nameLen <= name_size) {
		strncpy(dblock.dbuf.name, longname, nameLen);
		dblock.dbuf.prefix[0] = '\0';
	} else {
		nameBkpt = longname + nameLen;
		/*
		 * Put as much into name as possible and the rest into prefix,
		 * but only break on a slash.
		 */
		slash = longname;
		do {
			nameBkpt--;
			if ('/' == *nameBkpt)
				slash = nameBkpt;
		} while (nameLen - (nameBkpt-longname) <= name_size);

		if (('/' == *slash) && ((slash-longname) <= PREFIX_SIZE)){
			strncpy(dblock.dbuf.name, slash+1,
				(size_t)(nameLen - (slash+1-longname)));
			strncpy(dblock.dbuf.prefix, longname,
				(size_t)(slash-longname));
		} else {
			fprintf(stderr, MSGSTR(ELONGFN,
				"tar: %s: file name too long\n"), longname);
			if (nameLen == NAME_SIZE ||
			    *--nameBkpt == '/'
			    && nameLen - (nameBkpt+1-longname) <= NAME_SIZE
			    && nameBkpt-longname <= PREFIX_SIZE)
				fprintf(stderr, MSGSTR(ELONGFN2,
					"    (use 'n' option)\n"));
			return (FALSE) ;
		}
	}

	/*
	 * Get the node number of this device.  Store the number after the 
	 * prefix text's NULL terminator.
	 */
	ftype = sp -> st_mode & S_IFMT ;
	if ((ftype == S_IFCHR) || (ftype == S_IFBLK))
	{
		device_node = get_node(longname);
		len=strlen(dblock.dbuf.prefix);
		if (len > (PREFIX_SIZE-9)) {
			fprintf(stderr, "WARNING: Name too long to encode node.\n");
		}
		else {
			ptr = dblock.dbuf.prefix+len+1;
			sprintf(ptr, "%4s%4d", SPEC_MAGIC, device_node);
		}
	}

	sprintf(dblock.dbuf.mode, "%06o ", sp->st_mode & 07777);
	sprintf(dblock.dbuf.uid, "%06o ", sp->st_uid);
	sprintf(dblock.dbuf.gid, "%06o ", sp->st_gid);
#ifdef PFS
	/*
	 * Check if this is an extended ( > 2GB-1 bytes in size) file.
	 */
	if (GREATER(sp->st_size, eint_max)) {
		char	s[24];
		int	i, j;

		if (!Eflag) {
			/*
			 * No extended flag given: don't archive extended 
			 * files.
			 */
			fprintf(stderr,
				MSGSTR(ETNOEFLAG,
				       "tar: %s: skipping extended file (-E switch required)\n"),
				longname);
			return(FALSE);
		}

		extended_file++;

		/*
		 * Extended flag was used: use a hexadecimal string format for
		 * the file size rather than octal ... this allows a file of 
		 * size up to 255.999 TB instead of 63 GB.
		 */
		/* make sure file is not > 255.999 TB in size */
		if (GREATER(sp->st_size, etar_max)) {
			fprintf(stderr,
				MSGSTR(ETOOBIG, "tar: skipping %s, file too large\n"),
				longname);
			return(FALSE);
		}

		/* make size field all ASCII 0's */
		for (i = 0; i < sizeof(dblock.dbuf.size); i++)
			dblock.dbuf.size[i] = '0';

		/* create ASCII hex representation of the file size */
		if (_etoxs(sp->st_size, s) < 0) {
			fprintf(stderr,
				MSGSTR(EMATH, "tar: extended math failure\n"));
			exit(2);
		}

		/* copy string into size field, right-adjusted */
		for (i=strlen(s), j=sizeof(dblock.dbuf.size); i >= 0; i--, j--)
			dblock.dbuf.size[j] = s[i];
	} else {
		/*
		 * Use standard tar header format.
		 */
		sprintf(dblock.dbuf.size, "%011lo ", sp->st_size.slow);
	}
	sprintf(dblock.dbuf.mtime, "%011lo ", sp->st_mtime);
#else
	sprintf(dblock.dbuf.size, "%011lo ", sp->st_size);
	sprintf(dblock.dbuf.mtime, "%011lo ", sp->st_mtime);
#endif	PFS
	strncpy (dblock.dbuf.magic, TMAGIC , (size_t)TMAGLEN ) ;
	strncpy (dblock.dbuf.version, TVERSION , (size_t)TVERSLEN ) ;

				/*  set file type  */
	switch ( ftype ) {
		case S_IFIFO :
			dblock.dbuf.typeflag = FIFOTYPE ;
			break ;

		case S_IFCHR :
			dblock.dbuf.typeflag = CHRTYPE ;
			break ;

		case S_IFBLK :
			dblock.dbuf.typeflag = BLKTYPE ;
			break ;

		case S_IFDIR :
			dblock.dbuf.typeflag = DIRTYPE ;
			break ;

		case S_IFREG :
			dblock.dbuf.typeflag = REGTYPE ;
			break ;

		case S_IFLNK :
			dblock.dbuf.typeflag = SYMTYPE;
			break;
	}

				/*  set uname and gname (NULL terminated) */
	if (( pw_entry = getpwuid ( sp->st_uid )))
		strncpy ( dblock.dbuf.uname , pw_entry->pw_name, UNAMELEN ) ;
	dblock.dbuf.uname[UNAMELEN-1] = '\0';
	if (( gr_entry = getgrgid ( sp->st_gid )))
		strncpy ( dblock.dbuf.gname , gr_entry->gr_name, GNAMELEN ) ;
	dblock.dbuf.gname[GNAMELEN-1] = '\0';

				/*  set major and minor values  */
	if ( ftype == S_IFIFO || ftype == S_IFBLK || ftype == S_IFCHR ) {
		sprintf(dblock.dbuf.devminor, "%06o " , minor(sp->st_rdev));
		sprintf(dblock.dbuf.devmajor, "%06o " , major(sp->st_rdev));
	}
	else {		/* POSIX requires the 'right stuff' for all types */
		sprintf(dblock.dbuf.devminor, "%06o " , 0);
		sprintf(dblock.dbuf.devmajor, "%06o " , 0);
	}
		

	return (TRUE) ;
}


	/*	The checksum function will create a checksum for
	 *	the header of a file written to the output file.
	*/
checksum()
{
	register i;
	register char *cp;

	for (cp = dblock.dbuf.chksum;
	     cp < &dblock.dbuf.chksum[sizeof(dblock.dbuf.chksum)]; cp++)
		*cp = ' ';
	i = 0;
	for (cp = dblock.dummy; cp < &dblock.dummy[TBLOCK]; cp++)
		i += *cp;
	return (i);
}

#ifdef	PFS
/*
 * pfs_checksum computes a different checksum for extended files (files
 * greater than 2G-1 bytes in size), so that tar commands on architectures
 * other than the Paragon will error out ("directory checksum error") on the
 * extended file rather than interpreting the file size field in the header
 * incorrectly and attempting to extract the file.
 */
pfs_checksum()
{
	return(checksum() + PFS_CHKSUM_MAGIC);
}
#endif

checkw(c, name)
	char *name;
{
	int	yesno;
	char	ans[2];
	if (!wflag)
		return (1);
	printf("%c ", c);
	if (vflag)
		longt(&stbuf);
	printf("%s: ", name);
	ans[0] = (char)response();
	ans[1] = '\0';
	if ((yesno = NLyesno(ans)) == -1)
		yesno = (*ans == 'y' || *ans == 'Y');
	return (yesno);
}

response()
{
	int c;

	c = getwchar();
	if (c != '\n')
		while (getwchar() != '\n')
			;
	return (c);
}

checkf(name, mode)
	char *name;
	int mode;
{
	int l;

	if ((mode & S_IFMT) == S_IFDIR){
		if ((strcmp(name, "SCCS")==0) || (strcmp(name, "RCS")==0)) 
			return(0); 
		return(1);
	}
	if ((l = strlen(name)) < 3)
		return (1);
	if (name[l-2] == '.' && name[l-1] == 'o')
		return (0);
	if (strcmp(name, "core") == 0 ||
	    strcmp(name, "errs") == 0 ||
	    (strcmp(name, "a.out") == 0))
		return (0);
	/* SHOULD CHECK IF IT IS EXECUTABLE */
	return (1);
}

/* Is the current file a new file, or the newest one of the same name? */
checkupdate(arg)
	char *arg;
{
	char name[TAR_PATH_LEN];
	long mtime;
	daddr_t seekp;
	daddr_t	lookup();

	rewind(tfile);
	for (;;) {
		if ((seekp = lookup(arg)) < 0)
			return (1);
		fseek(tfile, seekp, 0);
		fscanf(tfile, "%s %lo", name, &mtime);
		return (stbuf.st_mtime > mtime);
	}
}

done(n)
{
	unlink(tname);
	exit(n);
}

/* 
 * Do we want the next entry on the tape, i.e. is it selected?  If
 * not, skip over the entire entry.  Return -1 if reached end of tape.
 */
wantit(argv)
	char *argv[];
{
	register char **cp;

	getdir() ;
	if (endtape())
		return (-1);
	if (*argv == 0)
		return (1);
	for (cp = argv; *cp; cp++)
		if (prefix(*cp, full_name))
			return (1);
	passtape();
	return (0);
}

/*
 * Does s2 begin with the string s1, on a directory boundary?
 */
prefix(s1, s2)
	register char *s1, *s2;
{
	while (*s1)
		if (*s1++ != *s2++)
			return (0);
	if (*s2)
		return (*s2 == '/');
	return (1);
}

/*
 * The size of a buffer that when read from a random place in tfile is (mostly)
 * guaranteed to contain a complete record (tarpathlen + mtime size).
 */
#define	N	512

daddr_t
lookup(s)
	char *s;
{
	register i;
	daddr_t a;

	for(i=0; s[i]; i++)
		if (s[i] == ' ')
			break;
	a = bsrch(s, i, low, high);
	return (a);
}

daddr_t
bsrch(s, n, l, h)
	daddr_t l, h;
	char *s;
{
	register i, j;
	char b[N];
	daddr_t m, m1;

loop:
	if (l >= h)
		return ((daddr_t) -1);
	m = l + (h-l)/2 - N/2;
	if (m < l)
		m = l;
	fseek(tfile, m, 0);
	fread((void *)b, (size_t)1, (size_t)N, tfile);
	for(i=0; i<N; i++) {
		if (b[i] == '\n')
			break;
		m++;
	}
	if (m >= h)
		return ((daddr_t) -1);
	m1 = m;
	j = i;
	for(i++; i<N; i++) {
		m1++;
		if (b[i] == '\n')
			break;
	}
	i = cmp(b+j, s, n);
	if (i < 0) {
		h = m;
		goto loop;
	}
	if (i > 0) {
		l = m1;
		goto loop;
	}
	return (m+1);
}

cmp(b, s, n)
	char *b, *s;
{
	register i;

	if (b[0] != '\n')
		exit(2);
	for(i=0; i<n; i++) {
		if (b[i+1] > s[i])
			return (-1);
		if (b[i+1] < s[i])
			return (1);
	}
	return ((b[i+1] == ' ' || b[i+1] == '/' && b[i+2] == ' ') ? 0 : -1);
}

backtape()
{
	static int mtdev = 1;
	static struct mtop mtop = {MTBSR, 1};
	struct mtget mtget;
	
	if (mtdev == 1)
		mtdev = ioctl(mt, MTIOCGET, (char *)&mtget);
	if (mtdev == 0) {
		if (ioctl(mt, MTIOCTOP, (char *)&mtop) < 0) {
			fprintf(stderr, MSGSTR(EBACKSP,
				"tar: tape backspace error: "));
			perror("");
			done(4);
		}
	} else
		lseek(mt, (daddr_t) -TBLOCK*nblock, 1);
	trecs--;
	recno--;
}


readtape(buffer)
	char *buffer;
{
	char *bufp;

	if (first == 0)
		getbuf();
	(void) readtbuf(&bufp, TBLOCK);
	bcopy(bufp, buffer, TBLOCK);
	return(TBLOCK);
}

readtbuf(bufpp, size)
	char **bufpp;
	int size;
{
	register int i;

	if (recno >= nblock || first == 0) {
		i = bread(mt, (char *)tbuf, TBLOCK*nblock);
		if (first == 0) {
			if ((i % TBLOCK) != 0) {
				fprintf(stderr, MSGSTR( EBLKSIZE,
					"tar: tape blocksize error\n"));
				done(3);
			}
			i /= TBLOCK;
			if (i != nblock) {
				fprintf(stderr, MSGSTR( BLKSIZE,
					"tar: blocksize = %d\n" ), i);
				nblock = i;
			}
			first = 1;
		}
		recno = 0;
	}
	if (size > ((nblock-recno)*TBLOCK))
		size = (nblock-recno)*TBLOCK;
	*bufpp = (char *)&tbuf[recno];
	recno += (size/TBLOCK);
	return (size);
}

writetbuf(buffer, n)
	register char *buffer;
	register int n;
{
	int i;

	if (first == 0) {
		getbuf();
		first = 1;
	}
	if (recno >= nblock) {
		bwrite ((char *) tbuf);
		recno = 0;
	}

	/*
	 *  Special case:  We have an empty tape buffer, and the
	 *  users data size is >= the tape block size:  Avoid
	 *  the bcopy and dma direct to tape.  BIG WIN.  Add the
	 *  residual to the tape buffer.
	 */
	while (recno == 0 && n >= nblock) {
		bwrite (buffer) ;
		n -= nblock;
		buffer += (nblock * TBLOCK);
	}
		
	while (n-- > 0) {
		bcopy(buffer, (char *)&tbuf[recno++], TBLOCK);
		buffer += TBLOCK;
		if (recno >= nblock) {
			bwrite ((char *) tbuf) ;
			recno = 0;
		}
	}

	/* Tell the user how much to write to get in sync */
	return (nblock - recno);
}


bwrite (buffer)
	char	*buffer ;
{
	int	wr ;

	if ( rptape && trecs >= rptape )
		nexttape (1) ;
tryagain:
	wr = write(mt, buffer, (unsigned)(TBLOCK*nblock)) ;
	if ( wr == -1 )
		if (multivol && (errno == ENXIO || errno == ENOSPC)) {
			nexttape (1) ;
			goto tryagain ;
		}
		else {
			perror ( MSGSTR( ETWRITE, "tar: tape write error" )) ;
			done(2);
		}
	else
		if (wr != TBLOCK*nblock) {
			fprintf(stderr, MSGSTR( ETWREOF,
				"tar: tape write error: unexpected EOF\n" ));
			done(2);
		}
	++trecs;
	return;
}


bread(fd, buf, size)
	int fd;
	char *buf;
	int size;
{
	int count;
	static int lastread = 0;

	if (rptape && trecs >= rptape)
		nexttape(0);
	trecs++;
				/***	the for loop is for a communications
					link by specifing the 'B' option,
					all other will exit the for loop
					after the first read unless prompted
					to enter a new device	***/
					
	for (count = 0; count < size; count += lastread) {
		lastread = read(fd, buf, (unsigned)(size - count));
		switch ( lastread )
		{
			case -1 :
				perror(MSGSTR(ETREAD, "tar: tape read error"));
				done(3);

			case 0 :
				if (!multivol) {
					if (count)
						return(count);
					fprintf(stderr, MSGSTR(ETRDEOF,
						"tar: tape read error: unexpected EOF\n"));
					done(2);
				}
				nexttape (0) ;	/*  prompt for new device  */
				break ;

			default :
				if ( !Bflag )
					return (lastread);
				break ;
		}
		buf += lastread;
	}
	return (count);
}


char *
getpwd(buf)
	char *buf;
{
	if (getwd(buf) == NULL) {
		fprintf(stderr, "tar: %s\n", buf);
		exit(1);
	}
	return (buf);
}

getbuf()
{
	if (nblock == 0) {
		fstat(mt, &stbuf);
		if ((stbuf.st_mode & S_IFMT) == S_IFCHR)
			nblock = NBLOCK;
		else {
			nblock = stbuf.st_blksize / TBLOCK;
			if (nblock == 0)
				nblock = NBLOCK;
		}
	}

	if (Sflag && multivol)		/*  set number of records per tape  */
		if ( bptape )
			rptape = bptape / nblock ;
		else {
   			unsigned long	len1rec; /* rcd len (100s of inches) */
			len1rec = ((unsigned long) nblock * (TBLOCK * 10000L))
				/ density + GAP;
			rptape = (tapelen * TAPE_FOOT) / len1rec;
		}

	tbuf = (union hblock *)malloc((size_t)nblock*TBLOCK);
	if (tbuf == NULL) {
		fprintf(stderr, MSGSTR(EBLKBIG,
			"tar: blocksize %d too big, can't get memory\n"),
			nblock);
		done(1);
	}
}

/*
 * Save this directory and its mtime on the stack, popping and setting
 * the mtimes of any stacked dirs which aren't parents of this one.
 * A null directory causes the entire stack to be unwound and set.
 *
 * Since all the elements of the directory "stack" share a common
 * prefix, we can make do with one string.  We keep only the current
 * directory path, with an associated array of mtime's, one for each
 * '/' in the path.  A negative mtime means no mtime.  The mtime's are
 * offset by one (first index 1, not 0) because calling this with a null
 * directory causes mtime[0] to be set.
 * 
 * This stack algorithm is not guaranteed to work for tapes created
 * with the 'r' option, but the vast majority of tapes with
 * directories are not.  This avoids saving every directory record on
 * the tape and setting all the times at the end.
 */
char dirstack[NAME_SIZE];
#define NTIM (NAME_SIZE/2+1)		/* a/b/c/d/... */
time_t mtime[NTIM];

dodirtimes()
{
	register char *p = dirstack;
	register char *q = full_name;
	register int ndir = 0;
	char *savp;
	int savndir;

	/* Find common prefix */
	while (*p == *q) {
		if (*p++ == '/')
			++ndir;
		q++;
	}

	savp = p;
	savndir = ndir;
	while (*p) {
		/*
		 * Not a child: unwind the stack, setting the times.
		 * The order we do this doesn't matter, so we go "forward."
		 */
		if (*p++ == '/')
			if (mtime[++ndir] >= 0) {
				*--p = '\0';	/* zap the slash */
				setimes(dirstack, mtime[ndir]);
				*p++ = '/';
			}
	}
	p = savp;
	ndir = savndir;

	/* Push this one on the "stack" */
	while (*p = *q++)	/* append the rest of the new dir */
		if (*p++ == '/')
			mtime[++ndir] = -1;
	mtime[ndir] = stbuf.st_mtime;	/* overwrite the last one */
}

setimes(path, mt)
	char *path;
	time_t mt;
{
	struct timeval tv[2];

	tv[0].tv_sec = time((time_t *) 0);
	tv[1].tv_sec = mt;
	tv[0].tv_usec = tv[1].tv_usec = 0;
	if (utimes(path, tv) < 0) {
		fprintf(stderr, MSGSTR( ESETTIME,
			"tar: can't set time on %s: " ), path);
		perror("");
	}
}

char *
getmem(size)
{
	char *p = (char *) malloc((size_t) size);

	if (p == NULL && freemem) {
		fprintf(stderr, MSGSTR(EMEM,
		    "tar: out of memory, link and directory modtime info lost\n"));
		freemem = 0;
	}
	return (p);
}

list_empty(listp)
struct list *listp;
{
	listp->first = listp->last = NULL;
}

list_append(listp, data)
struct list *listp;
char *data;
{
	struct elem *ep;
	int len;

	ep = (struct elem *) calloc(1, sizeof(struct elem));
	if (ep == NULL) {
		fprintf(stderr, MSGSTR(EMEM, "tar: out of memory\n"));
		done(5);
	}
	ep->data = malloc(len = (strlen(data) + 1));
	if (ep->data == NULL) {
		fprintf(stderr, MSGSTR(EMEM, "tar: out of memory\n"));
		done(5);
	}
	bcopy(data, ep->data, len);
	if (listp->first == NULL)
		listp->first = ep;
	else
		listp->last->next = ep;
	listp->last = ep;
}

char *
list_first(listp)
struct list *listp;
{
	struct elem *ep;
	char *data;

	if ((ep = listp->first) == NULL)
		return(NULL);
	listp->first = ep->next;
	data = ep->data;
	free((char *)ep);
	return(data);
}

int
excepted(data)
char *data;
{
	struct elem *ep;

	for (ep = except_list.first; ep != NULL; ep = ep->next)
		if (strcmp(ep->data, data) == 0)
			break;
	return(ep != NULL);
}

char *
stripprefix(filename)
register char *filename;
{
	register char *ptr;
	static int len = 0;
	
	if (!len)
		len = strlen(stripstring);
	if (strncmp(filename, stripstring, len))
		return(filename);
	else
		return(filename+len);
}

				/* move on to the next tape */
				/* mode is 0 for read, 1 for write */
nexttape(writing)
	int writing;
{
	int	c;
	char	tempdir[NAME_MAX];
	FILE	*devtty = stdin;
	int	tries = 0;

	if (!multivol) {
		fprintf(stderr, MSGSTR(EBADMVOL,
			"tar: %s is not a device capable of %s multi-volume archives.\n"),
			usefile, writing ? MSGSTR(TARCREATE, "creating")
					 : MSGSTR(TARXTRACT, "extracting"));
		done(4);
	}

	close(mt);
	fprintf(stderr, MSGSTR(BLKSON, "tar: %ld blocks on %s\n"),
		trecs * nblock, usefile);

	if (!isatty(fileno(devtty)) &&
	    (devtty = fopen("/dev/tty", "r")) == NULL) {
		fprintf(stderr, MSGSTR(EEOT, "tar: ran off end of tape\n"));
		done(4);
	}
loop:
	fprintf(stderr, MSGSTR(EOT,
	   "\007tar: End of device.  Mount next volume on %s and hit return."),
		usefile);
	do {
		if ((c = fgetc(devtty)) == EOF || term)
			done(4);
	} while (c != '\n');

	if (*startcwd && tries == 0) {
		(void) getpwd(tempdir);
		if (chdir(startcwd) < 0) {
			fprintf(stderr, MSGSTR(ECHDIR,
				"tar: can't change directories to "));
			perror(startcwd);
		}
	}
	if ((mt = open(usefile, writing ? O_RDWR : O_RDONLY)) < 0) {
		fprintf(stderr, MSGSTR(ETOPEN, "tar: cannot open "));
		perror(usefile);
		if (++tries >= 3) {
			fprintf(stderr, MSGSTR(EMAXRETRIES,
				"tar: maximum retries limit exceeded.\n"));
			done(3);
		}
		goto loop;
	}
	if (*startcwd && chdir(tempdir) < 0) {
			fprintf(stderr, MSGSTR( ECHBACK,
				"tar: cannot change back?: "));
			perror(tempdir);
			done(3);
	}

	fprintf(stderr, MSGSTR(PROCEED, "proceeding with device %s\n"),usefile);
	trecs = 0;

	return;
}


/*
 * FUNCTION:	get_node()
 *
 * Get the node number of the specified device. The node number will be
 * stored in the header (somewhere) so that the device can be restored
 * properly. Since there currently is no system call to get the node number,
 * use the raw 'syscall' method.
 */
int get_node(path)
char *path;
{
	struct devstat	buf;
	int node_num=0;
	char full_path[512];

	/*
	 * BUG #4928: Make sure the path contains an absolute path
	 * name. If not, make it so 
	 */
	if (*path != '/')
	{
		sprintf(full_path, "%s/%s",cur_wd, path);
		path=full_path;
	}
	if (syscall(267, path, &buf) < 0) 
	{
		node_num=0;
	}
	else
	{
		node_num = buf.dst_node;
	}
	return(node_num);
}


#ifdef	PFS
/*
 * Name:
 *      _emin1
 *
 * Description:
 *	Return the integer minimum of an integer and an extended number.
 *
 * Parameters:
 *	e	Extended number.
 *
 *	n	Integer (long).
 *
 * Returns:
 *	The integer minimum.
 */
long
_emin1(e, n)
esize_t e;
long    n;
{
	esize_t e2;

	e2.slow = n;
	e2.shigh = (n < 0) ? -1 : 0;

	if (_ecmp(e, e2) >= 0)
		return(n);

	return(e.slow);
}
#endif	PFS
