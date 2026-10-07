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
$ @(#)sed.msg	1.6  com/cmd/edit,3.1,9013 3/2/90 11:52:36
$ /*
$  * COMPONENT_NAME: (CMDEDIT) sed.msg
$  *
$  * FUNCTIONS: none
$  *
$  * ORIGINS: 3, 10, 27
$  *
$  * This module contains IBM CONFIDENTIAL code. -- (IBM
$  * Confidential Restricted when combined with the aggregated
$  * modules for this product)
$  * OBJECT CODE ONLY SOURCE MATERIALS
$  * (C) COPYRIGHT International Business Machines Corp. 1984, 1989
$  * All Rights Reserved
$  *
$  * US Government Users Restricted Rights - Use, duplication or
$  * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$  *
$  */
$
$quote "
$set    MS_SED
$len 
$       Do not translate sed.
$       Do not translate %s, %c, %d, etc. These will be replaced by file names,
$       commands, numbers, etc.
AD0MSG       "sed: Cannot specify an address with the %s function.\n"
AD1MSG       "sed: Function %s allows only one address.\n"
APPNDCNT     "sed: The file was not appended after line %ld.\n"
BADCMND      "sed: %s is not a recognized function.\n"
CGMSG        "sed: Function %s cannot be parsed.\n"
CMNDCNT      "sed: There are too many commands for the %s function.\n"
CREATERR     "sed: Cannot create file %s.\n"
DUPLBL       "sed: There are more than one %s labels.\n"
FILECNT      "sed: The w function allows a maximum of ten files.\n"
FILEOPEN     "sed: Cannot open file %s.\n"
FRSTRE       "sed: The first regular expression cannot be null.\n"
$       The term "regular expression" is a Unix technical term.
HOLDSPC      "sed: The hold space work area is full.\n"
LABELCNT     "sed: There are too many labels in file %s.\n"
LEFTBRC      "sed: There are too many '{'.\n"
LINECNT      "sed: There are too many line numbers specified.\n"
LINELEN      "sed: The output line exceeds maximum length.\n"
LTLMSG       "sed: The label %s is greater than eight characters."
MALLOCMSG    "sed: Memory allocation failed.\n"
OPENERR      "sed: Cannot find or open file %s.\n"
PATTFIL      "sed: Cannot open pattern file %s.\n"
PATTSPC      "sed: The pattern space work area is full.\n"
READCNT      "sed: There are too many reads after line %ld.\n"
RGHTBRC      "sed: There are too many '}'.\n"
TMMSG        "sed: The %s function did not end the command line,\n\
\tor was not preceded by a blank character.\n"
TOOMANYCMDNS "sed: There are more than 99 commands in pattern file %s.\n"
UNDFNLBL     "sed: %s is not a defined label.\n"
UNKFLG       "sed: %c is not a valid flag.\n"
NOSCRIPT     "sed: No editing script was provided.\n"
