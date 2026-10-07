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
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: tran.c,v $
 * Revision 1.2  1994/11/19  01:31:39  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/06/18  14:47:43  shala
 * Files for new version of awk command.
 *
 * Revision 2.5  90/12/06  14:10:29  devrcs
 * 	Fixed up comment headers.
 * 	[90/12/03  13:32:31  gm]
 * 
 * Revision 2.4  90/09/13  12:11:52  devrcs
 * 	Cleanup, RCS header, NLS, MSG
 * 	[90/08/29  10:01:15  knight]
 * 
 * Revision 2.3  90/07/05  23:26:14  devrcs
 * 	Use AIX source
 * 	[90/06/29  21:50:11  lehotsky]
 * 
 * $EndLog$
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: tran.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:31:39 $";
#endif
/*
 * COMPONENT_NAME: (CMDEDIT) tran.c
 *
 * FUNCTIONS: Calloc, Free, Malloc, arginit, freeelem, freesymtab, funnyvar,
 * hash, lookup, makesymtab, qstring, r_getfval, r_getsval, rehash, setfval,
 * setsval, setsymtab, syminit, and tostring
 *
 * ORIGINS: 3, 10, 27
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
 * Copyright (c) 1984, 19885, 1986, 1987 AT&T
 *      All Rights Reserved
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T.
 *
 * The copyright notice above does not evidence any
 * actual or intended publication of such source code.
 *
 * tran.c  1.5  com/cmd/edit/awk,3.1,9021 3/14/90 17:50:03
 */

/*
Copyright (c) 1984, 19885, 1986, 1987 AT&T
        All Rights Reserved

THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T.

The copyright notice above does not evidence any
actual or intended publication of such source code.
*/

#include <stdio.h>
#include "awk.h"
#include "awk.g.h"


#define FULLTAB 2       /* rehash when table gets this x full */
#define GROWTAB 4       /* grow table by this factor */

#ifdef KJI
#define SUBSEP_CHAR "\034"
#else
#define SUBSEP_CHAR "\031"
#endif

Array   *symtab;        /* main symbol table */

uuchar  **FS;           /* initial field sep */
uuchar  **RS;           /* initial record sep */
uuchar  **OFS;          /* output field sep */
uuchar  **ORS;          /* output record sep */
uuchar  **OFMT;         /* output format for numbers*/
Awkfloat *NF;           /* number of fields in current record */
Awkfloat *NR;           /* number of current record */
Awkfloat *FNR;          /* number of current record in current file */
uuchar  **FILENAME;     /* current filename argument */
Awkfloat *ARGC;         /* number of arguments from command line */
uuchar  **SUBSEP;       /* subscript separator for a[i,j,k]; default \034 */
Awkfloat *RSTART;       /* start of re matched with ~; origin 1 (!) */
Awkfloat *RLENGTH;      /* length of same */

Cell    *recloc;        /* location of record */
Cell    *nrloc;         /* NR */
Cell    *nfloc;         /* NF */
Cell    *fnrloc;        /* FNR */
Array   *ARGVtab;       /* symbol table containing ARGV[...] */
Cell    *rstartloc;     /* RSTART */
Cell    *rlengthloc;    /* RLENGTH */
Cell    *symtabloc;     /* SYMTAB */

Cell    *nullloc;
Node    *nullnode;      /* zero&null, converted into a node for comparisons */

void *Malloc();
extern Node *valtonode();
extern Cell fldtab[];

