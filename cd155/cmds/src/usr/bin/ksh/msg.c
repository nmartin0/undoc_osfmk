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
static char rcsid[] = "@(#)$RCSfile: msg.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:27:24 $";
#endif
/*
 * COMPONENT_NAME: (CMDKSH) Korn shell
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 26, 27
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
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * Copyright 1976, Bell Telephone Laboratories, Inc.
 */
/*

 *      Copyright (c) 1984, 1985, 1986, 1987, 
 *                  1988, 1989   AT&T
 *      All Rights Reserved

 *      THIS IS UNPUBLISHED PROPRIETARY SOURCE 
 *      CODE OF AT&T.
 *      The copyright notice above does not 
 *      evidence any actual or intended
 *      publication of such source code.

 */
/*
 *	UNIX shell
 *	S. R. Bourne
 *	Rewritten by David Korn
 *
 *	AT&T Bell Laboratories
 *
 */


#include	<errno.h>
#include	"defs.h"
#include	"sym.h"
#include	"builtins.h"
#include	"test.h"
#include	"timeout.h"
#include	"history.h"

#ifdef NLS
    MESG e_version = "\n@(#)OSF/1 Version 11/16/88d NLS\0\n"; /*NOTX*/
#else
# ifdef KJI
    MESG e_version = "\n@(#)OSF/1 Version 11/16/88d KJI\0\n"; /*NOTX*/
# else
#  ifdef MULTIBYTE
#   include     "national.h"
    MESG e_version = "\n@(#)OSF/1 Version M-11/16/88d\0\n"; /*NOTX*/
#  else
    MESG e_version = "\n@(#)OSF/1 Version 11/16/88d\0\n"; /*NOTX*/
#  endif /* MULTIBYTE */
# endif /* KJI */
#endif /* NLS */

extern struct Bfunction sh_randnum;
extern struct Bfunction sh_seconds;
extern struct Bfunction line_numbers;

/* error messages */
#ifdef MSG
/* below, the comment NOTX means that the string was not externalized */
#endif
MESG	e_tcsetpgrp     = "Error setting foreground process group.\n";
MESG	e_resetld	= "Unable to reset line disciplines\n";
MESG	e_setld		= "Unable to set line disciplines\n";
MESG	e_timewarn	= "\r\n\007shell time out in 60 seconds";
MESG	e_timeout	= "timed out waiting for input";
MESG	e_mailmsg	= "you have mail in $_";
MESG	e_query		= "no query process";
MESG	e_history	= "no history file";
MESG	e_option	= "bad option(s)";
MESG	e_space		= "no space";
MESG	e_argexp	= "argument expected";
MESG	e_bracket	= "] missing";
MESG	e_number	= "bad number";
MESG	e_nullset	= "parameter null or not set";
MESG	e_notset	= "parameter not set";
MESG	e_subst		= "bad substitution";
MESG	e_create	= "cannot create";
MESG	e_restricted	= "restricted";
MESG	e_fork		= "cannot fork: too many processes";
MESG	e_pexists	= "process already exists";
MESG	e_fexists	= "file already exists";
MESG	e_swap		= "cannot fork: no swap space";
MESG	e_pipe		= "cannot make pipe";
MESG	e_open		= "cannot open";
MESG	e_logout	= "Use 'exit' to logout";
MESG	e_arglist	= "arg list too long";
MESG	e_txtbsy	= "text busy";
MESG	e_toobig	= "too big";
MESG	e_exec		= "cannot execute";
MESG	e_pwd		= "cannot access parent directories";
MESG	e_found		= " not found";
MESG	e_flimit	= "too many open files";
MESG	e_ulimit	= "exceeds allowable limit";
MESG	e_subscript	= "subscript out of range";
MESG	e_nargs		= "bad argument count";
MESG	e_inlib		= "inlib install failed";
MESG	e_rmlib		= "rmlib removal failed";
#ifdef ELIBACC
    /* shared library error messages */
    MESG	e_libacc 	= "can't access a needed shared library";
    MESG	e_libbad	= "accessing a corrupted shared library";
    MESG	e_libscn	= ".lib section in a.out corrupted";
    MESG	e_libmax	= "attempting to link in too many libs";
#endif	/* ELIBACC */
#ifdef EMULTIHOP
    MESG   e_multihop	= "multihop attempted";
#endif /* EMULTIHOP */
#ifdef ENAMETOOLONG
    MESG   e_longname	= "name too long";
#endif /* ENAMETOOLONG */
#ifdef ENOLINK
    MESG	e_link		= "remote link inactive";
