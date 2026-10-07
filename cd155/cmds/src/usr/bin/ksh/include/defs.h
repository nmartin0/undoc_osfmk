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
/* static char rcsid[] = "@(#)$RCSfile: defs.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:27:52 $"; */
/*
 * COMPONENT_NAME: (CMDKSH) Korn shell
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 27
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
 */

#include	"sh_config.h"
#include	<setjmp.h>
#include	<signal.h>
#ifdef  KSH_88D
#ifndef NSIG
#       define  NSIG    32
#endif /* NSIG */
#endif /* KSH_88D */
#ifdef _unistd_h
#   include	<unistd.h>
#endif /* _unistd_h */
#ifdef _sys_times_
#   include	<sys/times.h>
#else
  	struct tms
	{
		time_t	tms_utime;
		time_t	tms_stime;
		time_t	tms_cutime;
		time_t	tms_cstime;
	};
#endif /* _sys_times */

struct sysnod		/* readonly tables */
{
#ifdef apollo
	/* pointers can not be in readonly sections */
	const char   sysnam[28];
#else
	const char	*sysnam;
#endif	/* apollo */
	unsigned sysval;
};

struct	blk		/* heap storage */
{
	struct blk	*word;
	/* data */
};

/* typedefs used in the shell */
typedef const char		MESG[];
typedef const struct sysnod	SYSTAB[];

#include	"name.h"
#include	"shnodes.h"
#include	"stak.h"
#include	"shtype.h"


/* error exits from various parts of shell */
#define ERROR	1
#define SYNBAD	2

#define BYTESPERWORD	((unsigned)sizeof(char *))
#define	NIL	((char*)0)
#define ENDARGS	NIL	/* arg list terminator */
#ifndef NULL
#   define NULL (void *)0
#endif
#define NULLSTR	((char*)e_nullstr)


#define round(a,b)	((sizeof(char*)==sizeof(int))?\
				(((int)(((a)+b)-1))&~((b)-1)):\
				(((long)(((a)+b)-1))&~((b)-1)))
#define eq(a,b)		(strcmp(a,b)==0)
#define max(a,b)	((a)>(b)?(a):(b))
#define assert(x)	;
#define exitset()	(sh.savexit=sh.exitval)
#define	ADR(x)		((char*)(x))

/* flags */

typedef long optflag;
#ifdef INT16
#   ifndef pdp11
#   define _OPTIM_	1
#   endif /* pdp11 */
#endif /* INT16 */

#ifdef _OPTIM_
#   define _HIGH_	1
#   define _LOW_	1-_HIGH_
#   define is_option(x)	((x)&0xffffL?\
			st.flags.i[_LOW_]&(unsigned int)(x):\
			st.flags.i[_HIGH_]&(unsigned int)((x)>>16))
#   define on_option(x)	((x)&0xffffL?\
				(st.flags.i[_LOW_] |= (unsigned int)(x)):\
				(st.flags.i[_HIGH_] |= (unsigned int)((x)>>16)))
#   define off_option(x)	((x)&0xffffL?\
				(st.flags.i[_LOW_] &= ~(unsigned int)(x)):\
				(st.flags.i[_HIGH_] &= ~(unsigned int)((x)>>16)))
#else
#   define is_option(x)	(st.flags.l & (x))
#   define on_option(x)	(st.flags.l |= (x))
#   define off_option(x)	(st.flags.l &= ~(x))
#endif /* _OPTIM_ */

#define Fixflg	1
#define Errflg	2
#define Readpr	3
#define Monitor	4
#define	Intflg	5
#define Rshflg	6
#define Execpr	7
#define Keyflg	8
#define Noset	9
#define Noglob	10
#define Allexp	11
#define Noeof	13
#define Noclob	14
#define Markdir	15
#define Bgnice	16
#define Editvi	17
#define Viraw	18
#define Oneflg	19
#define Hashall	20
#define Stdflg	21
#define Noexec	22
#define Notify	23
#define Gmacs	24
#define Emacs	25
#define	Privmod 26
#define	Inproc	27
#define Nolog	28

