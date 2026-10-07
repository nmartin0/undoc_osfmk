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
# Revision 1.3  1995/02/01  23:58:22  bolsen
#  Reviewer(s): Jerry Toman
#  Risk: Medium (lots of files)
#  Module(s): Too many to list
#  Configurations built: STD, LITE, & RAMDISK
#
#  Added or Updated the Locus Copyright message.
#
# Revision 1.2  1994/11/18  20:54:00  mtm
# Copyright additions/changes
#
# Revision 1.1  1994/07/09  00:15:33  slk
# Initial installation of 3480 tape tools
# library support for the i860.
#
#  Reviewer:
#  Risk: Low, effects only user tree (not generally built)
#  Benefit or PTS #: Support for 3480 tape tools
#  Testing: Ran test suites also installed at this time
#  Module(s):
#
# $EndLog$
#

default: all

#
# Include file and library search paths.
#
CPATH = $(TARGET_CPATH)
LPATH = $(TARGET_LPATH):.
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
#Directories to search for required source files, other than the
#obvious ".".  We have some source files in a machine-dependent
#directory so we search it.  Type "make print_env" to see VPATH.
#
#Trailing spaces on the VPATH definition line may break VPATH.
VPATH=$(target_cpu)

OBJS1 = \
	tio_init.o tread.o chk_ird.o twrite.o \
	tapedata.o tio_exit.o sync_tio.o \
	tlocblk.o tgetblk.o tio_util.o \
	convert.o talloc_.o tdsopen.o \
	topenvol.o therest.o tioctl.o\
	tread_.o twrite_.o \
	tgetblk_.o tlocblk_.o dbglabel.o \
	tdalloc_.o tdsopen_.o tdsclos_.o \
	feov_.o

OBJS2 = \
	conv_dat.o cnv_atoe.o cnv_etoa.o \
	ztoi.o itoz.o uitoa.o itoi.o \
	itoin.o float.o itop.o ptoa.o \
	ptoi.o ztoa.o ztoei.o ptoei.o \
	eitoz.o eitop.o cnvatoe_.o \
	 cnvetoa_.o eitop_.o eitoz_.o \
	float_.o itoi_.o itoin_.o \
	itop_.o itoz_.o ptoei_.o \
	ptoi_.o ztoei_.o ztoi_.o ptoa_.o \
	ztoa_.o

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
INC11 = -I$(SOURCECWD)/include

INCS = $(INC1) $(INC2) $(INC3) $(INC4) $(INC5) $(INC6) \
	$(INC7) $(INC8) $(INC9) $(INC10) $(INC11)

CLEANLIST = $(OBJS) sys $(OBJS/\.o/.as/)


all: tape3480.a conv.a lib3480.a 

ct:	ct.c lib3480.a
	$(CC) $(INCS) -o ct ct.c -l3480

dt:	dt.c lib3480.a
	$(CC) $(INCS) -o dt dt.c -l3480

ms:	ms.c lib3480.a
	$(CC) $(INCS) -o ms ms.c -l3480 

lib3480.a:  $(CONVOBJ) $(TAPEOBJ)
	$(AR) r lib3480.a $(OBJS1) $(OBJS2)
	$(RANLIB) lib3480.a

tape3480.a: $(OBJS1)
	$(AR) r tape3480.a $(OBJS1)
	$(RANLIB) tape3480.a

conv.a: $(OBJS2)
	$(AR) r conv.a $(OBJS2)
	$(RANLIB) conv.a

$(OBJS1): sys/syscall.h

$(OBJS2): sys/syscall.h

sys/syscall.h: sys ../../../server/conf/makesyscalls.sh \
			../../../server/conf/syscalls.master
	sh ../../../server/conf/makesyscalls.sh -h \
			../../../server/conf/syscalls.master
	mv syscall.h sys/syscall.h

sys:
	mkdir sys

.c.o:
	$(CC) -D_KERNEL -DTNC -c $(INCS) $*.c

.s.o:
	$(CPP) -DTNC -E $(INCS) -Ui386 -D$(target_cpu) $(DEFINES) \
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
