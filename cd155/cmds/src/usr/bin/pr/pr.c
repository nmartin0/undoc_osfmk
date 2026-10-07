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
static char rcsid[] = "@(#)$RCSfile: pr.c,v $ $Revision: 1.4 $ (OSF) $Date: 1994/11/19 01:34:30 $";
#endif

/*
 * COMPONENT_NAME: (CMDFILES) commands that manipulate files
 *
 * FUNCTIONS: pr
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
 * pr.c	1.11  com/cmd/files,3.1,9021 4/26/90 18:46:40
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <locale.h>
#include <sys/types.h>
#include <NLchar.h>
#include <nl_types.h>
#include "pr_msg.h"
nl_catd catd;
#define MSGSTR(Num,Str) catgets(catd,MS_PR,Num,Str)

#define ESC	'\033'
#define LENGTH	66
#define LINEW	72
#define NUMW	5
#define MARGIN	10
#define DEFTAB	8

#define NCNEWLINE	(NCdechr("\n"))
#define NCTAB		(NCdechr("\t"))
#define NCSPACE		(NCdechr(" "))
#define NCFF		(NCdechr("\f"))
#define NCEOF		((NLchar) -1)

/*
 *	PR command (print files in pages and columns, with headings)
 *	2+head+2+page[56]+5
 */

FILE  *mustopen();
char nulls[] = "";
typedef struct { FILE *f_f; char *f_name; NLchar f_nextc; } FILS;
NLchar Etabc, Itabc, Nsepc;
FILS *Files;
int Multi = 0, Nfiles = 0, Error = 0, onintr(void);

#include <signal.h>
#include <NLctype.h>
#include <sys/types.h>
#include <sys/stat.h>
typedef char CHAR;
typedef int ANY;
typedef unsigned UNS;
#define NFILES	10
char obuf[BUFSIZ];
int Ttyout;
#define istty(F)	isatty((int)fileno(F))
#define done()		/* no cleanup */
#define INTREXIT	_exit
#define CADDID()
#define HEAD    "%s  %s %s %d\n\n\n", date, head, MSGSTR(PAGE, "Page"), Page
#define cerror(S)	fprintf(stderr, "pr: %s", S)
#define STDINNAME()	nulls
#define TTY	"/dev/tty", "r"
#define PROMPT()	putc('\7', stderr) /* BEL */
#define NOSFILE	nulls
#define TABS(N,C)	if ((N = intopt(argv, &C)) < 0) N = DEFTAB
#define ETABS	(Inpos % Etabn)
#define ITABS	(Itabn > 0 && Nspace >= (nc = Itabn - Outpos % Itabn))
#define NSEPC	NCTAB

ANY *getspace();
long Lnumb = 0;
FILE *Ttyin = stdin;
int Dblspace = 1, Fpage = 1, Formfeed = 0,
	Length = LENGTH, Linew = 0, Offset = 0, Ncols = 1, Pause = 0, Sepc = 0,
	Colw, Plength, Margin = MARGIN, Numw, Report = 1,
	Etabn = 0, Itabn = 0;
char *Head = NULL;
CHAR *Buffer = NULL, *Bufend;
typedef struct { CHAR *c_ptr, *c_ptr0; long c_lno; } *COLP;
COLP Colpts;

int Page, Nspace, Inpos;
NLchar __C = '\0';

int Outpos, Lcolpos, Pcolpos, Line;

#define EMPTY	14	/* length of " -- empty file" */
typedef struct err { struct err *e_nextp; char *e_mess; } ERR;
ERR *Err = NULL, *Lasterr = (ERR *)&Err;

/*
 * NAME: fixtty
 *                                                                    
 * FUNCTION: set up a buffer for the tty
 */  
/* ARGSUSED */
fixtty(argc, argv) char **argv;
{
	setbuf(stdout, obuf);
	if (signal(SIGINT, SIG_IGN) != SIG_IGN) signal(SIGINT, (void (*)(int))onintr);
	Ttyout= istty(stdout);
	return (argc);
}

/*
 * NAME: GETDATE
 *                                                                    
 * FUNCTION:  return date file was last modified
 */  
