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
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
/* 
 * Mach Operating System
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: ps.c,v $
 * Revision 1.19  1994/11/19  01:34:41  mtm
 * Copyright additions/changes
 *
 * Revision 1.18  1994/06/20  23:47:36  stans
 *  Reviewer: self
 *  Risk: low
 *  Benefit or PTS #: code size reduction, skip runtime kernel type check.
 *  Testing: developer
 *
 * 	Changed MACH_2.5 --> MACH_25 so 'cpp' won't complain about trailing
 * 		tokens.
 * 	Removed Mach 2.5 or Mach 3.0 runtime kernel determination in favor of
 * 		compile time determination; ps for OSF1_SERVER will always be
 * 		Mach 3.0 (FLW).
 *
 * Revision 1.17  1993/10/27  21:12:52  shala
 * Increased NPROC to 512 for better "ps" performance on system with multiple
 * service nodes.
 *
 * Revision 1.16  1993/10/27  20:03:53  shala
 * Changed NPROC to 128 from 16 to speed up the table call for ps.
 *
 * Revision 1.15  1993/06/22  14:41:41  shala
 * Changed PIDLEN to be 6 to correct formatting. Fixed bug 3211 by locus.
 *
 * Revision 1.14  1993/05/04  17:50:37  shala
 * Fixed to work with 3.0 mach library .
 *
 * Revision 1.13  1993/01/02  21:56:06  stans
 * Added #include <kern/queue.h> before include <sys/proc.h> to resolve
 * 'queue_head_t" not being defined.
 *
 * Revision 1.12  1992/12/30  02:11:04  steved
 * Changed 'strcpy' to 'strncpy' to in 'save()' to fix core dumping on listing
 * applications with more than 13 characters. Specifically, the NQS software
 * space padded their process name ("NQS              ").
 *
 * Revision 1.11  1992/12/11  01:30:02  shala
 * Support new table call.
 *
 * Revision 2.2  92/06/10  17:37:54  pjg
 * 	Initial check-in in AD.
 * 
 * Revision 3.0  92/02/28  17:59:07  barbou
 * ps(1) command source file: modified from OSF/1.0.3
 * 
 * Revision 1.7.4.3  91/07/11  19:22:27  robert
 * 	added changed made by Eric S. in 1.1 - but
 * 	presumably not in 1.0.2, The 1.1 log revision
 * 	by Eric for this change is:
 * 
 * 	Revision 1.8.2.3  91/06/14  11:29:56  ers
 * 	Use host_info instead of xxx_host_info.
 * 	[91/06/14  11:19:25  ers]
 * 
 * Revision 1.7.4.2  91/06/12  13:30:29  jph
 * 	Out of bounds array reference fix in save() (dumps core on i386).
 * 	Terminating NULL added after strncpy() in save().
 * 	[91/06/12  13:29:50  jph]
 * 
 * Revision 1.7  90/10/31  14:53:32  devrcs
 * 	Fix usertime and systime commands.
 * 	[90/10/09  16:07:23  jvs]
 * 
 * Revision 1.6  90/10/07  16:51:13  devrcs
 * 	Correct size of sleep time field.  (Kernel returns a long.)
 * 	Also, kernel changes were made to return correct sleep time values.
 * 	[90/10/03  09:48:21  jvs]
 * 
 * Revision 1.5  90/09/23  16:26:15  devrcs
 * 	Clear newstring for BSD options in kludge_bsdoptions.
 * 	[90/09/08  09:48:11  brezak]
 * 
 * Revision 1.4  90/09/13  12:18:25  devrcs
 * 	fix catopen and catgets
 * 	[90/08/31  14:13:56  mbrown]
 * 
 * 	Fix printing of symbolic wait channel.
 * 	[90/08/22  18:05:54  brezak]
 * 
 * Revision 1.3  90/07/27  09:58:21  devrcs
 * 	xflg really should check for user. Add Eflg for SYSV
 * 	everything opt. Print <exiting> for PI_EXITING.
 * 	[90/07/18  18:38:18  brezak]
 * 
 * 	Added security hooks from SecureWare.
 * 	[90/07/16  16:09:22  seiden]
 * 
 * 	Add titles to message catalog.
 * 	[90/07/13  15:40:56  brezak]
 * 
 * 	Remove text_size and use resident_size for RSS.
 * 	[90/07/11  10:22:41  brezak]
 * 
 * Revision 1.2  90/07/17  11:55:31  devrcs
 * 	Fix xflg to not check for a user.
 * 	[90/07/10  18:39:39  brezak]
 * 
 * 	Add status field format. Don't get UAREA for zombies. Set args
 * 	to defunct for zombies.
 * 	[90/07/10  18:24:55  brezak]
 * 
 * 	Basic rewrite. Adopted table driven architecture from BSD4.4
 * 	ps. Added System V options and message catalogs from AIX
 * 	version. Preserved table and other Mach interfaces from CMU version.
 * 	[90/07/10  11:56:59  brezak]
 * 
 * $EndLog$
 */

#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: ps.c,v $ $Revision: 1.19 $ (OSF) $Date: 1994/11/19 01:34:41 $";
#endif

#include <sys/secdefines.h>
#if SEC_BASE
#include <sys/security.h>
#include <prot.h>

extern priv_t *privvec();
#endif


#include <sys/version.h>

#include <stdio.h>
#include <strings.h>
#include <pwd.h>
#include <time.h>
#include <mach.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <kern/queue.h>
#include <sys/param.h>
#include <sys/table.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <sys/ioctl.h>
#include <machine/vmparam.h>

#if defined(NLS) || defined(KJI)
#define	NLSKJI 1
#include <NLctype.h>
#include <NLchar.h>
#endif
#include <locale.h>

#ifdef MSG
#include "ps_msg.h"
nl_catd catd;
#define MSGSTR(Num,Str) catgets(catd,MS_PS,Num,Str)
#else
#define MSGSTR(Num,Str) Str
#endif

extern	uid_t	getuid();
extern	char	*calloc();
extern	char	*malloc();
extern	char	*realloc();
extern	char	*ttyname();
extern	char	*getenv();

#undef NZERO
#define NZERO 12

#ifndef	TH_USAGE_SCALE
#define	TH_USAGE_SCALE	1000
#endif	TH_USAGE_SCALE
#define usage_to_percent(u)	((u*100)/TH_USAGE_SCALE)
#define usage_to_tenths(u)	(((u*1000)/TH_USAGE_SCALE) % 10)

#define KERNBASE	0xff000000

#define FMTSEP		" \t,\n"

int	termwidth;
int	termheight;
#define UNLIMITED       0
int	totwidth;
int	sumrusage;
int	cmdstart;

#define	add_pid(p)	add_int((p), &pids_to_check, &num_pids_to_check)
#define check_pid(p)	check_int((p), pids_to_check, num_pids_to_check)
int     *pids_to_check;
int     num_pids_to_check;

#define	add_pgrp(p)	add_int((p), &pgrps_to_check, &num_pgrps_to_check)
#define check_pgrp(p)	check_int((p), pgrps_to_check, num_pgrps_to_check)
int     *pgrps_to_check;
int     num_pgrps_to_check;

#define	add_sess(p)	add_int((p), &sess_to_check, &num_sess_to_check)
#define check_sess(p)	check_int((p), sess_to_check, num_sess_to_check)
int     *sess_to_check;
int     num_sess_to_check;

#define	add_uid(p)	add_int((p), &uids_to_check, &num_uids_to_check)
#define check_uid(p)	check_int((p), uids_to_check, num_uids_to_check)
int     *uids_to_check;
int     num_uids_to_check;

#define	add_tty(p)	add_int((p), &ttys_to_check, &num_ttys_to_check)
#define check_tty(p)	check_int((p), ttys_to_check, num_ttys_to_check)
int     *ttys_to_check;
int     num_ttys_to_check;

int	aflg = 0, eflg = 0, Eflg = 0, xflg = 0, mflg = 0, gflg = 0;

int	prtheader;
int	lineno;

/*
 * flags to retrieve extra info
 */
int	needuser = 0, needcomm = 0;

/*
 * sorting order
 */
int	sortby;
#define	SORTMEM	1
#define	SORTRUN 2
#define	SORTCPU 3

/*
 * Options and usage
 */
