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

$ @(#)$RCSfile: catman.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:04:50 $

$ 
$ 	COMPONENT_NAME: (CMDMAN) commands that allow users to read online
$ 	                         documentation
$ 
$ 	FUNCTIONS: none
$ 
$ 	ORIGINS: 27
$ 
$ 	IBM CONFIDENTIAL -- (IBM Confidential Restricted when
$ 	combined with the aggregated modules for this product)
$ 	OBJECT CODE ONLY SOURCE MATERIALS
$ 	(C) COPYRIGHT International Business Machines Corp. 1989
$ 	All Rights Reserved
$ 
$ 	US Government Users Restricted Rights - Use, duplication or
$ 	disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$ 
$ 	catman.msg\t1.5  com/cmd/man,3.1,9021 3/4/90 17:16:49
$ 

$quote "         define quote character.
$ messages for catman
$len
$set MS_CATMAN        Define initial set#


MISSPATH             "catman: -M option missing path\n"
$ Do not move the %s from its present position.

USAGE                "Usage: catman [-p] [-n] [-w] [-M path] [sections]\n"
MANDIR               "catman: %s"
$ Do not translate.

CDMANDIR             "cd %s\n"
$ Do not translate.

OPENDIR              "catman: opendir: %s"
$ Do not translate.

MKDIR                "mkdir %s\n"
$ Do not translate.

MKDIRCAT             "catman: mkdir: %s"
$ Do not translate.

CAT                  "catman: %s"
$ Do not translate.

NOTDIR               "catman: %s: not a directory\n"
$ Do not translate catman.

LNCAT                "ln -s %1$s %2$s\n"
$ Do not translate.

SYMLINK              "catman: symlink: %s"
$ Do not translate.

NROFF                "nroff -man %1$s > %2$s"
$ Do not translate.

BADMAN               "catman: nroff: %1$s: The exit status is %2$d\n"
$ Do not translate nroff, catman.

STATUS               "catman: %s: The exit statis is %d\n"