syminit()
{
        symtab = makesymtab(NSYMTAB);
        setsymtab("0", "0", 0.0, NUM|STR|CON, symtab);
        /* this is used for if(x)... tests: */
        nullloc = setsymtab(MSGSTR(YZERONULL, "$zero&null"), "", 0.0, NUM|STR|CON, symtab);
        nullnode = valtonode(nullloc, CCON);
        /* recloc = setsymtab("$0", record, 0.0, REC|STR|DONTFREE, symtab); */
        recloc = &fldtab[0];
        FS = &setsymtab("FS", " ", 0.0, STR, symtab)->sval;
        RS = &setsymtab("RS", "\n", 0.0, STR, symtab)->sval;
        OFS = &setsymtab("OFS", " ", 0.0, STR, symtab)->sval;
        ORS = &setsymtab("ORS", "\n", 0.0, STR, symtab)->sval;
        OFMT = &setsymtab("OFMT", "%.6g", 0.0, STR, symtab)->sval;
        FILENAME = &setsymtab("FILENAME", "", 0.0, STR, symtab)->sval;
        nfloc = setsymtab("NF", "", 0.0, NUM, symtab);
        NF = &nfloc->fval;
        nrloc = setsymtab("NR", "", 0.0, NUM, symtab);
        NR = &nrloc->fval;
        fnrloc = setsymtab("FNR", "", 0.0, NUM, symtab);
        FNR = &fnrloc->fval;
        SUBSEP = &setsymtab("SUBSEP", SUBSEP_CHAR, 0.0, STR, symtab)->sval;
        rstartloc = setsymtab("RSTART", "", 0.0, NUM, symtab);
        RSTART = &rstartloc->fval;
        rlengthloc = setsymtab("RLENGTH", "", 0.0, NUM, symtab);
        RLENGTH = &rlengthloc->fval;
        symtabloc = setsymtab("SYMTAB", "", 0.0, ARR, symtab);
        symtabloc->sval = (uuchar *) symtab;
}

arginit(ac, av)
        int ac;
        uuchar *av[];
{
        Cell *cp;
        Array *makesymtab();
        int i;
        uuchar temp[5];

        ARGC = &setsymtab("ARGC", "", (Awkfloat) ac, NUM, symtab)->fval;
        cp = setsymtab("ARGV", "", 0.0, ARR, symtab);
        ARGVtab = makesymtab(NSYMTAB);  /* could be (int) ARGC as well */
        cp->sval = (uuchar *) ARGVtab;
        for (i = 0; i < ac; i++) {
	  	sprintf( (char*) temp, "%d", i);
                if (isnumber(*av))
                        setsymtab(temp, *av, atof(*av), STR|NUM, ARGVtab);
                else
                        setsymtab(temp, *av, 0.0, STR, ARGVtab);
                av++;
        }
}

Array *makesymtab(n)
        int n;
{
        Array *ap;
        Cell **tp;

        ap = (Array *) Malloc(sizeof(Array));
        tp = (Cell **) Calloc(n, sizeof(Cell *));
        if (ap == NULL || tp == NULL)
                ERROR MSGSTR(TSPCMKSYMTAB, "out of space in makesymtab") FATAL;
        ap->nelem = 0;
        ap->size = n;
        ap->tab = tp;
        return(ap);
}

freesymtab(ap)  /* free symbol table */
        Cell *ap;
{
        Cell *cp;
        Array *tp;
        int i;

        if (!isarr(ap))
                return;
        tp = (Array *) ap->sval;
        if (tp == NULL)
                return;
        for (i = 0; i < tp->size; i++) {
                for (cp = tp->tab[i]; cp != NULL; cp = cp->cnext) {
                        xfree(cp->nval);
                        xfree(cp->sval);
                        Free(cp);
                }
        }
        Free(tp->tab);
        Free(tp);
}

freeelem(ap, s)         /* free elem s from ap (i.e., ap["s"] */
        Cell *ap;
        uuchar *s;
{
        Array *tp;
        Cell *p, *prev = NULL;
        int h;

        tp = (Array *) ap->sval;
        h = hash(s, tp->size);
        for (p = tp->tab[h]; p != NULL; prev = p, p = p->cnext)
                if (strcmp(s, p->nval) == 0) {
                        if (prev == NULL)       /* 1st one */
                                tp->tab[h] = p->cnext;
                        else                    /* middle somewhere */
                                prev->cnext = p->cnext;
                        if (freeable(p))
                                xfree(p->sval);
                        Free(p->nval);
                        Free(p);
                        tp->nelem--;
                        return;
                }
}

Cell *setsymtab(n, s, f, t, tp)
        uuchar *n, *s;
        Awkfloat f;
        unsigned t;
        Array *tp;
{
        register h;
        register Cell *p;
        Cell *lookup();

        if (n != NULL && (p = lookup(n, tp)) != NULL) {
                dprintf("setsymtab found %o: n=%s", p, p->nval, NULL);
                dprintf(" s=\"%s\" f=%g t=%o\n", p->sval, p->fval, p->tval);
                return(p);
        }
        p = (Cell *) Malloc(sizeof(Cell));
        if (p == NULL)
                ERROR MSGSTR(TSYMTABOFLW, "symbol table overflow at %s"), n FATAL;
        p->nval = tostring(n);
        p->sval = s ? tostring(s) : tostring("");
        p->fval = f;
        p->tval = t;
        tp->nelem++;
        if (tp->nelem > FULLTAB * tp->size)
                rehash(tp);
        h = hash(n, tp->size);
        p->cnext = tp->tab[h];
        tp->tab[h] = p;
        dprintf("setsymtab set %o: n=%s", p, p->nval, NULL);
        dprintf(" s=\"%s\" f=%g t=%o\n", p->sval, p->fval, p->tval);
        return(p);
}

