/*
 *
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 *
 */

/*
 * pgs - Display Mach Memory Object Paging File Statistics
 *
 * HISTORY:
 * $Log: pgs.c,v $
 * Revision 1.2  1995/03/15  17:46:40  stans
 *  Add pts #
 *
 *  Reviewer:self
 *  Risk:low
 *  Benefit or PTS #:12113
 *  Testing:developer
 *
 * Revision 1.1  1995/03/15  17:34:52  stans
 * Initial revision
 *
 */

#include <mach.h>
#include <stdio.h>
#include <sys/types.h>
#include <mach/boolean.h>
#include <sys/table.h>
#include <time.h>
#include <strings.h>
#include <malloc.h>
#include <errno.h>
#include <sys/ioctl.h>

#define MK_DEFAULT_PAGING_FILE "/mach_servers/paging_file"

#define PG_KERN_DEFAULT 0
#define PG_VNODE_FILE 1
#define PG_VNODE_RAWPART 2

typedef struct tbl_pginfo_10 *tbl_pginfo_t;

char		*myname;
char		*useage = "useage: pgs -n node -dvkpDV sleep-interval-in-seconds\n\
  where:\n\
    -n node	display only for the specified node\n\
    -d		display only the boot-node default pager stats\n\
    -v		display only Vnode pagers stats\n\
    -k		paging file units in K bytes (default)\n\
    -p		paging file units in VM pages\n\
    -D		display delta change since last iteration\n\
    -V		Verbose display\n";

boolean_t	Kbytes=TRUE;	/* Kbytes or VM pages */

extern int	errno;

void
dump_stats(struct tbl_pginfo_10 *tbl_pginfo)
{
	char *pgtype[] = {
		"Micro-Kernel Default",	/* PG_KERN_DEFAULT */
		"Vnode",		/* PG_VNODE_FILE */
		"Vnode (raw partition)"	/* PG_VNODE_RAWPART */
	 }; 

	float	percent;
	u_long	inuse, t1;

	printf("\nNode[%d] %s Paging File @ '%s'%s\n", tbl_pginfo->pg_node,
		pgtype[tbl_pginfo->pg_type],
		(tbl_pginfo->pg_type == PG_KERN_DEFAULT ?
			MK_DEFAULT_PAGING_FILE : tbl_pginfo->pg_name),
		(tbl_pginfo->pg_prefer ? " (perfered)" : "") );

	inuse = tbl_pginfo->pg_npgs - tbl_pginfo->pg_free;
	percent = ((float)inuse / (float)tbl_pginfo->pg_npgs) * 100.0;
 
	printf("  Paging file allocation (in pages): total %d free %d used %d (%.3f%%)\n",
					tbl_pginfo->pg_npgs,
					tbl_pginfo->pg_free,
					inuse, percent );


	printf("  Page read:  requests %6d failures %d\n",
						tbl_pginfo->pg_pagein_count,
						tbl_pginfo->pg_pagein_fail);

	printf("  Page write: requests %6d failures %d\n",
						tbl_pginfo->pg_pageout_count,
						tbl_pginfo->pg_pageout_fail);

	printf("  Pages initialized    %6d\n", tbl_pginfo->pg_pageinit_count);
	printf("  Pages written        %6d\n", tbl_pginfo->pg_pageinit_write);

	printf("  Max page # allocated %6d\n", tbl_pginfo->pg_hipage);
}


void
print_title()
{
	char	*units="(in Kbytes)";

	if ( Kbytes == FALSE )
		units = "(in Pages) ";
	printf("\nNode Paging file   %s   Total    Free    Used  (%%)   pgin   pgout\n",units);
printf( "---------------------------------------------------------------------------\n");

#if 0
Node Paging file   (in Kbytes)   Total    Free    Used  (%%)  read    write
xxxx /mach_servers/paging_file xxxxxxx xxxxxxx xxxxxxx x.xxx xxxxxx  xxxxxx
#endif
}

