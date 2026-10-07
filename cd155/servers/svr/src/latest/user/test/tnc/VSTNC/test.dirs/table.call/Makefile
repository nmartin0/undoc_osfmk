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
# Revision 1.5  1995/02/02  00:56:12  bolsen
#  Reviewer(s): Jerry Toman
#  Risk: Medium (lots of files)
#  Module(s): Too many to list
#  Configurations built: STD, LITE, & RAMDISK
#
#  Added or Updated the Locus Copyright message.
#
# Revision 1.4  1994/11/18  21:08:48  mtm
# Copyright additions/changes
#
# Revision 1.3  1994/02/04  01:49:59  slk
#  Reviewer: Brent Olsen
#  Risk: Low
#  Benefit or PTS #: 7176	and update VSTNC.
#  Testing: Built, Ran VSTNC
#  Module(s):
#
# Revision 3.4  93/01/13  16:01:28  jpaul
# Add defines so TBL_NODEINFO will be picked up from the table.h include
# file.
# 
# Revision 3.3  92/12/02  16:41:24  jpaul
# Remove files before copying.
# 
# Revision 3.2  92/11/14  16:52:26  yazz
# Copyright added.
# 
#
# Makefile for VSTNC table.call
#

default: all

#
# i860 Support
#
CC = $(DOI860?icc:cc)
LD = $(DOI860?ld860:ld)
AS = $(DOI860?as860:as)
AR = $(DOI860?ar860:ar)

CPATH = $(TARGET_CPATH)
LPATH = $(TARGET_LPATH)
.EXPORT: CPATH LPATH

#
# Note that $(MAKETOP) is the top of the object tree, and
# $(MAKECONF:h) is the top of the source tree.  (If there were
# no separate object tree then these would be identical.)
# $(MKINC), the microkernel include files, is defined in Makeconf.
#

INCS = \
	-I.\
	-I$(MAKETOP)server \
	-I$(MAKETOP)server/include \
	-I$(MAKECONF:h)/server \
	-I$(MAKECONF:h)/server/$(cputype) \
	-I$(MKINC)

DEFS = -DTNC -DOSF1_ADFS

.INOBJECTDIR: table.call.run
	rm -rf ./$<
	cp -p $< ./$<

LIBS = -ltnc


all: table.call table.call.run

table.call: table.call.c ../common/common.o
	$(CC) $(INCS) $(DEFS) $> -o $@ $(LIBS)

../common/common.o:
	( cd ../common; $(MAKE) )


clean:
	rm -f table.call table.call.run
