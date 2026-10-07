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

PROGRAMS		= timed timedc
MSGHDRS                 = timed_msg.h
CATFILES                = timed.cat

ILIST			= timed timedc
IDIR			= /usr/sbin/

HFILES			= timed_msg.h
COM_OFILES		= byteorder.o measure.o 
timed_OFILES 		= acksend.o candidate.o correct.o master.o \
			  networkdelta.o readmsg.o slave.o timed.o logwtmp.o \
			  $(COM_OFILES)
timedc_OFILES		= cmds.o cmdtab.o timedc.o $(COM_OFILES)

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
