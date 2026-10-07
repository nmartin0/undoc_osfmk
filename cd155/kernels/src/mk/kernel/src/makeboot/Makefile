# 
# $Copyright
# Copyright 1991 , 1994, 1995 Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
#
# Mach Operating System
# Copyright (c) 1990 Carnegie-Mellon University
# All rights reserved.  The CMU software License Agreement specifies
# the terms and conditions for use and redistribution.
#
#
# HISTORY
# $Log: Makefile,v $
# Revision 1.5  1994/11/18  21:01:54  mtm
# Copyright additions/changes
#
# Revision 1.4  1993/06/30  22:59:20  dleslie
# Adding copyright notices required by legal folks
#
# Revision 1.3  1993/04/27  20:53:24  dleslie
# Copy of R1.0 sources onto main trunk
#
# Revision 1.1.10.2  1993/04/22  18:59:15  dleslie
# First R1_0 release
#
# Revision 2.2  91/05/08  13:09:21  dbg
# 	Created.
# 	[90/10/01            dbg]
# 
# Revision 2.1.1.1  91/02/26  11:16:58  dbg
# 	Created.
# 	[90/10/01            dbg]
# 

all:		makeboot.x

install:	makeboot.install

makeboot.install: $(DDIR)/makeboot

$(DDIR)/makeboot: makeboot.x
	cp -p makeboot.x $(DDIR)/makeboot
	strip $(DDIR)/makeboot
	chmod 755 $(DDIR)/makeboot

OBJS =	build_boot.o

# Base of source tree.  Look here for include files.
#IDIR = ../..

PMAXCFLAGS = -Dvoid=int
SUNCFLAGS = -Dvoid=int
#CFLAGS= $(CDEBUGFLAGS) $($(MACHINE)CFLAGS) -I$(IDIR)
CFLAGS= $(CDEBUGFLAGS) $($(MACHINE)CFLAGS)

makeboot.x: $(OBJS)
	$(CC) -o makeboot.x $(CFLAGS) $(OBJS) -lmach

.c.o:
	$(CC) -c -MD $(CFLAGS) $*.c
	md -u Makedep -d $*.d

OBJBASE = $(MAKECONF:h)/../../../obj/@sys/${BCSBBASE?$(BCSBBASE:t):latest}/kernel
BINBASE = $(MAKECONF:h)/../../../release/@sys/${BCSBBASE?$(BCSBBASE:t):latest}/bin

release:
	-if [ -f $(BINBASE)/makeboot ]; then true; else ln -s $(OBJBASE)/bin/makeboot $(BINBASE); fi

-include Makedep
