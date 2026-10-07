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

$
$ IBM CONFIDENTIAL
$ Copyright International Business Machines Corp. 1989
$ Unpublished Work
$ All Rights Reserved
$ Licensed Material - Property of IBM
$
$
$ RESTRICTED RIGHTS LEGEND
$ Use, Duplication or Disclosure by the Government is subject to
$ restrictions as set forth in paragraph (b)(3)(B) of the rights in
$ Technical Data and Computer Software clause in DAR 7-104.9(a).
$ 
$ @(#)init.msg	1.1  com/cmd/oper/init,3.1,8943 9/18/89 10:57:35
$ comment
$quote "         define quote character.

$set MS_INIT      Define initial set#

M_SETSID "setsid() failed: errno = %d\n"
M_OPEN "Cannot open %s  errno: %d\n"
M_NEWLVL "New run level: %c\n"
M_SUSER "SINGLE-USER MODE\n"
M_EXEC "execlp of %s failed; errno = %d\n"
M_PTAB "Internal process table is full.\n"
M_RAPID "Command is respawning too rapidly. Check for possible errors.\nid:%6s \"%s\"\n"
M_CMDFAIL "Command\n\"%s\"\n failed to execute.  errno = %d (exec of shell failed)\n"
M_REMOTE "\nInit: system console is remote: %s Type <DEL> to regain control.\n"
M_NEWLINE "\n"
M_LVLPROMPT "Enter run level (0-9, s, S, m, or M): "
M_CHANGE "will change to state %c\n"
M_BADCHAR "\nbad character <%3.3o>\n\n"
M_UTMP "failed write of utmp entry: \"%2.2s\"\n"
M_USAGE "Usage: init [0123456789SsMmQqabc]\n"
M_NOTTY "Standard input not a tty line\n"
M_MOVCNTL "Moving control to CONSOLE device.\n"
M_SUENTER "\n**** SINGLE-USER MODE ENTERED FROM %s ****\n"
M_NOSIG "Could not send signal to \"init\".\n"
M_DANGER "Paging space low!\n"
M_WARNING  "WARNING: Something is hung (won't die); ps axl advised\n"

$set MS_INIT_SEC

M_SECEXEC "failed to start single-user shell; errno = %d\n"
M_INSUFFPRIV "init: insufficient privileges\n"
M_NOTAUTH "Need sysadmin authorization to change run-level.\n"
M_EXECL_FAIL	"execl of %s failed; errno = %d\r\n"
