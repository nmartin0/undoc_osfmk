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
$ @(#)$RCSfile: comsat.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:04:57 $
$
$ COMPONENT_NAME: CMDMAILX comsat.msg
$
$ FUNCTIONS:
$
$ ORIGINS: 10  26  27
$
$ (C) COPYRIGHT International Business Machines Corp. 1985, 1989
$ All Rights Reserved
$ Licensed Materials - Property of IBM
$
$ US Government Users Restricted Rights - Use, duplication or
$ disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$

$quote "         define quote character.

$len 
$set MS_comsat        Define initial set#

M_USAGE              "usage: comsat [-s] [-d directory]"
M_ECHDIR             "chdir: %s: %m"
M_ETMP               "open: /etc/utmp: %m"
M_EMALLOC            "malloc failed"
M_NEWMAIL            "%s\n\
\007New mail for %s@%.*s\007 has arrived:%s\n\
----%s\n"
M_MORE               "...more...%s\n"
M_GETSOCK            "getsockname"
M_COMSAT             "comsat"
SETDEBUG             "setsockopt (SO_DEBUG): %m"
