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
static char rcsid[] = "@(#)$RCSfile: wc.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:47:20 $";
#endif
/*
 * COMPONENT_NAME: (CMDFILES) commands that manipulate files
 *
 * FUNCTIONS: wc
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
 */
#include        <NLctype.h>
#include	<stdio.h>
#include	<locale.h>
#include "wc_msg.h"
nl_catd catd;
#define MSGSTR(Num,Str) catgets(catd,MS_WC,Num,Str)
unsigned char    b[BUFSIZ+1];
int	f = 0;
long	wordct;             /* word count */
long	twordct;            /* total word count */
long	linect;             /* line count */
long	tlinect;            /* total line count */
long    bytect;             /* name changed from charct... */
long    tbytect;            /* total byte count */
#ifdef KJI
long    charct;             /* for real character count */
long	tcharct;            /* total character count */

NLchar nlc;
int countchar = 0;              /* set by -k flag */
#define KJI_SPACE               0x8140      /* KJI wide space char */
#endif

/*
 * NAME: wc [-lwc] names
 *                                                                    
 * FUNCTION: Counts the number of lines, words and characters in a file.
 *           -l       counts lines only.
 *           -w       counts words only.
 *           -c       counts characters only.
 *           -k       counts actual characters not bytes.
 */  
main(argc,argv)
int argc;
char **argv;
{
	unsigned char *p1, *p2;
	int c;
	int token;
	int i;
	int	status = 0;
	char	*wd;

	(void) setlocale(LC_ALL,"");
	catd = catopen(MF_WC,0);
	wd = "lwc";                  /* default mode */

	if(argc > 1 && *argv[1] == '-') {
		wd = ++argv[1];
		argc--;
		argv++;
	}

#ifdef KJI
	/* scan through wd to check for existence of -k option */
	/* the -k option modifies -c to count characters rather */
	/* than bytes. So, a single "-k" means "-klwc"...   */
	if(strchr(wd,'k'))
		++countchar;
	/* check for a lone "-k".  If so, then reset flags to default out */
	if(!strcmp(wd,"k"))
		wd = "klwc";
#endif

	i = 1;
	do {
		if(argc>1 && (f=open(argv[i],0))<0) {
			fprintf(stderr,MSGSTR(CANTOPEN,                 /*MSG*/
				"wc: cannot open %s\n"), argv[i]);      /*MSG*/
			status = 2;
			continue;
		}
		p1 = p2 = b;
		linect = 0;
		wordct = 0;
		bytect = 0;
		token = 0;
#ifdef KJI
		charct = 0;
#endif
  /* count lines, words and characters but check options before printing */
		for(;;) {
			if(p1 >= p2) {
				p1 = b;
				c = read(f, p1, BUFSIZ);
				if(c <= 0)
					break;
				bytect += c;
#ifdef KJI
				charct += c;
#endif
				p2 = p1+c;
			}
#ifdef KJI
					/* here we count real characters */
					/* if last char is a shift, read */
					/* one more byte. */
			if (countchar) {
				if (NCisshift(*p1)) {
					if (p1+1 == p2) {
					   c = read(f,p1+1,1);
					   charct++;
					   if (c <= 0) break;
					}
					p1 += _NCdec2(p1[0], p1[1], nlc);
					charct--;
				} else  nlc = *p1++;
				if (nlc > ' ' && nlc != KJI_SPACE) {
					if(!token) {
						wordct++;
						token++;
					}
					continue;
				} else  if (nlc == ' ' || nlc == '\t'
						       || nlc == KJI_SPACE) {
						token = 0;
						continue;
					} else  if (nlc == '\n') {
							linect++;
							token = 0;
							continue;
						} else  continue;
			} else  c = *p1++;
#else
			c = *p1++;
			/*   It isn't necessary to check for NLS font-shifts
			 *   here: they will fall through the loop with no
			 *   effect and the next byte will be processed as
			 *   a word character, incrementing wordct and token
			 *   if necessary.
			 */

#endif
			if (c > ' ') {
				if(!token) {
					wordct++;
					token++;
				}
				continue;
			}
			/* reordered these cases to get a bit more speed */
			else if (c==' '||c=='\t')
				token = 0;
			else if (c=='\n') {
				token = 0;
				linect++;
			}
		}

		/* print lines, words, chars */
#ifdef KJI
		wcp(wd, bytect, wordct, linect, charct);
#else
		wcp(wd, bytect, wordct, linect);
#endif
		if(argc>1) {
			printf("%s\n", argv[i]);
		}
		else
			printf("\n");
		close(f);
		tlinect += linect;
		twordct += wordct;
		tbytect += bytect;
#ifdef KJI
		tcharct += charct;
#endif
	} while(++i<argc);
	if(argc > 2) {
#ifdef KJI
		wcp(wd, tbytect, twordct, tlinect, tcharct);
#else
		wcp(wd, tbytect, twordct, tlinect);
#endif
		printf(MSGSTR(TOTAL,"total\n"));                       /*MSG*/
	}
	exit(status);
}

/*
 * NAME: wcp
 *                                                                    
 * FUNCTION: check options then print out the requested numbers
 */  
#ifdef KJI
wcp(wd, bytect, wordct, linect,charct)
long bytect; long wordct; long linect; long charct;
#else
wcp(wd, bytect, wordct, linect)
long bytect; long wordct; long linect;
#endif
char *wd;
{
	char *wdp=wd;

	while(*wdp) {
	switch(*wdp++) {
#ifdef KJI
		case 'k':
			break;  /* To catch the switch, regardless */
#endif
		case 'l':
			printf("%7ld ", linect);
			break;

		case 'w':
			printf("%7ld ", wordct);
			break;

		case 'c':
			printf("%7ld ",
#ifdef KJI
				countchar ? charct : bytect);
#else
				bytect);
#endif
			break;

		default:
#ifdef KJI
			fprintf(stderr,MSGSTR(USAGE_K, "usage: wc [-kclw] [name ...]\n"));      /*MSG*/
#else
			fprintf(stderr,MSGSTR(USAGE, "usage: wc [-clw] [name ...]\n")); /*MSG*/
#endif
			exit(2);
		}
	}
}

