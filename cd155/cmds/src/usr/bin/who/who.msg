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

$ @(#)$RCSfile: who.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:47:31 $
$ 
$ 
$ COMPONENT_NAME: (CMDSTAT) status
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
$ who.ms 1.9  com/cmd/stat,3.1,9021 4/14/90 16:32:08

$quote "         define quote character.

$len
$set MS_WHO        Define initial set#

NOTERM               "who: The process is not attached to a terminal.\n"
BADSWTCH             "who: The flag is not recognized.\n"
NOEXST               "who: %s either does not exist or is not readable.\n"
$
$ %s is replaced with a name.
EMPTYSLT             "There is an empty slot in the accounting file.\n"
ID		" id=%7.7s"
$ Do NOT translate
$ %7.7s is replaced by a name.
TERMID		" term=%d exit=%d"
$ Do NOt translate
USAGE                "Usage: who [-AabdlprsTtu] [{am i}] [File]\n"
$ Do not translate {am i}

AMI                  "am i"
$ Do not translate. It is part of the who command.

USEINFO              "\t  -u  Useful information\n"
TTYMSG               "\t  -T  Status of the tty (+ writable, - not writable, x exclusive open, ? no response)\n"
PROCCNT              "\t  -l  Login processes\n\
\t  -d  Dead processes\n"
BOOTTM               "\t  -b  Boot time \n\
\t  -t  Time changes\n"
OPTIONS              "\t  -a  All (Tabdlstu) options\n"
SHRTFRM              "\t  -s  Short form of who \n\
\n"
NAME            "Name"
$ yes translate....eight characters maximum
TTYSTATUS       "ST"
$ Do not translate
ACTIVITY        "Activity"
$ Yes translate....8 characters maximum
WHO_LINE	"Line "
$ Do not translate
WHO_TIME        "   Time   "
$ Yes translate....10 characters max
WHO_PID		"PID"
$ Do not translate
WHO_EXIT        "/Exit"
$ Do not translate