char *bsd_usage =	"usage: %s [%s] [%s format] [%stty] [process number]\n";
#define BSD_USAGE_OPTS  "ps", "aexgmlvusjSUALwhT", "Oo", "t"
char *bsd_options = 	"?aexgmlvusjSUALwho:O:t:Tp:";
char *sysv_usage =	"usage: %s [%s] [%s format] [%s tlist] [%s plist] [%s ulist] [%s glist] [%s slist]\n";
#define SYSV_USAGE_OPTS "ps", "-edamjfl", "-oO", "-t", "-p", "-u", "-g", "-s"
char *sysv_options =	"?eamdlfjt:p:u:g:s:o:O:";

/*
 * collected proc info
 */
typedef	struct proc_info	*proc_info_t;
struct	proc_info {
	uid_t			uid;
        uid_t		        ruid;
        uid_t		        svuid;
        gid_t		        rgid;
        gid_t		        svgid;
	pid_t			pid;
	pid_t			ppid;
	pid_t			pgrp;
        pid_t                   session;
        pid_t			tpgrp;
        pid_t                   tsession;
        int                     jobc;
	int			status;
	int			flag;
	int			state;
	int			pri;
	int			base_pri;
        char                    p_cursig;
        int                     p_sig;
        int                     p_sigmask;
        int                     p_sigignore;
        int                     p_sigcatch;
	boolean_t		all_swapped;
	time_value_t		p_utime;
        time_value_t		p_stime;
	vm_size_t		virtual_size;
	vm_size_t		resident_size;
	float			percent_mem;
	int			cpu_usage;
        dev_t                   tty;
	char			*command;
        char                    *args;
        long                    slptime;
        long                    wait_chan;
#ifdef MACH_25
        char                    wait_mesg[WMESGLEN+1];
#endif
        struct user             *u;
        int                     suspend_count;
	int			num_threads;
	proc_info_t		threads;	/* array */
};

proc_info_t			proc_table = (proc_info_t)0;
int				max_proc_table, nprocs;

/*
 * the following describes the possible fields that can be printed
 */
enum type	{ CHAR, UCHAR, SHORT, USHORT, LONG, ULONG, KPTR };

#define	UIDFMT	"u"
#define UIDLEN	5
#define PIDFMT	"d"
#define PIDLEN	6
#define	USERLEN	8

#define POFF(x)         ((int)&((proc_info_t)0)->x)
#define UOFF(x)         ((int)&((struct user *)0)->x)
#define	ROFF(x)		((int)&((struct rusage *)0)->x)

int     command(), ucomm(), pvar(), uvar(), rvar(), tsize(), pmem(), pcpu(),
        cputime(), rsize(), vsize(), pri(), state(), longname(), tname(),
        tdev(), runame(), uname(), wchan(), logname(), longtname(), pagein(),
        started(), lstarted(), nice(), usertime(), systime(), empty();
char	*savestr();

typedef struct var      *var_t;
struct var {
	char	*name[8];	/* name(s) of variable */
        int	header_msg;     /* message number for title */
	char	*header;	/* default header */
	int	flag;
#define	USER	0x01	/* requires user structure */
#define	LJUST	0x02	/* right adjust on output */
#define	COMM	0x04	/* requires exec arguments and environment (XXX) */
#define THREAD	0x08	/* print this field for threads */
	int	(*oproc)();	/* output routine */
	short	width;		/* printing width */
	/*
	 * The following (optional) elements are hooks for passing information
	 * to the generic output routine pvar
	 */
	int	off;		/* offset in structure */
	enum	type type;	/* type of element */
	char	*fmt;		/* printf format */
	/*
	 * glue to link selected fields together
	 */
	struct	var *next;
}  var[] = {
	{{"command", "comm", "args"},
                 COMMAND_MSG, "COMMAND", LJUST|COMM, command, 16}, 
	{{"ucomm"},
                 COMMAND_MSG, "COMMAND", LJUST, ucomm, MAXCOMLEN},
	{{"logname"},
                 LOGNAME_MSG, "LOGNAME", USER|LJUST, logname, MAXLOGNAME},
	{{"flag", "f"},
                 F_MSG, "F", 0, pvar, 8, POFF(flag), LONG, "x"},
	{{"status"},
                 STATUS_MSG, "STATUS", 0, pvar, 8, POFF(status), LONG, "d"},
	{{"uid"},
                 UID_MSG, "UID", 0, pvar, UIDLEN, POFF(uid),USHORT, UIDFMT},
	{{"ruid"},
                 RUID_MSG, "RUID", 0, pvar, UIDLEN, POFF(ruid), USHORT, UIDFMT},
	{{"svuid"},
                 SVUID_MSG, "SVUID", 0, pvar, UIDLEN, POFF(svuid), USHORT, UIDFMT},
	{{"rgid"},
                 RGID_MSG, "RGID", 0, pvar, UIDLEN, POFF(rgid), USHORT, UIDFMT},
	{{"svgid"},
                 SVGID_MSG, "SVGID", 0, pvar, UIDLEN, POFF(svgid), USHORT, UIDFMT},
	{{"pid"},
                 PID_MSG, "PID", 0, pvar, PIDLEN, POFF(pid), LONG, PIDFMT},
	{{"ppid"},
                 PPID_MSG, "PPID", 0, pvar, PIDLEN, POFF(ppid), LONG, PIDFMT},
	{{"cp", "cpu"},
                 CP_MSG, "CP", THREAD, pvar, 3, POFF(cpu_usage), UCHAR, "d"},
        {{"nwchan"},
                 WCHAN_MSG, "WCHAN", 0, pvar, 6, POFF(wait_chan), KPTR, "x"},
	{{"wchan"},
                 WCHAN_MSG, "WCHAN", LJUST|THREAD, wchan, 6},
        {{"cursig"},
                 CURSIG_MSG, "CURSIG", 0, pvar, 2, POFF(p_cursig), CHAR, "d"},
	{{"sig", "pending"},
                 PENDING_MSG, "PENDING", 0, pvar, 8, POFF(p_sig), LONG, "x"},
	{{"sigmask", "blocked"},
                 BLOCKED_MSG, "BLOCKED", 0, pvar, 8, POFF(p_sigmask), LONG, "x"},
	{{"sigignore", "ignored"},
                 IGNORED_MSG, "IGNORED", 0, pvar, 8, POFF(p_sigignore), LONG, "x"},
	{{"sigcatch", "caught"},
                 CAUGHT_MSG, "CAUGHT", 0, pvar, 8, POFF(p_sigcatch), LONG, "x"},
	{{"user", "uname"},
                 USER_MSG, "USER", LJUST, uname, USERLEN},
	{{"ruser", "runame"},
                 RUSER_MSG, "RUSER", LJUST, runame, USERLEN},
	{{"pgid"},
                 PGID_MSG, "PGID", 0, pvar, PIDLEN, POFF(pgrp), LONG, PIDFMT},
	{{"jobc"},
                 JOBC_MSG, "JOBC", 0, pvar, 4, POFF(jobc), LONG, "d"},
	{{"sess", "session"},
                 SESS_MSG, "SESS", 0, pvar, PIDLEN, POFF(session), LONG, PIDFMT},
	{{"tdev", "dev"},
                 TDEV_MSG, "TDEV", 0, tdev, 4},
	{{"tname", "tty", "tt"},
                 TT_MSG, "TT", LJUST, tname, 3},
	{{"longtname", "longtty"},
                 TT_MSG, "TT", LJUST, longtname, 8},
	{{"tpgid"},
                 TPGID_MSG, "TPGID", 0, pvar, PIDLEN, POFF(tpgrp), LONG, PIDFMT},
	{{"tsession", "tsess"},
                 TSESS_MSG, "TSESS", 0, pvar, PIDLEN, POFF(tsession), LONG, PIDFMT},
	{{"state", "stat"},
                 STAT_MSG, "STAT", THREAD, state, 4},
	{{"pri"},
                 PRI_MSG, "PRI", THREAD, pri, 3},
	{{"usrpri"},
                 UPR_MSG, "UPR", 0, pvar, 3, POFF(pri), CHAR, "d"},
	{{"nice", "ni"},
                 NI_MSG, "NI", THREAD, nice, 3},
	{{"vsize", "vsz"},
                 VSZ_MSG, "VSZ", 0, vsize, 5},
	{{"rssize", "rsz", "rss", "p_rss"},
                 RSS_MSG, "RSS", 0, rsize, 4},
#undef	u_procp
        {{"u_procp", "uprocp"},
                 UPROCP_MSG, "UPROCP", USER, uvar, 6, UOFF(u_procp), KPTR, "x"},
#undef	u_cmask
	{{"umask", "u_cmask"},
                 UMASK_MSG, "UMASK", USER, uvar, 3, UOFF(u_cmask), CHAR, "o"},
#undef	u_acflag
	{{"acflag", "acflg"},
                 ACFLG_MSG, "ACFLG", USER, uvar, 3, UOFF(u_acflag), SHORT, "x"},
	{{"start"},
                 STARTED_MSG, "STARTED", USER|LJUST, started, 8},
	{{"lstart"},
                 STARTED_MSG, "STARTED", USER|LJUST, lstarted, 28},
	{{"cputime", "time"},
                 TIME_MSG, "TIME", USER|THREAD, cputime, 9},
	{{"usertime"},
                 USER_MSG, "USER", USER|THREAD, usertime, 9},
	{{"systime"},
                 SYSTEM_MSG, "SYSTEM", USER|THREAD, systime, 9},
	{{"pcpu", "%cpu"},
                 CPU_MSG, "%CPU", USER|THREAD, pcpu, 4},
	{{"pmem", "%mem"},
                 MEM_MSG, "%MEM", USER, pmem, 4},
	{{"sl", "slp", "slptime"},
                 SL_MSG, "SL", THREAD, pvar, 8, POFF(slptime), LONG, "d"},
	{{"pagein", "majflt"},
                 PAGEIN_MSG, "PAGEIN", USER, pagein, 6},
	{{"minflt"},
                 MINFLT_MSG, "MINFLT", USER, rvar, 4, ROFF(ru_minflt), LONG, "d"},
	{{"majflt"},
                 MAJFLT_MSG, "MAJFLT", USER, rvar, 4, ROFF(ru_majflt), LONG, "d"},
	{{"nswap"},
                 NSWAP_MSG, "NSWAP", USER, rvar, 4, ROFF(ru_nswap), LONG, "d"},
	{{"inblock", "inblk"},
                 INBLK_MSG, "INBLK", USER, rvar, 4, ROFF(ru_inblock), LONG, "d"},
	{{"oublock", "oublk"},
                 OUBLK_MSG, "OUBLK", USER, rvar, 4, ROFF(ru_oublock), LONG, "d"},
	{{"msgsnd"},
                 MSGSND_MSG, "MSGSND", USER, rvar, 4, ROFF(ru_msgsnd), LONG, "d"},
	{{"msgrcv"},
                 MSGRCV_MSG, "MSGRCV", USER, rvar, 4, ROFF(ru_msgrcv), LONG, "d"},
	{{"nsignals", "nsigs"},
                 NSIGS_MSG, "NSIGS", USER, rvar, 4, ROFF(ru_nsignals), LONG, "d"},
	{{"nvcsw", "vcsw"},
                 VCSW_MSG, "VCSW", USER, rvar, 5, ROFF(ru_nvcsw), LONG, "d"},
	{{"nivcsw", "ivcsw"},
                 IVCSW_MSG, "IVCSW", USER, rvar, 5, ROFF(ru_nivcsw), LONG, "d"},
	{{"scount", "scnt"},
                 SCNT_MSG, "SCNT", THREAD, pvar, 2, POFF(suspend_count), LONG, "d"},
	NULL
};
var_t vhead, vtail;

