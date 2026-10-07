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
# (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
#
# OSF/1 Release 1.0.2

VPATH			= ../libc
INCFLAGS        = -I.

LIBRARIES		= libhippi.a

EXPLIB_TARGETS		= export_libhippi.a

ILIST			= libhippi.a
IDIR			= /usr/ccs/lib/
IMODE			= 644

CFLAGS			=
OFILES			= hippi_open.o hippi_memget.o hippi_memfree.o \
			  	  hippi_config.o hippi_bind.o hippi_read.o \
			  	  hippi_write.o hippi_close.o hippi_read_request.o \
				  hippi_read_complete.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/libs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
