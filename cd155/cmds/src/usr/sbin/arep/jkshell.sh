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
 

list=`ls *.c`
	for i in $list
	do
		echo "Processing $i"
		cp $i jk.$i
		cvs update -rR1_2 $i
		chmod 777 $i
		cp jk.$i $i
		rm jk.*
		cvs commit -rR1_2 $i
		diff $i $R11SRC/usr/ccs/lib/libmacs/$i

	done
cvs status *.c
exit 0
