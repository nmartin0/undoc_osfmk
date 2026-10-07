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
$ @(#)ksh.msg	1.6  com/cmd/ksh,3.1,9013 3/14/90 10:21:08
$
$ COMPONENT_NAME: (CMDKSH) Korn shell
$
$ FUNCTIONS:
$
$ ORIGINS: 27
$
$ IBM CONFIDENTIAL -- (IBM Confidential Restricted when
$ combined with the aggregated modules for this product)
$ OBJECT CODE ONLY SOURCE MATERIALS
$ (C) COPYRIGHT International Business Machines Corp. 1989
$ All Rights Reserved
$
$ US Government Users Restricted Rights - Use, duplication or
$ disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$
$
$   messages for ksh88 on AIX v2.x
$
$quote  "
$set MS_KSH
$ Do not translate ksh


$ $
$ $ from msg.c
$ $

$ $     information or multiple message text

E_ACCESS        "Permission denied."
E_ALIAS         ": ksh alias not found."
$ The alias is printed out before this message.
$ alias refers to the kshell alias.
E_ALINAME       "Invalid alias name."
E_AMBIGUOUS     "Job name is ambiguous."
E_ATLINE        " at line "
$ This is preceded by E_SYNBAD and is followed by a line number.
E_CRONDIR       "/usr/spool/cron/atjobs"
$ Do not translate E_CRONDIR
E_FORMAT        "The umask permission format is not correct, use chmod parameter syntax."
$ Do not translate umask, chmod
E_FOUND         " not found."
$ This is preceded by a filename, or whatever could not be found.
E_FUNCTION      "Arithmetic expression contains a function that is not known."
E_IDENT         "This is not an identifier."
$ This is preceded by the bad identifier name, which is followed by a colon.
E_LOGOUT        "Use 'exit' to logout."
E_MAILMSG       "You have mail in $_"
$       do not translate $_
E_OFF           "off"
$ This is used in a table for output from a query command
E_ON            "on"
$ This is used in a table for output from a query command
E_PWD           "Cannot access parent directories."
E_READONLY      "This variable is read only."
$ This is preceded by the read only variable name, which is followed by
$ a colon.
E_TIMEWARN      "\r\n\007Shell will time out in 60 seconds."
$       do not translate \r\n\007
E_TIMEOUT       "Timed out waiting for input."

E_UNEXPECTED    " is not expected."
$ This is preceded by the symbol that was not expected, which is followed
$ by a colon.  Leave the space at the front.

E_UNMATCHED     " is not matched."
$ This is preceded by the symbol that was not matched, which is followed
$ by a colon.  Leave the space at the front.

E_HEADING       "Current option settings are:"
$       header for printing current options in args.c

E_ENDOFFILE     "end of file"
$       end of file token as printed in cmd.c

$ $     job control information messages
E_COREDUMP      "(coredump)"
$ Do not translate
E_DONE          " Done"
$ Leave a space at the start. This comes out on the line to show
$ when a background job has completed.
E_JOBUSAGE      "Specify a process identifier or a %job number."
$ This tells the user to specify a process id or a % that is
$ followed by the job number.
E_KILLCOLON     "kill: "
$       The "kill" is from the kill command.
$ Do not translate
E_NO_JCTL       "Job control has been disabled."
E_NO_JOB        "The specified job does not exist."
E_NO_PROC       "The specified process does not exist."
E_RUNNING       " Running"
$ This is a status message, that is preceded by a job number.
E_RUNNINGJ      "There are running jobs."
E_TERMINATE     "There are stopped jobs."
E_UNKNOWN       "<job name not known>"

E_UNLIMITED     "unlimited"
$  Do not translate
$       ulimit messages

E_REAL          "\nreal"
$  Do not translate
$        Information message from time builtin command
$        Actual elapsed time
E_SYS           "sys"
$  Do not translate
$        Information message from time builtin command
$        Kernel time
E_USER          "user"
$  Do not translate
$        Information message from time builtin command
$        User time

$ $     information messages in name.c

IS_RESERVED     " is a keyword."
$ This and the next 8 messages are output from the kshell whence command
$ They are preceded by the name that is being queried.
IS_BUILTIN      " is a shell builtin."
$ builtin is referring to a function that is contained within the
$ shell program.
IS_ALIAS        " is an alias for "
$ This is followed by an alias name
IS_FUNCTION     " is a function."
IS_XALIAS       " is an exported alias for "
$ This is followed by an alias name
IS_TALIAS       " is a tracked alias for "
$ This is followed by an alias name
IS_XFUNCTION    " is an exported function."
IS_UFUNCTION    " is an undefined function."
IS_             " is "
$ This is followed by


$ $     general error messages

E_ARGEXP        "Specify a parameter with this command."
E_CREATE        "Cannot create the specified file."
E_EXEC          "Execute permission denied."
E_FILE          "Generated or received a file descriptor number that is not valid."
E_NARGS         "The number of parameters specified is not correct."
E_NUMBER        "The specified number is not valid for this command."
E_OPTION        "A specified flag is not valid for this command."
E_SUBST         "The specified substitution is not valid for this command."
E_TESTOP        "A test command parameter is not valid."
E_TOOBIG        "There is not enough memory available to run the command."
E_TRAP          "The specified trap syntax is not correct."
$ Do not translate trap.
E_TXTBSY        "Cannot access a required executable file. It is in use."
E_OPEN          "Cannot find or open the file."
E_PIPE          "Cannot create a pipe."
$ Do not translate pipe.

E_PROHIBITED    "Login shells that are setuid or setgid are not allowed."
$ Do not translate setuid/setgid, login
$       error for setuid/setgid shells in main.c

E_RESTRICTED    "The operation is not allowed in a restricted shell."
$       error for restricted shell operation

E_ARGTEST       "Specify a parameter with this command."
$       error for test command parameters in test.c
E_BRACKET       "A ] character is missing."

E_HISTORY       "Cannot access or open the history file."
$ Do not translate history.
$       error messge when history file open fails

$ $     error messages from failed execve in service.c

E_LIBACC        "Cannot access a needed shared library."
E_LIBBAD        "The shared library is damaged."
E_LIBSCN        "The executable file is damaged."
E_LIBMAX        "Cannot link the required number of libraries."
E_ARGLIST       "The parameter list is too long."

E_SHIFT         "The specified number is not valid for this command."
$       shift builtin command message

E_SPACE         "There is not enough memory available now."
$       error message for allocating memory in blok.c

E_FORK          "The fork function failed. Too many processes already exist."
$ Do not translate fork.
$       from fork command in xec.c

E_SWAP          "The fork function failed. There is not enough memory available."
$ Do not translate fork

E_INLIB          "The inlib install function failed."
E_RMLIB         "The rmlib removal function failed."

E_NO_START      "Cannot start job control."
$       error messages for job control

E_RESETLD       "ksh: Cannot reset line disciplines."
$       error message for reset of line disciplines

E_SETLD         "ksh: Cannot set line disciplines."
$       error message for set of line disciplines

$ $     error messages from builtin cd command
E_LONGNAME      "The specified path name is too long."
E_DIRECT        "The specified directory is not valid."
E_NOTDIR        "The specified path name is not a directory."

E_UMASK         "Valid umask digit values are from 0 to 7."
$ Do not translate umask
$       error message for invalid umask

E_QUERY         "No query process for pipe."
$ Do not translate pipe
$       error message for coprocess in builtin.c

$ $     error messages in function getch in macro.c
E_NULLSET       "Parameter null or not set."
E_NOTSET        "Parameter not set."

E_PEXISTS       "Process already exists for pipe."
$ Do not translate pipe
$       error messages for coprocess pipe in xec.c

E_FEXISTS       "The noclobber option was specified and the file already exists."
$ Do not translate noclobber
$       error messages for noclobber option in io.c

E_FLIMIT        "Trying to open too many files."
$       error message for max files open in io.c

E_ULIMIT        "The specified value exceeds the user's allowable limit."
$       error message from ulimit builtin command in builtin.c

E_SUBSCRIPT     "The specified subscript cannot be greater than 512."
$       error message in shlib/growaray.c


$ $
$ $ from strdata.c
$ $

E_MORETOKENS    "Expression is not complete; more tokens expected."
$ do not translate token.
E_PAREN         "There must be an equal number of ( and ) characters."
E_BADCOLON      "A : character is not used correctly."
E_DIVZERO       "Cannot divide by zero."
E_SYNBAD        "Syntax error"
$ This is followed by   E_ATLINE  which is followed by a colon then by the
$ bad symbol. This then is followed by  E_UNEXPECTED or E_UNMATCHED
$ Example:
$ cmd: Syntax error at line 3: * is not expected.
E_NOTLVALUE     "Assignment requires an lvalue."
$ Do not translate lvalue.
E_RECURSIVE     "There cannot be more than 9 levels of recursion."
E_QUESTCOLON    "The ? operator requires a : character."
E_INCOMPATIBLE  "The operand types are not compatible."
E_TCSETPGRP     "Error setting foreground process group."



$ $     messages that are currently not used because they are
$ $     inside an ifdef that is not defined

E_LINK          "remote link inactive"
$ Message not used...do not translate
E_MULTIHOP      "multihop attempted"
$ Message not used...do not translate
E_NEWTTY        "Switching to new tty driver..."
$ Message not used...do not translate
E_OLDTTY        "Reverting to old tty driver..."
$ Message not used...do not translate
