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

PROGRAMS		= telnet
MSGHDRS			= telnet_msg.h
CATFILES		= telnet.cat

ILIST			= telnet
IDIR			= /usr/bin/

CCTYPE			= writable_strings
CFLAGS			= -D_BSD  -DSRCRT -DKLUDGELINEMODE

HFILES			= telnet_msg.h defines.h externs.h general.h ring.h
OFILES			= commands.o main.o network.o ring.o sys_bsd.o \
			  telnet.o terminal.o tn3270.o utilities.o
LIBS			= -ltermlib -lbsd

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

commands.o sys_bsd.o telnet.o terminal.o: types.h
network.o sys_bsd.o tn3270.o: fdset.h
include ${MAKEFILEPATH}/depend.mk
-include Makedep