/*
 * Canned output formats
 */
#define DFMT	"pid tname state cputime comm"
#define LFMT \
	"uid pid ppid cp pri nice vsz rss wchan state tname cputime comm"
#define	JFMT	"user pid ppid pgid sess jobc state tname cputime comm"
#define	SFMT	"uid pid cursig sig sigmask sigignore sigcatch stat tname comm"
#define	VFMT \
	"pid tt state time sl pagein vsz rss %cpu %mem comm"
	/*"pid tt state time sl re pagein vsz rss lim tsiz trs %cpu %mem comm"*/
#define UFMT \
	"uname pid %cpu %mem vsz rss tt state start time comm"
        
#define F5FMT \
        "uname pid ppid %cpu start tt time comm"
#define L5FMT \
        "f state uid pid ppid %cpu pri nice rss wchan tt time ucomm"
#define FL5FMT \
        "f state uid pid ppid %cpu pri nice rss wchan start time comm"

/* 
 * combination variables. These provide a "short-hand" for
 * frequent use.
 */
struct combovar {
	char *name;
	char *replace;
} combovar[] = {
	"RUSAGE",	"minflt majflt nswap inblock oublock msgsnd msgrcv nsigs nvcsw nivcsw",
        "THREAD",   	"user %cpu pri scnt wchan usertime systime",
        "DFMT",		DFMT,
        "LFMT",		LFMT,
        "JFMT",		JFMT,
        "SFMT",		SFMT,
        "VFMT",		VFMT,
        "UFMT",		UFMT,
        "F5FMT",	F5FMT,
        "L5FMT",	L5FMT,
        "FL5FMT",	FL5FMT,
	0, 0
};

/*
 * What king of Mach is running ?
 */
#if	OSF1_SERVER
	mach_kernel = 3;	/* always Mach 3.0 if OSF/1 AD */
#else
int	mach_kernel = -1;	/* runtime determination of Mach kernel type */
#endif
        
/*
 * Buffer to retrieve arguments and environment
 */
int	arguments_size = 4096;
char    def_arguments_buf[4096];
char	*arguments = def_arguments_buf;

main(argc, argv)
        int	argc;
        char	*argv[];
{
        char *iptr;
        struct winsize	win;
        char *fmt = NULL, *parse_bsd_options(), *parse_sysv_options();
	task_t task;

#if SEC_BASE
    set_auth_parameters(argc, argv);
    initprivs();

    /* raise privilege to read kernel memory and retrieve task ports */
    if (forceprivs(privvec(SEC_DEBUG, -1), (priv_t *) 0)) {
        fprintf(stderr, "%s: insufficient privileges\n", command_name);
        exit(1);
    }
#endif
        
        (void) setlocale (LC_ALL,"");
#ifdef MSG
        catd = catopen((char *)MF_PS, 0);
#endif
        (void) mach_init();

        /* attempt to find out the size of the termainal */
        termwidth = 79;
        termheight = 24;
        if ((iptr = getenv("COLUMNS")) != NULL)
                termwidth = atoi(iptr);
        else if ((ioctl(1, TIOCGWINSZ, &win) == -1 &&
                  ioctl(2, TIOCGWINSZ, &win) == -1 &&
                  ioctl(0, TIOCGWINSZ, &win) == -1) ||
                 win.ws_col == 0)
#ifdef USE_TERMINFO
                if (getenv("TERM")) {
                        termheight = tgetnum("ro");
                        termwidth = tgetnum("co");
                }
                else
#endif
        {
                termwidth = 79;
                termheight = 24;
        }
        else {
                termwidth = win.ws_col - 1;
                termheight = win.ws_row;
        }

        /*
         * If argv[1] doesn't begin with a '-' then parse BSD options
         * otherwise use SYSV options.
         */
        if (argc > 1) {
                if (*argv[1] != '-')
                        fmt = parse_bsd_options(argc, argv);
                else
                        fmt = parse_sysv_options(argc, argv);
        }
        
	if (fmt)
		parsefmt(fmt);
        else
                parsefmt(DFMT);
        
	if (!aflg && !Eflg &&
            num_ttys_to_check == 0 &&
            num_pids_to_check == 0 &&
            num_pgrps_to_check == 0 &&
            num_sess_to_check == 0 &&
            num_uids_to_check == 0)
                add_uid(getuid());

        if (prtheader)
                prtheader = termheight > 5 ? termheight - 2 : 22;

#ifndef	OSF1_SERVER
	/*
	 * We could be running on Mach 2.5 or 3.0.
	 * Determine which...a real CMU hack.
	 */
	{
		int rc;
		switch(mach_msg_trap(0, 0, 0, 0, 0, 0, 0)) {
		    case KERN_SUCCESS:
			mach_kernel = 3;
			break;
		    case KERN_INVALID_ARGUMENT:
			mach_kernel = 2;
			break;
		    default:
			printf("Can't figure out which kernel is running ! (mach_msg_trap() returned 0x%x)\n", rc);
			break;
		}
	}
#endif	/* OSF1_SERVER */

        print_all_tasks();
        exit(0);
}

