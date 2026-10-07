# 
# $Copyright
# Copyright 1993, 1994 , 1995 Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
#
# Copyright (c) 1992-1995, Locus Computing Corporation
# All rights reserved
#
# HISTORY
# $Log: Makefile,v $
# Revision 1.9  1995/02/02  00:00:00  bolsen
#  Reviewer(s): Jerry Toman
#  Risk: Medium (lots of files)
#  Module(s): Too many to list
#  Configurations built: STD, LITE, & RAMDISK
#
#  Added or Updated the Locus Copyright message.
#
# Revision 1.8  1994/11/18  20:56:07  mtm
# Copyright additions/changes
#
# Revision 1.7  1994/03/14  17:48:30  slk
# Checkpoint Restart Code Drop
#  Reviewer: Chris Peak, chrisp@locus.com
#  Risk: Low
#  Benefit or PTS #: Enhancement
#  Testing: Locus VSTNC, individual checkpoint restart by hand
#  Module(s):
#
# Revision 1.6  1993/07/14  18:51:53  cfj
# OSF/1 AD 1.0.4 code drop from Locus.
#
# Revision 1.5  1993/05/06  19:30:22  stefan
# ad103+tnc merged with Intel code.
#
# Revision 1.1.1.4  1993/07/01  21:18:49  cfj
# Adding new code from vendor
#
# Revision 1.4  1993/01/28  16:48:37  dleslie
# New version string scheme, using mkidinfo
#
# Revision 1.3  1992/11/30  23:02:42  dleslie
# Copy of NX branch back into main trunk
#
# Revision 1.1.2.2  1992/11/06  20:36:40  dleslie
# Merged bug drop from Locus November 3, 1992, with NX development
#
# Revision 1.1.2.1  1992/11/06  00:13:12  dleslie
# Local changes for NX through noon, November 5, 1992.
#
# Revision 1.1.1.2  1993/05/03  17:58:23  cfj
# Initial 1.0.3 code drop
#
# Revision 3.6  1992/10/28  16:06:47  roman
# Add new TNC-specific system call table_node() to library.
#
# Revision 3.5  92/05/01  17:21:04  yazz
# Added -I. so locally built sys/syscall.h will be found.
# 
# Revision 3.4  92/05/01  15:38:11  yazz
# Re-groove to make both i386 and i860 work.
# Also defined $(SOURCECWD) which points to the current source
# directory -- handy for -I and other options.  (Bob Yazz)
# 
#
# Revision 3.3  92/04/22  15:26:34  yazz
# Replaced Makefile for machine-dependent libtnc.a files, using the
# proper VPATH.  Also built the syscall.h file using the standard
# script that the emulator and the server use.
#
# Revision 3.2  92/04/16  21:04:15  yazz
# Respect i386 specificness of these assembly files.
#
# Revision 3.1  92/04/14  14:56:44  klh
# Fix RCS comments
#
# $EndLog$
#

default: all

#
# Include file and library search paths.
#
CPATH = $(TARGET_CPATH)
LPATH = $(TARGET_LPATH)
.EXPORT: CPATH LPATH

#
# i860 Support
#
CC = $(DOI860?icc:cc)
CPP = $(DOI860?cc:cc)	# icc's C pre-processor does not work here
LD = $(DOI860?ld860:ld)
AS = $(DOI860?as860:as)
AR = $(DOI860?ar860:ar)
RANLIB = $(DOI860?ranlib860:ranlib)

#
# It's a hassle getting a macro that defines the pathname of the
# actual source directory, since make doesn't define one.  (Why?)
# We construct one using the pathname of the Makeconf file, taking
# its head (directory part including slash, less trailing filename),
# then appending $(MAKESUB) which is the relative pathname from the
# Makeconf file to the current source directory.
#
SOURCECWD = $(MAKECONF:h)$(MAKEDIR)

#
#	Directories to search for required source files, other than the
#	obvious ".".  We have some source files in a machine-dependent
#	directory so we search it.  Type "make print_env" to see VPATH.
#
#	Trailing spaces on the VPATH definition line may break VPATH.
VPATH=$(target_cpu)

OBJS = \
	rexecl.o \
	rexecle.o \
	rexeclp.o \
	rexecv.o \
	rexecvp.o \
	get_tnc_port.o \
	kill3.o \
	migrate.o \
	node_self.o \
	rexecve.o \
	rfork.o \
	rforkmulti.o \
	set_tnc_port.o \
	table_node.o \
	chkpnt_restart_traps.o \
	chkpnt_restart.o

INC1 = -I.
INC2 = -I$(SOURCECWD)
INC3 = -I$(SOURCECWD)/$(target_cpu)
INC4 = -I$(SOURCECWD)/../../../server
INC5 = -I../../../server
INC6 = -I$(SOURCECWD)/../../../usr/include
INC7 = -I$(SOURCECWD)/../../../usr/include/$(TARGET_MODEL)
INC8 = -I$(MKINC)
INC9 = -I$(SOURCECWD)/server
INC10 = -I$(SOURCECWD)/../../../server/include
INCS = $(INC1) $(INC2) $(INC3) $(INC4) $(INC5) $(INC6) \
	$(INC7) $(INC8) $(INC9) $(INC10)

CLEANLIST = $(OBJS) libtnc.a sys $(OBJS/\.o/.as/)


all: libtnc.a

libtnc.a: $(OBJS)
	mkidinfo -f libtnc
	$(CC) -c _cvs_id.c
	rm -f libtnc.a _cvs_id.c
	$(AR) r libtnc.a _cvs_id.o $(OBJS)
	$(RANLIB) libtnc.a

$(OBJS): sys/syscall.h

sys/syscall.h: sys ../../../server/conf/makesyscalls.sh \
			../../../server/conf/syscalls.master
	sh ../../../server/conf/makesyscalls.sh -h \
			../../../server/conf/syscalls.master
	mv syscall.h sys/syscall.h

sys:
	mkdir sys

.c.o:
	$(CC) -D_KERNEL -DTNC -DOSF1_ADFS -DCHKPNT -c $(INCS) $*.c

.s.o:
	$(CPP) -DTNC -DCHKPNT -E $(INCS) -Ui386 -D$(target_cpu) $(DEFINES) \
		-DASSEMBLER $*.s > $*.as
	$(AS) $(ASFLAGS) -o $*.o $*.as
	rm -f $*.as

clean:
	rm -rf $(CLEANLIST)

print_env:
	@echo SOURCECWD is $(SOURCECWD)
	@echo MAKEDIR is $(MAKEDIR)
	@echo MAKESUB is $(MAKESUB)
	@echo MAKETOP is $(MAKETOP)
	@echo VPATH is $(VPATH)
	@echo CPATH is $(CPATH)
	@echo LPATH is $(LPATH)
	@echo LOCAL_CPATH is $(LOCAL_CPATH)
	@echo LOCAL_LPATH is $(LOCAL_LPATH)
	@echo TARGET_CPATH is $(TARGET_CPATH)
	@echo TARGET_LPATH is $(TARGET_LPATH)
