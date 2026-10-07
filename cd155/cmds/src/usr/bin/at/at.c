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
static char rcsid[] = "@(#)$RCSfile: at.c,v $ $Revision: 1.4 $ (OSF) $Date: 1994/11/19 01:18:49 $";
#endif
/*
 * COMPONENT_NAME: (CMDOPER) commands needed for basic system needs
 *
 * FUNCTIONS: at
 *
 * ORIGINS: 27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 * 
 * at.c        1.28  com/cmd/oper/cron,3.1,9021 5/16//90 14:18:37
 * at.c	4.3 00:41:01 7/12/90 SecureWare 
 */

#include <sys/secdefines.h>
#if SEC_BASE
#include <sys/security.h>

extern priv_t *privvec();
#endif

#include <sys/types.h>
#include <sys/limits.h>
#include <sys/stat.h>
#include <sys/param.h>        /* for BSIZE needed in dirent.h */
#include <dirent.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <locale.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <ulimit.h>
#include <unistd.h>
#include "cron.h"
#include "cron_msg.h"
nl_catd catd;
#define	MSGSTR(Num,Str) catgets(catd,MS_CRON,Num,Str)


#define TMPFILE		"_at"	/* prefix for temporary files	*/
#if SEC_BASE
#define ATMODE		06040	/* Mode for creating files in ATDIR. */
#else
#define ATMODE		06444	/* Mode for creating files in ATDIR. */
#endif
#define BUFSIZE		512	/* for copying files */
#define LINESIZE	130	/* for listing jobs */
#define	MAXTRYS		60	/* max trys to create at job file */
#define	MPWDTRYS	15	/* max trys to create at pipe for pwd */

#define BADDATE		"bad date specification"
#define BADMINUTES	"minutes field is too large"
#define BADSHELL	"because your login shell isn't /bin/sh, you can't use at"
#define WARNSHELL	"warning: commands will be executed using /bin/sh\n"
#define CANTCD		"can't change directory to the at directory"
#define CANTCHOWN	"can't change the owner of your job to you"
#if SEC_BASE
#define	CANTCHMOD	"can't change the mode of your job"
#define	INSUFFPRIVS	"at: insufficient privileges"
#endif
#define CANTOPEN  	"can't open job file for you"
#define CANTLINK  	"can't link job file for you"
#define CANTCREATE	"can't create a job for you"
#define CANTCWD		"bad return code from the library routine: getcwd()"
#define CANTPEEK	"you may only look at your own jobs."
#define INVALIDUSER	"you are not a valid user (no entry in /etc/passwd)"
#define	NONUMBER	"proper syntax is:\n\tat -ln\nwhere n is a number"
#define ATNOREADDIR	"can't read the at directory"
#define NOTALLOWED	"you are not authorized to use at.  Sorry."
#define NOTHING		"nothing specified"
#define NOPROTO		"no prototype file"
#define PAST		"it's past that time"
#define CONFOPTS	"conflicting options"
#define NOQUE		"no queue specified"
#define BADQUE		"bad queue specification"
#define NOKSH 		"ksh is currently not available"
#define NOCSH		"csh is currently not available"
#define MALLOC		"out of space, can not continue"
#define BADCHOWN	"at: can not change atjob file owner"
#define ATRMDEL 	"at: only jobs belonging to user: %s may be deleted\n"
#define BSHPATH		"/usr/bin/sh"
#define KSHPATH		"/usr/bin/ksh"
#define CSHPATH		"/usr/bin/csh"

/*
	this data is used for parsing time
*/
#define	dysize(A)	(((A)%4) ? 365 : 366)

/* Current flags */
int	lflag = 0;		/* list atjob files */
int	rflag = 0;		/* remove atjobs files */
int	cflag =	0;		/* execute in c shell */
int	fflag =	0;		/* suppress deletion info */
int	kflag =	0;		/* execute in korn shell */
int	nflag =	0;		/* list number of files */
int	iflag =	0;		/* interactive delete */
int	uflag =	0;		/* user flag */
int	oflag =	0;		/* list in schedule order */
int	shflag = 0;		/* bourne shell flag - default */
int	qflag = 0;		/* select queue */
int	gmtflag = 0;		/* greenwich mean time */

