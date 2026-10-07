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
$ OSF/1 Release 1.0

$
$ @(#)unifdef.msg	1.3  com/cmd/prog/unifdef,3.1,8951 9/12/89 12:05:52
$
$ COMPONENT_NAME: (CMDPROG) Programming Utilities
$
$ FUNCTIONS: 
$
$ ORIGINS: 27
$
$ (C) COPYRIGHT International Business Machines Corp. 1985, 1989
$ All Rights Reserved
$ Licensed Materials - Property of IBM
$
$ US Government Users Restricted Rights - Use, duplication or
$ disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$
$ messages for unifdef
$

$set MS_UNIFDEF

$quote "

TOOMANY         "too many symbols.\n"
ILLOPTS         "unrecognized option: %s\n"
USAGE1          "Usage: %s [-l] [-t] [-c] [[-Dsym] [-Usym] [-idsym] [-iusym]]... [file]\n"
USAGE2          "    At least one arg from [-D -U -id -iu] is required\n"
ONEFILE         "can only do one file.\n"
LINEERR         "Error in %s line %d: %s.\n"
LINERRN         "Error in %s line %d: %s. "
DEPTH           "ifdef depth: %d\n"
NOSTR           ""
BADELSE         "Inappropriate else/elif"
BADENDIF        "Inappropriate endif"
BADEOF          "Premature EOF in ifdef"
BADCOMM         "Premature EOF in comment"
BADQUOTE        "Premature EOF in quoted character"
AA              ""
BADQSTR         "Premature EOF in quoted string"
