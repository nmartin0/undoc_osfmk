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
static char rcsid[] = "@(#)$RCSfile: xargs.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:50:15 $";
#endif
/*
 * COMPONENT_NAME: (CMDSH) Bourne shell and related commands
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 27
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
 * Copyright 1976, Bell Telephone Laboratories, Inc.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <locale.h>

#ifdef KJI
# include	<NLctype.h>
#endif

# include	"xargs_msg.h"
nl_catd	catd;
# define	MSGSTR(Num, Str)	catgets(catd, MS_XARGS, Num, Str)

/*
 * 'boolean' constants ...
 */
#define FALSE		0
#define TRUE		1

#define MAXSBUF		255		/* tmp replacement buffer size */
#define MAXIBUF		512		/* total replacement buffer size */
#define MAXINSERTS	5		/* max replacements per line */
#define BUFSIZE		570		/* size for generic character arrays */
#define MAXBUFLIM	(BUFSIZE - 100)	/* max command line size */
#define MAXARGS		255		/* max # of command line args */
#define	ECHO		"/bin/echo"	/* default command to run */

/*
 * functions called, both external and local ...
 */

int xindex(), echoargs(), lcall();
void ermsg(), addibuf();
char *addarg(), *getarg(), *checklen(), *insert();

#ifdef KJI
NLchar getchr();
#else
char getchr();
#endif

/*
 * globals ...
 */
char Errstr[BUFSIZE];		/* generic error message buffer	*/
char *arglist[MAXARGS+1];	/* ptrs to args for the command to execute */
char argbuf[BUFSIZE+1];		/* destination for copied in arguments */
char *next = argbuf;		/* pointer to next empty spot in 'argbuf' */
char *lastarg = "";		/* last arg we parsed, but didn't use yet */
char **ARGV = arglist;		/* next empty slot in 'arglist' */
char *LEOF = "_"; 		/* logical end-of-file string */
char *INSPAT = "{}";		/* replace string pattern */
struct inserts {
	char **p_ARGV;		/* where to put newarg ptr in arg list */
	char *p_skel;		/* ptr to arg template */
	} saveargv[MAXINSERTS];
char ins_buf[MAXIBUF];		/* insert buffer */
char *p_ibuf;			/* pointer within ins_buf */
int PROMPT = -1;		/* prompt /dev/tty file descriptor */
int BUFLIM = MAXBUFLIM;		/* max command line size */
int N_ARGS = 0;			/* # of standard input args to use per cmd */
int N_args = 0;			/* # of arguments we've read so far */
int N_lines = 0;		/* # of input lines used so far for this cmd */
int DASHX = FALSE;		/* -x arg given? */
int MORE = TRUE;		/* process more input? */
int PER_LINE = FALSE;		/* # of input lines to use per cmd */
int ERR = FALSE;		/* should we stop from errors yet? */
int OK = TRUE;			/* had any errors yet? */
int LEGAL = FALSE;		/* stop if argument list size > BUFLIM */
int TRACE = FALSE;		/* echo cmd and args each time? */
int INSERT = FALSE;		/* do command replacements? */
int linesize = 0;		/* current size of cmd & args */
int ibufsize = 0;		/* current length of ins_buf */

/*
 * NAME:	xargs
 *
 * SYNTAX:	xargs [flags] command
 *
 * FUNCTION:	Xargs constructs argument lists and runs commands
 *
 * RETURN VALUE DESCRIPTION:	1 on error, 0 otherwise
 */