char *
parse_bsd_options(argc, argv)
        int argc;
        char *argv[];
{
        extern char *optarg, *kludge_bsdps_options();
        extern int optind;
        int ch, errflg=0, fmtflg=0;
        char *fmt = NULL; 

        argv[1] = kludge_bsdps_options(argv[1]);

        while ((ch = getopt(argc, argv, bsd_options)) != EOF)
            switch ((char)ch) {
            case 'a':	aflg++;		break;  /* BSD: all with ttys */
            case 'e':	eflg++;		break;  /* BSD: environment */
            case 'x':	xflg++;		break;  /* BSD: everything */
            case 'm':	mflg++;		break;  /* BSD: threads */
            case 'g':	gflg++;		break;  /* BSD: session leaders */
            case 'l':                           /* BSD: long */
                    fmt = LFMT;
                    fmtflg++;
                    break;
            case 'v':                           /* BSD: vm info */
                    fmt = VFMT;
                    fmtflg++;
                    sortby = SORTMEM;
                    break;
            case 'u':                           /* BSD: user info */
                    fmt = UFMT;
                    fmtflg++;
                    /*sortby = SORTRUN;*/
                    sortby = SORTCPU;
                    break;
            case 's':                           /* BSD: signals info */
                    fmt = SFMT;
                    fmtflg++;
                    break;
            case 'j':                           /* BSD: job control info */
                    fmt = JFMT;
                    fmtflg++;
                    break;
            case 'S':                           /* BSD: usage summary */
                    sumrusage++;
                    break;
            case 'U':   /* /etc/rc calls ps -U but for now we do nothing */
                    exit(0);
                    break;
            case 'A':                           /* BSD: increase arg space */
                    arguments_size += arguments_size;
                    if (arguments_size & 0x3)
                            /* Needs a word multiple, sorry */
                            arguments_size &= ~ 0x3;
                    arguments = (char *)calloc((arguments_size / sizeof(int)), sizeof(int));
                    break;
            case 'L': {                         /* BSD: list format options */
                    int i = 0;
                    struct combovar *cb = &combovar[0];
                    char *cp;
                    var_t v;
                    
                    v = &var[0];
                    for (;;) {
                            if (v->name[0] != NULL) {
                                    cp = v->name[0];
                                    v++;
                            } else if (cb->name != NULL) {
                                    cp = cb->name;
                                    cb++;
                            } else
                                    break;
                            if (termwidth && 
                                (i += strlen(cp)+1) > termwidth)
                                    i = strlen(cp), printf("\n");
                            printf("%s ", cp);
                    }       
                    printf("\n");
                    exit(0);
            }
            case 'w':                           /* BSD: widen */
                    if (termwidth < 131)
                            termwidth = 131;
                    else
                            termwidth = UNLIMITED;
                    break;
            case 'o':                           /* BSD: custom options */
                    fmt = savestr(optarg);
                    fmtflg++;
                    break;
            case 'O':                           /* BSD: canned options */
                    fmt = (char *)malloc(strlen(optarg) + 25);
                    sprintf(fmt, "pid %s state tt time comm", optarg);
                    fmtflg++;
                    break;
            case 'T':                           /* BSD: controlling tty */
                    if ((optarg = ttyname(0)) == NULL)
                            error(NOT_TTY, "%s: not a terminal", "<stdin>");
                    /*FALLTHROUGH*/
            case 't':                           /* BSD: specified tty */
                    errflg = parsetty(optarg);
                    xflg++;
                    aflg++;
                    break;
            case 'p':                           /* BSD: specific pid */
                    add_pid(atoi(optarg));
                    xflg++;
                    aflg++;
                    break;
            case 'h':                           /* BSD: extra header */
                    prtheader++;
                    break;
            case '?':
            default:
                    errflg++;
                    break;
            }

        if (errflg) {
                fprintf(stderr, MSGSTR(BSD_USAGE, bsd_usage), BSD_USAGE_OPTS);
                exit(1);
        }

        if (fmtflg > 1)
                error(BAD_FMT, "specified too many format options");

        return(fmt);
}
        
char *
parse_sysv_options(argc, argv)
        int argc;
        char *argv[];
{
        extern char *optarg;
        extern int optind;
        int ch, errflg=0, fmtflg=0;
        int lfflg=0, llflg=0;
        char *fmt = NULL;
        
        while ((ch = getopt(argc, argv, sysv_options)) != EOF)
	    switch ((char)ch) {
            case 'e':	Eflg++;	xflg++;	break;  /* SYSV: everything */
            case 'a':	aflg++;		break;  /* SYSV: all with ttys */
            case 'm':	mflg++;		break;  /* SYSV: include threads */
            case 'd':	gflg++;		break;  /* SYSV: session leaders */
            case 'f':                           /* SYSV: flags info */
                    lfflg++;
                    break;
            case 'l':                           /* SYSV: long info */
                    llflg++;
                    break;
            case 'j':                           /* SYSV: job control info */
                    fmt = JFMT;
                    fmtflg++;
                    break;
            case 't':                           /* SYSV: specific ttys */
                    errflg = parsetty(optarg);
                    xflg++;
                    aflg++;
                    break;
            case 'p':                           /* SYSV: specific pids */
                    errflg = parsepid(optarg);
                    xflg++;
                    aflg++;
                    break;
            case 'g':                           /* SYSV: specific pgroups */
                    errflg = parsepgrp(optarg);
                    xflg++;
                    aflg++;
                    break;
            case 's':                           /* SYSV: specific sessions */
                    errflg = parsesess(optarg);
                    xflg++;
                    aflg++;
                    break;
            case 'u':                           /* SYSV: specific users */
                    errflg = parseuid(optarg);
                    xflg++;
                    aflg++;
                    break;
            case 'o':                           /* SYSV: custom options */
                    fmt = savestr(optarg);
                    break;
            case 'O':                           /* SYSV: canned options */
                    fmt = (char *)malloc(strlen(optarg) + 25);
                    sprintf(fmt, "pid %s state tt time comm", optarg);
                    break;
            case '?':
            default:
                    errflg++;
                    break;
            }

        if (errflg) {
                fprintf(stderr, MSGSTR(SYSV_USAGE, sysv_usage), SYSV_USAGE_OPTS);
                exit(1);
        }
        
        if (lfflg && llflg && fmtflg)
                error(BAD_FMT, "specified too many format options");
        else if (lfflg && llflg)
                fmt = FL5FMT;
        else if (lfflg)
                fmt = F5FMT;
        else if (llflg)
                fmt = L5FMT;

        return(fmt);
}

add_int(i, p, n)
        int i;
        int *p[];
        int *n;
{
        int *ip;
        
        if (*p == NULL)
                *p = (int *)malloc(sizeof(int));
        else
                *p = (int *)realloc(*p, (*n + 1) * sizeof(int));
        ip = *p;
        
        ip[(*n)++] = i;
}

check_int(i, p, n)
        int i;
        int p[];
        int n;
{
        int m;
        
        /* if list is empty; return TRUE */
        if (n == 0)
                return TRUE;
        
        for (m = 0; m < n; m++)
                if (p[m] == i)
                        return TRUE;
        return FALSE;
}

pscomp(p1, p2)
	proc_info_t	p1, p2;
{
	register int i;

	if (sortby == SORTCPU) {
	     return (p2->cpu_usage > p1->cpu_usage ? 1 : -1);
	}

	if (sortby == SORTMEM) {
	    return (p2->resident_size - p1->resident_size);
	}

	i = (int)p1->tty - (int)p2->tty;
	if (i == 0)
	    i = p1->pid - p2->pid;
	return (i);
}

parsetty(tp)
        char *tp;
{
        struct stat stbuf;
        char *tname, *ttyname();
        char termname[MAXPATHLEN+1];

        while((tname = strtok(tp, FMTSEP)) != NULL) {
                if (strlen(tname) == 2) {
                        if (strcmp(tname, "co") == 0)
                                strcpy(termname, "/dev/console");
                        else {
                                strcpy(termname, "/dev/tty");
                                strcat(termname, tname);
                        }
                }
                else if (*tname != '/') {
                        strcpy(termname, "/dev/");
                        strcat(termname, tname);
                } else
                        strcpy(termname, tname);
                if (stat(termname, &stbuf) == -1)
                        syserror(termname);
                if ((stbuf.st_mode & S_IFMT) != S_IFCHR)
                        error(NOT_TTY, "%s: not a terminal", termname);
                add_tty(stbuf.st_rdev);
                tp = NULL;
        }
        return(0);
}

parsepid(pp)
        char *pp;
{
        char *spid;

        while((spid = strtok(pp, FMTSEP)) != NULL) {
                if (isdigit(*spid))
                        add_pid(atoi(spid));
                else
                        error(NOT_INT, "argument must be an integer");
                pp = NULL;
        }
        return(0);
}

parsepgrp(gp)
        char *gp;
{
        char *spgrp;

        while((spgrp = strtok(gp, FMTSEP)) != NULL) {
                if (isdigit(*spgrp))
                        add_pgrp(atoi(spgrp));
                else
                        error(NOT_INT, "argument must be an integer");
                gp = NULL;
        }
        return(0);
}

