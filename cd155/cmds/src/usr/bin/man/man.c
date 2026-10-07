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

#ifndef lint
#ifndef _NOIDENT
static char rcsid[] = "@(#) $RCSfile: man.c,v $ $Revision: 1.6 $ (OSF) $Date: 1994/11/19 01:30:09 $";
#endif
#endif
/*
 * COMPONENT_NAME: (CMDMAN) commands that allow users to read online
 * documentation
 *
 * FUNCTIONS: runpath, manual, pathstat, nroff, troff, any, local_remove,
 *	      blklen, apropos, match, amatch, lmatch_sjis, lmatch, whatis,
 *	      wmatch, trim, tail, usage, usage2
 *
 * ORIGINS: 26, 27 
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
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 * 
 * 	man.c	1.21  com/cmd/man,3.1,9021 3/6/90 18:06:26
 */

/*              include file for message texts          */

#include "man_msg.h"
nl_catd  catd;   /* Cat descriptor for scmc conversion */

#define MSGSTR(num,str)         catgets(catd, MS_man, num, str)

#include <stdio.h>
#include <locale.h>
#include <errno.h>
#include <sgtty.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>


#include <NLctype.h>

/*
 * man
 * link also to apropos and whatis
 * This version uses more for underlining and paging.
 */
#define NROFFX "/usr/bin/nroff"        /*  need to check if nroff installed */
#define TROFFX "/usr/bin/troff"        /*  need to check if troff installed */
#define NROFFCAT "nroff -h -man -Tlp"  /* for nroffing to cat file */
#define NROFF   "nroff -man -Tlp"      /* for nroffing to tty */
#define MORE    "more -s"              /* paging filter */
#define CAT_    "/usr/bin/cat"         /* for when output is not a tty */
#define CAT_S   "/usr/bin/cat -s"      /* for '-' opt (no more) */
#define PCAT    "/usr/bin/pcat"        /* for when output is compressed */

#define TROFFCMD "troff -man %s"

#define ALLSECT "CLFnlpo12345678" /* Order to search for sections */
#define CSECT   "Cnlpo1"   /* Sections to search if C is specified */

#define WHATIS  "whatis"

int     nomore;
char    *CAT    = CAT_;
char    *manpath = "/usr/share/man:/usr/local/man";
char    *pager = MORE;

char    *getenv();
char    *calloc();
char    *trim();
char    *tail();
int     local_remove(void);
int     apropos();
int     whatis();

int     section;
int     troffit;
int     mypid;
int     zflag;
int     ftnflag=0;


char *progname;

main(argc, argv)
        int argc;
        char *argv[];
{


        char *mp;
        progname = argv[0];


        (void) setlocale(LC_ALL, "");

	catd = catopen(MF_MAN, 0);

        if ((mp = getenv("MANPATH")) != NULL)
                manpath = mp;
        if ((mp = getenv("PAGER")) != NULL)
                pager = mp;
        umask((mode_t)0);
/*      mypid = getpid(); */
/*  test if command is apropos or whatis and call
        (only in this case) runpath()
*/
        if (strcmp(tail(argv[0]), "apropos") == 0) {
                runpath(argc-1, argv+1, apropos);
                exit(0);
        }
        if (strcmp(tail(argv[0]), "whatis") == 0) {
                runpath(argc-1, argv+1, whatis);
                exit(0);
        }
        if (argc <= 1)
        {
                usage(progname);
                exit(1);
        }
        argc--, argv++;
        while (argc > 0 && argv[0][0] == '-')
        {

               if (NCisshift(argv[0][1]))
                {
                    usage(progname);
                    exit(1);
                }
                switch(argv[0][1]) {

                case 0:
                        nomore++;
                        CAT = CAT_S;
                        break;

                case 't':
                        troffit++;
                        break;

                case 'k':
                        apropos(argc-1, argv+1);
                        exit(0);

                case 'f':
                        whatis(argc-1, argv+1);
                        exit(0);

                case 'P':               /* backwards compatibility */
                case 'M':
                        if (argc < 2) {
                                fprintf(stderr,  MSGSTR(M_MSG_0, "%s: missing path\n") , *argv);
                                usage(progname);
                                exit(1);
                        }
                        if(argv[0][2] == 0) {
                                argc--, argv++;
                                manpath = *argv;
                        }
                        else manpath = *argv + 2;
                        break;
			
                default:
                        usage(progname);
                        exit(1);
                }
                argc--, argv++;
        }
        if (troffit == 0 && nomore == 0 && !isatty(1))
                nomore++;
        section = 0;
        while (argc > 0) { /* find section on command line, if there is one */
                if (

                !NCisshift(argv[0][0]) &&       /* a section is ASCII only */

                (strchr((void*)ALLSECT, (int)argv[0][0]) &&
                     (argv[0][1] == 0)) ||

                    /* This clause supports sub section names */
                    (NCisdigit((int)argv[0][0]) &&
                     (argv[0][1] == 0 || argv[0][2] == 0))) {
                        section = argv[0][0];
                        if (argv[0][1] == 'f')
                          ftnflag=1;
                        argc--, argv++;
                        if (argc == 0) {
                                fprintf(stderr,  MSGSTR(M_MSG_1, "Missing section\n") );
                                usage(progname);
                                exit(1);
                        }
                        continue;
                }
                manual(section, argv[0]);
                ftnflag=0;
                argc--, argv++;
        }
        exit(0);
}