int
main(argc, argv)
int argc;
char **argv;
{
	char *cmdname, *initbuf, **initlist, *flagval;
	int  initsize;
	register int j, n_inserts;
	register struct inserts *psave;

	(void) setlocale (LC_ALL, "");

	/* initialization */

	catd = catopen(MF_XARGS, 0);

	argc--; argv++;
	n_inserts = 0;
	psave = saveargv;

	/* look for flag arguments */

	while  ( *argv != NULL && (*argv)[0] == '-'  ) {
		flagval = *argv+1;

		switch ( *flagval++ ) {

		case 'x':	/* quit if any arg list size > BUFLIM */
			DASHX = LEGAL = TRUE;
			break;

		case 'l':	/* specify number of input arg lines per cmd */
			PER_LINE = LEGAL = TRUE;
			N_ARGS = 0;
			INSERT = FALSE;
			if( *flagval && (PER_LINE=atoi(flagval)) <= 0 ) {
				sprintf(Errstr, MSGSTR(LINECNT,
					"#lines must be positive int: %s\n"),
					*argv);
				ermsg(Errstr);
				}
			break;

		case 'i':	/* process replace strings "{}" in arglist */
			INSERT = PER_LINE = LEGAL = TRUE;
			N_ARGS = 0;
			if ( *flagval )
				INSPAT = flagval; /* change replace string */
			break;

		case 't':	/* echo each argument list to stderr */
			TRACE = TRUE;
			break;

		case 'e':	/* set logical end-of-file string */
			LEOF = flagval;
			break;

		case 's':	/* set max size of arg list (max: MAXBUFLIM) */
			BUFLIM = atoi(flagval);
			if( BUFLIM > MAXBUFLIM  ||  BUFLIM <= 0 ) {
				sprintf(Errstr, MSGSTR(LINESIZ,
					"0 < max-cmd-line-size <= 470: %s\n"),
					*argv);
				ermsg(Errstr);
				}
			break;

		case 'n':	/* number of arguments to use per cmd */
			if( (N_ARGS = atoi(flagval)) <= 0 ) {
				sprintf(Errstr, MSGSTR(ARGCNT,
					"#args must be positive int: %s\n"),
					*argv);
				ermsg(Errstr);
				}
			  else {
				LEGAL = DASHX || N_ARGS==1;
				INSERT = PER_LINE = FALSE;
				}
			break;

		case 'p':	/* prompt for each cmd before running */
			if( (PROMPT = open("/dev/tty",0)) == -1)
				ermsg(MSGSTR(TTYREAD,
					"can't read from tty for -p\n"));
			else
				TRACE = TRUE;
			break;

		default:
			sprintf(Errstr,
				MSGSTR(UNKWNOPT, "unknown option: %s\n"),
				*argv);
			ermsg(Errstr);
			break;
		}

		argv++;
		if ( --argc < 1)
			break;
		}

	if( ! OK )
		ERR = TRUE;

	/* pick up command name */

	if ( argc == 0 ) {
		cmdname = ECHO;
		*ARGV++ = addarg(cmdname);	/* add echo into our argv */
		}
	else
		cmdname = *argv;	/* will be added to argv below */

	/* pick up args on command line */

	while ( OK == TRUE && argc-- > 0) {
		if ( INSERT == TRUE && ! ERR ) {
			/* does this argument have an insert pattern? */
			if ( xindex(*argv, INSPAT) != -1 ) {
				/* yes, keep track of which arg has it */
				if ( ++n_inserts > MAXINSERTS ) {
					sprintf(Errstr, MSGSTR(ARGSIZ,
						"too many args with %s\n"),
						INSPAT);
					ermsg(Errstr);
					ERR = TRUE;
				}	
				psave->p_ARGV = ARGV;
				(psave++)->p_skel = *argv;
				}
			}
		/* add arg to our new argv */
		*ARGV++ = addarg( *argv++ );
		}

	/* pick up args from standard input */

	initbuf = next;			/* save first spot past cmd & argv */
	initlist = ARGV;		/* save first argv past cmd & argv */
	initsize = linesize;		/* save current total cmd size */

	/* loop once for each time we need to call cmd... */
	while ( OK == TRUE && MORE ) {
		/*
		 * reset our pointers and line size
		 */
		next = initbuf;
		ARGV = initlist;
		linesize = initsize;

		/*
		 * get any previous arguments we didn't process yet
		 */
		if ( *lastarg )
			*ARGV++ = addarg( lastarg );

		/*
		 * get the new args
		 */
		while ( (*ARGV++ = getarg()) && OK == TRUE )
			;

		/* insert arg if requested */

		if ( !ERR && INSERT == TRUE ) {
			p_ibuf = ins_buf;
			ARGV--;
			if (*ARGV == NULL)
				break;
			j = ibufsize = 0;
			for ( psave=saveargv;  ++j<=n_inserts;  ++psave ) {
				addibuf(psave);
				if ( ERR )
					break;
				}
			}
		*ARGV = 0;

		/* exec command */

		if ( ! ERR ) {
			if ( ! MORE &&
			    (PER_LINE && N_lines==0 || N_ARGS && N_args==0) )
				exit (0);
			OK = TRUE;
			j = TRACE ? echoargs() : TRUE;
			if( j ) {
				if ( lcall(cmdname, arglist) != -1 )
					continue;
				sprintf(Errstr, MSGSTR(NOEXEC,
					"%s not executed or returned -1\n"),
					cmdname);
				ermsg(Errstr);
				}
			}
		}

	exit (OK == TRUE ? 0 : 1);

	/* NOTREACHED */
}

