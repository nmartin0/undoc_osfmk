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

$ @(#)$RCSfile: apply.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:18:43 $
$
$
$ COMPONENT_NAME: (CMDSH) bourne shell and related commands  
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

$
$ messages for apply
$
$
$set MS_APPLY
$ Do not translate apply

USAGE "Usage: apply [-aCharacter] [-Number] Command Parameter ...\n"
TOOFEWARGS "apply: Specify a parameter after the %s command.\n"
$ %s will be some unix command that the user enters
NOSHELL "apply: Cannot run the environment $SHELL or the shell /bin/sh.\n"
$ Do not translate /bin/sh or $SHELL (it is an environment variable)
NOFORK "apply: The fork function failed.\n"
$ Do not translate fork