extern int errno;
extern	time_t	timezone;
extern	char	*argp;
char	*nargp;
char	login[UNAMESIZE];
char	argpbuf[50];		/* buffer for description of time */
char	pname[PATH_MAX];
char	pname1[PATH_MAX];
char	jobname[PATH_MAX];
time_t	when, now, gtime();
struct	tm	*tp, at, rt;
int	mday[12] =
{
	31,28,31,
	30,31,30,
	31,31,30,
	31,30,31,
};
int	mtab[12] =
{
	0,   31,  59,
	90,  120, 151,
	181, 212, 243,
	273, 304, 334,
};
int     dmsize[12] = {
	31,28,31,30,31,30,31,31,30,31,30,31};

/* end of time parser */

short	jobtype = ATEVENT;		/* set to 1 if batch job */
short	exeflag = FALSE;		/* execute the filename given? */
char	*tfname;			/* temporary file name */
char	*shell = BSHPATH;		/* shell to run under */

extern	char *xmalloc();
time_t  num();

extern	int list_aj();
extern	int remove_aj();

main(argc, argv)
int  argc;
char *argv[];
{
	struct passwd 	*nptr;
	int 		user, i, fd, catch(void);
	char 		*job, pid[6];
	char 		*pp;
	char 		*mkjobname(),*getuser();
	int  		rc ,st = 0;

	(void) setlocale(LC_ALL,"");
	catd = catopen(MF_CRON,0);

	jobname[0] = '\0';
	getNLS();

#if SEC_BASE
	set_auth_parameters(argc, argv);
	initprivs();

	if (forceprivs(privvec(SEC_CHMODSUGID, SEC_CHOWN,
#if SEC_MAC
				SEC_WRITEUPSYSHI,
#endif
#if SEC_ILB
				SEC_ILNOFLOAT,
#endif
#if SEC_NCAV
				SEC_ALLOWNCAVACCESS,
#endif
				-1), (priv_t *) 0))
		atabort(MSGSTR(MS_ATPRIVS, INSUFFPRIVS));
#if SEC_MAC
	disablepriv(SEC_MULTILEVELDIR);
#endif

	if ((nptr = getpwuid(user = getluid())) == NULL)
#else /* !SEC_BASE */
	if ((nptr=getpwuid(user=getuid())) == NULL) 
#endif /* !SEC_BASE */
		atabort(MSGSTR(MS_INVALIDUSER, INVALIDUSER));
	else 
		pp = nptr->pw_name;

	strcpy(login,pp);
	if (!allowed(login,ATALLOW,ATDENY)) 
		atabort(MSGSTR(MS_NOTALLOWED, NOTALLOWED));
	time(&now);

	argv++; argc--;
	/*
	 * Interpret command line flags if they exist.
	 */
	while (argc > 0 && **argv == '-') {
	  (*argv)++;
	  while (**argv) {
	    switch (*(*argv)++) {
	      case 'c' : /* Select csh */
		cflag = 1; 
	 	jobtype = CSHEVENT;
		shell = CSHPATH;
		break;
	      case 'k' : /* Select ksh */
	        kflag = 1; 
		jobtype = KSHEVENT;
		shell = KSHPATH;
		break;
	      case 's' : /* Select sh */
	 	shflag = 1;
		shell = BSHPATH;
		break;
	      case 'u' : /* delete all for specific user */
		uflag = 1;
		break;
	      case 'f' : /* suppress delete verification */
		fflag = 1;
		break;
	      case 'i' : /* interactive delete */
		iflag = 1;
		break;
	      case 'l' : /* List entries */
		lflag = 1;
		break;
	      case 'n' : /* # of files */
		nflag = 1;
		break;
	      case 'o' : /* scheduled order */
		oflag = 1;
		break;
	      case 'r' : /* Remove entries */
		rflag = 1;
		break;
	      case 'q' : /* Select queue */
		qflag = 1;
		if (**argv) {
		  jobtype = (*(*argv)++) - 'a';
		  if ((jobtype < ATEVENT) || (jobtype > CSHEVENT))
		    atabort(MSGSTR(MS_BADQUE, BADQUE));
		} else
		    atabort(MSGSTR(MS_NOQUE,NOQUE));
		    break;
	      case 'm' : /* Send mail (BSD option, AIX default) */
		break; 

	      default  : /* Unknown switch */
		usage();
	      }
	  }
	  --argc, ++argv;
	}
		  
	if (kflag && access(KSH, X_OK) == -1)
		atabort(MSGSTR(MS_NOKSH,NOKSH));

	if (cflag && access(CSH, X_OK) == -1)
		atabort(MSGSTR(MS_NOCSH,NOCSH));

	if ((lflag + rflag + shflag + cflag + kflag) > 1) 
		atabort(MSGSTR(MS_CONFOPTS,CONFOPTS));

	if (rflag) {
		/* remove jobs that are specified */
		if (argc == 0)
			atabort(MSGSTR(MS_NOTHING, NOTHING));
		i=0;
		do {
#if SEC_BASE
			if (uflag && strcmp(argv[i], login) && !at_authorized())
#else
			if (uflag && (user != ROOT) && (strcmp(argv[i], login)))
#endif
			{
				fprintf(stderr,MSGSTR(MS_ATRMDEL,ATRMDEL), login);
				exit(1);
			}
			if (iflag) {
				if (uflag)
					rc = remove_aj(CRON_PROMPT|CRON_USER,
						argv[i]);
				else
					if (argv[i] != NULL)
						rc = remove_aj(CRON_PROMPT,argv[i]);
			}
			else {
				if (fflag)
					if (uflag)
					    rc = remove_aj(CRON_QUIET|CRON_USER,
						argv[i]);
					else
					    rc = remove_aj(CRON_QUIET,argv[i]);
				else
					if (uflag)
					    rc = remove_aj(CRON_NON_VERBOSE|
					      CRON_USER,argv[i]);
					else
					    rc = remove_aj(CRON_NON_VERBOSE,argv[i]);
			}
		}
		while (argv[++i]!=NULL);
		exit(rc); 
	}

	if (lflag) {
		if (*argv == NULL)
			if (oflag)
				rc = list_aj(CRON_SORT_M,login);
			else
				rc = list_aj(CRON_SORT_E,login);
		else {
#if SEC_BASE
			rc = -1;
#else
			if ((user != ROOT) && 
			    ((argv[1]!=NULL) || (strcmp(*argv, login))))
				atabort(MSGSTR(MS_CANTPEEK, CANTPEEK));
#endif

			for (i=0; argv[i]!=NULL; i++) {
#if SEC_BASE
				if (strcmp(argv[i], login) && !at_authorized())
					continue;
#endif
				if (oflag)
					rc = list_aj(CRON_SORT_M,argv[i]);
				else
					rc = list_aj(CRON_SORT_E,argv[i]);
			}
#if SEC_BASE
			if (rc == -1)
				atabort(MSGSTR(MS_CANTPEEK, CANTPEEK));
#endif
		}
		exit(rc); 
	}

	if (nflag) {
		if (*argv == NULL)
			rc = list_aj(CRON_COUNT_JOBS,login);
		else
#if SEC_BASE
		{
			rc = -1;
			for (i=0; argv[i]!=NULL; i++) {
				if (strcmp(argv[i], login) && !at_authorized())
					continue;
				rc = list_aj(CRON_COUNT_JOBS,argv[i]);
			}
			if (rc == -1)
				atabort(MSGSTR(MS_CANTPEEK, CANTPEEK));
		}
#else
			for (i=0; argv[i]!=NULL; i++) 
				rc = list_aj(CRON_COUNT_JOBS,argv[i]);
#endif
		 exit(rc); 
	}

	fflush(stdout);
	/* figure out what time to run the job */

	if(argc == 0 && jobtype != BATCHEVENT)
		atabort(MSGSTR(MS_NOTHING, NOTHING));
	time(&now);
	if(jobtype != BATCHEVENT) {	/* at job */
		nargp = argpbuf;
		i = st;
		if (access(argv[argc-1],R_OK) != -1) {
			strncpy(jobname,argv[argc-1], strlen(argv[argc-1]));
			if (access(argv[argc-1],X_OK) != -1) 
				exeflag = TRUE;	/* execute this as a command */
			else
				exeflag = FALSE; /* copy the contents of 
						    the file (ala BSD) */
			argc--;
		}
		while(i < argc) {
			strcat(nargp,argv[i]);
			strcat(nargp, " ");
			i++;
		}
		if ((argp = malloc(strlen(nargp)+1)) == NULL)
			atabort(MSGSTR(MS_MALLOC,MALLOC));
		strtolower(argp,nargp);
		tp = localtime(&now);
		mday[1] = 28 + leap(tp->tm_year);
		yyparse();
		atime(&at, &rt);
		mday[1] = 28 + leap(tp->tm_year);
		when = gtime(&at);
		if(!gmtflag) {
			when += timezone;
			if(localtime(&when)->tm_isdst) /* daylight sav tm */
				when -= 60 * 60;
		}
	} else		/* batch job */
		when = now;
	if(when < now)	/* time has already past */
		atabort(MSGSTR(MS_TOOLATE, "too late"));

	fflush(stdout);
	sprintf(pid,"%-5d",getpid());
	tfname=xmalloc(strlen(ATDIR)+strlen(TMPFILE)+7);
	strcpy(tfname, ATDIR);
	strcat(strcat(strcat(tfname, "/"), TMPFILE), pid);
	/* catch SIGINT, HUP, and QUIT signals */
	if (signal(SIGINT,(void(*)(int))catch)==SIG_IGN) signal(SIGINT,SIG_IGN);
	if (signal(SIGHUP,(void(*)(int))catch)==SIG_IGN) signal(SIGHUP,SIG_IGN);
	if (signal(SIGQUIT,(void(*)(int))catch)==SIG_IGN)signal(SIGQUIT,SIG_IGN);
	if (signal(SIGTERM,(void(*)(int))catch)==SIG_IGN)signal(SIGTERM,SIG_IGN);
	if ((fd = open(tfname, O_CREAT|O_EXCL|O_WRONLY, ATMODE)) < 0)
		atabort(MSGSTR(MS_CANTOPEN, CANTOPEN)); /*MSG*/

#if SEC_BASE
	if (fchown(fd, user, getegid()) != 0)
#else
	if (fchown(fd, user, getgid()) != 0)
#endif
	{
		unlink(tfname);
		atabort(MSGSTR(MS_CANTCHOWN, CANTCHOWN)); /*MSG*/
	}
	close(1);
	dup(fd);		/* stdout is now our temp-file */
	close(fd);
	sprintf(pname, "%s", PROTO);
	sprintf(pname1, "%s.%c", PROTO, 'a'+jobtype);
	copy();
#if SEC_BASE
	fflush(stdout);	/* Bug fix, flush job file before sendmsg() */
	/*
	 * Reset the mode of the job file since the SUID and SGID
	 * bits will have been cleared by writes in copy().
	 */
	if (chmod(tfname, ATMODE)) {
		unlink(tfname);
		atabort(MSGSTR(MS_CANTCHMOD, CANTCHMOD));
	}
#endif

	/* make a job name out of the time (when) and link it to tfname */
	/*   returns the last component of the job name 		*/
	job = mkjobname (tfname, when);

	unlink(tfname);
	sendmsg(ADD, job);
	fprintf(stderr,MSGSTR(MS_JOBAT, "job %s will be run at %s\n"),
		job,NLctime(&when));

	exit(0);
}

