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
# OSF/1 Release 1.0

PROGRAMS		= gated
MSGHDRS                 = gated_msg.h
CATFILES                = gated.cat
GARBAGE			= vers.c Version

ILIST			= gated
IDIR			= /usr/sbin/

#CCTYPE			= writable_strings
#CFLAGS			= -D_BSD -DEGP_UPDATE_FORMAT -UNSS -DGATEWAY -DAGENT_SNMP
CFLAGS			= -D_BSD -DEGP_UPDATE_FORMAT -UNSS -DGATEWAY
HFILES			= ${MSGHDRS} af.h egp_param.h include.h \
			  rt_control.h defs.h hello.h rip.h rt_table.h \
			  egp.h if.h routed.h trace.h
OFILES			= af.o egp.o ext.o hello.o if.o icmp.o inet.o \
			  init.o main.o rip.o rt_control.o rt_egp.o \
			  rt_if.o rt_init.o rt_nss.o rt_table.o  \
			  sgmp.o snmp.o str.o trace.o vers.o
LIBS                    = -lbsd

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

vers.c: newvers.sh version
	sh newvers.sh version
include ${MAKEFILEPATH}/depend.mk
-include Makedep
