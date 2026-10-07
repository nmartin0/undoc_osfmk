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
static char rcsid[] = "@(#)$RCSfile: mkdir.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:30:21 $";
#endif
/********************************************
 *  The name of this file (in HP/UX -TEJ)is create.c.
 *  It was created in response to a
 *  requirement of POSIX.2.  As the POSIX create utility also subsumed
 *  the functionlity of mkdir(1) and mkfifo(1), the functionality for
 *  both of those utilities was brought into create.c, and they will be
 *  implemented as links to create.
 *
 *  The general structure of this program is :
 *	I.   figure out what name we were called by;
 *	II.  Parse options
 *	III. Perform global, universal processing;
 *	    A.  Set mode if -m option given;
 *	IV.  Loop through each of arguments;
 *	    A.  Create intermediate directories if -p option given;
 *	    B.  Create directory or fifo, depending on how called
 *
 *	11/7/89 -- Randy Campbell, UDL, Fort Collins
 *	This module is functional for the mkdir and mkfifo commands,
 *	but the create portion has not been implemented and may not
 *	be for the 8.0 release.
 ******************************************/

#include <NLctype.h>
#include <locale.h>

#include "mkdir_msg.h"
nl_catd catd;
#define MSGSTR(Num, Str) catgets(catd, MS_MKDIR, Num, Str)

#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

/*  Defines for getmode() */
#define USER	05700
#define GROUP	02070
#define OTHER	00007
#define ALL	07777
#define READ	00444
#define WRITE	00222
#define EXECUTE	00111
#define SETID	06000
#define SETUID	04000
#define SETGID	02000
#define STICKY	01000
/* end of defines for getmode() */

#define CREATE		0		/* utility number for case */
#define MKDIR		1		/* utility number for case */
#define MKFIFO		2		/* utility number for case */
int	utility_num	= 0;		/* active utility number */
					/*  "create" is default */

/* Default creation modes for the various utilities.  Needed by getmode */
mode_t	defmode[3] = { S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH | S_IRUSR | S_IWUSR,
		       S_IRWXU | S_IRWXG | S_IRWXO,
		       S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH | S_IRUSR | S_IWUSR
		     };


/*  Possible options for the various utilities */
char 	*opts[3] = {
	    "Pdfm:npqtx:",   	/* create */
	    "pm:",		/* mkdir  */
	    "pm:" };		/* mkfifo */

char no_access[] = "%s: cannot access ";	/* catgets 1 */
char no_make[]   = "%s: cannot create ";	/* catgets 2 */
char no_f_d[]    = "%s: -d and -f options are mutually exclusive\n"; /* catgets 3 */
char no_mult[] = "%s: can't have multiple -%c options\n"; /*  catgets 4 */
char bad_mode[] = "%s: incorrect syntax in mode argument\n"; /* catgets 5 */

char usage1[] = "usage: create [-d | -f] [-npqtP] [-m mode] [-x prefix] name ...\n";
char usage2[] = "usage: mkdir [-p] [-m mode] dirname ...\n";
char usage3[] = "usage: mkfifo [-p] [-m mode] file ...\n";

char	*optstring	= NULL;		/* active optstring */
char	*name;				/* name called by */
					/* option flags */
	int	Pflag	= 0;		/* POSIX.1 */
	int	dflag	= 0;		/* directory  */
	int	fflag	= 0;		/* fifo */
	int	mflag	= 0;		/* mode */
	int	nflag	= 0;		/* name */
	int	pflag	= 0;		/* path */
	int	qflag	= 0;		/* quite */
	int	tflag	= 0;		/* temporary */
	int	xflag	= 0;		/* prefix */

					/* option parameters */
	char	*modestr;		/* mode string (ms)*/
	char	*strtok();
	int	Hflag,usflag;
	int	err;			/* error counter */
	mode_t	mask;			/* mode mask  */
	mode_t	mode;			/* mode */

struct	stat buf;
	char	*prefix	= "";		/* prefix */

char m_op();

	int	c;			/* for getopt */
extern	char	*optarg;		/* for getopt */
extern	int	optind, opterr;         /* for getopt, opterr=0 for no errors */