/****************/
char *mkjobname(ttfname,t)
/****************/
char   *ttfname;
time_t t;
{
	int     i;
	char    *name;
	struct passwd *nptr;
	char *p,*getfilenam();
	char pp[UNAMESIZE];

	name = xmalloc(200);

	for (i = 0; i < MAXTRYS; i++) 
	{
		sprintf(name,"%s/%ld.%c", ATDIR, t, 'a'+jobtype);

		/* if file doesn't exist return */
		/*   if it does then try again  */

		if ((nptr=getpwuid(getuid())) == NULL) /* check the user id */
			atabort(MSGSTR(MS_INVALIDUSER, INVALIDUSER));
	
		strcpy(pp, nptr->pw_name);	/* user's crontab file */

		p = strrchr(name,'/');
		p++;
		strcat(pp,".");
		strcat(pp,p);
		*(strrchr (name, '/') + 1) = '\0';
		strcat (name, pp);

		if (link(ttfname,name) != -1)
			return (strrchr (name, '/') + 1);
			else if (errno != EEXIST)
		{
			unlink(ttfname);
			atabort(MSGSTR( MS_CANTLINK, CANTLINK)); /*MSG*/
		}
		t += 1;
	}
	atabort(MSGSTR(MS_QFULL, "queue full"));
}