#define FIXFLG	(1<<Fixflg) /* used also as a state */
#define	ERRFLG	(1<<Errflg) /* used also as a state */
#define	READPR	(1<<Readpr) /* used also as a state */
#define MONITOR	(1<<Monitor)/* used also as a state */
#define	INTFLG	(1<<Intflg) /* used also as a state */
#define	RSHFLG	(1L<<Rshflg)
#define	EXECPR	(1L<<Execpr)
#define	KEYFLG	(1L<<Keyflg)
#define NOSET	(1L<<Noset)
#define NOGLOB	(1L<<Noglob)
#define ALLEXP	(1L<<Allexp)
#define NOEOF	(1L<<Noeof)
#define NOCLOB	(1L<<Noclob)
#define EMACS	(1L<<Emacs)
#define BGNICE	(1L<<Bgnice)
#define EDITVI	(1L<<Editvi)
#define VIRAW	(1L<<Viraw)
#define	ONEFLG	(1L<<Oneflg)
#define HASHALL	(1L<<Hashall)
#define	STDFLG	(1L<<Stdflg)
#define	NOEXEC	(1L<<Noexec)
#define	NOTIFY	(1L<<Notify)
#define GMACS	(1L<<Gmacs)
#define MARKDIR	(1L<<Markdir)
#define PRIVM	(1L<<Privmod)
#define INPROC	(1L<<Inproc)
#define NOLOG	(1L<<Nolog)
#define CFLAG	(1L<<30)


/* states */
/* low numbered states are same as flags */
#define GRACE		0x1
#define	PROMPT		INTFLG
#ifndef KSH_88D
#define WAITING         0x40    /* set when waiting for command input */
#endif /* KSH_88D */
#define	FORKED		0x80
#define	PROFILE		0x100	/* set when processing profiles */
#define IS_TMP		0x200	/* set when TMPFD is available */
#ifdef  KSH_88D
#define WAITING         0x400   /* set when waiting for command input */
#endif /* KSH_88D */
#define RM_TMP		0x800	/* temp files to remove on exit */
#define FUNCTION 	0x1000	/* set when entering a function */
#define RWAIT		0x2000	/* set when waiting for a read */
#define BUILTIN		0x4000	/* set when processing built-in command */
#define LASTPIPE	0x8000	/* set for last element of a pipeline */
#ifdef VFORK
#   define VFORKED	0x10000	/* only used with VFORK mode */
#else
#   define VFORKED	0	
#endif /* VFORK */



#define FORKLIM 32		/* fork constant */
#define MEMSIZE   32*sizeof(int)/* default associative memory size for shell.
					Must be a power of 2 */
#define NL	'\n'
#define SP	' '
#define HIGHBIT	0200
#define TO_PRINT 0100		/* bit to set for printing control char */
#define MINTRAP	0
#define MAXTRAP NSIG+3		/* maximum number of traps */

/* print interface routines */
#ifdef _NO_PROTO
    extern void p_flush();
    extern void p_list();
    extern void p_nchr();
    extern void p_char();
    extern void p_num();
    extern void p_prp();
    extern void p_setout();
    extern void p_str();
    extern void p_sub();
    extern void	p_time();
#else /* ~ _NO_PROTO */
    extern void p_flush(void);
    extern void p_list(int,const char*[]);
    extern void p_nchr(int,int);
    extern void p_char(int);
    extern void p_num(int,int);
    extern void p_prp(const char*);
    extern void p_setout(int);
    extern void p_str(const char*,int);
    extern void p_sub(int,int);
    extern void p_time(clock_t,int);
#endif /* _NO_PROTO */


/* argument processing routines */
#ifdef _NO_PROTO
    extern char 		**arg_build();
    extern void 		arg_clear();
    extern char 		*arg_dolminus();
    extern struct dolnod	*arg_free();
    extern struct dolnod	*arg_new();
    extern int			arg_opts();
    extern void 		arg_reset();
    extern void 		arg_set();
    extern struct dolnod	*arg_use();
#else /* ~ _NO_PROTO */
    extern char 		**arg_build(int*,struct comnod*);
    extern void 		arg_clear(void);
    extern char 		*arg_dolminus(void);
    extern struct dolnod	*arg_free(struct dolnod*,int);
    extern struct dolnod	*arg_new(char*[],struct dolnod**);
    extern int			arg_opts(int,char**);
    extern void 		arg_reset(struct dolnod*,struct dolnod*);
    extern void 		arg_set(char*[]);
    extern struct dolnod	*arg_use(void);
