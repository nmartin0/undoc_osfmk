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
static char rcsid[] = "@(#) $RCSfile: unpack.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:45:05 $";
#endif

/*
 * COMPONENT_NAME: (CMDFILES) commands that manipulate files
 *
 * FUNCTIONS: unpack
 *
 * ORIGINS: 3, 27
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
#if !defined(lint)
static char sccsid[] = "unpack.c	1.11  com/cmd/files,3.1,9013 12/7/89 17:05:24";
#endif
/*
 * unpack: expand files
 *	Huffman decompressor
 *	Usage:	pcat filename...
 *	or	unpack filename...
 */


#include <stdio.h>
#include <locale.h>
#include <setjmp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utime.h>
#include <sys/access.h>
#include <sys/dir.h>

#include "unpack_msg.h"

nl_catd	catd;
#define MSGSTR(Num, Str) catgets(catd, MS_UNPACK, Num, Str)

static int decode();
static int getch();
static int getwd();
static void eprintf();
static void expand();
static void putch();
#ifdef lint
	int	_void_;
#	define VOID	_void_ = (int)
#else
#	define VOID
#endif

jmp_buf env;
struct	stat status;
struct	utimbuf times;
char	*argv0, *argvk;
short	errorm;

#define NAMELEN PATH_MAX+1
#define MAXBASE NAME_MAX-1
#define SUF0	'.'
#define SUF1	'z'
#define US	037
#define RS	036

#define BLKSIZE BUFSIZ


/* variables associated with i/o */
char	filename[NAMELEN+2];
short	infile;
short	outfile;
short	inleft;
char	*inp;
char	*outp;
char	inbuff[BUFSIZ];
char	outbuff[BUFSIZ];

/* the dictionary */
long	origsize;
short	maxlev;
short	intnodes[25];
char	*tree[25];
char	characters[256];
char	*eof;

/*
 * NAME: getdict
 *                                                                    
 * FUNCTION: read in the dictionary portion and build decoding 
 *           structures, return 1 if successful, 0 otherwise 
 */
static int
getdict ()
{
	int c, i, nchildren;

	/*
	 * check two-byte header
	 * get size of original file,
	 * get number of levels in maxlev,
	 * get number of leaves on level i in intnodes[i],
	 * set tree[i] to point to leaves for level i
	 */
	eof = &characters[0];

	inbuff[6] = 25;
	inleft = read (infile, &inbuff[0], BUFSIZ);
	if (inleft < 0) {
		eprintf (MSGSTR(READERR, ".z: read error"));
		return (0);
	}
	if (inbuff[0] != US)
		goto goof;

	if (inbuff[1] == US) {		/* oldstyle packing */
		if (setjmp (env))
			return (0);
		expand ();
		return (1);
	}
	if (inbuff[1] != RS)
		goto goof;

	inp = &inbuff[2];
	origsize = 0;
	for (i=0; i<4; i++)
		origsize = origsize*256 + ((*inp++) & 0377);
	maxlev = *inp++ & 0377;
	if (maxlev > 24) {
goof:		eprintf (MSGSTR(NOTPCKD, ".z: not in packed format"));
		return (0);
	}
	for (i=1; i<=maxlev; i++)
		intnodes[i] = *inp++ & 0377;
	for (i=1; i<=maxlev; i++) {
		tree[i] = eof;
		for (c=intnodes[i]; c>0; c--) {
			if (eof >= &characters[255])
				goto goof;
			*eof++ = *inp++;
		}
	}
	*eof++ = *inp++;
	intnodes[maxlev] += 2;
	inleft -= inp - &inbuff[0];
	if (inleft < 0)
		goto goof;

	/*
	 * convert intnodes[i] to be number of
	 * internal nodes possessed by level i
	 */

	nchildren = 0;
	for (i=maxlev; i>=1; i--) {
		c = intnodes[i];
		intnodes[i] = nchildren /= 2;
		nchildren += c;
	}
	return (decode ());
}

/*
 * NAME: decode
 *                                                                    
 * FUNCTION: unpack the file, return 1 if successful, 0 otherwise 
 */
static int
decode ()
{
	int bitsleft, c, i;
	int j, lev;
	char *p;

	outp = &outbuff[0];
	lev = 1;
	i = 0;
	while (1) {
		if (inleft <= 0) {
			inleft = read (infile, inp = &inbuff[0], BUFSIZ);
			if (inleft < 0) {
				eprintf (MSGSTR(RDERR1, ".z: read error"));
				return (0);
			}
		}
		if (--inleft < 0) {
uggh:			eprintf (MSGSTR(UNPCKERR, ".z: unpacking error"));
			return (0);
		}
		c = *inp++;
		bitsleft = 8;
		while (--bitsleft >= 0) {
			i *= 2;
			if (c & 0200)
				i++;
			c <<= 1;
			if ((j = i - intnodes[lev]) >= 0) {
				p = &tree[lev][j];
				if (p == eof) {
					c = outp - &outbuff[0];
					if (write (outfile, &outbuff[0], c) != c) {
wrerr:						eprintf (MSGSTR(WRITERR, ": write error"));
						return (0);
					}
					origsize -= c;
					if (origsize != 0)
						goto uggh;
					return (1);
				}
				*outp++ = *p;
				if (outp == &outbuff[BUFSIZ]) {
					if (write (outfile, outp = &outbuff[0], BUFSIZ) != BUFSIZ)
						goto wrerr;
					origsize -= BUFSIZ;
				}
				lev = 1;
				i = 0;
			} else
				lev++;
		}
	}
}

