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

$ @(#)$RCSfile: checknr.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:20:09 $

$ 
$ 	COMPONENT_NAME: (CMDTEXT) Text Formatting Services
$ 
$ 	FUNCTIONS:
$ 
$ 	ORIGINS: 10,26,27
$ 
$ 	This module contains IBM CONFIDENTIAL code. -- (IBM
$ 	Confidential Restricted when combined with the aggregated
$ 	modules for this product)
$ 	OBJECT CODE ONLY SOURCE MATERIALS
$ 	(C) COPYRIGHT International Business Machines Corp. 1989
$ 	All Rights Reserved
$ 
$ 	US Government Users Restricted Rights - Use, duplication or
$ 	disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$ 
$ 	checknr.msg	1.4  com/bsd.d/checknr.d,3.1,9021 3/13/90 16:48:21
$ 

$set MS_CHECKNR
$ Do not translate output formats used with C programs such as:
$ 	%d %i %x %X %u %c %s %f %e %E %g %G %p %n %ld
$ Do not translate shell variables such as:
$ 	$# $? $$ $! $- {$0 $1 $2 .... $some number}

$quote "

USAGE   "Usage: checknr [-s] [-f] [-a{.Macro1.Macro2...}] [-c{.Command1.Command2...}] [file...]\n"
EMPTY   "checknr: Specify a parameter with the -c or -a flag.\n"
TOOLONG "checknr: Macro or directive invalid.\n"
FUNMATCHED  "checknr: There is no match for the \\fP command.\n"
SUNMATCHED  "checknr: There is no match for the \\s0 command.\n"
UNMATCHED   "unmatched "
BUG         "checknr: Bug: stk[%1$d].opno = %2$d = .%3$s, .%4$s\n"
DOESNOTMATCH "does not match %d: \n"
MACUNMATCHED "unmatched .%s\n"
UNKCOM       "unknown command: .%s\n"
ILLEGAL      "checknr: illegal define: %s\n"
COMALLOW     "checknr: Only %d known commands allowed.\n"
