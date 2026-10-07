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

$ @(#)$RCSfile: split.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:40:13 $
$
$ COMPONENT_NAME: (CMDFILES) commands that manipulate files
$
$ FUNCTIONS:
$
$ ORIGINS: 27
$
$ IBM CONFIDENTIAL -- (IBM Confidential Restricted when
$ combined with the aggregated modules for this product)
$ OBJECT CODE ONLY SOURCE MATERIALS
$ (C) COPYRIGHT International Business Machines Corp. 1985, 1989
$ All Rights Reserved
$
$ US Government Users Restricted Rights - Use, duplication or
$ disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$
$ split.msg	1.9  com/cmd/files,3.1,9021 3/20/90 17:50:55
$
$set MS_SPLIT
$quote "

USAGE "Usage: split [-Number] [File [Prefix]]\n"
INPOPNER "split: Cannot open input file.\n"
OUTNMSIZ "split: Output file cannot exceed %d characters.\n"
$ %d is the maximum number of characters in the output file.
ABRTSPLT "split: Split is terminated. Exceeded the maximum of 676 output files.\n"
OUTCRTERR "split: Cannot create output.\n"
