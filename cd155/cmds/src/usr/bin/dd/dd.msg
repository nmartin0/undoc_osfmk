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
$ (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
$ ALL RIGHTS RESERVED
$
$
$ OSF/1 Release 1.0.1

$ @(#)$RCSfile: dd.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:22:20 $

$ @(#)dd.msg	1.5  com/cmd/arch,3.1,9021 3/21/90 21:24:57
$
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

$quote "
$set MS_DD
$ Do not translate dd

RECSIN "dd:  %1$u+%2$u records in.\n"
RECSOUT "dd:  %1$u+%2$u records out.\n"

TRUNRECS "dd:  %u are truncated records.\n"

EREAD "dd:  read error"
$ Do not translate read

EWRITE "dd:  write error"
$ Do not translate write

EARG "dd:  The command parameter %s is not correct.\n\
\tUsage: dd [if=InputFile] [of=OutputFile] [cbs=Number] [fskip=Number]\n\
\t          [skip=Number] [seek=Number] [count=Number] [ibs=Number]\n\
\t          [obs=Number] [conv=Parameter[, ...]]\n"

EBLKSIZ "dd:  Block sizes cannot be zero.\n"

ENOMRY "dd:  There is not enough memory available now.\n"

ESTOMP "dd:  Can't copy file onto itself.\n"

EPART	"dd: Unable to write complete record.\n"
