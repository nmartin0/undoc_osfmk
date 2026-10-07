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
static char *rcsid = "@(#)$RCSfile: ex_set.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:24:03 $";
#endif
/*
 * COMPONENT_NAME: (CMDEDIT) ex_set.c
 *
 * FUNCTION: prall, propt, propts, set, setend, seteq
 *
 * ORIGINS: 3, 10, 13, 26, 27
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
 * ex_set.c  1.4  com/cmd/edit/vi,3.1,9013 12/20/89 10:57:19
 * 
 * Copyright (c) 1981 Regents of the University of California
 * 
 */
/* Copyright (c) 1981 Regents of the University of California */

#include "ex.h"
#include "ex_temp.h"
#include "ex_tty.h"

static prall();
static propts();
static propt();

/*
 * seteq compares a (NLchar *) to a (char *)
 */
#define nseq(a,b)		((a) && (b) && seteq(a,b) == 0)
#define sneq(a,b)		((a) && (b) && seteq(b,a) == 0)

static seteq(n,s)
register NLchar * n;
register char *s;
{
    while (*n == *s++)
	if (*n++ == 0)
	    return 0;
    return (*n - *--s);
}

/*
 * Set command.
 */
static NLchar	optname[ONMSZ];

set()
{
	register NLchar *cp;
	register struct option *op;
	register int c;
	short no;
	extern short ospeed;

	setnoaddr();
	if (skipend()) {
		if (peekchar() != EOF)
			ignchar();
		propts();
		return;
	}
	do {
		cp = optname;
		do {
			if (cp < &optname[ONMSZ - 2])
				*cp++ = ex_getchar();
		} while (NCisalnum(peekchar()));
		*cp = 0;
		cp = optname;
		if (sneq("all", cp)) {
			if (inopen)
				pofix();
			prall();
			goto next;
		}
		no = 0;
		if (cp[0] == 'n' && cp[1] == 'o') {
			cp += 2;
			no++;
		}
		/* Implement w300, w1200, and w9600 specially */
		if (nseq(cp, "w300")) {
			if (ospeed >= B1200) {
dontset:
				ignore(ex_getchar());	/* = */
				ignore(getnum());	/* value */
				continue;
			}
			NCdecstr("window",cp,ONMSZ);
		} else if (nseq(cp, "w1200")) {
			if (ospeed < B1200 || ospeed >= B2400)
				goto dontset;
			NCdecstr("window",cp,ONMSZ);
		} else if (nseq(cp, "w9600")) {
			if (ospeed < B2400)
				goto dontset;
			NCdecstr("window",cp,ONMSZ);
		}
		for (op = options; op < &options[NOPTS]; op++)
			if (sneq(op->oname, cp) || op->oabbrev && sneq(op->oabbrev, cp))
				break;
		if (op->oname == 0) {
			if (nseq(cp, "vice")) 
				serror(msg(M_302, "Cannot set novice interactively - invoke vedit"));
			else 
				serror(msg(M_142, "%S: No such option@- 'set all' gives all option values"), cp);
		}
		c = skipwh();
		if (peekchar() == '?') {
			ignchar();
printone:
			propt(op);
			noonl();
			goto next;
		}
		if (op->otype == ONOFF) {
			op->ovalue = 1 - no;
			if (op == &options[PROMPT])
				oprompt = 1 - no;
			goto next;
		}
		if (no)
			serror(msg(M_143, "Option %s is not a toggle"), op->oname);
		if (c != 0 || setend())
			goto printone;
		if (ex_getchar() != '=')
			serror(msg(M_144, "Missing =@in assignment to option %s"), op->oname);
		switch (op->otype) {

		case NUMERIC:
			if (!(isascii(peekchar()) && isdigit(peekc)))
				error(msg(M_145, "Digits required@after ="));
			op->ovalue = getnum();
			if (value(TABSTOP) <= 0){
			    value(TABSTOP) = TABS;
			    serror(msg(M_514,
				"Bad value@for %s option -- reset to default"),
				"tabstop");
			}
			if (value(HARDTABS) <= 0){
			    value(HARDTABS) = TABS;
			    serror(msg(M_514,
				"Bad value@for %s option -- reset to default"),
				"hardtabs");
			}
			if (value(SHIFTWIDTH) <= 0) {
			    value(SHIFTWIDTH) = TABS;
			    serror(msg(M_514,
				"Bad value@for %s option -- reset to default"),
				"shiftwidth");
			}
			if (op == &options[WINDOW]) {
				if (value(WINDOW) >= lines)
					value(WINDOW) = lines-1;
				vsetsiz(value(WINDOW));
			}
			break;

		case STRING:
		case OTERM:
#if defined(KJI)
		case CLOSEP:	/* closing punctuation */
		case ONECOL:	/* partial character indicator */
		case WRAP:	/* wraptype */
#endif
		  {
			char s_type[ONMSZ];	/* terminal type */

			cp = optname;
			while (!setend()) {
				if (cp >= &optname[ONMSZ])
					error(msg(M_146, "String too long@in option assignment"));
				/* adb change:  allow whitepace in strings */
				if( (*cp = ex_getchar()) == '\\')
					if( peekchar() != EOF)
						*cp = ex_getchar();
				cp++;
			}
			*cp = 0;
			NCencstr(optname,s_type,ONMSZ);
			if (op->otype == OTERM) {
/*
 * At first glance it seems like we shouldn't care if the terminal type
 * is changed inside visual mode, as long as we assume the screen is
 * a mess and redraw it. However, it's a much harder problem than that.
 * If you happen to change from 1 crt to another that both have the same
 * size screen, it's OK. But if the screen size if different, the stuff
 * that gets initialized in vop() will be wrong. This could be overcome
 * by redoing the initialization, e.g. making the first 90% of vop into
 * a subroutine. However, the most useful case is where you forgot to do
 * a setenv before you went into the editor and it thinks you're on a dumb
 * terminal. Ex treats this like hardcopy and goes into HARDOPEN mode.
 * This loses because the first part of vop calls oop in this case.
 * The problem is so hard I gave up. I'm not saying it can't be done,
 * but I am saying it probably isn't worth the effort.
 */
				if (inopen)
error(msg(M_147, "Can't change type of terminal from within open/visual"));
				setterm(s_type);
			} else {
#if defined(KJI)
				if (op->otype == WRAP)
					switch(s_type[0]) {
					case 'f':
					case 'g':
					case 'r':
					case 'w':
						break;
					default:
error(msg(M_290, "f(lexible), g(eneral), r(igid), or w(ord) required@after ="));
						break;
					}
				else if (op->otype == ONECOL) {
					NLchar nc = NCdechr(s_type);

					/* PARTIALCHAR must be a printable single col char */
					if 	(is_dblwid(nc) || !is_print(nc))
error(msg(M_289, "partial character indicator should display in one column"));
				}
#endif
				if (op->otype != CLOSEP) {
					strcpy(op->osvalue, s_type);
				} else {
					char *p = s_type ;
					char *q = op->osvalue ;
					int i;

					/* multi-character punctuation is from 1 to 9 chars */
					while (*p) {
						if (*p >= '1' && *p <= '9') {
							i = *p - '0' + 1;
							/* copy and verify multi-char punctuation count */
							while (i--)
								if (c = NLsgetc(p))
									NLsputc(q,c);
								else
error(msg(M_293, "fewer characters than specified"));
						} else {
							*q++ = *p++;
						}
					}
					*q = '\0';
				}
				op->odefault = 1;
			}
			break;
		  }
		}
next:
		flush();
	} while (!skipend());
	eol();
}

