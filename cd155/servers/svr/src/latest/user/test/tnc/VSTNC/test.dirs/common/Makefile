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
# Revision 1.4  1995/02/02  00:31:20  bolsen
#  Reviewer(s): Jerry Toman
#  Risk: Medium (lots of files)
#  Module(s): Too many to list
#  Configurations built: STD, LITE, & RAMDISK
#
#  Added or Updated the Locus Copyright message.
#
# Revision 1.3  1994/11/18  21:05:48  mtm
# Copyright additions/changes
#
# Revision 1.2  1994/02/04  01:41:43  slk
#  Reviewer: Brent Olsen
#  Risk: Low
#  Benefit or PTS #: 7176	and update VSTNC.
#  Testing: Built, Ran VSTNC
#  Module(s):
#
# Revision 3.3  93/03/01  13:25:15  jpaul
# Add $DEFS for -DOSF1_ADFS to take correct table.h includes.
# 
# Revision 3.2  92/11/14  16:49:59  yazz
# Copyright added.
# 
# Revision 3.1  92/11/11  16:54:19  yazz
# cmu make
# 
# Makefile for VSTNC common.o routines
#

default: all

CPATH = $(TARGET_CPATH)
LPATH = $(TARGET_LPATH)
.EXPORT: CPATH LPATH

#
# i860 Support
#
CC = $(DOI860?icc:cc)
LD = $(DOI860?ld860:ld)
AS = $(DOI860?as860:as)
AR = $(DOI860?ar860:ar)

#
# Note that $(MAKETOP) is the top of the object tree, and
# $(MAKECONF:h) is the top of the source tree.  (If there were
# no separate object tree then these would be identical.)
# $(MKINC), the microkernel include files, is defined in Makeconf.
#

DEFS = -DTNC -DOSF1_ADFS

INCS = \
	-I.\
	-I$(MAKETOP)server \
	-I$(MAKETOP)server/include \
	-I$(MAKECONF:h)/server \
	-I$(MAKECONF:h)/server/$(cputype) \
	-I$(MKINC)

all: common.o

common.o:	common.c vstnc.h
	$(CC) -c $(INCS) $(DEFS) common.c -o common.o


clean:
	rm -f common.o


print_env:
	@echo make MKINC is $(MKINC)
	@echo make SVRINC is $(SVRINC)
	@echo make SVRINC_MACHINE is $(SVRINC_MACHINE)
	@echo env MKINC is $$MKINC
	@echo env SVRINC is $$SVRINC
	@echo env SVRINC_MACHINE is $$SVRINC_MACHINE
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
	@echo DEFS is $(DEFS)
