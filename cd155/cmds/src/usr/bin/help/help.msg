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

$ @(#)$RCSfile: help.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:26:04 $

$
$ COMPONENT_NAME: (CMDMAN) commands that allow users to read online
$                          documentation
$
$ FUNCTIONS: none
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
$ 	help.msg\t1.3  com/cmd/man,3.1,9021 3/4/90 17:17:05
$

$
$quote "         define quote character.
$ messages for help
$ note: translation of this message file is limited within one screen.
$len
$set MS_HELP        Define initial set#
$ Leave spaces in message.

HELP                 "\n\
Some basic Commands are:\n\
\n\
    man -k keyword\t- lists commands relevant to a keyword \n\
    man command\t\t- prints out the manual pages for a command \n\
    cat\t\t\t- concatenates files (and just prints them out) \n\
    vi\t\t\t- text editor \n\
    finger\t\t- user information lookup program \n\
    ls\t\t\t- lists contents of directory \n\
    mail\t\t- sends and receives mail \n\
    passwd\t\t- changes login password \n\
    sccshelp\t\t- views information on the Source Code Control System \n\
    tset\t\t- sets terminal modes \n\
    who\t\t\t- who is on the system \n\
    write\t\t- writes to another user \n\
\n\
To find programs about mail, use the command: \t\tman -k mail \n\
and print out the man command documentation via:\tman mail \n\
You can log out of the system by typing: \t\texit\n\
\n"
