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
# Revision 1.3  1995/02/02  00:21:33  bolsen
#  Reviewer(s): Jerry Toman
#  Risk: Medium (lots of files)
#  Module(s): Too many to list
#  Configurations built: STD, LITE, & RAMDISK
#
#  Added or Updated the Locus Copyright message.
#
# Revision 1.2  1994/11/18  20:57:28  mtm
# Copyright additions/changes
#
# Revision 1.1  1994/07/09  00:50:57  slk
# Initial installation of 3480 Tape tools.
#
#  Reviewer:
#  Risk: Low, only in user tree (not generally used)
#  Benefit or PTS #: Support 3480 tape tools
#  Testing: 3480 tape tests.
#  Module(s):
#
# $EndLog$ 
# 

default: all

#
# Include file and library search paths.
#
CPATH = $(TARGET_CPATH)
LPATH = $(TARGET_LPATH):../../lib/tnc
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

INCS = \
	-I.\
	-I$(MAKETOP)/server \
	-I$(MAKETOP)/server/include \
	-I$(MAKETOP)/usr/include \
	-I$(MAKETOP)/usr/include/$(TARGET_MODEL) \
	-I$(MAKECONF:h)/server \
	-I$(MAKECONF:h)/server/$(cputype) \
	-I$(MKINC)

DIR_TARGETS = \
	charperf \
	chartest \
	extpperf \
	extptest \
	extzperf \
	extztest \
	floatest \
	floatprf \
	itointst \
	itoiperf \
	itoitest \
	packperf \
	packtest \
	zoneperf \
	zonetest


CLEANLIST = $(DIR_TARGETS)


all: $(DIR_TARGETS)
	for i in $(DIR_TARGETS); do \
		if [ -d $(MAKECONF:h)/$(MAKESUB)/$$i ] ; then \
			if [ ! -d $$i ] ; then \
				mkdir $$i; \
			else \
				true; \
			fi; \
			( cd $$i; $(MAKE); ) ; \
		else \
			true; \
		fi; \
	done


$(FILE_TARGETS): $$@.c
	$(CC) -c -DTNC $(INCS) $@.c
	rm -f _cvs_id.c _cvs_id.o
	mkidinfo -f $@
	$(CC) -c _cvs_id.c
	$(CC) _cvs_id.o $@.o -o $@ -ltnc

clean:
	rm -rf $(CLEANLIST) _cvs_id.o _cvs_id.c

print_env:
	@echo DIR_TARGETS are $(DIR_TARGETS)
	@echo FILE_TARGETS are $(FILE_TARGETS)
	@echo env MAKECPP is $$MAKECPP
	@echo MAKECPP is $(MAKECPP)
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
