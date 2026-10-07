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

VPATH			= ..:../lvmcmdhdr

LIBRARIES		= lvmcmdlib.a
MSGHDRS			= lvm_msg.h

USE_STATIC_LIBRARIES	=

INCFLAGS		= -I../lvmcmdhdr
CFLAGS			= -UDEBUG
HFILES			= debug.h dump.h logvol.h lvmcmds.h lvmdefmsg.h \
			  lvmtab.h maps.h options.h user_interf.h utilities.h \
			  ${MSGHDRS}
OFILES			= options.o user_interf.o utilities.o debug.o\
			  lvmtab.o logvol.o dump.o maps.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/libs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
