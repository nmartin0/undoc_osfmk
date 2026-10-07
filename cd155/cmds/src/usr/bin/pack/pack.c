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
static char rcsid[] = "@(#) $RCSfile: pack.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:32:48 $";
#endif

/*
 * COMPONENT_NAME: (CMDFILES) commands that manipulate files
 *
 * FUNCTIONS: pack
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
static char sccsid[] = "pack.c	1.10  com/cmd/files,3.1,9013 12/7/89 17:02:33";
#endif
/*
 *	Huffman encoding program 
 *	Usage:	pack [[ - ] filename ... ] filename ...
 *		- option: enable/disable listing of statistics
 */


#include <stdio.h>
#include <locale.h>
#include <sys/types.h>
#include <sys/limits.h>
#include <sys/stat.h>
#include <utime.h>
#include <sys/access.h>
#include <sys/dir.h>

#include "pack_msg.h"

nl_catd	catd;
#define MSGSTR(Num, Str) catgets(catd, MS_PACK, Num, Str)

#define	END	256
#define BLKSIZE BUFSIZ
#define NAMELEN PATH_MAX+1
#define MAXBASE NAME_MAX-1
#define PACKED 017436 /* <US><RS> - Unlikely value */
#define	SUF0	'.'
#define	SUF1	'z'

#define ASCUS   037      /* ascii US */
#define ASCRS   036      /* ascii RS */
#define MASK   0377
#define SHIFT    24
#define MAXLEV   24      /* max number of levels */
#define BYTE 	  8
#define SZOUTB	  6     /* size of out buffer */

struct stat status, ostatus;
struct utimbuf times;

/* union for overlaying a long int with a set of four characters */

/* character counters */
long	count [END+1];
long    insize;
long	outsize;
long	dictsize;
int	diffbytes;

/* i/o stuff */
char	vflag = 0;
int	force = 0;	/* allow forced packing for consistency in directory */
char	filename [NAMELEN];
int	infile;		/* unpacked file */
int	outfile;	/* packed file */
char	inbuff [BLKSIZE];
char	outbuff [BLKSIZE+4];

/* variables associated with the tree */
int	maxlev;
int	levcount [25];
int	lastnode;
int	parent [2*END+1];

/* variables associated with the encoding process */
char	length [END+1];
long	bits [END+1];
long    mask;
long	inc;

/* the heap */
int	n;
struct	heap {
	long int count;
	int node;
} heap [END+2];
#define hmove(a,b) {(b).count = (a).count; (b).node = (a).node;}

/*
 * NAME: input
 *                                                                    
 * FUNCTION:  gather character frequency statistics 
 *            return 1 if successful, 0 otherwise 
 */
static int
input ()
{
	int i;
	for (i=0; i<END; i++)
		count[i] = 0;
	while ((i = read(infile, inbuff, BLKSIZE)) > 0)
		while (i > 0)
			count[inbuff[--i] & MASK] += 2;
	if (i == 0)
		return (1);
	(void)printf (MSGSTR(READERR,": read error\n"));
	return (0);
}

/*
 * NAME: output
 *                                                                    
 * FUNCTION:  encode the current file  return 1 if successful, 0 otherwise 
 */
