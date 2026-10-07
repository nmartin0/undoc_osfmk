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

PROGRAMS		= talk
MSGHDRS			= talk_msg.h
CATFILES		= talk.cat

ILIST			= talk
IDIR			= /usr/bin/
IGROUP			= ${TTY_GROUP}
IMODE			= 2711

HFILES			= talk.h talk_ctl.h talk_msg.h
OFILES			= talk.o get_names.o display.o io.o ctl.o init_disp.o \
			  msgs.o get_addrs.o ctl_transact.o invite.o look_up.o 
LIBS			= -lcurses

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
