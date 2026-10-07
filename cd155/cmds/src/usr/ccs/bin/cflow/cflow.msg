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
$ OSF/1 Release 1.0.2

$ @(#)$RCSfile: cflow.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:50:54 $
$
$ COMPONENT_NAME: (CMDPROG) Programming Utilities
$
$ FUNCTIONS: 
$
$ ORIGINS: 27
$
$ (C) COPYRIGHT International Business Machines Corp. 1988, 1989
$ All Rights Reserved
$ Licensed Materials - Property of IBM
$
$ US Government Users Restricted Rights - Use, duplication or
$ disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$
$ cflow.msg	1.6  com/cmd/prog/cflow/cflow1,3.1,9021 3/1/90 09:47:28

$len
$quote "         define quote character.

$set MS_DAG        Define initial set#
$
$ Do not translate cflow:, %s, %c.  

$
$ The following two message are accessed in cflow.sh by number
$ They should remain message numbers 1 and 2.
$
1         "cflow: File %s must have a .o, .s, .i, .c, .l, or .y extension.\n"
$ Do not translate .o, .s, .i, .c, .l, .y  
2    "Usage: cflow [-dNumber] [-i_] [-ip] [-ix] [-r] [-MA]\n\
\t[-NdNumber] [-NlNumber] [-NnNumber] [-NtNumber] File ...\n"


DBOPT    "cflow: %c is not a recognized flag or is not used correctly.\n"
$ %c is a flag.  
TMCHS    "cflow: There is not enough memory available to add more characters.\n"
TMNDS    "cflow: There is not enough memory available to add more nodes.\n"
TMLNKS   "cflow: There is not enough memory available to add more links.\n"
