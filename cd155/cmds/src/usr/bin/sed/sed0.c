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
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: sed0.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:38:44 $";
#endif
/*
 * COMPONENT_NAME: (CMDEDIT) sed0.c
 *
 * FUNCTIONS: main, fcomp, comploop, compsup, rline, address, cmp, 
 * text, search, dechain, and ycomp.
 *
 * ORIGINS: 3, 10, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1984, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * sed0.c  1.6  com/cmd/edit,3.1,9021 3/2/90 11:52:50";
 */

#include <stdio.h>
#include "sed.h"
#include <locale.h>

#include "sed_msg.h"

nl_catd catd;
#define MSGSTR(Num, Str) catgets(catd, MS_SED, Num, Str)


FILE    *fin;
FILE    *fcode[12];
char    *lastre;
char    sseof;
char    *reend;
char    *hend;

NLchar  *rhsend;
NLchar  rhsbuf[LBSIZE];
NLchar  *rhsp;

union reptr     *ptrend;
int     eflag;
extern     nbra;
char    linebuf[LBSIZE+1];
int     gflag;
int     nlno;
char    fname[12][40];
int     nfiles;
union reptr ptrspace[PTRSIZE];
union reptr *rep;
char    *cp;
char    respace[RESIZE];
struct label ltab[LABSIZE];
struct label    *lab;
struct label    *labend;
int     depth;
int     eargc;
char    **eargv;
union reptr     **cmpend[DEPTH];
char    *badp;
char    bad;

#define CCEOF   22

struct label    *labtab = ltab;
char    CGMES[] = "sed: command garbled: %s\n";
char    TMMES[] = "Too much text: %s\n";
char    LTL[]   = "Label too long: %s\n";
char    AD0MES[]        = "No addresses allowed: %s\n";
char    AD1MES[]        = "Only one address allowed: %s\n";
char    MALLOCMES[]        = "Malloc failed\n";
extern  sed;      /* IMPORTANT flag !!! */
extern char *comple();

static int ycomp();


