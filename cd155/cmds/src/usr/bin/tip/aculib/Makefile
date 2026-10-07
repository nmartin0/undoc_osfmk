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

VPATH			= ..

LIBRARIES		= aculib.a
MSGHDRS                 = tip_msg.h
CATFILES                = tip.cat

CFLAGS			= -D_BSD
INCFLAGS		= -I..
HFILES                  = tip.h tip_msg.h
OFILES			= biz22.o biz31.o courier.o df.o dn11.o hayes.o \
			  ventel.o v831.o v3451.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/libs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