runpath(ac, av, f)
        int ac;
        char *av[];
        int (*f)();
{

        if (ac > 0 && (strcmp(av[0], "-M") == 0 || strcmp(av[0], "-P") == 0)) {
                if (ac < 2) {
                        fprintf(stderr,  MSGSTR(M_MSG_2, "%s: missing path\n") , av[0]);
                        usage(progname);
                        exit(1);
                }
                manpath = av[1];
                ac -= 2, av += 2;
        }
	if (ac == 0) {
	  usage(progname);
	  exit(1);
	}

        (*f)(ac, av);
        exit(0);
}

manual(sec, name)
        char sec, *name;
{
        char section = sec;

        char work[200], work2[200];
        char cmdbuf[250];
        int  i;

        char path[BUFSIZ+1], realname[BUFSIZ+1];
        struct stat stbuf, stbuf2, stbuf3;
        struct sigaction sigact, osigact;
        int last;
        char *sp = ALLSECT;
        FILE *it;
        char abuf[BUFSIZ];
        char sflag = 0;
        char sbuf[2];

        strcpy(work, "manX/");
        strcat(work, name);
        last = strlen(work);
        if (section == 'C' || section == '1')  /* Search the sections in */
                sp = CSECT;                    /* CSECT for command */
        else if (section != 0) {  /* Section name was provided */
                sbuf[0] = section;
                sbuf[1] = '\0';
                sp = sbuf;
        }
        else sflag = 1;           /* No section given */
        for (section = *sp++; section; section = *sp++)
        { 
                zflag=0;
                work[3] = section;
                work[last] = 0;

                if (pathstat(work, path, &stbuf))
                        break;

		if (!ftnflag) {
                	work[last] = '.';
                	work[last+1] = section;
                	work[last+2] = '\0';
                	if (pathstat(work, path, &stbuf))
                       	 	break;
                        work[last] = '.';
                        work[last+1] = section;
                        work[last+2] = 'f';
                        work[last+3] = '\0';
                        if (pathstat(work, path, &stbuf))
                                 break;
		} else {

                	work[last] = '.';
                	work[last+1] = section;
                	work[last+2] = 'f';
                	work[last+3] = '\0';
                	if (pathstat(work, path, &stbuf))
                       		 break;
  		}

                /*  The filename may have the <.section> extension and
                    be in compressed format <.z> */
                work[last] = '.';
                work[last+1] = section;
                work[last+2] = '.';
                work[last+3] = 'z';
                work[last+4] = '\0';
                zflag++; 
                if (pathstat(work, path, &stbuf))
                       break;

        }

        if (!section)   /* Search failed */
        {
                if(sflag) {
                        fprintf(stderr, MSGSTR(M_MSG_3,
                          "No manual entry found for %s.\n") , name);
			  exit(1);
		 }
                else
                        fprintf(stderr, MSGSTR(M_MSG_4, 
                          "No entry found for %s in section %c of the manual\n") ,
                           name, sec);
                return;
        }

        sprintf(realname, "%s/%s", path, work);         /* Search succeeded */
        if (troffit) {
                troff(path, work);
                return;
        }
        if (!nomore) {
                if ((it = fopen(realname, "r")) == NULL) {
                        goto catit;
                }
                if (fgets(abuf, BUFSIZ-1, it) &&
                   strncmp(abuf, ".so ", 4) == 0) {
                        register char *cp = abuf+4;
                        char *dp;

                        while (*cp && *cp != '\n')
                                cp++;
                        *cp = 0;
                        while (cp > abuf && *--cp != '/')
                                ;
                        dp = ".so man";
                        if (cp != abuf+strlen(dp)+1) {
tohard:
                                fclose(it);
				nomore = 1;
                                strcpy(work, abuf+4);
                                goto hardway;
                        }

                        for (cp = abuf; *cp == *dp && *cp; cp+=NLchrlen(cp), dp+=NLchrlen(dp))
                                ;
                        if (*dp)
                                goto tohard;
                        for (i=0, dp = cp; i < 3; i++)
                            if (NCisshift(dp[-2]))
                                dp -= 2;
                            else
                                dp--;
                        strcpy(work, dp);
                }
                fclose(it);
        }
catit:
        strcpy(work2, "cat");
        work2[3] = work[3];
        work2[4] = 0;
        sprintf(realname, "%s/%s", path, work2);

        if (stat(realname, &stbuf2) < 0)
                goto hardway;
        strcpy(work2+4, work+4);
        sprintf(realname, "%s/%s", path, work2);

        if (stat(realname, &stbuf2) < 0 || stbuf2.st_mtime < stbuf.st_mtime) {
                if (nomore)
                        goto hardway;

		if ( (stat(NROFFX, &stbuf3) == -1) && (stat(TROFFX, &stbuf3) == -1) )
		{
			fprintf(stderr, MSGSTR(M_MSG_18, "Nroff/troff is not currently installed, this must be \n \tinstalled in order to use formatted man pages.\n"));
			exit(1);
			}
                fprintf(stdout, MSGSTR(M_MSG_5, "Reformatting page.  Wait...") );
                fflush(stdout);
                unlink(work2);
                (void) sigaction(SIGINT, (struct sigaction *)NULL,&osigact);
                if (osigact.sa_handler == SIG_DFL) {
                        sigact.sa_handler = (void (*)(int))local_remove;
                        sigact.sa_flags   = (int)NULL;
                        (void) sigaction(SIGINT, &sigact,(struct sigaction *)NULL);
                        (void) sigaction(SIGQUIT, &sigact,(struct sigaction *)NULL);
                        (void) sigaction(SIGTERM, &sigact,(struct sigaction *)NULL);
                }
                sprintf(cmdbuf, "%s %s/%s > /tmp/man%d; trap '' 1 15",
                        NROFFCAT, path, work, mypid);

                if (system(cmdbuf)) {
                        fprintf(stdout, MSGSTR(M_MSG_6, " aborted (sorry)\n") );
                        local_remove();
                        /*NOTREACHED*/
                }
                sprintf(cmdbuf, "/bin/mv -f /tmp/man%d %s/%s 2>/dev/null",
                        mypid, path, work2);

                if (system(cmdbuf)) {
                        sprintf(path,  "/");
                        sprintf(work2, "tmp/man%d", mypid);
                }
                fprintf(stdout, MSGSTR(M_MSG_7, " finished\n") );
        }
        strcpy(work, work2);
hardway:
        nroff(path, work);

        if (!NCisshift(work2[0]) && work2[0] == 't')
                local_remove();
}

