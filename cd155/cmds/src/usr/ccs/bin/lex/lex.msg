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
$ RESTRICTED RIGHTS LEGEND
$ Use, Duplication or Disclosure by the Government is subject to
$ restrictions as set forth in paragraph (b)(3)(B) of the rights in
$ Technical Data and Computer Software clause in DAR 7-104.9(a).
$ 
$ @(#)lex.msg	1.4  com/cmd/lang/lex,3.1,8943 9/7/89 18:36:27
$
$ COMPONENT_NAME: (CMDLANG) Language Utilities
$
$ FUNCTIONS: 
$
$ ORIGINS: 27
$
$ (C) COPYRIGHT International Business Machines Corp. 1988, 1989
$ All Rights Reserved
$ Licensed Materials - Property of IBM
$
$ US Government Users Restricted Rights - Use, duplication or
$ disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$
$set MS_LEX
$quote "
BADOPTION  	"Unknown option %c"
BADSTATE   	"bad state %d %o"
BIGPAR		"Parse tree too big %s"
BIGPAR2         "\nTry using %%e num"
CALLOCFAILED  	"OOPS - calloc returns a 0"
CHARPUSH        "Too many characters pushed"
CHARRANGE  	"Character value %d out of range"
CHTAB1  	"ch table needs redeclaration"
DCHAR0  	"Character %o used twice"
DCHARC  	"Character '%c' used twice"
EMAXPOS  	"Too many positions %s"
EMAXPOS1        "\nTry using %%p num"
ENDLESSTR  	"Non-terminated string"
ENTRANS  	"Too many transitions %s"
ENTRANS1  	"\nTry using %%a num"
EOFCOM   	"EOF inside comment"
EOFSTR          "EOF in string or character constant"
EPOS  		"Too many positions for one state - acompute"
EREVERSE  	"No space for char table reverse"
ERTEXTS  	"Too many right contexts"
ERR             "(Error) "
EXSTATES  	"Executable statements should occur right after %%"
INVALREQ  	"Invalid request %s"
LANG2LATE  	"Too late for language specifier"
LINE  		"line %d: %s\n"
LONGDEFS  	"Definitions too long"
LONGSTART  	"Start conditions too long"
MANYCLASSES  	"Too many large character classes"
MANYDEFS  	"Too many definitions"
MANYSTATES	"Too many states %s"
MANYSTATES2     "\nTry using %%n num"
NEGINTR  	"Iteration range must be positive"
NOCORE1  	"Too little core to begin"
NOCORE2  	"Too little core for state generation"
NOCORE3  	"Too little core for final packing"
NOCORE4  	"Too little core for parse tree"
NODEF  		"Definition %s not found"
NOINPUT  	"Can't read input file %s"
NOSTDIN		"Can't read standard input"
NOLDIGITS  	"Substitution strings may not begin with digits"
NOLEXDRIV  	"Lex driver missing, file %s"
NONTERM         "Action does not terminate"
NOPEN 		"Can't open %s"
NOPEN2          "Cannot open file %s" 
NOPORTCHAR  	"Non-portable Character Class"
NORATFOR  	"Ratfor not currently supported with lex"
NORULESEND  	"Rules section delimiter missing"
NOSTART  	"Undefined start condition %s"
NOTRANS  	"No translation given - null string assumed"
OVERFL		"output table overflow"
PEOF  		"Premature eof"
SHIFT           "Shift Character Found."
STRLONG  	"String too long"
MANYPCLASSES	"Too many packed character classes"
TSTART  	"Too many start conditions used"
TSTART1         "Too many start conditions defined"
WARN 		"(Warning) "
XTRASLASH  	"Extra slash removed"
STATISTIC	"%d/%d nodes(%%e), %d/%d positions(%%p), %d/%d (%%n), %ld transitions\n"
STATISTIC2      "%d/%d packed char classes(%%k)\n"
STATISTIC3      "%d/%d packed transitions(%%a)\n"
STATISTIC4	"%d/%d output slots(%%o)\n"