/****************/
catch(void)
/****************/
{
	unlink(tfname);
	exit(1);
}


/****************/
atabort(msg)
/****************/
char *msg;
{
	fprintf(stderr,MSGSTR(MS_ATERROR,"at: %s\n"),msg);
	exit(1);
}

yywrap()
{
	return(1);
}

yyerror(s1)
char *s1;
{
	if (s1 == NULL)
		atabort(MSGSTR(MS_BADDATE, BADDATE));
	else
		atabort(s1);
}

/*
 * add time structures logically
 */
atime(a, b)
register
struct tm *a, *b;
{
	if ((a->tm_sec += b->tm_sec) >= 60) {
		b->tm_min += a->tm_sec / 60;
		a->tm_sec %= 60;
	}
	if ((a->tm_min += b->tm_min) >= 60) {
		b->tm_hour += a->tm_min / 60;
		a->tm_min %= 60;
	}
	if ((a->tm_hour += b->tm_hour) >= 24) {
		b->tm_mday += a->tm_hour / 24;
		a->tm_hour %= 24;
	}
	a->tm_year += b->tm_year;
	if ((a->tm_mon += b->tm_mon) >= 12) {
		a->tm_year += a->tm_mon / 12;
		a->tm_mon %= 12;
	}
	a->tm_mday += b->tm_mday;
	while (a->tm_mday > mday[a->tm_mon]) {
		a->tm_mday -= mday[a->tm_mon++];
		if (a->tm_mon > 11) {
			a->tm_mon = 0;
			mday[1] = 28 + leap(++a->tm_year);
		}
	}
}