#endif /* ENOLINK */
MESG	e_access	= "permission denied";
MESG	e_direct	= "bad directory";
MESG	e_notdir	= "not a directory";
MESG	e_file		= "bad file unit number";
MESG	e_trap		= "bad trap";
MESG	e_readonly	= "is read only";
MESG	e_ident		= "is not an identifier";
#ifdef  KSH_88D
MESG    e_aliname       = "invalid alias name";
#endif /* KSH_88D */
MESG	e_testop	= "unknown test operator";
MESG	e_alias		= " alias not found";
MESG	e_function	= "unknown function";
MESG	e_format	= "bad format";
MESG	e_on		= "on";
MESG	e_off		= "off";
MESG	is_reserved	= " is a keyword";
MESG	is_builtin	= " is a shell builtin";
MESG	is_alias	= " is an alias for ";
MESG	is_function	= " is a function";
MESG	is_xalias	= " is an exported alias for ";
MESG	is_talias	= " is a tracked alias for ";
MESG	is_xfunction	= " is an exported function";
MESG	is_ufunction	= " is an undefined function";
MESG	is_		= " is ";
MESG	e_fnhdr		= "\n{\n"; /*NOTX*/
MESG	e_runvi		= "fc -e \"${VISUAL:-${EDITOR:-vi}}\" "; /*NOTX*/
#ifdef JOBS
#   ifdef SIGTSTP
	MESG	e_newtty	= "Switching to new tty driver...";
	MESG	e_oldtty	= "Reverting to old tty driver...";
	MESG	e_no_start	= "Cannot start job control";
	MESG	e_no_jctl	= "No job control";
	MESG	e_terminate	= "You have stopped jobs";
#   endif /*SIGTSTP */
   MESG	e_Done		= " Done";
   MESG	e_nlspace	= "\n      "; /*NOTX*/
   MESG	e_Running	= " Running";
   MESG	e_ambiguous	= "Ambiguous";
   MESG	e_runningj	= "You have running jobs";
   MESG	e_no_job	= "no such job";
   MESG	e_no_proc	= "no such process";
   MESG	e_killcolon	= "kill: ";
   MESG	e_jobusage	= "Arguments must be %job or process ids";
   MESG	e_kill		= "kill";
#endif /* JOBS */
MESG	e_coredump	= "(coredump)";
#ifdef DEVFD
    MESG	e_devfd		= "/dev/fd/"; /*NOTX*/
#endif /* DEVFD */
#ifdef VPIX
    MESG	e_vpix		= "/vpix"; /*NOTX*/
    MESG	e_vpixdir	= "/usr/bin"; /*NOTX*/
#endif /* VPIX */

/* string constants */
#ifdef  KSH_88D
MESG    test_unops      = "LSVOGCaohrwxdcbfugkpsnzt"; /*NOTX*/
#else
MESG    test_unops      = "LSVOGaohrwxdcbfugkpsnzt"; /*NOTX*/
#endif /* KSH_88D */
MESG	e_heading	= "Current option settings";
MESG	e_nullstr	= ""; /*NOTX*/
#ifdef KJI
		  	/* includes double-wide space */
MESG	e_sptbnl	= {0x20, 0x09, 0x0a, 0x81, 0x40, 0x00}; /*NOTX*/
#else
MESG	e_sptbnl	= " \t\n"; /*NOTX*/
#endif

MESG	e_defpath	= "/bin:/usr/bin:"; /*NOTX*/

MESG	e_defedit	= "/bin/ed"; /*NOTX*/
MESG	e_colon		= ": "; /*NOTX*/
MESG	e_minus		= "-"; /*NOTX*/
MESG	e_endoffile	= "end of file";
MESG	e_unexpected 	= " unexpected";
MESG	e_unmatched 	= " unmatched";
#ifdef  KSH_88D
MESG    e_unknown       = "<command unknown>";
#else
MESG    e_unknown       = "<job name unknown>";
#endif /* KSH_88D */
MESG	e_atline	= " at line ";
MESG	e_devnull	= "/dev/null"; /*NOTX*/
MESG	e_traceprompt	= "+ "; /*NOTX*/

MESG	e_supprompt	= "# "; /*NOTX*/
MESG	e_stdprompt	= "$ "; /*NOTX*/
MESG	e_profile	= "${HOME}/.profile"; /*NOTX*/
MESG	e_sysprofile	= "/etc/profile"; /*NOTX*/
MESG	e_suidprofile	= "/etc/suid_profile"; /*NOTX*/
#ifdef  KSH_88D
MESG    e_crondir       = "/usr/spool/cron/atjobs"; /*NOTX*/
#endif /* KSH_88D */

#ifndef INT16
   MESG	e_prohibited	= "login setuid/setgid shells prohibited";
#endif /* INT16 */
#ifdef SUID_EXEC
   MESG	e_suidexec	= "/etc/suid_exec"; /*NOTX*/
#endif /* SUID_EXEC */
MESG	e_devfdNN	= "/dev/fd/+([0-9])"; /*NOTX*/
MESG	hist_fname	= "/.sh_history"; /*NOTX*/
MESG	e_unlimited	= "unlimited";
MESG	e_echobin	= "/bin/echo"; /*NOTX*/
MESG	e_echoflag	= "-R"; /*NOTX*/
MESG	e_test		= "test"; /*NOTX*/
MESG	e_let		= "let"; /*NOTX*/
MESG	e_dot		= "."; /*NOTX*/
MESG	e_bltfn		= "function "; /*NOTX*/
MESG	e_intbase	= "base"; /*NOTX*/
MESG	e_envmarker	= "A__z"; /*NOTX*/
#ifdef FLOAT
    MESG	e_precision	= "precision"; /*NOTX*/
