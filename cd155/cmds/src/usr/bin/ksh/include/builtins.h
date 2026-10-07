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
/* static char rcsid[] = "@(#)$RCSfile: builtins.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:27:48 $"; */
/*
 * COMPONENT_NAME: (CMDKSH) Korn shell
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 27
 *
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
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
/* table of shell builtins */
/* currently can not be more than SYSMAX */

#define SYSMAX		0xff
#define SYSDECLARE	0x100

/* The following commands up to SYSNOCOMSUB create a separate process
 * to carry out command substitution.
 * Also, commands below SYSSPECIAL retain variable assignments  and
 * cause a script that contains them to abort when encountering errors.
 */

#define SYSEXEC		1
#define SYSLOGIN	2
#define SYSEVAL		3
#define SYSDOT 		4
#define SYSNOCOMSUB	4
#define SYSALIAS	5
#define SYSTYPESET	6
#define SYSRDONLY	7
#define SYSXPORT 	8
#define SYSNULL 	9
#define SYSRETURN	10
#define SYSEXIT		11
#define SYSCONT 	12
#define SYSBREAK 	13
#define	SYSSHFT		14
#define SYSWAIT		15
#define SYSTRAP 	16
#define SYSTIMES 	17
#define SYSSPECIAL	17	/* end of special commands */
#define SYSUNSET 	18
#define SYSUNALIAS	19
#define SYSUMASK	20
#define SYSULIMIT	21
#define SYSFC		22
#define SYSREAD		23
#define SYSSET		24
#define SYSCD 		25
#define SYSLET		26
#define SYSWHENCE	27
#define SYSTEST		28
#define SYSPRINT	29
#define SYSECHO		30
#define SYSPWD		31
#define SYSFG		32
#define SYSBG		33
#define	SYSJOBS		34
#define SYSKILL		35
#define SYSGETOPTS	36
#ifdef FD_3D
#   define	SYSVMAP		39
#   define	SYSVPATH	40
#endif /* FD_3D */
#ifdef  KSH_88D
#ifdef UNIVERSE
#   define SYSUNIVERSE          41
#endif /* UNIVERSE */
#ifdef POSIX
#   define SYSCOMMAND           42
#endif /* POSIX */
#endif /* KSH_88D */


#   define      SYSSHELL        43
#   define      SYSSU           44
#   define      SYSSETGROUPS    45
#   define      SYSLOGOUT       46
#   define      SYSSETSENV      47
#   define      SYSNEWGRP       48
#   define	SYSINLIB	49
#   define	SYSRMLIB	50
	
/* structure for builtin shell variable names and aliases */
struct name_value
{
	const char	*nv_name;
	const char	*nv_value;
	const unsigned short	nv_flags;
};

/* The following defines are coordinated with data in msg.c */

#define	PATHNOD		(sh.bltin_nodes)
#define PS1NOD		(sh.bltin_nodes+1)
#define PS2NOD		(sh.bltin_nodes+2)
#define IFSNOD		(sh.bltin_nodes+3)
#define PWDNOD		(sh.bltin_nodes+4)
#define HOME		(sh.bltin_nodes+5)
#define MAILNOD		(sh.bltin_nodes+6)
#define REPLYNOD	(sh.bltin_nodes+7)
#define SHELLNOD	(sh.bltin_nodes+8)
#define EDITNOD		(sh.bltin_nodes+9)
#define MCHKNOD		(sh.bltin_nodes+10)
#define RANDNOD		(sh.bltin_nodes+11)
#define ENVNOD		(sh.bltin_nodes+12)
#define HISTFILE	(sh.bltin_nodes+13)
#define HISTSIZE	(sh.bltin_nodes+14)
#define FCEDNOD		(sh.bltin_nodes+15)
#define CDPNOD		(sh.bltin_nodes+16)
#define MAILPNOD	(sh.bltin_nodes+17)
#define PS3NOD		(sh.bltin_nodes+18)
#define OLDPWDNOD	(sh.bltin_nodes+19)
#define VISINOD		(sh.bltin_nodes+20)
#define COLUMNS		(sh.bltin_nodes+21)
#define LINES		(sh.bltin_nodes+22)
#define PPIDNOD		(sh.bltin_nodes+23)
#define L_ARGNOD	(sh.bltin_nodes+24)
#define TMOUTNOD	(sh.bltin_nodes+25)
#define SECONDS		(sh.bltin_nodes+26)
/* ERRNO is 27 */
/* LINENO is 28 */
/* OPTIND is 29 */
/* OPTARG is 30 */
#define PS4NOD		(sh.bltin_nodes+31)
#define FPATHNOD	(sh.bltin_nodes+32)

