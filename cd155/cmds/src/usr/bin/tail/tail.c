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
static char rcsid[] = "@(#)$RCSfile: tail.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 01:40:42 $";
#endif
/*
 * COMPONENT_NAME: (CMDSCAN) commands that scan files
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 26, 27
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
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * Copyright 1976, Bell Telephone Laboratories, Inc.
 * tail.c	1.8  com/cmd/scan,3.1,9008 10/20/89 11:56:15";
 */

/*
 *
 *	Tail writes from a file beginning at a specified point to the
 *	end of file.  Points can be specified by:
 *
 *		+/- [n]l	for n lines
 *		+/- [n]b	for n blocks
 *		+/- [n]c	for n characters
 *		+/- [n]k	for n k-block (1024)
 *		+/- [n]v	for n multi-byte (NL) characters
 *
 *	The -f option tell you to loop forever looking for updates.  The
 *	assumption is the file is growing.
 *
 *	The -r option will copy output from end of file to specified point
 *	in reverse order.
 *                                                                    
 *	The -n option will copy the last <number> lines of the file
 *	to standard output.
 *
 *	The -c option will copy the last <number> bytes of the file
 *	to standard output.
 */                                                                   

#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <locale.h>
#ifdef KJI
#include <NLchar.h>
#endif
#include "tail_msg.h"
nl_catd catd;
#define MSGSTR(Num, Str) catgets(catd,MS_TAIL,Num, Str)

#define BLOCK   512     /* multiplier for b suffix */
#define KBLOCK  1024    /* multiplier for k suffix */
#define CHUNK   4096    /* how much we are willing to read at once */

#define	BSDSZ	32769	/* Berzerkely buffer size  */
#define	NEWLINE	'\n'

typedef int     bool;
#define NO      0	
#define YES     1

#include <stdlib.h>
extern void setcount(), dofollow(), dotail(), dochars(), dolines();
extern void dorev();

#ifdef KJI
void skipwidechars();	/* for tail +k */
void dowidechars();	/* for tail -k */
void doring();		/* for tail -k, no seek */
void setupring();	/* used by doring() */
bool charbdry();	/* synchronize on a character boundary */
char	mflag	  = NO;	/* multi-byte option       */
char	foundsync = NO;
#endif
static bool explicit = NO;	/* kludge to allow Berkeley compatibility */
				/* was n_tail set explicitly? */

bool    canseek;
bool    bylines = YES;
bool    follow  = NO;
bool    fromtop = NO;
bool	reverse = NO;
bool	fndunit = NO;
char   *progname;
long    n_tail  = 10L;
struct stat     statb;
char    bin[CHUNK];

main(argc, argv)
int argc; char **argv;
{
    (void) setlocale(LC_ALL,"");
    progname = argv[0];
    catd = NLcatopen(MF_TAIL,0);
    while (--argc) {
	register char *arg = *++argv;
	switch (*arg) {
	case '+':                               /* always a number */
	    fromtop = YES;
	    fndunit = YES;
	    setcount(arg);
	    break;
	case '-':                               /* may be number or "-f" */
	    if (arg[1] == 'f') {
		follow = 1;
	    } else if (arg[1] == 'r') {		/* ... or even -r */
		reverse = YES;
		if (!explicit) n_tail = BSDSZ;
	    } else if (arg[1] == 'n') {
		fromtop = NO;
		fndunit = NO;
		bylines = 1;
		if (arg[2])			/* number in same argument */
		    arg++;
		else {
		    arg = *++argv - 1;
		    argc--;
		}
		setcount(arg);
	    } else if (arg[1] == 'c') {
		fromtop = NO;
		fndunit = NO;
		bylines = 0;
		if (arg[2])			/* number in same argument */
		    arg++;
		else {
		    arg = *++argv - 1;
		    argc--;
		}
		setcount(arg);
	    } else {
		fromtop = NO;
		fndunit = YES;
		setcount(arg);
	    }
	    break;
	default:                                /* its a filename */
	    (void)close(0);
	    if (open(arg, 0) != 0) {
		perror(arg);
		exit(1);
	    }
	    break;
	}
    }
    fstat(0, &statb);
    canseek = (lseek(0, 0L, SEEK_CUR) != -1 && (statb.st_mode & S_IFMT) != S_IFCHR);
    if (reverse)
	dorev();
    else {
    	dotail();
    	if (follow && canseek) dofollow();
    }
    exit(0);
/* NOTREACHED */
}

