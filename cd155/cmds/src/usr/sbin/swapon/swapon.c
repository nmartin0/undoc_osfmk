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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * OSF/1 Release 1.0
 */
/*
 * HISTORY
 * $Log: swapon.c,v $
 * Revision 1.11  1994/11/19  03:16:58  mtm
 * Copyright additions/changes
 *
 * Revision 1.10  1993/10/25  18:52:20  jlitvin
 * Fix silly spelling error.
 *
 * Revision 1.9  1993/10/13  03:10:22  stans
 *    Recognize a block paging device instead of a paging file.
 *
 * Revision 1.8  1993/06/17  21:09:02  stans
 *    Actually remove the '-E' switch from getopts() arg list. Some more
 *    comments about fstab option processing.
 *
 * Revision 1.7  1993/05/11  19:06:38  stans
 *    Support 'fstab' options 'clsize' (cluster size) and 'pri' (prioprity).
 *    Current servers do not deal with these options yet so we just swollow
 *    them for the time being.
 *
 * Revision 1.6  1993/05/10  17:38:01  stans
 * '-E' switch has been removed in favor of '-Pstart..end' syntax. Ending
 * pass is optional.
 *
 * Revision 1.5  1993/04/18  01:51:00  stans
 *   Well it seems just adding the Log macro doesn't include all the history.
 *   Add by hand from 'cvs rlog' output.
 *
 * Revision 1.4  1993/04/18  01:46:16  stans
 *   Added HISTORY/log macros for CVS.
 *
 * Revision 1.3
 *   Cleanup error messages, merge verbose output into one routine usable
 *   by fstab and command line processing.
 *   Support fstab "-P startingPass" "-E endingPass" switches.
 *   Correctly process (swapon -a) /etc/fstab 'sw' entries, including options
 *
 * Revision 1.2
 *   output lo/hi water marks in terms of MegaBytes if appro; saves looking at
 *   large byte counts.
 *
 * Revision 1.1
 *   "OSF/1.0.4"
 */

#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: swapon.c,v $ $Revision: 1.11 $ (OSF) $Date: 1994/11/19 03:16:58 $";
#endif

#include <stdio.h>
#include <strings.h>
#include <fstab.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/secdefines.h>

#if SEC_BASE
#include <sys/security.h>
#include <prot.h>

extern priv_t *privvec();
#endif

#if defined(NLS) || defined(KJI)
#define	NLSKJI 1
#include <NLctype.h>
#include <NLchar.h>
#endif
#include <locale.h>

#ifdef MSG
#include "swapon_msg.h"
nl_catd catd;
#define MSGSTR(Num,Str) NLcatgets(catd,MS_SWAPON,Num,Str)
#else
#define MSGSTR(Num,Str) Str
#endif

#define	MS_NONE		0
#define	MS_PREFER	1
#define	MS_BLOCK	2

#define	AMEG (1024*1024)
#define	AGIG (1024*1024*1024)

char *program;

static  int	add();
static	void	usage();
	int	numarg();

