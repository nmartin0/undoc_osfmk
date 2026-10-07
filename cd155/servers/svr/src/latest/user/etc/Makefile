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
# HISTORY
# $Log: Makefile,v $
# Revision 1.8  1994/11/18  20:52:57  mtm
# Copyright additions/changes
#
# Revision 1.7  1994/03/14  17:47:41  slk
# Checkpoint Restart Code Drop
#  Reviewer: Chris Peak, chrisp@locus.com
#  Risk: Low
#  Benefit or PTS #: Enhancement
#  Testing: Locus VSTNC, individual checkpoint restart by hand
#  Module(s):
#
# Revision 1.6  1993/07/14  18:50:06  cfj
# OSF/1 AD 1.0.4 code drop from Locus.
#
# Revision 1.5  1993/05/06  19:27:39  stefan
# ad103+tnc merged with Intel code.
#
# Revision 1.1.1.4  1993/07/01  21:15:34  cfj
# Adding new code from vendor
#
# Revision 1.4  1993/01/28  16:48:22  dleslie
# New version string scheme, using mkidinfo
#
# Revision 1.3  1992/11/30  23:00:46  dleslie
# Copy of NX branch back into main trunk
#
# Revision 1.1.2.2  1992/11/06  20:35:58  dleslie
# Merged bug drop from Locus November 3, 1992, with NX development
#
# Revision 1.1.2.1  1992/11/05  23:49:01  dleslie
# Local changes for NX through noon, November 5, 1992.
#
# Revision 2.9  1992/10/14  17:29:31  cfj
# Fix up Makefiles so that they find libtnc.a
#
# Revision 1.1.1.2  1993/05/03  17:57:18  cfj
# Initial 1.0.3 code drop
#
# Revision 2.10  1993/04/23  16:14:10  bolsen
# [SPE 0007] removed the onnode directory and added the node_self file.
#
# Revision 2.9  92/10/29  13:20:10  chrisp
# Add INCLUDE path to the server's object directory so that header generated
# 	there are picked up by vs.
# 
# Revision 2.8  92/09/21  14:17:35  chrisp
# Define OSF1_ADFS so that node_t is declared by system headers.
# Move Mach include directory before the default OSF1 includes.
# Both of the above required to eliminate dpvp_defs.h in vs.c.
# 
# Revision 2.7  92/05/01  15:35:43  yazz
# Re-groove to make both i386 and i860 work.
# Also defined $(SOURCECWD) which points to the current source
# directory -- handy for -I and other options.  (Bob Yazz)
# 
# Revision 2.6  92/04/22  15:23:39  yazz
# Replaced Makefile.  Created lists of file targets and subdirectory
# targets.  Commented out the "lnsvr" target since it won't build
# currently.
# 
# Revision 2.5  92/04/16  11:11:04  hao
# Added line to make onnode.
# 
# Revision 2.4  92/03/30  08:51:34  mbarnett
# added line to cd to load_level directory to build load leveler daemon
# 
# Revision 2.3  92/03/12  11:30:26  klh
# Add diagnostic tool, vs. (chrisp)
# 
# Revision 2.2  91/11/27  11:45:51  rabii
# 	Initial Check-in
# 
# $EndLog$ 
#

default: all		# if this isn't first, .EXPORT: chokes

#
# Include file and library search paths.
#
CPATH = $(TARGET_CPATH)
LPATH = $(TARGET_LPATH):../lib/tnc
.EXPORT: LPATH CPATH

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

#
# Order is important here.  If you change it, save the original.
#
INCS = \
	-I$(SOURCECWD)/. \
	-I$(SOURCECWD)/../../server \
	-I../../server/$(CONFIG) \
	-I../../server \
	-I$(MKINC) \
	-I$(SOURCECWD)/../../usr/include \
	-I$(SOURCECWD)/../../usr/include/$(target_cpu)


FILE_TARGETS = \
	vs \
	rmknod \
	kill3 \
	node_self \
	chkpnt \
	restart \
	cdb


DIR_TARGETS = \
	load_level
####	lnsvr		# Does not build.  See Release Notes.

CLEANLIST = $(FILE_TARGETS) $(DIR_TARGETS)

all: $(FILE_TARGETS)
	for i in $(DIR_TARGETS); do \
		if [ ! -d $$i ] ; then \
			mkdir $$i; \
		fi; \
		( cd $$i; $(MAKE); ) \
	done

$(FILE_TARGETS): $$@.c
	mkidinfo -f $@
	$(CC) -DTNC -DOSF1_ADFS $(INCS) _cvs_id.c $@.c -o $@ -ltnc

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
