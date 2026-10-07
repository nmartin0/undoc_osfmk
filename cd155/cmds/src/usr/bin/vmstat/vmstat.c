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
 * (c) Copyright 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
 */

/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: vmstat.c,v $ $Revision: 1.7 $ (OSF) $Date: 1994/11/19 01:47:13 $";
#endif
/*
 * OSF/1 1.1
 */

#include <sys/secdefines.h>

#ifdef SEC_BASE
#include <sys/security.h>
#include <prot.h>

extern priv_t *privvec();
#endif

#include <stdio.h>
#include <ctype.h>
#include <mach.h>
#include <sys/param.h>
#include <sys/table.h>
#include <sys/types.h>
#include <machine/machparam.h>

extern	char	*malloc();

#include <locale.h>
#include "vmstat_msg.h"
nl_catd catd;
#define MSGSTR(Num,Str) catgets(catd, MS_VMSTAT, Num, Str)

#define	fatal(x,str)	{ fprintf(stderr, "%s: %s\n", progname, str); exit(x); }
#define	printsum(str,x)		printf("%10d   %-25s\n", x, str);
#define	printlsum(str,x)	printf("%10ld   %-25s\n", x, str);
#define pgtok(x) 	((x) * NBPG / 1024)
#define	delta(x, y)	(x.y - x##_last.y)


struct vm_statistics	vmstat, 
			vmstat_last;

struct tbl_sysinfo 	sysinfo, 
			sysinfo_last;

struct machine_slot	slot_info,
			slot_info_last;

struct tbl_intr 	instat, 
			instat_last;

long			boottime,
			deltaseconds;

double			cpu_user,
			cpu_sys,
			cpu_idle,
			cpu_total; 

int			proc_runqueue,
			proc_waiting,
			proc_unintr,
			proc_stopped;

/* The following structure is used to get an optimal statistic printout */
struct 	msg_info{
		char hdr_msg[20];	/* Header messages */
		char digits[11];	/* Max desimal number from a long */
		int  length;		/* Length of the longest entity of */
					/* hdr_msg or digits */
};
/* The number of elements in the header statistics */
#define	HDR_ELEMENTS	18

/* An array containing all the header messages and values */
struct msg_info info[HDR_ELEMENTS];

/* pr_header is set to TRUE if a header should be printed in display_info() */
int pr_header;

#define	MAXLINES	20
int	lines;
int	vmpercent;
char *	progname;

void	usage();

void	do_forks();
void	do_tabular();
void	do_sum();

void	fetch_proc();
void	fetch_sysinfo();
void	fetch_slotsinfo();
void	fetch_vmstats();
void	fetch_instat();

void	get_hdr_info();
void	get_val_info();
void	get_length_info();
void	display_info();
void	value_grown();

