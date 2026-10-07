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

$ @(#)$RCSfile: writesrv.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:50:12 $
$
$ COMPONENT_NAME: (CMDCOMM) user to user communication
$
$ FUNCTIONS: 
$
$ ORIGINS: 27
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
$ writesrv.msg	1.6  com/cmd/comm/write,3.1,9021 3/17/90 15:35:49
$

$quote "         define quote character.
$set MS_writesrv        Define initial set#

M_MSG_0 "%s cannot create another process.\n"
$ %s is replaced by the system that cannot create the process.
E_MSG_1 "Accept"
$ will be followed by a perror message.
M_MSG_2 "writesrv: Reading the stream message.\n"
M_MSG_3 "writesrv: The service specified is not known.\n"
$ Do not translate "writesrv".
M_MSG_4 "Opening the stream socket"
$ will be followed by a perror message.
E_MSG_5 "Binding the stream socket"
$ will be followed by a perror message.
M_MSG_6 "Getting the socket name"
$ will be followed by a perror message.
M_MSG_12 "writesrv: Cannot open %s.\n"
$ %s is replaced by a file name.
M_MSG_13 "writesrv: Cannot continue.\n"
$ Do not translate "writesrv".
M_MSG_14 "writesrv: The specified tty line does not exist.\n"
$ tty is a unix term for a terminal
M_MSG_15 "writesrv: Write permission has been denied on the target tty.\n"
M_MSG_16 "writesrv: Cannot open the target tty line for writing.\n"
M_MSG_17 "Node     Service  Handle      Time\n"
$ These words are column heads for a table. Do not change spacing.
M_MSG_18 "\tTo reply, enter:  write -h %d,%s|%s|%s\n"
$ %d is replaced by a number string.
$ the three %s are replaced by the three possible replies.
M_MSG_19 "writesrv: Cannot communicate with the write command.\n"
E_MSG_20 "writesrv: accept"
$ will be followed by a perror message.