void
main(argc, argv)
	int argc;
	char *argv[];
{
	extern char		*optarg;
	extern int		optind;
	register struct fstab	*fsp;
	int			verbose = 0, doall = 0;
	unsigned int		flags;
	int			lowat, hiwat, stats, ch, priority, clusterSize;
	char			*cp;
	int			startingPass=1, /* fstab starting pass number */
				endingPass=0;	/* fstab ending pass number,
						 * 0 == do all
						 */
#if SEC_BASE
	set_auth_parameters(argc, argv);
	initprivs();

	if (!authorized_user("mount")) {
		fprintf(stderr, "%s: need mount authorization\n", program);
		exit(1);
	}
	if (forceprivs(privvec(SEC_MOUNT, -1), (priv_t *) 0)) {
		fprintf(stderr, "%s: insufficient privileges\n", program);
		exit(1);
	}
#endif
	if (program = rindex(argv[0], '/'))
		program++;
	else
		program = argv[0];

#if SEC_BASE
	set_auth_parameters(argc, argv);
	initprivs();

	if (!authorized_user("mount")) {
		fprintf(stderr, MSGSTR(NO_AUTH, "%s: need mount authorization\n"), program);
		exit(1);
	}
	if (forceprivs(privvec(SEC_MOUNT, -1), (priv_t *) 0)) {
		fprintf(stderr, MSGSTR(PRIV, "%s: insufficient privileges\n"), program);
		exit(1);
	}
#endif
	flags = MS_NONE;
	lowat = 20 * 1024 * 1024;
	hiwat = 0;	/* unbounded growth */

	while ((ch = getopt(argc, argv, "avpl:h:P:")) != EOF)
		switch((char)ch) {
		case 'a':
			doall = 1;
			break;
                case 'v':
                        verbose = 1;
                        break;
                case 'p':
			flags |= MS_PREFER;
                        break;
                case 'l':
			lowat = numarg(optarg);
                        break;
                case 'h':
			hiwat = numarg(optarg);
                        break;

		case 'P': /* -P startingPass-number{..endingPass} */
                        /* if we have '..' syntax, isolate the staring pass */
                        if ( cp=strchr(optarg,'.') ) {
				/* must have '..' */
				if ( *(cp+1) != '.' ) {
					fprintf(stderr,
						"%s: Invalid -P %s.\n",optarg);
					exit(EINVAL);
				}
                                *cp = '\0';
                        }
			startingPass = atoi(optarg);
 			if (startingPass <= 0) {  
				fprintf(stderr,"%s: Invalid -P %d.\n",
					program,startingPass);
				exit(EINVAL);
			}
			/* ending pass specified? -Pstart..end */
			if ( cp ) {
				cp += 2;        /* skip over '..' */
				endingPass = atoi(cp);
			}
			if ( (endingPass != 0) && (endingPass < startingPass)) {
				fprintf(stderr,"%s: Invalid -P %d..%d, end < starting Pass.\n",
					program,startingPass,endingPass); 
				exit(EINVAL);
			}
			break;

		case '?':
		default:
			usage();
		}
	argv += optind;
        
	stats = 0;
	if (doall) {
		while (fsp = getfsent()) {
			char	*dev;

			if (strcmp(fsp->fs_type, FSTAB_SW))
				continue;
			/*
			 * do we care about this fstab pass?
			 */
			if ( fsp->fs_passno < startingPass )
				continue;
			if (endingPass && (fsp->fs_passno > endingPass))
				continue;

			do_options(fsp->fs_mntops, &flags, &lowat, &hiwat,
					&priority, &clusterSize);

			if ( flags & MS_BLOCK )
				dev = fsp->fs_spec;
			else
				dev = fsp->fs_file;

			if (add(dev, flags, lowat, hiwat, 1)) {
				stats = 1;
			}
			else {
				swapon_what(dev,flags,lowat,hiwat);
			}
		}
	}
	else if (!*argv)
		usage();

	for (; *argv; ++argv) {
		struct stat		sb;

		/*
		 * block special device? Indicate so in the 'flags'.
		 */
		flags &= ~MS_BLOCK;	/* assume it's not */
		if ( stat( *argv, &sb ) == 0 ) {
			if ( S_ISBLK(sb.st_mode) )
				flags |= MS_BLOCK;
		}

                if (verbose)
			swapon_what(*argv,flags,lowat,hiwat);

		stats |= add(*argv, flags, lowat, hiwat, 0);
        }
	exit(stats);
}

static int
numarg(arg)
    char *arg;
{
	int i, len = strlen(arg);
	int size = 0;
	int	page_size;

	page_size = getpagesize();
	for (i = 0; i < len; i++) {
		char c = arg[i];

		if (('0' <= c) && (c <= '9')) {
			size = 10*size + (c - '0');
			continue;
		}

		if ((i == 0) || (i != len-1))
			usage();

		switch (c) {
		case 'p': case 'P':
			size *= page_size;
			break;
		case 'k': case 'K':
			size *= 1024;
			break;
		case 'm': case 'M':
			size *= 1024*1024;
			break;
		case 'g': case 'G':
			size *= 1024*1024*1024;
			break;
		default:
			usage();
		}
	}
	return size;
}

/*
 * add a swap file
 *
 * inputs:
 *	name		filename
 *	flags		swapon flags (priv)
 *	lo		lowater mark in bytes
 *	hi		highwater mark in bytes
 *	ignoreebusy	boolean: ignore EBUSY error.
 *
 * outputs:
 *	0		== success
 *	otherwise	== error code.
 */

