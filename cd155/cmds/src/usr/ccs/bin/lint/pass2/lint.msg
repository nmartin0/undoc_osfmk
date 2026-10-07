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
$ @(#)lint.msg	1.5  com/cmd/prog/lint/pass2,3.1,9013 3/1/90 11:54:31
$
$
$ COMPONENT_NAME: (CMDPROG) Programming Utilities
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
$quote "
$len 
$set MS_LINT        Define initial set#

$  Message numbers have been assigned a range of 300-399.
$  Do not translate lint:, %s, %d, %1$s, %1$d, etc ...

1                    "lint: The %s flag option is missing.\n"
2                    "lint: %s flag is not recognized. It is ignored.\n"
3                    "lint: File %s must have a .c or .ln extension. It is ignored.\n"
$  Do not translate .c, .ln
M_MSG_258            "lint: There is not enough memory available now."
M_MSG_259            "The internal hash table overflowed."
M_MSG_260            "File %s data error"
M_MSG_261            "Action for symbol in file %s is not known"
$  Action is an operation that the lint program is trying to perform on symbol
$  within the file.
M_MSG_262            "Record directive in file %s is not known"
M_MSG_263            "\"%s\", line %d: warning: "
M_MSG_264            "\"%s\", line %d (\"%s\"): warning: "
M_MSG_265            "End of file reached before expected in %s."
M_MSG_266            "Cannot open file %s\n"
M_MSG_304            "function %s multiply defined"
$  Message 304-319 are written as standard output. They are not error messages.
$  Talk with your technical representatives before you translate these. Some
$  customers may have programs that depend on these messages comming out exactly
$  as they are.

M_MSG_305            "symbol %s multiply defined"
M_MSG_306            "symbol %s type inconsistent"
$  Type is a C-Language term which defines classes of objects.
M_MSG_307            "struct/union/enum %s inconsistently redefined"
$  Do not translate struct, union, enum
M_MSG_308            "function %s type inconsistent"
$  Type is a C-Language term which defines classes of objects.
M_MSG_309            "function %s argument type inconsistent"
$  Argument = parameter.
$  Type is a C-Language term which defines classes of objects.
M_MSG_310            "function argument %s count mismatch"
$  The definition of the function has a different number of parameters
$  than the call of the function.
M_MSG_311            "function %s used but not defined"
M_MSG_312            "symbol %s used but not defined"
M_MSG_313            "function %s defined but never used"
M_MSG_314            "symbol %s defined but never used"
M_MSG_315            "function %s declared but never used or defined"
M_MSG_316            "symbol %s declared but never used or defined"
M_MSG_317            "function %s return value used, but none returned"
$  The function call is requesting a return value, but the function itself does
$  not return a value.
M_MSG_318            "function %s return value is always ignored"
M_MSG_319            "function %s return value is sometimes ignored"
