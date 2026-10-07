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
# Revision 1.10  1995/02/02  00:22:59  bolsen
#  Reviewer(s): Jerry Toman
#  Risk: Medium (lots of files)
#  Module(s): Too many to list
#  Configurations built: STD, LITE, & RAMDISK
#
#  Added or Updated the Locus Copyright message.
#
# Revision 1.9  1994/11/18  21:04:49  mtm
# Copyright additions/changes
#
# Revision 1.8  1994/03/14  17:50:35  slk
# Checkpoint Restart Code Drop
#  Reviewer: Chris Peak, chrisp@locus.com
#  Risk: Low
#  Benefit or PTS #: Enhancement
#  Testing: Locus VSTNC, individual checkpoint restart by hand
#  Module(s):
#
# Revision 1.7  1994/02/04  01:39:09  slk
#  Reviewer: Brent Olsen
#  Risk: Low
#  Benefit or PTS #: 7176
#  Testing: Built, Ran VSTNC
#  Module(s):
#
# Revision 1.6  1993/07/14  18:54:36  cfj
# OSF/1 AD 1.0.4 code drop from Locus.
#
# Revision 1.5  1993/05/06  19:33:02  stefan
# ad103+tnc merged with Intel code.
#
# Revision 1.1.1.3  1993/07/01  21:22:30  cfj
# Adding new code from vendor
#
# Revision 1.4  1993/01/28  16:48:46  dleslie
# New version string scheme, using mkidinfo
#
# Revision 1.3  1992/12/11  03:09:47  cfj
# Merged 12-1-92 bug drop from Locus.
#
# Revision 1.2  1992/11/30  23:04:48  dleslie
# Copy of NX branch back into main trunk
#
# Revision 1.1.2.1  1992/11/05  23:50:29  dleslie
# Local changes for NX through noon, November 5, 1992.
#
# Revision 3.3  1992/10/14  17:31:13  cfj
# Fix up Makefiles so that they find libtnc.a
#
# Revision 1.1.1.1  1993/05/03  17:59:06  cfj
# Initial 1.0.3 code drop
#
# Revision 3.4  1992/11/14  16:48:57  yazz
# Caused the sub-make of test sub-directories (like VSTNC) to take place
# only if present in the source tree.  Thus both the customer and developer
# versions of the source tree will build completely, with no manual changes
# to the Makefiles required.  Also added copyright.
#
# Revision 3.3  92/11/11  16:53:30  yazz
# cmu make
# 
# Revision 3.2  92/05/01  15:38:55  yazz
# Re-groove to make both i386 and i860 work.
# Also defined $(SOURCECWD) which points to the current source
# directory -- handy for -I and other options.  (Bob Yazz)
# 
# Revision 3.1  92/04/14  15:03:14  klh
# Fix RCS Log.
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
        -I$(MAKETOP)/server/$(CONFIG) \
        -I$(MAKETOP)/server \
        -I$(MAKETOP)/server/include \
        -I$(MAKECONF:h)/server \
        -I$(MAKECONF:h)/server/$(cputype) \
        -I$(MKINC) \
        -I$(MAKECONF:h)/usr/include \
        -I$(MAKECONF:h)/usr/include/$(TARGET_MODEL)

DIR_TARGETS = \
	VSTNC 

FILE_TARGETS = \
	rforktest \
	migratetest \
	rexectest \
	seltest \
	multitest \
	deja \
	vu

CLEANLIST = $(FILE_TARGETS) $(DIR_TARGETS)


all: $(FILE_TARGETS)
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
	$(CC) -c -DTNC -DOSF1_ADFS $(INCS) $@.c
	rm -f _cvs_id.c _cvs_id.o
	mkidinfo -f $@
	$(CC) -c _cvs_id.c
	$(CC) _cvs_id.o $@.o -o $@ -ltnc

clean:
	rm -rf $(CLEANLIST) _cvs_id.o _cvs_id.c

print_env:
	@echo INCS are $(INCS)
	@echo env MAKECPP is $$MAKECPP
	@echo MAKECPP is $(MAKECPP)
	@echo SOURCECWD is $(SOURCECWD)
	@echo MAKEDIR is $(MAKEDIR)
	@echo MAKESUB is $(MAKESUB)
	@echo MAKETOP is $(MAKETOP)
	@echo MAKECONF is $(MAKECONF)
	@echo VPATH is $(VPATH)
	@echo CPATH is $(CPATH)
	@echo LPATH is $(LPATH)
	@echo LOCAL_CPATH is $(LOCAL_CPATH)
	@echo LOCAL_LPATH is $(LOCAL_LPATH)
	@echo TARGET_CPATH is $(TARGET_CPATH)
	@echo TARGET_LPATH is $(TARGET_LPATH)
