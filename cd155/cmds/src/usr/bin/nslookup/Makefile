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

PROGRAMS		= nslookup
MSGHDRS			= nslookup_msg.h
CATFILES		= nslookup.cat

ILIST			= nslookup
IDIR			= /usr/bin/

CCTYPE			= writable_strings
CFLAGS			= -D_BSD
HFILES			= nslookup_msg.h res.h
OFILES			= getinfo.o commands.o main.o skip.o debug.o list.o \
			  send.o subr.o 
LIBS			= -lbsd -ll -lresolv
GARBAGE			= commands.c

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
