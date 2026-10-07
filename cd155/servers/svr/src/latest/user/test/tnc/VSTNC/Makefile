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
# Revision 1.6  1995/02/02  00:23:43  bolsen
#  Reviewer(s): Jerry Toman
#  Risk: Medium (lots of files)
#  Module(s): Too many to list
#  Configurations built: STD, LITE, & RAMDISK
#
#  Added or Updated the Locus Copyright message.
#
# Revision 1.5  1994/11/18  21:05:04  mtm
# Copyright additions/changes
#
# Revision 1.4  1994/02/04  01:40:10  slk
#  Reviewer: Brent Olsen
#  Risk: Low
#  Benefit or PTS #: 7176	and update VSTNC.
#  Testing: Built, Ran VSTNC
#  Module(s):
#
# Revision 3.12  93/08/05  16:12:27  bolsen
# Re-fixed the following bug:
# [Bug 328] VSTNC fails because of outdated cmds.
# There was a problem with the symbolic links.
# 
# Revision 3.11  93/08/03  11:01:15  bolsen
# Fixed the following bug:
# [Bug 328] VSTNC fails because of outdated cmds.
# 
# Revision 3.10  93/03/10  14:22:23  yazz
# Removed from the Makefile the now-obsolete configuration files
# "config_data.i386" and "config_data.i860".
# 
# Revision 3.9  93/03/01  13:19:29  jpaul
# Add new variable. FILES_TO_COPY for make all and
# the INOBJECTDIR and rm line.
# 
# Revision 3.8  93/01/13  16:00:25  jpaul
# Add includes.
# 
# Revision 3.7  92/12/02  16:35:28  jpaul
# Reflect changes to config files.
# 
# Revision 3.6  92/11/30  14:57:28  jpaul
# Remove files before copying over.
# 
# Revision 3.5  92/11/30  12:01:45  jpaul
# makefile changed to not fail when trying to cp
# a file onto itself (mach returns -1 on the cp
# command and the make dies.)
# 
# Revision 3.4  92/11/25  10:43:53  jpaul
# Change VSTNC makefile to reflect new multi-platform
# configuration files
# 
# Revision 3.3  92/11/14  16:49:16  yazz
# Copyright added. 
# 
# Revision 3.2  92/11/11  16:53:41  yazz
# cmu make
# 
# Top level VSTNC Makefile.
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
LD = $(DOI860?ld860:ld)
AS = $(DOI860?as860:as)
AR = $(DOI860?ar860:ar)

DIR_TARGETS = test.dirs

FILES_TO_COPY = \
	run_tests run_tests_script README

FILES_TO_LINK = \
	kill3 node_self rmknod vs load_level/fast load_level/fastnode \
	load_level/load_leveld load_level/onnode

INCS = \
        -I.\
        -I$(MAKETOP)/server \
        -I$(MAKETOP)/server/include \
        -I$(MAKETOP)/usr/include \
        -I$(MAKETOP)/usr/include/$(TARGET_MODEL) \
        -I$(MAKECONF:h)/server \
        -I$(MAKECONF:h)/server/$(cputype) \
        -I$(MKINC)


all: results tncetc $(FILES_TO_COPY)
	for i in $(DIR_TARGETS); do \
		if [ ! -d $$i ] ; then \
			mkdir $$i; \
		fi; \
		( cd $$i; $(MAKE); ) \
	done

.INOBJECTDIR: $(FILES_TO_COPY)
	rm -rf ./$<
	cp -p $< ./$<

results:
	mkdir results

tncetc:
	rm -rf etc; \
	mkdir etc; \
	cd etc; \
	for i in $(FILES_TO_LINK); do \
		ln -s ../../../../etc/$$i; \
	done

clean:
	rm -rf results etc $(DIR_TARGETS) $(FILES_TO_COPY) 


print_env:
	@echo make OBJECTDIR is $(OBJECTDIR)
	@echo make SOURCEDIR is $(SOURCECWD)
	@echo env OBJECTDIR is $$OBJECTDIR
	@echo env SOURCEDIR is $$SOURCECWD
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
