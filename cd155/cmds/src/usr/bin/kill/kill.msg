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

$ @(#)$RCSfile: kill.msg,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 01:26:49 $
$
$ COMPONENT_NAME: (CMDCNTL) system control commands
$
$ FUNCTIONS:
$
$ ORIGINS: 27
$
$ IBM CONFIDENTIAL -- (IBM Confidential Restricted when
$ combined with the aggregated modules for this product)
$ OBJECT CODE ONLY SOURCE MATERIALS
$ (C) COPYRIGHT International Business Machines Corp. 1989, 1990
$ All Rights Reserved
$
$ US Government Users Restricted Rights - Use, duplication or
$ disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$
$ kill.msg	1.9  com/cmd/cntl,3.1,9013 3/11/90 12:15:12
$ kill.msg	4.1 23:50:11 7/12/90 SecureWare
$
$ messages for command kill
$

$set MS_KILL
$ Do not translate kill

$quote "

EPGROUP "Cannot kill the specified process group.\n"
EPDENIED "Permission denied.\n"
ESIGNAL "Specify a signal number less than 64.\n"
ENOPROC "The specified process does not exist.\n"
USAGE "Usage: kill [ -SignalNumber|-SignalName ] [ProcessID] ...\n"
$ The default signal number is -15 (software terminate).
USAGE1 "Usage: kill [ -s SignalName ] [ProcessID] ...\n"
USAGE2 "Usage: kill -l\n"
EKILL "kill: %d: %s\n"
$ Do not translate
BADNO   "The specified signal number or name is not valid.\n"
PRIV    "%s: insufficient privileges\n"
