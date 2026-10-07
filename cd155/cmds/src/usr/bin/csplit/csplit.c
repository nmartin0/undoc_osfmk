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
/*
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: csplit.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:21:52 $";
#endif
/*
static char sccsid[] = "@(#)csplit.c	1.10  com/cmd/files,3.1,9013 11/22/89 15:48:26";
*/
/*
 * COMPONENT_NAME: (CMDFILES) commands that manipulate files
 *
 * FUNCTIONS: csplit
 *
 * ORIGINS: 3,27
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
 */

#include <stdio.h>
#include <locale.h>
#include <signal.h>
#include <sys/dir.h>

#ifdef KJI
#include <NLchar.h>
#endif

#include <nl_types.h>
#include "csplit_msg.h"
nl_catd catd;
#define MSGSTR(Num,Str) catgets(catd,MS_CSPLIT,Num,Str)
/* The following macro returns true if the character is a valid
 * second byte of a two byte Kanji character.
*/
#define	valid_2nd(c) ((unsigned) c >= 0x40 && (unsigned) c <= 0xfc)

#define LAST	0L
#define ERR	-1
#define FALSE	0
#define TRUE	1
#define EXPMODE	2
#define LINMODE	3
#define EXPSIZ	(128*5)
#define	LINSIZ	256
#define MAXFLS	99

	/* Globals */

char linbuf[LINSIZ];		/* Input line buffer */
char expbuf[EXPSIZ];		/* Compiled expression buffer */
char file[PATH_MAX] = "xx";	/* File name buffer */
char *targ;			/* Arg ptr for error messages */
FILE *infile, *outfile;		/* I/O file streams */
int silent, keep, create;	/* Flags: -s(ilent), -k(eep), (create) */
long offset;			/* Regular expression offset value */
long curline;			/* Current line in input file */

/*
*	These defines are needed for regexp handling (see regexp(7))
*/
#define INIT		char *ptr = ++instring;
#define GETC()		(*(unsigned char *)ptr++)
#define PEEKC()		(*(unsigned char *)ptr)
#define UNGETC(c)	(--ptr)
#define RETURN(c)	return;
#define ERROR(c)	regerr(c);

#include <NLregexp.h>

/*
 * NAME: csplit [-s] [-k] [-f prefix] file arg1 [... argn]
 *                                                                    
 * FUNCTION: splits files by context
 *           -s         suppresses error messages
 *           -k         leaves created file segments intact in the event
 *                      of an error
 *           -f prefix  specifies the prefix name for the created
 *                      file segments  xx is the default prefix
 *	     -          use stdin as file argument
 */  
main(argc,argv)
int argc;
char **argv;
{
	int ch, mode, sig(void);
	char *f;
	short nofile = FALSE;

	(void ) setlocale(LC_ALL,"");
	catd = catopen(MF_CSPLIT,0);
	if(argc <= 2)
		fatal(MSGSTR(USAGE,"usage: csplit [-ks] [-f prefix] file args ...\n"),NULL); /*MSG*/

	while(**++argv == '-') {	/* Option handling */
		--argc;
		targ = *argv;
		while((ch = (int) *(++targ)) != (int) NULL)
			switch(ch) {
			case 'f':
				if (*(targ+1) != '\0') {
					if(strlen(targ+1) > NAME_MAX-1)
						fatal(MSGSTR(PRELONG,
						"csplit: prefix %s too long\n"),*argv);
					f = file;
					while (*(targ+1) != '\0')
						*f++ = *++targ;
					f='\0';
				}
				else {
					--argc;
					if(strlen(*++argv) > NAME_MAX-1)
						fatal(MSGSTR(PRELONG,
						"csplit: prefix %s too long\n"),*argv);
					strcpy(file,*argv);
				}
				break;
			case 's':
				silent++;
				break;
			case 'k':
				keep++;
				break;
			default:
				fatal(MSGSTR(USAGE,"usage: csplit [-ks] [-f prefix] file args ...\n")); /*MSG*/
			}
                if (*argv+1 == targ)
                        nofile = TRUE;

	}

        if (nofile) {
                infile = stdin;
        }
        else {
                if((infile = fopen(*argv,"r")) == NULL)
                        fatal(MSGSTR(CANTOPEN,"csplit: cannot open %s\n"),*argv); /*MSG*/
                --argc; ++argv;
        }
	curline = 1L;
	signal(SIGINT,(void (*)(int))sig);

	/*
	*	The following for loop handles the different argument types.
	*	A switch is performed on the first character of the argument
	*	and each case calls the appropriate argument handling routine.
	*/

	for(; *argv; ++argv) {
		targ = *argv;
		switch(**argv) {
		case '/':
			mode = EXPMODE;
			create = TRUE;
			re_arg(*argv);
			break;
		case '%':
			mode = EXPMODE;
			create = FALSE;
			re_arg(*argv);
			break;
		case '{':
			num_arg(*argv,mode);
			mode = FALSE;
			break;
		default:
			mode = LINMODE;
			create = TRUE;
			line_arg(*argv);
			break;
		}
	}
	create = TRUE;
	to_line(LAST);
}

