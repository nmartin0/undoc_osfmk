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
$ @(#)$RCSfile: rmchg.msg,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:37:51 $
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
$       rmchg.msg 1.4 com/cmd/sccs/hdr,3.1,9021 3/24/90 16:03:59
$
$quote "         define quote character.

$ Do not translate 
$ Do not translate the parenthesis (rc#) at the end of each message.
$len 
$set 10        Define initial set#

MISSINGR   "\nUse the -r flag with this command.\n"
$ Do not translate -r
NOTOWNER   "\nOnly the owner of the file or %s can use this command.\n"
$ %s is the creator of the delta. 
NOTLEAF    "\nOnly the most recently created delta on a branch.\n"
DELTSPEC   "\nCannot remove this delta or change its commentary.\n"
BADINVOKE  "\nCannot invoke rmdel or cdc except by the commands rmdel or cdc.\n"
RNOSID     "\nSpecify an SID with the -r flag.\n"
$ Do not translate -r
BEINGEDIT  "\nAnother user is working on the specified delta.\n"
NOTCOMCMR  "\nCASSI not compatible with CMRs on command line.\n"
$ Do not translate. This message will not be seen by the user. 
INVUSEZFLG "\nInvalid use of 'z' flag. CASSI incompatiblity.\n"
$ Do not translate. This message will not be seen by the user. 
CNTRLFIL   "\nUneditable CMR for CASSI controlled file.\n"
$ Do not translate. This message will not be seen by the user. 
BADRPLC    "\nBad cdc replace of CMRs (CASSI).\n"
$ Do not translate. This message will not be seen by the user. 
NOCHNG     "\nOnly one CMR left for this delta no change possible.\n"
$ Do not translate. This message will not be seen by the user. 
DELETEYN   "\nDo you want to delete %1$s (%2$s or %3$s)?\n"
$ Do not translate. This message will not be seen by the user. 
DELETEIT   "\Do you want to delete %s?\n"
$ Do not translate. This message will not be seen by the uesr. 
NOTEDITBL  "%s is not editable now.\n"
$ Do not translate. This message will not be seen by the user. 
NOFREDF    "\nNo fred file found- see sccs administrator.\n"
$ Do not translate. This message will not be seen by the user. 
NTEDITBLE  "\n%s is not editable.\n"
$ Do not translate. This message will not be seen by the user. 
MSGNTWRTBL "\nCASSI msg not writable.\n"
$ Do not translate. This message will not be seen by the user. 
CASSIBD    "\nCASSI BD/source not writable.\n"
$ Do not translate. This message will not be seen by the user. 
CHANGED    "** CHANGED ** "
