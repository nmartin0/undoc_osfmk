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
#if !defined( lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#) $RCSfile: ac.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:02:53 $";
#endif

/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/*
 * COMPONENT_NAME: (CMDACCT) Command Accounting
 *
 * FUNCTIONS: loop, print, upall, update, among, newday, pdate 
 *
 * ORIGINS: 9, 26, 27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

/*
#ifndef lint
static char sccsid[] = "ac.c	1.2  com/cmd/acct/ac,3.1,8943 10/24/89 10:29:19";
#endif
 */
/*
 * ac [ -w wtmp ] [ -d ] [ -p ] [ people ]
 */

#include <stdio.h>
#include <ctype.h>
#include <utmp.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <locale.h>

#include "ac_msg.h" 
nl_catd catd;
#define MSGSTR(Num,Str) catgets(catd,MS_AC,Num,Str) 

#define NMAX sizeof(ibuf.ut_name)
#define LMAX sizeof(ibuf.ut_line)

#define TSIZE  6242
#define	USIZE	500
struct  utmp ibuf;

struct ubuf {
	char	uname[NMAX];
	long	utime;
} ubuf[USIZE];

struct tbuf {
	struct	ubuf	*userp;
	long	ttime;
} tbuf[TSIZE];

char	*wtmp;
int	pflag, byday;
long	dtime;
long	midnight;
long	lastime;
long	day	= 86400L;
int	pcount;
char	**pptr;
#define USAGE_STR	"Usage: %s [-dp] [-w filename] [user(s)...]\n"

#include <NLchar.h>
#include <NLctype.h>

main(argc, argv) 
char **argv;
{
	int fl;
	int c;
	register i;
	FILE *wf;
	extern char *optarg;
	extern int optind, opterr;

	setlocale( LC_ALL, "" );
	catd = catopen(MF_AC,0);

	wtmp = WTMP_FILE;
	while ((c=getopt(argc, argv, "dpw:")) != EOF) {
		switch(c) {
		case 'd':
			byday++;
			break;

		case 'p':
			pflag++;
			break;

		case 'w':
			if ( optarg ) {
				wtmp = optarg;
			} else {
				fprintf(stderr, MSGSTR(NOFILENM,
						"ac: Missing filename\n")); 
				exit(1); 
			}
			break;

		default:
		    	fprintf(stderr, MSGSTR(ACUSAGE, USAGE_STR), argv[0]);
			exit(1);
		}
	}
	pcount = argc - optind;
	pptr = &argv[optind];
	if ((wf = fopen(wtmp, "r")) == NULL) {
		(void)fprintf(stderr, MSGSTR(CANTOPEN, 
					"ac: Can't open %s\n"), wtmp);
		exit(1);
	}
	for(;;) {
		if (fread((char *)&ibuf, sizeof(ibuf), 1, wf) != 1)
			break;
		fl = 0;
		for (i=0; i<NMAX; i++) {
			c = ibuf.ut_name[i];
			if (isprint(c) && !isspace(c)) {
				if (fl)
					goto skip;
				continue;
			}
			if (isspace(c) || c=='\0') {
				fl++;
				ibuf.ut_name[i] = '\0';
			} else
				goto skip;
		}
		loop();
    skip:;
	}
	ibuf.ut_name[0] = '\0';
	ibuf.ut_line[0] = '~';
	(void)time(&ibuf.ut_time);
	loop();
	print();
	return(0);
}