/*
 * NAME: atol
 *                                                                    
 * FUNCTION:  Atol takes an ascii argument (str) and converts it to a 
 *            long (plc).  It returns ERR if an illegal character.  
 *            The reason that atol does not return an answer (long) is 
 *            that any value for the long is legal, and this version of 
 *            atol detects error strings.
 */

atol(str,plc)
char *str;
long *plc;
{
	int f;
	*plc = 0;
	f = 0;
	for(;;str++) {
		switch(*str) {
		case ' ':
		case '\t':
			continue;
		case '-':
			f++;
		case '+':
			str++;
		}
		break;
	}
	for(; *str != (char) NULL; str++)
		if(*str >= '0' && *str <= '9')
			*plc = *plc * 10 + *str - '0';
		else
			return(ERR);
	if(f)
		*plc = -(*plc);
	return(TRUE);	/* not error */
}

/*
 * NAME: closefile
 *                                                                    
 * FUNCTION:
 *	Closefile prints the byte count of the file created, (via fseek
 *	and ftell), if the create flag is on and the silent flag is not on.
 *	If the create flag is on closefile then closes the file (fclose).
 */

closefile()
{
	if(!silent && create) {
		fseek(outfile,0L,2);
		fprintf(stdout,"%ld\n",ftell(outfile));
	}
	if(create)
		fclose(outfile);
}

/*
 * NAME: fatal
 *                                                                    
 * FUNCTION: 
 *	Fatal handles error messages and cleanup.
 *	Because "arg" can be the global file, and the cleanup processing
 *	uses the global file, the error message is printed first.  If the
 *	"keep" flag is not set, fatal unlinks all created files.  If the
 *	"keep" flag is set, fatal closes the current file (if there is one).
 *	Fatal exits with a value of 1.
 */

fatal(string,arg)
char *string, *arg;
{
	char *fls;
	int num;

	fprintf(stderr,string,arg);
	if(!keep) {
		if(outfile) {
			fclose(outfile);
			for(fls=file; *fls != (char) NULL; fls++);
			fls -= 2;
			for(num=atoi(fls); num >= 0; num--) {
				sprintf(fls,"%.02d",num);
				unlink(file);
			}
		}
	} else
		if(outfile)
			closefile();
	exit(1);
}

/*
 * NAME: findline
 *                                                                    
 * FUNCTION:
 *	Findline returns the line number referenced by the current argument.
 *	Its arguments are a pointer to the compiled regular expression (expr),
 *	and an offset (oset).  The variable lncnt is used to count the number
 *	of lines searched.  First the current stream location is saved via
 *	ftell(), and getline is called so that R.E. searching starts at the
 *	line after the previously referenced line.  The while loop checks
 *	that there are more lines (error if none), bumps the line count, and
 *	checks for the R.E. on each line.  If the R.E. matches on one of the
 *	lines the old stream location is restored, and the line number
 *	referenced by the R.E. and the offset is returned.
 */

long findline(expr,oset)
char *expr;
long oset;
{
	static int benhere;
	long lncnt = 0, saveloc;
	char *getline();

	saveloc = ftell(infile);
	if(curline != 1L || benhere)		/* If first line, first time, */
		getline(FALSE);			/* then don't skip */
	else
		lncnt--;
	benhere = 1;
	while(getline(FALSE) != NULL) {
		lncnt++;
		if(step( linbuf, expr)) {
			fseek(infile,saveloc,0);
			return(curline+lncnt+oset);
		}
	}
	fseek(infile,saveloc,0);
	return(curline+lncnt+oset+2);
}