/*
 * NAME: setcount
 *                                                                    
 * FUNCTION:  	Determine the count (+/-) and by what value to do the tail.
 *		n_tail is set to this value.  by_lines for lines else
 *		it is by character.
 *		Also, determine count for -n and -c options (fndunit==NO).
 *                                                                    
 * RETURN VALUE: void
 *			    
 */  

void setcount(arg) char *arg; {

    char *Usage = 
#ifdef KJI
	    "Usage: %s [+/-[number][lmbck] | -c number | -n number ] [-f | -r]  [filename]\n";
#else
	    "Usage: %s [+/-[number][lbck] | -c number | -n number ] [-f | -r] [filename]\n";
#endif

    arg++;
    if (isdigit((int)*arg)) {
	explicit = YES;
	n_tail = 0;
	do n_tail = n_tail * 10 + (*arg++ - '0'); while (isdigit((int)*arg));
	if (fromtop && n_tail > 0) --n_tail;
    }
    if (fndunit == YES) {
	while (*arg) {
	    switch (*arg++) {
	    case 'l':               /* lines */
		bylines = 1;
		break;
	    case 'b':
		bylines = 0;        /* blocks - convert to characters */
		n_tail *= BLOCK;
		break;
#ifdef KJI
	    case 'm':		/* tail the exact number of characters */
		mflag = 1;		/* instead of bytes */
		bylines = 0;	
		break;
#endif
	    case 'k':
		bylines = 0;        /* blocks - convert to characters */
		n_tail *= KBLOCK;
		break;
	    case 'c':               /* characters */
		bylines = 0;
		break;
	    case 'f':
		follow = 1;         /* "-10f" same as "-10 -f" */
		break;
	    case 'r':
		reverse = 1;	/* "-10r" same as "-10 -r" */
		if (!explicit)	/* kludge to allow Berkeley compatibility */
		    n_tail = BSDSZ;
		break;
	    default:
		fprintf(stderr, MSGSTR(USAGE, Usage), progname); /*MSG*/ 
		exit(2);
	    }
	}
    } else if (*arg != 0) {
	fprintf(stderr, MSGSTR(USAGE, Usage), progname); /*MSG*/
	exit(2);
    }
}

/*
 * NAME: dofollow
 *                                                                    
 * FUNCTION: every second write out any changes
 *                                                                    
 * RETURN VALUE: never returns.  Must be killed.
 */  

void dofollow() {
    register int n;
    while ((n = read(0, bin, (unsigned)CHUNK)) != -1) {
	if (n == 0) {
	    sleep((unsigned)1);
	} else {
	    write(1, bin, (unsigned)n);
	}
    }
}

/*
 * NAME: dotail
 *                                                                    
 * FUNCTION: 	Actually do the tail.  lseek to the starting point.
 *		skip NL characters if necessary, and print out the
 *		requested lines.
 *                                                                    
 * RETURN VALUE: none
 *			    
 */  

