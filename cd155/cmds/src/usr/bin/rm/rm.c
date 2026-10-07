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
static char rcsid[] = "@(#)$RCSfile: rm.c,v $ $Revision: 1.11 $ (OSF) $Date: 1994/11/19 01:36:57 $";
#endif
/*
 * COMPONENT_NAME: (CMDFS) commands that deal with the file system
 *
 * FUNCTIONS: rm, rmdir
 *
 * ORIGINS: 3, 26, 27
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
 * rm.c       1.13  com/cmd/fs/progs,3.1,9021 4/26/90
 *
 *
 * HISTORY
 * $Log: rm.c,v $
 * Revision 1.11  1994/11/19  01:36:57  mtm
 * Copyright additions/changes
 *
 * Revision 1.10  1994/10/19  21:17:10  jlitvin
 * Fix my fix for PTS #6620.  Only reuse a file descriptor after all of
 * the available descriptors have been used.
 *
 *  Reviewer: nandy
 *  Risk: low
 *  Benefit or PTS #: 10440
 *  Testing: PTS test cases
 *  Module(s): cmds_libs/usr/bin/rm/rm.c
 *
 * Revision 1.9  1994/06/01  18:45:42  jlitvin
 * Due to the tail recursion usage of opendir(), we were limited to about
 * 60 levels deep of pathnames since each opendir() opened another
 * element of the path.  Kludge around this by closing and reopening the
 * current directory name every loop.
 *
 *  Reviewer: suri
 *  Risk: low
 *  Benefit or PTS #: 6620
 *  Testing: PTS example
 *  Module(s): cmd_libs/src/usr/bin/rm/rm.c
 *
 * Revision 1.8  1994/04/12  23:19:48  rlg
 * Fixed problem with a misleading error message when removing a pfs file when
 * the access permissions are not set correctly in the stripe directories.
 *
 *  Reviewer:  Brad Rullman
 *  Risk:  low
 *  Benefit or PTS #:  PTS #8895
 *  Testing:  failing test case; fileio and pfs EATs
 *  Module(s):  rm.c
 *
 */

/*
**	rm [-fir] file ...  (also recognizes -R==-r, and -e for echo)
**	rmdir file ...
*/

#include	<stdio.h>
#include	<sys/types.h>
#include	<sys/limits.h>	/* for PATH_MAX */
#ifdef PFS
#include	<sys/estat.h>
#include	<sys/errno.h>
#else
#include	<sys/stat.h>
#endif PFS
#include	<dirent.h>
#include	<sys/access.h>
#include	<sys/mode.h>


#include <NLctype.h>
#include <NLchar.h>
#include <locale.h>

#include <nl_types.h>
#include "rm_msg.h"
nl_catd catd;
#define MSGSTR(Num,Str) catgets(catd,MS_RM,Num,Str)

int	errcode;
extern char *rindex();
extern char *NLgetenv(); 

main(argc,argv)
int argc;
char **argv;
{
    char *name;
    int rv = 2;

    (void) setlocale (LC_ALL,"");
    catd = catopen((char *)MF_RM,0);

    /* find our name */
    name = rindex(argv[0],'/');
    if (name) name++; else name = argv[0];

    if (!strcmp(name,"rmdir")) rv = rmdir_main(argc,argv);
    else rv = rm_main(argc, argv);	/* assume rm */
    exit(rv);
}

rm_main(argc, argv)
char *argv[];
{
    register char *arg;
    int	fflg, iflg, rflg, eflg;
    extern int optind;
    extern char *optarg;

    fflg = 0;
    iflg = 0;
    rflg = 0;
    eflg = 0;

	while (argc>1 && argv[1][0]=='-') {
	    arg=*++argv;
	    argc--;
	    if (arg[1] == '\0')	/* single dash */
		break;
	    while (*++arg != '\0') {
		switch(*arg) {
		case '-':
			goto endarg;
		case 'f':
			fflg++;
			break;
		case 'i':
			iflg++;
			break;
		case 'R':
		case 'r':
			rflg++;
			break;
		case 'e':
			eflg++;
			break;
		case '?':
		default:
		    fprintf(stderr, 
		       MSGSTR(USAGE,"usage: rm [-efirR] [-] file ...\n"));
		    return(2);
		}
	}
    }

endarg: 
    
    if (argc <= 1 && !fflg) {
	fprintf(stderr, MSGSTR(USAGE,"usage: rm [-efirR] [-] file ...\n"));
	return(2);
    }

    while (--argc > 0) {
	if(!strcmp(*++argv, "..")) {
		fprintf(stderr, MSGSTR(CANTREMOVE,"rm: cannot remove ..\n"));
		continue;
	}
	rm(*argv, fflg, rflg, iflg, eflg);
    } 

    return(errcode?2:0);
}

