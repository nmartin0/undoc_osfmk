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

#MINCL		= Make.incl
#MRULES		= Make.rules

#include $(MINCL)
#include $(MRULES)

#MAKE		= /bin/make
MAKE		= pmake

#  List of all subdirectories which need to be recursed by `make`
PROJDIRLIST	= libio/libio libio/scsi oemsrc/src 

#  List of all make files (except Make.incl, Make.rules, and (this) Makefile)
MKLIST	= libio/libio/makefile \
	libio/scsi/makefile \
	oemsrc/src/Makefile 

#.DEFAULT:
#	$(GET) $(@F) >/dev/null 2>&1

all:	$(MKLIST) $(PROJDIRLIST)

#dosacf:	Common Dacparams Dacevents Sysdep Acf

$(PROJDIRLIST)::
	cd $@; $(MAKE) -e