parsesess(sp)
        char *sp;
{
        char *sessp;

        while((sessp = strtok(sp, FMTSEP)) != NULL) {
                if (isdigit(*sessp))
                        add_sess(atoi(sessp));
                else
                        error(NOT_INT, "argument must be an integer");
                sp = NULL;
        }
        return(0);
}

parseuid(up)
        char *up;
{
        struct passwd *pw;
        char *suid;
        int uid;

        while((suid = strtok(up, FMTSEP)) != NULL) {
                if (isdigit(*suid)) {
                        uid = atoi(suid);
                }
                else {
                        if ((pw = getpwnam(suid)) == NULL)
                                error(NO_USER, "cannot find user: \"%s\"", suid);
                        uid = pw->pw_uid;
                }
                
                add_uid(uid);
                up = NULL;
        }
        return(0);
}

print_all_tasks()
{
        var_t v;
        int i, nthread;
        proc_info_t pi;
        
        /*
         * scan requested variables, noting what structures are needed,
         * and adjusting header widths as appropiate.
         */
        scanvars();

        max_proc_table = 200;
        proc_table = (proc_info_t)calloc((unsigned)max_proc_table,
                                         sizeof(struct proc_info));
        get_proc_table();

	/*
	 * print header
	 */
	printheader();
	if (nprocs == 0)
                return;
        
        /*
         * sort proc list
         */
        qsort((char *)proc_table, nprocs, sizeof(struct proc_info), pscomp);

        /*
         * for each proc, call each variable output function.
         */
        for (i = 0; i < nprocs; i++) {
                pi = &proc_table[i];
                for (v = vhead; v != NULL; v = v->next) {
                        (*v->oproc)(pi, v);
                        if (v->next != NULL)
                                putchar(' ');
                }
                putchar('\n');
                if (prtheader && lineno++ == prtheader-4) {
                        putchar('\n');
                        printheader();
                        lineno = 0;
                }
                if (mflg && pi->num_threads > 1) {
                        for (nthread = 0; nthread < pi->num_threads; nthread++) {
                                for (v = vhead; v != NULL; v = v->next) {
                                        if (v->flag & THREAD)
                                                (*v->oproc)(&pi->threads[nthread], v);
                                        else
                                                empty(&pi->threads[nthread], v);
                                        if (v->next != NULL)
                                                putchar(' ');
                                }
                                putchar('\n');
                                if (prtheader && lineno++ == prtheader-4) {
                                        putchar('\n');
                                        printheader();
                                        lineno = 0;
                                }
                        }
                }
        }
}

/*
 *	Translate thread state to a number in an ordered scale.
 *	When collapsing all the threads' states to one for the
 *	entire task, the lower-numbered state dominates.
 */
#define	STATE_MAX	7

int
mach_state_order(s, sleep_time)
        int s;
        long sleep_time;
 {
    switch (s) {
    case TH_STATE_RUNNING:      return(1);
    case TH_STATE_UNINTERRUPTIBLE:
                                return(2);
    case TH_STATE_WAITING:      return((sleep_time > 20) ? 4 : 3);
    case TH_STATE_STOPPED:      return(5);
    case TH_STATE_HALTED:       return(6);
    default:                    return(7);
    }
 }
			    /*01234567 */
char	mach_state_table[] = " RUSITH?";

char *
digits(n)
    float	n;
{
        static char	tmp[10];	/* STATIC! */
        
        if ((n > 0) && (n < 10))
                sprintf(tmp, "%4.2f", n);
        else if ((n > 0) && (n < 100))
                sprintf(tmp, "%4.1f", n);
        else
                sprintf(tmp, "%4.0f", n);
        return(tmp);
}

char *
mem_to_string(n)
    vm_size_t	n;
{
        static char buf[12];

        /* convert to bytes */
        n /= 1024;

        if (n > 1024*1024)
                sprintf(buf, "%sG", digits(((float)n)/(1024.0*1024.0)));
        else if (n > 1024)
                sprintf(buf, "%sM", digits((float)n/(1024.0)));
        else
                sprintf(buf, "%dK", n);

        return(buf);
}

parsefmt(fmt)
	char *fmt;
{
	char *f = fmt, *cp, *hp;
	struct var *v;
	char newbuf[1024], *nb = newbuf; /* XXX */
	char *lookupcombo();
	struct var *lookupvar();

	/*
	 * strtok is not &^%^& re-entrant, so we have
	 * only one level of expansion, looking for combo
	 * variables once here, and expanding the string
	 * before really parsing it.  With strtok_r,
	 * you would move the expansion to before the
	 * lookupvar inside the 2nd while loop with a
	 * recursive call to parsefmt.
	 */
	while ((cp = strtok(f, FMTSEP)) != NULL) {
		if ((hp = lookupcombo(cp)) == NULL)
			hp = cp;
		if (((nb + strlen(hp)) - newbuf) >= 1024)
			error(FMT_2BIG, "format too large");
		strcpy(nb, hp);
		while (*nb)
			nb++;
		*nb++ = ' ';
		*nb =  '\0';
		f = NULL;
	}
	f = newbuf;
	while ((cp = strtok(f, FMTSEP)) != NULL) {
		if (hp = index(cp, '='))
			*hp++ = '\0';
		v = lookupvar(cp);
		if (v == NULL)
			error(UNKNOWN_VAR, "unknown variable in format: %s", cp);
		if (v->next != NULL || vtail == v)
			error(VAR_2X, "can't specify a variable twice: %s", cp);
		if (hp)
			v->header = savestr(hp);
		if (vhead == NULL)
			vhead = vtail = v;
		else {
			vtail->next = v;
			vtail = v;
		}
		f = NULL;	/* for strtok */
	}
}

scanvars()
{
	register i;
	register struct var *v;

	for (v = vhead; v != NULL; v = v->next) {
		i = strlen(MSGSTR(v->header_msg, v->header));
		if (v->width < i)
			v->width = i;
		totwidth += v->width + 1;	/* +1 for space */
		if (v->flag & USER)
			needuser = 1;
		if (v->flag & COMM)
			needcomm = 1;
	}
	totwidth--;
}

printheader()
{
	register struct var *v;

	for (v = vhead; v != NULL; v = v->next) {
		if (v->flag & LJUST) {
			if (v->next == NULL) {
                                /* last one */
                                if (termwidth == UNLIMITED)
                                        printf("%s", MSGSTR(v->header_msg, v->header));
                                else {
                                        register left = termwidth - (totwidth - v->width);
                                        register char *cp = MSGSTR(v->header_msg, v->header);

                                        /* already wrapped, just use std width */
                                        if (left < 1)
                                                left = v->width;
                                        while (left-- && *cp)
                                                putchar(*cp++);
                                }
                        }
			else
				printf("%-*s",v->width, MSGSTR(v->header_msg, v->header));
		} else
			printf("%*s",v->width, MSGSTR(v->header_msg, v->header));
		if (v->next != NULL)
			putchar(' ');
	}
	putchar('\n');
}

command(pi, v) 
        proc_info_t pi;
        var_t v;
{
	if (v->next == NULL) {		
		/* last field */
		if (termwidth == UNLIMITED)
			printf("%s", pi->args);
		else {
			register left = termwidth - (totwidth - v->width);
			register char *cp = pi->args;

			if (left < 1)	/* already wrapped, just use std width */
				left = v->width;
			while (left-- && *cp)
				putchar(*cp++);
		}
	} else
		printf("%-*.*s", v->width, v->width, pi->args);
}

ucomm(pi, v) 
        proc_info_t pi;
        var_t v;
{
	if (v->next == NULL) {		
		/* last field */
		if (termwidth == UNLIMITED)
			printf("%s", pi->command);
		else {
			register left = termwidth - (totwidth - v->width);
			register char *cp = pi->command;

			if (left < 1)	/* already wrapped, just use std width */
				left = v->width;
			while (left-- && *cp)
				putchar(*cp++);
		}
	} else
		printf("%-*.*s", v->width, v->width, pi->command);
}

logname(pi, v)
        proc_info_t pi;
        var_t v;
{
        if (pi->u)
                printf("%-*s", v->width, pi->u->u_logname);
        else
                printf("%-*s", v->width, " ");
        
}

