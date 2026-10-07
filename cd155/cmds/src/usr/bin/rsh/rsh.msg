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
$ @(#)rsh.msg	1.3  com/sockcmd/simple,3.1,9011 1/3/90 11:34:28
$
$ COMPONENT_NAME: TCPIP rsh.msg
$
$ FUNCTIONS:
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
$ messages for rsh
$
$set MS_RSH

$quote "

RSH_CANT_EXEC	"rsh: can't exec %s.\n"
RSH_UNKNOWN_UID	"rsh: unknown user id.\n"
RSH_UNKSERVER	"rsh: shell/tcp: unknown service\n"
RSH_NOSTDERR	"rsh: can't establish stderr\n"
RSH_SETSOCK	"rsh: setsockopt: %s.\n"
RSH_FORK	"rsh: fork: %s.\n"
RSH_SELECT	"sh: select: %s.\n"
RSH_ERROR	"rsh: %s.\n"
RSH_USAGE	"usage: rsh [-ndx]%s[-l login] host [command]\n"
