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
$ @(#)$RCSfile: mkdir.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:30:21 $
$
$
$quote "
$len 
$set MS_MKDIR

NOACCESS "%s: cannot access "
NOMAKE	""%s: cannot create "
NOFD	"%s: -d and -f options are mutually exclusive\n"
NOMULT	"%s: can't have multiple -%c options\n"
BADMODE "%s: incorrect syntax in mode argument\n"
USAGE1	"usage: create [-d | -f] [-npqtP] [-m mode] [-x prefix] name ...\n"
USAGE2	"usage: mkdir [-p] [-m mode] dirname ...\n"
USAGE3	"usage: mkfifo [-p] [-m mode] file ...\n"