/*
 * Use the manpath to look for
 * the file name.  The result of
 * stat is returned in stbuf, the
 * successful path in path.
 */
pathstat(name, path, stbuf)
        char *name, path[];
        struct stat *stbuf;
{
        char *cp, *tp, *ep;
        char **cpp;
        static char *manpaths[] = {"man", "cat", 0};
        static char *nopaths[]  = {"", 0};

        if (strncmp(name, "man", (size_t)3) == 0)
                cpp = manpaths;
        else
                cpp = nopaths;
        for ( ; *cpp ; cpp++) {
                for (cp = manpath; cp && *cp; cp = tp) {
                        tp = (void*)strchr((void*)cp, ':');
                        if (tp) {
                                if (tp == cp) {
                                        sprintf(path, "%s%s", *cpp,
                                                name+strlen(*cpp));
                                }
                                else {
                                        sprintf(path, "%.*s/%s%s", tp-cp, cp,
                                                *cpp, name+strlen(*cpp));
                                }
                                ep = path + (tp-cp);
                                tp++;
                        } else {
                                sprintf(path, "%s/%s%s", cp, *cpp,
                                        name+strlen(*cpp));
                                ep = path + strlen(cp);
                        }
                        if (stat(path, stbuf) >= 0) {
                                *ep = '\0';

                                return (1);
                        }
                }
        }
        return (0);
}

