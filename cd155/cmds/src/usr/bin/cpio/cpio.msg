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

$ @(#)cpio.msg	5.1 17:50:41 8/15/90 SecureWare
$ @(#)$RCSfile: cpio.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:21:15 $

$ COMPONENT_NAME: (CMDARCH) archive files
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
$ cpio.msg	1.8  com/cmd/arch,3.1,9021 3/21/90 21:24:41
$
$quote "
$set MS_CPIO
$ Do not translate cpio

ATPFTS "Cannot pass a file to itself.\n"

BBS "Invalid buffer size '%s'\n"

BCIRR "Cannot use the -B or -C flags with the -p flag.\n"

BSR "The block size was reduced to %u bytes.\n"

CBS "Check block size.\n"
$ This is preceded by a perror message.

CCD "Cannot create directory for %1$s; errno is %2$d.\n"
$ Do not translate errno.
$ The %1$s will be a file name and the other will be an integer

CCDNW "Cannot move to directory.\n"
$ Don't spend a lot of time on this one -- it is never seen

CCF "Cannot create %1$s; errno is %2$d.\n"
$ Do not translate errno.
$ The %1$s will be a file name and the other will be an integer

CCHD "Cannot move to a specified directory.\n"
$ Don't spend a lot of time on this one -- it is never seen

CCLN "Cannot symlink <%s> and <%s>: "

CCM "Cannot change permissions of %1$s; errno is %2$d.\n"
$ Do not translate errno.
$ The %1$s will be a file name and the other will be an integer

CCO "Cannot change ownership of %1$s; errno is %2$d.\n"
$ Do not translate errno.
$ The %1$s will be a file name and the other will be an integer

CFN "A newer or same aged version of %s already exists.\n"
$ The %s will be a file name

CLSTS "Cannot link %1$s and %2$s: "
$ The variables will be file names

CNMN "Cannot create %1$s; errno is %2$d.\n"
$ Do not translate errno.
$ The %1$s will be a file name and the other will be an integer

COBS "The -B flag value overrides the -C flag value.\n"

COCS "The -C flag value overrides the -B flag value.\n"

COPN "Cannot open %s.\n"

CRNDT "Cannot use the -r flag (rename) because %s is missing.\n"
$ %s is replaced with "/dev/tty"

CRS "Cannot read from %s.\n"

CRSL "Cannot read from symbolic link <%s>: "

CUL "Cannot unlink %1$s; errno is %2$d.\n"
$ Do not translate errno.
$ The %1$s will be a file name and the other will be an integer

CWF "Cannot write to %s.\n"
$ The %s will be a file name

CWIF "Cannot write to the %s directory.\n"

DINPUT "input"

DOUTPUT "output"

EINPUT "Cannot open <%s> for input: "

EOUTPUT "Cannot open <%s> for output: "

EOM "\007Reached end of medium on %s\n"
$ Do not translate "\007cpio"
$ %s is replaced with DINPUT or DOUTPUT

FLTF "%1$s is linked to %2$s.\n"
$ Both will be file names

GOON "Enter the device or file name when ready to continue.\n"

IGNSPECIAL "Special file %s was ignored.\n"

IHDRQS "Cannot get information about %s.\n"
$ The %s will be a file name

IHDRQ "Cannot open %s.\n"
$ The %s will be a file name

IOERR "\007Encountered an error on "
$ This is followed by either DINPUT or DOUTPUT and then a perror message.

LOFDM "The file name length does not match the expected value.\n"

MISD "Cannot move to a necessary directory.\n"

NEW "Original file has a later modification date. Cannot write.\n"

NEXTVOL "Change to volume %d and press RETURN key. [q] "

NMFL "There is not enough memory available to continue linking files.\n"

NOBLKCOP "%ld blocks were copied.\n"

NOMLC "There is not enough memory available now.\n"

NOPROMPT "Cannot prompt for input (can't open %s): "
$ %s is replaced with "/dev/tty"; this is followed by a perror message.

NOSPECIAL "Special file %s cannot be archived.\n"

OMIL "Use one of the -o, -i, or -p flags.\n"

OOF "Cannot copy from the diskette.\n"
$ Do not translate cpio

PREX "Cannot use the -r flag with the -p flag.\n"

RPRPT "Rename <%s>: "
$ Do not translate.

SCLN "Cannot create a symbolic link.\n"

SKIP "Skipped\n"
$ Do not translate

TML "There are too many links.\n"

USAGE1 "Usage: %s %s [-Cbufsize] <name-list >collection\n"
USAGE2 "\t%s %s [-Cbufsize] -Ocollection <name-list\n"
USAGE3 "\t%s %s [-Cbufsize] [pattern ...] <collection\n"
USAGE4 "\t%s %s [-Cbufsize] -Icollection [pattern ...]\n"
USAGE5 "\t%s %s directory <name-list\n"

USAGEOPT1 "-%c must be specified before -%c option"
USAGEOPT2 "-%c option not permitted with -%c option"
USAGEOPT3 "No more than one -I or -O flag permitted"

USE_D "Use the -d flag to copy %s.\n"

WINPUT "Cannot read from the specified input: "

WOUTPUT "Cannot write to the specified output: "