/*
 * NAME: flush
 *                                                                    
 * FUNCTION: 
 *	Flush uses fputs to put lines on the output file stream (outfile)
 *	Since fputs does its own buffering, flush doesn't need to.
 *	Flush does nothing if the create flag is not set.
 */

flush()
{
	if(create)
		fputs(linbuf,outfile);
}

/*
 * NAME: getfile
 *                                                                    
 * FUNCTION:
 *	Getfile does nothing if the create flag is not set.  If the
 *	create flag is set, getfile positions the file pointer (fptr) at
 *	the end of the file name prefix on the first call (fptr=0).
 *	Next the file counter (ctr) is tested for MAXFLS, fatal if too
 *	many file creations are attempted.  Then the file counter is
 *	stored in the file name and incremented.  If the subsequent
 *	fopen fails, the file name is copied to tfile for the error
 *	message, the previous file name is restored for cleanup, and
 *	fatal is called.  If the fopen succecedes, the stream (opfil)
 *	is returned.
 */

FILE *getfile()
{
	static char *fptr;
	static int ctr;
	FILE *opfil;
	char tfile[PATH_MAX];

	if(create) {
		if(fptr == 0)
			for(fptr = file; *fptr != (char) NULL; fptr++);
		if(ctr > MAXFLS)
			fatal(MSGSTR(FILELIM,"csplit: %d file limit reached at arg %s\n"),MAXFLS+1,targ); /*MSG*/
		sprintf(fptr,"%.02d",ctr++);
		if((opfil = fopen(file,"w")) == NULL) {
			strcpy(tfile,file);
			sprintf(fptr,"%.02d",(ctr-2));
			fatal(MSGSTR(CANTCREAT,"csplit: cannot create %s\n"),tfile); /*MSG*/
		}
		return(opfil);
	}
	return(NULL);
}

/*
 * NAME: getline
 *                                                                    
 * FUNCTION:
 *	Getline gets a line via fgets from the input stream "infile".
 *	The line is put into linbuf and may not be larger than LINSIZ.
 *	If getline is called with a non-zero value, the current line
 *	is bumped, otherwise it is not (for R.E. searching).
 */

char *getline(bumpcur)
int bumpcur;
{
	char *ret;
	if(bumpcur)
		curline++;
	ret=fgets(linbuf,LINSIZ,infile);
	return(ret);
}

/*
 * NAME: line_arg
 *                                                                    
 * FUNCTION:
 *	Line_arg handles line number arguments.
 *	line_arg takes as its argument a pointer to a character string
 *	(assumed to be a line number).  If that character string can be
 *	converted to a number (long), to_line is called with that number,
 *	otherwise error.
 */

line_arg(line)
char *line;
{
	long to;

	if(atol(line,&to) == ERR)
		fatal(MSGSTR(BADLNUM,"csplit: %s: bad line number\n"),line); /*MSG*/
	to_line(to);
}

/*
 * NAME: num_arg
 *                                                                    
 * FUNCTION: 
 *	Num_arg handles repeat arguments.
 *	Num_arg copies the numeric argument to "rep" (error if number is
 *	larger than 11 characters or } is left off).  Num_arg then converts
 *	the number and checks for validity.  Next num_arg checks the mode
 *	of the previous argument, and applys the argument the correct number
 *	of times. If the mode is not set properly its an error.
 */

num_arg(arg,md)
char *arg;
int md;
{
	long repeat, toline;
	char rep[12];
	char *ptr;

	ptr = rep;
	for(++arg; *arg != '}'; arg++) {
		if(ptr == &rep[11])
			fatal(MSGSTR(RPT2LNG,"csplit: %s: repeat count too large\n"),targ); /*MSG*/
		if(*arg == (char) NULL)
			fatal(MSGSTR(MISSBRKT,"csplit:%s: missing '}'\n"),targ); /*MSG*/
		*ptr++ = *arg;
	}
	*ptr = (char) NULL;
	if((atol(rep,&repeat) == ERR) || repeat < 0L)
		fatal(MSGSTR(ILLRPT,"csplit: illegal repeat count: %s\n"),targ); /*MSG*/
	if(md == LINMODE) {
		toline = offset = curline;
		for(;repeat > 0L; repeat--) {
			toline += offset;
			to_line(toline);
		}
	} else	if(md == EXPMODE)
			for(;repeat > 0L; repeat--)
				to_line(findline(expbuf,offset));
		else
			fatal(MSGSTR(NOOP,"csplit: no operation for %s\n"),targ); /*MSG*/
}