nroff(pp, wp)
        char *pp, *wp;
{
        char cmdbuff[BUFSIZ];

        chdir(pp);

        if (
            !NCisshift(wp[0]) &&
                wp[0] == 'c' || wp[0] == 't')
	        {
                     if(zflag)  /* file is compressed with .z*/
            	     {
                           wp[strlen(wp)-2]='\0';
                           nomore++;
                           sprintf(cmdbuff, "%s %s|%s", PCAT, wp, pager);
                           (void) system(cmdbuff);
                           return; 
                     }
                sprintf(cmdbuff, "%s %s", nomore? CAT : pager, wp);
	       }
        else 
                sprintf(cmdbuff, nomore? "%s %s" : "%s %s|%s", NROFF, wp, pager);
        (void) system(cmdbuff);
        return;
}

troff(pp, wp)
        char *pp, *wp;
{
        char cmdbuf[BUFSIZ];

        chdir(pp);
        sprintf(cmdbuf, TROFFCMD, wp);
						
        (void) system(cmdbuf);
}

any(c, sp)
        register int c;
        register char *sp;
{
        register int d;

        if (NCisshift(c))       /* provided trim doesn't change to include KJI char */
                return(0);

        while (d = *sp++)
                if (c == d)
                        return (1);
        return (0);
}

local_remove(void)
{
        char name[15];

        sprintf(name, "/tmp/man%d", mypid);
        unlink(name);
        exit(1);
}

unsigned int
blklen(ip)
        register char **ip;
{
        register unsigned int i = 0;

        while (*ip++)
                i++;
        return (i);
}

apropos(argc, argv)
        int argc;
        char **argv;
{
        char buf[BUFSIZ], file[BUFSIZ+1];
        char savbuf[BUFSIZ];
        char *gotit, *cp, *tp;
        register char **vp;
	FILE *fp;

        if (argc == 0) {
                fprintf(stderr,  MSGSTR(M_MSG_8, "Missing argument\n") );
                usage2("apropos");
                exit(1);
        }
        gotit = calloc(1, blklen(argv));
        /*      First Check the "whatis" file           */
        for (cp = manpath; cp; cp = tp) {
                tp = (void*)strchr((void*)cp, ':');
                if (tp) {
                        if (tp == cp)
                                strcpy(file, WHATIS);
                        else
                                sprintf(file, "%.*s/%s", tp-cp, cp, WHATIS);
                        tp++;
                } else
                        sprintf(file, "%s/%s", cp, WHATIS);
                if ((fp = fopen(file, "r")) != NULL) {
                	while (fgets(buf, (int)(sizeof buf), fp) != NULL)
                        	for (vp = argv; *vp; vp++)
                               		if (match(buf, *vp) && strcmp(buf,savbuf)) {
                                        	printf("%s", buf);
						strcpy(savbuf, buf);
                                        	gotit[vp - argv] = 1;
                                      	  	for (vp++; *vp; vp++) {
                                               		if (match(buf, *vp))
                                                        	gotit[vp - argv] = 1;
						}
                                        	break;
                                	}
			fclose(fp);
        	}
	}
	for (vp = argv; *vp; vp++)
		if (gotit[vp - argv] == 0) {
			fprintf(stdout, MSGSTR(M_MSG_9, "%s: nothing appropriate\n") , *vp);
			exit(1);
		}
	}

match(bp, str)
	register char *bp;
	char *str;
{

	for (;;) {
		if (*bp == 0)
			return (0);
		if (amatch(bp, str))
			return (1);

		bp += NLchrlen(bp);
	}
}

amatch(cp, dp)
	register char *cp, *dp;
{

	while (*cp && *dp && lmatch_sjis(cp, dp))
		dp += NLchrlen(dp), cp += NLchrlen(cp);
	if (*dp == 0)
		return (1);
	return (0);
}


lmatch_sjis(c,d)
register        char    *c, *d;
{
	if (NCisshift(*c))
	    if (NCisshift(*d))
		if (lmatch(*c,*d) && lmatch(*++c, *++d) )
		    return(1);
		else
		    return(0);
	    else
		return(0);
	else if (NCisshift(*d))
	    return(0);
	else
	    return(lmatch(*c,*d));
}

