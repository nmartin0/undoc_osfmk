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
$ @(#)m4.msg	1.8  com/cmd/as/m4,3.1,9013 2/10/90 16:03:37
$
$ COMPONENT_NAME: (CMDAS) Assembler and Macroprocessor
$
$ FUNCTIONS:
$
$ ORIGINS: 3, 27
$
$ This module contains IBM CONFIDENTIAL code. -- (IBM
$ Confidential Restricted when combined with the aggregated
$ modules for this product)
$ OBJECT CODE ONLY SOURCE MATERIALS
$ (C) COPYRIGHT International Business Machines Corp. 1985, 1989
$ All Rights Reserved
$
$ US Government Users Restricted Rights - Use, duplication or
$ disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$
$quote "         define quote character.
$set MS_M4        Define initial set#
$
QEOF "A quote is not complete.\n\
\tCheck that the end quote character exists.\n"
CEOF "A comment is not complete.\n\
\tCheck that the end comment character exists.\n"
AEOF "A parameter list is not complete.\n\
\tCheck that the ) character exists.\n"
OFL "Cannot open the specified file.\n\
\tCheck path name and permissions.\n"
OST "There is not enough memory available now.\n\
\tTry again later or\n\
\tuse local problem reporting procedures.\n"
ST ",line %d: %s\n"
$ format for line number information displayed before error message
QLNB "The quote symbols cannot be more than %d bytes long.\n"
QLNC "The quote symbols cannot be more than %d characters long.\n"
BMN "A macro name cannot begin with a number and can\n\
\tcontain only alphabetic letters, numbers, and the _ character.\n"
MD "The macro name and definition cannot be identical.\n"
IE "A parameter to the eval macro is not correct.\n\
\tCheck the parameter for errors.\n"
$ Do not translate "eval" -- it is an m4 specific subcommand
IFLN "Do not use more than 9 levels of\n\
\tinclude and sinclude macros.\n"
$ Do not translate "include" or "sinclude" -- they are macro names
MTB "Do not specify more than %d bytes in a word.\n\
\tUse the -T flag to increase the size of the buffer.\n"
MTC "Do not specify more than %d characters in a word.\n\
\tUse the -T flag to increase the size of the buffer.\n"
MTI "A parameter list cannot contain more than %d items.\n\
\tUse the -S flag to increase the size of the buffer.\n"
PBMB "The push-back buffer cannot contain more than %d bytes.\n\
\tUse the -B flag to increase the size of the buffer.\n"
PBMC "The push-back buffer cannot contain more than %d characters.\n\
\tUse the -B flag to increase the size of the buffer.\n"
CMLB "A comment marker cannot be more than %d bytes long.\n"
CMLC "A comment marker cannot be more than %d characters long.\n"
AUGT "The parameters to the specified macro\n\
\tcannot contain more than %d bytes of text.\n"
MIA "%s is not a valid value for the -H flag.\n\
\tSpecify a numerical value greater than zero.\n"
BO "%s is not a recognized flag.\n\
\tUsage: m4 [-els] [-BNumber] [-HNumber] [-SNumber] [-TNumber]\n\
\t[[-DName[=Value]] [-UName] ...] [File ...]\n"
TR "Tracing (%1$d): %2$s"
$ informational message that tracing is turned on
