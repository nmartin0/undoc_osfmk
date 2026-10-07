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
$ @(#)ipcs.msg	5.1 17:38:49 8/15/90 SecureWare
$ @(#)ipcs.msg	1.1  com/cmd/ipc,3.1,9021 4/18/89 16:07:09
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

$quote "
$set MS_IPCS


USAGE    "usage:  ipcs [-abcmopqst]\n"

CHDR "T     ID     KEY      MODE       OWNER    GROUP"
CHDR2 "  CREATOR   CGROUP"
QOMSG " CBYTES  QNUM"
QBMSG " QBYTES"
QPMSG " LSPID LRPID"
QTMSG "  STIME    RTIME    CTIME "
QMSG "Message Queues"
MQNIS "Error accessing Message Queue facility.\n"
MOMSG " NATTCH"
MBMSG "  SEGSZ"
MPMSG "  CPID  LPID"
MTMSG "   ATIME    DTIME    CTIME "
MMSG "Shared Memory:\n"
NOSHMEM "Error accessing Shared Memory facility.\n"
SBMSG " NSEMS"
STMSG "   OTIME    CTIME "
SMSG "Semaphores:\n"
SFNIS "Error accessing Semaphore facility.\n"
NOENTRY " no-entry"

$set MS_IPCS_SEC

PRIV "ipcs: insufficient privileges\n"
MACINIT "ipcs: MAC initialization failed\n"