char *GETDATE()
{
	static char *now = NULL;
	static struct stat sbuf, nbuf;

	if (Nfiles > 1 || Files->f_name == nulls) {
		if (now == NULL) {
			time(&nbuf.st_mtime);
			now = (char *)NLctime(&nbuf.st_mtime);
		}
		return (now);
	} else {
		stat(Files->f_name, &sbuf);
		return ((char *)NLctime(&sbuf.st_mtime));
	}
}

/*
 * NAME: ffiler
 *                                                                    
 * FUNCTION: make error message "can't open 'file'"
 */  
char *ffiler(s) char *s;
{
	static char buf[100];

	sprintf(buf,"can't open %s", s);
	return (buf);
}

/*
 * NAME: pr [options] [files]
 *                                                                    
 * FUNCTION: Writes a file to standard output.  If a file is not specified
 *      or a '-' is for the file name then standard input is read.
 *      OPTIONS:
 *      -a        Displays multi-column output
 *      -d        Double-spaces the output
 *      -e[c][n]  Expands tabs to n+1, 2*n+1, 3*n+1 etc. Default is 8
 *                If a character c is specified then that character becomes
 *                the input tab character.
 *      -f        Uses a form-feed character to advance to a new page.
 *      -h "string"   Displays string as the page header instead of the file
 *      -i[c][n]  replaces white spaces with tabs at n+1, 2*n+1, 3*n+1 etc. 
 *                Default is 8.  If a character c is specified then that 
 *                character becomes the output tab  character.
 *      -lnum     Sets the length of a page to num
 *      -m        combines and writes all files at the same time, with each
 *                in a separate column (overrides -num and -a flags).
 *      -n[c][n]  number of digits used for numbering lines c is added to
 *                the line.
 *      -onum     Indents each line by num spaces.
 *      -p        pauses before beginning each page.
 *      -r        suppresses diagnostic messages if the system can't open files
 *      -schar    separates columns by the single character char instead 
 *                of spaces
 *      -t        does not display header and footer
 *      -wnum     sets the width of a line
 *      -num      produce num-column  output
 *      +num      begin the display with page num (default is 1)
 */  
main(argc, argv)
int argc;
char *argv[];
{
	FILS fstr[NFILES];
	int nfdone = 0;

	(void) setlocale(LC_ALL,"");
	catd = catopen(MF_PR,0);
	Nsepc = Itabc = Etabc = NCTAB;
	Files = fstr;
	for (argc = findopt(argc, argv); argc > 0; --argc, ++argv)
		if (Multi == 'm') {
			if (Nfiles >= NFILES - 1) die(MSGSTR(TOOMANYF,"too many files")); /*MSG*/
			if (mustopen(*argv, &Files[Nfiles++]) == NULL)
				++nfdone; /* suppress printing */
		} else {
			if (print(*argv))
				fclose(Files->f_f);
			++nfdone;
		}
	if (!nfdone) /* no files named, use stdin */
		print(NOSFILE); /* on GCOS, use current file, if any */
	errprint(); /* print accumulated error reports */
	exit(Error);
/* NOTREACHED */
}

/*
 * NAME: findopt
 *                                                                    
 * FUNCTION: get options and set flag
 */  