#endif /* FLOAT */
#ifdef PDUBIN
        MESG	e_setpwd	= "PWD=`/usr/pdu/bin/pwd 2>/dev/null`"; /*NOTX*/
#else
        MESG	e_setpwd	= "PWD=`/bin/pwd 2>/dev/null`"; /*NOTX*/
#endif /* PDUBIN */
MESG	e_real		= "\nreal";
MESG	e_user		= "user";
MESG	e_sys		= "sys";



#ifdef apollo
#   undef NULL
#   define NULL ""
#   define e_nullstr	""
#endif	/* apollo */

/* built in names */
const struct name_value node_names[] =
{
	"PATH",		NULL,	0,
	"PS1",		NULL,	0,
	"PS2",		"> ",	N_FREE,
#ifdef apollo
	"IFS",		" \t\n",	N_FREE,
#else
	"IFS",		e_sptbnl,	N_FREE,
#endif	/* apollo */
	"PWD",		NULL,	0,
	"HOME",		NULL,	0,
	"MAIL",		NULL,	0,
	"REPLY",	NULL,	0,
	"SHELL",	"/bin/sh",	N_FREE,
	"EDITOR",	NULL,	0,
#ifdef apollo
	"MAILCHECK",	NULL,	N_FREE|N_INTGER,
	"RANDOM",	NULL,	N_FREE|N_INTGER,
#else
	"MAILCHECK",	(char*)(&sh_mailchk),	N_FREE|N_INTGER,
	"RANDOM",	(char*)(&sh_randnum),	N_FREE|N_INTGER|N_BLTNOD,
#endif	/* apollo */
	"ENV",		NULL,	0,
	"HISTFILE",	NULL,	0,
	"HISTSIZE",	NULL,	0,
	"FCEDIT",	"/bin/ed",	N_FREE,
	"CDPATH",	NULL,	0,
	"MAILPATH",	NULL,	0,
	"PS3",		"#? ",	N_FREE,
	"OLDPWD",	NULL,	0,
	"VISUAL",	NULL,	0,
	"COLUMNS",	NULL,	0,
	"LINES",	NULL,	0,
#ifdef  KSH_88D
        "PPID",         (char*)(&sh.ppid),      N_FREE|N_INTGER,
#else
        "PPID",         (char*)(&sh.ppid),      N_FREE|N_INTGER|N_RDONLY,
#endif /* KSH_88D */
	"_",		(char*)(&sh.lastarg),	N_FREE|N_INDIRECT|N_EXPORT,
	"TMOUT",	(char*)(&sh_timeout),	N_INTGER,
	"SECONDS",	(char*)(&sh_seconds),	N_FREE|N_INTGER|N_BLTNOD,
	"ERRNO",	(char*)(&errno),	N_FREE|N_INTGER,
	"LINENO",	(char*)(&line_numbers),	N_FREE|N_INTGER|N_BLTNOD,
	"OPTIND",	(char*)(&opt_index),	N_FREE|N_INTGER,
	"OPTARG",	(char*)(&opt_arg),	N_FREE|N_INDIRECT,
	"PS4",		NULL,	0,
	"FPATH",	NULL,	0,
#if	(defined(NLS) || defined(KJI))
	"LANG",		NULL,	0,
	"NLSPATH",	NULL,	0,
#endif	/* NLS || KJI */
#ifdef	_locale_
	"LC_CTYPE",	NULL,	0,
	"LC_COLLATE",	NULL,	0,
	"LC_MONETARY",	NULL,	0,
	"LC_NUMERIC",	NULL,	0,
	"LC_TIME",	NULL,	0,
	"LC_MESSAGES",	NULL,	0,
#endif	/* _locale_ */
#ifdef VPIX
	"DOSPATH",	NULL,	0,
	"VPIXDIR",	NULL,	0,
#endif	/* VPIX */
#ifdef ACCT
	"SHACCT",	NULL,	0,
#endif	/* ACCT */
#ifdef MULTIBYTE
	"CSWIDTH",	NULL,	0,
#endif /* MULTIBYTE */
	e_nullstr,	NULL,	0
};

#ifdef VPIX
   const char *suffix_list[] = { ".com", ".exe", ".bat", e_nullstr };
#endif	/* VPIX */

