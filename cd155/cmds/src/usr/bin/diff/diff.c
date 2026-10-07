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
static char rcsid[] = "@(#)$RCSfile: diff.c,v $ $Revision: 1.3 $ (OSF) $Date: 1995/03/07 17:37:38 $";
#endif
/*
 * COMPONENT_NAME: (CMDFILES) commands that manipulate files
 *
 * FUNCTIONS: diff
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
 * diff.c        1.7  com/cmd/files/diff,3.1,9021 11/2/89 17:16:43";
 */

#include <stdio.h>
#include <locale.h>
#include <stdlib.h>

#include <NLctype.h>
#define ISDIGIT NCisdigit

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include "diff.h"

nl_catd catd;

/*
 * Output format options
 */
int        opt;
int        tflag=0;                        /* expand tabs on output */

/*
 * Algorithm related options
 */
int        hflag=0;                    /* -h, use halfhearted DIFFH */
int        bflag=0;                    /* ignore blanks in comparisons */
int        wflag=0;                    /* totally ignore blanks in comparisons*/
int        iflag=0;                    /* ignore case in comparisons */


/*
 * Options on hierarchical diffs.
 */
int        lflag=0;                      /* long output format with header */
int        rflag=0;                      /* recursively trace directories */
int        sflag=0;                      /* announce files which are same */
char       *begin = NULL;                /* do file only if name >= this */

/*
 * Variables for -I D_IFDEF option.
 */
int        wantelses;                    /* -E */
char       *def1;                        /* String for -1 */
char       *def2;                        /* String for -2 */

/*
 * Variables for -c context option.
 */
int        context;                /* lines of context to be printed */

/*
 * State for exit status.
 */
int        status;
int 	   oldstatus;
char        *tempfile;        /* used when comparing against std input */

/*
 * Variables for diffdir.
 */
char        **diffargv;        /* option list to pass to recursive diffs */

/*
 * Input file names.
 * With diffdir, file1 and file2 are allocated BUFSIZ space,
 * and padded with a '/', and then efile0 and efile1 point after
 * the '/'.
 */
char        *file1, *file2, *efile1, *efile2;
struct        stat stb1, stb2;
/*
 * This is allocated early, and used
 * to reset the free storage pointer to effect space compaction.
 */
char        *dummy;
char        *talloc(), *ralloc();
char        *savestr();
int        done();

/*
 * NAME:   diff - driver and subroutines
 * FUNCTION:  compare two files
 *   FLAGS:   
 *    Options when comparing directories are:
 *    -l       long output format; each text file diff is piped through pr to 
 *             paginate it, other differences are remembered and summarized 
 *             after all text file differences are reported.
 *   -r        causes application of diff recursively to common subdirectories
 *   -s        casuses diff to report files which are the same, which are 
 *             otherwise not memtioned. 
 *   -Sname    starts a directory diff in the middle beginning with file name.
 *   The following options are mutually exclusive: 
 *   -e        produces a script for the command ed
 *   -f        produces a script similar to -e but in the opposite order
 *   -n        produces a script similar to that of -e, but in the opposite 
 *             order and with a count of changed lines on each insert or 
 *             delete command.
 *   -c        produces a diff with lines of context
 *   -h        does a fast, half-hearted job.
 *   -Dstring  cause diff to create a merged version of file1 and file2,
 *             with C preprocessor controls included.
 *  Other flags
 *  -b         causes trailing blanks (spaces and tabs) to be ignored, and
 *             other strings of blanks to compare equal.
 *  -w         causes whitespace (blanks and tabs) to be totally ignored.
 *  -i         ignores the case of letters
 *  -t         will expand tabs in output lines
 */


#ifdef DIFF
char        diff[] = DIFF;
#else
char        diff[] = "/usr/bin/diff";
#endif
#ifdef DIFFH
char        diffh[] = DIFFH;
#else
char        diffh[] = "/usr/lbin/diffh";
#endif
#ifdef PR
char        pr[] = PR;
#else
char        pr[] = "/usr/bin/pr";
#endif

