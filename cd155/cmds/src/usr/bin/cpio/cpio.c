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
 * OSF/1 Release 1.0.1
 */
/* @(#)cpio.c	6.2 14:13:00 3/15/91 SecureWare, Inc. */

#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: cpio.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:21:11 $";
#endif

/*
 * COMPONENT_NAME: (CMDARCH) archive files
 *
 * FUNCTIONS: cpio
 *
 * ORIGINS: 3, 27
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
 * cpio.c  1.29  com/cmd/arch,3.1,9021 3/21/90 17:57:09
 */
/*	Copyright (c) 1988 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/
/*
#ident	"cpio:cpio.c	1.30.1.11"
	/sccs/src/cmd/s.cpio.c
	cpio.c	1.30.1.11	1/11/86 13:46:48
 */
/* cpio.c	5.1 17:50:39 8/15/90 SecureWare */

/*
	cpio -- copy file collections
 */

#include <sys/secdefines.h>
#if SEC_BASE
#include <sys/security.h>
#include <prot.h>
#endif

#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <utime.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <locale.h>
#include <NLchar.h>
#ifdef	KJI
#include <NLctype.h>
#endif

#if SEC_BASE && MLTAPE
extern unsigned char	*ie_findbuff();
extern unsigned char	*ie_allocbuff();
#if SEC_MAC
extern char		*ie_stripmld();
#endif
#if SEC_ACL_SWARE
extern short		ie_omit_acls;
#endif
#if SEC_PRIV
extern short		ie_omit_privs;
#endif
#if SEC_FSCHANGE
extern short		ie_recovery_mode;
#endif
#endif /* SEC_BASE && MLTAPE */

#define EQ(x,y)	(strcmp(x,y)==0)
#define MKSHORT(v,lv) {U.l=1L;if(U.c[0]) U.l=lv,v[0]=U.s[1],v[1]=U.s[0]; else U.l=lv,v[0]=U.s[0],v[1]=U.s[1];}
#if SEC_BASE && MLTAPE
#define	MAGIC	071727		/* mltape magic number */
#else
#define MAGIC	070707		/* cpio magic number */
#endif
#define IN	1		/* copy in */
#define OUT	2		/* copy out */
#define PASS	3		/* direct copy */
#define HDRSIZE	(Hdr.h_name - (char *)&Hdr)	/* header size minus filename field */
#define LINKS	1000		/* max no. of links allowed */
#if SEC_BASE && MLTAPE
#define CHARS	94		/* ASCII header size minus filename field */
#else
#define CHARS	76		/* ASCII header size minus filename field */
#endif
#define BUFSIZE 512		/* In u370, can't use BUFSIZ nor BSIZE */
#define CPIOBSZ 4096		/* file read/write */
#define TTYNAME	"/dev/tty"

#include <nl_types.h>
#include "cpio_msg.h"
nl_catd catd;
#define MSGSTR(Num,Str) catgets(catd,MS_CPIO,Num,Str)

struct	stat	Statb, Xstatb;
struct utimbuf tb;

	/* Cpio header format */
struct header {
	short	h_magic,
		h_dev;
	ushort	h_ino,
		h_mode,
		h_uid,
		h_gid;
#if SEC_BASE && MLTAPE
	short	h_Secsize,
		h_Mld,
		h_Tag;
#endif
	short	h_nlink,
		h_rdev,
		h_mtime[2],
		h_namesize,
		h_filesize[2];
	char	h_name[256];
} Hdr;

char		minorhi[4];		/* for high byte of minor, if any */
unsigned	Bufsize = BUFSIZE;		/* default record size */
short	Buf[CPIOBSZ/2], *Dbuf;
char	BBuf[CPIOBSZ], *Cbuf, linkname[PATH_MAX+1];
int	Wct, Wc;
short	*Wp;
char	*Cp;
	char timbuf[26];
	size_t strftime();
	struct tm *localtime();


short	Option,
	Dir,
	Uncond,
	Link,
#if SEC_BASE
	Lflag,	/* non-security enhancement */
#endif
	Rename,
	Toc,
	Verbose,
	Select,
	Mod_time,
	Acc_time,
	Cflag,
	fflag,
	Swap,
	byteswap,
	bothswap,
	halfswap;

int	Ifile,
	Ofile,
	Input = 0,
	Output = 1;
long	Blocks,
	Longfile,
	Longtime;

char	Fullname[256],	/***	these values must be set to 256	***/
	Name[256];	/***	to support pathname of 128 characters	***/
int	Pathend;
int usrmask;

FILE	*Rtty,
	*Wtty;

char	*swfile;
char	*eommsg;

char	**Pattern = 0;
char	Strhdr[500];
char	*Chdr = Strhdr;
dev_t	Dev;
short	Uid,
	Gid,
	A_directory,
	A_special,
	A_symlink,
	Filetype = S_IFMT;

extern	errno;
extern	char	*getcwd () ;
char	*malloc();
short	 encode();
dev_t	 decode();

union { long l; short s[2]; char c[4]; } U;

long mklong(v)
short v[];
{
	U.l = 1;
	if(U.c[0])
		U.s[0] = v[1], U.s[1] = v[0];
	else
		U.s[0] = v[0], U.s[1] = v[1];
	return (U.l);
}

