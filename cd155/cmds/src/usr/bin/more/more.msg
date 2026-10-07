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
$ @(#)$RCSfile: more.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:30:45 $
$
$
$ Message for the more command.
$
$quote "
$set MS_MORE

BACK		"\n...back %d %s\n\n"
PAGES		"pages"
PAGE		"page"
SKIP		"\n...skipping %d %s\n\n"
LINES		"lines"
LINE		"line"
GOTO		"\n... Going to line %d\n\n"
GOTO2		"\n...Going to last line\n\n"
BACK2		"\n***Back***\n\n"
SKIPPING	"\n...skipping\n\n"
CANTOPEN	"Can't open help file"
INSTRUCT	"[Press 'h' for instructions.]"
LINE2		"\"%s\" line %d"
NOTFILE		"[Not a file] line %d"
UNKNOWN		"\"Unknown\" [stdin] line %d"
VERSION		"more: version 2.0"
NOTFOUND	"Pattern not found"
REGEXP		"Regular expression botch"
PATTERN		"\nPattern not found\n"
PATTERN2	"Pattern not found"
CANTFORK	"Cannot fork new process\n"
NOCMD		"No previous command to substitute for"
$
$ '%sto' turns into 'back to' or just 'to'
$ so make sure you leave the space after BACK3 below
$
FILESKIP	"\n...Skipping %sto file %s\n\n"
BACK3		"back "
$
$ Make sure you leave a space at the end here
$
USAGE		"Usage: "
USAGE2		" [-cdflnpsuv] [+linenum | +/pattern] name1 name2 ...\n"
TERMDEV		"Cannot open alternate term device"
ALTTERM		"Alternate term not on fd 1\n"
DIRECT		"\n*** %s: directory ***\n\n"
NOTTEXT		"\n******** %s: Not a text file ********\n\n"
QQUIT		"[Use q or Q to quit]"
MORE		"--More--"
NEXTFILE	"(Next file: %s)"
CONT1		"[Press space to continue, q to quit, h for help]"
NOMEM		"Not enough memory for screeninfo array"
NOMEM2		"Not enough memory"
CONT2		"Press any key to continue"
LINETOLONG	"Line too long"
