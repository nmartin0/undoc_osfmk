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
$ COMPONENT_NAME: (CMDFILES) commands that manipulate files
$
$ FUNCTIONS:
$
$ ORIGINS: 27
$
$ This module contains IBM CONFIDENTIAL code. -- (IBM
$ Confidential Restricted when combined with the aggregated
$ modules for this product)
$ OBJECT CODE ONLY SOURCE MATERIALS
$ (C) COPYRIGHT International Business Machines Corp. 1985, 1989
$ All Rights Reserved
$
$ US Government Users Restricted Rights - Use, duplication or
$ disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$
$
$ messages for command diff (programs diff and diff -h)
$
$set MS_DIFF
$quote "
$
$ Do not translate diff, %s, %1$s, %d, -D, -s, -l, -e
$
ENOI "diff:Cannot use the -D flag when comparing directories.\n"
ENOSL "diff: warning: The -s or -l flag should not be used with the -e flag.\n"
EONLY "Only in %.*s"
$ DO NOT TRANSLATE
$ This is diff output, and the EXACT wording is dictated by standards.
$ Your customers will expect it this way.
$ What will follow is a list of files that are only in a certain directory
EIDENT "Common identical files"
$ DO NOT TRANSLATE
$ This is diff output, and the EXACT wording is dictated by standards.
$ Your customers will expect it this way.
$ Common meaning files which are in both directories
EBINARY "Binary files which differ"
$ DO NOT TRANSLATE
$ This is diff output, and the EXACT wording is dictated by standards.
$ Your customers will expect it this way.
ESUBDIR "Common subdirectories"
$ DO NOT TRANSLATE
$ This is diff output, and the EXACT wording is dictated by standards.
$ Your customers will expect it this way.
$ Common meaning subdirectories which are in both directories
$
ESTART "diff:Specify a file name with the -S flag.\n"
EUNKNOWN "usage:  diff [-bitw] [-c[Number]|-DString|-e|-f|-h|-n] File1 File2\n\
\tdiff [-lrs] [-SFile] [-bitw] [-c[Number]|-e|-f|-h|-n] [-r]\n\
\t     Directory1 Directory2\n"
$ do not translate "diff" or the flag names
$ %s will be replaced by the flag.
ETWO "diff: Specify two file names.\n"
EBADCNT "diff: Specify a number with the -c flag.\n"
EBAD "diff: Cannot use the -h flag with the -e, -f, -n, -c, or -D flags.\n"
EMEM "diff: There is not enough memory available now.\n"

SUBDIR "Common subdirectories: %s and %s\n"
$ DO NOT TRANSLATE
$ This is diff output, and the EXACT wording is dictated by standards.
$ Your customers will expect it this way.

FILES "Files %s and %s are identical\n"
$ the %s's are replaced by file names.
DBINARY "Binary files %s and %s differ\n"
$ DO NOT TRANSLATE
$ This is diff output, and the EXACT wording is dictated by standards.
$ Your customers will expect it this way.

NOPROC "diff: Cannot create another process.\n"

SUBARE "Common subdirectories are "
$ DO NOT TRANSLATE
$ This is diff output, and the EXACT wording is dictated by standards.
$ Your customers will expect it this way.

IDENT "%s and %s are identical\n"
$ DO NOT TRANSLATE
$ This is diff output, and the EXACT wording is dictated by standards.
$ Your customers will expect it this way.
$
SYMLINK "symbolic link files:\n\t"
$ DO NOT TRANSLATE
$ This is diff output, and the EXACT wording is dictated by standards.
$ Your customers will expect it this way.
$ file names will follow this

ESIZE "diff: The specified files are too large. Try the -h flag.\n"
ECREATE "diff: Cannot create %s.\n"
$ %s is replaced by a file name.
EOPEN "diff: Cannot open %s.\n"
$ %s is replaced by a file name.
EDIFFH "Cannot find the diffh command.\n"
$ do not translate diffh
EARGC "diff: Command usage error.\n"
$ Don't spend a lot of time on this one -- it is never seen

DMAGIC "Cannot open %s.\n"
$ %s is replaced by a file name.
DDIFFER1 "%s --> %s "
$ DO NOT TRANSLATE
$ Do not put a new line character at the end. DDIFFER2 will follow this.
DDIFFER2 " and %s --> %s differ\n"
$ DO NOT TRANSLATE
$ This is output for diff.

DSTDINS "diff: Cannot use the - character for more than one file name.\n"
ENODIF "There are no differences between the files.\n"
$ This is diff output. Do not translate.
DSTDIN "Cannot compare standard input against the directory.\n"
EENDIF "#endif /* %s */\n"
$ Do not translate this message at all.  Do NOT change anything.
EELSE "#else /* %s%s */\n"
$ Do not translate this message at all.  Do NOT change anything.
EIFNDEF "#ifndef %s\n"
$ Do not translate this message at all.  Do NOT change anything.
EIFDEF "#ifdef %s\n"
$ Do not translate this message at all.  Do NOT change anything.


EOPTIONS "Cannot use the -e or the -f flag with the -h flag.\n"
$ Obsolete message...do not translate
EARGS "Specify two file names.\n"
$ This is preceded on the line by diffh:
ESTDINS "Cannot use the - character for more than one file name.\n"
EACCESS "Cannot access or get status of file "
$ Leave the space at the end -- The file name will be printed on the same line
$ Translation suggestion: to make it more readable for your language, you
$ may want to change the format like this:
$ Cannot access or get status of file:     (leave a space at the end)

EOPEN2 "Cannot open file "
$ Leave the space at the end -- The file name will be printed on the same line
$ Translation suggestion: to make it more readable for your language, you
$ may want to change the format like this:
$ Cannot open the file:    (leave a space at the end)
EPROGERR "Program error "
$ Leave the space at the end -- A number -- something like "4"  or  "2"
$ will be printed on the same line.
ECHANGE "---change record omitted\n"
$ DO NOT TRANSLATE
ERESYNC "Cannot recover. Try using the bdiff command.\n"
