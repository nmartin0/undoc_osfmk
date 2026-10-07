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
static char rcsid[] = "@(#)$RCSfile: trbsd.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:43:46 $";
#endif
/*
 * COMPONENT_NAME: CMDFILES: tr
 *                                                                    
 * ORIGIN: IBM, BSD
 *
 * IBM CONFIDENTIAL
 * Copyright International Business Machines Corp. 1985, 1988
 * Unpublished Work
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the Rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 * 
 * tr.c	1.9  com/cmd/files/trbsd,3.1,9013 3/15/90 17:05:28";
 */                                                                   
/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */
#include <stdio.h>
#include <locale.h>
extern void exit();

/* tr - transliterate data stream */

#include "trbsd_msg.h"
#define MSGSTR(Num, Str) NLgetamsg(MF_TRBSD, MS_TRBSD, Num, Str)

#include <NLchar.h>
#ifndef KJI
#define	NCHARS	NLCHARMAX
#else
#define NCHARS  NLCOLMAX
#endif
typedef	int		CHAR;
int endchar = NCHARS;
#define	NCgetchar()	getwc(stdin)
#define	NCputchar(c)	putwc(c, stdout)
#define	GETC()	(Aflag ? getchar() : NCgetchar())

/** the following should be in the library instead **/
#define	NLsgetc(s)	(_NCis2((s)[0], (s)[1]) ? (s)+=2, _NCd2((unsigned char)((s)[-2]), ((unsigned char)((s)[-1]))) : (unsigned char)*(s)++)
/** end library **/

#ifdef STRIPNULLS
#define	EOS	0
#else
#define	EOS	NCHARS
#endif

int dflag = 0;
int sflag = 0;
int cflag = 0;
int Aflag = 0;
CHAR save = 0;
CHAR code[NCHARS];
char squeez[NCHARS];
CHAR vect[NCHARS];
struct string { int last, max; char *p; } string1, string2;

/*
 * NAME: tr [-cds] [string1 [string2]]
 * FUNCTION:  translate characters
 *  FLAGS:
 *   -c       complements the set of characters in string1 with respect to 
 *            string2
 *   -d       deletes all input characters in string1
 *   -s       squeezes all strings of repeated output characters that are in 
 *            string2 to single characters
 */
main(argc,argv)
char **argv;
{
	register i;
	int j;
	register d;
	int c;
	CHAR *compl;
	int lastd;

	(void) setlocale( LC_ALL, "" );
	string1.last = string2.last = 0;
	string1.max = string2.max = 0;
	string1.p = string2.p = "";

	if(--argc>0) {
		argv++;
		if(*argv[0]=='-'&&argv[0][1]!=0) {
			while(*++argv[0])
				switch(*argv[0]) {
				case 'c':
					cflag++;
					continue;
				case 'd':
					dflag++;
					continue;
				case 's':
					sflag++;
					continue;
				case 'A':
					Aflag++;
					endchar = 256;
					continue;
                                default:
                                        fprintf(stderr,
                                        MSGSTR(BADUSE,"usage:\ttr [ -c -cs -s -A ] string1 string2\n\
\ttr [ -d -cd ] string1\n") );
                                        exit(1);

				}
			argc--;
			argv++;
		}
	}
	if(argc>0) string1.p = argv[0];
	if(argc>1) string2.p = argv[1];
	if(cflag) {
		for(i=0; i<endchar; i++)
			vect[i] = 0;
		while ((c = next(&string1)) != EOS) {
#ifdef KJI
			if (!Aflag) c = _NCmap(c);
#endif
			vect[c] = 1;
		}
		j = 0;
		for(i=0; i<endchar; i++)
#ifdef KJI
			if(vect[i]==0) vect[j++] = (Aflag ? i : _NCunmap(i));
#else
			if(vect[i]==0) vect[j++] = i;
#endif
		vect[j] = EOS;
		compl = vect;
	}
	for(i=0; i<endchar; i++) {
		code[i] = EOS;
		squeez[i] = 0;
	}
	lastd = 0;
	for(;;){

		if(cflag) c = *compl++;
		else c = next(&string1);
		if(c==EOS) break;
		d = next(&string2);
	 	if (d==EOS) 
			if (sflag && !lastd) d = c;
                        else d = lastd;
		else lastd = d;
#ifdef KJI
		if (!Aflag) c = _NCmap(c);
#endif
		code[c] = d;
#ifdef KJI
		if (!Aflag) d = _NCmap(d);
#endif
		squeez[d] = 1;
	}
#ifdef KJI
	while ((d = next(&string2)) != EOS) {
		if (!Aflag) d = _NCmap(d);
		squeez[d] = 1;
	}
#else
	while ((d = next(&string2)) != EOS)
		squeez[d] = 1;
#endif
	for(i=0;i<endchar;i++) {
#ifdef KJI
		if(code[i]==EOS) code[i] = (Aflag ? i : _NCunmap(i));
#else
		if(code[i]==EOS) code[i] = i;
#endif
		else if(dflag) code[i] = EOS;
	}

	while ((c = GETC()) != EOF) {
#ifdef STRIPNULLS
		if(c == 0) continue;
#endif
#ifdef KJI
		if (!Aflag) c = _NCmap(c);
		if ((c = code[c]) != EOS) {
			if (!Aflag) d = _NCmap(c);
			if(!sflag || c!=save || !squeez[d])
#else
		if ((c = code[c]) != EOS)
			if(!sflag || c!=save || !squeez[c])
#endif
			{
				int count = 0;
				char twochar[2];

				save = c;
				{
				NLchar nlc;
				nlc = c;
				count = NCenc(&nlc, twochar);
				}
				if (count > 1) {  
					putchar(twochar[0]);
					putchar(twochar[1]);
				} else
					putchar(c);
			}
#ifdef KJI
		}
#endif
	}
	exit(0);
/* NOTREACHED */
}