main(argc, argv)
char **argv;
{
	register ct;
 	long	filesz;
 	long	bufsz = 0;
	register char *fullp;
	register i, symlsz;
	int ans;
	extern char *optarg;
	extern int  optind;

	(void) setlocale(LC_ALL, "");
	catd = catopen(MF_CPIO,0);

#if SEC_BASE
#if MLTAPE
	ie_init(argc, argv, 1);
#else
	ie_init(argc, argv, 0);
#endif
#endif
	signal(SIGSYS, SIG_IGN);
	if(argc < 2 || *argv[1] != '-')
		usage();
	Uid = getuid();
	usrmask = umask((mode_t)0);
	umask(usrmask);
	Gid = getgid();

#if SEC_BASE
#if SEC_FSCHANGE
#if MLTAPE
	/* The -R, -A, and -P -F options are security enhancements */
	while ((ans = getopt(argc, argv, "aABC:ifopPcdlLmrRSsbtuvM:6eI:O:F:")) != EOF)
#else /* !MLTAPE */
	/* The -R option is a security enhancement */
	while ((ans = getopt(argc, argv, "aBC:ifopcdlLmrRSsbtuvM:6eI:O:")) != EOF)
#endif /* !MLTAPE */
#else /* !SEC_FSCHANGE */
	/* The -L option is a non-security enhancement */
	while ((ans = getopt(argc, argv, "aBC:ifopcdlLmrSsbtuvM:6eI:O:")) != EOF)
#endif /* !SEC_FSCHANGE */
#else /* !SEC_BASE */
	while ((ans = getopt(argc, argv, "aBC:ifopcdlmrSsbtuvM:6eI:O:")) != EOF)
#endif /* !SEC_BASE */
	{
		switch(ans) {
		case 'a':		/* reset access time */
			Acc_time++;
			break;
		case 'B':		/* change record size to 5120 bytes */
 			if(bufsz)
				fprintf(stderr,
				MSGSTR(COBS,"Conflicting options, 'B' superceding 'C'\n"));
			Bufsize = 5120;
			break;
 		case 'C':
#if SEC_BASE	/* non-security enhancement */
			bufsz = getbufsize(optarg);
#else
			bufsz = atoi(optarg);
#endif
			if (bufsz == 0) {
 				fprintf(stderr, MSGSTR(BBS,
					"Invalid buffer size '%s'\n"), optarg);
 				exit(2);
 			}
 			if(Bufsize != BUFSIZE)
 				fprintf(stderr,
				MSGSTR(COCS,"Conflicting options, 'C' superceding previous option\n"));
 			Bufsize = bufsz;
 			break;
		case 'i':
			Option = IN;
			break;
		case 'f':	/* do not consider patterns in cmd line */
			fflag++;
			break;
		case 'o':
			Option = OUT;
			break;
		case 'p':
			Option = PASS;
			break;
		case 'c':		/* ASCII header */
			Cflag++;
			break;
		case 'd':		/* create directories when needed */
			Dir++;
			break;
		case 'l':		/* link files, when necessary */
			Link++;
			break;
#if SEC_BASE			/* non-security enhancement */
		case 'L':		/* follow symbolic links */
			Lflag++;
			break;
#endif
		case 'm':		/* retain mod time */
			Mod_time++;
			break;
		case 'r':		/* rename files interactively */
			Rename++;
			Rtty = fopen(TTYNAME, "r");
			Wtty = fopen(TTYNAME, "w");
			if(Rtty==NULL || Wtty==NULL) {
				fprintf(stderr,
				MSGSTR(CRNDT,"Cannot rename (%s missing)\n"),
					TTYNAME);
				exit(2);
			}
			break;
		case 'S':		/* swap halfwords */
			halfswap++;
			Swap++;
			break;
		case 's':		/* swap bytes */
			byteswap++;
			Swap++;
			break;
		case 'b':
			bothswap++;
			Swap++;
			break;
		case 't':		/* table of contents */
			Toc++;
			break;
		case 'u':		/* copy unconditionally */
			Uncond++;
			break;
		case 'v':		/* verbose table of contents */
			Verbose++;
			break;
		case '6':		/* for old, sixth-edition files */
			Filetype = 060000;
			break;
		case 'M':		/* alternate message for end-of-media */
			eommsg = optarg;
			break;
		case 'I':
			chkswfile(swfile, ans, Option);
#if SEC_BASE
			stopio(optarg);
#endif
			if ((i = open(optarg, O_RDONLY)) < 0 ||
			   dup2(i, Input) < 0) {
				fprintf(stderr, MSGSTR(EINPUT,
					"Cannot open <%s> for input: "),
					optarg);
				perror("");
				exit(2);
			}
			swfile = optarg;
			break;
		case 'O':
			chkswfile(swfile, ans, Option);
#if SEC_BASE
			stopio(optarg);
#endif
			if ((i = open(optarg, O_WRONLY|O_CREAT|O_TRUNC, 0666))
			   < 0  ||  dup2(i, Output) < 0) {
				fprintf(stderr, MSGSTR(EOUTPUT,
					"Cannot open <%s> for output: "),
					optarg);
				perror("");
				exit(2);
			}
			swfile = optarg;
			break;
#if MLTAPE
		case 'F':
			if (Option == IN) {
			        fprintf(stderr,
			                "-F can only be used with -o or -p\n");
			        exit(1);
			}

			ie_ml_find(optarg);
			break;

#if SEC_FSCHANGE
		case 'R':
			ie_recovery_mode = 1;
			break;
#endif
#if SEC_ACL_SWARE
		case 'A':
			ie_omit_acls = 1;
			break;
#endif
#if SEC_PRIV
		case 'P':
			ie_omit_privs = 1;
			break;
#endif
#endif /* MLTAPE */
		default:
			usage();
		}
	}
#if SEC_BASE && MLTAPE
	Cflag = 1;
#endif
	if(!Option) {
		fprintf(stderr,
		MSGSTR(OMIL,"Options must include o|i|p\n"));
		exit(2);
	}

#if SEC_BASE
	/* Reset egid to user's real gid */
	setgid(getgid());
#endif

	if(Option == PASS) {
		if(Rename) {
			fprintf(stderr,
			MSGSTR(PREX,"Pass and Rename cannot be used together\n"));
			exit(2);
		}
 		if(Bufsize != BUFSIZE) {
 			fprintf(stderr,
			MSGSTR(BCIRR,"`B' or 'C' option is irrelevant with the '-p' option\n"));
			Bufsize = BUFSIZE;
		}

	}else  {
		bufsz = Bufsize;
		while(((Cp = malloc(Bufsize)) == NULL) &&
				Bufsize > 512) Bufsize -= CPIOBSZ;
		if(Cp == NULL) {
			fprintf(stderr,
			MSGSTR(NOMLC,"Unable to malloc buffer space\n"));
			exit(2);
		}
		if(bufsz != Bufsize)
			fprintf(stderr,
			MSGSTR(BSR,"blocksize reduced to %u\n"),Bufsize);
		if(Cflag)
		    Cbuf = Cp;
		else 
		{
		    Wp = Dbuf = (short *)Cp;
		    Cp = NULL;
		}
	}
	Wct = Bufsize >> 1;
	Wc = Bufsize;
	argc -= optind;
	argv += optind;
	if (!eommsg)
		eommsg = MSGSTR(NEXTVOL,
			"Change to volume %d and press RETURN key. [q] ");

	switch(Option) {

	case OUT:		/* get filename, copy header and file out */
		if (argc != 0)
			usage();
#if SEC_BASE
		ie_check_device(swfile, AUTH_DEV_EXPORT);
#endif
		while(getname()) {
			if( mklong(Hdr.h_filesize) == 0L) {
			   if( Cflag )
				writehdr(Chdr,CHARS+Hdr.h_namesize);
			   else
				bwrite(&Hdr, HDRSIZE+Hdr.h_namesize);
#if SEC_BASE && MLTAPE
				writehdr(ie_findbuff(), Hdr.h_Secsize);
#endif
				continue;
			} else if( A_symlink ) {
				symlsz = (int) mklong(Hdr.h_filesize);
				if (readlink(Hdr.h_name, linkname, symlsz) < 0){
					printf(MSGSTR(CRSL,
					    "Cannot read symbolic link <%s>: "),
					     Hdr.h_name);
					perror("");
					continue;
				}
				linkname[symlsz] = '\0';
				if ( Cflag ) {
					writehdr(Chdr,CHARS+Hdr.h_namesize);
#if SEC_BASE && MLTAPE
					writehdr(ie_findbuff(), Hdr.h_Secsize);
#endif
					writehdr(linkname, symlsz);
				} else {
					bwrite(&Hdr, HDRSIZE+Hdr.h_namesize);
					bwrite(linkname, symlsz);
				}
				if(Verbose)
					(void) fprintf(stderr, "%s\n",
					    Hdr.h_name);
				continue;
			}
			if((Ifile = open(Hdr.h_name, 0)) < 0) {
				fprintf(stderr,
				MSGSTR(IHDRQ,"<%s> ?\n"), Hdr.h_name);
				continue;
			}
			if ( Cflag )
				writehdr(Chdr,CHARS+Hdr.h_namesize);
			else
				bwrite(&Hdr, HDRSIZE+Hdr.h_namesize);
#if SEC_BASE && MLTAPE
			writehdr(ie_findbuff(), Hdr.h_Secsize);
#endif
			for(filesz=mklong(Hdr.h_filesize); filesz>0; filesz-= CPIOBSZ){
				ct = filesz>CPIOBSZ ? CPIOBSZ: filesz;
				if(read(Ifile, Cflag ? BBuf: (char *)Buf, ct) == -1) {
					fprintf(stderr,
					MSGSTR(CRS,"Cannot read %s\n"), Hdr.h_name);
					continue;
				}
				Cflag ? writehdr(BBuf,ct): bwrite(Buf,ct);
			}
			close(Ifile);
			if(Acc_time){
				tb.actime = Statb.st_atime;
				tb.modtime = Statb.st_mtime;
				utime(Hdr.h_name, &tb);
			}
			if(Verbose)
				fprintf(stderr,"%s\n", Hdr.h_name);
		}

	/* copy trailer, after all files have been copied */
		strcpy(Hdr.h_name, "TRAILER!!!");
		Hdr.h_magic = MAGIC;
#if SEC_BASE && MLTAPE
		Hdr.h_Secsize = 0;
#endif
		MKSHORT(Hdr.h_filesize, 0L);
		Hdr.h_namesize = strlen("TRAILER!!!") + 1;
		if ( Cflag )  {
			bintochar(0L);
			writehdr(Chdr,CHARS+Hdr.h_namesize);
		}
		else
			bwrite(&Hdr, HDRSIZE+Hdr.h_namesize);
		Cflag ? writehdr(Cbuf, Bufsize): bwrite(Dbuf, Bufsize);
		break;

	case IN:
		if (argc > 0) {	/* save patterns, if any */
			Pattern = argv;
		}
#if SEC_BASE
		ie_check_device(swfile, AUTH_DEV_IMPORT);
#endif
		pwd();
		while(gethdr()) {
		    if (A_symlink) {
				symlsz = (int) mklong(Hdr.h_filesize);
				if (Cflag)
					readhdr(linkname, symlsz);
				else
					bread(linkname, symlsz);
				linkname[symlsz] = '\0';
				if (ckname(Hdr.h_name))
					if (!openout(Hdr.h_name, linkname))
						continue;
		    } else {
				/*	This if statement has been added
				**	in order to ignore any special
				**	files that may exist on the
				**	input archive. None of the code
				**	for handling the special files has
				**	been deleted.
				**  FIFO's will be archived
				*/
			if ( A_special && (Hdr.h_mode & S_IFMT) != S_IFIFO)
				Ofile = 0 ;
			else if (ckname(Hdr.h_name))
				Ofile = openout(Hdr.h_name, (char *)0);
			else
				Ofile = 0;
			for(filesz=mklong(Hdr.h_filesize); filesz>0; filesz-= CPIOBSZ){
				ct = filesz>CPIOBSZ ? CPIOBSZ: filesz;
				Cflag ? readhdr(BBuf,ct): bread(Buf, ct);
				if(Ofile) {
					if(Swap)
					   Cflag ? swap(BBuf,ct): swap(Buf,ct);
					if(write(Ofile, Cflag ? BBuf: (char *)Buf, ct) == -1) {
						fprintf(stderr,
						MSGSTR(CWF,"Cannot write %s\n"), Hdr.h_name);
						perror("cpio ");
					 continue;
					}
				}
			}
			if(Ofile) {
				close(Ofile);
#if SEC_BASE
				if (ie_chmod(Hdr.h_name, Hdr.h_mode,
						A_directory) < 0)
#else
				if(Uid) {
					i = (Hdr.h_mode & (0777000 | (~usrmask & 0777)));
				}
				else 
					i = Hdr.h_mode;
				if(chmod(Hdr.h_name, (mode_t)i) < 0)
#endif
				{
					fprintf(stderr,
					MSGSTR(CCM,"Cannot chmod <%s> (errno:%d)\n"), Hdr.h_name, errno);
				}
				set_time(Hdr.h_name, mklong(Hdr.h_mtime), mklong(Hdr.h_mtime));
#if SEC_PRIV && MLTAPE
				ie_setprivs(Hdr.h_name);
#endif
#if SEC_MAC && MLTAPE
				ie_resetlev();
#endif
			}
			/* ignore special files except for FIFO's */
			if(A_special && (Hdr.h_mode & S_IFMT != S_IFIFO))
			{
				fprintf ( stderr , MSGSTR ( IGNSPECIAL ,
				"special file <%s> ignored\n" ) ,
					Hdr.h_name ) ;
				continue ;
			}
		    }

		    if(Select)
			if(Verbose) {
				if(Toc)
					pentry(Hdr.h_name);
				else if (Ofile || A_symlink)
					puts(Hdr.h_name);
			}
			else if(Toc)
				puts(Hdr.h_name);
		}
		break;

	case PASS:		/* move files around */
		if(argc != 1)
			usage();
		if(access(argv[0], 2) == -1) {
			fprintf(stderr, MSGSTR(CWIF, "Cannot write in <%s>\n"),
				argv[0]);
			exit(2);
		}
		strcpy(Fullname, argv[0]);	/* destination directory */
		if (stat(Fullname, &Xstatb) < 0 ||
		   (Xstatb.st_mode&S_IFMT) != S_IFDIR) {
			fprintf(stderr, MSGSTR(CWIF, "Cannot write in <%s>\n"),
				argv[0]);
			exit(2);
		}
		Dev = Xstatb.st_dev;
		i = strlen(Fullname);
		if (Fullname[i-1] != '/') {
			Fullname[i++] = '/';
			Fullname[i] = '\0';
		}
		fullp = Fullname + i;
#if SEC_BASE
		ie_check_device(swfile, AUTH_DEV_PASS);
#endif

		while(getname()) {
			if(!ckname(Hdr.h_name))
				continue;
			i = 0;
			while(Hdr.h_name[i] == '/')
				i++;
			strcpy(fullp, &(Hdr.h_name[i]));

			if (A_symlink) {
				if((symlsz = readlink(Hdr.h_name, linkname,
						BUFSIZE -1)) < 0) {
					printf(MSGSTR(CRSL,
					    "Cannot read symbolic link <%s>: "),
					    Hdr.h_name);
					perror("");
					continue;
				}
				linkname[symlsz] = '\0';
				if (openout(Fullname, linkname)) {
					Blocks +=
					    ((symlsz + (BUFSIZE-1)) / BUFSIZE);
					if(Verbose)
						puts(Fullname);
				}
				continue;
			}

			if (Link  &&  !A_directory  &&  Dev == Statb.st_dev) {
			    if(link(Hdr.h_name, Fullname) < 0) {
				struct stat sb;

				switch (errno) {
				    case EEXIST:	/* already there */
					if (Uncond || (lstat(Fullname, &sb) == 0
					  && mklong(Hdr.h_mtime) > sb.st_mtime))
						unlink(Fullname);
					else {
					    fprintf(stderr, MSGSTR(CFN,
					    "current <%s> newer or same age\n"),
					    Fullname);
					    continue;
					}
					break;

				    case ENOENT:	/* missing dir.? */
					if (missdir(Fullname) != 0) {
					    fprintf(stderr, MSGSTR(CCD,
						"Cannot create directory for <%s> (errno:%d)\n"),
						Fullname, errno);
					    continue;
					}
					break;
				
				    default:	/* not sure - try again */
					unlink(Fullname);
					missdir(Fullname);
					break;
				}
				/* try link once more */
				if (link(Hdr.h_name, Fullname) < 0) {
					fprintf(stderr, MSGSTR(CLSTS,
						"Cannot link <%s> & <%s>: "),
						Hdr.h_name, Fullname);
					continue;
				}
			    }
			    goto ckverbose;
			}
			if(!(Ofile = openout(Fullname, (char *)0)))
				continue;
			if((Ifile = open(Hdr.h_name, 0)) < 0) {
				fprintf(stderr,
				MSGSTR(IHDRQ,"<%s> ?\n"), Hdr.h_name);
				close(Ofile);
				continue;
			}
			filesz = Statb.st_size;
			for(; filesz > 0; filesz -= CPIOBSZ) {
				ct = filesz>CPIOBSZ ? CPIOBSZ: filesz;
				if(read(Ifile, Buf, ct) == -1) {
					fprintf(stderr,
					MSGSTR(CRS,"Cannot read %s\n"), Hdr.h_name);

					break;
				}
				if(Ofile)
					if(write(Ofile, Buf, ct) == -1) {
					 fprintf(stderr,
					 MSGSTR(CWF,"Cannot write %s\n"), Hdr.h_name);
					 perror("cpio ");
					 break;
					}
				Blocks += ((ct + (BUFSIZE - 1)) / BUFSIZE);
			}
			close(Ifile);
			if(Acc_time){
				tb.actime = Statb.st_atime;
				tb.modtime = Statb.st_mtime;
				utime(Hdr.h_name, &tb);
			};
			if(Ofile) {
				close(Ofile);
#if SEC_BASE
				if (ie_chmod(Fullname, Hdr.h_mode,
						A_directory) < 0)
#else
				if(Uid) {
					i = (Hdr.h_mode & (0777000 | (~usrmask & 0777)));
				}
				else 
					i = Hdr.h_mode;
				if(chmod(Fullname, (mode_t)i) < 0)
#endif
				{
					fprintf(stderr,
					MSGSTR(CCM,"Cannot chmod <%s> (errno:%d)\n"), Fullname, errno);
				}
				set_time(Fullname, Statb.st_atime, mklong(Hdr.h_mtime));
#if SEC_FSCHANGE && MLTAPE
				ie_copyattr(Hdr.h_name, Fullname);
#endif
ckverbose:
				if(Verbose)
					puts(Fullname);
			}
		}
	}
	/* print number of blocks actually copied */
	   fprintf(stderr,
	   MSGSTR(NOBLKCOP,"%ld blocks\n"), Blocks * (Bufsize>>9));
	exit(0);
}
usage()
{
#if SEC_BASE
#if MLTAPE
	char	*prg = "mltape";
	char	*ofl = "-o[acvABLP]";
	char	*ifl = "-i[bcdmrstuvfABPRS6]";
	char	*pfl = "-p[adlmruvALP]";
#else /* !MLTAPE */
	char	*prg = "cpio";
	char	*ofl = "-o[acvBL]";
	char	*ifl = "-i[bcdmrstuvfBRS6]";
	char	*pfl = "-p[adlmruvL]";
#endif /* !MLTAPE */
#else /* !SEC_BASE */
	char	*prg = "cpio";
	char	*ofl = "-o[acvB]";
	char	*ifl = "-i[bcdmrstuvfBS6]";
	char	*pfl = "-p[adlmruv]";
#endif /* !SEC_BASE */

    fprintf(stderr,
	MSGSTR(USAGE1, "Usage: %s %s [-Cbufsize] <name-list >collection\n"),
	prg, ofl);
    fprintf(stderr,
	MSGSTR(USAGE2, "\t%s %s [-Cbufsize] -Ocollection <name-list\n"),
	prg, ofl);
    fprintf(stderr,
	MSGSTR(USAGE3, "\t%s %s [-Cbufsize] [pattern ...] <collection\n"),
	prg, ifl);
    fprintf(stderr,
	MSGSTR(USAGE4, "\t%s %s [-Cbufsize] -Icollection [pattern ...]\n"),
	prg, ifl);
    fprintf(stderr,
	MSGSTR(USAGE5, "\t%s %s directory <name-list\n"),
	prg, pfl);

    exit(2);
}

