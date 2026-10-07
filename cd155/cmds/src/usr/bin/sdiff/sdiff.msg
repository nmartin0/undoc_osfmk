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

$ @(#)$RCSfile: sdiff.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:38:37 $
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
$ sdiff.msg	1.5  com/cmd/files,3.1,9013 9/12/89 13:11:28
$
$quote "
$set MS_SDIFF

LLERR	"The line length %s is too short."
ILLARG	"Usage: sdiff [-l][-s][-o Output][-w Number] File1 File2\n"
USAGE	"Usage: sdiff [-l] [-s] [-o Output] [-w Number] File1 File2\n"
$ do not translate sdiff or the command flags
OPERR	"Cannot open file %s.\n"
OPERRT	"Cannot open the temporary file %s.\n"
OPERRO	"Cannot open the output file %s.\n"
NOFNDC	"%c is not a recognized subcommand for the -o flag.\n"
OPERRT1	"main: Cannot open the temporary file %s.\n"
ILLCMD	"%s is not a recognized subcommand for the -o flag.\n"
$ do not translate the flag names
CPPERR	"cpp: Cannot open file %s.\n"
NOFORK	"Cannot create another process.\n"
