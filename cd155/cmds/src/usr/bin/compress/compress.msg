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
$ @(#)compress.msg	1.4  com/cmd/files,3.1,9013 3/12/90 15:36:00
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
$
$ messages for compress
$
$set MS_COMPRESS
$		GLOBAL COMMENT FOR TRANSLATORS
$ Do not translate compress -- this is the command name.
$ Do not translate %o %s %1$s %2$s etc.
$ These are output formats for file names, directory
$ names, permission codes, command names, and the like,
$ depending on their context within each message. 
$ The program replaces %o,%1$o,etc with an octal number.
$ The program replaces %d,%1$d,etc with a decimal number.
$ the program replaces %.2f with a fixed-point number.
$ The program replaces %.1f%% with a percentage.
$ The program replaces %s,%1$s,etc with a string of 
$ characters.  

$quote "

CPUSAGE "Usage: compress [-cfv] [-b Bits] [File ...]\n"
UNCPUSAGE "Usage: %s [-cfv] [File ...]\n"
ZCATUSAGE "Usage: %s [File ...]\n"
$
MISSMAXBT "compress: Specify a number of bits with the -b flag.\n"
$
UNKFLG "The %c flag is not recognized.\n"
$
NOTCOMP "The %s file is not compressed.\n"
$ Do not translate uncompress.
$
BITSWRONG "File %1$s was compressed with %2$d bits. Only %3$d bits are allowed.\n"
$
ALREADYZ "compress: %s already has a .Z suffix. It is not changed.\n"
$ Do not translate .Z
$ %s is replaced with a file name.
$
NAME2LONG "compress: File name %s is too long to add the .Z suffix.\n"
$ Do not translate .Z
$ %s is replaced with a file name.
$
ALRDYEXST ""
$ This is a null line. Do not translate.
$
WSHOVRWRIT "Do you wish to write over the existing file %s? "
$ Do not put a new line at the end. Leave a blank at the end.
$
NOTOVRWRIT "\tThe file is not written over.\n"
$
INNOTCOMP "The standard input is not compressed.\n"
$
STDINBITS "The standard input was compressed with %2$d bits. Only %3$d bits are allowed.\n"
$
COMPRESSION "Compression: "
$  the program inserts a calculated percentage after the (:) colon.
$ Leave a space after the colon. Do not put a new line at the end.
$
NOTREG "This is not a regular file. It is not changed."
$ This message is preceded by the filename on the same line.
$ Do not put a new line at the end.
$
HASLINKS "This file already has %d links. It is not changed."
$ This message is preceded by the filename on the same line.
$ Do not put a new line at the end.
$
UNCHANGE "This file is not changed; compression does not save space."
$ This message is preceded by the filename on the same line.
$ Do not put a new line at the end.
$
REPLACEW "This file is replaced with %s."
$ This message is preceded by the filename on the same line.
$ Do not put a new line at the end.
$
CORRUPTIN "uncompress: The input file contains bad data; SIGSEGV signal received.\n"
$ Do not translate SIGSEGV
$
OPTS "Options: "
$ Leave a space after the colon.
