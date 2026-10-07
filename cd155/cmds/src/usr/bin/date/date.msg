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

$ @(#)$RCSfile: date.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:22:11 $
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
$ (C) COPYRIGHT International Business Machines Corp. 1989,1990
$ All Rights Reserved
$
$ US Government Users Restricted Rights - Use, duplication or
$ disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$
$ date.msg	1.6  com/cmd/stat,3.1,9021 3/21/90 23:42:22
$ date.msg	4.1 15:15:03 7/9/90 SecureWare

$
$ messages for command date
$

$quote "
$set MS_DATE


BADFORMAT "date: %c is not a recognized field descriptor.\n\
\tUsage: date [-u] [+FieldDescriptors]\n"

ECONV "Usage: date [-u] [+FieldDescriptors]\n"

BADPERM "date: You must be root to change the date.\n"
$ Do not translate root

UNK_SERVICE "date: The time service is not available on your system.\n"

NOPORTS "date: All ports are in use.\n"

WRONG_ACK "date: Wrong acknowledgement received from time server: %s\n"

CANTREACH "date: Cannot reach the time server; time set locally.\n"

USAGEMONTH "MM"
$ This must be 2 letters for the month
USAGEDAY "dd"
$ This must be 2 letters for the day
USAGEHOUR "hh"
$ This must be 2 letters for the hour
USAGEMINUTE "mm"
$ This must be 2 letters for the minutes

USAGEROOT "Usage: date [-nu] [%s.ss][yy] [+FieldDescriptors]\n"

USAGEROOT2 "Usage: date [-nu] [yy%s.ss] [+FieldDescriptors]\n"

USAGE "Usage: date [-u] [+FieldDescriptors]\n"
$ The system generates usage statements for the BADFORMAT and ECONV messages.
$ Two flags are added; -n -u.

EPRIV "date: insufficient privileges\n"
