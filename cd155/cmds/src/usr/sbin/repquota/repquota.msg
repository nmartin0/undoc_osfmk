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

$ Messages for the edquota command
$quote "
$set MS_REPQUOTA

NOFSTAB		"%s not found in fstab\n"
USE1		"repquota [-v] [-g] [-u] -a"
USE2		"repquota [-v] [-g] [-u] filesys ..."
NOQUOTA		"*** Warning: Quotas are not compiled into this kernel\n"
REPORT		"*** Report for %s quotas on %s (%s)\n"
LIMITS		"                        Block limits               File limits\n"
MSGHD		"User            used    soft    hard  grace    used  soft  hard  grace\n"
DAYS		"%ddays"

$set MS_REPQUOTA_SEC

AUTH		"%s: need sysadmin authorization\n"
PRIV		"%s: insufficient privileges\n"
