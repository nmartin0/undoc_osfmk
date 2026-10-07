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
$ (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
$ ALL RIGHTS RESERVED
$ 
$ 
$ OSF/1 Release 1.0.1
$
$ @(#)$RCSfile: diff3.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:47:01 $
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
$ diff3.msg	1.6  com/cmd/files/diff,3.1,9021 3/19/90 13:02:40
$
$quote "

$ Do not translate diff3.
$ messages for the diff3prog.c source file
$ messages for the diff3.sh shell script

$set 1

1 "diff3: Cannot find one of the specified files.\n"
2 "Usage: diff3 [-exEX3] File1 File2 File3\n"

$set MS_DIFF3

ARGCNT "diff3: Specify three existing files.\n"
CANTOPEN "diff3: Cannot open %s.\n"
$ %s is a file.
CHNGCNT "diff3: There are more than 200 different lines between the 3 files.\n"
LOGERR "diff3: The diff3 program logic cannot handle the specified files.\n"

UNKFLG "Usage: diff3 [-exEX3] File1 File2 File3\n"