#if SEC_BASE	/* non-security enhancement */
getbufsize(cp)
	register char *cp;
{
	register int    size = 0;

	size = atoi(cp);
	while (isdigit(*cp))
		++cp;
	switch (*cp) {
	case 'b':
		size *= 512;
		break;
	case 'k':
		size *= 1024;
		break;
	case 'm':
		size *= 1024 * 1024;
		break;
	}
	return size;
}
#endif

chkswfile(sp, c, option)
char	*sp;
char	c;
short	option;
{
	if (!option) {
		fprintf(stderr, MSGSTR(USAGEOPT1,
			"-%c must be specified before -%c option"),
			c == 'I' ? 'i' : 'o', c );
		exit(2);
	}
	if ((c == 'I' && option != IN)  ||  (c == 'O' && option != OUT)) {
		fprintf(stderr, MSGSTR(USAGEOPT2,
			"-%c option not permitted with -%c option"), c, option);
		exit(2);
	}
	if (sp) {
		fprintf(stderr, MSGSTR(USAGEOPT3,
			"No more than one -I or -O flag permitted"));
		exit(2);
	}
	return;
}

getname()		/* get file name, get info for header */
{
	register char *namep = Name;
	register ushort ftype;
	long tlong;
#if SEC_BASE	/* non-security enhancement */
	extern int	stat(), lstat();
#endif
#if SEC_MAC && MLTAPE
	char *tempname;
#endif

	for(;;) {
		/*
		 * Sware Bug fix.  Must reset namep on each iteration since we
		 * may iterate several times before returning.
		 */
		namep = Name;
#if SEC_MAC && MLTAPE
		ie_resetlev();
#endif
		if(gets(namep) == NULL)
			return (0);
		if(*namep == '.' && namep[1] == '/') {
			namep++;
			while (*namep == '/') namep++;
		}
#if SEC_MAC && MLTAPE
		/*
		 * Determine whether mld or mld child, set header values,
		 * skip if mld child only.
		 */
		tempname = ie_stripmld(namep, &Hdr.h_Mld, &Hdr.h_Tag);
		if (tempname == (char *) -1)
			return 0;
		if (!tempname)
			continue;
		namep = tempname;
		if (Hdr.h_Mld)
			enablepriv(SEC_MULTILEVELDIR);
#endif
		strcpy(Hdr.h_name, namep);
#if SEC_BASE	/* non-security enhancement */
		if ((Lflag ? stat(namep, &Statb) : lstat(namep, &Statb)) < 0)
#else
		if(lstat(namep, &Statb) < 0)
#endif
		{
			fprintf(stderr,
			MSGSTR(IHDRQS,"< %s > ?\n"), Hdr.h_name);
#if SEC_MAC && MLTAPE
			disablepriv(SEC_MULTILEVELDIR);
#endif
			continue;
		}
#if SEC_MAC && MLTAPE
		disablepriv(SEC_MULTILEVELDIR);
#endif
		ftype = Statb.st_mode & Filetype;
		A_directory = (ftype == S_IFDIR);
		A_symlink = (ftype == S_IFLNK);
		A_special = (ftype == S_IFBLK)
			|| (ftype == S_IFCHR)
			|| (ftype == S_IFIFO);
#if SEC_BASE && MLTAPE
		Hdr.h_Secsize = ie_ml_export(namep, A_symlink, A_directory);
		if (!Hdr.h_Secsize)
			continue;
#endif
#if SEC_ARCH && !MLTAPE
		if (!ie_sl_export(namep, A_symlink))
			continue;
#endif

			/*	This if statement will not allow special
			**	files to be archived. This is due to the
			**	change in the major and minor numbers
			**	from 8 bit numbers to 16 bit numbers.
			**	None of the code has been deleted which
			**	handles the archiving of special files.
			**  FIFO's will be archived
			*/
		if ( A_special  && (ftype != S_IFIFO))	/* skip special files */
		{
			fprintf ( stderr , MSGSTR ( NOSPECIAL ,
				"special file <%s> not archived\n" ) ,
				namep ) ;
			continue ;
		}

		Hdr.h_magic = MAGIC;
		Hdr.h_namesize = strlen(Hdr.h_name) + 1;
		Hdr.h_uid = (ushort)Statb.st_uid;
		Hdr.h_gid = (ushort)Statb.st_gid;
		Hdr.h_dev = (short) Statb.st_dev;
		Hdr.h_ino = (ushort)Statb.st_ino;
		Hdr.h_mode = (ushort)Statb.st_mode;
		MKSHORT(Hdr.h_mtime, Statb.st_mtime);
		Hdr.h_nlink = Statb.st_nlink;
		tlong = (((Hdr.h_mode&S_IFMT) == S_IFREG) || ((Hdr.h_mode&S_IFMT) == S_IFLNK)) ? Statb.st_size: 0L;
		MKSHORT(Hdr.h_filesize, tlong);
		Hdr.h_rdev = encode(Statb.st_rdev, minorhi);
		strcpy(Hdr.h_name + Hdr.h_namesize, minorhi);
		if( Cflag )
			bintochar(tlong);
		else
			Hdr.h_namesize += sizeof(minorhi);
		return (1);
	}
}