static int
output ()
{
	int c, i, inleft;
	char *inp, *outp;
	int bitsleft, j;
	long temp;

	/* output ``PACKED'' header */
	outbuff[0] = ASCUS; 	/* ascii US */
	outbuff[1] = ASCRS; 	/* ascii RS */
	/* output the length and the dictionary */
	temp = insize;
	for (i=5; i>=2; i--) {
		outbuff[i] =  (char) (temp & MASK);
		temp >>= BYTE;
	}
	outp = &outbuff[SZOUTB];
	*outp++ = maxlev;
	for (i=1; i<maxlev; i++)
		*outp++ = levcount[i];
	*outp++ = levcount[maxlev]-2;
	for (i=1; i<=maxlev; i++)
		for (c=0; c<END; c++)
			if (length[c] == i)
				*outp++ = c;
	dictsize = outp-&outbuff[0];

	/* output the text */
	(void)lseek(infile, 0L, 0);
	outsize = 0;
	bitsleft = BYTE;
	inleft = 0;
	do {
		if (inleft <= 0) {
			inp = &inbuff[0];	
			inleft = read(infile, inp, BLKSIZE);
			if (inleft < 0) {
				(void)printf (MSGSTR(READERR1, ": read error\n"));
				return (0);
			}
		}
		c = (--inleft < 0) ? END : (*inp++ & MASK);
		mask = bits[c]<<bitsleft;
		if (bitsleft == BYTE)
			*outp = (mask>>SHIFT)&MASK;
		else
			*outp |= ((mask>>SHIFT)&MASK);
		bitsleft -= length[c];
		if (bitsleft < 0) {
			j = 2;
			do {
				*++outp = (mask>>BYTE*j)&MASK;
				j--;
				bitsleft += BYTE;
			} while (bitsleft < 0);
		}
		if (outp >= &outbuff[BLKSIZE]) {
			if (write(outfile, outbuff, BLKSIZE) != BLKSIZE) {
      				(void)printf (MSGSTR(WRITERR, ".z: write error\n"));
				return (0);
			}
			for (j=0; j<4; j++)
				outbuff[j] = outbuff[j+BLKSIZE];
			outp -= BLKSIZE;
			outsize += BLKSIZE;
		}
	} while (c != END);
	if (bitsleft < BYTE)
		outp++;
	c = outp-outbuff;
	if (write(outfile, outbuff, c) != c) {
       	 	(void)printf (MSGSTR(WRITERR, ".z: write error\n"));
		return (0);
	}
	outsize += c;
	return (1);
}

/*
 * NAME: heapify
 *                                                                    
 * FUNCTION:  makes a heap out of heap[i],...,heap[n] 
 */
static int
heapify (i)
{
	int k;
	int lastparent;
	struct heap heapsubi;

	hmove (heap[i], heapsubi);
	lastparent = n/2;
	while (i <= lastparent) {
		k = 2*i;
		if ((heap[k].count > heap[k+1].count) && (k < n))
			k++;
		if (heapsubi.count < heap[k].count)
			break;
		hmove (heap[k], heap[i]);
		i = k;
	}
	hmove (heapsubi, heap[i]);
}

/*
 * NAME: packfile
 *                                                                    
 * FUNCTION:  pack file, return 1 after successful packing, 0 otherwise 
 */
static int 
packfile ()
{
	int c, i, p;
	long bitsout;

	/* gather frequency statistics */
	if (input() == 0)
		return (0);

	/* put occurring chars in heap with their counts */
	diffbytes = -1;
	count[END] = 1;
	insize = n = 0;
	for (i=END; i>=0; i--) {
		parent[i] = 0;
		if (count[i] > 0) {
			diffbytes++;
			insize += count[i];
			heap[++n].count = count[i];
			heap[n].node = i;
		}
	}
	if (diffbytes == 1) {
		(void)printf (MSGSTR(TRIVIAL, ": trivial file\n"));
		return (0);
	}
	insize >>= 1;
	for (i=n/2; i>=1; i--)
		heapify(i);

	/* build Huffman tree */
	lastnode = END;
	while (n > 1) {
		parent[heap[1].node] = ++lastnode;
		inc = heap[1].count;
		hmove (heap[n], heap[1]);
		n--;
		heapify(1);
		parent[heap[1].node] = lastnode;
		heap[1].node = lastnode;
		heap[1].count += inc;
		heapify(1);
	}
	parent[lastnode] = 0;

	/* assign lengths to encoding for each character */
	bitsout = maxlev = 0;
	for (i=1; i<=MAXLEV; i++)
		levcount[i] = 0;
	for (i=0; i<=END; i++) {
		c = 0;
		for (p=parent[i]; p!=0; p=parent[p])
			c++;
		levcount[c]++;
		length[i] = c;
		if (c > maxlev)
			maxlev = c;
		bitsout += (c * (count[i]>>1));
	}
	if (maxlev > MAXLEV ) {
		/* can't occur unless insize >= 2**24 */
		(void)printf (MSGSTR(LVLCNT, ": Huffman tree has too many levels\n"));
		return(0);
	}

	/* don't bother if no compression results */
	outsize = (long ) ((bitsout+7)>>3)+SZOUTB+maxlev+diffbytes;
	if ((((insize+BLKSIZE-1)/BLKSIZE) <= ((outsize+BLKSIZE-1)/BLKSIZE))
	    && !force) {
		(void)printf (MSGSTR(NOSAVE, ": no saving\n"));
		return(0);
	}

	/* compute bit patterns for each character */
	inc = 1L << SHIFT;
	inc >>= maxlev;
	mask = 0;
	for (i=maxlev; i>0; i--) {
		for (c=0; c<=END; c++)
			if (length[c] == i) {
				bits[c] = mask;
				mask += inc;
			}
		mask &= ~inc;
		inc <<= 1;
	}

	return (output());
}

