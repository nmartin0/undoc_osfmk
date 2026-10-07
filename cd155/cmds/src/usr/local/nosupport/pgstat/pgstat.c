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
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: pgstat.c,v $ $Revision: 1.5 $ (OSF) $Date: 1994/11/29 22:07:53 $";
#endif
/*
 * HISTORY
 * $Log: pgstat.c,v $
 * Revision 1.5  1994/11/29  22:07:53  jlitvin
 * This is a Stefan Tritscher bug fix.  He noticed that the pgstat tool
 * wasn't working.  Once he fixed this, he noticed a serious bug in the
 * server (PTS #11713).
 *
 *  Reviewer: suri
 *  Risk: low
 *  Benefit or PTS #: 11670
 *  Testing: developer
 *  Module(s): cmds_libs/src/usr/local/nosupport/pgstat/pgstat.c
 *
 * Revision 1.4  1994/11/19  03:00:51  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/08  18:49:38  stans
 *     Temporarily removed OSF 1.1 table syscall items (global paging) so
 *     pgstat will compile/run under Paragon which is an OSF 1.0.4 table
 *     syscall base.
 *
 * Revision 1.2  1993/07/01  17:00:09  stans
 * OSF 1.2 cmds src update
 *
 * Revision 1.3.5.2  1992/07/01  19:58:47  garyf
 * 	#6862: fix divide by zero problem
 * 	[1992/07/01  19:58:27  garyf]
 *
 * Revision 1.3.3.3  1992/04/01  15:29:23  mmp
 * 	if there are no active paging files, just get global info
 * 	(bug 5991)
 * 	[1992/04/01  13:50:44  mmp]
 * 
 * Revision 1.3.3.2  1991/10/20  16:21:56  rod
 * 	Vnode pager clustering and clustered pagein:
 * 		1) use new pginfo structure and reformat output (gmf);
 * 		2) print global information (gmf);
 * 		3) turn on -f and count options (gmf);
 * 		4) add -g, -i options (gmf).
 * 
 * Revision 1.3  91/06/10  16:23:38  devrcs
 * 	Dynamically size header for name, to allow long names like /tmp_paging/paging/space
 * 	[91/06/03  16:18:35  meissner]
 * 
 * Revision 1.2  91/03/23  17:56:17  devrcs
 * 	Add more comments, header stuff.
 * 	[91/03/01  16:40:18  jeffc]
 * 
 * 	Initial version.
 * 	[91/03/01  08:44:35  jeffc]
 * 
 * $EndLog$
 */
/*
 * NAME
 *	pgstat - Show system paging file statistics.
 *
 * SYNOPSIS
 *	pgstat [-f file ... ] [-g] [-k] [-n] [-r rows ] [-t] [ -i interval] [count]
 *
 *	pgstat shows information about paging file allocation and activity.
 *
 * FLAGS
 *	-f file ...
 *		Restrict information display to the named files.
 *	-g	Display global data only.
 *
 *	-i interval	Repeat display every interval seconds.
 *
 *	-k 	Express space numbers in units of kilobytes (1024 bytes).
 *		Default is to express space in pages.
 *
 *	-n 	Do not display the header.
 *
 *	-r rows Number of rows to display between headers. Ignored if -n
 *		is also specified.
 *
 *	-t 	Display totals for repeated displays rather than
 *		differences.
 *
 * DESCRIPTION
 *	pgstat retrives and displays allocation and I/O statistics from the
 *	system vnode pager. For each paging file selected, the following
 *	information is displayed:
 *
 *	Allocation Statistics
 *	 - number of pages in this file allocated
 *	 - size of this paging file
 *	 - percentage of space allocated
 *
 *	Paging Statistics
 *	 - number of pageins from this file
 *	 - number of pageouts to this file
 *	 - number of page initialize requests to this file
 *	 - number of page initialize requests that resulted in a pageout
 *	   (this number is included in the pageout count)
 *
 *	If an interval is specified, the command loops, displaying new
 *	information every interval seconds. The second and subsequent
 *	displays are expressed as differences from the previous display.
 *
 *	Pageins are the number of pages read from backing store to satisfy
 *	page faults. Since paging files are only used for anonymous backing
 *	store, this does not include demand-paged text pageins. See vmstat(1)
 *	for system-wide paging statistics. The pagein number also does not
 *	include requests for pages that the pager does not have (i.e., zero
 *	fill on demand pages).
 *
 *	Pageouts are pages written to the backing store.
 * 
 *	An asterisk (*) next to the pagein or pageout number indicates
 *	that one or more operations failed, usually due to the paging
 *	file running out of space.
 *
 *	Page initializations are a special pageout request that occurs
 *	when the memory manager "pushes" a private copy of a mapped file
 *	into the paging file. The pager determines if it is necessary
 *	to write this page to backing store, or if it already has a 
 *	copy of the data.
 *
 */
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/table.h>