void
print_stats(
	int		idx,
	tbl_pginfo_t	tbl_pginfo,
	tbl_pginfo_t	last_time,
	boolean_t	deltas )
{
	float		percent;
	u_long		p_inuse, inuse, total, free, pagein, pageout;
	tbl_pginfo_t	current = &tbl_pginfo[idx];
	tbl_pginfo_t	prev = &last_time[idx];

	total = current->pg_npgs;
	free = current->pg_free;
	inuse = total - free;
	percent = ( (float)inuse / (float)total ) * 100.0;

	pagein = current->pg_pagein_count;
	pageout = current->pg_pageout_count;

	/*
	 * Doing deltas from last call? If not then dump out current stats,
	 * otherwise print deltas.
	 */
	if ( deltas ) {
		p_inuse = prev->pg_npgs - prev->pg_free;
		inuse = p_inuse - inuse;
		free = prev->pg_free - current->pg_free;
		pagein = prev->pg_pagein_count - current->pg_pagein_count;
		pageout = prev->pg_pageout_count - current->pg_pageout_count;
	}

	if ( Kbytes ) {
		total = (total * vm_page_size) / 1024;
		free = (free * vm_page_size) / 1024;
		inuse = (inuse * vm_page_size) / 1024;
	}

	printf("%4d %-25s %7d %7d %7d %3.3f %6d %6d\n",
			current->pg_node,
			(current->pg_type == PG_KERN_DEFAULT ?
				MK_DEFAULT_PAGING_FILE : current->pg_name),
			total, free, inuse, percent, pagein, pageout );
}

pgs_useage_exit( char *msg )
{
	if ( msg )
		fprintf(stderr,"%s: %s\n",myname,msg);
	fprintf(stderr,"%s: %s\n",myname,useage);
	exit(1);
}

/*
 * use table() syscall to retrieve paging file information.
 *
 * inputs:
 *	info		paging file buffer pointer
 *	num_pagers	count of paging files in the system.
 * outputs:
 *	none or exit() on error.
 */

void
get_paging_file_info( tbl_pginfo_t info, int num_pagers )
{
	kern_return_t	kr;

	kr = table(TBL_PGINFO, -1, info, num_pagers,
				sizeof(struct tbl_pginfo_10)); 
	if (kr == -1) {
		perror("get_paging_file_info()");
		exit(1);
	}
}

