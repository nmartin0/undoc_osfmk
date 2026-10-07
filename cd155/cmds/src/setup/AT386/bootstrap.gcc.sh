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
chmod u+w Makefile
chmod u+w config.gcc
chmod u+x ./config.gcc
chmod u+w ./move-if-change
chmod u+x ./move-if-change
chmod u+w c-parse.tab.c
chmod u+w cexp.c
touch c-parse.tab.c
touch cexp.c
./config.gcc i386-mach
chmod u+w toplev.c
ed toplev.c <<!
/HZ/
s/HZ/100/
w
q
.
!
ed Makefile  <<!
/^CC = cc/
s/^CC = cc/CC = gcc/
w
q
!

/bin/make

ed Makefile  <<!
/^CC = gcc/
s/^CC = gcc/CC = cc/
w
q
!
exit 0