void dotail() {
    register int nc = 0;
#ifndef KJI /* might have to take the address of p! */
    register 
#endif
	char *p = bin;

    if (fromtop) {
	if (n_tail == 0) {
	    /* nothing */
#ifdef KJI
	} else if (mflag) {
	    skipwidechars();
#endif
	} else if (bylines) {
	    do {
		if (nc == 0) {
		    if ((nc = read(0, bin, (unsigned)CHUNK)) <= 0) return;
		    p = bin;
		}
		--nc;
	    } while (*p++ != '\n' || --n_tail > 0);
	} else if (canseek) { 
#ifdef KJI
	    /* read CHUNK/2 extra bytes and find a sync point. */
	    if ( n_tail > CHUNK/2 ) { /* not close to the beginning  */
		p = bin + (CHUNK/2);
		lseek(0, n_tail-(CHUNK/2), SEEK_CUR); 
	    } else {
		p = bin + n_tail;
		foundsync = 1; /* the beginning of the file is a sync point */
	    }
	    if ((nc = read(0, bin, (unsigned)CHUNK)) <= 0) return;
#else
	    lseek(0,n_tail,SEEK_CUR);
#endif
	} else {
#ifdef KJI
	    do {
		if ((nc = read(0, bin, (unsigned)CHUNK)) <= 0) return;
	    } while ((n_tail -= nc ) > CHUNK/2);
	    if ( n_tail > 0 ) p = bin + n_tail;
	    else p = bin + nc + n_tail;
#else
	    do {
		if ((nc = read(0, bin, (unsigned)CHUNK)) <= 0) return;
	    } while ((n_tail -= nc) > 0); 
	    p = bin + nc + n_tail; 
	    nc = -n_tail;
#endif
	}
#ifdef KJI
	if (! bylines ) { /* bylines code has already assured synchronization */
	    foundsync += charbdry(bin,&p); /* p now points to a character */
	    while ( ! foundsync ) { 
	        if ( ! NCisshift(*p) ) 
		    foundsync = 1;
	        if ( ++p > bin + nc ) /* ran out of buffer */
		    if (( nc = read(0,(p = bin), (unsigned)CHUNK)) <= 0) 
		        return; 
	    }
	    nc -= p - bin; 
	}
#endif
	if (nc > 0)  write(1, p, (unsigned)nc);
	while ((nc = read(0, bin, (unsigned)CHUNK)) > 0) write(1, bin, (unsigned)nc);

/* from here down are the -number cases */
    } else if (bylines) {	
	dolines();
#ifdef KJI
    } else if (mflag) {
	dowidechars();
#endif
    } else if (canseek) { 
#ifdef KJI
	if ( (n_tail + CHUNK/2) > statb.st_size ) 
		foundsync=1; /* beginning of file is a sync point */
	else
		lseek(0, -(n_tail+CHUNK/2), SEEK_END);
	if (( nc = read(0, bin, (unsigned)CHUNK)) <= 0 ) return; /* nothing there */
	if ( nc > n_tail ) {
		p = bin + nc - n_tail;
		foundsync += charbdry(bin, &p);  
	} else {
		foundsync++;
	}
	while ( ! foundsync ) { 
	    if ( ! NCisshift(*p) ) /* next byte is a sync point */
		foundsync = 1;
	    if ( ++p > bin + nc ) /* ran out of buffer */
		if (( nc = read(0,(p = bin), (unsigned)CHUNK)) <= 0) 
		    return;
	}
	nc -= p - bin;
	if (nc > 0) write(1, p, (unsigned)nc);
#else
	lseek(0, -n_tail, SEEK_END);
#endif
	while ((nc = read(0, bin, (unsigned)CHUNK)) > 0) write(1, bin, (unsigned)nc);
    } else 
	    dochars(); 
}

/*
 * NAME: dorev
 *                                                                    
 * FUNCTION:   - output last n_tail lines in reverse order (last first)
 * This function faithfully emulates the Berzerkely -r operation of tail.
 * Including limitations.
 *                                                                    
 * RETURN VALUE:  none
 */  


