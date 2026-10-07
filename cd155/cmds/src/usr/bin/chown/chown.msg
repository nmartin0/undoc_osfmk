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

$ @(#)$RCSfile: chown.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:20:22 $
$
$ COMPONENT_NAME:  (CMDSDAC) security: access control
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
$ chown.msg	1.9  com/cmd/oper,3.1,9013 2/26/90 13:37:03
$ chown.msg	4.1 23:48:53 7/12/90 SecureWare

$ messages for chown command

$set MS_CHOWN
$quote "

USAGE	  "usage: %s -fR {username|uid}[.{groupname|gid}] file ...\n"
NOBACK    "can not change directory back to %s"
BADUID    "%s: unknown username %s\n"
BADGID    "%s: unknown groupname %s\n"
PRIV      "%s: insufficient privileges\n"
