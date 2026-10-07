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

$ @(#)$RCSfile: mt.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:31:14 $

$quote "
$set MS_MT
$ do not translate mt

USAGE "Usage: mt [ -f device ] command [ count ]\n"

INVALID "mt: %s is not a recognized command.\n"
$ %s is replaced by a string

NEGRCNT "mt: negative repeat count\n"

FAILED "failed"
$ This is preceded by the tape & command names, and followed by a perror msg.

UNKNOWN "unknown tape drive type (%d)\n"

STATUS "%s tape drive, residual=%d\n"

AUTH "mt: need tape authorization.\n"

NOPRIV "mt: insufficient privileges.\n"