#endif /* _NO_PROTO */

extern char		*opt_arg;
extern int		opt_char;
extern int		opt_index;

/* routines for name/value pair environment */
#ifdef _NO_PROTO
    extern void 		env_arrayset();
    extern void 		env_check_nls_and_locale ();
    extern char 		**env_gen();
    extern int			env_init();
    extern struct namnod	*env_namset();
    extern void 		env_nolocal();
    extern void 		env_prattr();
    extern int			env_prnamval();
    extern void 		env_readline();
    extern void 		env_setlist();
    extern void 		env_scan();
#else /* ~ _NO_PROTO */
    extern void 		env_arrayset(struct namnod*,int,char*[]);
    extern void 		env_check_nls_and_locale (struct namnod*, int);
    extern char 		**env_gen(void);
    extern int			env_init(void);
    extern struct namnod	*env_namset(char*,struct Amemory*,int);
    extern void 		env_nolocal(struct namnod*);
    extern void 		env_prattr(struct namnod*);
    extern int			env_prnamval(struct namnod*,int);
    extern void 		env_readline(char**,int,int);
    extern void 		env_setlist(struct argnod*,int);
    extern void 		env_scan(int,int,struct Amemory*,int);
#endif /* _NO_PROTO */

/* pathname handling routines */
#ifdef _NO_PROTO
    extern void 	path_alias();
    extern char 	*path_absolute();
    extern char 	*path_basename();
    extern char		*pathcanon();
    extern void 	path_exec();
    extern int		path_open();
    extern char 	*path_get();
    extern char 	*path_join();
    extern char 	*path_pwd();
    extern int		path_search();
#   if defined(LSTAT)
	extern int	path_physical();
#   endif
#   ifndef INT16
	extern char	*path_relative();
#   endif /* INT16 */
#else /* ~ _NO_PROTO */
    extern void 	path_alias(struct namnod*,char*);
    extern char 	*path_absolute(const char*);
    extern char 	*path_basename(const char*);
    extern char		*pathcanon(char*);
    extern void 	path_exec(char*[],struct argnod*);
    extern int		path_open(const char*,char*);
    extern char 	*path_get(const char*);
    extern char 	*path_join(char*,const char*);
    extern char 	*path_pwd(int);
    extern int		path_search(const char*,int);
#   if defined(LSTAT)
	extern int	path_physical(char*);
#   endif
#   ifndef INT16
	extern char	*path_relative(const char*);
#   endif /* INT16 */
#endif /* _NO_PROTO */

/* error messages */
extern MESG	e_access;
extern MESG	e_alias;
extern MESG	e_argexp;
extern MESG	e_arglist;
extern MESG	e_atline;
extern MESG	e_bltfn;
extern MESG	e_colon;
#ifdef  KSH_88D
extern MESG     e_crondir;
#endif /* KSH_88D */
extern MESG	e_defpath;
extern MESG	e_dot;
extern MESG	e_echobin;
extern MESG	e_echoflag;
extern MESG	e_envmarker;
extern MESG	e_exec;
extern MESG	e_fnhdr;
extern MESG	e_fork;
extern MESG	e_found;
extern MESG	e_function;
extern MESG	e_heading;
extern MESG	e_inlib;
#ifdef ELIBACC
    extern MESG	e_libacc;
    extern MESG	e_libbad;
    extern MESG	e_libscn;
    extern MESG	e_libmax;
#endif	/* ELIBACC */
extern MESG	e_logout;
extern MESG	e_mailmsg;
extern MESG	e_minus;
extern MESG	e_nargs;
extern MESG	e_nullstr;
extern MESG	e_off;
extern MESG	e_on;
extern MESG	e_option;
extern MESG	e_pexists;
#ifdef FLOAT
   extern MESG	e_precision;
#endif /* FLOAT */
#ifdef SHELLMAGIC
   extern MESG	e_prohibited;
