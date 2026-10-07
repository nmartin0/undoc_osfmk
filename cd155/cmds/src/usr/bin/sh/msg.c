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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.2
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: msg.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 01:39:15 $";
#endif
/*
 * COMPONENT_NAME: (CMDSH) Bourne shell and related commands
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

#include	"defs.h"
#include	"sym.h"

/*
 * error messages
 */
uchar_t	badopt[]	= "bad option(s)";
uchar_t	badhash[]	= "bad hash option(s)";
uchar_t	mailmsg[]	= "you have mail\n";
uchar_t	nospace[]	= "no space";
uchar_t	nostack[]	= "no stack space";
uchar_t	synmsg[]	= "syntax error";
uchar_t	cd_args[]	= "too many arguments";
uchar_t	badnum[]	= "bad number";
uchar_t	badparam[]	= "parameter null or not set";
uchar_t	unset[]		= "parameter not set";
uchar_t	badsub[]	= "bad substitution";
uchar_t	badcreate[]	= "cannot create";
uchar_t	nofork[]	= "fork failed - too many processes";
uchar_t	noswap[]	= "cannot fork: no swap space";
uchar_t	restricted[]	= "restricted";
uchar_t	piperr[]	= "cannot make pipe";
uchar_t	badopen[]	= "cannot open";
uchar_t	coredump[]	= " - core dumped";
uchar_t	arglist[]	= "arg list too long";
uchar_t	txtbsy[]	= "text busy";
uchar_t	toobig[]	= "too big";
uchar_t	badexec[]	= "cannot execute";
uchar_t	notfound[]	= "not found";
uchar_t	badfile[]	= "bad file number";
uchar_t	badshift[]	= "cannot shift";
uchar_t	baddir[]	= "bad directory";
uchar_t	badtrap[]	= "bad trap";
uchar_t	wtfailed[]	= "is read only";
uchar_t	notid[]		= "is not an identifier";
uchar_t 	badulimit[]	= "Bad ulimit";
uchar_t	badreturn[] = "cannot return when not in function";
uchar_t	badexport[] = "cannot export functions";
uchar_t	badunset[] 	= "cannot unset";
uchar_t	nohome[]	= "no home directory";
uchar_t 	badperm[]	= "execute permission denied";
uchar_t	longpwd[]	= "sh error: pwd too long";
uchar_t	badinlib[]	= "loader install procedure failed";
uchar_t	badrmlib[]	= "loader removal procedure failed";
uchar_t	no_args[]	= "requires argument.";


/*
 * messages for 'builtin' functions
 */
uchar_t	btest[]		= "test";
uchar_t	badop[]		= "unknown operator ";
/*
 * built in names
 */
uchar_t	pathname[]	= "PATH";
uchar_t	cdpname[]	= "CDPATH";
uchar_t	homename[]	= "HOME";
uchar_t	mailname[]	= "MAIL";
uchar_t	ifsname[]	= "IFS";
uchar_t	ps1name[]	= "PS1";
uchar_t	ps2name[]	= "PS2";
uchar_t	mchkname[]	= "MAILCHECK";
uchar_t	acctname[]  	= "SHACCT";
uchar_t	mailpname[]	= "MAILPATH";
uchar_t    mailmname[]	= "MAILMSG";
uchar_t    timename[]	= "TIMEOUT";
uchar_t    shellname[]     = "SHELL";
uchar_t	lang[]		= "LANG";
uchar_t	nlspath[]	= "NLSPATH";
uchar_t    ctype[]		= "LC_CTYPE";
uchar_t    collate[]	= "LC_COLLATE";
uchar_t    monetary[]	= "LC_MONETARY";
uchar_t    numeric[]	= "LC_NUMERIC";
uchar_t    lctime[]	= "LC_TIME";
uchar_t    messages[]	= "LC_MESSAGES";


/*
 * string constants
 */
uchar_t	nullstr[]	= "";
#ifdef KJI
			  /* includes double-wide space */
uchar_t	sptbnl[]	= {0x20, 0x09, 0x0a, 0x81, 0x40, 0x00};
#else
uchar_t	sptbnl[]	= " \t\n";
#endif
uchar_t	shdefpath[]	= ":/usr/bin";
uchar_t	sudefpath[]	= "/sbin:/usr/sbin:/usr/bin";

uchar_t	colon[]		= ": ";
uchar_t	minus[]		= "-";
uchar_t	endoffile[]	= "end of file";
uchar_t	unexpected[] 	= " unexpected";
uchar_t	atline[]	= " at line ";
uchar_t	devnull[]	= "/dev/null";
uchar_t	execpmsg[]	= "+ ";
uchar_t	readmsg[]	= "> ";
uchar_t	shstdprompt[]	= "$ ";
uchar_t	shsupprompt[]	= "# ";

uchar_t	profile[]	= ".profile";
uchar_t	sysprofile[]	= "/etc/profile";

/*
 * tables
 */

struct sysnod reserved[] =
{
	{ "case",	CASYM	},
	{ "do",		DOSYM	},
	{ "done",	ODSYM	},
	{ "elif",	EFSYM	},
	{ "else",	ELSYM	},
	{ "esac",	ESSYM	},
	{ "fi",		FISYM	},
	{ "for",	FORSYM	},
	{ "if",		IFSYM	},
	{ "in",		INSYM	},
	{ "then",	THSYM	},
	{ "until",	UNSYM	},
	{ "while",	WHSYM	},
	{ "{",		BRSYM	},
	{ "}",		KTSYM	}
};

int no_reserved = 15;

uchar_t	export[] = "export";
uchar_t	duperr[] = "cannot dup";
uchar_t	readonly[] = "readonly";


struct sysnod commands[] =
{
	{ ".",		SYSDOT	},
	{ ":",		SYSNULL	},
	{ "[",		SYSTST },
	{ "break",	SYSBREAK },
	{ "cd",		SYSCD	},
	{ "continue",	SYSCONT	},
	{ "echo",	SYSECHO },
	{ "eval",	SYSEVAL	},
	{ "exec",	SYSEXEC	},
	{ "exit",	SYSEXIT	},
	{ "export",	SYSXPORT },
	{ "hash",	SYSHASH	},
	{ "inlib", 	SYSINLIB },
	{ "login",	SYSLOGIN },
        { "newgrp",     SYSNEWGRP },
	{ "pwd",	SYSPWD },
	{ "read",	SYSREAD	},
	{ "readonly",	SYSRDONLY },
	{ "return",	SYSRETURN },
	{ "rmlib", 	SYSRMLIB },
	{ "set",	SYSSET	},
	{ "shift",	SYSSHFT	},
	{ "test",	SYSTST },
	{ "times",	SYSTIMES },
	{ "trap",	SYSTRAP	},
	{ "type",	SYSTYPE },
	{ "ulimit",	SYSULIMIT },
	{ "umask",	SYSUMASK },
	{ "unset", 	SYSUNS },
	{ "wait",	SYSWAIT	}
};

int	no_commands = sizeof(commands) / sizeof(struct sysnod);
