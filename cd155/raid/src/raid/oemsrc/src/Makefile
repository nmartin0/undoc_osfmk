# $Copyright
# Copyright 1992, 1993, 1994, 1995 Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# TOPDIR must be defined as the relative path FROM *this* directory TO the top
# level source directory.
TOPDIR		= .
# FROMTOP must be defined as the relative path FROM the top level source
# directory TO *this* subdir.  NOTE: Both FROMTOP and TOPDIR are redefined as
# make recurses the source directory hierarchy.  FROMTOP must always begin
# with a '.' in all make files.
FROMTOP		= .

MINCL		= Make.incl
MRULES		= Make.rules

include $(MINCL)
include $(MRULES)

#MAKE		= /bin/make
MAKE		= pmake

#  List of all subdirectories which need to be recursed by `make`
PROJDIRLIST	= Sysdep Common Dacparams Dacevents Ace Acf Arc Amd Apc \
		Apr Special

#  List of all make files (except Make.incl, Make.rules, and (this) Makefile)
MKLIST	= Sysdep/Sysdep.mk \
	Sysdep/$(SYS)/$(SYS).mk \
	Special/Special.mk \
	Special/Can/Can.mk \
#	Special/Lad/Lad.mk \
	Common/Common.mk \
	Dacparams/Dacparams.mk \
	Dacevents/Dacevents.mk \
	Ace/Ace.mk \
	Acf/Acf.mk \
	Acf/Actions/Actions.mk \
	Acf/Actions/Download/Download.mk \
	Acf/Menus/Menus.mk \
	Arc/Arc.mk \
	Arc/Dacparams/Dacparams.mk \
	Amd/Amd.mk \
	Apc/Apc.mk \
	Apr/Apr.mk

#.DEFAULT:
#	$(GET) $(@F) >/dev/null 2>&1

all:	./Common/Include/stddefs.h $(MKLIST) $(PROJDIRLIST)

dosacf:	Common Dacparams Dacevents Sysdep Acf

$(PROJDIRLIST)::
	cd $@; $(MAKE) -ef $@.mk