#endif /* SHELLMAGIC */
extern MESG	e_pwd;
extern MESG	e_query;
extern MESG	e_real;
extern MESG	e_rmlib;
extern MESG	e_resetld;
extern MESG	e_restricted;
extern MESG	e_setld;
extern MESG	e_setpwd;
extern MESG	e_sptbnl;
extern MESG	e_subst;
extern MESG	e_stdprompt;
extern MESG	e_supprompt;
extern MESG	e_tshstdprompt;
extern MESG	e_tshsupprompt;
extern MESG	e_swap;
extern MESG	e_sys;
extern MESG	e_trap;
extern MESG	e_tcsetpgrp;
extern MESG	e_toobig;
extern MESG	e_traceprompt;
extern MESG	e_txtbsy;
extern MESG	e_user;
extern MESG	e_version;
extern MESG	is_;
extern MESG	is_alias;
extern MESG	is_builtin;
extern MESG	is_function;
extern MESG	is_reserved;
extern MESG	is_talias;
extern MESG	is_xalias;
extern MESG	is_xfunction;
extern MESG	is_ufunction;
extern const char	e_recursive[];

/* tsh-specific messages */
extern MESG	e_nooption;
extern MESG	e_nousralias;
extern MESG	e_notrkalias;
extern MESG	e_nousrfunc;
extern MESG	e_nohistcap;

/* frequently referenced routines */
#ifdef _NO_PROTO
    extern unsigned	alarm();
    extern void 	free();
    extern void 	*malloc();
    extern char 	*mac_expand();
    extern char 	*mac_trim();
    extern int 		mac_here();
    extern char 	*mac_try();
#ifndef KSH_88D
    extern void         mac_check();
#endif /* KSH_88D */
    extern int		sh_access();
    extern char 	*sh_copy();
    extern void 	sh_fail();
    extern void 	sh_funct();
    extern char 	*sh_heap();
    extern char 	*sh_itos();
    extern int		sh_lookup();
    extern char 	*sh_substitute();
    extern void 	sh_trim();
    extern void 	sh_whence();
    extern char 	*strchr();
#ifndef KSH_88D
    extern int          strlen();
#endif /* KSH_88D */
    extern int		strmatch();
    extern char 	*strrchr();
    extern char 	*strcpy();
    extern char 	*strncpy();
    extern time_t	time();
    extern clock_t	times();
#   ifdef FLOAT
	extern double	sh_arith();
#else
	extern long	sh_arith();
#   endif /* FLOAT */
#else /* ~ _NO_PROTO */
    extern unsigned	alarm(unsigned);
    extern void 	free(void*);
/*    extern void 	*malloc(size_t);*/
    extern char 	*mac_expand(char*);
    extern char 	*mac_trim(char*,int);
    extern int 		mac_here(struct ionod*);
    extern char         *mac_try(char*);
#ifndef KSH_88D
    extern void 	mac_check(void);
#endif /* KSH_88D */
    extern int		sh_access(char*,int);
    extern char 	*sh_copy(const char*,char*);
    extern void 	sh_fail(const char*,MESG);
    extern void 	sh_funct(union anynode*,char*[],int,struct argnod*);
    extern char 	*sh_heap(const char*);
    extern char 	*sh_itos(int);
    extern int		sh_lookup(const char*,SYSTAB);
    extern char 	*sh_substitute(const char*,const char*,char*);
    extern void 	sh_trim(char*);
    extern void 	sh_whence(char**,int);
    extern char 	*strchr(const char*,int);
#ifndef KSH_88D
    extern int          strlen(const char*);
#endif /* KSH_88D */
    extern int		strmatch(const char*,const char*);
    extern char 	*strrchr(const char*,int);
    extern char 	*strcpy(char*,const char*);
    extern char 	*strncpy(char*,const char*,int);
    extern time_t	time(time_t*);
    extern clock_t	times(struct tms*);
#   ifdef FLOAT
	extern double	sh_arith(char*);
#else
	extern long	sh_arith(char*);
#   endif /* FLOAT */
#endif /* _NO_PROTO */

#if (defined(NLS) || defined(KJI))
    extern char 	*NLgetenv();
    extern int		NLstrcmp();
#endif /* NLS KJI */

