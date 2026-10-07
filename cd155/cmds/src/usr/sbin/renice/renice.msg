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

$ @(#)$RCSfile: renice.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:14:12 $

$ 
$ COMPONENT_NAME: (CMDCNTL) system control commands
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
$ renice.msg	1.7  com/cmd/cntl,3.1,9021 3/11/90 12:16:06
$ renice.msg	4.2 02:01:03 7/13/90 SecureWare
$ 
$ messages for command renice
$ 
$quote "

$set MS_RENICE
$ Do not translate renice

USAGE           "Usage: %s Priority [ [ -p ] Pids ] [ [ -g ] Pgrps ] [ [ -u ] Users ]\n"
NEWPRI          "%d: old priority %d, new priority %d \n"
$ Only the words old priority and new priority can be translated.
$ Do not change their order.
UNK_USER        "%s: User %s is not known.\n"
$ Do not move the first %s from the beginning of the message. It is replaced
$ by the command name (renice)
$ The 2nd %s is replaced by a user name.
BADVALUE        "%s: The process or process group %s is not an integer.\n"
$ Do not move the first %s from the beginning of the message. It is replaced
$ by the command name (renice)
GETPRIO         "%s: %d: "
$ Do not translate
$ There should not be a trailing newline.
SETPRIO         "%s: %d: "
$ Do not translate
$ There should not be a trailing newline.
PRIV		"%s: insufficient privileges\n"
