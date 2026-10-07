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
$ @OSF_COPYRIGHT@
$
$
$ HISTORY
$ $Log: awk.msg,v $
$ Revision 1.2  1994/11/19  01:31:27  mtm
$ Copyright additions/changes
$
$ Revision 1.1  1993/06/18  14:46:43  shala
$ Files for new version of awk command.
$
$ Revision 1.4  90/12/06  14:09:58  devrcs
$ 	Fixed up comment headers.
$ 	[90/12/03  13:31:11  gm]
$ 
$ Revision 1.3  90/09/13  12:11:21  devrcs
$ 	Cleanup, RCS header, NLS, MSG
$ 	[90/08/29  09:57:42  knight]
$ 
$ Revision 1.2  90/07/05  23:25:38  devrcs
$ 	Remove IBM message numbers
$ 	[90/07/03  11:30:44  lehotsky]
$ 
$ 	Populate from AIX sources
$ 	[90/06/29  21:44:27  lehotsky]
$ 
$ $EndLog$

$ @(#)$RCSfile: awk.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:31:27 $

$ 
$ 	/*
$ 	 * COMPONENT_NAME: (CMDEDIT) awk.msg
$ 	 *
$ 	 * FUNCTIONS: none
$ 	 *
$ 	 * ORIGINS: 3, 10, 27
$ 	 *
$ 	 * This module contains IBM CONFIDENTIAL code. -- (IBM
$ 	 * Confidential Restricted when combined with the aggregated
$ 	 * modules for this product)
$ 	 * OBJECT CODE ONLY SOURCE MATERIALS
$ 	 * (C) COPYRIGHT International Business Machines Corp. 1989
$ 	 * All Rights Reserved
$ 	 *
$ 	 * US Government Users Restricted Rights - Use, duplication or
$ 	 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$ 	 *
$ 	 * awk.msg	1.6  com/cmd/edit/awk,3.1,9021 3/22/90 08:45:17
$ 	 */

$
$ messages for AWK
$

$set MS_AWK
$      Do not translate awk.
$      Do not translate %s, %c, %d, etc. These will be replaced by file names,
$      commands, numbers, etc.
$      The term "regular expression" is a unix technical term.
$      Some messages begin with a space. Please leave space in the translation.
$      Please do not add new lines that are not included in the source.
$quote "


YBAIL        "awk: Quitting"
YZERONULL    "awk: $zero&null"
$      Do not translate -- this is a debug message
YILGLSTMT    "awk: The statement cannot be correctly parsed."
YILLNXT      "awk: A next statement cannot be used inside a function."
YIDXNREXP    "awk: Regular expressions cannot be used with the index function."
YARRNFUNC    "awk: The %s array cannot be used as a function."
YFUNCREDEF   "awk: The %s function cannot be defined more than once."
XLEXUSG      "awk: lex %1$s [%2$s]\n"
$       Do not translate -- this is a debug message
XSTRTOOLONG  "awk: The string or regular expression\n\
\t%.10s cannot contain more than 399 bytes."
XILLNSTFN    "awk: Functions cannot contain other functions."
XILLRET      "awk: A return statement must be inside a function."
XNLINRE      "awk: Regular expression %.10s cannot contain a newline."
XNLINSTR     "awk: The string %.10s cannot contain a newline character."
BNORMFA      "awk: There is not enough memory available for the finite automaton."
$       Do not translate finite automaton.
BSPCMKDFA    "awk: There is an internal software error in the makdfa function."
$       Do not translate makdfa.
BSPCMKINIT   "awk: There is an internal software error in the makeinit function."
$       Do not translate makeinit.
BUNKTYPPNTR  "awk: The type %d in the penter function is not known."
$       Do not translate penter.
BUNKTYPFRTR  "awk: The type %d in the freetr function is not known."
$       Do not translate freetr.
BUNKTYPCFOIL "awk: The type %d in the cfoll function is not known."
$        Do not translate cfoll.
BUNKTYPFRST  "awk: The type %d in the first function is not known." 
BCHRCL2BIG   "awk: A regular expression bracket expression\n\
\tcannot contain more than 1,024 bytes."
BERRINRE     "awk: There is a regular expression error."
BSPCTOSTRING "awk: There is an internal software error in the tostring function."
$       Do not translate tostring.
BRE2BIG      "awk: Regular expression %s cannot contain more than 1,024 bytes."
BFOLSETOFLW  "awk: There is an internal software error in the cfoll function.\n\
\tFollow set overflow."
BSPCPMTCH    "awk: There is an internal software error in the pmatch function."
$       Do not translate pmatch.
BNOSTATSPC   "awk: There is not enough memory available now."
BEMPTYRE     "awk: A regular expression cannot be empty."
BSNTXRE      "awk: There is a usage error in regular expression %1$s at %2$s"
BILLPRMRYRE  "awk: The regular expression %1$s contains\n\
\ta primary character that is not correct at %2$s"
BNONTERMCHCL "awk: The regular expression character class %s\n\
\tis not correctly formed with the [ and ] characters."
BNLINCHRCL   "awk: The regular expression character class %s\n\
\tcannot contain a newline character."
BNOSPCCGOTO  "awk: There is an internal software error in the cgoto function."
$       Do not translate cgoto.
LOPEN        "awk: Cannot find or open file %s."
LIREC2LONG   "awk: Input line %.20s cannot be longer than 3,000 bytes."
L2MNYFLDS    "awk: Line %.20s cannot have more than 99 fields."
LCRT2MANYFLDS "awk: Cannot create more than 99 fields."
LGIANTREC    "awk:  A single giant record was built\n\
\tstarting at `%.20s...'\n"
LFLDACC      "awk: The field %d must be in the range 0 to 99."
LSRCLINE     " The source line is %d."
LINFUNC      " The function is %s."
LFPE         "awk: A floating point exception exists."
LMSNG        "\tawk: There is a missing %c character.\n"
LDMSNG       "\tawk: There are %1$d missing %2$c characters.\n"
LXTRA        "\tawk: There is an extra %c character.\n"
LDXTRA       "\tawk: There are %1$d extra %2$c characters.\n"
LIRECNUM     " The input line number is %g."
LFILENAME    " The file is %s."
LSRCLINENO   " The source line number is %d.\n"
$       This line is displayed after the MUSAGE message.
LCNTXT       " The error context is\n\t\t"
$       This line is displayed after the LSRCLINENO message.
$       The \t\t at the end is required to place the >>> <<< which surrounds
$       the error line as posted on the next line by the eprint function.
LEDOM        "awk: The %s parameter to a math library function is not in the domain."
LERANGE      "awk: The %s return value of a math library function is out of range."
MUSAGE       "Usage: %s [-F Character][-f File|Commands][Variable=Value][File ...]"
MOPEN        "awk: Cannot find or open file %s." 
PNODEALLOC   "awk: Internal software error in the nodealloc function."
$       Do not translate nodealloc.
PFUNCNOTARR  "awk: Use %s as a function and not as an array."
PARRANDFUNC  "awk: %s cannot be an array name and a function name."
RILLSTMT     "awk: A statement occurred that is not valid."
RUNEXPBT     "awk: A break, continue, or next statement is\n\
\tnot in the correct location."
RUNEXPEX     "awk: A break or next statement is\n\
\tnot in the correct location."
RUNDEFFN     "awk: Function %s is not defined."
RFNASARG     "awk: Function %s cannot be used as a parameter."
RFNNESTDP    "awk: Function %s cannot contain more than 99\n\
\tlevels of nesting."
RARGMSG      "awk: Supply parameter number %1$d of function %2$s."
RILLJMPTYP   "awk: Jump type %d is not correct."
RNOTARR      "awk: %s cannot be used as an array."
RUNKBOOL     "awk: The boolean operator %d is not known."
RUNKRELOP    "awk: The relational operator %d is not known."
RNOTEMPSPC   "awk: There is not enough memory available now."
RILLFLD      "awk: Field $(%s) is not correct."
RCONVSUPP    "awk: %%%%*... conversion is not supported in %s.  Try concatenation."
$       Do not translate the % signs at the beginning of the message.
RFMTLNG      "awk: Format item %.20s cannot be longer than 3,000 bytes."
R2FEWARGS    "awk: There are not enough parameters in printf statement %s."
$       Do not translate printf.
R0DIV        "awk: Cannot divide by zero."
RMOD0DIV     "awk: Cannot divide by zero when using the modulo operator."
$       Modulo is an arithmetic operator like addition or subtraction.
RILLARITH    "awk: Arithmetic operator %d is not correct."
RREADVAL     "Cannot read the value of"
$       This gets handed to message TCANTFN/TCANTARR as %1s$
RDIV0SLEQ    "awk: Cannot divide by zero when using the /= operator."
RDIV0MODEQ   "awk: Cannot divide by zero when using the %%%%= operator."
$      Do not translate the % signs in the message. 
RILLASGN     "awk: Assignment operator %d is not correct."
RILLSPLT     "awk: Internal software error in the split function."
RNOTARRAY    "awk: %s must be an array."
RILLFNTYP    "awk: Function type %d does not exist."
ROPEN        "awk: Cannot find or open file %s." 
RNULLFILENAME "awk: A print or getline function must have a file name."
$      Do not translate getline or print.
RTOOMANY     "awk: Cannot open file %s.\n\
\tA maximum of 15 files can be open at one time."
RILLREDIR    "awk: The redirection operator was not used correctly."
RSUBTOOBIG   "awk: The result %.20s of the sub function\n\
\tcannot be longer than 3,000 bytes."
RTGTSTR      "awk: target string: %s, *patbeg = %o, patlen = %d\n"
$       Do not translate -- this is a debug message
RMATCHFND    "     match found: "
$       Do not translate -- this is a debug message
RGSUB2BIG    "awk: The result %.20s of the gsub function\n\
\tcannot be longer than 3,000 bytes."
$      Do not translate gsub.
TZERONULL    "awk: $zero&null"
$       Do not translate -- this is a debug message
TSPCMKSYMTAB "awk: Internal software error in the makesymtab function."
$      Do not translate makesymtab.
TSYMTABOFLW  "awk: Internal software error in the symbol table at %s."
TSPCREHASH   "awk: Internal software error in the rehash function."
TASGNTO      "Cannot make an assignment to"
$      This gets handed to message TCANTFN/TCANTARR as %1s$
TCANTARR     "awk: %1$s %2$s. It is an array name."
TCANTFN      "awk: %1$s %2$s. It is a function."
TFUNNYVAR    "awk: Unexpected variable %o: n=%s s=\")%s\" f=%g t=%o"
TILLREF      "awk: The reference to array %s is not valid."
TREADVAL     "Cannot read the value of "
$      This gets handed to message TCANTFN/TCANTARR as %1s$
TSPCTOSTR   "awk: Internal software error in the tostring function on %s." 
TSTR2LNG    "awk: String %.10s cannot be longer than 399 bytes."
TNLINSTR    "awk: String %.10s cannot contain a newline character."
