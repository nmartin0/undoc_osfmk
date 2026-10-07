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
$ @(#)$RCSfile: get.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:37:47 $
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
$       get.msg 1.4 com/cmd/sccs/hdr,3.1,9021 3/24/90 16:02:56
$
$quote "         define quote character.

$ Do not translate get:
$ Do not translate the paranthesis (ge#) at the end of each line.
$len 
$set 7        Define initial set#

ENOTWITHM "\nget: Cannot use the -e flag with the -m flag.\n"
$ Do not translate -e, -m
WRITFILEEXST "get: A writable version of %s exists.\n"
ALRDYINC     "\nget: The delta %s is already included.\n"
$ %s is a delta. 
ALRDYEXCLD    "\nget: The delta %s is already excluded.\n"
$ %s is a delta. 
INTRNERR11    "\nget: There is an internal software error.\n"
BNGEDT        "\nget: Another user is editing %s.\n"
WARNMSG       "WARNING: The file %s is being edited.\n"
SERNOTOOLRG   "\nget: The specified serial number does not exist.\n"
$ you can do a get -a to specify delta serial number 
$ instead of SID number.  This is not documented in commands reference. 
$ By the way, the serial number overrides the SID number 
CMRTOOLNG     "\nget: The CMR list is too long.\n"
$ Do not translate this message. The user will never see it. 
ULINES        "%u lines\n"
$ %u is the number of lines. 
BADSIDCALC    "\nget: There is a bad SID calculated in newsid().\n"
$ Do not translate this message. The user will never see it. 
NEWDELTA      "new delta %s\n"
$ %s is the new delta. 
CKCASSINOCMR  "\nget: Background CASSI get with no CMRs.\n"
$ Do not translate this message. The user will never see it. 
BKCASSIINVCMR "\nget: Background CASSI get with invalid CMR.\n"
$ Do not translate this message. The user will never see it. 
COMMACMR      "\nget: Input Comma Separated List of CMRs: "
$ Do not translate this message. The user will never see it. 
TOOMANYCMR    "\nget: Too many CMRs.\n"
$ Do not translate this message. The user will never see it. 
ONEVALCMR     "\nget: Must enter at least one valid CMR.\n"
$ Do not translate this message. The user will never see it. 
REENTCMR      "\nget: Re-enter invalid CMRs, or press return.\n"
$ Do not translate this message. The user will never see it. 