main(argc, argv)
	int	argc;
	char	*argv[];
{
        extern char 	       *optarg;
        extern int      	optind;
        register int    	c;
	int			count;
	int			nocount;
	int			interval;
	long			nproc;
	time_t			now;
	struct tbl_procinfo    *procTable;

	progname = argv[0];
	count = 0;
	nocount = 1;
	interval = 0;

	/* Open the message catalogue */
	catd = catopen(MF_VMSTAT, 0);

#if SEC_BASE
	set_auth_parameters(argc, argv);
	initprivs();

	if (forceprivs(privvec(SEC_DEBUG, -1), (priv_t *) 0)) {
		fprintf(stderr, MSGSTR(MSG_NOPRIV,
			"%s: insufficient privileges\n"), command_name);
		exit(1);
	}
#else
        if (getuid() && geteuid())
		fatal(1, MSGSTR(EPERM, "missing privilege to run"));
#endif
	/* Allocate all proc table memory once, now */
	nproc = table(TBL_PROCINFO, 0, (char *)0, 32767, 0);
	procTable = (struct tbl_procinfo *)malloc(nproc *
						  sizeof(struct tbl_procinfo));

	if (procTable == 0)
		fatal(1, MSGSTR(ENOMEM,
				"insufficient memory for process table"));

        while ((c = getopt(argc, argv, "scif")) != EOF) {
                switch (c) {
                case 'c':
                        break;
                case 'f':
			fetch_instat();
			do_forks();
			exit(0);
                case 'i':
                        break;
                case 's':
			fetch_vmstats();
			fetch_sysinfo();
			fetch_slotsinfo();
			fetch_proc(nproc, procTable);
			fetch_instat();
			do_sum();
			exit(0);
                default:
                        usage();
                        exit(1);
                }
        }
        if (optind < argc)
                interval = atoi(argv[optind++]);
        if (optind < argc) {
                count = atoi(argv[optind++]);
		nocount = 0;
	}

#ifdef MACH2_5
	/* We must disable the notification port.  Otherwise, if vmstat
	 * is indefinitely looping, NOTIFY_PORT_DELETED messages will
	 * pile up until all system memory is consumed!
	 */
	if (task_set_notify_port(mach_task_self(), (mach_port_t)0) != KERN_SUCCESS) {
		fatal(1, MSGSTR(EPERM, "could not disable notify port"));
	}
#endif /* MACH2_5 */

        if (table(TBL_SYSINFO, 0, &sysinfo, 1, sizeof(sysinfo)) != 1) 
		fatal(1, MSGSTR(NO_SYSINFO, "cannot get system info"));
        time(&now);
        boottime 	= now - sysinfo.si_boottime;
        deltaseconds 	= boottime;

	if (interval == 0) {
		fetch_vmstats();
		fetch_sysinfo();
		fetch_slotsinfo();
		fetch_proc(nproc, procTable);
		fetch_instat();
		do_tabular();
		exit(0);
	} 

	lines = 0;
	while (nocount || count--) {

		fetch_vmstats();
		fetch_sysinfo();
		fetch_slotsinfo();
		fetch_proc(nproc, procTable);
		fetch_instat();
		do_tabular();

		sleep(interval);
		deltaseconds = interval;

		if (++lines > MAXLINES)
			lines = 0;
	}
}


void
usage()
{
	fprintf(stderr, 
		MSGSTR(USAGE, "usage: vmstat [-f | -s] [interval [count]]\n"));
	exit(1);
}


void
do_tabular()
{
	static int first = TRUE;

	if (lines == 0) {
		printf(MSGSTR(RPT_VMSTATS, 
			"Virtual Memory Statistics: (pagesize = %d)\n"),
			vmstat.pagesize);

		if (first) {
			/* get hold of the header text */
			get_hdr_info();
		}
	}

	/* read all the virtual memory values into "info" */
	get_val_info();

	if (first) {
		/* 
		 * the first time the longest string of info[i].hdr_msg and
		 * info[i].digits has to be calculated.
		 * This function must be called after get_hdr_info() and
		 * get_val_info().
		 */
		get_length_info();
		first = FALSE;
	}
	/*
	 * test if the values need more space then the header. If
	 * thats the case, a new header has to be printed.
	 */
	value_grown();

	/* display the information */
	display_info();
}


void
do_forks()
{
	printsum(MSGSTR(RPT_FORKS, "forks"),
			instat.in_forks);
	printsum(MSGSTR(RPT_VFORKS, "vforks"),
			instat.in_vforks);
}


