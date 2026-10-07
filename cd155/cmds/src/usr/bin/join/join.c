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
static char rcsid[] = "@(#)$RCSfile: join.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:26:37 $";
#endif
/*
 * COMPONENT_NAME: (CMDFILES) commands that manipulate files
 *
 * FUNCTIONS: join
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
 * join.c	1.6  com/cmd/files,3.1,9008 11/2/89 15:00:17
 */

#include	<stdio.h>
#include	<locale.h>
#include	<NLchar.h>
#ifdef KJI
#include        <NLctype.h>
#endif
#include "join_msg.h"

nl_catd catd;
#define MSGSTR(Num, Str) catgets(catd, MS_JOIN, Num, Str)

#define F1 0
#define F2 1
#define	NFLD	20	/* max field per line */
#ifdef NLS
#define comp() NLstrcmp(ppi[F1][j1],ppi[F2][j2])
#else
#define comp() strcmp(ppi[F1][j1],ppi[F2][j2])
#endif 

FILE *f[2];
char buf[2][BUFSIZ];	/*input lines */
char *ppi[2][NFLD];	/* pointers to fields in lines */
int	j1	= 1;	/* join of this field of file 1 */
int	j2	= 1;	/* join of this field of file 2 */
int	olist[2*NFLD];	/* output these fields */
int	olistf[2*NFLD];	/* from these files */
int	no;	/* number of entries in olist */
NLchar	sep1	= ' ';
NLchar 	sep2	= '\t';
char	*nothing = "";
char	*null	= "";
int	unpub1;
int	unpub2;
int	aflg;

/*
 * NAME: join [-anum] [-e string] [-j[n] [m] [-o n.m[,n.m ...]] [-tchar] fl1 fl2
 * FUNCTION: Joins data fields of two fields.
 */
main(argc, argv)
char *argv[];
{
	register int i;
	register int n1, n2;
	long top2, bot2;

	(void ) setlocale(LC_ALL,"");
	catd = catopen(MF_JOIN, 0);

	while (argc > 1 && argv[1][0] == '-') {
		if (argv[1][1] == '\0')
			break;
		switch (argv[1][1]) {
		case 'a':
			switch(argv[1][2]) {
			case '1':
				aflg |= 1;
				break;
			case '2':
				aflg |= 2;
				break;
			default:
				aflg |= 3;
			}
			break;
		case 'e':
			null = argv[2];
			argv++;
			argc--;
			break;
		case 't':
			if (!NCisshift(argv[1][2]))
				sep1 = sep2 = argv[1][2];
			else
				sep1 = sep2 = _NCd2(argv[1][2], argv[1][3]);
			break;
		case 'o':
			for (no = 0; no < 2*NFLD; no++) {
				if (argv[2][0] == '1' && argv[2][1] == '.') {
					olistf[no] = F1;
					olist[no] = atoi(&argv[2][2]);
				} else if (argv[2][0] == '2' && argv[2][1] == '.') {
					olist[no] = atoi(&argv[2][2]);
					olistf[no] = F2;
				} else
					break;
				argc--;
				argv++;
			}
			break;
		case 'j':
			if ((i = atoi (argv[2])) < 1 || i > 20)
				error (MSGSTR(BADFLDNUM, "%s: bad field number"), argv[2]);
			if (argv[1][2] == '1')
				j1 = i;
			else if (argv[1][2] == '2')
				j2 = i;
			else
				j1 = j2 = i;

			argc--;
			argv++;
			break;
		}
		argc--;
		argv++;
	}
	for (i = 0; i < no; i++)
		olist[i]--;	/* 0 origin */
	if (argc != 3)
		error(MSGSTR(USAGE, "usage: join [-an] [-e s] [-jn m] [-tc] [-o list] file1 file2"));
	j1--;
	j2--;	/* everyone else believes in 0 origin */

	if (argv[1][0] == '-')
		f[F1] = stdin;
	else if ((f[F1] = fopen(argv[1], "r")) == NULL)
		error(MSGSTR(CANTOPEN, "can't open %s"), argv[1]);
	if ((f[F2] = fopen(argv[2], "r")) == NULL)
		error(MSGSTR(CANTOPEN, "can't open %s"), argv[2]);

#define get1() n1=input(F1)
#define get2() n2=input(F2)

	get1();
	bot2 = ftell(f[F2]);
	get2();
	while(n1>0 && n2>0 || aflg!=0 && n1+n2>0) {
		if(n1>0 && n2>0 && comp()>0 || n1==0) {
			if(aflg&2) output(0, n2);
			bot2 = ftell(f[F2]);
			get2();
		} else if(n1>0 && n2>0 && comp()<0 || n2==0) {
			if(aflg&1) output(n1, 0);
			get1();
		} else /*(n1>0 && n2>0 && comp()==0)*/ {
			while(n2>0 && comp()==0) {
				output(n1, n2);
				top2 = ftell(f[F2]);
				get2();
			}
			fseek(f[F2], bot2, 0);
			get2();
			get1();
			for(;;) {
				if(n1>0 && n2>0 && comp()==0) {
					output(n1, n2);
					get2();
				} else if(n1>0 && n2>0 && comp()<0 || n2==0) {
					fseek(f[F2], bot2, 0);
					get2();
					get1();
				} else /*(n1>0 && n2>0 && comp()>0 || n1==0)*/{
					fseek(f[F2], top2, 0);
					bot2 = top2;
					get2();
					break;
				}
			}
		}
	}
	return(0);
}

