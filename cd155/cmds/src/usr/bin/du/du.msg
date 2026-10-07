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
$	"@(#)du.msg	1.5  com/cmd/fs/progs,3.1,9013 3/6/90 13:02:25"
$
$ COMPONENT_NAME: (CMDFS) commands that deal with the file system
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
$quote "
$		GLOBAL COMMENT FOR TRANSLATORS
$ Do not translate du -- this is the command name.
$ Do not translate %s.
$set MS_DU
$
USAGE "usage: du [-alrs] [name ...]\n"
$
CANTFIND "du: Cannot find the current directory.\n"
$
NAMETOOLONG "du: %s\nThe specified path name exceeded 255 bytes.\n"
$
$ DO NOT move the %s from where it is. It will be a very long name, and
$ will "wrap".
TOODEEP	"du: %s\nThe generated path name exceeded 255 bytes.\n"

NODOTDOT "du: Cannot change directory to ../%s \n"
$ Do not translate "..". This is a part of the path.
$
DU "du: "
$ Do not translate.
