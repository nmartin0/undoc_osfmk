# 
# $Copyright
# Copyright 1992, 1993, 1994, 1995  Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
# TOPDIR must be defined as the relative path FROM *this* directory TO the top
# level source directory.
TOPDIR		= ..
# FROMTOP must be defined as the relative path FROM the top level source
# directory TO *this* subdir.  NOTE: Both FROMTOP and TOPDIR are redefined as
# make recurses the source directory hierarchy.  FROMTOP must always begin
# with a '.' in all make files.
FROMTOP		= ./Special


MINCL		= $(TOPDIR)/Make.incl
MRULES		= $(TOPDIR)/Make.rules

include $(MINCL)
include $(MRULES)

all:	Can Lad	

#  .DEFAULT rule is used to retrieve .c & .h source files from SCCS directories
#.DEFAULT:
#	$(GET) $(@F) >/dev/null 2>&1

Can::
	cd $@; $(MAKE) -ef $@.mk

Lad::
	cd $@; $(MAKE) -ef $@.mk

depend:
	cd Can; $(MAKE) -ef Can.mk depend
	cd Lad; $(MAKE) -ef Lad.mk depend