/*
 * NAME: unpack
 *                                                                    
 * FUNCTION: expand packed files
 */  
main (argc, argv)
	char *argv[];
{
	int i, k;
	int sep, pcat = 0;
	char *p1, *cp;
	int fcount = 0;		/* failure count */

	(void) setlocale(LC_ALL,"");
	catd = catopen(MF_UNPACK, 0);

	p1 = *argv;
	while(*p1++);		/* Point p1 to end of argv[0] string */
	while(--p1 >= *argv)
		if(*p1 == '/')break;
	*argv = p1 + 1;
	argv0 = argv[0];
	if(**argv == 'p')pcat++;	/* User entered pcat (or /xx/xx/pcat) */
	for (k=1; k<argc; k++) {
		errorm = 0;
		sep = -1;
		cp = filename;
		argvk = argv[k];
		for (i=0; i < (NAMELEN-3) && (*cp = argvk[i]); i++)
			if (*cp++ == '/')
				sep = i;
		if (cp[-1] == SUF1 && cp[-2] == SUF0) {
			argvk[i-2] = '\0'; /* Remove suffix and try again */
			k--;
			continue;
		}

		fcount++;	/* expect the worst */
		if (i >= (NAMELEN-3) || (i - sep) > MAXBASE) {
			eprintf (MSGSTR(NMLENERR, ": file name too long"));
			goto done;
		}
		*cp++ = SUF0;
		*cp++ = SUF1;
		*cp = '\0';
		if ((infile = open (filename, 0)) == -1) {
			eprintf (MSGSTR(CANTOPNZ, ".z: cannot open"));
			goto done;
		}

		if (pcat)
			outfile = 1;	/* standard output */
		else {
			if (stat (argvk, &status) != -1) {
				eprintf (MSGSTR(FILEXST, ": already exists"));
				goto done;
			}
			VOID fstat ((int)infile, &status);
			if (status.st_nlink != 1) {
				eprintf (MSGSTR(LINKERR, 
					".z: Warning: file has links"));
				errorm = 0;
			/* the error code should be reset because the above
			   message is actually a warning message. */
			}


			/* create file with minimum permissions. */
			/* Assign permissions later */
			if ((outfile = creat (argvk, 0600)) == -1) {
				eprintf (MSGSTR(CREATERR, ": cannot create"));
				goto done;
			}

			VOID chown (argvk, status.st_uid, status.st_gid);
		}


		if (getdict ()) {	/* unpack */
			fcount--; 	/* success after all */
			if (!pcat) {
				eprintf (MSGSTR(UNPCKD, ": unpacked"));
				VOID unlink (filename);

				/*
				 * preserve acc & mod dates
				 */
				times.actime = status.st_atime;
				times.modtime = status.st_mtime;
				VOID utime (argvk, &times);
			}
		}
		else
			if (!pcat)
				VOID unlink (argvk);
done:		if (errorm)
			VOID fprintf (stderr, "\n");
		VOID close (infile);
		if (!pcat)
			VOID close (outfile);
	}
	return (fcount);
}

/*
 * NAME: eprintf
 *                                                                    
 * FUNCTION: print error messages
 */  
static void
eprintf (s)
	char *s;
{
	if (!errorm) {
		errorm = 1;
		VOID fprintf (stderr, "%s: %s", argv0, argvk);
	}
	VOID fprintf (stderr, s);
}

/*
 * NAME: expand
 *                                                                    
 * FUNCTION:  This code is for unpacking files that
 *            were packed using the previous algorithm.
 */
int	Tree[1024];
static void
expand ()
{
	int tp, bit;
	short word;
	int keysize, i, *t;

	outp = outbuff;
	inp = &inbuff[2];
	inleft -= 2;
	origsize = ((long) (unsigned) getwd ())*256*256;
	origsize += (unsigned) getwd ();
	t = Tree;
	for (keysize = getwd (); keysize--; ) {
		if ((i = getch ()) == 0377)
			*t++ = getwd ();
		else
			*t++ = i & 0377;
	}

	bit = tp = 0;
	for (;;) {
		if (bit <= 0) {
			word = getwd ();
			bit = 16;
		}
		tp += Tree[tp + (word<0)];
		word <<= 1;
		bit--;
		if (Tree[tp] == 0) {
			putch (Tree[tp+1]);
			tp = 0;
			if ((origsize -= 1) == 0) {
				(void)write (outfile, outbuff, outp - outbuff);
				return;
			}
		}
	}
}

/*
 * NAME: getch
 *                                                                    
 * FUNCTION:  get next character from input file
 */  
static int
getch ()
{
	if (inleft <= 0) {
		inleft = read (infile, inp = inbuff, BUFSIZ);
		if (inleft < 0) {
			eprintf (MSGSTR(ZRDERR, ".z: read error"));
			longjmp (env, 1);
		}
	}
	inleft--;
	return (*inp++ & 0377);
}

/*
 * NAME: getwd
 *                                                                    
 * FUNCTION:  get word from file
 */  
static int
getwd ()
{
	char c;
	int d;
	c = getch ();
	d = getch ();
	d <<= 8;
	d |= c & 0377;
	return (d);
}

/*
 * NAME: putch
 *                                                                    
 * FUNCTION: put character into out buffer
 */  
static void
putch (c)
	char c;
{
	int n;

	*outp++ = c;
	if (outp == &outbuff[BUFSIZ]) {
		n = write (outfile, outp = outbuff, BUFSIZ);
		if (n < BUFSIZ) {
			eprintf (MSGSTR(WRITERR1, ": write error"));
			longjmp (env, 2);
		}
	}
}