findopt(argc, argv) char *argv[];
{
	char **eargv = argv;
	int eargc = 0, c;
	char asc_sep[2];

	argc = fixtty(argc, argv);
	while (--argc > 0) {
		switch (c = **++argv) {
		case '-':
			if ((c = *++*argv) == '\0') break;
		case '+':
			do {
				if (isdigit(c))
					{ --*argv; Ncols = atoix(argv); }
				else switch (c = tolower(c)) {
				case '+': if ((Fpage = atoix(argv)) < 1)
						Fpage = 1;
					continue;
				case 'd': Dblspace = 2; continue;
				case 'e': TABS(Etabn, Etabc); continue;
				case 'f': ++Formfeed; continue;
				case 'h': if (--argc > 0) Head = argv[1];
					continue;
				case 'i': TABS(Itabn, Itabc); continue;
				case 'l': Length = atoix(argv); continue;
				case 'a':
				case 'm': Multi = c; continue;
				case 'o': Offset = atoix(argv); continue;
				case 'p': ++Pause; continue;
				case 'r': Report = 0; continue;
				case 's':
					if ((asc_sep[0] = (*argv)[1]) != '\0') {
						++*argv;
						if (NCisshift(asc_sep[0])) {
							asc_sep[1] = (*argv)[1];
							++*argv;
						}
						Sepc = NCdechr(asc_sep);
					}
					else Sepc = NCTAB;
					continue;
				case 't': Margin = 0; continue;
				case 'w': Linew = atoix(argv); continue;
				case 'n':
				case 'x': /* retained for historical reasons */
					++Lnumb;
					if ((Numw = intopt(argv, &Nsepc)) <= 0)
						Numw = NUMW;
				case 'b': /* retained for historical reasons */
				case 'q': /* retained for historical reasons */
				case 'j': /* ignore GCOS jprint option */
					continue;
				default : die(MSGSTR(BADOPT,"bad option")); /*MSG*/
				}
			} while ((c = *++*argv) != '\0');
			if (Head == argv[1]) ++argv;
			continue;
		}
		*eargv++ = *argv;
		++eargc;
	}
	if (Length == 0) Length = LENGTH;
	if (Length <= Margin) Margin = 0;
	Plength = Length - Margin/2;
	if (Multi == 'm') Ncols = eargc;
	switch (Ncols) {
	case 0:
		Ncols = 1;
	case 1:
		break;
	default:
		if (Etabn == 0) /* respect explicit tab specification */
			Etabn = DEFTAB;
		if (Itabn == 0)
			Itabn = DEFTAB;
	}
	if (Linew == 0) Linew = Ncols != 1 && Sepc == 0 ? LINEW : 512;
	if (Lnumb) {
		int numw;

		if (Nsepc == NCTAB) {
			if(Itabn == 0)
					numw = Numw + DEFTAB - (Numw % DEFTAB);
			else
					numw = Numw + Itabn - (Numw % Itabn);
		}else {
				numw = Numw + ((NCisprint(Nsepc)) ? 1 : 0);
		}
		Linew -= (Multi == 'm') ? numw : numw * Ncols;
	}
	if ((Colw = (Linew - Ncols + 1)/Ncols) < 1)
		die(MSGSTR(WDTHSMALL,"width too small")); /*MSG*/
	if (Ncols != 1 && Multi == 0) {
		UNS buflen = ((UNS)(Plength/Dblspace + 1))*2*(Linew+1)*sizeof(CHAR);
		Buffer = (CHAR *)getspace(buflen);
		Bufend = &Buffer[buflen];
		Colpts = (COLP)getspace((UNS)((Ncols+1)*sizeof(*Colpts)));
	}
	if (Ttyout && (Pause || Formfeed) && !istty(stdin))
		Ttyin = fopen(TTY);
	return (eargc);
}

/*
 * NAME: intopt
 *                                                                    
 * FUNCTION: get num and char and get the ascii values
 */  
intopt(argv, optp)
char *argv[];
NLchar *optp;
{
	char c;
	char asc_val[2];

	if ((c = (*argv)[1]) != '\0' && !NCisdigit(c)) {
		asc_val[0] = c;
		++*argv;
		if (NCisshift(c)) {
			asc_val[1] = (*argv)[1];
			++*argv;
		}
		*optp = NCdechr(asc_val);
	}
	return ((c = atoix(argv)) != 0 ? c : -1);
}


/*
 * NAME: print
 *                                                                    
 * FUNCTION: print header for next page
 */  
print(name) char *name;
{
	static int notfirst = 0;
	char *date = NULL, *head = NULL;
        char *npoint = NULL;
	char asc_ver[2];
	short len;
	int c;

	if (Multi != 'm' || Nfiles == 0)
		if (mustopen(name, &Files[Nfiles]) == NULL)
			return(0);
	len = NCencode(&Files->f_nextc, asc_ver);
	if (Buffer) {
		if (len == 2)
			ungetc((int)asc_ver[1], Files->f_f);
		ungetc((int)asc_ver[0], Files->f_f);
	}
	if (Lnumb) Lnumb = 1;
	for (Page = 0; ; putpage()) {
		if (__C == NCEOF) break;
		if (Buffer) {
			 if (nexbuf() == 1)
				return(1);
		}
		Inpos = 0;
		if (get(0) == NCEOF) break;
		fflush(stdout);
		if (++Page >= Fpage) {
			if (Ttyout && (Pause || Formfeed && !notfirst++)) {
				PROMPT(); /* prompt with bell and pause */
				while ((c = getc(Ttyin)) != EOF && c != '\n') ;
			}
			if (Margin == 0) continue;
			CADDID();
                        if (date == NULL)
                        {
                            date = GETDATE();
                            npoint = strpbrk(date,"\n");
                            *npoint = '\0';  /* get rid of return char */
                        }
			if (head == NULL) head = Head != NULL ? Head :
				Nfiles < 2 ? Files->f_name : nulls;
			printf("\n\n");
			Nspace = Offset;
			putspace();
			printf(HEAD);
		}
	}
	__C = '\0';
	return (1);
}