leap(year)
{
	return(year % 4 == 0);
}

/*
 * return time from time structure
 * determine the number of seconds to target time from 1970
 */
time_t
gtime(ttp)
struct	tm *ttp;
{
	register int i;
	time_t	tv;

	tv = 0;
	for (i = 1970; i < ttp->tm_year+1900; i++)
		tv += dysize(i);
	if (dysize(ttp->tm_year) == 366 && ttp->tm_mon >= 2)
		++tv;
	for (i = 0; i < ttp->tm_mon; ++i)
		tv += dmsize[i];
	tv += ttp->tm_mday - 1;
	tv = 24 * tv + ttp->tm_hour;
	tv = 60 * tv + ttp->tm_min;
	tv = 60 * tv + ttp->tm_sec;
	return(tv);
}

/*
 * make job file from user enviro + stdin
 *
 * AIX code replaced by (almost) straight SYS V.2 code.
 */
copy()
{
	register int c;
	register FILE *pfp;
	register FILE *xfp;
	register char **ep;
	char	dirbuf[PATH_MAX];
	char	tmpchar;
	char *getwd();
	char *strchr();
	mode_t um;
	char *val;
	extern char **environ;

	printf(": %s job\n",jobtype ? "batch" : "at");
	for (ep=environ; *ep; ep++) {
		if (strncmp(*ep, "TERMCAP=", 8) == 0)
			continue;
		if (strncmp(*ep, "TERM=", 5) == 0) {
			printf("TERM='dumb'\n");
			printf("export TERM\n");
			continue;
		}
		if ( strchr(*ep,'\'')!=NULL )
			continue;
		if ((val=strchr(*ep,'='))==NULL)
			continue;
		if (strncmp(*ep, "SHELL=", 6) == 0) {
			printf("export SHELL; SHELL='%s'\n", shell);
			continue;
		}
		*val++ = '\0';
		printf("export %s; %s='%s'\n",*ep,*ep,val);
		*--val = '=';
	}
	if((pfp = fopen(pname1,"r")) == NULL && (pfp=fopen(pname,"r"))==NULL)
		atabort(MSGSTR(MS_NOPROTO, NOPROTO));

	um = umask(0);
	(void) umask(um);
	while ((c = getc(pfp)) != EOF) {
		if (c != '$')
			putchar(c);
		else switch (c = getc(pfp)) {
		case EOF:
			goto out;
		case 'd':
			dirbuf[0] = NULL;
			if (getwd(dirbuf) == NULL)
				atabort(MSGSTR(MS_CANTCWD, CANTCWD));
			printf("%s", dirbuf);
			break;
		case 'l':
			printf("%ld",ulimit(UL_GETFSIZE,-1L));
			break;
		case 'm':
			printf("%o", um);
			break;
		case '<':
			printf("%s << 'QAZWSXEDCRFVTGBYHNUJMIKOLP'\n", shell);
			if (exeflag && (jobname[0] != '\0')) {
				printf("%s\n", jobname);
				break;
			}
			xfp = stdin;		/* job from stdin by default */
			if (jobname[0] != '\0') {
  				/* job from a file */
				if ((xfp=fopen(jobname, "r")) == NULL) {
					perror(jobname);
				}

			}
			while ((c = getc(xfp)) != EOF) {
				tmpchar = c;
				printf("%c",tmpchar);
			}
			/* don't put on single quotes, csh doesn't like it */
			printf("QAZWSXEDCRFVTGBYHNUJMIKOLP\n");
			break;
		case 't':
			printf(":%lu", when);
			break;
		default:
			putchar(c);
		}
	}
out:
	fclose(pfp);
}