/* built in aliases - automatically exported */
const struct name_value alias_names[] =
{
	"autoload",	"typeset -fu",	N_FREE|N_EXPORT,
#ifdef  KSH_88D
#ifdef  POSIX
        "command",      "command ",     N_FREE|N_EXPORT,
#endif /* POSIX */
#endif /* KSH_88D */
	"false",	"let 0",	N_FREE|N_EXPORT,
	"functions",	"typeset -f",	N_FREE|N_EXPORT,
	"hash",		"alias -t -",	N_FREE|N_EXPORT,
	"history",	"fc -l",	N_FREE|N_EXPORT,
	"integer",	"typeset -i",	N_FREE|N_EXPORT,
#ifdef  KSH_88D
#ifdef  POSIX
        "local",        "local ",       N_FREE|N_EXPORT,
#endif /* POSIX */
#endif /* KSH_88D */
	"nohup",	"nohup ",	N_FREE|N_EXPORT,
	"r",		"fc -e -",	N_FREE|N_EXPORT,
	"true",		":",		N_FREE|N_EXPORT,
	"type",		"whence -v",	N_FREE|N_EXPORT,
#  ifdef SIGTSTP
	"stop",		"kill -STOP",	N_FREE|N_EXPORT,
	"suspend",	"kill -STOP $$",	N_FREE|N_EXPORT,
#  endif /*SIGTSTP */
	e_nullstr,	NULL,	0
};

const struct name_value tracked_names[] =
{
	"cat",		"/bin/cat",	N_FREE|N_EXPORT|T_FLAG,
	"chmod",	"/bin/chmod",	N_FREE|N_EXPORT|T_FLAG,
	"cc",		"/bin/cc",	N_FREE|N_EXPORT|T_FLAG,
	"cp",		"/bin/cp",	N_FREE|N_EXPORT|T_FLAG,
	"date",		"/bin/date",	N_FREE|N_EXPORT|T_FLAG,
	"ed",		"/bin/ed",	N_FREE|N_EXPORT|T_FLAG,
# ifdef _bin_grep_
	"grep",		"/bin/grep",	N_FREE|N_EXPORT|T_FLAG,
# else
#  ifdef _usr_ucb_
	"grep",		"/usr/ucb/grep",N_FREE|N_EXPORT|T_FLAG,
#  endif /* _usr_ucb_ */
# endif	/* _bin_grep */
# ifdef _usr_bin_lp
	"lp",		"/usr/bin/lp",	N_FREE|N_EXPORT|T_FLAG,
# endif /* _usr_bin_lpr */
# ifdef _usr_bin_lpr
	"lpr",		"/usr/bin/lpr",	N_FREE|N_EXPORT|T_FLAG,
# endif /* _usr_bin_lpr */
	"ls",		"/bin/ls",	N_FREE|N_EXPORT|T_FLAG,
	"make",		"/bin/make",	N_FREE|N_EXPORT|T_FLAG,
	"mail",		"/bin/mail",	N_FREE|N_EXPORT|T_FLAG,
	"mv",		"/bin/mv",	N_FREE|N_EXPORT|T_FLAG,
	"pr",		"/bin/pr",	N_FREE|N_EXPORT|T_FLAG,
	"rm",		"/bin/rm",	N_FREE|N_EXPORT|T_FLAG,
	"sed",		"/bin/sed",	N_FREE|N_EXPORT|T_FLAG,
	"sh",		"/bin/sh",	N_FREE|N_EXPORT|T_FLAG,
# ifdef _usr_bin_vi_
	"vi",		"/usr/bin/vi",	N_FREE|N_EXPORT|T_FLAG,
# else
#  ifdef _usr_ucb_
	"vi",		"/usr/ucb/vi",	N_FREE|N_EXPORT|T_FLAG,
#  endif /* _usr_ucb_ */
# endif	/* _usr_bin_vi_ */
	"who",		"/bin/who",	N_FREE|N_EXPORT|T_FLAG,

	e_nullstr,	NULL,	0
};

/* tables */
SYSTAB tab_reserved =
{
#ifdef  KSH_88D
#ifdef  POSIX
                {"!",           NOTSYM},
#endif /* POSIX */
#endif /* KSH_88D */
#ifdef NEWTEST
		{"[[",		BTSTSYM},
#endif /* NEWTEST */
		{"case",	CASYM},
		{"do",		DOSYM},
		{"done",	ODSYM},
		{"elif",	EFSYM},
		{"else",	ELSYM},
		{"esac",	ESSYM},
		{"fi",		FISYM},
		{"for",		FORSYM},
		{"function",	PROCSYM},
		{"if",		IFSYM},
		{"in",		INSYM},
		{"select",	SELSYM},
		{"then",	THSYM},
		{"time",	TIMSYM},
		{"until",	UNSYM},
		{"while",	WHSYM},
		{"{",		BRSYM},
		{"}",		KTSYM},
		{e_nullstr,	0},
};

/*
 * The signal numbers go in the low bits and the attributes go in the high bits
 */