/*
 * get input line and split into fields
 * returns zero on EOF, otherwise returns number of fields
 */
input(n) {
	register int i;
#ifdef KJI
	register NLchar c;
#else
	register int c;
#endif
	register char *bp;
	register char **pp;

	bp = buf[n];
	pp = ppi[n];
	if (fgets(bp, BUFSIZ, f[n]) == NULL)
		return(0);
	i = 0;
	do {
		i++;
#ifdef KJI
		if (NCisspace(sep1)) {       /* strip multiples */
			while (NCisspace(NCdechr(bp)))
				bp += NLchrlen(bp);
		}
#else
		if (sep1 == ' ')	/* strip multiples */
			while ((c = *bp) == sep1 || c == sep2)
				bp++;	/* skip blanks */
		else
			c = *bp;
#endif

		*pp++ = bp;	/* record beginning */
#ifdef KJI
		c = NCdechr(bp);
		while (c != sep1 && c != '\n' && c != sep2 && c != '\0') {
			bp += NCchrlen(c);
			c = NCdechr(bp);
		}
		*bp = '\0';   /* mark end by overwriting blank */
		bp += NCchrlen(sep1);
#else
		while ((c = *bp) != sep1 && c != '\n' && c != sep2 && c != '\0')
			bp++;
		*bp++ = '\0';	/* mark end by overwriting blank */
#endif
			/* fails badly if string doesn't have \n at end */
	} while (c != '\n' && c != '\0');

	for (c = i; c < NFLD; c++)
		*pp++ = nothing;

	return(i);
}

output(on1, on2)	/* print items from olist */
register int on1, on2;
{
	register int i;
	register char *temp;

	if (no <= 0) {	/* default case */
		if (!*(temp = on1 ? ppi[F1][j1]: ppi[F2][j2]))
			temp = null;
		printf ("%s", temp);

		for (i = 0; i < on1; i++)
			if (i != j1)
#ifdef KJI
				if(NCchrlen(sep1) > 1)
					printf("%c%c%s", _NCtop(sep1), _NCbot(sep1), ppi[F1][i]);
				else
				printf("%c%s", sep1, ppi[F1][i]);
#else
				printf("%c%s", sep1, ppi[F1][i]);
#endif
		for (i = 0; i < on2; i++)
			if (i != j2)
#ifdef KJI
				if(NCchrlen(sep1) > 1)
					printf("%c%c%s", _NCtop(sep1), _NCbot(sep1), ppi[F2][i]);
				else
				printf("%c%s", sep1, ppi[F2][i]);
#else
				printf("%c%s", sep1, ppi[F2][i]);
#endif
		printf("\n");
	} else {
		for (i = 0; i < no; i++) {
			temp = ppi[olistf[i]][olist[i]];
			if(olistf[i]==F1 && on1<=olist[i] ||
			   olistf[i]==F2 && on2<=olist[i] ||
			   *temp==0)
				temp = null;
			printf("%s", temp);
			if (i == no - 1)
				printf("\n");
			else
#ifdef KJI
				if(NCchrlen(sep1) > 1)
					printf("%c%c", _NCtop(sep1), _NCbot(sep1));
				else
				printf("%c", sep1);
#else
				printf("%c", sep1);
#endif
		}
	}
}

error(s1, s2, s3, s4, s5)
char *s1;
{
	fprintf(stderr, "join: ");
	fprintf(stderr, s1, s2, s3, s4, s5);
	fprintf(stderr, "\n");
	exit(1);
}