/*
 * NAME: re_arg
 *                                                                    
 * FUNCTION:
 *	Re_arg handles regular expression arguments.
 *	Re_arg takes a csplit regular expression argument.  It checks for
 *	delimiter balance, computes any offset, and compiles the regular
 *	expression.  Findline is called with the compiled expression and
 *	offset, and returns the corresponding line number, which is used
 *	as input to the to_line function.
 */

re_arg(string)
char *string;
{
	char *ptr;
	char ch;
	

#ifdef KJI

	ch = *string;
	ptr = string+1;
	while (*ptr != ch) {
		if(NCisshift(*ptr)) {
			++ptr;
			if(!valid_2nd(*ptr))
				fatal(MSGSTR(ILL2ND,"csplit: illegial 2nd byte: 0%o\n"),*ptr); /*MSG*/
			++ptr;
			continue;
		}
		if(*ptr == '\\')
			++ptr;
		if(*ptr == NULL)
			fatal(MSGSTR(MISSDEL,"csplit: %s: missing delimiter\n"),targ); /*MSG*/
		++ptr;
	}
#else /*KJI*/
	ch = *string;
	ptr = string;
	while(*(++ptr) != ch) {
		if(*ptr == '\\')
			++ptr;
		if(*ptr == (char)  NULL)
			fatal(MSGSTR(MISSDEL,"csplit: %s: missing delimiter\n"),targ); /*MSG*/
	}
#endif /*KJI*/
	if(atol(++ptr,&offset) == ERR)
		fatal(MSGSTR(ILLOFF,"csplit: %s: illegal offset\n"),string); /*MSG*/
	compile(string, expbuf, &expbuf[EXPSIZ], (int) ch);
	to_line(findline(expbuf,offset));
}

/*
 * NAME: sig
 *                                                                    
 * FUNCTION:
 *	Sig handles breaks.  When a break occurs the signal is reset,
 *	and fatal is called to clean up and print the argument which
 *	was being processed at the time the interrupt occured.
 */

sig(void)
{
	signal(SIGINT,(void (*)(int))sig);
	fatal(MSGSTR(INTSIG,"csplit: interrupt - program aborted at arg '%s'\n"),targ);	/*MSG*/
}

/*
 * NAME: to_line
 *                                                                    
 * FUNCTION: 
 *	To_line creates split files.
 *	To_line gets as its argument the line which the current argument
 *	referenced.  To_line calls getfile for a new output stream, which
 *	does nothing if create is False.  If to_line's argument is not LAST
 *	it checks that the current line is not greater than its argument.
 *	While the current line is less than the desired line to_line gets
 *	lines and flushes (error if EOF is reached).
 *	If to_line's argument is LAST, it checks for more lines, and gets
 *	and flushes lines till the end of file.
 *	Finally, to_line calls closefile to close the output stream.
 */

to_line(ln)
long ln;
{
	outfile = getfile();
	if(ln != LAST) {
		if(curline > ln)
			fatal(MSGSTR(OUTRNG,"csplit: %s - out of range\n"),targ); /*MSG*/
		while(curline < ln) {
			if(getline(TRUE) == NULL)
				fatal(MSGSTR(OUTRNG,"csplit: %s - out of range\n"),targ); /*MSG*/
			flush();
		}
	} else		/* last file */
		if(getline(TRUE) != NULL) {
			flush();
			while(TRUE) {
				if(getline(TRUE) == NULL)
					break;
				flush();
			}
		} else
			fatal(MSGSTR(OUTRNG,"csplit: %s - out of range\n"),targ); /*MSG*/
	closefile();
}

/*
 * NAME: regerr
 *                                                                    
 * FUNCTION:  REGEXP ERR ROUTINE
 */

regerr(c)
int c;
{
printf(MSGSTR(BOGUSERR,"csplit: %d This is the error code\n"),c); /*MSG*/
printf(MSGSTR(ILLRE,"csplit: illegal Regular Expression\n")); /*MSG*/
exit(1);
}
