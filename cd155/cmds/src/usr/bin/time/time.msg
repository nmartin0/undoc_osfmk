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

$ @(#)$RCSfile: time.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:42:27 $
$
$ COMPONENT_NAME: (CMDSTAT) status
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
$ time.msg	1.6  com/cmd/stat,3.1,9021 3/21/90 23:43:41

$quote "
$set MS_TIME
NOFORK "time: Cannot create another process.\n"
ABTERM "time: The process was stopped abnormally. Try again.\n"
$
$ For the following three values, make them the same length so
$ the output will align properly
$ For example, if System translates to 10 letters, make REAL and USER be
$ 10 letters (include blanks at the end.)
$
REAL "Real  "
USER "User  "
SYS  "System"

USAGE "Usage: time [CommandString]\n"