/****************/
sendmsg(action,fname)
/****************/
char action;
char *fname;
{

	static	int	msgfd = -2;
	struct	message	*pmsg;

#if SEC_MAC || SEC_NCAV
	pmsg = (struct message *) at_alloc_message(sizeof *pmsg);
#else
	pmsg = &msgbuf;
#endif
	if(msgfd == -2)
		if((msgfd = open(FIFO,O_WRONLY|O_NDELAY)) < 0) {
			if(errno == ENXIO || errno == ENOENT)
				fprintf(stderr,MSGSTR(MS_NOCRON, "cron may not be running - call your system administrator\n"));
			else
				fprintf(stderr,MSGSTR(MS_MSGQERROR, "at: error in message queue open\n"));
			return;
		}
	pmsg->etype = AT;
	pmsg->action = action;
	strncpy(pmsg->fname,fname,(size_t)FLEN);
	strncpy(pmsg->logname,login,(size_t)LLEN);
#if SEC_MAC || SEC_NCAV
	if (!at_send_message(msgfd, (char *) pmsg, sizeof *pmsg))
#else
	if(write(msgfd,(char *)pmsg,(unsigned)sizeof(struct message)) != sizeof(struct message))
#endif
		fprintf(stderr,MSGSTR(MS_MSGSERROR, 
				"at: error in message send\n"));
	
}

/*
 * NAME: usage
 * FUNCTION: display a usage statement to the user
 */
usage()
{
	fprintf(stderr, MSGSTR(MS_USAGE1,
	 	"Usage: at [-cs] time [date] [inc]\n"));
	fprintf(stderr, MSGSTR(MS_USAGE2,"       at -l [-qn]\n"));
	fprintf(stderr, MSGSTR(MS_USAGE3,"       at -r job ...\n"));
	exit(1);
}

