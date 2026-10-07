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

#
# @(#) $RCSfile: Makefile,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 03:03:39 $
#

VPATH			= ..

LIBRARIES		= acctlib.a
MSGHDRS			= acct_msg.h

EXPLIB_TARGETS		=

INCFLAGS		= -I..
HFILES			= acctdef.h table.h
OFILES			= cat.o copyn.o devtolin.o eprintf.o \
			  extend.o hashuser.o lintodev.o namtouid.o \
			  pnpsplit.o substr.o tmless.o \
			  tmsecs.o uidtonam.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/libs.mk
include ${MAKEFILEPATH}/objects.mk

pnpsplit.o: acct_msg.h
include ${MAKEFILEPATH}/depend.mk
-include Makedep
