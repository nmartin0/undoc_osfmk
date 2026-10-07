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
$ (c) Copyright 1992, INTEL, CORP.
$ ALL RIGHTS RESERVED
$
$
$ INTEL Release 1.0

$
$
$ messages for command lsize
$

$set MS_LSIZE

$quote "

USAGE "Usage: lsize [-a] size[k|m|g] file ...\n\
 [-a]:   Append to the file\n\
 size:   Size in bytes [kilobytes][megabytes][gigabytes]\n\
 file:   File name\n"
NOSPACE "lsize: No space left on device, %s bytes allocated for %s.\n"
SIZE "lsize: Invalid size %s.\n"
OVERFLOW "lsize: Size %s is too large.\n"