SYSTAB	sig_names =
{
#ifdef SIGABRT
		{"ABRT",	(SIGABRT+1)|(SIGDONE<<SIGBITS)},
#endif /*SIGABRT */
		{"ALRM",	(SIGALRM+1)|((SIGCAUGHT|SIGFAULT)<<SIGBITS)},
		{"BUS",		(SIGBUS+1)|(SIGDONE<<SIGBITS)},
#ifdef SIGCHLD
		{"CHLD",	(SIGCHLD+1)|((SIGCAUGHT|SIGFAULT)<<SIGBITS)},
#   ifdef SIGCLD
#	if SIGCLD!=SIGCHLD
		{"CLD",		(SIGCLD+1)|((SIGCAUGHT|SIGFAULT)<<SIGBITS)},
#	endif
#   endif	/* SIGCLD */
#else
#   ifdef SIGCLD
		{"CLD",		(SIGCLD+1)|((SIGCAUGHT|SIGFAULT)<<SIGBITS)},
#   endif	/* SIGCLD */
#endif	/* SIGCHLD */
#ifdef SIGCONT
		{"CONT",	(SIGCONT+1)},
#endif	/* SIGCONT */
		{"DEBUG",	(DEBUGTRAP+1)},
#ifdef SIGEMT
		{"EMT",		(SIGEMT+1)|(SIGDONE<<SIGBITS)},
#endif	/* SIGEMT */
		{"ERR",		(ERRTRAP+1)},
		{"EXIT",	1},
		{"FPE",		(SIGFPE+1)|(SIGDONE<<SIGBITS)},
		{"HUP",		(SIGHUP+1)|(SIGDONE<<SIGBITS)},
		{"ILL",		(SIGILL+1)|(SIGDONE<<SIGBITS)},
		{"INT",		(SIGINT+1)|(SIGCAUGHT<<SIGBITS)},
#ifdef SIGIO
		{"IO",		(SIGIO+1)},
#endif	/* SIGIO */
		{"IOT",		(SIGIOT+1)|(SIGDONE<<SIGBITS)},
		{"KILL",	(SIGKILL+1)},
#ifdef SIGLAB
		{"LAB",		(SIGLAB+1)},
#endif	/* SIGLAB */
#ifdef SIGLOST
		{"LOST",	(SIGLOST+1)},
#endif	/* SIGLOST */
#ifdef SIGPHONE
		{"PHONE",	(SIGPHONE+1)},
#endif	/* SIGPHONE */
		{"PIPE",	(SIGPIPE+1)|(SIGDONE<<SIGBITS)},
#ifdef SIGPOLL
		{"POLL",	(SIGPOLL+1)},
#endif	/* SIGPOLL */
#ifdef SIGPROF
		{"PROF",	(SIGPROF+1)},
#endif	/* SIGPROF */
#ifdef SIGPWR
#   if SIGPWR>0
		{"PWR",		(SIGPWR+1)},
#   endif
#endif	/* SIGPWR */
		{"QUIT",	(SIGQUIT+1)|((SIGCAUGHT|SIGIGNORE)<<SIGBITS)},
		{"SEGV",	(SIGSEGV+1)},
#ifdef SIGSTOP
		{"STOP",	(SIGSTOP+1)},
#endif	/* SIGSTOP */
		{"SYS",		(SIGSYS+1)|(SIGDONE<<SIGBITS)},
		{"TERM",	(SIGTERM+1)|(SIGDONE<<SIGBITS)},
#ifdef SIGTINT
		{"TINT",	(SIGTINT+1)},
#endif	/* SIGTINT */
		{"TRAP",	(SIGTRAP+1)|(SIGDONE<<SIGBITS)},
#ifdef SIGTSTP
		{"TSTP",	(SIGTSTP+1)},
#endif	/* SIGTSTP */
#ifdef SIGTTIN
		{"TTIN",	(SIGTTIN+1)},
#endif	/* SIGTTIN */
#ifdef SIGTTOU
		{"TTOU",	(SIGTTOU+1)},
#endif	/* SIGTTOU */
#ifdef SIGURG
		{"URG",		(SIGURG+1)},
#endif	/* SIGURG */
#ifdef SIGUSR1
		{"USR1",	(SIGUSR1+1)|(SIGDONE<<SIGBITS)},
#endif	/* SIGUSR1 */
#ifdef SIGUSR2
		{"USR2",	(SIGUSR2+1)|(SIGDONE<<SIGBITS)},
#endif	/* SIGUSR2 */
#ifdef SIGVTALRM
		{"VTALRM",	(SIGVTALRM+1)},
#endif	/* SIGVTALRM */
#ifdef SIGWINCH
		{"WINCH",	(SIGWINCH+1)},
#endif	/* SIGWINCH */
#ifdef SIGWINDOW
		{"WINDOW",	(SIGWINDOW+1)},
#endif	/* SIGWINDOW */
#ifdef SIGWIND
		{"WIND",	(SIGWIND+1)},
#endif	/* SIGWIND */
#ifdef SIGXCPU
		{"XCPU",	(SIGXCPU+1)},
#endif	/* SIGXCPU */
#ifdef SIGXFSZ
		{"XFSZ",	(SIGXFSZ+1)|((SIGCAUGHT|SIGIGNORE)<<SIGBITS)},
#endif	/* SIGXFSZ */
		{e_nullstr,	0}
};