rm(arg, fflg, rflg, iflg, eflg)
char arg[];
{
#ifdef PFS
    struct estat	buf;
    int			error;
#else
    struct stat		buf;
#endif PFS
    int			status;
    struct dirent	*direct;
    char		name[PATH_MAX+1];
    DIR			*d;

#ifdef PFS
    if ((error = _lestat(arg, &buf)) &&
	((errno == ENOTPFS) || (errno == ENOSDIR))) {
	if (!fflg) {
	    if (errno == ENOTPFS)
		printf(MSGSTR(NOTPFS,
		    "rm: %s is not a valid PFS file, remove anyway? "), arg);
	    else if (errno == ENOSDIR)
		printf(MSGSTR(NOSDIR,
		    "rm: %s: PFS stripe data not available, remove anyway? "),
			arg);
	    if (!yes())
		return(0);
	}
	goto UNLINK;
    } else if (error) {
#else
    if(lstat(arg, &buf)) {
#endif PFS
	if (fflg==0) {
	    fprintf(stderr, "rm: ");
	    perror(arg);
	    ++errcode;
	}
	return(errcode);
    }
    if ((buf.st_mode&S_IFMT) == S_IFDIR) {
        errcode = 0;
	if(rflg) {
	    if(iflg && !dotname(arg)) {
		printf(MSGSTR(DIRECTORY,"rm: remove files in directory %s? "), arg);
		if(!yes())
		    return(0);
	    }
	    if((d=opendir(arg)) == NULL) {
		fprintf(stderr,MSGSTR(CANTREAD,"rm : cannot read %s\n"), arg);
		return(2);
	    }
	    while( (direct = readdir(d)) != NULL) {
		if(!dotname(direct->d_name)) {
                    if ( (strlen(arg) + strlen(direct->d_name)) < PATH_MAX)
                        sprintf(name, "%s/%s", arg, direct->d_name);
                    else {
                        fprintf(stderr, "rm: File name too long.\n");
                        return(2);
                    }

		    /*
		     * If there are no more file descriptors available,
		     * re-use the last one.
		     */
		    if (d->dd_fd >= OPEN_MAX-1) {
		    	closedir(d);
		    	errcode = rm(name, fflg, rflg, iflg, eflg);
		    	if (errcode) return(errcode);
		    	if((d=opendir(arg)) == NULL) {
		        	fprintf(stderr,MSGSTR(CANTREAD,"rm : cannot read %s\n"), arg);
				return(2);
			}
		    } else {
		    	errcode = rm(name, fflg, rflg, iflg, eflg);
		    	if (errcode) return(errcode);
		    }
		}
	    }
	    closedir(d);
	    if(dotname(arg))
		return(0);
    	    if(iflg) {
		printf(MSGSTR(REMPROMPT, "rm: remove %s? "), arg);
		if(!yes())
	    	return(0);
 	    }
	    if ((rmdir(arg) != 0) && (fflg == 0 || iflg)) {
		fprintf(stderr, MSGSTR(NOTREMOVED,"rm: %s not removed.  "),arg);
		perror("");
		errcode++;
		return(errcode);
	    }
 	    if (eflg)
		printf(MSGSTR(REMOVEDDIR, "rm: removing directory %s\n"), arg);

	} else {
		fprintf(stderr, MSGSTR(ISDIRECT,"rm: %s directory\n"), arg);
		++errcode;
	}
	return(errcode);
    }

    if(iflg) {
	printf(MSGSTR(REMPROMPT, "rm: remove %s? "), arg);
	if(!yes())
	    return(0);
    }
    else if(!fflg) {
	if(access(arg, W_OK) < 0 && isatty(fileno(stdin))) {
    		if ((buf.st_mode&S_IFMT) != S_IFLNK) {
	    		printf(MSGSTR(OVERRIDE,"rm: override protection %o for %s? "),
				buf.st_mode&0777, arg);
	    		if(!yes())
				return(0);
		}
	}
    }

#ifdef	PFS

UNLINK:
    status = unlink(arg);
    if(status) {
	if (errno == ENOSDIR) {
	    fprintf(stderr,
		    MSGSTR(NOSDIRPERM,
			   "rm:  Warning: unable to remove all PFS stripe date for %s.  "
),
		    arg);
	    perror("");
	} else if (fflg==0 || iflg) {
	    fprintf(stderr,
		    MSGSTR(NOTREMOVED,
			   "rm: file %s not removed.  "),arg);
	    perror("");
	    ++errcode;
	}

#else

    if(unlink(arg) && (fflg==0 || iflg)) {
	fprintf(stderr,
		MSGSTR(NOTREMOVED,
		       "rm: %s not removed.  "),arg);
	perror("");
	++errcode;

#endif

    } else {
    	if (eflg)
		printf(MSGSTR(FILEREMOVED, "rm: removing %s\n"), arg);
    }
}

/*
 * Do we have "." or ".."?
 * 	1 - yes
 * 	0 - no
 */
dotname(s)
char *s;
{
	return(s[0] == '.' && (!s[1] || s[1] == '.' && !s[2]));
}

rmdir_main(argc,argv)
char **argv;
int argc;
{
    char rc = 0;
    int Errors = 0;

    if(argc < 2) {
	fprintf(stderr, MSGSTR(RMDIR,"rmdir: usage: rmdir dirname ...\n"));
	return(2);
    }
    while(--argc) {
	char *cp = *++argv;
	if ((rc = rmdir(cp)) != 0) {
	    fprintf(stderr,"rmdir: ");
	    perror(cp);
	    ++Errors;
        }
    }
    return(Errors? 2: 0);
}

yes()
{
	int	i, b;
	char response[2];

	i = b = getchar();
	while(b != '\n' && b > 0)
		b = getchar();
	response[0] = i; response[1]='\0';
	return(NLyesno(response) == 1);
}
