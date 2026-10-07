#! /bin/sh
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
# (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
# OSF/1 Release 1.0.2

# @(#)$RCSfile: diction.sh,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:22:35 $

#	diction.sh	4.5	(Berkeley)	82/11/06
#

D=/usr/bin
B=/usr/lbin
echo $*
rest=
flag=
nflag=
mflag=-me
lflag=-ml
kflag=
file=
for i
do case $i in
 -f) flag=-f;shift; file=$1; shift; continue;;
-n) nflag=-n;shift; continue;;
-k) kflag=-k;shift; continue;;
 -mm) mflag=$1; shift; continue;;
-ms) mflag=$1;shift;continue;;
-me) mflag=$1;shift;continue;;
-ma) mflag=$1;shift;continue;;
-ml) lflag=$1;shift;continue;;
*) rest=$*; break;;
esac
done
 $D/deroff $kflag $lflag $mflag $rest^$B/dprog -d $nflag $flag $file