state(pi, v)
        proc_info_t pi;
        var_t v;
{
        static char buf[4];
        char *cp = buf;
        
        *cp++ = mach_state_table[pi->state];
        if (pi->all_swapped)
                *cp++ = 'W';
        else
                *cp++ = ' ';
        if (pi->base_pri > NZERO)
                *cp++ = 'N';
	else if (pi->base_pri < NZERO)
                *cp++ = '<';
        else
                *cp++ = ' ';
        if ((pi->flag & SCTTY) && pi->pgrp == pi->tpgrp)
                *cp++ = '+';
        *cp = '\0';
        printf("%-*s", v->width, buf);
}

pri(pi, v)
        proc_info_t pi;
        var_t v;
{
	printf("%*d", v->width, pi->pri - PZERO);
}

uname(pi, v)
        proc_info_t pi;
        var_t v;
{
	register struct passwd *pw;
	struct passwd *getpwuid();

	pw = getpwuid(pi->uid);
	if (pw == NULL) {
		return( NULL );
	}
	printf("%-*s", v->width, pw->pw_name);
}

runame(pi, v)
        proc_info_t pi;
        var_t v;
{
	register struct passwd *pw;
	struct passwd *getpwuid();

	pw = getpwuid(pi->ruid);
	if (pw == NULL) {
		return( NULL );
	}
	printf("%-*s", v->width, pw->pw_name);
}

tdev(pi, v)
        proc_info_t pi;
        var_t v;
{
	dev_t dev = pi->tty;

	if (dev == NODEV)
		printf("%*s", v->width, "??");
	else {
		char buff[16];

		sprintf(buff, "%d/%d", major(dev), minor(dev));
		printf("%*s", v->width, buff);
	}
}

extern char *devname();

tname(pi, v)
        proc_info_t pi;
        var_t v;
{
        dev_t dev = pi->tty;
        char *tname;

        if (dev == NODEV || (tname = devname(dev, S_IFCHR)) == NULL)
                printf("%-*s", v->width, "??");
        else {
                if (strncmp(tname, "tty", 3) == 0)
                        tname += 3;
                printf("%*.*s%c", v->width-1, v->width-1, tname,
                       (pi->flag&SCTTY)?' ':'-');
        }
}

longtname(pi, v)
        proc_info_t pi;
        var_t v;
{
        dev_t dev = pi->tty;
        char *tname;

        if (dev == NODEV || (tname = devname(dev, S_IFCHR)) == NULL)
                printf("%-*s", v->width, "??");
        else
                printf("%-*s", v->width, tname);
}

/*
 * Print start time. If time is less than 24hrs then print the time
 * otherwise print the start date.
 */
#undef	u_start

started(pi, v)
        proc_info_t pi;
        var_t v;
{
        char *tp = NULL;
#if defined(NLS) ||defined (KJI)
#endif

        if (pi->u) {
#if defined(NLS) ||defined (KJI)
                char time_str[9];
                struct tm *sttm;
                time_t stime = pi->u->u_start.tv_sec;
                
                sttm = localtime(&stime);
                if (time((time_t *)0) -  stime < 24L*60L*60L)
                        (void)strftime(time_str, sizeof(time_str), "%8X", sttm);
                else
                        (void)strftime(time_str, sizeof(time_str), "%8sD", sttm);
                tp = time_str;
#else
                tp = attime(&pi->u->u_start.tv_sec);
#endif
        }
        else
                tp = "-";
        printf("%-*s", v->width, tp);
}

lstarted(pi, v)
        proc_info_t pi;
        var_t v;
{
	char *tp; 

        if (pi->u) {
#if defined(NLS) ||defined (KJI)
                char time_str[29];
                struct tm *sttm;

                sttm = localtime(&pi->u->u_start.tv_sec);
                (void) strftime(time_str, sizeof(time_str), "%28c", sttm);
                tp = time_str;
#else
                extern char *ctime();
		(tp = ctime(&pi->u->u_start.tv_sec))[24] = '\0';
#endif
        }                
	else
		tp = "-";
	printf("%-*s", v->width, tp);
}

wchan(pi, v)
        proc_info_t pi;
        var_t v;
{
#ifdef MACH_25
        char *mesg = pi->wait_mesg;

        switch (pi->wait_chan) {
        case 0L:
                printf("%-*s", v->width, "-");
                break;
        case -1L:
                printf("%-*s", v->width, "*");
                break;
        default:
                if (mesg && *mesg) 	/* was && pi->state != 2) */
                        printf("%-*.*s", v->width, v->width, mesg);
                else
                        printf("%*lx", v->width, (pi->wait_chan & ~KERNBASE));
                break;
        }
#else
        printf("%-*s", v->width, "*");
#endif

}
        
vsize(pi, v)
        proc_info_t pi;
        var_t v;
{
        printf("%*.*s", v->width, v->width, mem_to_string(pi->virtual_size));
}

rsize(pi, v)
        proc_info_t pi;
        var_t v;
{
        printf("%*.*s", v->width, v->width, mem_to_string(pi->resident_size));
}

cputime(pi, v)
        proc_info_t pi;
        var_t v;
{
	long secs;
	long psecs;	/* "parts" of a second. first micro, then centi */
#undef	u_cru
        
	if (pi->status == PI_ZOMBIE || pi->u == NULL) {
		secs = 0;
		psecs = 0;
	} else {
		secs = pi->p_utime.seconds + 
			pi->p_stime.seconds;
		psecs = pi->p_utime.microseconds + 
			pi->p_stime.microseconds;
		if (sumrusage) {
			secs += pi->u->u_cru.ru_utime.tv_sec + 
				pi->u->u_cru.ru_stime.tv_sec;
			psecs += pi->u->u_cru.ru_utime.tv_usec + 
				pi->u->u_cru.ru_stime.tv_usec;
		}
		/*
		 * round and scale to 100's
		 */
		psecs = (psecs + 5000) / 10000;
		if (psecs >= 100) {
			psecs -= 100;
			secs++;
		}
	}
        ptime(v, secs, psecs);
}

usertime(pi, v)
        proc_info_t pi;
        var_t v;
{
	long secs;
	long psecs;	/* "parts" of a second. first micro, then centi */
        
	if (pi->status == PI_ZOMBIE || pi->u == NULL) {
		secs = 0;
		psecs = 0;
	} else {
		secs = pi->p_utime.seconds;
		psecs = pi->p_utime.microseconds;

		/*
		 * round and scale to 100's
		 */
		psecs = (psecs + 5000) / 10000;
		if (psecs >= 100) {
			psecs -= 100;
			secs++;
		}
	}
        ptime(v, secs, psecs);
}

systime(pi, v)
        proc_info_t pi;
        var_t v;
{
	long secs;
	long psecs;	/* "parts" of a second. first micro, then centi */
        
	if (pi->status == PI_ZOMBIE || pi->u == NULL) {
		secs = 0;
		psecs = 0;
	} else {
		secs = pi->p_stime.seconds;
		psecs = pi->p_stime.microseconds;

		/*
		 * round and scale to 100's
		 */
		psecs = (psecs + 5000) / 10000;
		if (psecs >= 100) {
			psecs -= 100;
			secs++;
		}
	}
        ptime(v, secs, psecs);
}

ptime(v, secs, psecs)
        var_t v;
        int secs, psecs;
{
	char obuff[128];

	sprintf(obuff, "%3ld:%02ld.%02ld", secs/60, secs%60, psecs);
	printf("%*s", v->width, obuff);
}

pcpu(pi, v)
        proc_info_t pi;
        var_t v;
{
        double percent_cpu;
        
        percent_cpu = usage_to_percent(pi->cpu_usage) + (usage_to_tenths(pi->cpu_usage)/10);
        printf("%*.1f", v->width, percent_cpu);
}

pmem(pi, v)
        proc_info_t pi;
        var_t v;
{
	printf("%*.1f", v->width, pi->percent_mem);
}

maxrss(pi, v)    /* dummy */
        proc_info_t pi;
        var_t v;
{
        printf("%*s", v->width, "-");
}

pagein(pi, v)
        proc_info_t pi;
        var_t v;
{
#undef	u_ru
	printf("%*d", v->width, pi->u ? pi->u->u_ru.ru_majflt : 0);
}

nice(pi, v)
        proc_info_t pi;
        var_t v;
{
	printf("%*d", v->width, pi->base_pri - NZERO);
}
        
/*
 * Generic output routines.  Print fields from various prototype
 * structures.
 */
pvar(pi, v) 
        proc_info_t pi;
        var_t v;
{
	printval((char *)((char *)pi + v->off), v);
}

uvar(pi, v) 
        proc_info_t pi;
        var_t v;
{
 
	if (pi->u)
		printval((char *)((char *)pi->u + v->off), v);
	else
		printf("%*s", v->width, "-");
}

