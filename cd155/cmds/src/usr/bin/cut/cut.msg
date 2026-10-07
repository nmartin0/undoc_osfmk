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

$ @(#)$RCSfile: cut.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:22:08 $
$
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
$ cut.msg	1.7  com/cmd/files,3.1,9021 3/19/90 13:09:46
$
$quote "
$set MS_CUT
$		GLOBAL COMMENT FOR TRANSLATORS
$ Do not translate cut -- this is the command name.
$ Do not translate %s.
$ The program replaces %s with a string of 
$ characters.   

NODELIMITER "cut: Provide a delimiting character with the -d flag.\n"
$ 
USAGE "Usage: cut {-cList|-fList [-dCharacter] [-s]} [File...]\n"
$ 
NOFIELDS "cut: The specified file must have fields.\n"
$
CANTOPEN "cut: Cannot open %s.\n"
$ 
LONGLINE "cut: A line in the file is too long or has mor than 512 fields.\n"
$ 
BCKSPC "cut: Cannot process a file containing multiple adjacent backspaces.\n"
$ 
BADLIST "cut: Specify correct parameters with -c or -f flags.\n"
$ 