static int
add(name, flags, lo, hi, ignoreebusy)
	char *name;
	int flags, lo, hi, ignoreebusy;
{
	extern int	errno;
	struct stat	sb;
	int		create=TRUE;

#if SEC_BASE
	disablepriv(SEC_SUSPEND_AUDIT);
#endif
	/*
	 * Does the file exists, if NOT then create it 
	 */
	if ( stat(name,&sb) ) {
		create = TRUE;
	}
	else {
		/* MUST be a regular file */
		if ( sb.st_mode & S_IFREG )
			create = TRUE;
		else
			create = FALSE;
	}

	if ( create ) {
		int	fd;

		/* create a zero length file */
		if ( (fd=open(name, O_WRONLY+O_CREAT+O_TRUNC, 0600)) < 0 ) {
			return(errno);
		}
		close(fd);
	}

	if (swapon(name, flags, lo, hi) == -1) {
		switch (errno) {
		case EINVAL:
			fprintf(stderr,
                                MSGSTR(NO_DEV, "%s: %s: device not configured\n"),
                                program, name);
			break;
		case EBUSY:
			if (!ignoreebusy)
				fprintf(stderr,
				    MSGSTR(IN_USE, "%s: %s: device already in use\n"),
				     program, name);
			break;
		default:
			fprintf(stderr, "%s: %s: ", program, name);
			perror((char *)NULL);
			break;
		}
		return(1);
	}
	return(0);
}

static void
usage()
{
	fprintf(stderr, MSGSTR(USAGE, "usage: %s [-avp] [-l size] [-h size] filename\n"), program);
	exit(1);
}

/*
 * process options string from a /etc/fstab sw entry
 *
 * inputs:
 *	opts	options string as return from getfsent() 'fs_mntops' field.
 *	flags	pointer to flags int.
 *	lo	pointer to lowater int.
 *	hi	pointer to hiwater int.
 *	pri	pointer to priority int.
 *	clsz	pointer to paging cluster size int.
 * 
 * outputs:
 *	none.
 *
 * side effects:
 *	input parameter pointers are utilized in setting what they point at
 *	 according to options keywords.
 *
 */
do_options( opts, flags, lo, hi, pri, clsz )
	char	*opts;
	int	*flags, *lo, *hi, *pri, *clsz;
{
	register char	*cp, *t;
	int		found=FALSE;
	char		tok[64];

	if ( opts == (char *)0 )
		return;

	if (strncmp(opts,"sw,",3) != 0) 
		return;

	opts += 3;

	cp=strtok(opts,",\0");
	while( cp ) {
		if ( strncmp(cp,"lowater=",8) == 0 ) {
			*lo = numarg(cp+8);
		}
		else if ( strncmp(cp,"hiwater=",8) == 0 ) {
			*hi = numarg(cp+8);
		}
		else if ( strncmp(cp,"prefer",6) == 0 ) {
			*flags |= MS_PREFER;
		}
		else if ( strncmp(cp,"block",5) == 0 ) {
			*flags |= MS_BLOCK;
		}
		else if ( strncmp(cp,"pri=",4) == 0 ) {
			*pri = numarg(cp+4);
		}
		else if ( strncmp(cp,"clsize=",7) == 0 ) {
			*clsz = numarg(cp+7);
		}
		else
			fprintf(stderr,"\n%s: ignoring bad option '%s'\n",
				program,cp);

		cp = strtok( (char *)0, ",\0");
	}
}


static
char *fmt="%s: adding %s as %spaging %s\n\tlow water mark %s, high water mark %s\n";
char *fmt_blk="%s: adding %s as %spaging %s\n";

swapon_what(name,flags,lowat,hiwat)
	char	*name;
	int	flags,lowat,hiwat;
{
	char	lo[32],hi[32];

	/*
	 * special case raw block device paging partition.
	 */
	if ( flags & MS_BLOCK ) {
		printf(MSGSTR(VERBOSE2, fmt_blk), program, name,
			(flags & MS_PREFER) ?
				(char *)MSGSTR(PREFERED, "preferred ") : "", 
			"device");
		return;
	}

	/*
	 * output paging file sizes in MB instead of bytes.
	 */
	if ( lowat == 0 )
		strcpy(lo,"(NO shrink on release)");
	else {
		if ( lowat > AMEG )
			sprintf(lo,"%uMB",(lowat/AMEG));
		else
			sprintf(lo,"%u",lowat);
	}

	if ( hiwat == 0 )
		strcpy(hi,"(Infinity)");
	else {
		if ( hiwat > AMEG )
			sprintf(hi,"%uMB",(hiwat/AMEG));
		else
			sprintf(hi,"%u",hiwat);
	}

	printf(MSGSTR(VERBOSE2, fmt), program, name,
		(flags & MS_PREFER) ?
		(char *)MSGSTR(PREFERED, "preferred ") : "", 
		(flags & MS_BLOCK) ? "device" : "file",
		lo, hi);
}