void
do_sum()
{
	long	x;

	printf(MSGSTR(RPT_VMSTATS, 
		"Virtual Memory Statistics: (pagesize = %d)\n"),
		vmstat.pagesize);

	printlsum(MSGSTR(RPT_ACTIVE, "active pages"),
		vmstat.active_count);

	printlsum(MSGSTR(RPT_INACTIVE, "inactive pages"),
		vmstat.inactive_count);

	printlsum(MSGSTR(RPT_FREEPAGES, "free pages"), 
		vmstat.free_count);

	printlsum(MSGSTR(RPT_WIRED, "wired pages"),
		vmstat.wire_count);
		
	printlsum(MSGSTR(RPT_FAULTS, "virtual memory page faults"),
		vmstat.faults);

	printlsum(MSGSTR(RPT_COW, "copy-on-write page faults"),
		vmstat.cow_faults);
		
	printlsum(MSGSTR(RPT_ZEROFILL, "zero fill page faults"),
		vmstat.zero_fill_count);
		
	printlsum(MSGSTR(RPT_REACTIVE, "reattaches from reclaim list"),
		vmstat.reactivations);
		
	printlsum(MSGSTR(RPT_PAGEINS, "pages paged in"),
		vmstat.pageins);
		
	printlsum(MSGSTR(RPT_PAGEOUTS, "pages paged out"),
		vmstat.pageouts);
		
	printlsum(MSGSTR(RPT_LOOKUPS, "number of lookups"),
			vmstat.lookups); 

	printlsum(MSGSTR(RPT_HITS, "number of hits"),
			vmstat.hits); 

	x = 0;
	if (vmstat.lookups > 0)
		x = vmstat.hits * 100 / vmstat.lookups;
	printlsum(MSGSTR(RPT_CACHE, "hit ratio percentage"),
		x);

	printlsum(MSGSTR(RPT_CONTEXT, "task and thread context switches"),
		instat.in_context);
	printlsum(MSGSTR(RPT_DEVINTR, "device interrupts"),
		instat.in_devintr);
	printlsum(MSGSTR(RPT_SYSCALL, "system calls"),
		instat.in_syscalls);
}

void
fetch_vmstats()
{
	static int	initialized = 0;

	if (!initialized++)
 		bzero(&vmstat, sizeof(vmstat));

	vmstat_last = vmstat;

	if (vm_statistics(current_task(), &vmstat) != KERN_SUCCESS)
                fatal(2, MSGSTR(NO_VMSTATS, "cannot get vm statistics info"));

}

void
fetch_sysinfo()
{
	static int	initialized = 0;

	if (!initialized++)
		bzero(&sysinfo, sizeof(sysinfo));

	sysinfo_last = sysinfo;

        if (table(TBL_SYSINFO, 0, &sysinfo, 1, sizeof(sysinfo)) != 1) 
		fatal(1, MSGSTR(NO_SYSINFO, "cannot get system info"));
}

void
fetch_slotsinfo()
{
	struct machine_slot _slot_info;
	int		slot;
	int		rc;
	static int	initialized = 0;

	if (!initialized++)
		bzero(&slot_info, sizeof(slot_info));

	slot_info_last = slot_info;

	bzero(&slot_info, sizeof(slot_info));
	for (slot=0; ; ++slot)  {
		rc = xxx_slot_info(
			mach_task_self(),
			slot,
			&_slot_info);
		if ( rc != KERN_SUCCESS )
			break;
#define	Cadd(f) slot_info.f += _slot_info.f
		Cadd(cpu_ticks[CPU_STATE_SYSTEM]);
		Cadd(cpu_ticks[CPU_STATE_USER]);
		Cadd(cpu_ticks[CPU_STATE_IDLE]);
#undef	Cadd
	}
}


void
fetch_instat()
{
	static int	initialized = 0;

	if (!initialized++)
		bzero(&instat, sizeof(instat));

	instat_last = instat;

        if (table(TBL_INTR, 0, &instat, 1, sizeof(instat)) != 1) 
		fatal(1, MSGSTR(NO_INTR, "cannot get interrupt info"));
}



void
fetch_proc(nproc, procTable)
long nproc;
struct tbl_procinfo *procTable;
{
	int 	i;
	int	j;
	task_t			task;
	thread_array_t		thread_table;
	unsigned int		table_size;
	unsigned int		cnt;
	thread_basic_info_t     thi;
	thread_basic_info_data_t thi_data;

	proc_runqueue = 0;
	proc_waiting = 0;
	proc_unintr = 0;
	proc_stopped = 0;

