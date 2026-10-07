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

$ @(#)$RCSfile: wc.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:47:21 $
$
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
$quote "
$set MS_WC

USAGE "Usage: wc [-clw] [File ...]\n"
$ The above is the usage statement that is not KJI.
USAGE_K "Usage: wc [-clwk] [File ...]\n"
$ The above is the KJI usage statement.
CANTOPEN "wc: Cannot open %s. Check path name and permissions.\n"
$ %s is the file name.
TOTAL "total\n"
$ When the user uses the wc command on more than one file at a time, the
$ word "total" appears to the right of the total characters, words, and lines
$ of all files specified.