bintochar(t)		/* ASCII header write */
long t;
{
#if SEC_BASE && MLTAPE
	sprintf(Chdr,"%.6o%.6ho%.6ho%.6ho%.6ho%.6ho%.6ho%.6ho%.6ho%.6ho%.6ho%.11lo%.6ho%.11lo%s",
		MAGIC,Hdr.h_dev & 00000177777,Hdr.h_ino,Hdr.h_mode,
		Hdr.h_uid,Hdr.h_gid,Hdr.h_Secsize,Hdr.h_Mld,Hdr.h_Tag,
		Hdr.h_nlink, Hdr.h_rdev & 00000177777,
		Statb.st_mtime,(short)strlen(Hdr.h_name)+1,t,Hdr.h_name);
#else
	sprintf(Chdr,"%.6o%.6ho%.6ho%.6ho%.6ho%.6ho%.6ho%.6ho%.11lo%.6ho%.11lo%s",
		MAGIC,Hdr.h_dev & 00000177777,Hdr.h_ino,Hdr.h_mode,
		Hdr.h_uid,Hdr.h_gid,Hdr.h_nlink, Hdr.h_rdev & 00000177777,
		Statb.st_mtime,(short)strlen(Hdr.h_name)+1,t,Hdr.h_name);
#endif
}

