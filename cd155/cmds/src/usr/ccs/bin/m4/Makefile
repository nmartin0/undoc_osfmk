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

PROGRAMS		= m4
MSGHDRS                 = m4_msg.h
CATFILES                = m4.cat

ILIST			= m4
IDIR			= /usr/ccs/bin/

OFILES			= m4.o m4y.o m4ext.o m4macs.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

m4.o m4ext.o m4macs.o: m4.h m4_msg.h
include ${MAKEFILEPATH}/depend.mk
-include Makedep
