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
# Revision 1.5  1994/11/18  20:53:52  mtm
# Copyright additions/changes
#
# Revision 1.4  1993/07/14  18:51:48  cfj
# OSF/1 AD 1.0.4 code drop from Locus.
#
# Revision 1.3  1993/05/06  19:30:16  stefan
# ad103+tnc merged with Intel code.
#
# Revision 1.1.1.3  1993/07/01  21:18:32  cfj
# Adding new code from vendor
#
# Revision 1.2  1992/11/30  23:02:35  dleslie
# Copy of NX branch back into main trunk
#
# Revision 1.1.2.1  1992/11/05  23:50:17  dleslie
# Local changes for NX through noon, November 5, 1992.
#
# Revision 4.1  1992/11/04  01:03:33  cfj
# Bump major revision number.
#
# Revision 1.1.1.1  1993/05/03  17:58:20  cfj
# Initial 1.0.3 code drop
#
# Revision 3.1  1992/05/01  15:37:52  yazz
# Re-groove to make both i386 and i860 work.
# Also defined $(SOURCECWD) which points to the current source
# directory -- handy for -I and other options.  (Bob Yazz)
#
# Revision 3.0  92/04/22  15:18:31  yazz
# Genesis.
# 
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

DIR_TARGETS = tnc

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