void dorev()
{
	static char buf[BSDSZ];		/* circular buffer storage area	*/

	register char *p;		/* current location in buffer	*/
	register char *bufend;		/* circular buffer end		*/

	char *last;			/* location of last newline	*/
	int partial;			/* is the buffer full or partial*/
	int lines;			/* number of lines output	*/

	int nc;				/* number of characters read	*/
	int offset = 0;			/* offset into buffer		*/
	long size = BSDSZ;		/* size of file/size of buffer	*/


	if (n_tail <= 0) return;

/*
 * read the file, saving the last BSDSZ bytes; handle wrapping
 */

	/* if input is seekable, win on large input files */
	if (canseek) {
		size = lseek(0, 0L, SEEK_END);
		if (size > BSDSZ) size = BSDSZ;
		lseek(0, -size, SEEK_CUR);
	}

	partial = YES;
	while ((nc = read(0, buf + offset, (unsigned)(BSDSZ - offset))) > 0) {
		if ((offset += nc) >= BSDSZ) {
			partial = NO;
			offset = 0;
		}
	}

	/* force a trailing newline (also ensures buffer is never empty) */
	if (buf[offset==0 ? BSDSZ-1 : offset-1] != NEWLINE) {
		buf[offset] = NEWLINE;
		if (++offset >= BSDSZ) {
			offset = 0;
			partial = NO;
		}
	}

	/* point to last character in buffer */
	if (offset > 0) bufend = buf + offset - 1;
	else		bufend = &buf[BSDSZ];

/*
 * output lines in reverse order
 */

	lines = 0;
	p = bufend;

	do {
		last = p;

		/* scan for a newline, handle wrap-around */
		do {
			if (--p < buf) {
				if (partial) {
					write(1, buf, (unsigned)(last-buf+1));
					return;
				}
				p = buf + BSDSZ;
			}
		} while (*p != NEWLINE && p != bufend);

		/* found a newline, output collected line */
		if (p < last)
			write(1, p+1, (unsigned)(last-p));
		else {
			write(1, p+1, (unsigned)(&buf[BSDSZ]-p));
			write(1, buf, (unsigned)(last-buf+1));
		}
	
	} while (++lines < n_tail && p != bufend); /* up to n_tail lines */
	return;
}
/*
 * NAME: dochars
 *                                                                    
 * FUNCTION: 
 *
 * Last n_tail chars, no seek.  We use a circular buffer.  If the static bin
 * isn't big enough, we get the space from malloc(), rounding up to the next
 * CHUNK boundary if possible.  PORT WARNING: This routines assumes that long
 * (n_tail) and unsigned int (argument expected by malloc) and the difference
 * between two pointers (argument passed to to read/write) are all the same.
 * Not making this assumption requires a bit of finesse.
 */
#ifdef KJI
/* getting synchronized on the start of a character (without rewriting this
 * routine completely) is a bit tricky.  We start by saving CHUNK/2 extra
 * bytes so we can check for character boundaries.  If this works (charbdry()
 * returns TRUE), we have no problems, otherwise, we have to move forward of
 * the requested point until one byte past the first non shift character, which
 * may not occur before EOF.  If one wants to GUARANTEE getting output from a 
 * file containing kanji characters, one should use the (new) +|-k flag.
 */
#endif
/*
 *
 * RETURN VALUE: none
 */
void dochars() {
    register char *b;
    register int nc;
    register long m, size;
    register bool isfull;
#ifdef KJI
    char *cp, *syncp, *curp;	/* for boundary checking */
    n_tail += CHUNK/2;		/* get extra to check for shift-ness */
#endif
    if (n_tail <= CHUNK) {
	size = CHUNK;
	b = bin;
    } else {
	size = (n_tail+CHUNK-1) / CHUNK * CHUNK;
	if ((b = malloc((size_t)size)) == NULL) {
	    size = n_tail;
	    if ((b = malloc((size_t)size)) == NULL) {
		fprintf(stderr, MSGSTR(TOOMUCH, "%s: too much!\n"), progname); /*MSG*/
		exit(1);
	    }
	}
    }
    isfull = NO;
    m = 0;
    while ((nc = read(0, b+m, (unsigned)(size-m))) > 0) {
	if ((m += nc) >= size) { 
	    isfull = YES;
	    m = 0;
	}
    }
#ifdef KJI
    if ( m >= n_tail ) { /* the easy case, the buffer is contiguous */
	cp = b + m - n_tail + CHUNK/2;	/* the point actually asked for */
	if ( !charbdry(b, &cp) && isfull ) { /* going back wasn't sufficient */
	    /* now we have to go forward and find a real sync point */
	    while ( cp++ < b + m ) {
		if ( foundsync ) break;
		if ( !NCisshift(*cp) ) foundsync++;
	    }
	}
    } else if ( m >= n_tail - CHUNK/2 ) { /* the actual number requested */
	cp = b + m - n_tail + CHUNK/2;
	if ( !charbdry(b, &cp) && isfull ) { 
	/* if !isfull, b is valid sync point AKA beginning of file */
	    syncp = b + m; /* last available sync point */
	    curp = b + size;
	    if(NCisshift(*curp)) {		/* if the last char is a */
		if (charbdry(syncp,&curp)){	/* shift, and curp ends up */
		    if ( curp == b+size ) {	/* still pointing to the */
			syncp = b + 1;		/* last byte, then it is the */
			charbdry(syncp,&cp);	/* start of a 2 byte char  */
		    }
		} else { /* no choice, we must look forward */
		    while ( cp++ < b + m ) {
			if ( foundsync ) break;
			if ( !NCisshift(*cp) ) foundsync++;
		    }
		}
	    } 
	} 
    } else if ( isfull ) { /* point wanted is in the end if the buffer */
	cp = b+size+m-n_tail-CHUNK/2;
	if (charbdry(b+m,&cp)) {
	    write(1, cp, b+size-cp);
	    cp = b; 
	} else { 
	    while ( cp++ < (b + size) && !foundsync) {
		if ( !NCisshift(*cp) ) foundsync++;
	    }
	    if ( cp < b + size ) { 
		write(1,cp,b+size-cp);
		cp = b; 
	    } else { /* haven't found the syncpoint yet */
		for (cp=b; cp < b+m; cp++) {
		    if ( foundsync ) break;
		    if (!NCisshift(*cp)) foundsync++;
		}
	    }
	}
    } else cp = b; /* read was less than requested, sync is assured */
    write(1,cp, b+m-cp);
#else
    if (m >= n_tail) { 
	write(1, b+m-n_tail, (unsigned)(n_tail));
    } else {
	if (isfull) write(1, b+size+m-n_tail, (unsigned)(n_tail-m));
	write(1, b, (unsigned)m);
    }
#endif
}