chartobin()		/* ASCII header read */
{
#if SEC_BASE && MLTAPE
	sscanf(Chdr,"%6ho%6ho%6ho%6ho%6ho%6ho%6ho%6ho%6ho%6ho%6ho%11lo%6ho%11lo",
		&Hdr.h_magic,&Hdr.h_dev,&Hdr.h_ino,&Hdr.h_mode,&Hdr.h_uid,
		&Hdr.h_gid,&Hdr.h_Secsize,&Hdr.h_Mld,&Hdr.h_Tag,
		&Hdr.h_nlink,&Hdr.h_rdev,&Longtime,&Hdr.h_namesize,
		&Longfile);
#else
	sscanf(Chdr,"%6ho%6ho%6ho%6ho%6ho%6ho%6ho%6ho%11lo%6ho%11lo",
		&Hdr.h_magic,&Hdr.h_dev,&Hdr.h_ino,&Hdr.h_mode,&Hdr.h_uid,
		&Hdr.h_gid,&Hdr.h_nlink,&Hdr.h_rdev,&Longtime,&Hdr.h_namesize,
		&Longfile);
#endif
	MKSHORT(Hdr.h_filesize, Longfile);
	MKSHORT(Hdr.h_mtime, Longtime);
}

gethdr()		/* get file headers */
{
	register ushort ftype;

#if SEC_MAC && MLTAPE
	ie_resetlev();
#endif
	if (Cflag)  {
		readhdr(Chdr,CHARS);
		chartobin();
	}
	else
		bread(&Hdr, HDRSIZE);

	if(Hdr.h_magic != MAGIC) {
		fprintf(stderr,MSGSTR(OOF,"Out of phase--get help\n"));
		exit(2);
	}
	if(Cflag)
		readhdr(Hdr.h_name, Hdr.h_namesize);
	else
	{
		bread(Hdr.h_name, Hdr.h_namesize);
		if (Hdr.h_namesize == strlen(Hdr.h_name) + 1 + sizeof(minorhi))
		{
		    Hdr.h_namesize -= sizeof(minorhi);
		    sscanf (Hdr.h_name + Hdr.h_namesize, "%3s", minorhi);
		}
		else if (Hdr.h_namesize != strlen(Hdr.h_name) + 1)
		{
		    fprintf(stderr, 
		    MSGSTR(LOFDM,"Length of file name doesn't match expected length.\n"));
		    exit(2);
		}
		else
		    minorhi[0] = '\0';
	}
	if(EQ(Hdr.h_name, "TRAILER!!!"))
		return (0);
	ftype = Hdr.h_mode & Filetype;
	A_directory = (ftype == S_IFDIR);
	A_special =(ftype == S_IFBLK)
		|| (ftype == S_IFCHR)
		|| (ftype == S_IFIFO);
	A_symlink = (ftype == S_IFLNK);
#if SEC_BASE && MLTAPE
	readhdr(ie_allocbuff(Hdr.h_Secsize), Hdr.h_Secsize);
#endif
	return (1);
}

ckname(namep)	/* check filenames with patterns given on cmd line */
register char *namep;
{
	++Select;
	if(fflag ^ !nmatch(namep, Pattern)) {
		Select = 0;
		return (0);
	}
	if(Rename && !A_directory) {	/* rename interactively */
		fprintf(Wtty, MSGSTR(RPRPT, "Rename <%s>: "), namep);
		fgets(namep, sizeof(Hdr.h_name), Rtty);
		if(feof(Rtty))
			exit(2);
		namep[strlen(namep) - 1] = '\0';
		if(EQ(namep, "")) {
			printf(MSGSTR(SKIP,"Skipped\n"));
			return (0);
		}
	}
	return (!Toc);
}