#ifdef	OSF1AD
#define	tbl_pginfo tbl_pginfo_10
#define OSF_11 0	/* still at OSF 1.0.4 cmds & libs */
#endif

extern char *optarg;
extern int optind;

char *progname;
char *filename;
int noheader = 0;
int didheader = 0;
int didgstats = 0;
int linecount = 0;
int pagesize = 0;
int Rows = 24;
int rows;
int name_size = 0;
int interval = 0;
int gflag = 0;
int doglobal = 1;

extern void usage(char *);
extern void header(void);
extern void print_stats(struct tbl_pginfo *, struct tbl_pginfo *, int);
#if	OSF_11
extern void print_gstats(struct tbl_pginfo *, struct tbl_pginfo *);
#endif

int
main(int argc, char **argv)
{
	int count = -1;
	int kbytes = 0, totals = 0;
        int i, c;
	int npagefile, onpagefile = 0, pnpagefile = 0;
	int nfetch, onfetch = 0;
	struct tbl_pginfo *pfiles = NULL;
	struct tbl_pginfo *prev_pfiles = NULL;
	struct tbl_pginfo *pfi, *opfi;
	struct tbl_pagerglobal_info *pggp;

	progname = argv[0];
	pagesize = sysconf(_SC_PAGE_SIZE);

        while ((c = getopt(argc, argv, "i:f:gknr:t")) != EOF) {
                switch (c) {
		case 'f': filename = optarg; break;
		case 'g': gflag++; break;
		case 'i': interval = atoi(optarg); break;
		case 'k': kbytes = pagesize/1024; break;
		case 'n': noheader++; break;
		case 'r': Rows = atoi(optarg); Rows = (Rows<3)?3:Rows; break;
		case 't': totals++; break;
                default:
                        usage(progname);
                        return(1);
                }
        }
        if (optind < argc)
                count = atoi(argv[optind++]);
	if (filename) {
		if (!gflag)
			doglobal = 0;
		else
			gflag = 0;
	}
loop:
	/* Determine the size of the paging file table */
	if ((npagefile = table(TBL_PGINFO, -1, (char *)0, SHRT_MAX, 0)) < 0) {
		fprintf(stderr, "%s: cannot retrieve pager file information\n",
			progname);
		return(2);
	}
	if (npagefile == 0) {
		fprintf(stderr, "%s: no paging files active\n", progname);
		/* get global info only */
		npagefile = 1;
		gflag++;
	}
	if (npagefile > onpagefile) {
		if (pfiles) free(pfiles);
		pfiles = (struct tbl_pginfo *)malloc(sizeof(struct tbl_pginfo)
				* npagefile);
		onpagefile = npagefile;
	}

	/* Fetch all elements of the paging file table */
        nfetch = table(TBL_PGINFO, -1, pfiles, npagefile,
		sizeof(struct tbl_pginfo));

	/* Size the name field to be the largest name installed.  */
	name_size = strlen("Global Info:");
	for (i = 0; i < nfetch; i++) {
		int len = strlen (pfiles[i].pg_name);
		if (name_size < len)
			name_size = len;
	}

	if (!noheader && (nfetch != onfetch)) {
		rows = (((Rows - 2) / nfetch) * nfetch) + 2;
		onfetch = nfetch;
		header();
	}

	didgstats = 0;
	for (i = 0; i < nfetch; i++) {
		pfi = &pfiles[i];
		if (prev_pfiles && (i < pnpagefile) && !totals && !didheader)
			opfi = &prev_pfiles[i];
		else 
			opfi = NULL;
#if	OSF_11
		if (doglobal)
			print_gstats(pfi, opfi);
#endif
		if (!gflag && ((filename == NULL) || 
		    !strcmp(filename, pfi->pg_name)))
			print_stats(pfi, opfi, kbytes);
	}
	didheader = 0;
	if (--count && interval) {
		if (nfetch > pnpagefile) {
			/* allocate new "previous" array */
			if (prev_pfiles) free(prev_pfiles);
			prev_pfiles = (struct tbl_pginfo *)
				malloc(sizeof(struct tbl_pginfo) * nfetch);
			pnpagefile = nfetch;
		}
		bcopy(pfiles, prev_pfiles,
			sizeof(struct tbl_pginfo) * nfetch);
		sleep(interval);
		goto loop;
	}
	return(0);
}

void
usage(char *s)
{
	fprintf(stderr, "%s: [-f file ... ] [-g] [-k] [-n] [-r rows ] [-t] [ -i interval] [count]\n", s);
}

