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

$ @(#)$RCSfile: cat.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:20:01 $
$
$ COMPONENT_NAME: (CMDSCAN) commands that scan files
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
$ cat.msg 1.6  com/cmd/scan,3.1,9021 3/4/90 17:33:02
$
$quote "         define quote character.
$len
$set MS_CAT        Define initial set#

$ messages for command cat
$   GLOBAL COMMENT FOR TRANSLATORS
$       Do not translate cat -- this is the command name.
$       Do not translate %o %s %1$s %2$s etc.
$       These are output formats for file names, directory
$       names, permission codes, command names, and the like,
$       depending on their context within each message.
ESTATOUT             "cat: Cannot get status on standard output.\n"
EOPEN                "cat: Cannot open %s.\n"
ESTAT                "cat: Cannot get status on %s.\n"
$
$ %s is a file name.
$
EINISOUT             "cat: Cannot use %s as both input and output.\n"
$
$ %s is a file name.
$
ECLOSE               "cat: Cannot close the file.\n"
$
EOUTPUT              "cat: Cannot write to output.\n"
$
EUSAGE               "Usage: cat [-rsu] [-n[b]] [-v[et]] [-|File ...]\n"
$
$       Do not translate any of -rsu, -n, b, -v, et.
