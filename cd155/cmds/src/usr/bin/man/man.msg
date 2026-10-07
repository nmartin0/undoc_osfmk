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

$ 	@(#)man.msg\t1.6  com/cmd/man,3.1,9021 3/4/90 17:25:23
$ 
$ 	COMPONENT_NAME: (CMDMAN) commands that allow users to read online
$ 	                         documentation
$ 
$ 	FUNCTIONS: none
$ 
$ 	ORIGINS: 27
$ 
$ 	IBM CONFIDENTIAL -- (IBM Confidential Restricted when
$ 	combined with the aggregated modules for this product)
$ 	OBJECT CODE ONLY SOURCE MATERIALS
$ 	(C) COPYRIGHT International Business Machines Corp. 1989
$ 	All Rights Reserved
$ 
$ 	US Government Users Restricted Rights - Use, duplication or
$ 	disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$ 	[90/06/19  21:17:17  lehotsky]
$ 

$
$quote "         define quote character.
$len
$set MS_man        Define initial set#
$ $error numbers will be 3xx

$ Do not translate man, %s, etc.

M_MSG_0              "man: %s: The path is missing.\n"
M_MSG_1              "man: The section is missing.\n"
M_MSG_2              "man: %s: The path is missing.\n"
M_MSG_3              "There is not an entry for %s.\n"
M_MSG_4              "There is not an entry for %1$s in section %2$c.\n"
M_MSG_5              "Reformatting page. Please wait ..."
M_MSG_6              "man: The program has ended abnormally.\n"
M_MSG_7              " completed\n"
M_MSG_8              "man: A parameter is missing.\n"
M_MSG_9              "%s: Cannot find matching entry.\n"
M_MSG_10             "man: A parameter is missing.\n"
M_MSG_11             "man: %s is not found.\n"
M_MSG_12             "Usage: %s [-] [-MPath] "
$ This and the rest of the messages make up the usage statement.
$ Ensure that it matches your publications.
M_MSG_13             "\t[Section] Name ...\n"
M_MSG_14             "\t%s -k Keyword ... \n"
$ Do not translate -k
M_MSG_15             "\t%s -f File ... \n"
$ Do not translate -f
M_MSG_16             "Usage: %s "
M_MSG_17             "[-M path] keyword ...\n"
M_MSG_18	     "man: Nroff/troff must be installed in order\n\
to use formatted man pages.\n"