/*
 * NAME: pack [-] [-f] file
 *                                                                    
 * FUNCTION: Compresses files. 
 *           -       displayes statistics about the input file
 *           -f      forces compaction
 */  
main(argc, argv)
int argc; char *argv[];
{
	int i;
	char *cp;
	int k, sep;
	int fcount = 0; /* count failures */


	(void ) setlocale(LC_ALL,"");
	catd = catopen(MF_PACK, 0);

	for (k=1; k<argc; k++)
	{       if (argv[k][0] == '-' && argv[k][1] == '\0')
		{       vflag = 1 - vflag;
			continue;
		}
		if (argv[k][0] == '-' && argv[k][1] == 'f') {
			force++;
			continue;
		}
		fcount++; /* increase failure count - expect the worst */
		(void)printf ("%s: %s", argv[0], argv[k]);
		sep = -1;  cp = filename;
		for (i=0; i < (NAMELEN-3) && (*cp = argv[k][i]); i++)
			if (*cp++ == '/') sep = i;
		if (cp[-1]==SUF1 && cp[-2]==SUF0)
		{	(void)printf (MSGSTR(PCKDMSG, ": already packed\n"));
			continue;
		}
		if (i >= (NAMELEN-3) || (i-sep) > MAXBASE)
		{       (void)printf (MSGSTR(FILNAMLEN, ": file name too long\n"));
			continue;
		}
		if ((infile = open (filename, 0)) < 0)
		{       (void)printf (MSGSTR(CANTOPEN, ": cannot open\n"));
			continue;
		}
	        (void)fstat(infile,&status);
		if (status.st_mode&040000)
		{       
			(void)printf (MSGSTR(PACKDIR, ": cannot pack a directory\n"));
			goto closein;
		}
		if( status.st_nlink != 1 )
		{	(void)printf(MSGSTR(LINKCNT, ": has links\n"));
			goto closein;
		}
		*cp++ = SUF0;  *cp++ = SUF1;  *cp = '\0';
		if( stat(filename, &ostatus) != -1)
		{
			(void)printf(MSGSTR(ZEXISTS, ".z: already exists\n"));
			goto closein;
		}


		/* create .z file with minimum permissions. */
		/* Assign permissions later. */
		if ((outfile = creat (filename, 0600)) < 0)
		{       (void)printf (MSGSTR(CANTCREAT, ".z: cannot create\n"));
			goto closein;
		}

		(void)chown (filename, status.st_uid, status.st_gid);

		if (packfile()) {
			if (unlink(argv[k]) != 0){
				(void)fprintf(stderr, MSGSTR(CANTUNLNK,
				"%s: can't unlink %s\n"), argv[0], argv[k]);
			}
			fcount--;  /* success after all */
			(void)printf (MSGSTR(COMPRESS, ": %.1f%% Compression\n"),
			   ((double )(-outsize+(insize))/(double)insize)*100);
			/* output statistics */
			if (vflag) {
				(void)printf(MSGSTR(SIZEMSG, 
				   "\tfrom %ld to %ld bytes\n"),  
					insize, outsize);
				(void)printf(MSGSTR(HUFFLVLS, 
				"\tHuffman tree has %d levels below root\n"
					), maxlev);
				(void)printf(MSGSTR(DISTINCT, 
				  "\t%d distinct bytes in input\n"), 
					diffbytes);
				(void)printf(MSGSTR(DICTOVRHD, 
				  "\tdictionary overhead = %ld bytes\n"), 
					dictsize);
				(void)printf(MSGSTR(EFFENTROPY, 
				  "\teffective  entropy  = %.2f bits/byte\n")
				  , ((double) outsize / (double) insize) * 8 );
				(void)printf(MSGSTR(ASYMENTROPY,
				 "\tasymptotic entropy  = %.2f bits/byte\n")
				 ,((double) (outsize-dictsize) / (double) insize) * 8 );
			}
		}
		else
		{       
			(void)printf (MSGSTR(FILUNCHG, "\t- file unchanged\n"));
			(void)unlink(filename);
		}

      closein:	(void)close (outfile);
		(void)close (infile);
		times.actime = status.st_atime;
		times.modtime = status.st_mtime;
		(void)utime(filename, &times);	/* preserve acc & mod times */
	}
	return (fcount);
}