lmatch(c, d)
	register int c, d;
{

	if (c == d)
		return (1);
	if (!NCisalpha(c) || !NCisalpha(d))
		return (0);
	if (NCislower(c))
		c = _NCtoupper(c);
	if (NCislower(d))
		d = _NCtoupper(d);
	return (c == d);
}

whatis(argc, argv)
	int argc;
	char **argv;
{
	register char *gotit, **vp;
	char buf[BUFSIZ], file[BUFSIZ+1], *cp, *tp;
	FILE *fp;

	if (argc == 0) {
		fprintf(stderr,  MSGSTR(M_MSG_10, "Missing argument\n") );
		usage2("whatis");
		exit(1);
	}

	for (vp = argv; *vp; vp++)
		*vp = trim(*vp);
	gotit = calloc(1, blklen(argv));
	for (cp = manpath; cp; cp = tp)
	{
		tp = (void*)strchr((void*)cp, ':');
		if (tp)
		{
			if (tp == cp)
				strcpy(file, WHATIS);
			else
				sprintf(file, "%.*s/%s", tp-cp, cp, WHATIS);
			tp++;
		} else
			sprintf(file, "%s/%s", cp, WHATIS);
		if ((fp = fopen(file, "r" )) != NULL) {
			while (fgets(buf, (int)sizeof buf, fp) != NULL)
				for (vp = argv; *vp; vp++)
					if (wmatch(buf, *vp)) {
						printf("%s", buf);
						gotit[vp - argv] = 1;
						for (vp++; *vp; vp++)
                               		                if (wmatch(buf, *vp))
                               		                        gotit[vp - argv] = 1;
                               		        break;
                               		}
			fclose(fp);
		}
 	}
        for (vp = argv; *vp; vp++)
                if (gotit[vp - argv] == 0) {
                        fprintf(stdout, MSGSTR(M_MSG_11, "%s: not found\n") , *vp);
                        fprintf(stdout, MSGSTR(M_MSG_11, "%s: not found\n") , *vp);
			exit(1);
			}
}

wmatch(buf, str)
        char *buf, *str;
{
        register char *bp, *cp;

        bp = buf;
again:
        cp = str;

        while (*bp && *cp && lmatch_sjis(bp, cp))
                bp += NLchrlen(bp), cp += NLchrlen(cp);

        if (*cp == 0 && (*bp == '(' || *bp == ',' || *bp == '\t' || *bp == ' '))
                return (1);

        while (NCisalpha((int)*bp) || NCisdigit((int)*bp))
                bp += NLchrlen(bp);
        if (*bp != ',')
                return (0);
        bp += NLchrlen(bp);
        while (NCisspace((int)*bp))
                bp += NLchrlen(bp);
        goto again;
}

char *
trim(cp)
        register char *cp;
{
        register char *dp;

        for (dp = cp; *dp; dp++)
                if (*dp == '/')
                        cp = dp + 1;
        if (cp[0] != '.') {
                if (cp + 3 <= dp && dp[-2] == '.' &&
                    any(dp[-1], "cosa12345678npP"))
                        dp[-2] = 0;
                if (cp + 4 <= dp && dp[-3] == '.' &&
                    any(dp[-2], "13") && NCisalpha((int)dp[-1]))
                        dp[-3] = 0;
        }
        return (cp);
}

char *
tail(s)
        char *s;
{
        char *suffix;

                /* find the last '/' if there is one */ ;
        suffix = (void*)strrchr((void*)s,'/');
        if(suffix!=NULL)
                return(suffix+1);
        else
                return(s);

}

usage(progname)
char *progname;
{
        fprintf(stderr,  MSGSTR(M_MSG_12, "Usage: %s [-] [-Mpath] ") , progname);
        fprintf(stderr,  MSGSTR(M_MSG_13, "[section] name ...\n") , progname);
        fprintf(stderr,  MSGSTR(M_MSG_14, "  %s -k keyword ... \n") , progname);
        fprintf(stderr,  MSGSTR(M_MSG_15, "  %s -f file ... \n") , progname);
}
usage2(progname)
char *progname;
{
        fprintf(stderr,  MSGSTR(M_MSG_16, "Usage: %s ") , progname);
        fprintf(stderr,  MSGSTR(M_MSG_17, "[-M path] keyword ...\n") , progname);
}
