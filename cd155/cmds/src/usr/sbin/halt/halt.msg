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
$ RESTRICTED RIGHTS LEGEND
$ Use, Duplication or Disclosure by the Government is subject to
$ restrictions as set forth in paragraph (b)(3)(B) of the rights in
$ Technical Data and Computer Software clause in DAR 7-104.9(a).
$ 
$ @(#)halt.msg	1.3  com/cmd/oper,3.1,8943 9/12/89 13:21:45
$
$ COMPONENT_NAME: (CMDOPER) commands needed for basic system needs
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

$
$ messages for halt
$

$set MS_HALT

$quote "

NO_ROOT "NOT super-user\n"
USAGE	"usage: halt [-y][-q][-l][-n]\n"
DIALUP	"halt: dangerous on a dialup; use ``halt -y'' if you are really sure\n"
IDLE	"halt: can't idle init\n"
LOGHLT	"halted by %s"
CAUTION	"CAUTION: some process(es) wouldn't die\n"
HALTOVER	"....Halt completed....\n"

$set MS_HALT_SEC

AUTH	"%s: need sysadmin authorization\n"
PRIV	"%s: insufficient privileges\n"
