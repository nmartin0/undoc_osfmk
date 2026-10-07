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

$ @(#)$RCSfile: leave.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:29:10 $

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
$ leave.msg	1.4  com/cmd/oper,3.1,9013 2/28/90 16:16:31
$
$ messages for leave  
$quote "         define quote character.

$len 
$set MS_LEAVE        Define initial set#
$ Message numbers have been assigned a range of 144-150
$ Do not translate leave:, %d, %s, %1$d, %1$s, etc ...


WHEN                 "At what time do you need to leave?\n"
$ this message is when leave prompts for how long till the user needs to leave.  
$ it is not displayed if a time is given on the command line.  

PAST                 "leave: The requested time has already passed.\n"
$ this message is printed when the time entered by the user has already passed.  

USAGE                "Usage: leave [[+]hhmm]\n"
$ usage message  

FIVE                 "\07\07You have to leave in 5 minutes.\n"
$ this message is given five minutes before the specified time  

ONE                  "\07\07Just one more minute.\n"
$ this message is given one minute before the specified time  

TIME                 "\07\07It is time to leave.\n"
$ this message is given at the time to leave   

LATE                 "\07\07You are 1 minute late.\n"
$ this message is given 1 minute after the time to leave  

BYE                  "This is the last notice. You are now 2 minutes late. Bye.\n"
$ this message is given 2 minutes after the time to leave  

SET                  "The alarm is set for %s.\n"
$ %s is the return from ctime().  