/*
 *   Korn uses an #ifdef scheme to build a machine-independent list of 
 *   signals and associated messages. 
 *
 *   The message strings defined here are used as the default.
 *
 *   Identical messages must be the first entries in the ksh.msg file, 
 *   ordered using hardcoded 1-based small integers which are equal to 
 *   the actual signal values.
 *
 *   Unfortunately, signal values are not equivalent across AIX v2.x and
 *   AIX v3.x. Not only are new signals present in v3, but signals with
 *   the same name do not have the same value (eg. SIGUSR1 = 16 in the 
 *   v2 signal.h; SIGUSR1 = 30 in v3.
 *
 *   Since unique keys are required for the message facility, two message
 *   files are required: one for v2, and one for v3. The ksh.msg file MUST 
 *   reflect the actual ordering in signal.h. Each signal must have a message 
 *   associated. If signal.h changes in the future, ksh.msg must also change.
 */

#ifndef	_sys_siglist_
/*
 *	AIX 3.1 will use signal messages from libc. The file in
 *	lib/c/gen/siglist.c contains the signal messages in the variable
 *	sys_siglist. A define has been added to include/sh_config.h for this.
 */

SYSTAB	sig_messages =
{
#ifdef SIGABRT
		{"Abort",			(SIGABRT+1)},
#endif /*SIGABRT */
		{"Alarm call",			(SIGALRM+1)},
		{"Bus error",			(SIGBUS+1)},
#ifdef SIGCHLD
		{"Child stopped or terminated",	(SIGCHLD+1)},
#   ifdef SIGCLD
#	if SIGCLD!=SIGCHLD
		{"Death of Child", 		(SIGCLD+1)},
#	endif
#   endif	/* SIGCLD */
#else
#   ifdef SIGCLD
		{"Death of Child", 		(SIGCLD+1)},
#   endif	/* SIGCLD */
#endif	/* SIGCHLD */
#ifdef SIGCONT
		{"Stopped process continued",	(SIGCONT+1)},
#endif	/* SIGCONT */
#ifdef SIGEMT
		{"EMT trap",			(SIGEMT+1)},
#endif	/* SIGEMT */
		{"Floating exception",		(SIGFPE+1)},
		{"Hangup",			(SIGHUP+1)},
		{"Illegal instruction",		(SIGILL+1)},
#ifdef JOBS
		{"Interrupt",			(SIGINT+1)},
#else
		{e_nullstr,			(SIGINT+1)},
#endif	/* JOBS */
#ifdef SIGIO
		{"IO signal",			(SIGIO+1)},
#endif	/* SIGIO */
		{"Abort",			(SIGIOT+1)},
		{"Killed",			(SIGKILL+1)},
		{"Quit",			(SIGQUIT+1)},
#ifdef JOBS
		{"Broken Pipe",			(SIGPIPE+1)},
#else
		{e_nullstr,			(SIGPIPE+1)},
#endif	/* JOBS */
#ifdef SIGPROF
		{"Profiling time alarm",	(SIGPROF+1)},
#endif	/* SIGPROF */
#ifdef SIGPWR
#   if SIGPWR>0
		{"Power fail",			(SIGPWR+1)},
#   endif
#endif	/* SIGPWR */
		{"Memory fault",		(SIGSEGV+1)},
#ifdef SIGSTOP
		{"Stopped (signal)",		(SIGSTOP+1)},
#endif	/* SIGSTOP */
		{"Bad system call", 		(SIGSYS+1)},
		{"Terminated",			(SIGTERM+1)},
#ifdef SIGTINT
#   ifdef JOBS
		{"Interrupt",			(SIGTINT+1)},
#   else
		{e_nullstr,			(SIGTINT+1)},
#   endif /* JOBS */
#endif	/* SIGTINT */
		{"Trace/BPT trap",		(SIGTRAP+1)},
#ifdef SIGTSTP
		{"Stopped",			(SIGTSTP+1)},
#endif	/* SIGTSTP */
#ifdef SIGTTIN
		{"Stopped (tty input)",		(SIGTTIN+1)},
#endif	/* SIGTTIN */
#ifdef SIGTTOU
		{"Stopped(tty output)",		(SIGTTOU+1)},
#endif	/* SIGTTOU */
#ifdef SIGURG
		{"Socket interrupt",		(SIGURG+1)},
#endif	/* SIGURG */
#ifdef SIGUSR1
		{"User signal 1",		(SIGUSR1+1)},
#endif	/* SIGUSR1 */
#ifdef SIGUSR2
		{"User signal 2",		(SIGUSR2+1)},
#endif	/* SIGUSR2 */
#ifdef SIGVTALRM
		{"Virtual time alarm",		(SIGVTALRM+1)},
#endif	/* SIGVTALRM */
#ifdef SIGWINCH
		{"Window size change", 		(SIGWINCH+1)},
#endif	/* SIGWINCH */
#ifdef SIGXCPU
		{"Exceeded CPU time limit",	(SIGXCPU+1)},
#endif	/* SIGXCPU */
#ifdef SIGXFSZ
		{"Exceeded file size limit",	(SIGXFSZ+1)},
#endif	/* SIGXFSZ */
#ifdef SIGLOST
		{"Resources lost", 		(SIGLOST+1)},
#endif	/* SIGLOST */
#ifdef SIGLAB
		{"Security label changed",	(SIGLAB+1)},
#endif	/* SIGLAB */
		{e_nullstr,	0}
};
#endif