/*
 * NAME:	checklen
 *
 * FUNCTION:	checklen - check length of current arguments plus a new one
 *
 * NOTES:	Checklen checks the length of the current arguments plus
 *		the new one.  If we've gone last BUFLIM, we possibly print
 *		an error and set some error flags.
 *
 * RETURN VALUE DESCRIPTION:	NULL if we've gone past BUFLIM, else
 *		we return the new argument
 */

char *
checklen(arg)
char *arg;
{
	register int oklen;

	oklen = TRUE;
	if ( (linesize += strlen(arg)+1) > BUFLIM ) {
		lastarg = arg;
		oklen = OK = FALSE;
		if ( LEGAL ) {
			ERR = TRUE;
			ermsg(MSGSTR(ARGLIST, "arg list too long\n"));
			}
		else if( N_args > 1 )
			N_args = 1;
		else {
			ermsg(MSGSTR(SNGLARG,
		"a single arg was greater than the max arglist size\n"));
			ERR = TRUE;
			}
		}

	return ( oklen == TRUE  ? arg : 0 );
}

/*
 * NAME:	addarg
 *
 * FUNCTION:	addarg - copy in our new arg
 *
 * NOTES:	Addarg copies in our new arg, then calls checklen().
 *
 * RETURN VALUE DESCRIPTION:	return value from checklen()
 */

char *
addarg(arg)
char *arg;
{
	(void) strcpy(next, arg);
	arg = next;
	next += strlen(arg) + 1;

	return ( checklen(arg) );
}

/*
 * NAME:	getarg
 *
 * FUNCTION:	getarg - read our next argument from stdin
 *
 * NOTES:	Getarg reads/parses our next argument from stdin.
 *
 * RETURN VALUE DESCRIPTION:	0 if there are no more args or if
 *		the arg doesn't fit, else a pointer to the arg
 */

