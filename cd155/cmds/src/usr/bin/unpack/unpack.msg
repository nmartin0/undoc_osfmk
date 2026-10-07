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
$ @(#) $RCSfile: unpack.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:45:05 $
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
$ unpack.msg	1.5  com/cmd/files,3.1,9013 12/1/89 15:03:23
$
$
$set MS_UNPACK
$		GLOBAL COMMENT FOR TRANSLATORS
$ Do not translate unpack -- this is the command name.
$ The program replaces %o,%1$o,etc with an octal number.
$ A trailing \n is inserted by the unpack command, so don't 
$ put a newline into the messages.
$quote " 
$ 
READERR ".z: There is a read error."
$ 
NOTPCKD ".z: The specified file is not in packed format."
$ 
RDERR1 ".z: There is a read error."
$ 
UNPCKERR ".z: There is an unpacking error."
$ 
WRITERR ": There is a write error."
$ 
NMLENERR ": The file name is too long."
$ 
CANTOPNZ ".z: Cannot open the specified file."
$ 
FILEXST ": A file by this name already exists."
$ 
LINKERR ".z: Warning: The file has links."
$
CREATERR ": The file cannot be created."
$ 
CHMODERR "unpack: Cannot change the mode to %o."
$ 
UNPCKD ": The file is now unpacked."
$ 
ZRDERR ".z: An error occurred while reading the specified file."
$ 
WRITERR1 ": A write error has occurred."
