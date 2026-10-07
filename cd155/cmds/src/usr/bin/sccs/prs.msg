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
$ @(#)$RCSfile: prs.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:37:50 $
$
$ IBM CONFIDENTIAL
$ Copyright International Business Machines Corp. 1989
$ Unpublished Work
$ All Rights Reserved
$ Licensed Material - Property of IBM
$
$ US Government Users Restricted Rights - Use, duplication or
$ disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
$
$       prs.msg 1.4 com/cmd/sccs/hdr,3.1,9021 4/3/90 17:02:31
$
$quote "         define quote character.

$len 
$ Do not translate prs:
$set 9        Define initial set#

DEFLINE              ":Dt:\t:DL:\n\
MRs:\n\
:MR:COMMENTS:\n\
:C:"
$ Do not translate anything on this line except for Modification Requests and 
$ Comments. Everything else will not be seen by the user and must remain 
$ exactly as it is. 
INVCUTOFF "\nprs: Specify the cutoff date in the form: yy[mm[dd[hh[mm[ss]]]]]\n"
$ yy is replaced by a 2 digit number for year. 
$ mm is replaced by a 2 digit number for month. 
$ dd is replaced by a 2 digit number for day. 
$ hh is replaced by a 2 digit number for hour. 
$ mm is replaced by a 2 digit number for minute. 
$ ss is replaced by a 2 digit number for seconds. 
$ Note to translators--Do not change the order of the format. 
$ Do not translate (prs4). 
CANTSPECDATE "\nprs: Cannot specify both a cutoff date and an SID.\n"
$ Do not translate (prs5). 
$ Do not translate SID. 
SPECEORLWITHC "\nprs: Specify the -e flag or the -l flag with the -c flag.\n"
$ Do not translate -e, -l, or -c. They are flags. 
$ Do not translate (prs6) 
NONEMSG "none"
YESMSG  "yes"
NOMSG   "no"
BRANCH  "branch\n"
CEILING "ceiling\t%s\n"
$ %s is the release number. 
DEFAULTSID   "default SID\t%s\n"
$ Do not translate SID. 
$ %s is the default SID number. 
FLOOR        "floor\t%s\n"
$ %s is the release number. 
IDKEYWRDERR  "id keywd err/warn\n"
JOINTEDIT    "joint edit\n"
$ A joint edit is a simultaneous edit. 
LOCKRLS      "locked releases\t%s\n"
$ %s is the locked release or releases. 
MODULE       "module\t%s\n"
$ %s is the module name. 
NULLDELTA    "null delta\n"
$ A null delta is an empty delta. 
CSECTNAME    "csect name\t%s\n"
$ %s is text. 
TYPE         "The  keyword is replaced by \t%s.\n"
$ Do not translate . 
$ %s is a type. 
VALIDATE      "The program that validates Modification Requests is\t"