#if	(defined(NLS) || defined(KJI))
#define	NLSNOD	35
#define LANGNOD		(sh.bltin_nodes+33)
#define	NLSPATHNOD	(sh.bltin_nodes+34)
#else
#define	NLSNOD	33
#endif

#ifdef	_locale_
#define	LOCALENOD	NLSNOD+6
#define LCTYPENOD	(sh.bltin_nodes+NLSNOD)
#define	LCOLLTNOD	(sh.bltin_nodes+NLSNOD+1)
#define	LCMONTNOD	(sh.bltin_nodes+NLSNOD+2)
#define	LCNUMNOD	(sh.bltin_nodes+NLSNOD+3)
#define	LCTIMENOD	(sh.bltin_nodes+NLSNOD+4)
#define	LCMESSNOD	(sh.bltin_nodes+NLSNOD+5)
#else
#define	LOCALENOD	NLSNOD
#endif	/* _locale_ */

#ifdef VPIX
#   define DOSPATHNOD	(sh.bltin_nodes+LOCALENOD)
#   define VPIXNOD	(sh.bltin_nodes+LOCALENOD+1)
#   define NVPIX	2
#else
#   define NVPIX	0
#endif /* VPIX */
#ifdef ACCT
#   define ACCTNOD 	(sh.bltin_nodes+LOCALENOD+NVPIX)
#   define NACCT	NVPIX+1
#else
#   define NACCT	NVPIX
#endif	/* ACCT */
#ifdef MULTIBYTE
#   define CSWIDTHNOD 	(sh.bltin_nodes+LOCALENOD+NACCT)
#endif /* MULTIBYTE */


#define is_rbuiltin(t)	(((t)->tre.tretyp&COMMSK)==TCOM && \
			((((t)->tre.tretyp)>>(COMBITS+1))>SYSNOCOMSUB) && \
			((((t)->tre.tretyp)>>(COMBITS+1))!=SYSFC)) 

#ifdef _NO_PROTO
    extern void sh_builtin();
#else /* ~ _NO_PROTO */
    extern void sh_builtin(int,int,char*[],union anynode*);
#endif /* _NO_PROTO */

extern const char	e_unlimited[];
extern const char	e_ulimit[];
extern const char	e_notdir[];
extern const char	e_direct[];
extern const char	e_defedit[];
#ifdef EMULTIHOP
    extern const char	e_multihop[];
#endif /* EMULTIHOP */
#ifdef ENAMETOOLONG
    extern const char	e_longname[];
#endif /* ENAMETOOLONG */
#ifdef ENOLINK
    extern const char	e_link[];
#endif /* ENOLINK */
#ifdef VPIX
    extern const char	e_vpix[];
    extern const char	e_vpixdir[];
#endif /* VPIX */
extern const struct name_value node_names[];
extern const struct name_value alias_names[];
extern const struct name_value tracked_names[];
extern const struct sysnod	tab_builtins[];
#ifdef _sys_resource_
    extern const struct sysnod limit_names[];
#else
#   ifdef VLIMIT
	extern const struct sysnod limit_names[];
#   endif	/* VLIMIT */
#endif	/* _sys_resource_ */