hash(s, n)      /* form hash value for string s */
        register uuchar *s;
        int n;
{
        register unsigned hashval;

        for (hashval = 0; *s != '\0'; s++)
                hashval = (*s + 31 * hashval);
        return hashval % n;
}

rehash(tp)      /* rehash items in small table into big one */
        Array *tp;
{
        int i, nh, nsz;
        Cell *cp, *op, **np;

        nsz = GROWTAB * tp->size;
        np = (Cell **) Calloc(nsz, sizeof(Cell *));
        if (np == NULL)
                ERROR MSGSTR(TSPCREHASH, "out of space in rehash") FATAL;
        for (i = 0; i < tp->size; i++) {
                for (cp = tp->tab[i]; cp; cp = op) {
                        op = cp->cnext;
                        nh = hash(cp->nval, nsz);
                        cp->cnext = np[nh];
                        np[nh] = cp;
                }
        }
        free(tp->tab);
        tp->tab = np;
        tp->size = nsz;
}

Cell *lookup(s, tp)     /* look for s in tp */
        register uuchar *s;
        Array *tp;
{
        register Cell *p, *prev = NULL;         /* ? note that prev is unused */
        int h;

        h = hash(s, tp->size);
        for (p = tp->tab[h]; p != NULL; prev = p, p = p->cnext)
                if (strcmp(s, p->nval) == 0)
                        return(p);      /* found it */
        return(NULL);                   /* not found */
}

Awkfloat setfval(vp, f)
        register Cell *vp;
        Awkfloat f;
{
        if ((vp->tval & (NUM | STR)) == 0)
                funnyvar(vp, MSGSTR(TASGNTO, "assign to"));
        if (vp->tval & FLD) {
                donerec = 0;    /* mark $0 invalid */
                if (vp-fldtab > *NF)
                        newfld(vp-fldtab);
                dprintf("setting field %d to %g\n", vp-fldtab, f);
        } else if (vp->tval & REC) {
                donefld = 0;    /* mark $1... invalid */
                donerec = 1;
        }
        vp->tval &= ~STR;       /* mark string invalid */
        vp->tval |= NUM;        /* mark number ok */
        dprintf("setfval %o: %s = %g, t=%o\n", vp, vp->nval, f, vp->tval);
        return vp->fval = f;
}

funnyvar(vp, rw)
        Cell *vp;
        char *rw;
{
        if (vp->tval & ARR)
                ERROR MSGSTR(TCANTARR, "can't %s %s; it's an array name."), rw, vp->nval FATAL;
        if (vp->tval & FCN)
                ERROR MSGSTR(TCANTFN, "can't %s %s; it's a function."), rw, vp->nval FATAL;
        ERROR MSGSTR(TFUNNYVAR, "funny variable %o: n=%s s=\")%s\" f=%g t=%o"),
                vp, vp->nval, vp->sval, vp->fval, vp->tval);
}

uuchar *setsval(vp, s)
register Cell *vp;
uuchar *s;
{
        if ((vp->tval & (NUM | STR)) == 0)
                funnyvar(vp, MSGSTR(TASGNTO, "assign to"));
        if (vp->tval & FLD) {
                donerec = 0;    /* mark $0 invalid */
                if (vp-fldtab > *NF)
                        newfld(vp-fldtab);
                dprintf("setting field %d to %s\n", vp-fldtab, s);
        } else if (vp->tval & REC) {
                donefld = 0;    /* mark $1... invalid */
                donerec = 1;
        }
        vp->tval &= ~NUM;
        vp->tval |= STR;
        if (!(vp->tval&DONTFREE))
                xfree(vp->sval);
        vp->tval &= ~DONTFREE;
        dprintf("setsval %o: %s = \"%s\", t=%o\n", vp, vp->nval, s, vp->tval);
        return(vp->sval = tostring(s));
}