/*
 * NAME: putpage
 *                                                                    
 * FUNCTION: print out the current page
 */  
putpage()
{
	int colno;

	for (Line = Margin/2; ; get(0)) {
		for (Nspace = Offset, colno = 0, Outpos = 0; __C != NCFF; ) {
			if (Lnumb && __C != NCEOF && ((colno == 0 && Multi == 'm') || Multi != 'm')) {
				if (Page >= Fpage) {
					putspace();
					printf("%*ld%c", Numw, Buffer ?
						Colpts[colno].c_lno++ : Lnumb, Nsepc);
				}
				++Lnumb;
			}
			for (Lcolpos = 0, Pcolpos = 0;
				__C != NCNEWLINE && __C != NCFF && __C != NCEOF; get(colno))
					put(__C);
			if (__C == NCEOF || ++colno == Ncols ||
				__C == NCNEWLINE && get(colno) == NCEOF) break;
			if (Sepc) put(Sepc);
			else if ((Nspace += Colw - Lcolpos + 1) < 1) Nspace = 1;
		}
		if (__C == NCEOF) {
			if (Margin != 0) break;
			if (colno != 0) put(NCNEWLINE);
			return;
		}
		if (__C == NCFF) break;
		put(NCNEWLINE);
		if (Dblspace == 2 && Line < Plength) put(NCNEWLINE);
		if (Line >= Plength) break;
	}
	if (Formfeed) put(NCFF);
	else while (Line < Length) put(NCNEWLINE);
}

/*
 * NAME: nexbuf
 *                                                                    
 * FUNCTION: build next line
 */  
nexbuf()
{
	CHAR *s = Buffer;
	COLP p = Colpts;
	int j, bline = 0;
	char inchar[2];
	NLchar c;
	short len;

	for ( ; ; ) {
		p->c_ptr0 = p->c_ptr = s;
		if (p == &Colpts[Ncols]) return (0);
		(p++)->c_lno = Lnumb + bline;
		for (j = (Length - Margin)/Dblspace; --j >= 0; ++bline)
			for (Inpos = 0; ; ) {
				inchar[0] = getc(Files->f_f);
				if (NCisshift(inchar[0]))
					inchar[1] = getc(Files->f_f);
				c = NCdechr(inchar);
				if (feof(Files->f_f)) {
					for (*s = EOF; p <= &Colpts[Ncols]; ++p)
						p->c_ptr0 = p->c_ptr = s;
					balance(bline);
					return (0);
				}
				if (NCisprint(c)) ++Inpos;
				if (Inpos <= Colw || c == NCNEWLINE) {
					len = NCencode(&c, s);
					if (len == 2)
						++s;
					if (++s >= Bufend) 
						return (1);
				}
				if (c == NCNEWLINE) break;
				if (!NCisshift(inchar[0])) 
					switch (inchar[0]) {
					case '\b': if (Inpos == 0) --s;
					case ESC:  if (Inpos > 0) --Inpos;
					}
			}
	}
        return (0);
}

/*
 * NAME: balance
 *                                                                    
 * FUNCTION: line balancing for last page
 */  
balance(bline) 
{
	CHAR *s = Buffer;
	COLP p = Colpts;
	int colno = 0, j, c, l;

	c = bline % Ncols;
	l = (bline + Ncols - 1)/Ncols;
	bline = 0;
	do {
		for (j = 0; j < l; ++j)
			while (*s++ != '\n') ;
		(++p)->c_lno = Lnumb + (bline += l);
		p->c_ptr0 = p->c_ptr = s;
		if (++colno == c) --l;
	} while (colno < Ncols - 1);
}

/*
 * NAME: get
 *                                                                    
 * FUNCTION: build next column
 */  