/*
 * Last n_tail lines: the most likely case, and also the hardest.
 * The strategy is to maintain a linked list of buffers, of which all but
 * the last are full.  If the file is seekable, the "last" (partial) buffer
 * is the first one read; its size is adjusted according to the size of the
 * file.
 */

typedef struct Buffer Buffer;
struct Buffer {
    Buffer *next;
    long    lines;
    char    buf[CHUNK];
};
Buffer *qh = NULL;      /* head of linked list of full buffers */
Buffer *qp;             /* partial buffer at end of file */
int     psize;          /* size of partial buffer */

/*
 * NAME: mkbuf
 *                                                                    
 * FUNCTION:  get a buffer.
 *                                                                    
 * RETURN VALUE:  The buffer just malloc'ed is returned.
 *			    
 */  

Buffer 
*mkbuf() 
{
    register Buffer *q;
    if ((q = (Buffer *)malloc((size_t)sizeof(Buffer))) == NULL) {
	fprintf(stderr, MSGSTR(TOOMUCH, "%s: too much!\n"), progname); /*MSG*/
	exit(1);
    }
    q->lines = 0;
    return (q);
}

/*
 * NAME: rmbuf
 *                                                                    
 * FUNCTION:  Release a buffer.
 *                                                                    
 * RETURN VALUE: none
 */  

void rmbuf(q) Buffer *q; {
    free((void *)q);
}

/*
 * NAME: nlines
 *                                                                    
 * FUNCTION:  Count the number of lines in a buffer.
 *                                                                    
 * RETURN VALUE:  The number of lines.
 */  

int nlines(p, n) register char *p; register int n; {
    register int r = 0;
    while (--n >= 0) if (*p++ == '\n') ++r;
    return (r);
}

/*
 * NAME: dolines
 *                                                                    
 * FUNCTION:  	If seek-able file. seek to the tailing point.  Otherwise,
 *		read from the file, keeping a ring buffer of the amount
 *		you want to print at the end.
 *                                                                    
 * RETURN VALUE:  void
 */  

