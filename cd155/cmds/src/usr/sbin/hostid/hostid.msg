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
$ @(#)$RCSfile: hostid.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:07:17 $
$
$ COMPONENT_NAME: TCPIP hostid.msg
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
$
$ messages for hostid
$ The range of message numbers assigned to this file is 821
$ hostid.msg	1.4  com/sockcmd/util,3.1,9021 3/5/90 17:08:43
$
$len 
$quote "         define quote character.
$set MS_HOSTID        Define initial set#

SYS_ATTR              "%s: insufficient privileges\n"
$  1	This message is printed when hostid program is ran
$  2	by user with insufficient privileges.
SYS_ADM              "%s: need sysadmin authorization\n"
$  1	This message is printed when hostid program is ran
$  2	by user other then system administrator.
USAGE                "Usage: %s [HexNumber or InternetAddress or HostName]\n"
$  2    This message is printed when the hostid program is not 
$  2    used correctly. 
SETHOST              "hostid: sethostid.\n"