next(s)
struct string *s;
{
#ifdef KJI
int save;
#endif
again:
	if(s->max) {
#ifdef KJI
                save = s->last;
                s->last = next_kanji(s->last);
		if(save < s->max)
#else
		if(s->last++ < s->max)
#endif KJI
			return(s->last);
		s->max = s->last = 0;
	}
	if(s->last && *s->p=='-') {
		nextc(s);
		s->max = nextc(s);
		if(s->max==0) {
			s->p--;
			return('-');
		}
		if(s->max < s->last)  {
			s->last = s->max-1;
			return('-');
		}
		goto again;
	}
	return(s->last = nextc(s));
}
nextc(s)
struct string *s;
{
	register i, c, n;
#ifdef MSG
	char * msgstr;
#endif

	c = (Aflag ? (unsigned char)*s->p++ : NLsgetc(s->p));
	if (c == '\0') {
		--s->p;
		return (EOS);
	}
	if(c=='\\') {
		i = n = 0;
		while(i<3 && (c = *s->p)>='0' && c<='7') {
			n = n*8 + c - '0';
			i++;
			s->p++;
		}
		if(i>0) c = n;
		else c = *s->p++;
		if (c >= endchar) {
#ifdef MSG
			msgstr = MSGSTR(BADVAL, "Bad octal value\n");
			write(2, msgstr, strlen(msgstr));
#else
			write(2, "Bad octal value\n", 16);
#endif
			exit(1);
		}
#ifdef STRIPNULLS
		if(c==0) *--s->p = 0;
#endif
	}
	return(c);
}
#ifdef KJI
int next_kanji(k)
int k;
{
unsigned int c1, c2, p,ku,ten,low,high;

       if(k < 256) return(++k);
        c1 = (0xff00 & k)>>8;
        c2 = 0xff & k;
        p = 0;
        if (c2 < 0x7f)
              c2 -= 0x1f;
        else if (c2 < 0x9f)
              c2 -= 0x20;
        else {
              c2 -= 0x7e;
              p++;
              }
        if (c1 < 0xa0)
                ku = (((c1 - 0x81) << 1) + 0x21 + p) - 32;
        else
                ku = (((c1 - 0xe0) << 1) + 0x5f + p) - 32;
        ten = c2 -32 ;
        if(++ten > 94){ ++ku;ten=1;}
        c1= ku-1 + 32;
        c2 = ten + 32;
        if (c1 < 0x5e)
            high = (c1 >> 1) + 0x71;
        else
            high = (c1 >> 1) + 0xb1;
        if (c1 & 1)
            low = c2 + 0x7e;
        else if (c2 <= 0x5f)
            low = c2 + 0x1f;
        else
            low = c2 + 0x20;
        return(0xffff &(high <<8 | low));
}
#endif KJI