void dolines() {
    register int nc, nl;
    register long tlines = 0;
    qp = mkbuf();
    if (canseek) {
	long e = lseek(0, 0L, SEEK_END);
	long r = lseek(0, -(e % CHUNK), SEEK_CUR);
	while (psize < e-r) {
	    if ((nc = read(0, &qp->buf[psize], (unsigned)(e-r-psize))) <= 0) {
		fprintf(stderr, MSGSTR(FSHRUNK, "%s: file shrunk!\n"), progname); /*MSG*/
		exit(1);
	    }
	    nl = nlines(&qp->buf[psize], nc);
	    psize += nc;
	    qp->lines += nl;
	    tlines += nl;
	}
	while (r > 0 && tlines <= n_tail) {
	    register int k;
	    register Buffer *temp = mkbuf();
	    temp->next = qh;
	    qh = temp;
	    r = lseek(0, r-CHUNK, SEEK_SET);
	    for (k = 0; k < CHUNK; k += nc) {
		if ((nc = read(0, &qh->buf[k], (unsigned)(CHUNK-k))) <= 0) {
		    fprintf(stderr, MSGSTR(FSHRUNK, "%s: file shrunk!\n"), progname); /*MSG*/
		    exit(1);
		}
	    }
	    tlines += qh->lines = nlines(&qh->buf[0], CHUNK);
	}
	lseek(0, e, SEEK_SET);
    } else {
	register Buffer **qt = &qh;
	while ((nc = read(0, &qp->buf[psize], (unsigned)(CHUNK-psize))) > 0) {
	    nl = nlines(&qp->buf[psize], nc);
	    psize += nc;
	    qp->lines += nl;
	    tlines += nl;
	    while (qh != NULL && tlines > qh->lines + n_tail) {
		register Buffer *temp = qh;
		qh = qh->next;
		if (qh == NULL) qt = &qh;
		tlines -= temp->lines;
		rmbuf(temp);
	    }
	    if (psize >= CHUNK) {
		qp->next = NULL;
		*qt = qp;
		qt = &qp->next;
		qp = mkbuf();
		psize = 0;
	    }
	}
    }
    if (qh != NULL) {
	register char *p = &qh->buf[0];
	while (tlines > n_tail) if (*p++ == '\n') --tlines;
	if (p < &qh->buf[CHUNK]) write(1, p, (unsigned)(&qh->buf[CHUNK]-p));
	while ((qh = qh->next) != NULL) write(1, &qh->buf[0], (unsigned)CHUNK);
	if (psize > 0) write(1, &qp->buf[0], (unsigned)psize);
    } else {
	register char *p = &qp->buf[0];
#ifndef OLDBUG
	while (tlines > n_tail && p - &qp->buf[0] < psize ) 
		if (*p++ == '\n') --tlines;
#else /* OLDBUG in original code */
	while (tlines > n_tail) if (*p++ == '\n') --tlines;
#endif
	psize -= p - &qp->buf[0];
	if (psize > 0) write(1, p, (unsigned)psize);
    }
}

#ifdef KJI
/*
 * NAME: charbdry
 *                                                                    
 * FUNCTION and RETURN VALUE: 
 * find the nearest character boundary to strptr, return 1 if it was determined
 * without reference to strstart (which is an ASSUMED sync point) return 0 if
 * we had to use that assumption.
 *			    
 */
bool
charbdry(strstart, strptr)
char *strstart;
char **strptr;
{
	char *p;
	if (*strptr <= strstart) return (0);
	for(p= *strptr;p > strstart && NCisshift(p[-1]); --p)
		;
	if (((*strptr-p) & 1)) *strptr -= 1; 
	return( (p > strstart) ? 1 : 0 );
}

/*
 * NAME: dowidechars
 *                                                                    
 * FUNCTION: 
 * show last n_tail (actual) characters, we might have to read the whole file
 * to be sure we're aligned... We first try to read just the last n_tail*2+1024
 * bytes and probably we'll be lucky and hit a syncronization point.
 *                                                                    
 * RETURN VALUE: none
 */  


void
dowidechars()
{
	char *syncp;
	char *curp;
	char *endp;
	int  numfound = 0;
	long bufsize;
	long nc;
	long size;
	
	if ( canseek ) { 
	    /* easy case, static buffer is big enough */
	    if ( n_tail <= (CHUNK - 1024)/2 ) {
		bufsize = CHUNK;
		syncp = bin;
	    } else {
		bufsize = n_tail*2+1024;
		if ((syncp = malloc(bufsize)) == (char *)NULL) {
		    bufsize = n_tail*2+1; 
		    if ((syncp = malloc(bufsize)) == (char *)NULL) {
			fprintf(stderr, MSGSTR(TOOMUCH, "%s: too much!\n"), progname); /*MSG*/
			return;
		    }
		}
	    }
            size = lseek(0,0L,SEEK_END);
            if ( size > bufsize)
               lseek(0,-bufsize,SEEK_END);
            else
               lseek(0,0L,SEEK_SET);

	    if ((nc = read(0,syncp,bufsize)) <= 0) return;
	    curp = endp = syncp + nc;
	    /* see if enough sync points are available */
	    while ( curp > syncp && charbdry(syncp,&curp) ) {
		if ( numfound++ == n_tail ) break; 
		curp--;
	    }
	    if ( numfound == n_tail || nc < bufsize ) { 
		/* found enough or started at BOF */
		write(1,curp, endp - curp);
		return;
	    }
	    lseek(0,0L,SEEK_SET); /* reset to beginning of file */
	} 
	/* didn't get synced (or the file isn't seekable), now we try the slow way */
	doring();
}