setend()
{

	return (iswhite(peekchar()) || endcmd(peekchar()));
}

static prall()
{
	register int incr = (NOPTS + 2) / 3;
	register int rows = incr;
	register struct option *op = options;

	for (; rows; rows--, op++) {
		propt(op);
		gotab(24);
		propt(&op[incr]);
		if (&op[2*incr] < &options[NOPTS]) {
			gotab(56);
			propt(&op[2 * incr]);
		}
		putNFL();
	}
}

static propts()
{
	register struct option *op;

	for (op = options; op < &options[NOPTS]; op++) {
		if (op == &options[TTYTYPE])
			continue;
		switch (op->otype) {

		case ONOFF:
		case NUMERIC:
			if (op->ovalue == op->odefault)
				continue;
			break;

#if defined(KJI)
		case CLOSEP:
		case ONECOL:
		case WRAP:
#endif
		case STRING:
			if (op->odefault == 0)
				continue;
			break;
		}
		propt(op);
		ex_putchar(' ');
	}
	noonl();
	flush();
}

static propt(op)
	register struct option *op;
{
	register char *name;
	
	name = op->oname;

	switch (op->otype) {

	case ONOFF:
		ex_printf("%s%s", op->ovalue ? "" : "no", name);
		break;

	case NUMERIC:
		ex_printf("%s=%d", name, op->ovalue);
		break;

#if defined(KJI)
	case CLOSEP:
	case ONECOL:
	case WRAP:
#endif
	case STRING:
	case OTERM:
		ex_printf("%s=%s", name, op->osvalue);
		break;
	}
}
