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
# Revision 1.6  1995/02/02  00:24:26  bolsen
#  Reviewer(s): Jerry Toman
#  Risk: Medium (lots of files)
#  Module(s): Too many to list
#  Configurations built: STD, LITE, & RAMDISK
#
#  Added or Updated the Locus Copyright message.
#
# Revision 1.5  1994/11/18  21:05:18  mtm
# Copyright additions/changes
#
# Revision 1.4  1994/03/14  17:51:25  slk
# Checkpoint Restart Code Drop
#  Reviewer: Chris Peak, chrisp@locus.com
#  Risk: Low
#  Benefit or PTS #: Enhancement
#  Testing: Locus VSTNC, individual checkpoint restart by hand
#  Module(s):
#
# Revision 1.3  1994/02/04  01:41:20  slk
#  Reviewer: Brent Olsen
#  Risk: Low
#  Benefit or PTS #: 7176	and update VSTNC.
#  Testing: Built, Ran VSTNC
#  Module(s):
#
# Revision 3.3  92/12/02  16:36:12  jpaul
# Remove files before copying.
# 
# Revision 3.2  92/11/14  16:49:28  yazz
# Copyright added.
# 
# Makefile for VSTNC Load_leveld.cmd
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

.INOBJECTDIR: Load_leveld.cmd.run migrate_commands parameters
	rm -rf ./$<
	cp -p $< ./$<


LIBS = -ltnc


all: Load_leveld.cmd forever_y forever_n \
	Load_leveld.cmd.run migrate_commands parameters

Load_leveld.cmd: Load_leveld.cmd.c ../common/common.o
	$(CC) $(INCS) $> -o $@ $(LIBS)

forever_y: forever.c
	$(CC) $> -o $@

forever_n: forever_y
	ln forever_y forever_n

../common/common.o:
	( cd ../common; $(MAKE) )


clean:
	rm -f Load_leveld.cmd forever_n forever_y Load_leveld.cmd.run \
	migrate_commands parameters
