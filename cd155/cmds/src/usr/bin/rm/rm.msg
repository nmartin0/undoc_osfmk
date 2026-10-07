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

$ @(#)$RCSfile: rm.msg,v $ $Revision: 1.4 $ (OSF) $Date: 1994/11/19 01:36:58 $
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
$ rm.msg	1.10  com/cmd/fs/progs,3.1,9021 5/14/90 14:42:27
$
$quote "
$set MS_RM
$		GLOBAL COMMENT FOR TRANSLATORS
$ Do not translate rm -- this is the command name.
$ Do not translate %o %s  %1$s %2$s etc.
$ These are output formats for file names, directory
$ names, permission codes, command names, and the like,
$ depending on their context within each message. 
CANTREMOVE "rm: Cannot remove the parent directory.\n"
$ Do not translate rm. 
NOEXIST "rm: File %s does not exist.\n"
$ Do not translate rm. Do not translate %s.
$ %s is a file name.
DIRECTORY "rm: Remove files in directory %s? "
$ Do not translate rm. Do not translate %s.
$ %s is a directory name.
$ Type y for a yes response. Any other key on the keyboard 
$ automatically gives the default response which is "no".
CANTREAD "rm: Cannot read %s.\n"
$ Do not translate rm. Do not translate %s.
ISDIRECT "rm: cannot remove directory %s\n"
$ Do not translate rm. Do not translate %s.
$ %s is a directory name.
OVERRIDE "rm: Do you wish to override protection setting %1$o for %2$s? "
$ Type y for a yes response. Any other key on the keyboard 
$ automatically gives the default response which is "no".
$ Do not translate rm. Do not translate %1$o  %2$s.
$ %1$o is a number.  %2$s is a file name or directory name.
NOTPFS "rm: %s is not a valid PFS file, remove anyway? "
$ Type y for a yes response. Any other key on the keyboard 
$ automatically gives the default response which is "no".
$ Do not translate rm. Do not translate %s.
$ %s is a file name.
NOSDIR "rm: %s: PFS stripe data not available, remove anyway? "),
$ Type y for a yes response. Any other key on the keyboard 
$ automatically gives the default response which is "no".
$ Do not translate rm. Do not translate %s.
$ %s is a file name.
NOSDIRPERM "rm:   Warning: unable to remove all PFS stripe date for %s.\n"
$ Do not translate rm. Do not translate %s.
$ %s is a file name.
NOTREMOVED "rm: Cannot remove %s.\n"
$ Do not translate rm. Do not translate %s.
$ %s is a file name.
WHICHPROG "%s: Specify rm or rmdir command.\n"
$ do not translate rm or rmdir %s. %s is a file name or directory
$ name.
DELETE_MSG "rm: Remove %1$s? Press %2$s for yes. "
$ Type y for a yes response. Any other key on the keyboard 
$ automatically gives the default response which is "no".
$ Do not translate rm. Do not translate %s.
$ %1$s is a file name.
$ %2$s is a string from an environment variable YESSTR.  
YES_MSG	"yes\n"
NO_MSG	"no\n"
DIRECTORY_MSG "%s: Cannot remove directory.\n"
$ Do not translate %s. %s is a directory name.
PERM_MSG "rm: Cannot remove %s.\n"
USAGE "Usage: rm [-fir] [-] File ...\n"
RMDIR "Usage: rmdir Directory ...\n"

REMOVEDDIR "rm: removing directory %s\n"
FILEREMOVED "rm: removing %s\n"
$ Do not translate %s, or rm.
REMPROMPT "rm: Remove %s? "
