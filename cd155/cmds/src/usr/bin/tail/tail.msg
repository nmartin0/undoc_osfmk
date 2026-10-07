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

$ @(#)$RCSfile: tail.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:40:43 $
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
$ tail.msg	1.8  com/cmd/scan,3.1,9021 3/4/90 17:35:33
$
$quote "
$set MS_TAIL

USAGE                "Usage: %s [+Number|-Number][l|b|c|k|m] [-f] [File]\n"
$       %s is the command name.
TOOMUCH              "%s: There is not enough memory available now.\n"
$       %s is the command name. It MUST stay where it is.
FSHRUNK              "%s: The size of the file has changed during the run of tail.\n"
$       %s is the command name. It MUST stay where it is.
