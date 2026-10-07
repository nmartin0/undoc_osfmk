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

$ @(#)$RCSfile: sort.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:39:58 $
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
$ sort.msg  1.8  com/cmd/files,3.1,9021 3/20/90 17:50:43

$quote "
$
$set MS_SORT
$
$ Note to NLS translator: don't add a new line character to
$ the end of any of these that do not already have one.
$ sort:         Will be added to the front of all the messages
$
USAGE "Usage: sort [-Abcdfimnru] [-T Directory] [-tCharacter]\n\
\t[+Position1[-Position2] ...] [-o File] [File ...]"
IDENT "The -o flag requires a file name parameter."
PATH "The path name is too long: "
$ leave a space at the end
KEYS "Cannot use more than 10 sort keys."
CHECK "Specify only one input file when using the -c flag."
ALLOC "There is not enough memory available to perform the sort."
$ Do not translate sort
LOCATE "Cannot locate the temporary directory or /tmp."
$ Do not translate /tmp
MALLOC "There is not enough memory available to perform the merge."
$ Do not translate merge
NEWLINE "Warning: A newline character was added to the end of the file."
NEWLINE2 "Warning: A newline character was added to the end of the input file."
FATAL "A line of the input file contains more than %d characters."
NEWLINE3 "Warning: A newline character was added to the end of the file."
TOOLONG "A line of the input file contained more than %d characters."
CREATE "Cannot create "
$ Leave a space at the end. The message will be followed by file name
OPEN "Cannot open "
$ Leave a space at the end. The message will be followed by file name
EREAD "The read system call failed. Cannot read from "
$ Do not translate read
$ Leave a space at the end. This is followed by the STDIN
$ message or a file name.
STDIN "standard input."
EWRITE "A write error occurred while "
$ Do not translate write
$ Leave a space at the end.   This is followed by the MERGE or SORT message.
MERGE "merging."
SORT "sorting."
DISORDER "This line is out of order: "
$ Leave a space at the end.
NUNIQUE "Duplicate line: "
$ Leave a space at the end.
NULLS "Cannot process the specified file; it contains null characters."
