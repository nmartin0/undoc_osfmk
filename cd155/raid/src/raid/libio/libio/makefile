# $Copyright
# Copyright 1992, 1993, 1994, 1995 Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
include ../../oemsrc/src/Uv4Mk.incl
CC	=	icc
CFLAGS =	-I../h -I../../oemsrc/src/Common/Include \
		-I../../oemsrc/include/sys/scsi -DPARAGON860

LIBIO =		../../oemsrc/lib/Uv4/libio.a

$(LIBIO):	libio.o
		rm -rf $@
		$(AR) r $@ libio.o

libio.o:	libio.c \
		../h/libio.h \
		../../oemsrc/src/Common/Include/cdb_struct.h

libio.h:	../../oemsrc/include/sys/scsi/SCSI.h
