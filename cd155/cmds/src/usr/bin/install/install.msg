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
$ (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
$ ALL RIGHTS RESERVED
$ 
$ 
$ OSF/1 Release 1.0.2

$ @(#)$RCSfile: install.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:26:17 $

$ 
$ COMPONENT_NAME: (CMDFILES) commands that manipulate files
$ 
$ FUNCTIONS:
$ 
$ ORIGINS: 27
$ 
$ IBM CONFIDENTIAL -- (IBM Confidential Restricted when
$ combined with the aggregated modules for this product)
$ OBJECT CODE ONLY SOURCE MATERIALS
$ (C) COPYRIGHT International Business Machines Corp. 1985, 1989
$ All Rights Reserved
$ 
$ US Government Users Restricted Rights - Use, duplication or
$ disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$ 
$ install.msg	1.5  com/cmd/files, 9104320c, bos320 10/15/90 18:17:10
$
$quote "
$
$set MS_INSTALL

1 "Usage: install [-c DirectoryA] [-f DirectoryB] [-i] [-M] [-m Mode] [-u Owner]\n\
\t       [-g Group] [-S] [-n DirectoryC] [-o] [-s] File [DirectoryX ...]\n"

2 "install: The %1$s and %2$s flags may not be used together.\n" 
$ The two flags %1$s and %2$s may not both appear in the same install command.

3 "install: File %1$s already exists in directory %2$s .\n"
$
4 "install: File %1$s is moved to %2$s .\n"
$ %2$s is the pathname of the destination.
5 "install: The command \"cp %1$s %2$s\" failed.\n"
$ This message reports that a cp command failed.
6 "File %1$s is installed as %2$s .\n"
$ File %1$s was installed as path name %2$s.
7 "The old file %1$s is moved to %2$s .\n"
$ File %1$s already existed. The old one was moved to path name %2$s.
8 "install: File %s was not found.\n"
$
9 "File %1$s is installed as %2$s by default!\n"
$ File %1$s was installed as path name %2$s.
10 "install: Specify permissions for -m option in absolute mode.\n"
$