openout(namep, symlname)   /* open files for writing, set all necessary info */
register char *namep;
{
	register f;
	register char *np;
	int ans;

	if(!strncmp(namep, "./", 2))
		namep += 2;
	np = namep;
	if(A_directory) {
		if(Rename
		|| EQ(namep, ".")
		|| EQ(namep, ".."))	/* do not consider . or .. files */
			return (0);
		if(stat(namep, &Xstatb) == -1) {

			if (!Dir)	{
				fprintf(stderr,
				MSGSTR(USE_D,"Use `-d' option to copy <%s>\n"),Hdr.h_name);
				return (0);
			}
/* try creating (only twice) */
 			missdir (namep) ;	/*  check for missing dirs  */
#if SEC_BASE && MLTAPE
			f = ie_ml_import(namep, 0777, 1, Hdr.h_Mld);
			if (f < 0)
				return 0;
			if (f == 0)
#else
 			if(makdir(namep) != 0)
#endif
			{
				fprintf(stderr,
				MSGSTR(CCD,"Cannot create directory for <%s> (errno:%d)\n"), namep, errno);
				return(0);
			}
			if (Verbose)
				puts(namep);
		}

ret:
#if SEC_FSCHANGE && MLTAPE
		if (Option == PASS)
			ie_copyattr(Hdr.h_name, namep);
#if SEC_MAC
		if (Hdr.h_Mld)
			enablepriv(SEC_MULTILEVELDIR);
#endif
#endif
#if SEC_BASE
		if (ie_chmod(namep, Hdr.h_mode, 1) < 0)
#else
		if(Uid) {
			f = (Hdr.h_mode & (0777000 | (~usrmask & 0777)));
		}
		else 
			f = Hdr.h_mode;
		if(chmod(namep, (mode_t)f) < 0)
#endif
		{
			fprintf(stderr,
			MSGSTR(CCM,"Cannot chmod <%s> (errno:%d)\n"), namep, errno);
		}
#if SEC_ARCH && !MLTAPE
		if (!ie_sl_set_attributes(namep)) {
			rmdir(namep);
			return 0;
		}
#endif
#if SEC_BASE
		if (hassysauth(SEC_OWNER))
#else
		if(Uid == 0)
#endif
			if(chown(namep, Hdr.h_uid, Hdr.h_gid) < 0) {
				fprintf(stderr,
				MSGSTR(CCO,"Cannot chown <%s> (errno:%d)\n"), namep, errno);
			}
		set_time(namep, mklong(Hdr.h_mtime), mklong(Hdr.h_mtime));
#if SEC_MAC && MLTAPE
		if (Hdr.h_Mld)
			disablepriv(SEC_MULTILEVELDIR);
#endif
		return (0);
	}
	if(Hdr.h_nlink > 1)
		if(!postml(namep, np))
			return (0);
	if(lstat(namep, &Xstatb) == 0) {
#if SEC_BASE
		if (Uncond && (!A_special && (Xstatb.st_mode & S_IWRITE) ||
				hassysauth(SEC_OWNER)))
#else
		if(Uncond && !((!(Xstatb.st_mode & S_IWRITE) || A_special) && (Uid != 0)))
#endif
		{
			if(unlink(namep) < 0) {
				fprintf(stderr,
				MSGSTR(CUL,"cannot unlink current <%s> (errno:%d)\n"), namep, errno);
			}
		}
		if(!Uncond && (mklong(Hdr.h_mtime) <= Xstatb.st_mtime)) {
		/* There's a newer version of file on destination */
			fprintf(stderr,
				MSGSTR(CFN,"current <%s> newer or same age\n"),
				np);
			return (0);
		}
	}
	if(Option == PASS
	&& Hdr.h_ino == Xstatb.st_ino
	&& Hdr.h_dev == Xstatb.st_dev) {
		fprintf(stderr,
			MSGSTR(ATPFTS,"Attempt to pass file to self!\n"));
		exit(2);
	}
	if (A_symlink) {		/* try symlinking (only twice) */
		ans = 0;
		unlink(namep);
		do {
			if (symlink(symlname, namep) < 0)
				ans++;
			else {
				ans = 0;
				break;
			}
		} while (ans < 2 && missdir(np) == 0);
		if (ans == 1) {
			fprintf(stderr, MSGSTR(CCD,
			    "Cannot create directory for <%s> (errno:%d)\n"),
			    namep, errno);
			return(0);
		}else if (ans == 2) {
			fprintf(stderr, MSGSTR(CCLN,
				"Cannot symlink <%s> and <%s>: "),
				namep, symlname);
			perror("");
			return(0);
		}
#if SEC_BASE && MLTAPE
		if (ie_ml_import(namep, 0, 0, 1) != 1) {
			unlink(namep);
			return 0;
		}
#endif
#if SEC_ARCH && !MLTAPE
		if (!ie_sl_set_attributes(namep)) {
			unlink(namep);
			return 0;
		}
#endif
		return(1);
	}
	if(A_special) {
		if((Hdr.h_mode & Filetype) == S_IFIFO) {
			Hdr.h_rdev = 0;
			minorhi[0] = '\0';
		}

/* try creating (only twice) */
		ans = 0;
		do {
#if SEC_BASE
#if MLTAPE
			f = ie_ml_import(namep, Hdr.h_mode, 2,
					decode(Hdr.h_rdev, minorhi));
			if (f < 0)
				return 0;
			if (f == 0)
#else /* !MLTAPE */
			if (ie_openout(namep, Hdr.h_mode, 2,
					decode(Hdr.h_rdev, minorhi)) < 0)
#endif /* !MLTAPE */
#else /* !SEC_BASE */
			if(mknod(namep, Hdr.h_mode, decode(Hdr.h_rdev, minorhi)) 
									 < 0)
#endif /* !SEC_BASE */
			{
				ans += 1;
			}else {
				ans = 0;
				break;
			}
		}while(ans < 2 && missdir(np) == 0);
		if(ans == 1) {
			fprintf(stderr,
			MSGSTR(CCD,"Cannot create directory for <%s> (errno:%d)\n"), namep, errno);
			return(0);
		}else if(ans == 2) {
			fprintf(stderr,
			MSGSTR(CNMN,"Cannot mknod <%s> (errno:%d)\n"), namep, errno);
			return(0);
		}
		if(Option == PASS && Verbose)
			 puts(namep);
		goto ret;
	}

/* try creating (only twice) */
	ans = 0;
	do {
#if SEC_BASE && MLTAPE
		f = ie_ml_import(namep, Hdr.h_mode, 0, 0);
		if (f < 0)
			return 0;
		if (f == 0)
#else
		if((f = creat(namep, Hdr.h_mode)) < 0)
#endif
		{
			ans += 1;
		}else {
			ans = 0;
			break;
		}
	}while(ans < 2 && missdir(np) == 0);
	if(ans == 1) {
		fprintf(stderr,
		MSGSTR(CCD,"Cannot create directory for <%s> (errno:%d)\n"), namep, errno);
		return(0);
	}else if(ans == 2) {
		fprintf(stderr,
		MSGSTR(CCF,"Cannot create <%s> (errno:%d)\n"), namep, errno);
		return(0);
	}
#if SEC_ARCH && !MLTAPE
	if (!ie_sl_set_attributes(namep)) {
		unlink(namep);
		return 0;
	}
#endif

#if SEC_BASE
	if (hassysauth(SEC_OWNER))
#else
	if(Uid == 0)
#endif
		chown(namep, Hdr.h_uid, Hdr.h_gid);
	return (f);
}

bread(b, c)
register c;
register short *b;
{
	static nleft = 0;
	static short *ip;
	register int rv;
	register short *p = ip;
	register int in;

	c = (c+1)>>1;
	while(c--) {
		if(nleft == 0) {
			in = 0;
			while((rv=read(Input, &(((char *)Dbuf)[in]), Bufsize - in)) != Bufsize - in) {
				if(rv <= 0) {
					Input = chgreel(0, Input, rv);
					continue;
				}
				in += rv;
				nleft += (rv >> 1);
			 	if((rv % 2) || (rv == 1)){ 
					rv = read(Input, &(((char *)Dbuf)[in++]),1);
					++nleft;
				}
			}
			nleft += (rv >> 1);
			p = Dbuf;
			++Blocks;
		}
		*b++ = *p++;
		--nleft;
	}
	ip = p;
}

readhdr(b, c)
register c;
register char *b;
{
	static nleft = 0;
	static char *ip;
	register int rv;
	register char *p = ip;
	register int in;

	while(c--)  {
		if(nleft == 0) {
			in = 0;
			while((rv=read(Input, &(((char *)Cbuf)[in]), Bufsize - in)) != Bufsize - in) {
				if(rv <= 0) {
					Input = chgreel(0, Input, rv);
					continue;
				}
				in += rv;
				nleft += rv;
			}
			nleft += rv;
			p = Cbuf;
			++Blocks;
		}
		*b++ = *p++;
		--nleft;
	}
	ip = p;
}

