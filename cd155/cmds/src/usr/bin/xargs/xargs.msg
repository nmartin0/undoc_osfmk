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

$ @(#)$RCSfile: xargs.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:50:16 $
$
$
$ COMPONENT_NAME: (CMDSH) Bourne shell and related commands
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
$set MS_XARGS

LINECNT "%s is not a correct value for the number of input lines.\n"
$ The %s will be a command line argument like "-l4" or something similar.
$ Do not translate -l.
LINESIZ "Command line size cannot be more than 470; %s is not valid.\n"
$ The %s will be a command line argument
ARGCNT "%s is not valid. Specify a positive integer after the -n.\n"
$ The %s will be a command line argument
TTYREAD "Cannot find or open /dev/tty for prompting.\n"
$ "/dev/tty" is a file name and shouldn't be translated
UNKWNOPT "The %s flag is not valid.\n\
\tUsage: xargs [-p|-t] [-nNumber] [-l[Number]] [-sSize] [-x]\n\
\t       [-e[EndOfFileString]] [-i[ReplacementString]] [CommandString]\n"
ARGSIZ "Cannot replace more than 5 instances of %s.\n"
$ The %s will be {} or whatever follows the -i flag on the command line
NOEXEC "The %s command was not found or could not be run.\n"
ARGLIST "A command line size exceeds 470 or the specified limit.\n"
SNGLARG "The size of a single parameter exceeded the command line limit.\n"
MSNGQUOT "An end quote is missing from the string: %s.\n"
$ %s is replaced by a string of varying length. You may wish to put
$ it on a separate line.
MAXSARGSIZ "A parameter constructed by replacing %s exceeded 255 bytes.\n"
$ The %s will be {} or whatever follows the -i flag on the command line

OVRFLOW "A command line constructed by replacement exceeded 512 bytes.\n"
QUESTION        "?..."
