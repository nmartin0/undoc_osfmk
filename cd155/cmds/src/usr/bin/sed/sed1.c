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
static char rcsid[] = "@(#)$RCSfile: sed1.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 01:38:45 $";
#endif
/*
 * COMPONENT_NAME: (CMDEDIT) sed1.c
 *
 * FUNCTIONS: execute, match, substitute, dosub, place, command, gline, compile,
 * and arout.
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
 *  sed1.c  1.7  com/cmd/edit,3.1.2,9038312 8/21/90 08:32:55
 */

#include <stdio.h>

#include "sed.h"


#include <NLctype.h>
#include <NLregexp.h>
#define  NBRA		_NBRA	/* Needed for namespace-pollution cleanup */


#include "sed_msg.h"

extern nl_catd  catd;
#define MSGSTR(Num, Str) catgets(catd, MS_SED, Num, Str)


union reptr     *abuf[ABUFSIZE];
union reptr **aptr;
char    ibuf[512];
char    *cbp;
char    *ebp;
char    genbuf[LBSIZE];
char    *lbend;
char    *lcomend;
int     dolflag;
int     sflag;
int     jflag;
int     delflag;
long    lnum;
char    holdsp[LBSIZE+1];
char    *spend;
char    *hspend;
int     nflag;
long    tlno[NLINES];
int     f;
union reptr     *pending;
#if !defined(NLS) && !defined(KJI)
char    *trans[040]  = {
	"\\01",
	"\\02",
	"\\03",
	"\\04",
	"\\05",
	"\\06",
	"\\07",
	"-<",
	"->",
	"\n",
	"\\13",
	"\\14",
	"\\15",
	"\\16",
	"\\17",
	"\\20",
	"\\21",
	"\\22",
	"\\23",
	"\\24",
	"\\25",
	"\\26",
	"\\27",
	"\\30",
	"\\31",
	"\\32",
	"\\33",
	"\\34",
	"\\35",
	"\\36",
	"\\37"
};
char    rub[] = {"\\177"};
#endif

execute(file)
char *file;
{
	register char *p1, *p2;
	register union reptr    *ipc;
	int     c;
	char    *execp;

	if (file) {
		if ((f = open(file, 0)) < 0) {
			fprintf(stderr, MSGSTR(OPENERR, "Can't open %s\n"), file);  /* MSG */
		}
	} else
		f = 0;

	ebp = ibuf;
	cbp = ibuf;

	if(pending) {
		ipc = pending;
		pending = 0;
		goto yes;
	}

	for(;;) {
		if((execp = gline(linebuf)) == badp) {
			close(f);
			return;
		}
		spend = execp;

		for(ipc = ptrspace; ipc->r1.command; ) {

			p1 = ipc->r1.ad1;
			p2 = ipc->r1.ad2;

			if(p1) {

				if(ipc->r1.inar) {
					if(*p2 == CEND) {
						p1 = 0;
					} else if(*p2 == CLNUM) {
						c = p2[1];
						if(lnum > tlno[c]) {
							ipc->r1.inar = 0;
							if(ipc->r1.negfl)
								goto yes;
							ipc++;
							continue;
						}
						if(lnum == tlno[c]) {
							ipc->r1.inar = 0;
						}
					} else if(match(p2, 0)) {
						ipc->r1.inar = 0;
					}
				} else if(*p1 == CEND) {
					if(!dolflag) {
						if(ipc->r1.negfl)
							goto yes;
						ipc++;
						continue;
					}

				} else if(*p1 == CLNUM) {
					c = p1[1];
					if(lnum != tlno[c]) {
						if(ipc->r1.negfl)
							goto yes;
						ipc++;
						continue;
					}
					if(p2)
						ipc->r1.inar = 1;
				} else if(match(p1, 0)) {
					if(p2)
						ipc->r1.inar = 1;
				} else {
					if(ipc->r1.negfl)
						goto yes;
					ipc++;
					continue;
				}
				/*
				 * if range is invalid (ie 5,2) or 
				 * simple (ie 5,5),  we aren't in a range.
				 */
				if (p2 && (*p2 == CLNUM) && 
				    (lnum >= tlno[p2[1]]))
					ipc->r1.inar = 0;
			}

			if(ipc->r1.negfl) {
				ipc++;
				continue;
			}
	yes:
			command(ipc);

			if(delflag)
				break;

			if(jflag) {
				jflag = 0;
				if((ipc = ipc->r2.lb1) == 0) {
					ipc = ptrspace;
					break;
				}
			} else
				ipc++;

		}
		if(!nflag && !delflag) {
			for(p1 = linebuf; p1 < spend; p1++)
				putc(*p1, stdout);
			putc('\n', stdout);
		}

		if(aptr > abuf) {
			arout();
		}

		delflag = 0;

	}
}
match(expbuf, gf)
char    *expbuf;
{
	register char   *p1, *p2;

	if(gf) {
		if(*expbuf)     return(0);
		p1 = linebuf;
		p2 = genbuf;
		while(*p1++ = *p2++);
		p1 = (char *) loc2;
		locs =(char *) p1;
	} else {
		p1 = linebuf;
		locs = 0;
	}

	circf = *expbuf++;
	return(step( (char *) p1, (char *) expbuf));
}

