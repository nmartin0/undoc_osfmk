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
# Revision 1.8  1995/02/02  00:16:48  bolsen
#  Reviewer(s): Jerry Toman
#  Risk: Medium (lots of files)
#  Module(s): Too many to list
#  Configurations built: STD, LITE, & RAMDISK
#
#  Added or Updated the Locus Copyright message.
#
# Revision 1.7  1994/11/18  20:57:07  mtm
# Copyright additions/changes
#
# Revision 1.6  1994/03/14  17:50:09  slk
# Checkpoint Restart Code Drop
#  Reviewer: Chris Peak, chrisp@locus.com
#  Risk: Low
#  Benefit or PTS #: Enhancement
#  Testing: Locus VSTNC, individual checkpoint restart by hand
#  Module(s):
#
# Revision 1.5  1993/07/14  18:53:54  cfj
# OSF/1 AD 1.0.4 code drop from Locus.
#
# Revision 1.4  1993/01/28  16:48:44  dleslie
# New version string scheme, using mkidinfo
#
# Revision 1.1.1.2  1993/07/01  21:21:30  cfj
# Adding new code from vendor
#
# Revision 1.3  1992/12/11  03:09:28  cfj
# Merged 12-1-92 bug drop from Locus.
#
# Revision 1.2  1992/11/30  23:04:22  dleslie
# Copy of NX branch back into main trunk
#
# Revision 1.1.2.1  1992/11/06  00:14:55  dleslie
# Local changes for NX through noon, November 5, 1992.
#
# Revision 3.1  1992/10/14  17:30:47  cfj
# Fix up Makefiles so that they find libtnc.a
#
# Revision 3.1  92/11/23  11:46:30  chrisp
# Add worm, duck, pie and stresslib.
# 
# Revision 3.0  92/07/21  12:05:31  chrisp
# First release
# 
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
# It's a hassle getting a macro that defines the pathname of the
# actual source directory, since make doesn't define one.  (Why?)
# We construct one using the pathname of the Makeconf file, taking
# its head (directory part including slash, less trailing filename),
# then appending $(MAKESUB) which is the relative pathname from the
# Makeconf file to the current source directory.
#
SOURCECWD = $(MAKECONF:h)$(MAKEDIR)

PROG_TARGETS = \
	frog \
	bunny \
	pie \
	node_self

SCRIPT_TARGETS = \
	bee \
	artemis \
	pan \
	janus \
	worm \
	duck \
	stresslib

INCS = \
	-I$(SOURCECWD)/../../../server \
	-I$/../../../server \
	-I$(SOURCECWD)/../../../usr/include \
	-I$(SOURCECWD)/../../../usr/include/$(TARGET_MODEL) \
	-I$(MKINC)

CLEANLIST = $(PROG_TARGETS) $(SCRIPT_TARGETS)


all: $(PROG_TARGETS) $(SCRIPT_TARGETS)

$(PROG_TARGETS): $$@.c
	rm -f _cvs_id.c
	mkidinfo -f $@
	$(CC) -DOSF1_ADFS -DTNC $(INCS) _cvs_id.c $@.c -o $@ -ltnc

$(SCRIPT_TARGETS): $$@
	@cp $(SOURCECWD)/$@ .
	@chmod +x $@

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