loop()
{
	register i;
	register struct tbuf *tp;
	register struct ubuf *up;

	if(ibuf.ut_line[0] == '|') {
		dtime = ibuf.ut_time;
		return;
	}
	if(ibuf.ut_line[0] == '{') {
		if(dtime == 0)
			return;
		for(tp = tbuf; tp < &tbuf[TSIZE]; tp++)
			tp->ttime += ibuf.ut_time-dtime;
		dtime = 0;
		return;
	}
	if (lastime>ibuf.ut_time || lastime+(1.5*day)<ibuf.ut_time)
		midnight = 0;
	if (midnight==0)
		newday();
	lastime = ibuf.ut_time;
	if (byday && ibuf.ut_time > midnight) {
		upall(1);
		print();
		newday();
		for (up=ubuf; up < &ubuf[USIZE]; up++)
			up->utime = 0;
	}
	if (ibuf.ut_line[0] == '~') {
		ibuf.ut_name[0] = '\0';
		upall(0);
		return;
	}
	/*
	if (ibuf.ut_line[0]=='t')
		i = (ibuf.ut_line[3]-'0')*10 + (ibuf.ut_line[4]-'0');
	else
		i = TSIZE-1;
	if (i<0 || i>=TSIZE)
		i = TSIZE-1;
	*/

	/*
	 * Correction contributed by Phyllis Kantar @ Rand-unix
	 *
	 * Fixes long standing problem with tty names other than 00-99
	 */
	if (ibuf.ut_line[0]=='t') {
		i = (ibuf.ut_line[3]-'0');
		if(ibuf.ut_line[4])
			i = i*79 + (ibuf.ut_line[4]-'0');
	} else
		i = TSIZE-1;
	if (i<0 || i>=TSIZE) {
		i = TSIZE-1;
		(void)printf(MSGSTR(BADTTY, "ac: Bad tty name: %s\n"), 
							ibuf.ut_line);
	}

	tp = &tbuf[i];
	update(tp, 0);
}

print()
{
	int i;
	long ttime, t;

	ttime = 0;
	for (i=0; i<USIZE; i++) {
		if(!among(i))
			continue;
		t = ubuf[i].utime;
		if (t>0)
			ttime += t;
		if (pflag && ubuf[i].utime > 0) {
			(void)printf("\t%-*.*s %6.2f\n", NMAX, NMAX,
			    ubuf[i].uname, ubuf[i].utime/3600.);
		}
	}
	if (ttime > 0) {
		pdate();
		(void)printf(MSGSTR(TOTAL, "\ttotal %9.2f\n"), ttime/3600.);
	}
}

upall(f)
{
	register struct tbuf *tp;

	for (tp=tbuf; tp < &tbuf[TSIZE]; tp++)
		update(tp, f);
}

update(tp, f)
struct tbuf *tp;
{
	int j;
	struct ubuf *up;
	long t, t1;

	if (f)
		t = midnight;
	else
		t = ibuf.ut_time;
	if (tp->userp) {
		t1 = t - tp->ttime;
		if (t1 > 0)
			tp->userp->utime += t1;
	}
	tp->ttime = t;
	if (f)
		return;
	if (ibuf.ut_name[0]=='\0') {
		tp->userp = 0;
		return;
	}
	for (up=ubuf; up < &ubuf[USIZE]; up++) {
		if (up->uname[0] == '\0')
			break;
		for (j=0; j<NMAX && up->uname[j]==ibuf.ut_name[j]; j++);
		if (j>=NMAX)
			break;
	}
	for (j=0; j<NMAX; j++)
		up->uname[j] = ibuf.ut_name[j];
	tp->userp = up;
}

among(i)
{
	register j, k;
	register char *p;

	if (pcount==0)
		return(1);
	for (j=0; j<pcount; j++) {
		p = pptr[j];
		for (k=0; k<NMAX; k++) {
			if (*p == ubuf[i].uname[k]) {
				if (*p++ == '\0' || k == NMAX-1)
					return(1);
			} else
				break;
		}
	}
	return(0);
}

newday()
{
	long ttime;
	struct timeb tb;
	struct tm *localtime();

	(void)time(&ttime);
	if (midnight == 0) {
		ftime(&tb);
		midnight = 60*(long)tb.timezone;
		if (localtime(&ttime)->tm_isdst)
			midnight -= 3600;
	}
	while (midnight <= ibuf.ut_time)
		midnight += day;
}

pdate()
{
	long x;
	char timestring[6];
	struct tm	*tmp;

	if (byday==0)
		return;
	x = midnight-1;

	tmp = localtime(&x);
	strftime(timestring, 6, "%sD", tmp);
	(void)printf("%s ", timestring);
}
