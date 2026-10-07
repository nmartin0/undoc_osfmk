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

$ @(#)$RCSfile: egrep.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:25:49 $
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
$ egrep.msg 1.7  com/cmd/scan,3.1,9013 3/4/90 17:33:15
$
$quote "         define quote character.
$len
$set MS_EGREP        Define initial set#
$ Do not translate egrep, %s.

SYNERR               "egrep: Command usage error.\n\
\tUsage: egrep [-hsbcilnv] [-e Expression] [-f File] [Strings] [File ...]\n"
$ $ Check Usage statement.
NOSPACE              "egrep: The regular expression is too large.\n"
FUNNY                "egrep: Cannot interpret the specified regular expression.\n"
CANTOPEN             "egrep: Cannot open %s.\n"
USGE                 "Usage: egrep [-hsbcilnv] [-e Expression] [-f File] [Strings] [File ...]\n"
$ $ Check Usage statement.
