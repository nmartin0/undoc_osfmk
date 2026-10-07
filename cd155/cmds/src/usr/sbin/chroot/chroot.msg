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
 
$ @(#)chroot.msg	5.1 15:41:43 8/16/90 SecureWare
$
$ (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
$ ALL RIGHTS RESERVED
$
$
$ OSF/1 Release 1.0.2

$
$ RESTRICTED RIGHTS LEGEND
$ Use, Duplication or Disclosure by the Government is subject to
$ restrictions as set forth in paragraph (b)(3)(B) of the rights in
$ Technical Data and Computer Software clause in DAR 7-104.9(a).
$ 
0 	"@(#)chroot.msg	1.3  com/cmd/fs/progs,3.1,8943 10/3/89 11:09:09"
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
$set MS_CHROOT
$		GLOBAL COMMENT FOR TRANSLATORS
$ Do not translate chroot -- this is the command name.
$ Do not translate %o %s  %1$s %2$s etc.
$ These are output formats for file names, directory
$ names, permission codes, command names, and the like,
$ depending on their context within each message. 
USAGE "Usage: chroot Directory Command Parameter ...\n\
\tChanges the root directory of a command.\n"
CANTCHROOT "chroot: Cannot move to the specified root directory.\n\
\tCheck path name and permissions.\n"
$ chroot can change a "command" to a new root but it
$ cannot change a directory to a new root.
NOTFOUND "chroot: %s command is not found.\n\
\tCheck path name and permissions.\n"
$ do not translate chroot. Do not translate %s.
AUTH     "%s: need chroot authorization\n"
PRIV     "%s: insufficient privileges\n"