SYSTAB tab_options=
{
	{"allexport",		Allexp},
	{"bgnice",		Bgnice},
	{"emacs",		Emacs},
	{"errexit",		Errflg},
	{"gmacs",		Gmacs},
	{"ignoreeof",		Noeof},
	{"interactive",		Intflg},
	{"keyword",		Keyflg},
	{"markdirs",		Markdir},
	{"monitor",		Monitor},
	{"noexec",		Noexec},
	{"noclobber",		Noclob},
	{"noglob",		Noglob},
	{"nolog",		Nolog},
	{"nounset",		Noset},
	{"privileged",		Privmod},
	{"restricted",		Rshflg},
	{"trackall",		Hashall},
	{"verbose",		Readpr},
	{"vi",			Editvi},
	{"viraw",		Viraw},
	{"xtrace",		Execpr},
	{e_nullstr,		0}
};

#ifdef _sys_resource_
#   ifndef included_sys_time_
#	include <sys/time.h>
#   endif
#   include	<sys/resource.h>/* needed for ulimit */
#   define	LIM_FSIZE	RLIMIT_FSIZE
#   define	LIM_DATA	RLIMIT_DATA
#   define	LIM_STACK	RLIMIT_STACK
#   define	LIM_CORE	RLIMIT_CORE
#   define	LIM_CPU		RLIMIT_CPU
#   ifdef RLIMIT_RSS
#	define	LIM_MAXRSS	RLIMIT_RSS
#   endif /* RLIMIT_RSS */
#else
#   ifdef VLIMIT
#	include	<sys/vlimit.h>
#   endif /* VLIMIT */
#endif	/* _sys_resource_ */

#ifdef LIM_CPU
#   define size_resource(a,b) ((a)|((b)<<11))	
SYSTAB limit_names =
{
	{"time(seconds)   ",	size_resource(1,LIM_CPU)},
#ifdef  KSH_88D
        {"file(blocks)    ",    size_resource(512,LIM_FSIZE)},
        {"data(kbytes)    ",    size_resource(1024,LIM_DATA)},
        {"stack(kbytes)   ",    size_resource(1024,LIM_STACK)},
#endif /* KSH_88D */
#   ifdef LIM_MAXRSS
	{"memory(kbytes)  ",	size_resource(1024,LIM_MAXRSS)},
#   else
	{"memory(kbytes)  ",	size_resource(1024,0)},
#   endif /* LIM_MAXRSS */
#ifndef KSH_88D
	{"data(kbytes)    ",	size_resource(1024,LIM_DATA)},
	{"stack(kbytes)   ",	size_resource(1024,LIM_STACK)},
	{"file(blocks)    ",	size_resource(512,LIM_FSIZE)},
#endif /* !KSH_88D */
	{"coredump(blocks)",	size_resource(512,LIM_CORE)},
#ifdef  KSH_88D
#   ifdef RLIMIT_NOFILE
        {"nofiles(descriptors)",        size_resource(1,RLIMIT_NOFILE)},
#   else
        {"nofiles(descriptors)",        size_resource(1,0)},
#   endif /* RLIMIT_NOFILE */
#else
#   ifdef RLIMIT_NOFILE
	{"file descriptors",	size_resource(1,RLIMIT_NOFILE)},
#   else
	{"file descriptors",	size_resource(1,0)},
#   endif /* RLIMIT_NOFILE */
#   ifdef RLIMIT_SWAP
	{"swap(kbytes)    ",	size_resource(1024,RLIMIT_SWAP)},
#   else
	{"swap(kbytes)    ",	size_resource(1024,0)},
#   endif /* RLIMIT_SWAP */
#endif /* KSH_88D */
#   ifdef RLIMIT_VMEM
	{"vmemory(kbytes) ",	size_resource(1024,RLIMIT_VMEM)}
#   else
	{"vmemory(kbytes) ",	size_resource(1024,0)}
#   endif /* RLIMIT_VMEM */
};
#endif	/* LIM_CPU */