	table(TBL_PROCINFO, 0, (char *)procTable, nproc,
	      sizeof(struct tbl_procinfo));
	for (i=0; i < nproc; i++) {

		if (procTable[i].pi_status != PI_ACTIVE)  
			continue;

		if (task_by_pid(mach_task_self(), procTable[i].pi_pid, &task)
		    != KERN_SUCCESS)
			continue;

		thi = &thi_data;
		if (task_threads(task, &thread_table, &table_size)
		    != KERN_SUCCESS)
			continue;
		for (j=0; j < table_size; j++) {
			cnt = THREAD_BASIC_INFO_COUNT;
			if (thread_info(thread_table[j], THREAD_BASIC_INFO,
				(thread_info_t)thi, &cnt)
			    != KERN_SUCCESS)
				continue;
		
			switch (thi->run_state) {
						/* Runnable / running */
			case TH_STATE_RUNNING:	
				proc_runqueue++;
				break;
						/* Sleeping intr-able */
			case TH_STATE_WAITING:      
				proc_waiting++;
				break;
						/* Sleeping unintr-able */
			case TH_STATE_UNINTERRUPTIBLE:
				proc_unintr++;
				break;
				 		/* Stopped */
			case TH_STATE_STOPPED:
				proc_stopped++;
				break;
						/* Halted */
			case TH_STATE_HALTED:	
			default:			/* ??? */
				break;
			}
		}

		vm_deallocate(mach_task_self(), (vm_address_t)thread_table,
			      table_size * sizeof(thread_t));
	}
}

/* 
 * get_hdr_info() fills headr information into the array "info".
 *
 * NOTE: The array is filled in a fixed order. If elements are deleted/
 *	 added or the order is changed, the code has to be carefully
 *	 changed !!
 */
void
get_hdr_info()
{
	/* Array containing the default header strings */
	static char *def_hdr_msg[HDR_ELEMENTS] = { "r", "w", "u", "act",
		"free", "wire", "fault", "cow", "zero", "react", "pin",
		"pout", "in", "sy", "cs", "us", "sy", "id" };

	/* Message numbers has to match the order in "dev_hdr_msg" */
	static int msg_nr[HDR_ELEMENTS] = { HDR_MSG_R, HDR_MSG_W, HDR_MSG_U,
		HDR_MSG_ACT, HDR_MSG_FREE, HDR_MSG_WIRE, HDR_MSG_FAULT,
		HDR_MSG_COW, HDR_MSG_ZERO, HDR_MSG_REACT, HDR_MSG_PIN,
		HDR_MSG_POUT, HDR_MSG_IN, HDR_MSG_SY, HDR_MSG_CS,
		HDR_MSG_US, HDR_MSG_SY2, HDR_MSG_ID };

	int i;

	for(i = 0; i < HDR_ELEMENTS; i++) {
		strcpy(info[i].hdr_msg, MSGSTR(msg_nr[i], def_hdr_msg[i]));
	}
}


/* 
 * get_val_info() fills values into "info[i].digits".
 *
 * NOTE: The fill is done in a fixed order. If elements are deleted/
 *	 added or the order is changed, the code has to be carefully
 *	 changed !!
 */
