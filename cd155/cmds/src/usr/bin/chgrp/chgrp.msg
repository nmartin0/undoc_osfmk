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

$ @(#)$RCSfile: chgrp.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:20:12 $
$ 
$
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
$
$ chgrp.msg	1.7  com/cmd/oper,3.1,9013 2/28/90 16:15:34
$
$quote "         define quote character.
$len 
$set MS_chgrp        Define initial set#

$ Do not translate chgrp, %d, %s, %1$d, %1$s, etc ... 

M_MSG_0              "%1$s: Usage: %2$s [-fR] GroupName File ...\n"

M_MSG_1              "%s: A flag is not known.\n\
\tUsage: chgrp [-fR] GroupName File ...\n"
$ NOTE  
$ NOTE: In M_MSG_2, %1$s will be the name of the program and  
$ NOTE: %2$s will be the name of the group.  
$ NOTE   

M_MSG_2              "%1$s: %2$s is not a recognized group.\n\
\tCheck the /etc/group file for all known groups.\n"

M_MSG_3              "Cannot change group of %s."

M_MSG_4              "Cannot change to directory %s."