void
header(void)
{
	printf("Paging Statistics (page size of %d bytes):\n", pagesize);
	printf("%-*s %s %s %6s %s %10s %12s %s\n",
	        name_size,
		"Paging File",
		"Pri,clsize",
		"Used", "Total", "% ",
		"Calls:In",
		"Calls:Out",
		"Unav.,Init,Write");
	linecount += 2;
	didheader++;
}

void
print_stats(struct tbl_pginfo *pfi, struct tbl_pginfo *opfi, int kbytes)
{
struct tbl_pginfo pageinfo;

	if (!noheader && (linecount % rows) == 0) {
		header();
	} else if (opfi) {	/* Do delta calculations */
		pageinfo = *pfi;
		pageinfo.pg_pagein_count -= opfi->pg_pagein_count;
		pageinfo.pg_pagein_fail -= opfi->pg_pagein_fail;
		pageinfo.pg_pageout_count -= opfi->pg_pageout_count;
		pageinfo.pg_pageout_fail -= opfi->pg_pageout_fail;
#if	OSF_11
		pageinfo.pg_unavail_count -= opfi->pg_unavail_count;
#endif
		pfi = &pageinfo;
	}

	if (kbytes == 0) {
		printf("%-*s %3d,%d %9d %6d %2d%% %6d:%-5d%c %5d:%-5d%c %3d, %s, %s\n",
		        name_size, pfi->pg_name,
#if	OSF_11
			pfi->pg_priority, pfi->pg_cluster_size,
#else
			0,0,
#endif
			(pfi->pg_npgs-pfi->pg_free), pfi->pg_npgs,
			pfi->pg_npgs ? 100*(pfi->pg_npgs - pfi->pg_free)/pfi->pg_npgs: 0,
			pfi->pg_pagein_count, pfi->pg_pagein_count,
			pfi->pg_pagein_fail?'*':' ',
			pfi->pg_pageout_count, pfi->pg_pageout_count,
			pfi->pg_pageout_fail?'*':' ',
#if	OSF_11
			pfi->pg_unavail_count, "-", "-");
#else
			0, "-", "-");
#endif
	} else {
		printf("%-*s %3d,%d %9dK %6dK %2d%% %6d:%-5d%c %5d:%-5d%c %3d, %s, %s\n",
		        name_size, pfi->pg_name,
#if	OSF_11
			pfi->pg_priority, pfi->pg_cluster_size,
#else
			0,0,
#endif
			(pfi->pg_npgs-pfi->pg_free)*kbytes, pfi->pg_npgs*kbytes,
			pfi->pg_npgs ? 100*(pfi->pg_npgs - pfi->pg_free)/pfi->pg_npgs : 0,
			pfi->pg_pagein_count, pfi->pg_pagein_count,
			pfi->pg_pagein_fail?'*':' ',
			pfi->pg_pageout_count, pfi->pg_pageout_count,
			pfi->pg_pageout_fail?'*':' ',
#if	OSF_11
			pfi->pg_unavail_count, "-", "-");
#else
			0, "-", "-");
#endif
	}
	linecount += 1;
}

#if	OSF_11

void
print_gstats(struct tbl_pginfo *pfi, struct tbl_pginfo *opfi)
{
	struct tbl_pagerglobal_info pgi;
	struct tbl_pagerglobal_info *opgi;
	struct tbl_pagerglobal_info *pgip;

	if (didgstats++)
		return;
	if (!noheader && (linecount % rows) == 0)
		header();
	pgip = &pfi->pg_global;
	if (opfi) {	/* Do delta calculations */
		pgi = pfi->pg_global;
		opgi = &opfi->pg_global;
		pgi.pgg_pageout_calls -= opgi->pgg_pageout_calls;
		pgi.pgg_pagein_calls -= opgi->pgg_pagein_calls;
		pgi.pgg_pages_in -= opgi->pgg_pages_in;
		pgi.pgg_pages_out -= opgi->pgg_pages_out;
		pgi.pgg_data_unavail -= opgi->pgg_data_unavail;
		pgi.pgg_data_init_pages -= opgi->pgg_data_init_pages;
		pgi.pgg_data_init_writes -= opgi->pgg_data_init_writes;
		pgip = &pgi;
	}
	printf("%-*s %3s,%s %9s %5s %2s %8d:%-5d %6d:%-5d %4d, %d, %d\n",
	        name_size, "Global Info:", "-", "-", " - ", " -", "",
		pgip->pgg_pagein_calls, pgip->pgg_pages_in,
		pgip->pgg_pageout_calls, pgip->pgg_pages_out,
		pgip->pgg_data_unavail, pgip->pgg_data_init_pages, 
		pgip->pgg_data_init_writes);
	linecount += 1;
}
#endif	/* OSF_11 */