/* The strategy for the Kanji modifications is very simple...
 * Merely look to see that we aren't at the end of the column width.
 * If we are, and a two byte Kanji character won't fit in the width,
 * then simply return a space to fill up the room.  Note that we
 * don't have to keep the old character that we just replaced with
 * a space because when it gets to that deteriorated stage, we
 * are never going to output anything real from this column, anyway.
 * Note also that we keep track of the Kanji character state with
 * the static variable "inkji" so that we don't erroneously truncate
 * the second byte of a Kanji character.
 */
get(colno)
{
	static int peekc = 0;
	COLP p;
	FILS *q;
	NLchar c;
	char asc_ver[2];
	short len;

	if (peekc)
		{ peekc = 0; c = Etabc; }
	else if (Buffer) {
		p = &Colpts[colno];
		if (p->c_ptr >= (p+1)->c_ptr0) c = EOF;
		else {
			if ((asc_ver[0] = *p->c_ptr) != (char) EOF)
				p->c_ptr++;
			if (NCisshift(asc_ver[0]))
				asc_ver[1] = *(p->c_ptr++);
			c = NCdechr (asc_ver);
		}
	} else if ((c = 
		(q = &Files[Multi == 'a' ? 0 : colno])->f_nextc) == NCEOF) {
		for (q = &Files[Nfiles];--q >= Files && q->f_nextc == NCEOF; ) ;
		if (q >= Files) c = NCNEWLINE;
	} else
#ifdef KJI
		q->f_nextc = getwc(q->f_f);
#else
		q->f_nextc = getc(q->f_f);
#endif
	if (Etabn != 0 && c == Etabc) {
		++Inpos;
		peekc = ETABS;
		c = NCSPACE;
	} else if (c != NCEOF && NCisprint(c))
		++Inpos;
	else {
		if (c != NCEOF) {
			len = NCencode(&c, asc_ver);
			if (len == 1)
				switch (asc_ver[0]) {
				case '\b':
				case ESC:
					if (Inpos > 0) --Inpos;
					break;
				case '\f':
					if (Ncols == 1) break;
					c = NCNEWLINE;
				case '\n':
				case '\r':
					Inpos = 0;
				}
		}
	}
	return (__C = c);
}

/*
 * NAME: put
 *                                                                    
 * FUNCTION: put out char c into the buffer, but check for special charcters
 *           first.
 */  
put(c)
NLchar c;
{
	int move;
	char outchar[2];
	short len;

	len = NCencode(&c, outchar);
	if (len == 1)
		switch (outchar[0]) {
		case ' ':
			if(Ncols < 2 || Lcolpos < Colw) {
				++Nspace;
				++Lcolpos;
			}
			return;
		case '\t':
			if(Itabn == 0) {
				move = DEFTAB - (Lcolpos % DEFTAB);
				break;
			}
			if(Lcolpos < Colw) {
				move = Itabn - ((Lcolpos + Itabn) % Itabn);
				move = (move < Colw-Lcolpos) ? move : Colw-Lcolpos;
				Nspace += move;
				Lcolpos += move;
			}
			return;
		case '\b':
			if (Lcolpos == 0) return;
			if (Nspace > 0) { --Nspace; --Lcolpos; return; }
			if (Lcolpos > Pcolpos) { --Lcolpos; return; }
		case ESC:
			move = -1;
			break;
		case '\n':
			++Line;
		case '\r':
		case '\f':
			Pcolpos = 0; Lcolpos = 0; Nspace = 0; Outpos = 0;
		default:
#ifdef KJI
			move = NCisprint(c) ? NCchrlen(c) : 0;
#else
			move = (NCisprint(c) != 0);
#endif
		}
	else
#ifdef KJI
		move = NCisprint(c) ? NCchrlen(c) : 0;
#else
		move = (NCisprint(c) != 0);
#endif
	if (Page < Fpage) return;
	if (Lcolpos > 0 || move > 0) Lcolpos += move;
	putspace();
#ifdef KJI
	if (len == 1) {
		if ((Ncols<2 && Outpos < Linew) || (Lcolpos <= Colw)) {
			putchar(outchar[0]);
			Outpos += move;
			Pcolpos = Lcolpos;
		}
	} else {
		if ((Ncols<2 && Outpos == Linew-1) || (Lcolpos == Colw+1)) {
			putchar(' ');
			Outpos++;
			Pcolpos = Lcolpos-1;
		} else {
			if ((Ncols<2 && Outpos<Linew-1) || (Lcolpos <= Colw)) {
				putchar(outchar[0]);
				putchar(outchar[1]);
				Outpos += move;
				Pcolpos = Lcolpos;
			}
		}
	}

#else
	if ((Ncols<2 && Outpos < Linew) || Lcolpos <= Colw) {
		putchar(outchar[0]);
		if (len == 2)
			putchar(outchar[1]);
		Outpos += move;
		Pcolpos = Lcolpos;
	}
#endif
}