substitute(ipc)
union reptr     *ipc;
{
	register char   *p1, *p2;
	int scount, sdone;
	scount = sdone = 0;

	p1 = linebuf;
	p2 = genbuf;
	while(*p2++ = *p1++);

	while(match(ipc->r1.re1, scount++)) {
		if(ipc->r1.gfl == GLOBAL_SUB || ipc->r1.gfl == scount) {
			sdone++;
			dosub(ipc->r1.rhs);
			if (ipc->r1.gfl != GLOBAL_SUB && scount >= ipc->r1.gfl)
				break;
		}
		if (!*loc2)
			break;
	}
	if (sdone)
		sflag = 1;
	return(sdone);
}

dosub(rhsbuf)

NLchar  *rhsbuf;
{

	register char *lp, *sp;
	register NLchar *rp;
#ifdef KJI
	wchar_t c;
#else
	int c;
#endif

	lp = linebuf;
	sp = genbuf;
	rp = rhsbuf;
	while (lp <(char *)  loc1)
		*sp++ = *lp++;
	while(c = *rp++) {
		if (c == '&') {
			sp = place(sp, loc1, loc2);
			continue;
#if !defined(NLS) && !defined(KJI)
		} else if (c&0200 && (c &= 0177) >= '1' && c < NBRA+'1') {
#else
#ifdef KJI
		} /* the high order bit is legal in kanji, test */
		  /* instead for 0x8000 which is an illegal kanji char */
		  else if (((c&0xff00) == 0x8000) &&
				       (c &= 0377) >= '1' && c < NBRA+'1') {
#else
		} else if (c&0x8000 && (c &= 0377) >= '1' && c < NBRA+'1') {
#endif
#endif
			sp = place(sp, braslist[c-'1'], braelist[c-'1']);
			continue;
		}
#if !defined(NLS) && !defined(KJI)
		*sp++ = c&0177;
#else
#ifdef KJI
		sp += NCenc(&c, sp);
#else
		*sp++ = c&0377;
#endif
#endif
		if (sp >= &genbuf[LBSIZE])
			fprintf(stderr, MSGSTR( LINELEN, "Output line too long.\n"));  /* MSG */
	}
	lp = (char *) loc2;
	loc2 = (char *)( sp - genbuf + linebuf);
	while (*sp++ = *lp++)
		if (sp >= &genbuf[LBSIZE]) {
			fprintf(stderr, MSGSTR( LINELEN, "Output line too long.\n"));  /* MSG */
		}
	lp = linebuf;
	sp = genbuf;
	while (*lp++ = *sp++);
	spend = lp-1;
}
char    *place(asp, al1, al2)
char    *asp, *al1, *al2;
{
	register char *sp, *l1, *l2;

	sp = asp;
	l1 = al1;
	l2 = al2;
	while (l1 < l2) {
		*sp++ = *l1++;
		if (sp >= &genbuf[LBSIZE])
			fprintf(stderr, MSGSTR( LINELEN, "Output line too long.\n"));  /* MSG */
	}
	return(sp);
}

command(ipc)
union reptr     *ipc;
{
	register int    i;
	register char   *p1, *p2, *p3;
	char    *execp;

	int col;



