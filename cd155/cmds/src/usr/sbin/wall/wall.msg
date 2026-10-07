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
$ @(#)$RCSfile: wall.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:18:50 $
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
$ wall.msg 1.10  com/cmd/comm,3.1,9021 3/17/90 17:42:50
$ 
$set MS_WALL
$quote "

BRODCAST "\r\n\007\007Broadcast Message from %s@%s (%.*s) at %d:%02d ...\r\n\n"
$ %s is the user who is sending the message.
$ %s is the workstation of the user sending the message.
$ %.*s is the tty line of the sending user.
$ %d:$02d is the HH:MM time the message was sent
TOOLONG "wall: The message cannot be more than %d characters long."
$ %d is the message character limit.
TRYAGAIN "wall: Cannot create another process at this time.\n"
AUTH	"%s: need sysadmin authorization\n"
PRIV	"%s: insufficient privileges\n"