int
main(argc,argv)
	int	argc;
	char	*argv[];
{
	int	i;
	char 	*slash;

	/* determine the name called by, by taking everything after the 
	 * last slash (if any).  utility_num identifies which command */
	if ((slash = strrchr(argv[0],'/')) == 0)
		name = argv[0];
	else
		name = (slash + 1);
	if(name[0] == 'c')
	    utility_num = 0;		/* create */
	else if (name[0] == 'm' && name[1] == 'k')
		if (name[2] == 'd')
		    utility_num = 1; 	/* mkdir  */
		else utility_num = 2;	/* mkfifo */

	/* optstring is used in getopt to parse correct options for the
	 * calling command */
	optstring = opts[utility_num];

	(void) setlocale(LC_ALL, "");

	catd = catopen(MF_MKDIR, 0);

	/* parse options */
	while ((c=getopt(argc,argv,optstring))!=EOF)
	{
	    switch(c) 
	    {

		case 'P':	/* create only */
		    ++Pflag;
		    break;

		case 'd':	/* create only */
		    if (fflag)
		    {
			(void)fprintf(stderr,
			    MSGSTR(NOFD, no_f_d),
			    name);
			exit(2);
		    }
		    ++dflag;
		    break;

		case 'f':	/* create only */
		    if (dflag)
		    {
			(void)fprintf(stderr,
			    MSGSTR(NOFD,no_f_d),
			    name);
			exit(2);
		    }
		    ++fflag;
		   break;

		case 'm':	/* create, mkdir, mkfifo */
		    if (mflag)
		    {
			(void)fprintf(stderr,
			    MSGSTR(NOMULT, no_mult),
			    name, c);
			exit(2);
		    }
		    ++mflag;
		    modestr = optarg;
		    break;

		case 'n':	/* create only */
		    ++nflag;
		    break;

		case 'p':	 /* create, mkdir, mkfifo */
		    ++pflag;
		    break;

		case 'q':	/* create only */
		    ++qflag;
		    break;

		case 't':	/* create only */
		    ++tflag;
		    break;

		case 'x':	/* create only */
		    if (xflag)
		    {
			(void)fprintf(stderr,
			    MSGSTR(NOMULT, no_mult),
			    name, c);
			exit(2);
		    }
		    ++xflag;
		    prefix = optarg;
		    break;

		case '?':
		default:
		    err = 2;
		    break;

	    } /* switch */
	} /* while */

	if (argc == 1 || argc == optind || err)	{	/* errors detected */
	    usage(utility_num);
	    exit(2);
	}


	/*  if the -m option was given, its argument, modestr, is
	 *  passed to getmode along with the default creation mode 
	 *  for the type of file being created (defmode[utility_num]).
	 *  getmode returns a value suitable for use with mkdir(2) or
	 *  mkfifo(3c)
	 */
	if (mflag) {
	    int int_mode;

	    int_mode = getmode(modestr,defmode[utility_num]);
	    if (int_mode < 0) {
		(void)fprintf(stderr,
		    MSGSTR(BADMODE, bad_mode), name);
	  	exit(2);
	    }
	    mode = (mode_t)int_mode;
	}
	else mode = defmode[utility_num];

	/* use the remaining arguments as file names */
	for (i = optind; i < argc; i++)
	{
	    /*  Process common flags */
	    if (pflag) {
		/*  User's umask may affect file/dir creation modes */
		mask = umask(0);
        	(void)umask(mask);
		if(mkdirp(argv[i]) < 0) {
			err = 2;
			continue;
		}
	    }

	    switch(utility_num)
	    {

		case CREATE:
	    		if(create(argv[i], mode, mask) < 0)
			    err = 2;
			break;

		case MKDIR:
			if(Mkdir(argv[i],mode) < 0) 
			    err = 2;
			break;

		case MKFIFO:
			if(Mkfifo(argv[i],mode) < 0)
			     err = 2;
			break;

	    }

	}

	exit(err);
}


/*******************
 *  create() is stubbed out for now, not implemented in OSF/1
 ******************/
int
create(name, modestr, mask)		/* create utility */
	char	*name;
	char	*modestr;
	mode_t	mask;
{

	return(0);
}


/*********************************
 *  Mkdir takes a pathname and a mode and passes them as arguments 
 *  to mkdir(2).  For failures,
 *  report() is called to generate an error message, and -1 is returned.
 *********************************/
int
Mkdir(d,m)
unsigned char *d;
mode_t m;
{

    if (mkdir((char *)d, m) == 0)
	return(0);
		       
    report(d);
    return(-1);
}

/******************************
 *  Mkfifo operates very much like like Mkdir in that it passes a pathname
 *  and a mode to mkfifo(3c) and reports no error if the pathname names
 *  an existing FIFO.
 ******************************/
int
Mkfifo(d,m)
unsigned char *d;
mode_t m;
{

    if (mkfifo((char *)d, m) == 0)
	return(0);
		       
    report(d);
    return(-1);
}

/******************************
 *  mkdirp is the routine used by all calling commands to create
 *  intermediate path components if the -p option is given.  
 *  All components are created by mkdir(2) using a mode of 0777.
 *  If the umask is set so as to prevent the user wx bits from being
 *  set, chmod(2) is called to ensure that at least those mode bits
 *  are set so that following path components can be created.
 *  If any directory already exists, it is silently ignored.
 ******************************/
int
mkdirp(dir)
unsigned char *dir;
{
    mode_t dmode = 0777;
    unsigned char *dirp;
    int	save_errno = errno;

    /*
     * Skip any leading '/' characters
     */
    for (dirp = dir; *dirp == '/'; dirp++)
	continue;

    /*
     * For each component of the path, make sure the component
     * exists.  If it doesn't exist, create it.
     */
    while ((dirp = NLstrchr(dirp, '/')) != (unsigned char *)NULL)
    {
	*dirp = '\0';
	if (mkdir((char *)dir, dmode) != 0 && errno != EEXIST)
	{
	    (void)fprintf(stderr,
		MSGSTR(NOMAKE, no_make), name);
	    perror((char *)dir);
	    return (-1);
	}
	/*  If this directory did not already exist AND
	 *  the umask prevented the user wx bits from being set,
	 *  then chmod it to set at least u=wx so the next one can be
	 *  created.
	 */
	if((mask & 0300) && errno != EEXIST)
	    chmod((char *)dir,(mode ^ mask) | 0300);

	for (*dirp++ = '/'; *dirp == '/'; dirp++)
	    continue;
    }
    errno = save_errno;
    return(0);
}