	switch(ipc->r1.command) {

		case ACOM:
			*aptr++ = ipc;
			if(aptr >= &abuf[ABUFSIZE]) {
				fprintf(stderr, MSGSTR( APPNDCNT, "Too many appends after line %ld\n"), lnum);  /* MSG */
			}
			*aptr = 0;
			break;

		case CCOM:
			delflag = 1;
			if(!ipc->r1.inar || dolflag) {
				for(p1 = ipc->r1.re1; *p1; )
					putc(*p1++, stdout);
				putc('\n', stdout);
			}
			break;
		case DCOM:
			delflag++;
			break;
		case CDCOM:
			p1 = p2 = linebuf;

			while(*p1 != '\n') {
				if(*p1++ == 0) {
					delflag++;
					return;
				}
			}

			p1++;
			while(*p2++ = *p1++);
			spend = p2-1;
			jflag++;
			break;

		case EQCOM:
			fprintf(stdout, "%ld\n", lnum);
			break;

		case GCOM:
			p1 = linebuf;
			p2 = holdsp;
			while(*p1++ = *p2++);
			spend = p1-1;
			break;

		case CGCOM:
			if (spend > linebuf) {
				*spend++ = '\n';
			}
			if (spend + (hspend - holdsp) > lbend) {
				fprintf(stderr, MSGSTR(PATTSPC, "Pattern space overflow\n" ));  /* MSG */
				exit(2);
			}
			p1 = spend;
			p2 = holdsp;
			while(*p1++ = *p2++);
			spend = p1-1;
			break;

		case HCOM:
			p1 = holdsp;
			p2 = linebuf;
			while(*p1++ = *p2++);
			hspend = p1-1;
			break;

		case CHCOM:
			if (hspend > holdsp) {
				*hspend++ = '\n';
			}
			if (hspend + (spend - linebuf) > hend) {
				fprintf(stderr, MSGSTR(HOLDSPC, "Hold space overflow\n"));  /* MSG */
				exit(2);
			}
			p1 = hspend;
			p2 = linebuf;
			while(*p1++ = *p2++);
			hspend = p1-1;
			break;

		case ICOM:
			for(p1 = ipc->r1.re1; *p1; )
				putc(*p1++, stdout);
			putc('\n', stdout);
			break;

		case BCOM:
			jflag = 1;
			break;


		case LCOM:
			p1 = linebuf;
			p2 = genbuf;
#if !defined(NLS) && !defined(KJI)
			genbuf[72] = 0;
			while(*p1)
				if(*p1 >= 040) {
					if(*p1 == 0177) {
						p3 = rub;
						while(*p2++ = *p3++)
							if(p2 >= lcomend) {
								*p2 = '\\';
								fprintf(stdout, "%s\n", genbuf);
								p2 = genbuf;
							}
						p2--;
						p1++;
						continue;
					}
					*p2++ = *p1++;
					if(p2 >= lcomend) {
						*p2 = '\\';
						fprintf(stdout, "%s\n", genbuf);
						p2 = genbuf;
					}
				} else {
					p3 = trans[*p1-1];
					while(*p2++ = *p3++)
						if(p2 >= lcomend) {
							*p2 = '\\';
							fprintf(stdout, "%s\n", genbuf);
							p2 = genbuf;
						}
					p2--;
					p1++;
				}
#else
			col = 0;
			while (*p1) {
				int c = NLsgetc(p1);
				if (c == '\n') {
					*p2++ = c;
					col = 0;
				} else {
#ifdef KJI
					int chrwid = (c=='\b'||c=='\t' ? 1 : !NCisprint(c) ? 6 : c>0xff ? 2 : 1);
#else
					int chrwid = (c=='\b'||c=='\t' ? 1 : !NCisprint(c) ? 6 : 1);
#endif
					if (col+chrwid >= 72) {
						*p2 = '\0';
						fprintf(stdout, "%s\\\n", genbuf);
						p2 = genbuf;
						col = 0;
					}
					col += chrwid;
					if (c == '\b') {
						*p2++ = '-';  *p2++ = '\b';  *p2++ = '<';
					} else if (c == '\t') {
						*p2++ = '-';  *p2++ = '\b';  *p2++ = '>';
					} else if (NCisprint(c)) {
						NLsputc(p2, c);
					} else {
						p2 += sprintf(p2, "\\x%.4x", c);
					}
				}
			}
#endif
			*p2 = 0;
			fprintf(stdout, "%s\n", genbuf);
			break;

		case NCOM:
			if(!nflag) {
				for(p1 = linebuf; p1 < spend; p1++)
					putc(*p1, stdout);
				putc('\n', stdout);
			}

			if(aptr > abuf)
				arout();
			if((execp = gline(linebuf)) == badp) {
				pending = ipc;
				delflag = 1;
				break;
			}
			spend = execp;

			break;
		case CNCOM:
			if(aptr > abuf)
				arout();
			*spend++ = '\n';
			if((execp = gline(spend)) == badp) {
				pending = ipc;
				delflag = 1;
				break;
			}
			spend = execp;
			break;

		case PCOM:
			for(p1 = linebuf; p1 < spend; p1++)
				putc(*p1, stdout);
			putc('\n', stdout);
			break;
		case CPCOM:
	cpcom:
			for(p1 = linebuf; *p1 != '\n' && *p1 != '\0'; )
				putc(*p1++, stdout);
			putc('\n', stdout);
			break;

		case QCOM:
			if(!nflag) {
				for(p1 = linebuf; p1 < spend; p1++)
					putc(*p1, stdout);
				putc('\n', stdout);
			}
			if(aptr > abuf) arout();
			fclose(stdout);
			exit(0);
		case RCOM:

			*aptr++ = ipc;
			if(aptr >= &abuf[ABUFSIZE])
				fprintf(stderr, MSGSTR( READCNT, "Too many reads after line%ld\n"),  /* MSG */
					lnum);

			*aptr = 0;

			break;

		case SCOM:
			i = substitute(ipc);
			if(ipc->r1.pfl && nflag && i)
				if(ipc->r1.pfl == 1) {
					for(p1 = linebuf; p1 < spend; p1++)
						putc(*p1, stdout);
					putc('\n', stdout);
				}
				else
					goto cpcom;
			if(i && ipc->r1.fcode)
				goto wcom;
			break;

		case TCOM:
			if(sflag == 0)  break;
			sflag = 0;
			jflag = 1;
			break;

		wcom:
		case WCOM:
			fprintf(ipc->r1.fcode, "%s\n", linebuf);
			break;
		case XCOM:
			p1 = linebuf;
			p2 = genbuf;
			while(*p2++ = *p1++);
			p1 = holdsp;
			p2 = linebuf;
			while(*p2++ = *p1++);
			spend = p2 - 1;
			p1 = genbuf;
			p2 = holdsp;
			while(*p2++ = *p1++);
			hspend = p2 - 1;
			break;

		case YCOM:
			p1 = linebuf;

			{
			NLchar c, tc, *rhsp;
			rhsp = ipc->r1.rhs;
			p3 = genbuf;
			do {
				c = NLsgetc(p1);
#ifdef KJI
				if ((tc = rhsp[_NCmap(c)]) == '\0') tc = c;
#else
				tc = rhsp[c];
#endif
				NLsputc(p3, tc);
				if (p3 >= &genbuf[LBSIZE]) {
					fprintf(stderr, MSGSTR( LINELEN, "output line too long.\n"));  /* MSG */
					p3[-NCchrlen(tc)] = '\0';
					break;
				}
			} while(tc);

			p3=genbuf;
			p1=linebuf;
			while(*p1++ = *p3++);
			spend = p1-1;
			}
			break;
	}

}