SYSTAB	tab_builtins=
{
		{".",		SYSDOT},
		{":",		SYSNULL},
		{"[",		SYSTEST},
		{ "alias",	SYSALIAS|SYSDECLARE},
#ifdef SIGTSTP
		{ "bg",		SYSBG},
#endif /* SIGTSTP */
		{"break",	SYSBREAK},
		{"cd",		SYSCD},
#ifdef  KSH_88D
#ifdef  POSIX
                {"command",     SYSCOMMAND},
#endif /* POSIX */
#endif /* KSH_88D */
		{"continue",	SYSCONT},
		{"echo",	SYSECHO},
		{"exec",	SYSEXEC},
		{"exit",	SYSEXIT},
		{"export",	SYSXPORT|SYSDECLARE},
		{"eval",	SYSEVAL},
		{"fc",		SYSFC},
#ifdef SIGTSTP
		{"fg",		SYSFG},
#endif /* SIGTSTP */
		{"getopts",	SYSGETOPTS},
		{"inlib",	SYSINLIB},
#ifdef JOBS
		{"jobs",	SYSJOBS},
		{"kill",	SYSKILL},
#endif	/* JOBS */
		{"let",		SYSLET},
		{"login",	SYSLOGIN},
		{"logout",	SYSLOGOUT},
#ifdef  KSH_88D
#ifdef  _bin_newgrp_
                {"newgrp",      SYSLOGIN},
#endif /* _bin_newgrp_ */
#else
                {"newgrp",      SYSNEWGRP},
#endif /* KSH_88D */
		{"print",	SYSPRINT},
		{"pwd",		SYSPWD},
		{"read",	SYSREAD},
		{"readonly",	SYSRDONLY|SYSDECLARE},
		{"rmlib",	SYSRMLIB},
		{"return",	SYSRETURN},
		{"set",		SYSSET},
		{"setgroups",	SYSSETGROUPS},
		{"setsenv",	SYSSETSENV},
#ifdef  KSH_88D
#ifdef SYSCOMPILE
                {"shcomp",      SYSCOMPILE},
#endif /* SYSCOMPILE */
#endif /* KSH_88D */
		{"shift",	SYSSHFT},
#ifdef  KSH_88D
#ifdef SYSSLEEP
                {"sleep",       SYSSLEEP},
#endif /* SYSSLEEP */
#endif /* KSH_88D */
#ifdef OLDTEST
		{"test",	SYSTEST},
#endif /* OLDTEST */
		{"times",	SYSTIMES},
		{"trap",	SYSTRAP},
		{"typeset",	SYSTYPESET|SYSDECLARE},
		{"ulimit",	SYSULIMIT},
		{"umask",	SYSUMASK},
		{"unalias",	SYSUNALIAS},
#ifdef  KSH_88D
#ifdef UNIVERSE
                {"universe",    SYSUNIVERSE},
#endif /* UNIVERSE */
#endif /* KSH_88D */
		{"unset",	SYSUNSET},
#ifdef FS_3D
		{"vmap",	SYSVMAP},
		{"vpath",	SYSVPATH},
#endif /* FS_3D */
		{"wait",	SYSWAIT},
		{"whence",	SYSWHENCE},
		{e_nullstr,		0}
};

SYSTAB	test_optable =
{
		{"!=",		TEST_SNE},
		{"-a",		TEST_AND},
		{"-ef",		TEST_EF},
		{"-eq",		TEST_EQ},
		{"-ge",		TEST_GE},
		{"-gt",		TEST_GT},
		{"-le",		TEST_LE},
		{"-lt",		TEST_LT},
		{"-ne",		TEST_NE},
		{"-nt",		TEST_NT},
		{"-o",		TEST_OR},
		{"-ot",		TEST_OT},
		{"=",		TEST_SEQ},
#ifdef NEWTEST
#ifdef  KSH_88D
                {"<",           TEST_SLT},
                {">",           TEST_SGT},
#endif /* KSH_88D */
		{"]]",		TEST_END},
#endif /* NEWTEST */
		{e_nullstr,	0}
};

SYSTAB	tab_attributes =
{
		{"export",	N_EXPORT},
		{"readonly",	N_RDONLY},
		{"tagged",	T_FLAG},
#ifdef FLOAT
		{"exponential",	(N_DOUBLE|N_INTGER|N_EXPNOTE)},
		{"float",	(N_DOUBLE|N_INTGER)},
#endif /* FLOAT */
		{"long",	(L_FLAG|N_INTGER)},
		{"unsigned",	(N_UNSIGN|N_INTGER)},
		{"function",	(N_BLTNOD|N_INTGER)},
		{"integer",	N_INTGER},
		{"filename",	N_HOST},
		{"lowercase",	N_UTOL},
		{"zerofill",	N_ZFILL},
		{"leftjust",	N_LJUST},
		{"rightjust",	N_RJUST},
		{"uppercase",	N_LTOU},
		{e_nullstr,	0}
};


#ifdef  KSH_88D
#ifndef IODELAY
#       undef   _SELECT5_
#endif /* IODELAY */
#ifdef _sgtty_
#   ifdef _SELECT5_
        const int tty_speeds[] = {0, 50, 75, 110, 134, 150, 200, 300,
                        600,1200,1800,2400,9600,19200,0};
#   endif /* _SELECT5_ */
#endif /* _sgtty_ */
#else
#ifdef _sgtty_
#   ifdef _SELECT_
        const int tty_speeds[] = {0, 50, 75, 110, 134, 150, 200, 300,
                        600,1200,1800,2400,9600,19200,0};
#   endif /* _SELECT_ */
#endif /* _sgtty_ */
#endif /* KSH_88D */