rvar(pi, v)
        proc_info_t pi;
        var_t v;
{
	
	if (pi->u)
		printval((char *)((char *)(&pi->u->u_ru) + v->off), v);
	else
		printf("%*s", v->width, "-");
}

empty(pi, v) 
        proc_info_t pi;
        var_t v;
{
	if (v->next == NULL && termwidth != UNLIMITED) {		
		/* last field */
                register left = termwidth - (totwidth - v->width);
                if (left < 1)	/* already wrapped, just use std width */
                        left = v->width;
                while (left--)
                        putchar(' ');
        }
	else
		printf("%-*.*s", v->width, v->width, " ");
}

struct var *
lookupvar(cp)
	char *cp;
{
	register int i, j;

	for (i=0; var[i].name[0] != NULL; i++)
		for (j=0; var[i].name[j] != NULL; j++)
			if (strcmp(cp, var[i].name[j]) == 0)
				return (&var[i]);
	return (NULL);
}

char *
lookupcombo(cp)
	char *cp;
{
	register struct combovar *cv = &combovar[0];

	for (; cv->name; cv++)
                if (strcmp(cp, cv->name) == 0) {
			return (cv->replace);
                }
	return (NULL);
}

printval(bp, v)
	char *bp;
	struct var *v;
{
	static char ofmt[32] = "%";
	register char *cp = ofmt+1, *fcp = v->fmt;

	if (v->flag & LJUST)
		*cp++ = '-';
	*cp++ = '*';
	while (*cp++ = *fcp++)
		;

	switch (v->type) {
	case CHAR:
		printf(ofmt, v->width, *(char *)bp);
		break;

	case UCHAR:
		printf(ofmt, v->width, *(u_char *)bp);
		break;

	case SHORT:
		printf(ofmt, v->width, *(short *)bp);
		break;

	case USHORT:
		printf(ofmt, v->width, *(u_short *)bp);
		break;

	case LONG:
		printf(ofmt, v->width, *(long *)bp);
		break;

	case ULONG:
		printf(ofmt, v->width, *(u_long *)bp);
		break;

	case KPTR:
		printf(ofmt, v->width, *(u_long *)bp & ~KERNBASE);
		break;

	default:
		error(UNKNOWN_TYPE, "unknown type %d", v->type);
	}
}

/* All of this should come out of the process manager... */

get_proc_table()
{
        register int i,j;
        long	nproc;
#define NPROC    512
        struct tbl_procinfo proc[NPROC];
        struct tbl_procinfo *mproc;
    
        nproc = table(TBL_PROCINFO, 0, (char *)0, 32767, 0);

        for (i=0; i < nproc; i += NPROC) {
                j = nproc - i;
                if (j > NPROC)
                        j = NPROC;
                j = table(TBL_PROCINFO, i, (char *)proc, NPROC, sizeof(proc[0]));
                for (j = j-1; j >= 0; j--) {
                        mproc = &proc[j];
                        if (mproc->pi_status == PI_EMPTY)
                                continue;
                        if (!gflg && !xflg) {
                                if (mproc->pi_status == PI_ZOMBIE ||
                                    mproc->pi_status == PI_EXITING)
                                        continue;
                        }
#if SEC_MAC
			if (!ps_proc_dominate(mproc->pi_pid))
			        continue;
#endif
                        
                        if (num_uids_to_check) {
                                if (check_uid(mproc->pi_uid))
                                        save(mproc);
                        }
                        else if (num_ttys_to_check) {
                                if (check_tty(mproc->pi_ttyd))
                                        save(mproc);
                        }
                        else if (num_pgrps_to_check) {
                                if (check_pgrp(mproc->pi_pgrp))
                                        save(mproc);
                        }
                        else if (num_sess_to_check) {
                                if (check_sess(mproc->pi_session))
                                        save(mproc);
                        }
                        else if (num_pids_to_check) {
                                if (check_pid(mproc->pi_pid))
                                        save(mproc);
                        }
                        else
                                save(mproc);
                }
        }
#undef	NPROC
}

save(mproc)
        struct tbl_procinfo *mproc;
{
        proc_info_t pi;
        struct user u;
        task_t task;
	kern_return_t	rc;

        if (!xflg && (mproc->pi_ttyd == NODEV || (mproc->pi_flag & SCTTY) == 0))
                return;
#if 0
        else if (!gflg && !xflg && mproc->pi_pgrp == mproc->pi_pid)
                return;
#endif
        
        nprocs++;
        if (nprocs > max_proc_table) {
                max_proc_table *= 2;
                proc_table = (proc_info_t)realloc((char *)proc_table,
                                                  (unsigned)max_proc_table*sizeof(*proc_table));
        }
        pi = &proc_table[nprocs-1];
        pi->tty		= mproc->pi_ttyd;
        pi->uid		= mproc->pi_uid;
        pi->ruid	= mproc->pi_ruid;
        pi->svuid	= mproc->pi_svuid;
        pi->rgid	= mproc->pi_rgid;
        pi->svgid	= mproc->pi_svgid;
        pi->pid		= mproc->pi_pid;
        pi->ppid	= mproc->pi_ppid;
        pi->pgrp	= mproc->pi_pgrp;
        pi->session	= mproc->pi_session;
        pi->tpgrp	= mproc->pi_tpgrp;
        pi->tsession	= mproc->pi_tsession;
        pi->jobc	= mproc->pi_jobc;
        pi->status	= mproc->pi_status;
        pi->flag	= mproc->pi_flag;
        pi->command	= savestr(mproc->pi_comm);
        pi->p_cursig	= (char)mproc->pi_cursig;
        pi->p_sig	= mproc->pi_sig;
        pi->p_sigmask	= mproc->pi_sigmask;
        pi->p_sigignore = mproc->pi_sigignore;
        pi->p_sigcatch	= mproc->pi_sigcatch;

        /*
         *	Find the other stuff
         */
	switch (mach_kernel) {
	case 3:
		task = task_by_pid(pi->pid);
#ifndef	MACH_PORT_VALID
	/* from the Mach3.0 mach/port.h */
#define MACH_PORT_NULL	(0)
#define MACH_PORT_DEAD	(~0)
#define MACH_PORT_VALID(name)	\
	(((name) != MACH_PORT_NULL) && ((name) != MACH_PORT_DEAD))
#endif
		if (!MACH_PORT_VALID(task))
			rc = KERN_FAILURE;
		else
			rc = KERN_SUCCESS;
		break;
	default:
		rc = task_by_pid(mach_task_self(), pi->pid, &task);
		break;
	}
		