main(argc, argv)
char    *argv[];
{

	setlocale(LC_ALL,"");		/* required by NLS environment tests */

	catd = catopen(MF_SED,0);


	sed = 1;
	eargc = argc;
	eargv = argv;

	badp = &bad;
	aptr = abuf;
	lab = labtab + 1;       /* 0 reserved for end-pointer */
	rep = ptrspace;
	rep->r1.ad1 = respace;
	lbend = &linebuf[LBSIZE];
	hend = &holdsp[LBSIZE];
	lcomend = &genbuf[71];
	ptrend = &ptrspace[PTRSIZE];
	reend = &respace[RESIZE];
	labend = &labtab[LABSIZE];

	rhsp = rhsbuf;
	rhsend = &rhsbuf[LBSIZE];

	lnum = 0;
	pending = 0;
	depth = 0;
	spend = linebuf;
	hspend = holdsp;
	fcode[0] = stdout;
	nfiles = 1;

	if(eargc == 1)
		exit(0);


	while (--eargc > 0 && (++eargv)[0][0] == '-')
		switch (eargv[0][1]) {

		case 'n':
			nflag++;
			continue;

		case 'f':
			if(eargc-- <= 0)        exit(2);

			if((fin = fopen(*++eargv, "r")) == NULL) {
				fprintf(stderr, MSGSTR(PATTFIL, "Cannot open pattern-file: %s\n"), *eargv);  /* MSG */
				exit(2);
			}

			fcomp();
			fclose(fin);
			continue;

		case 'e':
			eflag++;
			if (*(eargv + 1) == NULL) {
				fprintf(stderr, MSGSTR(NOSCRIPT, "No editing script given.\n"));
				exit(2);
			}
			fcomp();
			eflag = 0;
			continue;

		case 'g':
			gflag++;
			continue;

		default:
			fprintf(stdout, MSGSTR(UNKFLG, "Unknown flag: %c\n"), eargv[0][1]);  /* MSG */
			continue;
		}


	if(rep == ptrspace) {
		eargv--;
		eargc++;
		eflag++;
		fcomp();
		eargv++;
		eargc--;
		eflag = 0;
	}

	if(depth) {
		fprintf(stderr, MSGSTR(LEFTBRC, "Too many {'s\n"));  /* MSG */
		exit(2);
	}

	labtab->address = rep;

	dechain();

/*      abort(); */        /*DEBUG*/

	if(eargc <= 0)
		execute((char *)NULL);
	else while(--eargc >= 0) {
		execute(*eargv++);
	}
	fclose(stdout);
	exit(0);
}
fcomp()
{

	register char   *p, *op;
	char	*tp;
	char    *address();
	union reptr     *pt, *pt1;
	int     i;
	struct label    *lpt;

	op = lastre;

	if(rline(linebuf) < 0)  return;
	if(*linebuf == '#') {
		if(linebuf[1] == 'n')
			nflag = 1;
	}
	else {
		cp = linebuf;
		goto comploop;
	}

	for(;;) {
		if(rline(linebuf) < 0)  break;

		cp = linebuf;

comploop:

      /* fprintf(stdout, "cp: %s\n", cp); */        /*DEBUG*/

		while(*cp == ' ' || *cp == '\t')        cp++;
		if(*cp == '\0')  continue;
		if(*cp == ';') {
			cp++;
			goto comploop;
		}

		p = address(rep->r1.ad1);
		if(p == badp) {
			fprintf(stderr, MSGSTR(CGMSG, CGMES), linebuf);  /*MSG */
			exit(2);
		}

		if(p == rep->r1.ad1) {
			if(op)
				rep->r1.ad1 = op;
			else {
				fprintf(stderr, MSGSTR(FRSTRE, "First RE may not be null\n"));  /* MSG */
				exit(2);
			}
		} else if(p == 0) {
			p = rep->r1.ad1;
			rep->r1.ad1 = 0;
		} else {
			op = rep->r1.ad1;
			if(*cp == ',' || *cp == ';') {
				cp++;
				if((rep->r1.ad2 = p) > reend) {
					fprintf(stderr, MSGSTR(TMMSG, TMMES), linebuf);  /* MSG */
					exit(2);
				}
				p = address(rep->r1.ad2);
				if(p == badp || p == 0) {
					fprintf(stderr, MSGSTR( CGMSG, CGMES), linebuf);  /* MSG */
					exit(2);
				}
				if(p == rep->r1.ad2)
					rep->r1.ad2 = op;
				else
					op = rep->r1.ad2;

			} else
				rep->r1.ad2 = 0;
		}

		if(p > reend) {
			fprintf(stderr, MSGSTR(TMMSG, TMMES), linebuf);  /* MSG */
			exit(2);
		}

		while(*cp == ' ' || *cp == '\t')        cp++;

swit:
		switch(*cp++) {

			default:
				fprintf(stderr, MSGSTR( BADCMND, "Unrecognized command: %s\n"), linebuf);  /* MSG */
				exit(2);

			case '!':
				rep->r1.negfl = 1;
				goto swit;

			case '{':
				rep->r1.command = BCOM;
				rep->r1.negfl = !(rep->r1.negfl);
				cmpend[depth++] = &rep->r2.lb1;
				if(++rep >= ptrend) {
					fprintf(stderr, MSGSTR( TOOMANYCMDNS, "Too many commands: %s\n"), linebuf);  /* MSG */
					exit(2);
				}
				rep->r1.ad1 = p;
				if(*cp == '\0') continue;

				goto comploop;

			case '}':
				if(rep->r1.ad1) {
					fprintf(stderr, MSGSTR(AD0MSG, AD0MES), linebuf);  /* MSG */
					exit(2);
				}

				if(--depth < 0) {
					fprintf(stderr, MSGSTR( RGHTBRC, "Too many }'s\n"));  /* MSG */
					exit(2);
				}
				*cmpend[depth] = rep;

				rep->r1.ad1 = p;
				continue;

			case '=':
				rep->r1.command = EQCOM;
				if(rep->r1.ad2) {
					fprintf(stderr, MSGSTR(AD1MSG, AD1MES), linebuf);  /* MSG */
					exit(2);
				}
				break;

			case ':':
				if(rep->r1.ad1) {
					fprintf(stderr, MSGSTR( AD0MSG, AD0MES), linebuf);  /* MSG */
					exit(2);
				}

				while(*cp++ == ' ');
				cp--;


				tp = lab->asc;
				while((*tp++ = *cp++))
					if(tp >= &(lab->asc[8])) {
						fprintf(stderr, MSGSTR(LTLMSG, LTL), linebuf);  /* MSG */
						exit(2);
					}
				*--tp = '\0';

				if(lpt = search(lab)) {
					if(lpt->address) {
						fprintf(stderr, MSGSTR( DUPLBL, "Duplicate labels: %s\n"), linebuf);  /* MSG */
						exit(2);
					}
				} else {
					lab->chain = 0;
					lpt = lab;
					if(++lab >= labend) {
						fprintf(stderr, MSGSTR( LABELCNT, "Too many labels: %s\n"), linebuf);  /* MSG */
						exit(2);
					}
				}
				lpt->address = rep;
				rep->r1.ad1 = p;

				continue;

			case 'a':
				rep->r1.command = ACOM;
				if(rep->r1.ad2) {
					fprintf(stderr, MSGSTR( AD1MSG, AD1MES), linebuf);  /* MSG */
					exit(2);
				}
				if(*cp == '\\') cp++;
				if(*cp++ != '\n') {
					fprintf(stderr, MSGSTR( CGMSG, CGMES), linebuf);  /* MSG */
					exit(2);
				}
				rep->r1.re1 = p;
				p = text(rep->r1.re1);
				break;
			case 'c':
				rep->r1.command = CCOM;
				if(*cp == '\\') cp++;
				if(*cp++ != ('\n')) {
					fprintf(stderr, MSGSTR( CGMSG, CGMES), linebuf);  /* MSG */
					exit(2);
				}
				rep->r1.re1 = p;
				p = text(rep->r1.re1);
				break;
			case 'i':
				rep->r1.command = ICOM;
				if(rep->r1.ad2) {
					fprintf(stderr, MSGSTR( AD1MSG, AD1MES), linebuf);  /* MSG */
					exit(2);
				}
				if(*cp == '\\') cp++;
				if(*cp++ != ('\n')) {
					fprintf(stderr, MSGSTR( CGMSG, CGMES), linebuf);  /* MSG */
					exit(2);
				}
				rep->r1.re1 = p;
				p = text(rep->r1.re1);
				break;

			case 'g':
				rep->r1.command = GCOM;
				break;

			case 'G':
				rep->r1.command = CGCOM;
				break;

			case 'h':
				rep->r1.command = HCOM;
				break;

			case 'H':
				rep->r1.command = CHCOM;
				break;

			case 't':
				rep->r1.command = TCOM;
				goto jtcommon;

			case 'b':
				rep->r1.command = BCOM;
jtcommon:
				while(*cp++ == ' ');
				cp--;

				if(*cp == '\0') {
					if(pt = labtab->chain) {
						while(pt1 = pt->r2.lb1)
							pt = pt1;
						pt->r2.lb1 = rep;
					} else
						labtab->chain = rep;
					break;
				}
				tp = lab->asc;
				while((*tp++ = *cp++))
					if(tp >= &(lab->asc[8])) {
						fprintf(stderr, MSGSTR( LTLMSG, LTL), linebuf);  /* MSG */
						exit(2);
					}
				cp--;
				*--tp = '\0';

				if(lpt = search(lab)) {
					if(lpt->address) {
						rep->r2.lb1 = lpt->address;
					} else {
						pt = lpt->chain;
						while(pt1 = pt->r2.lb1)
							pt = pt1;
						pt->r2.lb1 = rep;
					}
				} else {
					lab->chain = rep;
					lab->address = 0;
					if(++lab >= labend) {
						fprintf(stderr, MSGSTR( LABELCNT, "Too many labels: %s\n"), linebuf);  /* MSG */
						exit(2);
					}
				}
				break;

			case 'n':
				rep->r1.command = NCOM;
				break;

			case 'N':
				rep->r1.command = CNCOM;
				break;

			case 'p':
				rep->r1.command = PCOM;
				break;

			case 'P':
				rep->r1.command = CPCOM;
				break;

			case 'r':
				rep->r1.command = RCOM;
				if(rep->r1.ad2) {
					fprintf(stderr, MSGSTR( AD1MSG, AD1MES), linebuf);  /* MSG */
					exit(2);
				}
				if(*cp++ != ' ') {
					fprintf(stderr, MSGSTR( CGMSG, CGMES), linebuf);  /* MSG */
					exit(2);
				}
				rep->r1.re1 = p;
				p = text(rep->r1.re1);
				break;

			case 'd':
				rep->r1.command = DCOM;
				break;

			case 'D':
				rep->r1.command = CDCOM;
				rep->r2.lb1 = ptrspace;
				break;

			case 'q':
				rep->r1.command = QCOM;
				if(rep->r1.ad2) {
					fprintf(stderr, MSGSTR( AD1MSG, AD1MES), linebuf);  /* MSG */
					exit(2);
				}
				break;

			case 'l':
				rep->r1.command = LCOM;
				break;

			case 's':
				rep->r1.command = SCOM;
				sseof = *cp++;
				rep->r1.re1 = p;
				p = comple((char *) 0, rep->r1.re1, reend, sseof);
				if(p == badp) {
					fprintf(stderr, MSGSTR( CGMSG, CGMES), linebuf);  /* MSG */
					exit(2);
				}
				if(p == rep->r1.re1) {
					rep->r1.re1 = op;
				} else {
					op = rep->r1.re1;
				}


				if((rep->r1.rhs = rhsp) > rhsend) {
					fprintf(stderr, MSGSTR( TMMSG, TMMES), linebuf);  /* MSG */
					exit(2);
				}


				if((p = compsub(rep->r1.rhs, p)) == badp) {
					fprintf(stderr, MSGSTR( CGMSG, CGMES), linebuf);  /* MSG */
					exit(2);
				}

				if(gflag)
					rep->r1.gfl = GLOBAL_SUB;
				else
					rep->r1.gfl = 1;
				while (strspn(cp, "gpPw0123456789")) {
					if(*cp == 'g')
						rep->r1.gfl = GLOBAL_SUB;
					else if(*cp == 'p')
						rep->r1.pfl = 1;
					else if(*cp == 'P')
						rep->r1.pfl = 2;
					else if(*cp == 'w') {
						cp++;
						if(*cp++ !=  ' ') {
							fprintf(stderr, MSGSTR( CGMSG, CGMES), linebuf);  /* MSG */
							exit(2);
						}
						if(nfiles > 10) {
							fprintf(stderr, MSGSTR( FILECNT, "Too many files in w commands\n"));  /* MSG */
							exit(2);
						}

						text(fname[nfiles]);
						for(i = nfiles - 1; i >= 0; i--)
							if(cmp(fname[nfiles],fname[i]) == 0) {
								rep->r1.fcode = fcode[i];
								goto done;
							}
						if((rep->r1.fcode = fopen(fname[nfiles], "w")) == NULL) {
							fprintf(stderr, MSGSTR( FILEOPEN, "cannot open %s\n"), fname[nfiles]);  /* MSG */
							exit(2);
						}
						fcode[nfiles++] = rep->r1.fcode;
					break;
					}
					else {
						rep->r1.gfl = strtol(cp,&tp,10);
						if (rep->r1.gfl == 0) {
							fprintf(stderr, MSGSTR( CGMSG, CGMES), linebuf);  /* MSG */
							exit(2);
						}
						cp = --tp;
					}
					cp++;
				}
				break;

			case 'w':
				rep->r1.command = WCOM;
				if(*cp++ != ' ') {
					fprintf(stderr, MSGSTR( CGMSG, CGMES), linebuf);  /* MSG */
					exit(2);
				}
				if(nfiles > 10){
					fprintf(stderr, MSGSTR( FILECNT, "Too many files in w commands\n"));  /* MSG */
					exit(2);
				}

				text(fname[nfiles]);
				for(i = nfiles - 1; i >= 0; i--)
					if(cmp(fname[nfiles], fname[i]) == 0) {
						rep->r1.fcode = fcode[i];
						goto done;
					}

				if((rep->r1.fcode = fopen(fname[nfiles], "w")) == NULL) {
					fprintf(stderr, MSGSTR(CREATERR, "Cannot create %s\n"), fname[nfiles]);  /* MSG */
					exit(2);
				}
				fcode[nfiles++] = rep->r1.fcode;
				break;

			case 'x':
				rep->r1.command = XCOM;
				break;

			case 'y':
				rep->r1.command = YCOM;
				sseof = *cp++;

#ifdef KJI
				rep->r1.rhs = (NLchar *) 
				  malloc((NLCOLMAX+1)*sizeof(NLchar));
#else
				rep->r1.rhs = (NLchar *) 
				  malloc(NLCHARMAX*sizeof(NLchar));
#endif
				if (rep->r1.rhs == 0) {
					fprintf(stderr, 
						MSGSTR(MALLOCMSG, MALLOCMES));
					exit(2);
				}
				if(!ycomp(rep->r1.rhs)) {
					fprintf(stderr, 
						MSGSTR(CGMSG, CGMES), linebuf);
					exit(2);
				}
				break;

		}
done:
		if (rep->r1.command == SCOM && !rep->r1.re1)
			if (lastre)
				rep->r1.re1 = lastre;
			else {
				fprintf(stderr, MSGSTR(FRSTRE, "First RE may not be null\n"));  /* MSG */
				exit(2);
			}
		if(++rep >= ptrend) {
			fprintf(stderr, MSGSTR(CMNDCNT, "Too many commands, last: %s\n"), linebuf);  /* MSG */
			exit(2);
		}

		rep->r1.ad1 = p;

		if(*cp++ != '\0') {
			if(cp[-1] == ';')
				goto comploop;
			fprintf(stderr, MSGSTR(CGMSG, CGMES), linebuf);  /* MSG */
			exit(2);
		}

	}
	rep->r1.command = 0;
	lastre = op;
}

