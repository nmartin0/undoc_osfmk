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
# Revision 1.6  1995/02/02  00:26:27  bolsen
#  Reviewer(s): Jerry Toman
#  Risk: Medium (lots of files)
#  Module(s): Too many to list
#  Configurations built: STD, LITE, & RAMDISK
#
#  Added or Updated the Locus Copyright message.
#
# Revision 1.5  1994/11/18  21:05:12  mtm
# Copyright additions/changes
#
# Revision 1.4  1994/03/14  17:50:58  slk
# Checkpoint Restart Code Drop
#  Reviewer: Chris Peak, chrisp@locus.com
#  Risk: Low
#  Benefit or PTS #: Enhancement
#  Testing: Locus VSTNC, individual checkpoint restart by hand
#  Module(s):
#
# Revision 1.3  1994/02/04  01:40:47  slk
#  Reviewer: Brent Olsen
#  Risk: Low
#  Benefit or PTS #: 7176	and update VSTNC.
#  Testing: Built, Ran VSTNC
#  Module(s):
#
# Revision 3.3  93/11/10  14:33:19  slk
# Add Checkpoint restart tests.
# Do not run these on non-chkpnt builds.
# 
# Revision 3.2  92/11/14  16:49:46  yazz
# Copyright added.
# 
# Revision 3.1  92/11/11  16:54:04  yazz
# cmu make
#
# Makefile for VSTNC test.dirs
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
# Note that in this list, the node_self() and table(TBL_NODEINFO) calls
# are tested first.  This is to detect early any errors in the configuration
# file, which is expected to have in it the node number where the tests
# are started as well as a complete list of all valid node numbers.
#
TEST_LIST = \
	common \
	Load_leveld.cmd \
	node_self.call \
	table.call \
	fast.cmd \
	fastnode.cmd \
	get_tnc_port.call \
	kill3.call \
	kill3.cmd \
	migrate.call \
	onnode.cmd \
	rexec.call \
	rfork.call \
	rforkmulti.call \
	set_tnc_port.call \
	sigmig.feature \
	chkpnt.cmd \
	chkpnt_pgrp.cmd \
	chkpnt_self.call \
	exec_restart \
	forkfamily.call \
	getprocinfo.call \
	on_chkpnt.call \
	on_restart.call \
	restart.call \
	restart.cmd
#
# checkpoint restart specific tests.
#	chkpnt.cmd \
#	chkpnt_pgrp.cmd \
#	chkpnt_self.call \
#	exec_restart \
#	forkfamily.call \
#	getprocinfo.call \
#	on_chkpnt.call \
#	on_restart.call \
#	restart.call \
#	restart.cmd
#


all:
	for i in $(TEST_LIST); do \
		if [ ! -d $$i ] ; then \
			mkdir $$i; \
		fi; \
		( cd $$i; $(MAKE); ) \
	done

$(TEST_LIST)::
	( cd $@; $(MAKE); )


clean:
	rm -rf $(TEST_LIST)


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