/*
 * Saves the state of the shell
 */

struct sh_scoped
{
	struct ionod	*iotemp;
	unsigned	states;
	union
	{
		long	l;
#ifdef _OPTIM_
		int	i[2];
#endif /* _OPTIM_ */
	}		flags;
	jmp_buf		jmpbuf;
	int		breakcnt;
	int		execbrk;
	int		loopcnt;
	int		fn_depth;
	int		dot_depth;
	int		peekn;
	char		*cmdadr;
	VOID		(*intfn)();	/* Interrupt handler */	
	int		cmdline;
	int		firstline;
	int		exec_flag;
	int		subflag;
	int		dolc;
	char		**dolv;
	struct ionod	*iopend;
	struct argnod	*gchain;
	int		ioset;
	int		linked;
	struct fileblk	*standin;
	int		curin;
	int		standout;
	char		*trapcom[MAXTRAP];
	char		trapflg[MAXTRAP];
};

extern struct sh_scoped st;

#define opt_flags		st.flags.l

struct sh_static
{
	struct Amemory	*alias_tree;	/* for alias names */
	struct Amemory	*track_tree;	/* for tracked aliases*/
	struct Amemory	*fun_tree;	/* for function names */
	struct Amemory	*var_tree;	/* for shell variables*/
	struct namnod	*bltin_nodes;	/* pointer to built-in variables */
	char		*shname;	/* shell name */
	char		*lastpath;	/* last absolute path found */
	char		*comdiv;	/* points to sh -c argument */
	uid_t 		userid;		/* real user id */
	uid_t 		euserid;	/* effective user id */
	gid_t 		groupid;	/* real group id */
	gid_t 		egroupid;	/* effective group id */
	jmp_buf		subshell;	/* jump here for subshell */
	jmp_buf		errshell;	/* return here on failures */
	jmp_buf		*freturn;	/* return for functions return or
						fatal errors */
#ifndef	_sys_siglist_
	char		*sigmsg[NSIG+1];/* pointers to signal messages */
#endif
	struct blk	*stakbsy;	/* ptr to list of stack blocks that
						covered by heap allocation. */
	STKPTR		stakbas;	/* Base of the entire stack */
	STKPTR		brkend;		/* Current sbrk value */
	STKPTR		stakbot;	/* Base of current item */
	STKPTR		staktop;	/* Top of current item */
	int		exitval;
	char		*lastarg;
	char		*pwd;		/* present working directory */
	int		oldexit;
	int		curout;		/* current output descriptor */
	pid_t		pid;		/* process id of shell */
	pid_t		bckpid;		/* background process id */
	pid_t		subpid;		/* command substitution process id */
	long		ppid;		/* parent process id of shell */
	int		savexit;
	int		topfd;
	int		trapnote;
	char		login_sh;
	char		nested_sub;	/* for nested command substitution */
	char		heretrace;	/* set when tracing here doc */	
	char		*readscript;	/* set before reading a script */
	int		reserv;		/* set when reserved word possible */
	int		wdset;		/* states for lexical analyzer
						see sym.h */
	struct argnod	*wdarg;		/* points to current token */
	int		wdnum;		/* number associated with token */
	int		wdval;		/* type of current token */
	int		owdval;		/* saved token type for EOF */
	int		olineno;	/* linenumber of saved token */
	int		lastsig;	/* last signal received */
	union io_eval	un;		/* used for sh_eval */
	int		*inpipe;	/* input pipe pointer */
	int		*outpipe;	/* output pipe pointer */
	int		cpipe[2];
	pid_t		cpid;
#ifdef  KSH_88D
        struct fileblk  *savio;         /* top of io stack */
#endif /* KSH_88D */
};

extern struct sh_static sh;
extern time_t	sh_mailchk;
extern long	sh_timeout;
extern int	errno;
extern	pid_t	line_pid;	/*  PTM 12563 and PTM 13362  */


#ifdef pdp11
#   define ulong	long
#   ifndef INT16
#   define INT16
#   endif /* INT16 */
#else
#   define ulong	unsigned long
#endif	/* pdp11 */

#ifdef INT16
#   define path_relative(p)	(p)
#endif	/* INT16 */