void
get_val_info()
{
	int i;

	/* transform the values into a string */
	sprintf(info[0].digits, "%d", proc_runqueue);
	sprintf(info[1].digits, "%d", proc_waiting);
	sprintf(info[2].digits, "%d", proc_unintr);
	sprintf(info[3].digits, "%ld", (long) vmstat.active_count);
	sprintf(info[4].digits, "%ld", (long) vmstat.free_count);
	sprintf(info[5].digits, "%ld", (long) vmstat.wire_count);
	sprintf(info[6].digits, "%ld", (long) delta(vmstat, faults));
	sprintf(info[7].digits, "%ld", (long) delta(vmstat, cow_faults));
	sprintf(info[8].digits, "%ld", (long) delta(vmstat, zero_fill_count));
	sprintf(info[9].digits, "%ld", (long) delta(vmstat, reactivations));
	sprintf(info[10].digits, "%ld", (long) delta(vmstat, pageins));
	sprintf(info[11].digits, "%ld", (long) delta(vmstat, pageouts));
	sprintf(info[12].digits, "%ld", (long) delta(instat, in_devintr) /
						deltaseconds);
	sprintf(info[13].digits, "%ld", (long) delta(instat, in_syscalls) /
						deltaseconds);
	sprintf(info[14].digits, "%ld", (long) delta(instat, in_context) /
						deltaseconds);

#ifdef	NOTDEF_ALL_NODES_INFO
	cpu_user = delta(sysinfo,si_user) + delta(sysinfo, si_nice);
	cpu_sys = delta(sysinfo,si_sys);
	cpu_idle = delta(sysinfo,si_idle);
#else
	cpu_user = delta(slot_info,cpu_ticks[CPU_STATE_USER]);
	cpu_sys = delta(slot_info,cpu_ticks[CPU_STATE_SYSTEM]);
	cpu_idle = delta(slot_info,cpu_ticks[CPU_STATE_IDLE]);
#endif
	if ((cpu_total=cpu_user +cpu_sys +cpu_idle) == 0.)
		cpu_total = 1;

	sprintf(info[15].digits, "%3.0f", (float) cpu_user * 100. / cpu_total);
	sprintf(info[16].digits, "%3.0f", (float) cpu_sys * 100. / cpu_total);
	sprintf(info[17].digits, "%3.0f", (float) cpu_idle * 100. / cpu_total);
}

/*
 * get_length_info() calculates which entity is the longest of
 *	"info[i].hdr_msg" and "info[i].digits" and stores the longest
 *	entity into "info[i].length"
 */
void
get_length_info()
{
	int i;

	for(i = 0; i < HDR_ELEMENTS; i++) {
		info[i].length = 
			strlen(info[i].hdr_msg) > strlen(info[i].digits) ?
				strlen(info[i].hdr_msg) :
				strlen(info[i].digits);
	}
}

/* display_info() displays the information in "info".	*/
void
display_info()
{

	int len[4], i;

	if ((lines == 0) || (pr_header == TRUE)) {
		/* 
		 * Print the header information. 
		 * Since the header information consist of two lines
		 * which is dependent on each other, the first line
		 * is alligned with the second one.
		 */
		len[0] = info[0].length + info[1].length + info[2].length + 3;
		len[1] = info[3].length + info[4].length + info[5].length + 2;
		len[2] = info[6].length + info[7].length + info[8].length +
			 info[9].length + info[10].length + info[11].length + 4;
		len[3] = info[12].length + info[13].length + 
			info[14].length + 3;

		printf(" %-*.*s %-*.*s %-*.*s %-*.*s %s\n",
			len[0], len[0], MSGSTR(HDR_MSG_PROCS, "procs"),
			len[1], len[1], MSGSTR(HDR_MSG_MEMORY, "memory"),
			len[2], len[2], MSGSTR(HDR_MSG_PAGES, "pages"),
			len[3], len[3], MSGSTR(HDR_MSG_INTR, "inr"),
			MSGSTR(HDR_MSG_CPU, "cpu"));

		/* print the second header line */
		for (i = 0; i < HDR_ELEMENTS; i++) {
			printf("%*.*s", info[i].length + 1, info[i].length + 1,
				info[i].hdr_msg);
		}
		printf("\n");
	}

	/* print the values */
	for (i = 0; i < HDR_ELEMENTS; i++) {
		printf("%*.*s", info[i].length + 1, info[i].length + 1,
			info[i].digits);
	}
	printf("\n");
	fflush(stdout);	/* flush buffered data to prevent from losing
			   data when the process is stopped */
}


/*
 * value_grown() tests if some of the values need more space then the header. 
 *	If thats the case, the global variable "pr_header" will be set
 *	and a new header will be printed in display_info().
 *	Note that the header is printed only if the size of the value
 *	gets bigger.
 */
void
value_grown()
{
	int i;

	pr_header = FALSE;

	/* 
	 * test if the values has grown bigger than the space reserved
	 * for it in the output table.
	 */
	for(i = 0; i < HDR_ELEMENTS; i++) {
		if (strlen(info[i].digits) > info[i].length) {
			info[i].length = strlen(info[i].digits);
			pr_header = TRUE;
		}
	}
}