Awkfloat r_getfval(vp)
register Cell *vp;
{
#ifdef CAREFUL
        if (vp->tval & ARR)
                ERROR MSGSTR(TILLREF, "illegal reference to array %s"), vp->nval FATAL;
                return 0.0;
#endif
        if ((vp->tval & (NUM | STR)) == 0)
                funnyvar(vp, MSGSTR(TREADVAL, "read value of"));
        if ((vp->tval & FLD) && donefld == 0)
                fldbld();
        else if ((vp->tval & REC) && donerec == 0)
                recbld();
        if (!isnum(vp)) {       /* not a number */
                vp->fval = atof(vp->sval);      /* best guess */
                if (isnumber(vp->sval) && !(vp->tval&CON))
                        vp->tval |= NUM;        /* make NUM only sparingly */
        }
        dprintf("getfval %o: %s = %g, t=%o\n", vp, vp->nval, vp->fval, vp->tval);
        return(vp->fval);
}

uuchar *r_getsval(vp)
register Cell *vp;
{
        uuchar s[100];

#ifdef CAREFUL
        if (vp->tval & ARR)
                ERROR MSGSTR(TILLREF, "illegal reference to array %s"), vp->nval FATAL;
                return "";
#endif
        if ((vp->tval & (NUM | STR)) == 0)
                funnyvar(vp, MSGSTR(TREADVAL, "read value of"));
        if ((vp->tval & FLD) && donefld == 0)
                fldbld();
        else if ((vp->tval & REC) && donerec == 0)
                recbld();
        if ((vp->tval & STR) == 0) {
                if (!(vp->tval&DONTFREE))
                        xfree(vp->sval);
                if ((long)vp->fval == vp->fval)
                        sprintf( (char*)s, "%.20g", vp->fval);
                else
                        sprintf((char*)s, (char *)*OFMT, vp->fval);
                vp->sval = tostring(s);
                vp->tval &= ~DONTFREE;
                vp->tval |= STR;
        }
        dprintf("getsval %o: %s = \"%s\", t=%o\n", vp, vp->nval, vp->sval, vp->tval);
        return(vp->sval);
}

uuchar *tostring(s)
register uuchar *s;
{
        register uuchar *p;

        p = Malloc(strlen(s)+1);
        if (p == NULL)
                ERROR MSGSTR(TSPCTOSTR, "out of space in tostring on %s"), s FATAL;
        strcpy(p, s);
        return(p);
}

uuchar *qstring(s, delim)       /* collect string up to delim */
        uuchar *s;
        int delim;
{
        uuchar *q;
        int c, n;

        for (q = cbuf; (c = *s) != delim; s++) {
                if (q >= cbuf + CBUFLEN - 1)
                        ERROR MSGSTR(TSTR2LNG, "string %.10s... too long"), cbuf SYNTAX;
                else if (c == '\n')
                        ERROR MSGSTR(TNLINSTR, "newline in string %.10s..."), cbuf SYNTAX;
                else if (c != '\\')
                        *q++ = c;
                else    /* \something */
                        switch (c = *++s) {
                        case '\\':      *q++ = '\\'; break;
                        case 'n':       *q++ = '\n'; break;
                        case 't':       *q++ = '\t'; break;
                        case 'b':       *q++ = '\b'; break;
                        case 'f':       *q++ = '\f'; break;
                        case 'r':       *q++ = '\r'; break;
                        default:
                                if (!isdigit(c)) {
                                        *q++ = c;
                                        break;
                                }
                                n = c - '0';
                                if (isdigit(s[1])) {
                                        n = 8 * n + *++s - '0';
                                        if (isdigit(s[1]))
                                                n = 8 * n + *++s - '0';
                                }
                                *q++ = n;
                                break;
                        }
        }
        *q = '\0';
        return cbuf;
}

#ifdef MYALLOC

long    stamp;

void *Malloc(n)
        int n;
{
        void *p;

        p = malloc(n);
        fprintf(stderr, "%6d a %d %d\n", ++stamp, p, n);
        return p;
}

uuchar *Calloc(n, sz)
        int n, sz;
{
        uuchar *p;

        p = (uuchar *) calloc(n, sz);
        fprintf(stderr, "%6d a %d %d (%d %d)\n", ++stamp, p, n*sz, n, sz);
        return p;
}

Free(p)
        uuchar *p;
{
        fprintf(stderr, "%6d f %d\n", ++stamp, p);
        free(p);
}

#endif
