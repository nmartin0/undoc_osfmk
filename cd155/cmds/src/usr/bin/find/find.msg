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
$ @(#) $RCSfile: find.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:24:52 $
$

$
$	 COMPONENT_NAME: (CMDSCAN) commands that scan files
$
$	 FUNCTIONS:
$
$	 ORIGINS: 27
$
$	 IBM CONFIDENTIAL -- (IBM Confidential Restricted when
$	 combined with the aggregated modules for this product)
$	 OBJECT CODE ONLY SOURCE MATERIALS
$	 (C) COPYRIGHT International Business Machines Corp. 1989
$	 All Rights Reserved
$
$	 US Government Users Restricted Rights - Use, duplication or
$	 disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$	[90/05/18  09:41:11  mbrown]
$
$	 find.msg\t1.8  com/cmd/scan,3.1,9013 3/11/90 12:23:40
$
$quote "         define quote character.
$len
$set MS_FIND        Define initial set#
$  Do not translate find.
$  Do not translate %s, %d, etc. These will be replaced by files, numbers etc.
$

BADPWD               "find: Cannot get the name of the current directory.\n"
$       Do not translate pwd.
USAGE                "Usage: find path-list expression-list\n"
$ $ Check usage statement.
PARSERR              "find: There is a command usage error.\n"
CONJ                 "find: There is a missing conjunction.\n"
BADSTART             "find: The starting directory is not valid.\n"
BADSTAT              "find: The status on %s is not valid.\n"
OPFOP                "find: An operand follows another operand.\n"
$ NOTE
$ NOTE: the OPFOP message will never happen so don't bother translating
$ NOTE
NOUSER               "find: The user name does not exist.\n"
NOGRP                "find: The group name does not exist.\n"
NOCREATE             "find: Cannot create file %s.\n"
NOACCES              "find: Cannot access file %s.\n"
BADNODE              "find: %s is not a valid node id.\n"
BADPERM              "find: an octal number is required after %s.\n"
BADOPTION            "find: %s is not a valid option.\n"
INCSTATE             "find: An expression term lacks a required parameter.\n"
BADFSTYPE            "find: %s is not a valid filesystem type.\n"
NOCOPY               "find: Cannot copy %s.\n"
OKPROMT              "%1$s %2$s (%3$s) "
$ Do not translate this message.
NOREAD               "find: Cannot read file %s.\n"
NOCHDIR              "find: Cannot change directory to %s.\n"
NOOPEN               "find: Cannot open file %s.\n"
BADPATHLEN           "find: The pathname cannot be longer than 512 bytes.\n"
BADTREE              "find: Cannot change to the parent directory.\n"
BADDIR               "find: %s is not a valid directory.\n"
NOSTAT               "find: Cannot get status on %s.\n"
WRTOUT               "find: Cannot write output.\n"
READIN               "find: Cannot read input.\n"
ASKDEV               "To continue, enter the device or file name.\n"
NOGOOD               "find: Cannot open the specified device.\n"
