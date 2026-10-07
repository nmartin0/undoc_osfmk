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

$quote "
$set MS_DF

USAGE	"usage: df [-ikn] [-t nfs|ufs|s5fs] [file | file_system ...]\n"
MSG1	"%-*.*s%s    used   avail capacity"
FSYS	"Filesystem"
KBYTES	"  kbytes"
BLKS	"512-blks"
IUSED	" iused   ifree  %%iused"
MOUNTED	"  Mounted on\n"
SCREWY	"df: %s: screwy fstab entry\n"
UNKDEV	"df: %s: mounted on unknown device\n"
CANTOP	"Cant's open %s\n"
READERR	"\nread error off = %ld\n"
CERRNO	"count = %d; errno = %d\n"
FTYPE	"df: %s: unknown file system type.\n"
