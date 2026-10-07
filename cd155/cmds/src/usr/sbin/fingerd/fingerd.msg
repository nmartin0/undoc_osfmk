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

$ @(#)$RCSfile: fingerd.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:06:01 $
$ 
$ COMPONENT_NAME: TCPIP fingerd.msg
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
$ fingerd.msg       1.3  com/sockcmd/finger,3.1,9021 11/8/89 10:37:54
$ 
$ messages for fingerd
$ 

$set    MS_FINGERD

$quote  "

$ The name of the peer to which a connection will be made cannot be
$ ascertained. Getpeername returns an error message which replaces %m.
PEER         "getpeername"
PIPE		"pipe"
FORK		"fork"
FOPEN		"fopen"
ERRMSG		"fingerd: %s: %s\r\n"
SETDEBUG        "setsockopt (SO_DEBUG): %m"
