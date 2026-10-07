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

$ @(#)$RCSfile: grep.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:25:55 $
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
$ grep.msg 1.7  com/cmd/scan,3.1,9013 3/4/90 17:33:54
$
$quote "         define quote character.
$len
$set MS_GREP        Define initial set#

PARG  "grep: Do not use the -p flag more than 20 times.\n"
USAGE "Usage: grep [-bcilnqsvw] [-e Pattern] [-p ParagraphSeparators]\n\
\t[Pattern] [File ...]\n"
$ $ Check the usage statement.
OPERR "grep: Cannot open %s.\n"
OVFLO "grep: The maximum paragraph length is 5,000 characters.\n"
REERR "grep: The specified pattern is not valid.\n"
STDINSTR	"(standard input)"
