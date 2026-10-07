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

$ @(#)$RCSfile: finger.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:24:55 $
$ 
$ COMPONENT_NAME: TCPIP finger.msg
$ 
$ FUNCTIONS: s
$ 
$ ORIGINS: 10  26  27
$ 
$ (C) COPYRIGHT International Business Machines Corp. 1985, 1989
$ All Rights Reserved
$ Licensed Materials - Property of IBM
$ 
$ US Government Users Restricted Rights - Use, duplication or
$ disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$ 
$ messages for finger
$ 

$set    MS_FINGER

$quote  "

USAGE		"Usage: finger [-bfhilmpqsw] [login1 [login2 ...] ]\n"
ERR_OPEN	"finger: error opening %s\n"
FWOPEN		"finger: %s open error\n"
FWREAD		"finger: %s read error\n"
NOLOG		 "No one logged on\n"
HEADER1	"Login       Name              TTY Idle    When            Office\n"
HEADER2		"Login    TTY Idle    When            Office\n"
HEADER3		"Login      TTY            When"
HEADER4		"             Idle"
SITEINFO	"\nSite Info: %s"
OFFICE		"\nOffice: %-.11s"
HOMEPHONE1	"\t\tHome phone: %s"
HOMEPHONE2	"\t\t\tHome phone: %s"
PROJECT		"Project: "
NOPLAN		"No Plan.\n"
PALN		"Plan:\n"
PNAME		"%-*.*s  "
QUICKPRINT1	"%c%-*s %-16.16s"
QUICKPRINT2	" %-*s %-16.16s"
NOTLOG		"          Not Logged In\n"
LOGIN1		"Login name: %-10s\t\t\tIn real life: ???\n"
LOGIN2		"Login name: %-10s"
MSGOFF		"	(messages off)	"
REALNAME	"In real life: %s"
PHONE		"\nPhone: %s"
DIR		"\nDirectory: %-25s"
SHELL		"\tShell: %-s"
SINCE1		"\nOn since %15.15s"
SINCE2		"\n   on %s"
IDLE		" Idle Time"
LASTLOGIN1	"\nLast login %10.10s, %4.4s on %s"
FROM		" from %s"
LASTLOGIN2	"\nLast login %16.16s on %s"
NEVER		"\nNever logged in."
STATTTY		"finger: Can't stat %s\n"
DAY		"%d days"
DAYHR1		"%d day %d hours"
DAYHR2		"%d day %d hour"
DAYHR3		"%d days %d hours"
DAYHR4		"%d days %d hour"
HOUR		"%d hours"
HRMIN1		"%d hour %d minutes"
HRMIN2		"%d hour %d minute"
HRMIN3		"%d hours %d minutes"
HRMIN4		"%d hours %d minute"
MINUTE		"%2d minutes"
SECOND		"%2d seconds"
MINSEC1		"%d minute %d seconds"
MINSEC2		"%d minute %d second"
MINSEC3		"%d minutes %d seconds"
MINSEC4		"%d minutes %d second"
UNKHOST		"unknown host: %s\n"
UNKSERVICE	"tcp/finger: unknown service\n"
SOCKET		"socket"
CONN		"connect"