char *
getarg()
{
#ifdef KJI
	extern NLchar getchr();
	register NLchar c, c1;
#else
	register char c, c1;
#endif
	register char *arg;
	char *retarg;

	/*
	 * skip white space
	 */
#ifdef KJI
	while ((c=getchr()) && NCisspace(c))
#else
	while ( (c=getchr()) == ' ' || c == '\n' || c == '\t' )
#endif
		;

	/*
	 * all done?
	 */
	if ( c == '\0' ) {
		MORE = FALSE;
		return 0;
		}

	arg = next;
	/*
	 * read chars and process them ...
	 */
	for ( ; ; c = getchr() )
		switch ( c ) {

		case '\t':		/* white space */
		case ' ' :
			/*
			 * only save white space if we have to do replace
			 * patterns...
			 */
#ifdef KJI
			space:
			if ( INSERT == TRUE ) {
				if (NCisshift(_NCtop(c))) {
					*next++ = _NCtop(c);
					*next++ = _NCbot(c);
					}
				else
					*next++ = c;
				break;
				}
#else
			if ( INSERT == TRUE ) {
				*next++ = c;
				break;
				}
#endif

		case '\0':		/* end of line/file */
		case '\n':
			*next++ = '\0';	/* null terminate current arg */
			/*
			 * process logical eof
			 */
			if( strcmp(arg, LEOF) == 0 || c == '\0' ) {
				MORE = FALSE;
				while ( c != '\n' )
					c = getchr();
				return 0;
				}
			else {
				/*
				 * add arg to our arguments
				 */
				++N_args;
				if( retarg = checklen(arg) ) {
					if( (PER_LINE && c=='\n' &&
					     ++N_lines>=PER_LINE)
					||   (N_ARGS && N_args>=N_ARGS) ) {
						N_lines = N_args = 0;
						lastarg = "";
						OK = FALSE;
						}
					}
				return retarg;
				}

		case '\\':		/* backslash */
#ifdef KJI
			c = getchr();
			if (NCisshift(_NCtop(c))) {
				*next++ = _NCtop(c);
				*next++ = _NCbot(c);
				}
			else
				*next++ = c;
#else
			*next++ = getchr();
#endif
			break;

		case '"':		/* quotes and double quotes */
		case '\'':
			while( (c1=getchr()) != c) {
				/* copy chars in till hit the next one */
				if( c1 == '\0' || c1 == '\n' ) {
					/* missing a quote... */
					*next++ = '\0';
					sprintf(Errstr, MSGSTR(MSNGQUOT,
						"missing quote?: %s\n"), arg);
					ermsg(Errstr);
					ERR = TRUE;
					return (0);
					}
#ifdef KJI
				if (NCisshift(_NCtop(c1))) {
					*next++ = _NCtop(c1);
					*next++ = _NCbot(c1);
					}
				else
#endif
				*next++ = c1;
				}
			break;

		default:
#ifdef KJI
			if (isjspace(c))
				goto space;

			if (NCisshift(_NCtop(c))) {
				*next++ = _NCtop(c);
				*next++ = _NCbot(c);
				}
			else
#endif
			*next++ = c;
			break;
		}
}

/*
 * NAME:	ermsg
 *
 * FUNCTION:	ermsg - print an error message to standard error
 *
 * NOTES:	Errmsg prints the error messages 'messages' to
 *		standard error and sets the OK flag to false.
 *
 * RETURN VALUE DESCRIPTION:	none
 */

void
ermsg(messages)
char *messages;
{
	write(2, "xargs: ", 7);
	write(2, messages, strlen(messages));

	OK = FALSE;
}

/*
 * NAME:	echoargs
 *
 * FUNCTION:	echoargs - print out arguments and prompt for yes or
 *		no
 *
 * NOTES:	Echoargs prints out the arguments and prompts the user
 *		for a yes or no answer.
 *
 * RETURN VALUE DESCRIPTION:	TRUE if the user responds with 'y', else
 *		FALSE
 */

int
echoargs()
{
	register char **anarg;
	char yesorno[2], junk[1];
	register int j;
	char *prompt;

	anarg = arglist-1;
	while ( *++anarg ) {
		write(2, *anarg, strlen(*anarg) );
		write(2," ",1);
		}

	if( PROMPT == -1 ) {
		write(2,"\n",1);
		return TRUE;
		}

	prompt = MSGSTR(QUESTION, "?...");
	write(2, prompt, strlen(prompt));

	if( read(PROMPT,yesorno,1) == 0 )
		exit(0);

	yesorno[1] = '\0';
	if( yesorno[0] == '\n' )
		return FALSE;

	while( ((j=read(PROMPT,junk,1))==1) && (junk[0]!='\n') )
		;

	if( j==0 )
		exit (0);

 	if(NLyesno(yesorno) > 0)
		return(TRUE);
	else
		return(FALSE);
}

