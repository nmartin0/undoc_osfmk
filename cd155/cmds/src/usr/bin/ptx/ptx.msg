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
$
$ @(#)$RCSfile: ptx.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:35:03 $
$
$ COMPONENT_NAME: (CMDTEXT) Text Formatting
$
$ FUNCTIONS: ptx.msg
$
$ ORIGINS: 3,27,28
$
$ This module contains IBM CONFIDENTIAL code. -- (IBM
$ Confidential Restricted when combined with the aggregated
$ modules for this product)
$ OBJECT CODE ONLY SOURCE MATERIALS
$ (C) COPYRIGHT International Business Machines Corp. 1989
$ All Rights Reserved
$
$ US Government Users Restricted Rights - Use, duplication or
$ disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$
$ ptx.msg	1.3  com/ptx.d,3.1,9021 3/13/90 16:47:21
$
$quote "         define quote character.

$len 
$set MS_ptx        Define initial set#
$ Do not translate output formats used with C programs such as:
$ 	%d %i %x %X %u %c %s %f %e %E %g %G %p %n %ld
$ Do not translate shell variables such as:
$ 	$# $? $$ $! $- {$0 $1 $2 .... $some number}
$ Do not add newline characters to these messages.
M_MSG_2     "ptx: The width cannot be zero."
M_MSG_3     "ptx: The width cannot be greater than 200 characters."
M_MSG_4     "ptx: Specify one of these flags -o or -i."
M_MSG_5     "ptx: Specify one of these flags -o or -i"
M_MSG_6     "Usage ptx [-f] [-t] [-r] [-w Number] [-g Number] \n[-o Only|-i Ignore] [-b Breakfile] [File1] [File2]"
M_MSG_7     "ptx: Specify no more than 3 filenames."
M_MSG_8     "ptx: Cannot find or open break character file."
$ Do not translate break character
M_MSG_9     "ptx: There is not enough memory now."
M_MSG_10    "ptx: Cannot find or open file."
M_MSG_11    "ptx: There are too many words."
M_MSG_12    "ptx: There are too many words in the file."
M_MSG_13    "ptx: Cannot find or open sorting file"
M_MSG_14    "ptx: Cannot find or open data file"
M_MSG_15    "ptx: Cannot find or open output file"
M_MSG_16    "ptx: Cannot fork"
M_MSG_18    "ptx: Cannot find or open sorting file"
