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
$ @OSF_COPYRIGHT@
$
$
$ HISTORY
$ $Log: ps.msg,v $
$ Revision 1.5  1994/11/19  01:34:42  mtm
$ Copyright additions/changes
$
$ Revision 1.4  1992/05/11  18:03:44  stans
$ Grenoble code
$
$ Revision 1.4  90/10/07  16:51:20  devrcs
$ 	Added EndLog Marker.
$ 	[90/09/28  15:02:53  gm]
$ 
$ Revision 1.3  90/07/27  09:58:29  devrcs
$ 	Add EXITING_STR.
$ 	[90/07/18  18:38:37  brezak]
$ 
$ 	Add title strings.
$ 	[90/07/13  15:43:59  brezak]
$ 
$ Revision 1.2  90/07/17  11:55:35  devrcs
$ 	Add ZOMBIE_STR and NOT_INT.
$ 	[90/07/10  16:05:10  brezak]
$ 
$ 	Message catalog for ps.
$ 	[90/07/10  11:59:03  brezak]
$ 
$ $EndLog$

$
$ IBM CONFIDENTIAL
$ Copyright International Business Machines Corp. 1989
$ Unpublished Work
$ All Rights Reserved
$ Licensed Material - Property of IBM
$
$ US Government Users Restricted Rights - Use, duplication or
$ disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$
$ @(#)$RCSfile: ps.msg,v $ $Revision: 1.5 $ (OSF) $Date: 1994/11/19 01:34:42 $
$
$ error/informational text for the ps command
$

$       NOTE: the portion of a text string that begins with "ps:" should
$               not be changed.  This is the command name.
$

$quote "

$set MS_PS

$
$ The following string gives the syntax of the ps command.  The '%s' fields
$ will contain the invariant part of the command (i.e., the command name and
$ flags).  Translation of the keywords following a flag may be facilitated by
$ reviewing their definitions in the AIX Operating System Commands Reference
$ manual 
$
$ USAGE "Usage: %1$s [%2$s]\n\
$ \t[%5$sTList][%6$sPList][%7$sUList][%8$sGList]\n\
$ \tReports process status.\n"
$
BSD_USAGE       "Usage: %s [%s] [%s format] [%stty] [process number]\n"
SYSV_USAGE      "Usage: %s [%s] [%s format] [%s tlist] [%s plist] [%s ulist] [%s glist] [%s slist]\n"

NOT_TTY         "Cannot identify terminal: %s"
FMT_2BIG        "Format specification too large"
UNKNOWN_VAR     "Unknown variable in format: %s"
VAR_2X          "Can't specify a variable twice: %s"
UNKNOWN_TYPE    "Unknown format type: %d"
NO_MEM          "Cannot allocate dynamic memory"
NO_USER		"Cannot find user: \"%s\""
NO_UAREA	"Cannot get user info for process %d"
BAD_FMT		"Cannot specify more that one format for output."
NOT_INT		"Argument must be an integer"

$ This string is printed in place of the command arguments for processes
$ that are zombies.
ZOMBIE_STR	"<defunct>"

$ This string is printed in place of the command arguments for processes
$ that are exiting.
EXITING_STR	"<exiting>"

$ These are the strings for the column titles. Most are mnemonic.
$ If you change these be aware that the length of the fields are
$ important - try to keep them the same size.
COMMAND_MSG	"COMMAND"
LOGNAME_MSG	"LOGNAME"
F_MSG		"F"
STATUS_MSG	"STATUS"
UID_MSG		"UID"
RUID_MSG	"RUID"
SVUID_MSG	"SVUID"
RGID_MSG	"RGID"
SVGID_MSG	"SVGID"
PID_MSG		"PID"
PPID_MSG	"PPID"
CP_MSG		"CP"
WCHAN_MSG	"WCHAN"
CURSIG_MSG	"CURSIG"
PENDING_MSG	"PENDING"
BLOCKED_MSG	"BLOCKED"
IGNORED_MSG	"IGNORED"
CAUGHT_MSG	"CAUGHT"
RUSER_MSG	"RUSER"
PGID_MSG	"PGID"
JOBC_MSG	"JOBC"
SESS_MSG	"SESS"
TDEV_MSG	"TDEV"
TT_MSG		"TT"
TPGID_MSG	"TPGID"
TSESS_MSG	"TSESS"
STAT_MSG	"STAT"
PRI_MSG		"PRI"
UPR_MSG		"UPR"
NI_MSG		"NI"
VSZ_MSG		"VSZ"
RSS_MSG		"RSS"
UPROCP_MSG	"UPROCP"
UMASK_MSG	"UMASK"
ACFLG_MSG	"ACFLG"
STARTED_MSG	"STARTED"
TIME_MSG	"TIME"
USER_MSG	"USER"
SYSTEM_MSG	"SYSTEM"
CPU_MSG		"%CPU"
MEM_MSG		"%MEM"
SL_MSG		"SL"
PAGEIN_MSG	"PAGEIN"
MINFLT_MSG	"MINFLT"
MAJFLT_MSG	"MAJFLT"
NSWAP_MSG	"NSWAP"
INBLK_MSG	"INBLK"
OUBLK_MSG	"OUBLK"
MSGSND_MSG	"MSGSND"
MSGRCV_MSG	"MSGRCV"
NSIGS_MSG	"NSIGS"
VCSW_MSG	"VCSW"
IVCSW_MSG	"IVCSW"
SCNT_MSG	"SCNT"