bwrite(rp, c)
register short *rp;
register c;
{
	short		*wp = Wp;
	char		*buf_adr = (char *) Dbuf;
	int		wr_size = Bufsize;
	int		wr ;

	c = (c+1) >> 1;
	while(c--) {
		if(!Wct) {
again:
			wr = write (Output, buf_adr, wr_size) ;
			if (wr <= 0) {
				Output = chgreel(1, Output, wr);
				goto again;
			}
			else
				if ( wr != wr_size )
				{
					wr_size -= wr ;
					buf_adr += wr ;
					goto again ;
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

writehdr(rp, c)
register char *rp;
register c;
{
	char		*cp = Cp;
	char		*buf_adr = (char *) Cbuf;
	int		wr_size = Bufsize;
	int		wr ;

	while(c--)  {
		if(!Wc)  {
again:
			wr = write(Output, buf_adr, wr_size) ;
			if( wr <= 0)  {
				Output = chgreel(1, Output, wr);
				goto again;
			}
			else
				if ( wr != wr_size )
				{
					wr_size -= wr ;
					buf_adr += wr ;
					goto again ;
				}
			Wc = Bufsize;
			cp = Cbuf;
			++Blocks;
		}
		*cp++ = *rp++;
		--Wc;
	}
	Cp = cp;
}

postml(namep, np)		/* linking funtion */
register char *namep, *np;
{
	register i;
	static struct ml {
		short	m_dev,
			m_ino;
		char	m_name[2];
	} *ml[LINKS];
	static	mlinks = 0;
	char *mlp;
	int ans;

	for(i = 0; i < mlinks; ++i) {
		if(mlinks == LINKS) break;
		if(ml[i]->m_ino==Hdr.h_ino &&
			ml[i]->m_dev==Hdr.h_dev) {
			if(Verbose)
			  printf(MSGSTR(FLTF,"%s linked to %s\n"), ml[i]->m_name,
				np);
			unlink(namep);
			if(Option == IN && *ml[i]->m_name != '/') {
				Fullname[Pathend] = '\0';
				strcat(Fullname, ml[i]->m_name);
				mlp = Fullname;
			}
			mlp = ml[i]->m_name;
#if SEC_BASE && MLTAPE
			/* See if we are allowed to link the file */
			if (ie_ml_import(namep, 0, 3, 0) == -1)
				return 0;
#endif

/* try linking (only twice) */
			ans = 0;
			do {
				if(link(mlp, namep) < 0) {
					ans += 1;
				}else {
					ans = 0;
					break;
				}
			}while(ans < 2 && missdir(np) == 0);
			if(ans == 1) {
				fprintf(stderr,
				MSGSTR(CCD,"Cannot create directory for <%s> (errno:%d)\n"), np, errno);
				return(0);
			}else if(ans == 2) {
				fprintf(stderr, MSGSTR(CLSTS,
					"Cannot link <%s> & <%s>: "),
					ml[i]->m_name, np);
				perror("");
				return(0);
			}

			set_time(namep, mklong(Hdr.h_mtime), mklong(Hdr.h_mtime));
			return (0);
		}
	}
	if(mlinks == LINKS
	|| !(ml[mlinks] = (struct ml *)malloc(strlen(np) + 2 + sizeof(struct ml)))) {
		static int first=1;

		if(first)
			if(mlinks == LINKS)
				fprintf(stderr,
				MSGSTR(TML,"Too many links\n"));
			else
				fprintf(stderr,
				MSGSTR(NMFL,"No memory for links\n"));
		mlinks = LINKS;
		first = 0;
		return (1);
	}
	ml[mlinks]->m_dev = Hdr.h_dev;
	ml[mlinks]->m_ino = Hdr.h_ino;
	strcpy(ml[mlinks]->m_name, np);
	++mlinks;
	return (1);
}

pentry(namep)		/* print verbose table of contents */
register char *namep;
{

	static short lastid = -1;
#include <pwd.h>
	static struct passwd *pw;
	static char tbuf[32];


	printf("%-7o", Hdr.h_mode & 0177777);
	if(lastid == Hdr.h_uid)
		printf("%-6s", pw->pw_name);
	else {
		setpwent();
		if(pw = getpwuid((uid_t)Hdr.h_uid)) {
			printf("%-6s", pw->pw_name);
			lastid = Hdr.h_uid;
		} else {
			printf("%-6d", Hdr.h_uid);
			lastid = -1;
		}
	}
	printf("%7ld ", mklong(Hdr.h_filesize));
	U.l = mklong(Hdr.h_mtime);
	strftime(timbuf,(size_t)26,"%sD %T %Y",localtime((long *)&U.l));  
	printf("%s %s",timbuf, namep);  
	if (A_symlink)
		printf(" -> %s", linkname);
	putchar('\n');
#if SEC_BASE && MLTAPE
	ie_unpack();
	ie_pentry(Hdr.h_Mld);	/* print security attributes */
#endif
}

		/* pattern matching functions */
nmatch(s, pat)
char *s, **pat;
{
	if (!pat)
		return (1);
	while(*pat) {
		if((**pat == '!' && !gmatch(s, *pat+1))
		|| gmatch(s, *pat))
			return (1);
		++pat;
	}
	return (0);
}
gmatch(s, p)
register char *s, *p;
{
	register int c;
	register cc, ok, lc, scc;
	register cc1, scc1, lc1;

	scc = *s;
	if (NCisshift (scc)) 
	{
		scc1 = *++s;
		if (_NCdec2 (scc, scc1, scc) == 1) s--;
	}
	lc = 0xffff;
	switch (c = *p) {

	case '[':
		ok = 0;
		while (cc = *++p) {
			if (NCisshift (cc))
			{
				cc1 = *++p;
				if (_NCdec2 (cc, cc1, cc) == 1) p--;
			}
			switch (cc) {

			case ']':
				if (ok)
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
				scc1 = colval(scc);
				lc1 = colval(lc);
				cc1 = colval(cc);
				/* if all of the colvals are nonzero */
				if( scc1 && lc1 && cc1 )
					ok |= ((lc1 <= scc1) && (scc1 <= cc1));
				else
					ok |= ((lc == scc) || (cc == scc));
				break;
			case '[':
				if (p[1] == ':')
				{
					if (!strncmp(p,"[:alpha:]",9)) {
					   ok |= (isascii(scc) && isalpha(scc));
					   p += 8;
					   break;
					   }
					if (!strncmp(p,"[:upper:]",9)) {
					   ok |= (isascii(scc) && isupper(scc));
					   p += 8;
					   break;
					   }
					if (!strncmp(p,"[:lower:]",9)) {
					   ok |= (isascii(scc) && islower(scc));
					   p += 8;
					   break;
					   }
					if (!strncmp(p,"[:digit:]",9)) {
					   ok |= (isascii(scc) && isdigit(scc));
					   p += 8;
					   break;
					   }
					if (!strncmp(p,"[:alnum:]",9)) {
					   ok |= (isascii(scc) && isalnum(scc));
					   p += 8;
					   break;
					   }
					if (!strncmp(p,"[:print:]",9)) {
					   ok |= (isascii(scc) && isprint(scc));
					   p += 8;
					   break;
					   }
					if (!strncmp(p,"[:punct:]",9)) {
					   ok |= (isascii(scc) && ispunct(scc));
					   p += 8;
					   break;
					   }
#    ifdef KJI
					if (!strncmp(p,"[:jalpha:]",10)) {
					   ok |= isjalpha(scc);
					   p += 9;
					   break;
					   }
					if (!strncmp(p,"[:jdigit:]",10)) {
					   ok |= isjdigit(scc);
					   p += 9;
					   break;
					   }
					if (!strncmp(p,"[:jpunct:]",10)) {
					   ok |= isjpunct(scc);
					   p += 9;
					   break;
					   }
					if (!strncmp(p,"[:jparen:]",10)) {
					   ok |= isjparen(scc);
					   p += 9;
					   break;
					   }
					if (!strncmp(p,"[:jkanji:]",10)) {
					   ok |= isjkanji(scc);
					   p += 9;
					   break;
					   }
					if (!strncmp(p,"[:jhira:]",9)) {
					   ok |= isjhira(scc);
					   p += 8;
					   break;
					   }
					if (!strncmp(p,"[:jkata:]",9)) {
					   ok |= isjkata(scc);
					   p += 8;
					   break;
					   }
#    endif
				}
			}
			if (scc==(lc=cc)) ok++;
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

umatch(s, p)
register char *s, *p;
{
	register int scc;
	if(*p==0)
		return(1);
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

colval(c)		/* get uniq colval for c */
wchar_t c;
{
	char buf[3], *bp = buf;
	short int cu, co, tcu;

	buf[0] = buf[1] = buf[2] = '\0';
	_NCe2(c,buf[0],buf[1]);
	cu = NCcoluniq(c);
	if ( ((co = NCcollate(c)) < 0 ) &&
	      (co = _NLxcolu(co, &bp, 0, &cu)) ) ;
	return (int)cu;
}

makdir(namep)		/* make needed directories */
register char *namep;
{

#if SEC_BASE && MLTAPE
	return ie_mkdir(namep);
#else
	if ( 0 > mkdir ( namep , (mode_t)0777 ))
		if ( errno != EEXIST ) {	/*  ignore error when	*/
			perror ( "cpio" ) ;	/*  directory exists	*/
			return (1) ;
		}
	return (0) ;
#endif
}

swap(buf, ct)		/* swap halfwords, bytes or both */
register ct;
register char *buf;
{
	register char c;
	register union swp { long	longw; short	shortv[2]; char charv[4]; } *pbuf;
	int savect, n, i;
	char *savebuf;
	short cc;

	savect = ct;	savebuf = buf;
	if(byteswap || bothswap) {
		if (ct % 2) buf[ct] = 0;
		ct = (ct + 1) / 2;
		while (ct--) {
			c = *buf;
			*buf = *(buf + 1);
			*(buf + 1) = c;
			buf += 2;
		}
		if (bothswap) {
			ct = savect;
			pbuf = (union swp *)savebuf;
			if (n = ct % sizeof(union swp)) {
				if(n % 2)
					for(i = ct + 1; i <= ct + (sizeof(union swp) - n); i++) 
						pbuf->charv[i] = 0;
				else
					for (i = ct; i < ct + (sizeof(union swp) - n); i++) 
						pbuf->charv[i] = 0;
			}
			ct = (ct + (sizeof(union swp) -1)) / sizeof(union swp);
			while(ct--) {
				cc = pbuf->shortv[0];
				pbuf->shortv[0] = pbuf->shortv[1];
				pbuf->shortv[1] = cc;
				++pbuf;
			}
		}
	}
	else if (halfswap) {
		pbuf = (union swp *)buf;
		if (n = ct % sizeof(union swp))
			for (i = ct; i < ct + (sizeof(union swp) - n); i++) pbuf->charv[i] = 0;
		ct = (ct + (sizeof(union swp) -1)) / sizeof(union swp);
		while (ct--) {
			cc = pbuf->shortv[0];
			pbuf->shortv[0] = pbuf->shortv[1];
			pbuf->shortv[1] = cc;
			++pbuf;
		}
	}
}
set_time(namep, atime, mtime)	/* set access and modification times */
register *namep;
long atime, mtime;
{
	struct utimbuf timevec;

	if(!Mod_time)
		return;
	timevec.actime = atime;
	timevec.modtime = mtime;
	utime((char *)namep, &timevec);
}
chgreel(x, fl, rv)
{
	register f;
	char str[BUFSIZ];
	struct stat statb;
	static int	reelcount = 1;

	fstat(fl, &statb);
	if((statb.st_mode&S_IFMT) != S_IFCHR) {
		if(x)
			fprintf(stderr, MSGSTR(WOUTPUT,"Can't write output: "));
		else
			fprintf(stderr, MSGSTR(WINPUT, "Can't read input: "));
		perror("");
		exit(2);
	}

	if (rv == 0  ||  errno == ENOSPC || errno == ENXIO)
		fprintf(stderr, MSGSTR(EOM,
			"\007Reached end of medium on %s\n"),
			x? MSGSTR(DOUTPUT, "output") : MSGSTR(DINPUT, "input"));
	else {
		fprintf(stderr, MSGSTR(IOERR, "\007Encountered an error on "));
		perror(x ? MSGSTR(DOUTPUT, "output") : MSGSTR(DINPUT, "input"));
		if (errno == EFAULT)
			fprintf(stderr, MSGSTR(CBS, "Check block size\n"));
		exit(2);
	}

	if (!Rtty && !(Rtty = fopen(TTYNAME, "r"))) {
		fprintf(stderr, MSGSTR(NOPROMPT,
			"Cannot prompt (can't open %s): "), TTYNAME);
		perror("");
		exit(2);
	}

	close(fl);
	reelcount++;
again:
	if (swfile) {
	    askagain:
		fprintf(stderr, eommsg, reelcount);
		fgets(str, sizeof(str), Rtty);
		switch (*str) {
		case '\n':
			strcpy(str, swfile);
			break;
		case 'q':
			exit(2);
		default:
			goto askagain;
		}
	}
	else {
		fprintf(stderr, MSGSTR(GOON,
		  "If you want to go on, type device/file name when ready.\n"));
		fgets(str, sizeof(str), Rtty);
		str[strlen(str) - 1] = '\0';
		if(!*str)
			exit(2);
	}
	if((f = open(str, x ? 1: 0)) < 0) {
		fprintf(stderr, MSGSTR(COPN, "Cannot Open %s\n"), str);
		goto again;
	}
	return (f);
}
missdir(namep)
register char *namep;
{
	register char *np;
	register ct = 2;

	for(np = namep; *np; ++np)
		if(*np == '/') {
			if(np == namep) continue;	/* skip over 'root slash' */
			*np = '\0';
			if(stat(namep, &Xstatb) == -1) {
				if(Dir) {
					if((ct = makdir(namep)) != 0) {
						*np = '/';
						return(ct);
					}
				}else {
					fprintf(stderr,
					MSGSTR(MISD,"missing 'd' option\n"));
					*np = '/';
					return (-1);
				}
			}
			*np = '/';
		}
	if (ct == 2) ct = 0;		/* the file already exists */
	return (ct);
}

pwd()		/* get working directory */
{
	char	*buf ;
	buf = getcwd ( Fullname , sizeof (Fullname)) ;
	Pathend = strlen(Fullname);
	Fullname[Pathend - 1] = '/';
}


/*
 *  Encode the maj/min device numbers to a "machine-independent" form.
 */
short
encode(dev, spare)
dev_t dev;
char *spare;
{
	int minhi;
	int min;
	int maj;
	short temp;

	min = minor(dev);
	maj = major(dev);
	minhi = ((min & 0xff00) >> 8);
	temp= (maj << 8) | (min & 0xff);
	sprintf (spare, "%.3ho", minhi);
	return (temp);
}

/*
 *  Decode the maj/min device numbers from a "machine-independent" form.
 */
dev_t
decode(dev, spare)
short dev;
char *spare;
{
	short minhi = 0;
	int min;
	int maj;

	if (Cflag)
		*spare = '\0';

	sscanf (spare, "%3ho", &minhi);
	min = ((minhi << 8) & 0xff00) | (dev & 0xff);
	maj = (dev >> 8) & 0xff;

	return (makedev (maj, min));
}
