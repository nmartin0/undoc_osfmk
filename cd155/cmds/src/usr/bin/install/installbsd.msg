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

$ @(#)$RCSfile: installbsd.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:26:19 $

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
$ instbsd.msg	1.4  com/cmd/files/installbsd,3.1,9021 3/19/90 16:31:18
$
$ message for the install.sh shell script
$quote "
$
$
$set MS_INSTBSD
$ Do not translate install

1 "install: Cannot specify the -c flag more than once."
2 "install: Specify a destination."
3 "install: There are too many files specified: $*"
$ The files specified follow after the colon.
4 "install: Cannot move the file"
$ It appears that at times this message will be followed by message 5

5 "onto itself."
$ It appears that at times this message will follow message 4.
6 "install: Cannot open the file"
$ This is followed by the file name.