char    *compsub(rhsbuf,preserve)
NLchar    *rhsbuf;
char *preserve;
{

	register char   *q;
	register NLchar *p;

	p = rhsbuf;
	q = cp;
	for(;;) {
#if !defined(NLS) && !defined(KJI)
		if((*p = *q++) == '\\') {
			*p = *q++;
#else
#ifdef KJI
		*p = NCdechr(q);
		q += NLchrlen(q);

		if(*p == '\\') {
			*p = NCdechr(q);
			q += NLchrlen(q);

#else
		if((*p = (NLchar)*q++) == '\\') {
			*p = (NLchar)*q++;
#endif
#endif
			if(*p > nbra + '0' && *p <= '9')
				return(badp);

			*p++ |= 0x8000;
			continue;
		}
		if(*p == sseof) {
			*p++ = '\0';
			cp = q;

			rhsp = p;               /* update the rhsbuf pointer */
			return(preserve);
		}
		if(*p++ == '\0') {
			return(badp);
		}

	}
}

rline(lbuf)
char    *lbuf;
{
	register char   *p, *q;
	register        t;
	static char     *saveq;

	p = lbuf - 1;

	if(eflag) {
		if(eflag > 0) {
			eflag = -1;
			if(eargc-- <= 0)
				exit(2);
			q = *++eargv;

			while(*++p = *q++) {
				if(*p == '\\') {
					if((*++p = *q++) == '\0') {
						saveq = 0;
						return(-1);
					} else
						continue;
				}
				if(*p == '\n') {
					*p = '\0';
					saveq = q;
					return(1);
				}
			}
			saveq = 0;
			return(1);
		}
		if((q = saveq) == 0)    return(-1);


		while(*++p = *q++) {
			if(*p == '\\') {
				if((*++p = *q++) == '\0') {
					saveq = 0;
					return(-1);
				} else
					continue;
			}
			if(*p == '\n') {
				*p = '\0';
				saveq = q;
				return(1);
			}
		}
		saveq = 0;
		return(1);
	}

	while((t = getc(fin)) != EOF) {
		*++p = t;
		if(*p == '\\') {
			t = getc(fin);
			*++p = t;
		}
		else if(*p == '\n') {
			*p = '\0';
			return(1);
		}
	}
	return(-1);
}

char    *address(expbuf)
char    *expbuf;
{
	register char   *rcp;
	long    lno;

	if(*cp == '$') {
		cp++;
		*expbuf++ = CEND;
		*expbuf++ = CCEOF;
		return(expbuf);
	}
	if (*cp == '/' || *cp == '\\' ) {
		if ( *cp == '\\' )
			cp++;
		sseof = *cp++;
		return(comple((char *) 0, expbuf, reend, sseof));
	}

	rcp = cp;
	lno = 0;

	while(*rcp >= '0' && *rcp <= '9')
		lno = lno*10 + *rcp++ - '0';

	if(rcp > cp) {
		*expbuf++ = CLNUM;
		*expbuf++ = nlno;
		tlno[nlno++] = lno;
		if(nlno >= NLINES) {
			fprintf(stderr, MSGSTR( LINECNT, "Too many line numbers\n"));  /* MSG */
			exit(2);
		}
		*expbuf++ = CCEOF;
		cp = rcp;
		return(expbuf);
	}
	return(0);
}
cmp(a, b)
char    *a,*b;
{
	register char   *ra, *rb;

	ra = a - 1;
	rb = b - 1;

	while(*++ra == *++rb)
		if(*ra == '\0') return(0);
	return(1);
}

char    *text(textbuf)
char    *textbuf;
{
	register char   *p, *q;

	p = textbuf;
	q = cp;
	for(;;) {

		if((*p = *q++) == '\\')
			*p = *q++;
		if(*p == '\0') {
			cp = --q;
			return(++p);
		}
		p++;
	}
}


struct label    *search(ptr)
struct label    *ptr;
{
	struct label    *rp;

	rp = labtab;
	while(rp < ptr) {
		if(cmp(rp->asc, ptr->asc) == 0)
			return(rp);
		rp++;
	}

	return(0);
}


dechain()
{
	struct label    *lptr;
	union reptr     *rptr, *trptr;

	for(lptr = labtab; lptr < lab; lptr++) {

		if(lptr->address == 0) {
			fprintf(stderr, MSGSTR( UNDFNLBL, "Undefined label: %s\n"), lptr->asc);  /* MSG */
			exit(2);
		}

		if(lptr->chain) {
			rptr = lptr->chain;
			while(trptr = rptr->r2.lb1) {
				rptr->r2.lb1 = lptr->address;
				rptr = trptr;
			}
			rptr->r2.lb1 = lptr->address;
		}
	}
}

#if !defined(NLS) && !defined(KJI)
char *ycomp(expbuf)
char    *expbuf;
{
	register char   c, *ep, *tsp;
	char    *sp;

	ep = expbuf;
	sp = cp;
	for(tsp = cp; *tsp != sseof; tsp++) {
		if(*tsp == '\\')
			tsp++;
		if(*tsp == '\n')
			return(badp);
	}
	tsp++;

	while((c = *sp++ & 0177) != sseof) {
		if(c == '\\' && *sp == 'n') {
			sp++;
			c = '\n';
		}
		if((ep[c] = *tsp++) == '\\' && *tsp == 'n') {
			ep[c] = '\n';
			tsp++;
		}
		if(ep[c] == sseof || ep[c] == '\0')
			return(badp);
	}
	if(*tsp != sseof)
		return(badp);
	cp = ++tsp;

	for(c = 0; !(c & 0200); c++)
		if(ep[c] == 0)
			ep[c] = c;

	return(ep + 0200);
}
#else
static int ycomp(expbuf)
NLchar    *expbuf;
{
	register NLchar c;
	register NLchar *ep;
	register char   *tsp, *sp;

	ep = expbuf;
	sp = cp;

#ifdef KJI
	for(c = 0; c < NLCOLMAX; c++)
		ep[c] = 0;
#else
	for(c = 0; c < NLCHARMAX; c++)
		ep[c] = c;
#endif

	tsp = cp;
	do {
		if (*tsp == '\0') 
			return (0);
	} while (NLsgetc(tsp) != sseof);

	while ((c = NLsgetc(sp)) != sseof) {
		if (*tsp == '\0' || *tsp == sseof)
			return (0);
		if(c == '\\' && *sp == 'n') {
			sp++;
			c = '\n';
		}
#ifdef KJI
		c = _NCmap(c);
#endif
		ep[c] = NLsgetc(tsp);
		if(ep[c] == '\\' && *tsp == 'n') {
			ep[c] = '\n';
			tsp++;
		}
	}
	if(*tsp != sseof)
		return (0);
	cp = ++tsp;

	return (1);
}
#endif