char    *gline(addr)
char    *addr;
{
	register char   *p1, *p2;
	register        c;
	p1 = addr;
	p2 = cbp;
	for (;;) {
		if (p2 >= ebp) {
			if ((c = read(f, ibuf, 512)) <= 0) {
				return(badp);
			}
			p2 = ibuf;
			ebp = ibuf+c;
		}
		if ((c = *p2++) == '\n') {
			if(p2 >=  ebp) {
				if((c = read(f, ibuf, 512)) <= 0) {
					close(f);
					if(eargc == 0)
							dolflag = 1;
				}

				p2 = ibuf;
				ebp = ibuf + c;
			}
			break;
		}
		if(c)
		if(p1 < lbend)
			*p1++ = c;
	}
	lnum++;
	sflag = 0;
	*p1 = 0;
	cbp = p2;

	return(p1);
}

char *comple(x1, ep, x3, x4)
char *x1, *x3;
char x4;
register char *ep;
{
	register char *p;

	p = compile( (char *) x1, (char *) ( ep + 1), (char *)  x3, (int) x4);
	if(p == ep + 1)
		return(ep);
	*ep = circf;
	return(p);
}

arout()
{
	register char   *p1;
	FILE    *fi;
	char    c;
	int     t;

	aptr = abuf - 1;
	while(*++aptr) {
		if((*aptr)->r1.command == ACOM) {
			for(p1 = (*aptr)->r1.re1; *p1; )
				putc(*p1++, stdout);
			putc('\n', stdout);
		} else {
			if((fi = fopen((*aptr)->r1.re1, "r")) == NULL)
				continue;
			while((t = getc(fi)) != EOF) {
				c = t;
				putc(c, stdout);
			}
			fclose(fi);
		}
	}
	aptr = abuf;
	*aptr = 0;
}
