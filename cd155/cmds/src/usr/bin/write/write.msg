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

$ @(#)$RCSfile: write.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:50:08 $
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
$ write.msg 1.8  com/cmd/comm/write,3.1,9021 3/17/90 15:31:41
$
$quote "         define quote character.

$set MS_write        Define initial set#
M_MSG_1 "Message from %1$s on %2$s (%3$s) [%4$s] ...\n"
$ first %s will be replaced by user id
$ second %s will be replaced by tty
$ third %s will be replaced with hostname
$ fourth %s will be replaced with the current date and time.
M_MSG_2 "write: Cannot find your user name or user number.\n"
M_MSG_3 "write: Cannot determine your tty line.\n"
$ The tty is the identification of the user's terminal.
M_MSG_4 "write: Cannot determine your host name."
M_MSG_5 "write: Your reply must be %1$s, %2$s, or %3$s.\n"
M_MSG_8 "write: Cannot open %s.\n"
M_MSG_9 "write: Cannot continue.\n"
M_MSG_10 "write: %s is not logged on.\n"
M_MSG_11 "%s is logged on at more than one place.\n"
M_MSG_12 "You are connected to %s.\n"
M_MSG_13 "Other locations are:\n"
M_MSG_15 "%1$s on %2$s is logged on at more than one place.\n"
M_MSG_16 "You are connected to %s.\n"
M_MSG_17 "Other locations are:\n"
M_MSG_19 "write: There is no such tty.\n"
M_MSG_21 "write: Permission denied.\n"
M_MSG_22 "write: Cannot create another process."
$ will be followed by a perror message
M_MSG_24 "write: This is not a known service.\n"
E_MSG_25 "write: Cannot make the correct connections."
$ will be followed by a perror message
M_MSG_26 "write: This is not a known host.\n"
E_MSG_27 "write: Cannot make the correct connection."
$ will be followed by a perror message
M_MSG_28 "write: %1$s is not logged on at %2$s.\n"
M_MSG_29 "write: There is no such tty on the remote host.\n"
M_MSG_30 "write: Permission denied.\n"
M_MSG_31 "write: Cannot open %1$s on %2$s.\n"
$ utmp file not found
M_MSG_32 "write: There is not enough memory available now.\n"
M_MSG_33 "write: The -h value on %s is not correct.\n"
M_MSG_34 "write: Could not send a reply.\n"
M_MSG_35 "Usage: write [-r|-q|-h Handle,%s|%s|%s] [-n Node] UserName[@Host] [Line]\n"
$ The three %s will be replaced with the three possible replies.
M_MSG_36 "write: Cannot get a reply.\n"
M_MSG_37 "write: Cannot communicate with the writesrv.\n"
$ do not translate writsrv it is the name of the daemon than has to be running
M_MSG_38 "write: malloc"
$ will be followed by a perror message
M_MSG_39 "<EOT>\n"
$ EOT stands for end of transmission.
M_MSG_40 "write: %s is not running.\n"
M_MSG_41 "write: The writesrv cannot provide this service now.\n"
M_MSG_42 "write: The writesrv is shutting down.\n"
UNKNOWN "UNKNOWN"
$ unknown tty, will in the place of the second %s in M_MSG_1
