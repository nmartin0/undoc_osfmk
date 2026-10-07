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

$ @(#)$RCSfile: logger.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:29:28 $

$ COMPONENT_NAME: (CMDOPER) commands needed for basic system needs  
$
$ FUNCTIONS:   
$
$ ORIGINS: 27  
$
$ IBM CONFIDENTIAL -- (IBM Confidential Restricted when  
$ combined with the aggregated modules for this product)  
$ OBJECT CODE ONLY SOURCE MATERIALS  
$
$ (C) COPYRIGHT International Business Machines Corp. 1989  
$ All Rights Reserved  
$
$ US Government Users Restricted Rights - Use, duplication or  
$ disclosure restricted by GSA ADP Schedule Contract with IBM Corp.  
$ messages for logger  
$
$ logger.msg	1.6  com/cmd/oper,3.1,9021 2/28/90 16:17:30
$
$quote "         define quote character.

$len 
$set MS_LOGGER        Define initial set#
$ Do not translate logger, %d, %s, %1$d, %1$s, etc ...

USAGE		"logger: [-i] [-f file] [-p pri] [-t tag] [ message ... ]\n"

UNKFACILITY	"logger: The specified facility name is not known: "

UNKPRIORITY	"logger: The specified priority name is not known: "
