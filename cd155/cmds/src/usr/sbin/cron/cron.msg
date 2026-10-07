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
$ @(#)$RCSfile: cron.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:05:04 $
$ 
$ COMPONENT_NAME: (CMDOPER) commands needed for basic system needs
$ 
$ FUNCTIONS:
$ 
$ ORIGINS: 27
$ 
$ IBM CONFIDENTIAL -- (IBM Confidential Restricted when
$ combined with the aggregated modules for this product)
$ OBJECT CODE ONLY SOURCE MATERIALS
$ 
$ (C) COPYRIGHT International Business Machines Corp. 1989
$ All Rights Reserved
$ 
$ US Government Users Restricted Rights - Use, duplication or
$ disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$
$ cron.msg  1.14  com/cmd/oper/cron,3.1,9021 4/18/90 09:02:05
$ 
$quote "         define quote character.

$len 
$set MS_CRON        Define initial set#
$ Do not translate "at" at the beginning of line.
$ Do not translate %s, %d, %1$s, %1$d, etc ...
$ Do not translate "cron" or "crontab" (these are command names) 
$ Do not translate any of the specific directory names: 
$ Things like:  /var/spool/cron/crontabs 

MS_BADCD             "Cannot change to the /var/spool/cron/crontabs directory.\n"

MS_NOREADDIR         "Cannot read the /var/spool/cron/crontabs directory.\n"

MS_BADJOBOPEN        "Cannot open the at file.\n"
$ "at" is a job type, it is a unix command (do not translate) 

MS_BADSHELL          "Your login shell must be /bin/sh\n\
\tto use the cron command.\n"
$ This is an obsolete message, Do not translate  

MS_BADSTAT           "Cannot access the crontab file.\n"
$ Do not translate crontab. 

MS_CANTCDHOME        "Cannot change to your home directory.\n"

MS_CANTEXECSH        "Cannot run the shell for one of your commands.\n"

MS_EOLN              "Reached an end-of-line that is not expected.\n"
$ Do not translate crontab 

MS_NOREAD            "Cannot read the crontab file.\n"
$ Do not translate crontab. 

MS_NOSTDIN           "Cannot read from standard input.\n"

MS_OUTOFBOUND        "The date or time is either too large or too small.\n"

MS_STDERRMSG         "\n\n\
*****************************************************************\n\
\tcron: The previous message is the standard output\n\
\tand standard error of one of the cron commands.\n"
$ this is informational --  Do not translate cron. 

MS_STDOUTERR         "A cron command generated standard output\n\
\tor standard error, but cron could not mail you this output.\n\
\tRemember to redirect the standard output and standard error\n\
\tfor each of your commands.\n"
$ Do not translate cron. 

MS_UNEXPECT          "Reached a symbol that is not expected.\n"
$ Do not translate crontab 

MS_NOQUEDEFS         "Cannot find or access the /var/adm/cron/queuedefs file.\n"

MS_NOQDEFS2          "Using the default queue definitions.\n"
$ This one follows the MS_NOQUEDEFS message 

MS_CRSTART           "The cron program has started. The process number is %d.\n"
$ Do not translate cron -- %d will be a number. 

MS_NOFIFO1           "Cannot access the named pipe /var/adm/cron/FIFO.\n"

MS_BANGOPEN          "open"
$ Do not translate 

MS_NOFIFO2           "Cannot open the named pipe /var/adm/cron/FIFO.\n"

MS_NOWRITE           "Cannot write to %s.\n"
$ %s is replaced by a file or directory name. 

MS_NOCHDIR           "Cannot change to the /var/spool/cron/atjobs directory.\n"
$ Do not translate /var/spool/cron/atjobs 

MS_NOREADAT          "Cannot read the /var/spool/cron/atjobs directory.\n"
$ Do not translate /var/spool/cron/atjobs 

MS_BADTAB            "The crontab file contains an error.\n"
$ Do not translate crontab. 

MS_IGNORED           "This entry has been ignored.\n"
$ get this message after MS_BADTAB 

MS_QLIMAX            "cron: The %c queue maximum run limit has been reached.\n"
$ %c is replaced a character 

MS_MAXRUN            "cron: The MAXRUN limit of %d processes has been reached.\n"
$ %d is replaced a number 

MS_ATLIMIT           "cron: The ATLIMIT of %1$d has been reached for user %2$d.\n"
$ %1$d and  %2$d are replaced by numbers 
$ an "at job" is a type of unix job.  "at" is a unix command 

MS_CRONLIMIT         "cron: The CRONLIMIT of %1$d has been reached for user %2$d.\n"
$ %1$d and  %2$d are replaced by nnumbers 

MS_NOFORK            "The fork system call failed.\n"
$ Do not translate fork 

MS_BADPID            "The %d PID is ignored because it is not expected.\n"
$ %d is replaced a number 

MS_FIFO1             "Cannot access the named pipe queue /var/adm/cron/FIFO.\n"

MS_READ              "read"
$ Do not translate 

MS_MSGALLOC          "Cannot allocate message buffer.\n"

MS_BADFMT            "A message to cron is in the wrong format.\n"

MS_ABORT             "cron failed: "
$ Do not add a newline.  Leave the space at the end. 
$ One of the previous messages will be printed after. 

MS_ABORTHDR          "** CRON FAILED **"
$ Do not translate cron 

MS_CRRESCHED         "The cron job is being rescheduled.\n"
$ Do not translate cron. 

MS_ATRESCHED         "The at job is being rescheduled.\n"
$ Do not translate at. 

MS_XMALLOC           "Cannot allocate %d bytes of memory.\n"
$ %d is replaced by a number 

MS_BADDATE           "at: The specified date is not in the correct format.\n"

MS_BADMINUTES        "at: The minutes field must be in the range 0 to 59.\n"

MS_ATBADSHELL        "at: Your login shell must be /bin/sh\n\
\tto use the at command.\n"
$ Do not translate, message is obsolete 
$ do not translate "at" or /bin/sh 

MS_WARNSHELL         "Warning: Commands will be run using the /bin/sh shell.\n"

MS_CANTCD            "Cannot change to the /var/spool/cron/atjobs directory.\n"
$ Do not translate /var/spool/cron/atjobs 

MS_CANTCHOWN         "Cannot make you the owner of the at job.\n"
$ Do not translate "at" 

MS_CANTCHMOD         "Cannot set the mode of the job file.\n"

MS_ATPRIVS           "at: insufficient privileges\n"

MS_CANTOPEN          "at: Cannot open a file in the /var/spool/cron/atjobs directory.\n"
$ Do not translate /var/spool/cron/atjobs 
$ Do not translate "at" 

MS_ERRNO             "at: errno is %d\n"
$ Do not translate 
$ This will never be seen 

MS_CANTLINK          "at: Cannot link the command file.\n"
$ Do not translate "at" 

MS_CANTCWD           "at: Cannot find the current working directory.\n"
$ obsolete message 

MS_INVALIDUSER       "Your user identification is not valid.\n"

MS_ATNOREADDIR       "Cannot read the /var/spool/cron/atjobs directory.\n"

MS_NOTALLOWED        "You are not authorized to use the at command.\n"
$ do not translate "at" 

MS_CRNOTALLOWED      "You are not authorized to use the cron command.\n"
$ Do not translate "cron" 

MS_NOTHING           "at: Command usage error; a parameter is missing.\n"

MS_PAST              "at: The specified time has already passed.\n"

MS_NOTOWNER          "at: You must be the owner of job %s.\n"
$ do not translate at   --    %s is replaced by the job name.  

MS_NONEXIST          "at: Job name %s does not exist.\n"
$ do not translate at   --    %s is replaced by the job name.  

MS_BADJNAME          "at: %s is not a valid job name.\n"
$ do not translate at   --    %s is replaced by the job name.  

MS_USEREQ            "User: %1$s\tJob: %2$s\tTime: %3$s"
$ %1$s is a user name   %2$s is the job name   %3$s is the time 
$ Do not add a  newline character 

MS_NOQUE             "at: Specify a queue name with the -q flag.\n"

MS_TOOLATE           "at: The specified time has already passed.\n"

MS_JOBAT             "Job %1$s will be run at %2$s.\n"
$ %1$s is the job number   %2$s is the time  
$ at is NOT the command name, it's just the normal preposition "at" 

MS_TIMEWARN          "at: This job may not be run at the specified time.\n"

MS_QFULL             "at: Cannot accept more jobs into the queue.\n"
$ Do not translate 'at' 

MS_NOPROTO           "at: There is no prototype file.\n"

MS_NOCRON            "The cron program may need to be started.\n"
$ Do not translate cron. 

MS_MSGQERROR         "at: Cannot open the message queue.\n"
$ do not translate at. 

MS_MSGSERROR         "at: Cannot write to the message queue.\n"
$ do not translate at. 

MS_BADOPEN           "Cannot open the cron file in the /var/spool/cron/crontabs directory.\n"
$ Do not translate cron 

MS_BADCREATE         "Cannot create the cron file in the /var/spool/cron/crontabs directory.\n"
$ Do not translate cron 

MS_LINERR            "A line of the crontab file:\n\
\t%1$s\n\
contains the following error:\n\
\t%2$s\n"
$ %1$s is replaced by a line from a file. 
$ %2$s is replaced by the message MS_EOLN or MS_OUTOFBOUND or MS_UNEXPECT 
$ Do not translate cron. 

MS_BTIME             "at: There is an error in the minute or hour specification.\n"

MS_BHOUR             "at: Specify an hour in the range 1 through 12.\n"

MS_BDATE             "at: There is an error in the month or the day specification.\n"

MS_BYEAR             "at: The specified year must be in the range 1970 through 2425.\n"

MS_WORD              "at: %s is not a recognized date or time specification.\n"

MS_BADQUE            "at: Bad queue specification.\n"

MS_CONFOPTS          "at: The specified flags cannot be used together.\n"

MS_INVALIDJOB        "at: %s is not a valid job name.\n"
$ %s is replaced by a job name 
$ Do not translate the command word "at" 

MS_MALLOC            "at: There is not enough memory available now.\n"

MS_ATCANTCD          "at: Cannot change to the /var/spool/cron/atjobs directory.\n"

MS_CRCANTCD          "crontab: Cannot change to the /var/spool/cron/atjobs directory.\n"

MS_NONUMBER          "at: Specify a number with the -l flag.\n"

MS_CRNOREADDIR       "crontab: Cannot read the /var/spool/cron/crontabs directory.\n"

MS_CRBADUSAGE        "Usage: crontab [-l|-r|-v|File]\n"

MS_ATERROR           "at: %s\n"
$ do not translate at 

MS_CRADMERROR        "cronadm: %s\n"
$ do not translate cronadm 

MS_SERROR            "syntax error\n"

MS_NOKSH             "Cannot find or access /usr/bin/ksh.\n"
$ do not translate /usr/bin/ksh 

MS_NOCSH             "Cannot find or access /usr/bin/csh.\n"
$ do not translate /usr/bin/csh 

MS_BADUNLINK         "crontab: The unlink function failed on the crontab file.\n"
$ do not translate unlink 

MS_BADATUNLINK       "at: The unlink function failed on the atjob file.\n"

MS_BADSCANDIR        "at: The scandir function failed on /var/spool/cron/atjobs.\n"

MS_BADCHOWN          "at: Cannot change the owner of the atjob file.\n"

MS_ATDELTD           "The %s at file is deleted.\n"
$ do not translate at 

MS_DELETE            "Delete %s?  Press y for yes: "

MS_NOFILE            "cronadm: Specify a job name or a user name.\n"

MS_CRONCNT           "There are %d files in the queue.\n"

MS_FILENM            "Crontab file: %s\n"

MS_SUBTIME           "Crontab file: %s\t Submission time: %s\n"

MS_CRDELETD          "Crontab file: %s deleted\n"

MS_CRABORT           "crontab: %s"

MS_AUDITER           "%1$s: The auditproc function failed with errno %2$d.\n"

MS_ATRMUSAGE         "Usage: atrm [-f|-i] [-] [job #|Username]\n"

MS_ATQUSAGE          "Usage: atq [-c|-n] [Username...]\n"

MS_CRADMUSAGE1       "Usage: cronadm cron [-l|-r|-v] [Username...]\n"

MS_CRADMUSAGE2       "               at [-l|-r] [Job Number|Username...]\n"


MS_YESUP             "YES\n"

MS_YESLOW            "yes\n"

MS_NO                "no\n"

MS_USAGE1            "Usage: at [-cklms] Time [Date]\n"
$ Do not translate the command "at" or the flags 
$ This message and the next 2 get printed together 

MS_USAGE2            "\tat -l [-q{a|b}]\n"
$ Do not translate the command "at" or the flags 

MS_USAGE3            "\tat -r JobNumber ...\n"
$ Do not translate the command at or the flags 

MS_BADPARM           "%s: Specify only one flag or file name.\n"

MS_ATRMDEL           "atrm: Only jobs belonging to user %s may be deleted.\n"
$ Do not translate atrm.

MS_CANTPEEK	     "you may only look at your jobs.\n"

MS_NOBLANK	     "Blank lines are not allowed.\n"

MS_INSUFFPRIV        "%s: insufficient privileges\n"
