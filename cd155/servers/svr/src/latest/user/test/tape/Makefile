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
# Revision 1.2  1994/11/18  20:57:26  mtm
# Copyright additions/changes
#
# Revision 1.1  1994/07/09  00:50:43  slk
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
# It's a hassle getting a macro that defines the pathname of the
# actual source directory, since make doesn't define one.  (Why?)
# We construct one using the pathname of the Makeconf file, taking
# its head (directory part including slash, less trailing filename),
# then appending $(MAKESUB) which is the relative pathname from the
# Makeconf file to the current source directory.
#
SOURCECWD = $(MAKECONF:h)$(MAKEDIR)

# Recall that you cannot cd into a sub-directory to perform a sub-make
# unless the directory also exists in the object tree.  Hence, the
# mkdir command below is required.

DIR_TARGETS = \
	auto \
	manual 

CLEANLIST = $(DIR_TARGETS)


all:	
	for i in $(DIR_TARGETS); do \
		if [ ! -d $$i ] ; then \
			mkdir $$i; \
		fi; \
		( cd $$i; $(MAKE); ) \
	done

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