/*
 * NAME:	insert
 *
 * FUNCTION:	insert - handle a replacement
 *
 * NOTES:	Insert takes a pattern and a replacement string and
 *		does any replacements necessary in the pattern.  The new
 *		string is returned.
 *
 * RETURN VALUE DESCRIPTION:	The new string after replacements.
 */

#ifdef KJI

char *
insert(pattern, subst)
char *pattern;			/* pattern in argv */
char *subst;			/* string to substitute with */
{
	static char buffer[MAXSBUF+1];
	int len, clen, ipatlen;
	register char *pat;
	register char *bufend;
	register char *pbuf;

	len = strlen(subst);
	ipatlen = strlen(INSPAT);
	pat = pattern;
	pbuf = buffer;
	bufend = &buffer[MAXSBUF];

	while (*pat) {
		if( xindex(pat, INSPAT) == 0 ) {
			if ( pbuf+len >= bufend )
				break;
			else {
				strcpy(pbuf, subst);
				pat += ipatlen;
				pbuf += len;
				}
			}
		else {
			clen = NLchrlen(pat);
			if (pbuf+clen >= bufend )
				break;
			for (; clen > 0; clen--)
				*pbuf++ = *pat++;
			}
		}

	if ( ! *pat ) {
		*pbuf = '\0';
		return (buffer);
		}

	sprintf(Errstr, MSGSTR(MAXSARGSIZ,
		"max arg size with insertion via %s's exceeded\n"), INSPAT);
	ermsg(Errstr);
	ERR = TRUE;

	return 0;
}

#else	/* NOT KJI */

char *
insert(pattern, subst)
char *pattern;
char *subst;
{
	static char buffer[MAXSBUF+1];
	int len, ipatlen;
	register char *pat;
	register char *bufend;
	register char *pbuf;

	len = strlen(subst);
	ipatlen = strlen(INSPAT)-1;
	pat = pattern-1;
	pbuf = buffer;
	bufend = &buffer[MAXSBUF];

	while ( *++pat ) {
		if( xindex(pat,INSPAT) == 0 ) {
			if ( pbuf+len >= bufend )
				break;
			else {
				strcpy(pbuf, subst);
				pat += ipatlen;
				pbuf += len;
				}
			}
		else {
			*pbuf++ = *pat;
			if (pbuf >= bufend )
				break;
			}
		}

	if ( ! *pat ) {
		*pbuf = '\0';
		return (buffer);
		}

	sprintf(Errstr, MSGSTR(MAXSARGSIZ,
		"max arg size with insertion via %s's exceeded\n"), INSPAT);
	ermsg(Errstr);
	ERR = TRUE;

	return 0;
}

#endif KJI

/*
 * NAME:	addibuf
 *
 * FUNCTION:	addibuf - perform a command replacement
 *
 * NOTES:	Addibuf looks at a struct insert structure, which
 *		contains the information for one replacement in
 *		the command string, and does the replacement.
 *
 * RETURN VALUE DESCRIPTION:	none
 */

void
addibuf(p)
struct inserts *p;
{
	register char *newarg, *skel, *sub;
	int l;

	skel = p->p_skel;	/* place in argv the replacement was */
	sub = *ARGV;		/* destination ARGV for the replacement */
	linesize -= strlen(skel)+1;
	newarg = insert(skel, sub);	/* do the replacement */

	/*
	 * make sure the replacement fits
	 */
	if ( checklen(newarg) ) {
		if( (ibufsize += (l=strlen(newarg)+1)) > MAXIBUF) {
			ermsg(MSGSTR(OVRFLOW, "insert-buffer overflow\n"));
			ERR = TRUE;
			}

		strcpy(p_ibuf, newarg);	/* save replacement string */
		*(p->p_ARGV) = p_ibuf;	/* point to it */
		p_ibuf += l;		/* increment replacement pointer */
		}
}

