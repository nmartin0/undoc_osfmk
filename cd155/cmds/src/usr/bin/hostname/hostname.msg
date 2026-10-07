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
$ @(#)$RCSfile: hostname.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:26:06 $
$
$ COMPONENT_NAME: CMDNET hostname.msg
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
$set MS_HOSTNAME        Define initial set#

AUD_SYS              "hostname: The routine auditproc failed.  The error is %m.\n"
$  This message is printed when there is an error in the audit routine.  
USAGE                "Usage: %s [-s] [Host].\n"
SETHOST              "hostname: The sethostname system call failed.\n"
GETHOSTNAME          "hostname: The gethostname system call failed.\n"