main( argc, argv )
	int	argc;
	char	*argv[];
{
	struct tbl_pginfo_10	*pginfo, *last_time;
	int			num_pagers, i;
	int			nap_time=0, snode = -1;
	char			*cp;
	boolean_t		Vnode = FALSE;
	boolean_t		verbose = FALSE;
	boolean_t		Kdefault = FALSE;
	boolean_t		doDeltas = FALSE;
	boolean_t		time_stamp = FALSE;
	struct winsize		tinfo;
	int			LINES_PER_SCREEN;

	/*
	 * determine screen size.
	 */
	(void) ioctl(0,TIOCGWINSZ,&tinfo);
	if ( tinfo.ws_row > 0 )
		LINES_PER_SCREEN = tinfo.ws_row;
	else
		LINES_PER_SCREEN = 25;

	/* remember my name */
	if ( (cp=strrchr(argv[0],'/')) )
		myname = cp+1;
	else
		myname = argv[0];

	/* process cmd-line args if necessary */
	if ( argc > 1 ) {
		argc--; argv++;
		for(; (argc > 0 && argv[0][0] == '-'); argc--,argv++ ) {
			char	pbuf[128];

			switch( argv[0][1] ) {
			  case 'n':
				if ( argv[0][2] )
					cp = &argv[0][2];
				else {
					argv++; argc--;
					cp = argv[0];
				}
				snode = atoi( cp );
				break;
			  case 'v':
				Vnode = TRUE;
				break;
			  case 'd':
				Kdefault = TRUE;
				break;
			  case 'D':
				doDeltas = TRUE;
				break;
			  case 'V':
				verbose = TRUE;
				break;
			  case 'p':
				Kbytes = FALSE;
				break;
			  case 'k':
				Kbytes = TRUE;
				break;
			  case 't':
				time_stamp = TRUE;
				break;
			  case 'h':
				pgs_useage_exit(0);
				break;
			  default:
				sprintf(pbuf,"unknown switch -%c",argv[0][1]);
				pgs_useage_exit(pbuf);
				break;
			}
		}

		/*
		 * set the sleep interval in seconds if necessary
		 */
		if ( argc > 0 ) {
			nap_time = atoi(argv[0]);
		}
	}

	/*
	 * retrieve the number of paging nodes.
	 */
	num_pagers = table(TBL_PGINFO, -1, (char *)0, 32767, 0);
	if (num_pagers == -1) {
		perror("table");
		exit(errno);
	}

	/*
	 * table() syscall space allocation.
	 */
	pginfo = (struct tbl_pginfo_10 *)
			calloc(num_pagers, sizeof(struct tbl_pginfo_10));

	if (pginfo == NULL) {
		perror("calloc");
		exit(errno);
	}

	/*
	 * allocate a buffer for old data (delta's) if necessary.
	 */
	if ( !verbose ) {
		last_time = (struct tbl_pginfo_10 *)
			calloc(num_pagers, sizeof(struct tbl_pginfo_10));

		if (last_time == NULL) {
			perror("calloc2");
			exit(errno);
		}
		bzero( last_time, (num_pagers * sizeof(struct tbl_pginfo_10)) );
	}

	get_paging_file_info( pginfo, num_pagers );

	if ( verbose ) {
		printf("Number Pagers: %d\n", num_pagers);
		for(i=0; i<num_pagers; i++) {
			(void) dump_stats(pginfo++);
		}
	}
	else { /* display paging file information in intervals */
		int		line_count=0, lines_output=0;
		boolean_t	deltas;

		do {
			line_count -= lines_output;
			if ( line_count <= 0 ) {
				line_count = LINES_PER_SCREEN - 3;
				print_title();
				deltas = FALSE;
			}
			else
				deltas = TRUE;

			if ( time_stamp ) {
				time_t	t;
				char	*time_str;
				int	eos;

				t = time( (time_t *)0 );
				time_str = ctime ( &t );
				eos = strlen(time_str);
				time_str[eos-1] = ']';
				printf("[%s\n",time_str);
			}

			for(i=0,lines_output=0; i < num_pagers; i++) {
				if ( Vnode ) {
					if(pginfo[i].pg_type == PG_KERN_DEFAULT)
						continue;
				}
				if ( Kdefault ) {
					if(pginfo[i].pg_type != PG_KERN_DEFAULT)
						continue;
				}
				if ( snode != -1 && pginfo[i].pg_node != snode )
					continue;

				lines_output++;
				(void) print_stats( i, pginfo, last_time,
						(doDeltas ? deltas : FALSE) );
			}

			if ( lines_output == 0 ) {
				fprintf(stderr,
					"%s: node %d does not export a pager\n",
					myname,snode);
				exit(1);
			}

			/*
			 * to preserve the above test, increment the time stamp
			 * modification to the lines output count.
			 */
			if ( time_stamp )
				lines_output++;

			if ( nap_time ) {
				sleep(nap_time);
				/*
				 * if doing more than one node then separate
				 * output.
				 */
				if ( snode == -1 && Kdefault == FALSE ) {
					printf("\n");
					lines_output++;
				}
				if ( doDeltas )
					bcopy( pginfo, last_time,
						(num_pagers *
						sizeof(struct tbl_pginfo_10)));

				get_paging_file_info( pginfo, num_pagers );
			}
		} while( nap_time );
	}
}