/*
 * NAME:	getchr
 *
 * FUNCTION:	getchr - read the next character from standard input
 *
 * NOTES:	Getchr reads the next character from standard input
 *		and returns it.  0 is returned on end of file or error.
 *
 * RETURN VALUE DESCRIPTION:	0 if end of file is encountered or
 *		we get an error from read.  else the character read is
 *		returned
 */

#ifdef KJI

NLchar
getchr()
{
	char c, c1;

	if ( read(0, &c, 1) == 1 ) {
		if (NCisshift(c)) {
			if ( read(0, &c1, 1) == 1 )
				return (_NCd2(c, c1));
			return ((NLchar) 0);
			}
		return ((NLchar) c);
		}

	return ((NLchar) 0);
}

#else

char
getchr()
{
	char c;

	if ( read(0, &c, 1) == 1 )
		return (c);

	return (0);
}

#endif

/*
 * NAME:	lcall
 *
 * FUNCTION:	lcall - exec program with arguments
 *
 * NOTES:	Lcall forks a new process and executes the program 'sub'
 *		using 'subargs' as it's arguments.  It also waits for the
 *		program to finish and returns a status code indicating
 *		the success of the program.
 *
 * RETURN VALUE DESCRIPTION:	-1 if the fork or exec failed, else
 *		the exit status of the program.
 */

int
lcall(sub, subargs)
char *sub, **subargs;
{

	int retcode;
	register pid_t iwait; 
	pid_t child;

	switch( child=fork() ) {

	default:
		while( (iwait = wait(&retcode)) != child  &&  iwait!= -1 )
			;

		/* exit code macros are in sys/wait.h ... */
		if( iwait == -1  ||  !WIFEXITED(retcode))
			return -1;

		return( WEXITSTATUS(retcode) );

	case 0:    /* child */
		execvp(sub, subargs);
		sprintf(Errstr, MSGSTR(NOEXEC,
				"%s not executed or returned -1\n"),
				sub);
		ermsg(Errstr);
		exit (-1);

	case -1:
		return (-1);
	}

	/* NOTREACHED */
}

/*
 * NAME:	xindex
 *
 * FUNCTION:	xindex - search for substring
 *
 * NOTES:	Xindex searches for a substring 'as2' in the string 'as1'.
 *		If the substring exists, it returns the offset of the
 *		first occurrence.  If it doesn't exist, it returns -1.
 *
 * RETURN VALUE DESCRIPTION:	-1 if the substring doesn't exist, else
 *		the index of the first occurrence of the substring
 */

#ifdef KJI

int
xindex(as1,as2)
char *as1,*as2;
{
	register char *s1,*s2;
	register NLchar c1, c2, c1sav;
	int offset;

	s1 = as1;
	s2 = as2;
	c2 = NCdechr(s2);

	while ((c1 = NCdechr(s1)) != 0)
		if (c1 == c2) {
			offset = s1 - as1;
			c1sav = c1;
			do {
				s1 += NCchrlen(c1);
				s2 += NCchrlen(c2);
				c1 = NCdechr(s1);
				c2 = NCdechr(s2);
			} while ((c1 == c2) && c2);

			if (c2 == 0)
				return(offset);
			s1 = offset + as1 + NCchrlen(c1sav);
			s2 = as2;
			c2 = NCdechr(s2);
		}
		else {
			s1 += NCchrlen(c1);
		}

	 return(-1);
}

#else	/* NOT KJI */

int
xindex(as1,as2)
char *as1,*as2;
{
	register char *s1,*s2,c;
	int offset;

	s1 = as1;
	s2 = as2;
	c = *s2;

	while (*s1)
		if (*s1++ == c) {
			offset = s1 - as1 - 1;
			s2++;
			while ((c = *s2++) == *s1++ && c)
				;
			if (c == 0)
				return(offset);
			s1 = offset + as1 + 1;
			s2 = as2;
			c = *s2;
		}

	 return(-1);
}

#endif KJI
