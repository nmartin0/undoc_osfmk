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

$ @(#)$RCSfile: touch.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:43:17 $
$
$ COMPONENT_NAME: (CMDSCAN) commands that scan files
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
$ touch.msg	1.6  com/cmd/scan,3.1,9013 3/4/90 17:36:01
$
$quote "         define quote character.
$len
$set MS_TOUCH        Define initial set#
$ Do not translate touch, %s.

TUSAGE               "Usage: touch [-amc] [mmddhhmm[yy]] File ...\n"
$       Do not translate touch.
$       Do not translate the flags.
$       mm is represents a  2-digit number for month.
$       dd is represents a  2-digit number for day.
$       hh is represents a  2-digit number for hour.
$       mm is represents a  2-digit number for minute.
$       yy is represents a  2-digit number for year.
TCONV                "touch: The access or modification time format is: [mmddhhmm[yy]].\n"
$       mm is represents a  2-digit number for month.
$       dd is represents a  2-digit number for day.
$       hh is represents a  2-digit number for hour.
$       mm is represents a  2-digit number for minute.
$       yy is represents a  2-digit number for year.
TCREATE              "touch: Cannot create %s.\n"
TSTAT                "touch: Cannot get status on %s.\n"
TCHTIME              "touch: Cannot change the modification time on %s.\n"
