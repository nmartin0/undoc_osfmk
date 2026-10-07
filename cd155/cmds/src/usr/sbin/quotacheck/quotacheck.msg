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
$ Messages for the quota command
$quote "
$set MS_QUOTACHECK

NOOPEN		"Can't open "
NOTFOUND	"%s not found in %s\n"
CREATE		"Creating quota file %s\n"
USE1		"Usage:\n\t%s\n\t%s\n"
USE2		"quotacheck [-g] [-u] [-v] -a"
USE3		"quotacheck [-g] [-u] [-v] filesys ..."
NOMEM		"out of memory for quota structures\n"
PRIV		"%s: insufficient privileges\n"
CHK		"*** Checking "
AND		" and "
QUOTAS		" quotas for %s (%s)\n"
WARN1		"*** Warning: %s\n"
WARN2		"Quotas are not compiled into this kernel"
FIX		"%-8s fixed:"
INO		"\tinodes %d -> %d"
BLK		"\tblocks %d -> %d"
NOMEMU		"out of memory for fileusage structures\n"
BADINO		"bad inode number %d to nextinode\n"
NOALLOC		"Cannot allocate space for inode buffer\n"

$set MS_QUOTACHECK_SEC

AUTH		"%s: need sysadmin authorization\n"