        if (rc != KERN_SUCCESS) {
                pi->status = PI_ZOMBIE;
        }
        else {
                task_basic_info_data_t	ti;
                unsigned int		count;
                thread_array_t		thread_table;
                unsigned int		table_size;
                host_basic_info_data_t	hi;

                thread_basic_info_t	thi;
                thread_basic_info_data_t thi_data;
                int			i, t_state;

                count = TASK_BASIC_INFO_COUNT;
                if (task_info(task, TASK_BASIC_INFO, (task_info_t)&ti,
                              &count)
                    != KERN_SUCCESS) {
                        pi->status = PI_ZOMBIE;
                }
                else {
			table_size = HOST_BASIC_INFO_COUNT;
                        (void) host_info(mach_host_self(), HOST_BASIC_INFO,
							&hi, &table_size);

                        pi->virtual_size = ti.virtual_size;
                        pi->resident_size = ti.resident_size;

                        pi->percent_mem = ((double)ti.resident_size /
                                           (double)hi.memory_size) * 100.0;

                        (void)task_threads(task, &thread_table, &table_size);
                        
                        pi->p_utime = ti.user_time;
                        pi->p_stime = ti.system_time;

                        pi->state = STATE_MAX;
                        pi->pri = 255;
                        pi->base_pri = 255;
                        pi->all_swapped = TRUE;
                        pi->cpu_usage = 0;
                        pi->slptime = 0;
                        pi->wait_chan = 0;
                        pi->suspend_count = 0;
#ifdef MACH_25
                        *pi->wait_mesg = '\0';
#endif
                        pi->u = NULL;

                        pi->num_threads = table_size;
                        thi = &thi_data;

                        /* Allocate space to store thread info */
                        if (mflg) {
                                if ((pi->threads = (proc_info_t)malloc(table_size * sizeof(struct proc_info))) == NULL)
                                        error(NO_MEM, "out of memory");
                        }
                        
                        for (i = 0; i < table_size; i++) {
                                count = THREAD_BASIC_INFO_COUNT;
                                if (thread_info(thread_table[i], THREAD_BASIC_INFO,
                                                (thread_info_t)thi, &count) == KERN_SUCCESS) {
                                        /* Save per thread info */
                                        if (mflg) {
                                                proc_info_t ti = &pi->threads[i];
                                                ti->cpu_usage	= thi->cpu_usage;
                                                ti->state	= mach_state_order(thi->run_state, thi->sleep_time);
                                                ti->all_swapped = ((thi->flags&TH_FLAGS_SWAPPED) != 0);
                                                ti->base_pri	= thi->base_priority;
                                                ti->pri		= thi->cur_priority;

#ifdef MACH_25
                                                ti->wait_chan	= thi->wait_event;
					        strncpy(ti->wait_mesg, thi->wait_mesg, WMESGLEN);
#endif                                           
                                                ti->p_utime	= thi->user_time;
                                                ti->p_stime	= thi->system_time;
                                                ti->slptime	= thi->sleep_time;
                                                ti->suspend_count = thi->suspend_count;
                                                ti->u		= (struct user *)-1;    /* non-null */

                                        }
                                        /* Save task summary info */
                                        time_value_add(&pi->p_utime, &thi->user_time);
                                        time_value_add(&pi->p_stime, &thi->system_time);
                                        t_state = mach_state_order(thi->run_state,
                                                                   thi->sleep_time);
                                        if (t_state < pi->state)
                                                pi->state = t_state;
                                        if (thi->cur_priority < pi->pri)
                                                pi->pri = thi->cur_priority;
                                        if (thi->base_priority < pi->base_pri)
                                                pi->base_pri = thi->base_priority;
                                        if ((thi->flags & TH_FLAGS_SWAPPED) == 0)
                                                pi->all_swapped = FALSE;
                                        pi->cpu_usage += thi->cpu_usage;
                                        pi->slptime += thi->sleep_time;
                                        pi->suspend_count += thi->suspend_count;
                                        
                                        /*
                                         * If more that one
                                         * thread is sleeping
                                         * then set to -1.
                                         */
#ifdef MACH_25
                                        if (pi->wait_chan && thi->wait_event)
                                                pi->wait_chan = -1;
                                        else if (thi->wait_event) {
                                                pi->wait_chan = thi->wait_event;
                                                strncpy(pi->wait_mesg, thi->wait_mesg, WMESGLEN);
                                        } 
#endif
                                }
                        }
                }
        }

        /*
         * If this is a ZOMBIE don't get the u_area and use defunct
         * as the command string.
         */
        if (pi->status == PI_ZOMBIE) {
                pi->args = pi->command = MSGSTR(ZOMBIE_STR, "<defunct>");
                return;
        }
        
        /*
         * Get u.area if needed
         */
        if (needuser) {
                if (table(TBL_UAREA, pi->pid, (char *)&u, 1, sizeof(u)) == 1) {
                        if ((pi->u = (struct user *)malloc(sizeof(struct user))) == NULL)
                                error(NO_MEM, "out of memory");
                        else
                                bcopy(&u, pi->u, sizeof(u));
                }
                else {
                        /* error(NO_UAREA, "failed to get user info for process %d", pi->pid); */
			/* We lost the process - must have exited. */
			free((caddr_t)pi->u);
			pi->u = NULL;
		}
        }

        /*
         * If this is EXITING use exiting as command string
         */
        if (pi->status == PI_EXITING) {
                pi->args = pi->command = MSGSTR(EXITING_STR, "<exiting>");
                return;
        }

        /*
         * now go find the command arguments and environment
         */
        if (needcomm) {
                /*
                 *	Get command and arguments.
                 */
                int		arg_length, env_length;
                register char	*ap, *ep, *cp;

                bzero(arguments, arguments_size);
                if (table(TBL_ARGUMENTS, pi->pid, arguments, 1, arguments_size) != 1) {
                        /* table command failed, use the short command */
                        sprintf(arguments, "[%s]", mproc->pi_comm);
                }
                ap = arguments;

                /* Find length of total arguments */
                for (cp = &arguments[arguments_size - 1]; (*cp == '\0' || *cp == ' ') && cp > ap; --cp);
                arg_length = cp - ap + 1;

                /* Concat arguments */
                for (cp = ap; cp < &arguments[arg_length]; cp++)
                        if (*cp == '\0')
                                *cp = ' ';

                if (ap[0] == '-' || ap[0] == '?' || ap[0] <= ' ') {
                        /*
                         *	Not enough information - add short command name
                         */
                        pi->args = malloc((unsigned)arg_length
                                          + sizeof(mproc->pi_comm)
                                          + 4);
                        (void) strncpy(pi->args, ap, arg_length);
			pi->args[arg_length] = '\0';
                        (void) strcat(pi->args, " (");
                        (void) strncat(pi->args, mproc->pi_comm,
                                       sizeof(mproc->pi_comm));
                        (void) strcat(pi->args, ")");
                }
                else {
                        pi->args = malloc((unsigned)arg_length + 1);
                        (void) strncpy(pi->args, ap, arg_length);
			pi->args[arg_length] = '\0';
                }

                /*
                 * Get environment string
                 */
                ep = &arguments[arg_length];
                if (eflg) {
                        if (table(TBL_ENVIRONMENT, pi->pid, ep, 1, (arguments_size-arg_length)) == 1) {
                                /* Find length of total environment strings */
                                for (cp = &arguments[arguments_size - 1]; (*cp == '\0' || *cp == ' ') && cp > ep; --cp);
                                env_length = cp - ep + 1;

                                /* Concat arguments */
                                for (cp = ep; cp < &ep[env_length]; cp++)
                                        if (*cp == '\0')
                                                *cp = ' ';

                                /* Add to agument string */
                                pi->args = realloc(pi->args, strlen(pi->args) + (unsigned)env_length + 1);
                                (void) strcat(pi->args, " ");
                                (void) strcat(pi->args, ep);
                        }
                }
        }
}

char *
savestr(cp)
	char *cp;
{
	register unsigned len;
	register char *dp;

	len = strlen(cp);
	dp = (char *)calloc(len+1, sizeof (char));
	(void) strcpy(dp, cp);
	return (dp);
}

error(msg, a, b, c, d, e)
        int msg;
	char *a, *b, *c, *d, *e;
{
	fprintf(stderr, "ps: ");
	fprintf(stderr, MSGSTR(msg, a), b, c, d, e);
	fprintf(stderr, "\n");
	exit(1);
}

syserror(a)
	char *a;
{
	extern errno;

	error("%s: %s", a, strerror(errno));
}

/*
 * ICK (all for getopt), would rather hide the ugliness
 * here than taint the main code.
 *
 *  ps foo -> ps -foo
 *  ps 34 -> ps -p34
 *
 * The old convention that 't' with no trailing tty arg means the users
 * tty, is only supported if argv[1] doesn't begin with a '-'.  This same
 * feature is available with the option 'T', which takes no argument.
 */
char *
kludge_bsdps_options(s)
	char *s;
{
	int len = strlen(s), numlen = 0;
	char *newopts, *ns, *cp;

	if ((newopts = ns = (char *)calloc(len+2, 1)) == NULL)
		error(NO_MEM, "out of memory");
	/*
	 * options begin with '-'
	 */
	if (*s != '-')
		*ns++ = '-';	/* add option flag */
	/*
	 * gaze to end of argv[1]
	 */
	cp = s + len - 1;
	/*
	 * if last letter is a 't' flag with no argument (in the context
	 * of the oldps options -- option string NOT starting with a '-' --
	 * then convert to 'T' (meaning *this* terminal, i.e. ttyname(0).
	 */
	if (*cp == 't' && *s != '-')
		*cp = 'T';
	else {
		/*
		 * otherwise check for trailing number, which *may* be a
		 * pid.
		 */
		while (isdigit(*cp)) {
			--cp;
			numlen++;
		}
	}
	cp++;
	bcopy(s, ns, cp - s);	/* copy everything up to trailing number */
	while (*ns)
		ns++;
	/*
	 * if there's a trailing number, and not a preceding 'p' (pid) or
	 * 't' (tty) flag, then assume it's a pid and insert a 'p' flag.
	 */
	if (isdigit(*cp) && (cp == s || *(cp-1) != 't' && *(cp-1) != 'p' &&
	   ((cp-1) == s || *(cp-2) != 't')))
		*ns++ = 'p';
	strcat(ns, cp);		/* and append the number */

	return (newopts);
}
