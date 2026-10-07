$ 
$ $Copyright
$ Copyright 1993, 1994, 1995  Intel Corporation
$ INTEL CONFIDENTIAL
$ The technical data and computer software contained herein are subject
$ to the copyright notices; trademarks; and use and disclosure
$ restrictions identified in the file located in /etc/copyright on
$ this system.
$ Copyright$
$ 
 
$
$ (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
$ ALL RIGHTS RESERVED
$
$
$ OSF/1 Release 1.0

$ @(#)csh.msg	1.4  com/cmd/csh,3.1,9008 11/15/89 15:51:57
$ *
$ * COMPONENT_NAME: CMDCSH  c shell(csh)
$ *
$ * FUNCTIONS:
$ *
$ * ORIGINS: 10,26,27
$ *
$ * This module contains IBM CONFIDENTIAL code. -- (IBM
$ * Confidential Restricted when combined with the aggregated
$ * modules for this product)
$ * OBJECT CODE ONLY SOURCE MATERIALS
$ * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
$ * All Rights Reserved
$ *
$ * US Government Users Restricted Rights - Use, duplication or
$ * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$ *
$ * (Copyright statements and/or associated legends of other
$ * companies whose code appears in any part of this module must
$ * be copied here.)
$ *
$ 

$
$ 	Messages for the C shell.
$
$quote  "

$set MS_CSH

$ The following message is for dodirs in dir.c
M_DIRS		"Usage: dirs [ -l ]"

$ The following messages are for dochngd in dir.c
M_NOHOME	"No home directory"
M_NOCHANGE	"Can't change to home directory"

$ The following message is for dopushd in dir.c
M_NODIR		"No other directory"

$ The following message is for dfind in dir.c
M_STACK		"Directory stack not that deep"

$ The following messages are for dopopd in dir.c
M_BADDIR	"Bad directory"
M_EMPTY		"Directory stack empty"

$ The following message is for Dfix1 in dol.c
M_AMBIG		"Ambiguous"

$ The following message is for Dword in dol.c
M_NOQUOTE	"Unmatched %c"

$ The following messages are for Dgetdol in dol.c
M_TOOLONG	"$< line too long"
M_NOFILE	"No file for $0"
M_VARSYN	"Variable syntax"
M_SUBOUT	"Subscript out of range"
M_DOLMOD	"Bad : mod in $"

$ The following messages are for heredoc in dol.c
M_TERM		"<< terminator not found"
M_OVER		"Line overflow"

$ These are for the loader builtins
M_BADINLIB	"inlib install failed.\n"
M_BADRMLIB	"rmlib removal failed.\n"

M_NOERR		"Unknown error %d occurred.\n"

$ The following message is for doexec in exec.c
M_NOMATCH	"No match"

$ The following message is for pexerr in exec.c
M_NOTFOUND	"Command not found"

$ The following messages are for exp5 in exp.c
M_DIV		"Divide by 0"
M_MOD		"Mod by 0"

$ The following messages are for exp6 in exp.c
M_EXPR		"Expression syntax"
M_NOFNAME	"Missing file name"

$ The following messages are for func in func.c
M_TOOFEW	"Too few arguments"
M_TOOMANY	"Too many arguments"

$ The following message is for doonintr and donohup in func.c
M_CANT		"Can't from terminal"

$ The following message is for doalias in func.c
M_ALIAS		"Too dangerous to alias that"

$ The following messages are for doif in func.c
M_EMPTYIF	"Empty if"
M_THEN		"Improper then"

$ The following message is for doswitch in func.c
M_SYNERR	"Syntax error"

$ The following message is for dobreak, docontin and doend in func.c
M_WHILE		"Not in while/foreach"

$ The following messages are for doforeach in func.c
M_INVVAR	"Invalid variable"
M_PAREN		"Words not ()'ed"

$ The following messages are for getword in func.c
M_ZIF		"then/endif not found"
M_ZELSE		"endif not found"
M_ZSWITCH	"endsw not found"
M_ZBREAK	"end not found"
M_ZGOTO		"label not found"

$ The following message is for findlim in func.c
M_NOLIMIT	"No such limit"

$ The following message is for dolimit in func.c
M_NORAISE	"Limits cannot be raised\n"

$ The following message is for getval in func.c
M_SCALE		"Improper or unknown scale factor"

$ The following message is for islogin in func.c
M_NOTLOGIN	"Not login shell"

$ The following message is for limtail in func.c
M_BAD		"Bad scaling; did you mean ``%s''?"

$ The following messages are for dosuspend in func.c
M_SUSPEND	"Can't suspend a login shell (yet)"
M_TTY		"Switching to new tty driver...\n"

$ The following message is for plim in func.c
M_UNLIMITED	"unlimited"

$ The following message is for expand in glob.c
M_UNKNOWN	"Unknown user: %s"

$ The following messages are for execbrc, amatch, Gmatch in glob.c
M_MISSBRK	"Missing ]"
M_MISSBRC	"Missing }"

$ The following message is for Gcat in glob.c
M_ARGS		"Arguments too long"

$ The following message is for addpath in glob.c
M_PATH		"Pathname too long"

$ The following message is for dobackp in glob.c
M_NOBACK	"Unmatched `"

$ The following message is for psave in glob.c
M_WORD		"Word too long"

$ The following message is for pword in glob.c
M_COMSUB	"Too many words from ``"

$ The following message is for word in lex.c
M_UNMATCH	"Unmatched "

$ The following message is for addla in lex.c
M_LABUF		"Expansion buf ovflo"

$ The following message is for getexcl in lex.c
M_BADEXCL	"Bad ! form"

$ The following messages are for getsub in lex.c
M_SUB		"No prev sub"
M_BADSUB	"Bad substitute"
M_BADLHS	"No prev lhs"
M_BADRHS	"Rhs too long"
M_BADEXCLM	"Bad ! modifier: "

$ The following message is for dosub in lex.c
M_MODFAIL	"Modifier failed"

$ The following message is for subword in lex.c
M_OVFLO		"Subst buf ovflo"

$ The following message is for getsel in lex.c
M_BADARG	"Bad ! arg selector"

$ The following message is for gethent in lex.c
NO_SEARCH	"No prev search"

$ The following message is for noev in lex.c
M_NOEVENT	": Event not found"

$ The following messages are for readc in lex.c
M_RESET		"Reset tty pgrp from %d to %d\n"
M_LOGOUT	"\nUse \"logout\" to logout.\n"
M_EXIT		"\nUse \"exit\" to leave csh.\n"

$ The following message is for calloc in misc.c
M_NOMEM		"Out of memory"

$ The following message is for udvar in misc.c
M_UNDEF		"Undefined variable"

$ The following message is for alias in parse.c
M_ALIASLOOP	"Alias loop"

$ The following message is for asyn0 and syn0 in parse.c
M_RPARENS	"Too many )'s"
M_LPARENS	"Too many ('s"

$ The following messages are for syn3 in parse.c
M_BADPAREN	"Badly placed ("
M_REDIRECT	"Missing name for redirect"
M_BADOUT	"Ambiguous output redirect"
M_HERE		"Can't << within ()'s"
M_INPUT		"Ambiguous input redirect"
M_BADPARENS	"Badly placed ()'s"
M_NULL		"Invalid null command"

$ The following message is for pjwait in proc.c
M_BUG1		"BUG: waiting for background job!\n"

$ The following message is for pflush in proc.c
M_BUG2		"BUG: process flushed twice"

$ The following messages are for pprint in proc.c
M_RUN		"Running "
M_EXITNUM	"Exit %-16d"
M_DONE		"Done"
M_CORE		" (core dumped)"
M_WD		"(wd now: "
M_BUG3		"BUG: status=%-9o"

$ The following message is for dojobs in proc.c
M_JOBS		"Usage: jobs [ -l ]"

$ The following messages are for dokill in proc.c
M_BADSIG	"Bad signal number"
M_NOSIG		"Unknown signal; kill -l lists signals"

$ The following messages are for pkill in proc.c
M_STOPD		"%s: Already stopped\n"
M_ID		"Arguments should be jobs or process id's"

$ The following message is for panystop in proc.c
M_SJOBS		"\nThere are stopped jobs"

$ The following messages are for pfind in proc.c
M_CURJOB	"No current job"
M_PREVJOB	"No previous job"
M_SUCHJOB	"No such job"
M_NOJOB		"No job matches pattern"

$ The following message is for xpfork in sem.c
M_NOPROCS	"No more processes"

$ The following messages are for okpcntl in sem.c
M_NOJCNTL	"No job control in this shell"
M_NOSJCNTL	"No job control in subshells"

$ The following message is for mypipe in sem.c
M_NOPIPE	"Can't make pipe"

$ The following message is for chkclob in sem.c
M_FILE		"%s: File exists"

$ The following message is for doset in set.c
M_MISSP		"Missing )"

$ The following message is for getinx in set.c
M_SUBERR	"Subscript error"

$ The following message is for getn in set.c
M_BADNUM	"Badly formed number"

$ The following message is for shift in set.c
M_NOMORE	"No more words"

$ The following message is for exportpath in set.c
M_LONGPATH	"Warning: ridiculously long PATH truncated\n"

$ The following message is for main in sh.c
M_NOTTY		"Warning: no access to tty; thus no job control in this shell...\n"
M_BYEBYE	"logout\n"

$ The following message is for dosource in sh.c
M_OPTION	"Unknown option : %s \n"

$ The following message is for pintr1 in sh.c
M_INTER		"Interrupted"

$ The following messages are for mailchk in sh.c
M_NEWMAIL	"You have new mail.\n"
M_MAIL		"You have mail.\n"
M_NEWMAILIN	"New mail in %s.\n"
M_MAILIN	"Mail in %s.\n"

$ The following messages are for ptimes in time.c
M_U		"u "
M_S		"s "

$ The following messages are for file.c
YIKES1          "\nYikes!! Too many names in password file!!\n"
YIKES2          "\nYikes!! Too many files!!\n"
YIKES3		"\nTIOCSTI ioctl unsupported -- file name completion code not possible\n"
