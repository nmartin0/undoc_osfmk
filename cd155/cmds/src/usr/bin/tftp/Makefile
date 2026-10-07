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

PROGRAMS		= tftp
MSGHDRS			= tftp_msg.h
CATFILES		= tftp.cat

ILIST			= tftp
IDIR			= /usr/bin/

CFLAGS			= -D_BSD
HFILES			= tftp_msg.h
OFILES			= main.o tftp.o tftpsubs.o
LIBS			= -lbsd

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