/*
 * NAME: putspace
 *                                                                    
 * FUNCTION: put out white space (tab or space or specified char)
 */  
putspace()
{
	int nc;

	for ( ; Nspace > 0; Outpos += nc, Nspace -= nc)
		if (ITABS) {
			char asc_val[2];
			short len;
			len = NCencode(&Itabc, asc_val);
			putchar (asc_val[0]);
			if (len == 2)
				putchar(asc_val[1]);
		}
		else {
			nc = 1;
			putchar(' ');
		}
}

/*
 * NAME: atoix
 *                                                                    
 * FUNCTION: convert character string of digits to a integer
 */  
atoix(p) char **p;
{
	int n = 0, c;

	while (isdigit(c = *++*p)) n = 10*n + c - '0';
	--*p;
	return (n);
}

/*
 * NAME: mustopen
 *                                                                    
 * FUNCTION: open a file
 */  
/* Defer message about failure to open file to prevent messing up
   alignment of page with tear perforations or form markers.
   Treat empty file as special case and report as diagnostic.
*/

FILE *mustopen(s, f) char *s; FILS *f;
{
	char *empty;
	if (*s == '\0') {
		f->f_name = STDINNAME();
		f->f_f = stdin;
	} else if ((f->f_f = fopen(f->f_name = s, "r")) == NULL) {
		s = ffiler(f->f_name);
		s = (char *) strcpy((char *)getspace((UNS)(strlen(s) + 1)), s);
	}
	if (f->f_f != NULL) {
#ifdef KJI
		if ((f->f_nextc = getwc(f->f_f)) != (char) EOF || Multi == 'm')
#else
		if ((f->f_nextc = getc(f->f_f)) != NCEOF || Multi == 'm')
#endif /* KJI */
			return (f->f_f);
		empty = MSGSTR(EMPTYF,"%s -- empty file"); /*MSG*/
		sprintf(s = (char *)getspace((UNS)(strlen(f->f_name) + 1 + strlen(empty))), empty, f->f_name); /*MSG*/
/*		sprintf(s = (char *)getspace((UNS)(strlen(f->f_name) + 1 + EMPTY)),
			"%s -- empty file", f->f_name);
*/
		fclose(f->f_f);
	}
	Error = 1;
	if (Report)
		if (Ttyout) { /* accumulate error reports */
			Lasterr = Lasterr->e_nextp = (ERR *)getspace((UNS)sizeof(ERR));
			Lasterr->e_nextp = NULL;
			Lasterr->e_mess = s;
		} else { /* ok to print error report now */
			cerror(s);
			putc('\n', stderr);
		}
	return ((FILE *)NULL);
}

/*
 * NAME: getspace
 *                                                                    
 * FUNCTION: get more space from memory
 */  
ANY *getspace(n) UNS n;
{
	ANY *t;

	if ((t = (ANY *)malloc((size_t)n)) == NULL) die(MSGSTR(NOCORE,"out of space"));	/*MSG*/
	return (t);
}

/*
 * NAME: die
 *                                                                    
 * FUNCTION: print error messages and exit
 */  
die(s) char *s;
{
	++Error;
	errprint();
	cerror(s);
	putc('\n', stderr);
	exit(1);
}

/*
 * NAME: onintr
 *                                                                    
 * FUNCTION: on interrupt print error messages and exit
 */  
onintr(void)
{
	++Error;
	errprint();
	INTREXIT(1);
}

/*
 * NAME: errprint
 *                                                                    
 * FUNCTION:  print accumulated error reports 
 */  
errprint() 
{
	fflush(stdout);
	for ( ; Err != NULL; Err = Err->e_nextp) {
		cerror(Err->e_mess);
		putc('\n', stderr);
	}
	done();
}
