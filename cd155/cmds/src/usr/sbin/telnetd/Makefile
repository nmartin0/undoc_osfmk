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

PROGRAMS		= telnetd
MSGHDRS			= telnetd_msg.h
CATFILES		= telnetd.cat

ILIST			= telnetd
IDIR			= /usr/sbin/

CCTYPE			= writable_strings
CFLAGS			= -D_BSD -DDIAGNOSTICS
HFILES			= telnetd_msg.h defs.h ext.h pathnames.h telnetd.h
OFILES			= telnetd.o state.o termstat.o slc.o sys_term.o \
			  utility.o global.o get_date.o gettytab.o
LIBS			= -ltermlib -lutil -lbsd

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