main(argc, argv)
        int argc;
        char **argv;
{
        register char *argp;

        (void) setlocale(LC_ALL,"");

        catd = catopen(MF_DIFF,0);

        def1 = "FILE1"; def2 = "FILE2";
		oldstatus = status = 0;
        diffargv = argv;
        argc--, argv++;
        while (argc > 2 && argv[0][0] == '-') {
                argp = &argv[0][1];
                argv++, argc--;
                while (*argp) switch(*argp++) {

#ifdef notdef
                case 'I':
                        opt = DI_IFDEF;
                        wantelses = 0;
                        continue;
                case 'E':
                        opt = DI_IFDEF;
                        wantelses = 1;
                        continue;
                case '1':
                        opt = DI_IFDEF;
                        def1 = argp;
                        *--argp = 0;
                        continue;
#endif
                case 'D':
                        /* -Dfoo = -E -1 -2foo */
                        wantelses = 1;
                        def1 = "";
                        /* fall through */
#ifdef notdef
                case '2':
#endif
                        opt = DI_IFDEF;
                        def2 = argp;
                        *--argp = 0;
                        continue;
                case 'e':
                        opt = DI_EDIT;
                        continue;
                case 'f':
                        opt = DI_REVERSE;
                        continue;
                case 'n':
                        opt = DI_NREVERSE;
                        continue;
                case 'b':
                        bflag = 1;
                        continue;
                case 'w':
                        wflag = 1;
                        continue;
                case 'i':
                        iflag = 1;
                        continue;
                case 't':
                        tflag = 1;
                        continue;
                case 'c':
                        opt = DI_CONTEXT;
                        if (ISDIGIT(*argp)) {
                                context = atoi(argp);
                                while (ISDIGIT(*argp))
                                        argp++;
                                if (*argp) {
                                        fprintf(stderr,MSGSTR(EBADCNT,
                                            "diff: -c: bad count\n"));
										status=2;
                                        done();
                                }
                                argp = "";
                        } else
                                context = 3;
                        continue;
                case 'h':
                        hflag++;
                        continue;
                case 'S':
                        if (*argp == 0) {
                                fprintf(stderr,MSGSTR(ESTART,
                                         "diff: use -Sstart\n"));
								status=2;
                                done();
                        }
                        begin = argp;
                        *--argp = 0;                /* don't pass it on */
                        continue;
                case 'r':
                        rflag++;
                        continue;
                case 's':
                        sflag++;
                        continue;
                case 'l':
                        lflag++;
                        continue;
                default:
                        fprintf(stderr,MSGSTR(EUNKNOWN,
                                "diff: -%s: unknown option\n"),--argp);
						status=2;
                        done();
                }
        }
        if (argc != 2) {
                fprintf(stderr,MSGSTR(ETWO, 
                        "diff: two filename arguments required\n"));
				status=2;
                done();
        }
        file1 = argv[0];
        file2 = argv[1];
        if (hflag && opt) {
                fprintf(stderr,MSGSTR(EBAD, 
                        "diff: -h doesn't support -e, -f, -n, -c, or -D\n"));
				status=2;
                done();
        }
        if (!NLstrcmp(file1, "-"))
                stb1.st_mode = S_IFREG;
        else if (stat(file1, &stb1) < 0) {
                fprintf(stderr, "diff: ");
                perror(file1);
				status=2;
                done();
        }
        if (!NLstrcmp(file2, "-"))
                stb2.st_mode = S_IFREG;
        else if (stat(file2, &stb2) < 0) {
                fprintf(stderr, "diff: ");
                perror(file2);
				status=2;
                done();
        }
        if ((stb1.st_mode & S_IFMT) == S_IFDIR &&
            (stb2.st_mode & S_IFMT) == S_IFDIR) {
                diffdir(argv);
        } else
                diffreg();
        done();
}

/* save a copy of string cp */
char *
savestr(cp)
register char *cp;
{
        register char *dp;

	dp = (char *) malloc((size_t)(NLstrlen(cp)+1));
        if (dp == 0) {
                fprintf(stderr,MSGSTR(EMEM,"diff: ran out of memory\n"));
                done();
        }
        NLstrcpy(dp, cp);
        return (dp);
}

/* return the integer with the minimun value */
min(a,b)
        int a,b;
{

        return (a < b ? a : b);
}

/* return the integer with the maxinum value */
max(a,b)
        int a,b;
{

        return (a > b ? a : b);
}

/* delete tempfile and exit */
done()
{
        if(tempfile!=NULL)
        {
           unlink(tempfile);
        }
        exit(status);
}

/*   malloc with test */
char *
talloc(n)
{
        register char *p;
        p = malloc((size_t)n);
        if(p!=NULL)
                return(p);
        noroom();
}

char *
ralloc(p,n)        /*compacting reallocation */
char *p;
{
        register char *q;
        q = realloc((void *)p, (size_t)n);
        if(q==NULL)
                noroom();
        return(q);
}

/* out of memory, print error message and exit */
noroom()
{
        fprintf(stderr,MSGSTR(ESIZE,"diff: files too big, try -h\n"));
        done();
}