/*
 * NAME: skipwidechars
 *                                                                    
 * FUNCTION:  discard n_tail characters, print out the rest of the file
 *                                                                    
 * RETURN VALUE: none
 */  

void 
skipwidechars()
{	
	int nc, nl = 0;
	char *cp;

      	while (nl < n_tail && (nc = read(0,bin,(unsigned)CHUNK)) > 0 ) {
		for( cp=bin; cp-bin < nc && nl<n_tail; nl++) 
			cp += NLchrlen(cp);
		if ( cp - bin > nc )   /* opps! got a half character */
			read(0,bin,1); /* throw away the other half */
	}
      	if ( cp - bin < nc ) { /* write out partial buffer */
    	 	write(1, cp, nc - (int)(cp - bin) );
	}
      	while ((nc = read(0,bin,(unsigned)CHUNK)) > 0 ) /* and the rest of the file */
		write(1,bin,nc);
}

typedef struct Wchr Wchr;
struct Wchr {
	Wchr *next;
	char type;
	char c[2];
};
Wchr *First = NULL;
#define TWOCHAR 2
#define NORMAL 1
#define EMPTY 0
/*
 * NAME: setupring
 *                                                                    
 * FUNCTION: setup a linked list of buffers for tailing non-seekable files
 *                                                                    
 * RETURN VALUE: none
 */  

void
setupring(num)
int num;
{
	Wchr *last;
	Wchr *wchr;
	if (( First = (Wchr *)malloc(sizeof (Wchr))) == NULL ) {
	    fprintf(stderr,MSGSTR(TOOMUCH,"%s: too much!\n"), progname); /*MSG*/
	    exit(1);
	}
	First->type = EMPTY;
	last = First;
	while(--num) {
	    if (( wchr = (Wchr *)malloc(sizeof (Wchr))) == NULL ) {
		fprintf(stderr, MSGSTR(TOOMUCH, "%s: too much!\n"), progname); /*MSG*/
		exit(1);
	    }
	    wchr->type = EMPTY;
	    wchr->next = last;
	    last = wchr;
	}
	First->next = last;
}
/*
 * NAME: doring
 *                                                                    
 * FUNCTION: 
 *	 put characters into a ring buffer, output the buffer after EOF 
 *	 this isn't very efficient, Shift-JIS has it's price 		 
 *                                                                    
 * RETURN VALUE: none
 */  

void
doring()
{
	int nc;
	char *cp, *endp;
	Wchr *cur;
	Wchr *last;

	setupring(n_tail);
	cur = First;
	while (( nc = read(0,bin,(unsigned)CHUNK)) > 0 ) {
		cp = bin;
		endp = bin + nc;
		while(endp > cp) {
			cur->c[0] = *cp++;
			if (NCisshift(cp[-1]) ) {
			    if ( cp > endp ) { /* got only half a character */
				if (read(0,bin,1) == 0) /* get the rest of it */
				    break;  /* can this happen? */
				cp = bin;
				endp = 0; /* force next outer loop */
			    }
			    cur->c[1] = *cp++; /* put the other half in */
			    cur->type = TWOCHAR;
			} else 
			    cur->type = NORMAL;
			last = cur;	
			cur = cur->next;
		}
	}
	/* print out the ring, starting at the slot already pointed to. */
	do {
		switch(cur->type) {
			case EMPTY: 
				break; /* nothing in this slot */
			case NORMAL: 
				putchar(cur->c[0]);
				break;
			case TWOCHAR: 
				putchar(cur->c[0]);
				putchar(cur->c[1]);
				break;
		}
		cur = cur->next;
	} while ( cur != last->next);
	exit (0);
}
#endif