/******************************
 *  getmode takes a modestring (supplied as the argument to the -m
 *  option) and a default file mode, and returns a mode suitable for
 *  use as the mode argument to mkdir() or mkfifo().
 *  getmode has to know the default because the '+' and '-' operators
 *  are interpreted relative to the default for the type of file being
 *  created.
 *  If getmode can't make sense out of the modestring, it returns -1.
 *******************************/
int
getmode(ms,d)
char *ms; /* modestring */
mode_t	d;
{
	mode_t um;
	int m,md,mode_who,mode_perm;
	char mode_op;


	um = ~mask;
	m=abs(ms);
	if (m != 0) return (m);
	m=d;

	/*  Separate parts of the modestring may be comma-separated */
	ms=strtok(ms,",");
	Hflag=0;
	while (ms && *ms) {
		usflag=0;

		/*  Dtermine to whom this part applies  */
		mode_who=who(ms);

		/*  Get operator  */
		if ((mode_op=m_op(ms))=='\0') {
			return(-1);
		}

		/*  get permissions */
		mode_perm=perm(ms);
		if (!mode_who) {
			switch(mode_op) {
				case '+':
					md=mode_perm&(um|SETID); break;
				case '-':
					md=mode_perm; break;
				case '=':
					md=mode_perm&um; break;
				default:
					return(-1);
			}
		} else {
			md = mode_perm&(usflag|mode_who);
		}
		if (Hflag) md|=SETUID;
		switch (mode_op) {
			case '+':
				m|=md;
				break;
			case '-':
				m&=(~md);
				break;
			case '=':
				m=(m&~mode_who)|md;
				break;
		}
		ms=strtok(NULL,",");
	}
	return(m);	
}

who(ms)		/* who is mode string for? */
char *ms;
{
	int md=0;

	for (;*ms;ms++) {
		switch(*ms) {
			case 'u':
				usflag |= SETUID;
				md |= USER;
				break;
			case 'g':
				usflag |= SETGID;
				md |= GROUP;
				break;
			case 'o':
				md |= OTHER;
				break;
			case 'a':
				md = ALL;
				break;
		}
	}
	return(md);
}

char m_op(ms)		/* find operator in mode string */
char *ms;
{
	for (;*ms;ms++) {
		switch(*ms) {
			case '+':
			case '-':
			case '=':
				return(*ms);
		}
	}
	return('\0');
}

perm(ms)	/* create permissions mask from mode string */
char *ms;
{
	int md=0;

	for (;*ms;ms++) {
		switch(*ms) {
			case 'r':
				md |= READ;
				break;
			case 'w':
				md |= WRITE;
				break;
			case 'x':
				md |= EXECUTE;
				break;
			case 's':
				md |= SETID;
				break;
			case 't':
				md |= STICKY;
				break;
			case 'H':
				++Hflag;
				md |= SETUID;
				break;
		}
	}
	return(md);
}

abs(ms)		/*  break out mode given as an integer in octal */
char *ms;
{
        register c, i;

        i = 0;
        while ((c = *ms++) >= '0' && c <= '7')
                i = (i << 3) + (c - '0');
        ms--;
        return(i);
}

/*******************************
 *  report is called from Mkdir or Mkfifo to generate an error message
 *  if the dir or FIFO could not be created for some reason.
 ********************************/
int
report(d)
unsigned char *d;
{
	int save_errno = errno;

	/*
	 * For EACCES, ENOENT and ENOTDIR errors, we print the name
	 * of the parent of the target directory instead of the
	 * target directory.
	 */
	if (errno == EACCES || errno == ENOENT || errno == ENOTDIR)
	{
		unsigned char *slash;

		if ((slash = NLstrrchr(d, '/')) != (unsigned char *)NULL)
		{
			if (slash == d)
				slash++;
			*slash = '\0';
		}
		else
		{
			d[0] = '.';
			d[1] = '\0';
		}

		(void)fprintf(stderr,
		    MSGSTR(NOACCESS, no_access), name);
	}
	else
		(void)fprintf(stderr,
		    MSGSTR(NOMAKE, no_make), name);

	errno = save_errno;
	perror((char *)d);
}

int	
usage(n)			/* print usage message for utility */
	int	n;
{
    if (n >= CREATE && n <= MKFIFO) {
	switch (n) {
		case CREATE:
			fputs(MSGSTR(USAGE1, usage1), stderr);
			break;
		case MKDIR:
			fputs(MSGSTR(USAGE2, usage2), stderr);
			break;
		case MKFIFO:
			fputs(MSGSTR(USAGE3, usage3), stderr);
			break;
	}  /* switch */
    } /* if */
}
