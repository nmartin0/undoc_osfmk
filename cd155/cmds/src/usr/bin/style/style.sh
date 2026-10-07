#!/bin/sh
# 
# $Copyright
# Copyright 1993, 1994, 1995  Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
#
# (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
#
# OSF/1 Release 1.0

#
# @(#)style.sh	1.9 3/3/90 14:41:22
#
# COMPONENT_NAME: (CMDTEXT) Text Formatting Services
#
# FUNCTIONS:
#
# ORIGINS: 26,27,28
#
# (C) COPYRIGHT International Business Machines Corp. 1989
# All Rights Reserved
# Licensed Materials - Property of IBM
#
# US Government Users Restricted Rights - Use, duplication or
# disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
#
#
#	style.sh	4.5	(Berkeley)	82/11/06
#
L=/usr/lib
#B=/usr/bin
echo " " $*
sflag=-s
eflag=
Pflag=
nflag=
lflag=
lcon=
rflag=
rcon=
mflag=-me
mlflag=-ml
kflag=
for i in $*
do case $i in
-r) rflag=-r; shift; rcon=$1;shift;continue;;
-l)lflag=-l; shift; lcon=$1;shift;continue;;
-mm) mflag=-mm;shift;continue;;
-ms) mflag=-ms;shift;continue;;
-me) mflag=-me;shift;continue;;
-ma) mflag=-ma;shift;continue;;
-li|-ml) mlflag=-ml;shift;continue;;
+li|-tt)mlflag=;shift;continue;;
-p) sflag=-p;shift;continue;;
-a) sflag=-a;shift;continue;;
-e) eflag=-e;shift;continue;;
-P) Pflag=-P;shift;continue;;
-n) nflag=-n;shift;continue;;
-N) nflag=-N;shift;continue;;
-k) kflag=-k;shift;continue;;
-flags) dspmsg style.cat 1 "%s [-flags] [-r num] [-l num] [-e] [-p] [-n] [-N]\
 [-a] [-P] [-mm|-ms] [-li|+li] [file ...]\n" $0;exit;;
-*) dspmsg style.cat 2 "unknown style flag %s\n" $i; exit;;
*) break;;
esac
done
#$B/deroff $kflag $mflag $mlflag $*^$L/style1^$L/style2^$L/style3 $rflag $rcon $lflag $lcon $sflag $nflag $eflag $Pflag

