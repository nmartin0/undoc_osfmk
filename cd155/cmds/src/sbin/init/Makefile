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

# @(#)Makefile	4.2 13:29:05 7/11/90 SecureWare

SEC_PROGRAMS		= init
PROGRAMS		= init
MSGHDRS			= init_msg.h
CATFILES		= init.cat

SEC_ILIST		= init
ILIST			= init
IDIR			= /sbin/

USE_STATIC_LIBRARIES	=

CFLAGS			= ${SEC_DEFS} #-DXDEBUG -DDEBUG1
HFILES			= ${MSGHDRS} init.h
OFILES			= init.o getcmd.o signals.o output.o \
			  ${SEC_DEFS?init_sec.o:}
LIBS			= ${SEC_LIBS}

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
