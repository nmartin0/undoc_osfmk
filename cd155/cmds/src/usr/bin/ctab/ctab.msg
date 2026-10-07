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
$ IBM CONFIDENTIAL
$ Copyright International Business Machines Corp. 1989
$ Unpublished Work
$ All Rights Reserved
$ Licensed Material - Property of IBM
$
$
$ US Government Users Restricted Rights - Use, duplication or
$ disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$ 
$ @(#)ctab.msg	1.4  com/cmd/nls,3.1,9013 2/28/90 13:41:56
$
$	Messages for command ctab.
$	Do NOT add a newline character to messages that do not have one.
$
$ Note for the technical reviewer:
$ 	1 - Please replace the xxxx-xxx stuff with the
$	    component numbers and message id's
$
$	2 - The notes after various messages are for the NLS translators
$
$set MS_CTAB
$quote "
1 "No filename was given."
$ Don't spend a lot of time translating this one, it will never appear.
2 "Check path name and permissions. Cannot open file "
$ Leave the space at the end of the message. File name will follow.
3 "Cannot open the specified output file.\n\
\tCheck the write permissions for the directory."
4 "Cannot write to the output file.\n\
\tUse local problem reporting procedures."
5 "Use local problem reporting procedures.\n\
\tCannot close input file "
$ Leave the space at the end of the message. File name will follow.
6 "Use local problem reporting procedures.\n\
\tCannot close output file "
$ Leave the space at the end of the message. File name will follow.
7 "The seek system call failed in the output file.\n\
\tUse local problem reporting procedures."
8 "Specify a subject character in field 1."
9 "Specify only 1 character in the case field."
10 "The subject field contains incorrect characters."
11 "The subject field contains a repeat or\n\
\ttranslation character that is not recognized."
$ Don't spend a lot of time translating this one, it will never appear.
12 "The subject character must be a single character for translation."
13 "The words on and off are the only recognized\n\
\tvalues for the eclass option."
$ The words "on" and "off" should not be translated because
$ the code looks for these literal strings. The word eclass
$ should not be translated
14 "The parameter after the option statement\n\
\tmust be one of eclass, sep, trans, repeat or comment.\n\
\tThis option is not recognized: "
$ option,eclass,sep,trans,repeat and comment are all ctab specific words.
$ Leave the space at the end of the message. A word will follow.
15 "Supply a character with the sep option."
$ Do not translate sep -- it's a ctab specific word.
16 "Supply a character with the trans option."
$ Do not translate trans -- it's a ctab specific word.
17 "Supply a character with the repeat option."
$ Do not translate repeat -- it's a ctab specific word.
18 "Supply a character with the comment option."
$ Do not translate comment -- it's a ctab specific word.
19 "Specify only one character as a\n\
\tparameter for the option "
$ Leave the space at the end -- a word will follow
20 "There is an internal software error.\n\
\tUse local problem reporting procedures."
21 "Extra characters in the third field."
$ Don't spend a lot of time translating this one, it will never appear.
22 "Field 3 must contain one of the\n\
\tcharacters L, l, U, u, C, c, S, s, a, A, p, P, g, G, x, X, n or N."
23 "Do not specify more than one entry\n\
\tfor the same subject character."
24 "The translation string cannot contain the subject character."
25 "Specify the first character of the\n\
\tcollate sequence as a subject character.\n\
\tNo collating sequence was given for "
$ Leave space at the end -- something will follow
26 "An incorrect octal escape sequence was given."
27 "Field 1 does not contain any translate characters."
28 "Give a case specifier for field 3."
29 "Warning: Field 4 (\"equivalence class\") no longer supported\n\
\tRemove the field from the input file."
30 "The NULL character (\\000) is only allowed first in the\n\
\tcollation sequence."
31 "An invalid character (or a multi-byte character) was given."
M_POST  "Error in post checking: %1$s %2$s\n"
M_OTHER "Error on line %1$d: %2$s %3$s\n"
$ These are the messages that will always appear and contain the
$ other messages ---  %1$d will be a number,
$ %2$s (or %1$s) will be one of the above messages, and
$ %3$s (or %2$s) will be whatever follows messages 2, 5, 6, 14, 20 or 27
M_USAGE "Usage: ctab [-i InputFile] [-o OutputFile]\n\
\tProduces a collating table.\n"
