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

$ @(#)$RCSfile: cmp.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:20:37 $
$
$ COMPONENT_NAME: (CMDFILES) commands that manipulate files
$
$ FUNCTIONS:
$
$ ORIGINS: 27
$
$ IBM CONFIDENTIAL -- (IBM Confidential Restricted when
$ combined with the aggregated modules for this product)
$ OBJECT CODE ONLY SOURCE MATERIALS
$ (C) COPYRIGHT International Business Machines Corp. 1985, 1989
$ All Rights Reserved
$
$ US Government Users Restricted Rights - Use, duplication or
$ disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$
$ cmp.msg	1.9  com/cmd/files,3.1,9021 3/16/90 14:34:01
$
$quote "

$set MS_CMP
$ Do not translate cmp

DIFF "%s %s differ: byte %d, line %d\n"
$ DO NOT TRANSLATE
$ This is part of the output of the diff command. The standards dictate
$ that it be in precisely this format. Customers will expect it this way.
USG "Usage: cmp [-l|-s] File1 File2\n"
NOTOPEN "cmp: Cannot find or open %s.\n"
EOFMSG "cmp: EOF on %s\n"
$ DO NOT TRANSLATE
$ This is part of the output of the diff command. The standards dictate
$ that it be in precisely this format. Customers will expect it this way.
