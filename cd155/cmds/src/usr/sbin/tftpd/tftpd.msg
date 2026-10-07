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
$ @(#)tftpd.msg	1.4  com/sockcmd/tftpd,3.1,8943 11/8/89 10:46:59
$
$ (C) COPYRIGHT International Business Machines Corp. 1985, 1989
$ All Rights Reserved
$ Licensed Materials - Property of IBM
$
$ US Government Users Restricted Rights - Use, duplication or
$ disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$
$ messages for tftpd
$

$set MS_TFTPD

$quote "

BAD_BUS         "bad bus open, rc = %d \n"
UNDEFERR        "Undefined error code"
BADFILE         "File not found"
ILLACESS        "Access violation"
DISKFULL        "Disk full or allocation exceeded"
ILLOPE          "Illegal TFTP operation"
BADID           "Unknown transfer ID"
FILEXIST        "File already exists"
BADUSER         "No such user"
MAXTIME         "timer: maxtimeout reached.\n"
SENDERR1        "tftpd: write: %m\n"
RECERR1         "tftpd: read: %m\n"
SENDERR2        "tftpd: write: %m\n"
RECERR2         "tftpd: read: %m\n"
LOGSND		"Sent %s to %s\n"
LOGRCV		"Recv %s from %s\n"
SETDEBUG	"setsockopt (SO_DEBUG): %m"
TFTPD_IOCTL	"ioctl(FIONBIO): %m\n"
TFTPD_RECVFROM	"recvfrom: %m\n"
TFTPD_FORK	"fork: %m\n"
TFTPD_SOCKET	"socket: %m\n"
TFTPD_BIND	"bind: %m\n"
TFTPD_CONNECT	"connect: %m\n"
TFTPD_NAK	"nak: %m\n"
