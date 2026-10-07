# $Copyright
# Copyright 1992, 1993, 1994, 1995 Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
include ../../oemsrc/src/Uv4Mk.incl
CC      =       icc
CFLAGS =        -I../h -I../../oemsrc/src/Common/Include \
		-I../../oemsrc/include/sys/scsi -DPARAGON860
#CFLAGS =	-DSUPERUSER
LIBIO  =	../../oemsrc/lib/Uv4/libio.a

inquire:	inquire.o $(LIB) ../h/passthru.h
	$(CC) -g $(CFLAGS) -o $@ inquire.o $(LIBIO)

scsi:	scsi.o $(LIB) ../h/passthru.h
	$(CC) $(CFLAGS) -o $@ scsi.o $(LIBIO)

scsicmd:	scsicmd.o $(LIB) ../h/passthru.h
	$(CC) -g $(CFLAGS) -o $@ scsicmd.o $(LIBIO)

scrub:	scrub.o $(LIB) ../h/passthru.h
	$(CC) -g $(CFLAGS) -o $@ scrub.o $(LIBIO) -lnx

raidfix:	raidfix.o $(LIB) ../h/passthru.h
	$(CC) -g $(CFLAGS) -o $@ raidfix.o $(LIBIO)

scsi.o:	scsi.c

scsicmd.o:	scsicmd.c

scrub.o:	scrub.c

inquire.o:	inquire.c
