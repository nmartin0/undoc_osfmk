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
$ @(#)$RCSfile: admin.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:37:35 $
$
$ IBM CONFIDENTIAL
$ Copyright International Business Machines Corp. 1989
$ Unpublished Work
$ All Rights Reserved
$ Licensed Material - Property of IBM
$
$ US Government Users Restricted Rights - Use, duplication or
$ disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$
$       admin.msg 1.6 com/cmd/sccs/hdr,3.1,9021 4/3/90 17:01:28
$
$quote "         define quote character.

$len 
$ DO NOT TRANSLATE the (ad#) at the end of each of these lines.
$ DO NOT TRANSLATE SCCS, SID, admin:, 
$ DO NOT TRANSLATE %c, %s, %1$s, etc...
$ DO NOT TRANSLATE -i, -y, -t, -m, -n, -d, etc..
$set 4        Define initial set#

DNOARG               "\n\
admin: Specify a header flag with the -d flag. (ad1)\n"
FLGNOVAL2            "admin: Specify a value for the %c header flag. (ad2)\n"
UNKNWNFLG            "admin: %c is not a recognized header flag. (ad3)\n\
Usage: admin {-n|-i[Name]} [-f{l{a|Number ...}|\n\
\t{b|cNumber|dSID|fNumber|i|j|mModule|n|qText|tType}} ...]\n\
\t[-fv[Program] -m[MRList]] [-aUser ...] [-rNumber.Number]\n\
\t[-t File] [-y[Comment]] NewFile ...\n\
\tCreates and initializes SCCS files.\n\
\n\
\tUsage: admin [-aUser ...] [-eUser ...] [-f{l{a|Number ...}|\n\
\tb|cNumber|dSID|fNumber|i|j|mModule|n|qText|tType} ...]\n\
\t[-d{l{a|Number ...}|b|c|d|f|i|j|m|n|q|t ...] [-dv|-fv[Program]]\n\
\t[-t[File]] File ...\n\
\tChanges existing SCCS files.\n"
FLGTWICE "admin: Cannot use the %c header flag more than once.\n"
FFLGNOARG            "\n\
admin: Specify a header flag with the -f flag.\n"
AFLGARG              "\n\
admin: Specify a user name, group name or group number with\n\
\tthe -a flag.\n"
AFLGKEYLTRS          "\n\
admin: Cannot use the -a flag more than 10 times\n\
\ton each command line.\n"
BADEARG              "\n\
admin: Specify user name, group name or group number to\n\
\tremove.\n"
EFLGKEYLTRS          "\n\
admin: Cannot use the -e flag more than 10 times\n\
\ton each command line.\n"
CFLGVAL              "admin: The %c header flag cannot have a value\n\
\twhen used with the -d flag.\n"
FLGVAL13             "admin: The %c header flag cannot have a value\n\
\twhen used with the -f flag.\n"
NODFLTSID            "\n\
admin: Specify an SID number with the d header flag.\n"
$ Do not translate SID or d
MTONEFIL             "\n\
admin: Create only one SCCS file at a time with the -i flag.\n"
$ Do not translate SCCS or -i.
RONLYIORN            "\n\
admin: Use the -r flag only with the -i flag or the -n flag.\n"
TFLGNOARG            "\n\
admin: Specify a file name for the -t flag when creating\n\
\ta new SCCS file.\n\
."
NODWITHN             "\n\
admin: Cannot use the -d flag with the -n  or -i flags.\n"
FILEEXISTS           "admin: File %s exists. Cannot use the -n flag or\n\
\tthe -i flag with an existing file.\n"
FLEXST20             "admin: File %s does not exist.\n"
ILLGLDATA            "admin: %1$s contains illegal data on line %2$d.\n\
\t (ad21)\n"
$ %1$s is the file name.  %2$s is the line number. 
FLOORNOTNUM          "admin: Specify a value between 0 and 10000\n\
\twith the f header flag.\n"
FLOORRNG             "admin: Specify a value between 0 and 10000\n\
\twith the f header flag.\n"
CEILNONNUM           "admin: Specify a value between 0 and 10000\n\
\twith the c header flag.\n"
CEILRNG              "admin: Specify a value between 0 and 10000\n\
\twith the c header flag.\n"
DIRIKYLTR            "\n\
admin: Specify a file, not a directory, with the -i flag.\n"
BADLISTFMT           "\n\
admin: The version and release must be numbers\n\
\tseparated by a comma.\n"
ELMNTRNG             "\n\
admin: Cannot lock a version number or a release number\n\
\tgreater than 9999.\n"
BDIR                 "admin: Cannot use the -%2$c flag with directory %1$s.\n\
\tSpecify a file, not a directory, with the -i flag or the -t flag.\n"
$ %2$c is a flag. %1$s is a directory. 
ILLYORM              "\n\
admin: Use the -y flag or the -m flag only when creating\n\
\tan SCCS file."
BADMCMR              "\n\
admin: Bad CMR number or numbers."
$ Do not translate. This message will not be seen by the user. 
BYMSG                "by"
DATETIME             "date and time created"
NOAPPL               "\n\
admin: No application with application flag."
$ Do not translate. This message will not be seen by the user. 
NOFREDFILE           "\n\
admin: No .FRED file exists for this application."
$ Do not translate. This message will not be seen by the user. 
NOFZWITHFV           "\n\
admin: Can't use -fz with -fv."
$ Do not translate. This message will not be seen by the user. 
NOTWOVERRTNS         "\n\
admin: Can't have two verification routines."
$ Do not translate. This message will not be seen by the user. 
LINECNTWARN          "\n\
WARNING - The number of lines in this file exceeds\n\
\t99999.  The header section of the s. file will record 99999 in the\n\
\tlines inserted field.\n"
$ The source file line count exceeds 5 digits. 
$ The s. file has a 5 digit limit so it will record 99999. 
