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
# Revision 1.3  1995/02/02  00:41:59  bolsen
#  Reviewer(s): Jerry Toman
#  Risk: Medium (lots of files)
#  Module(s): Too many to list
#  Configurations built: STD, LITE, & RAMDISK
#
#  Added or Updated the Locus Copyright message.
#
# Revision 1.2  1994/11/18  21:07:10  mtm
# Copyright additions/changes
#
# Revision 1.1  1994/03/14  17:55:28  slk
# Checkpoint Restart Code Drop
#  Reviewer: Chris Peak, chrisp@locus.com
#  Risk: Low
#  Benefit or PTS #: Enhancement
#  Testing: Locus VSTNC, individual checkpoint restart by hand
#  Module(s):
#
# Revision 2.2  93/11/10  12:17:00  slk
# Check into main tree for checkpoint restart merge.
# 
# Revision 2.1.1.1  93/08/27  14:24:39  hao
# 	Initial check-in.
# 
#
# Makefile for VSTNC on_chkpnt.cmd
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
	-I$(MAKETOP)/server \
	-I$(MAKETOP)/server/include \
	-I$(MAKECONF:h)/server \
	-I$(MAKECONF:h)/server/$(cputype) \
	-I$(MKINC) 

.INOBJECTDIR: on_chkpnt.call.run
	rm -rf ./$<
	cp -p $< ./$<


LIBS = -ltnc


all: on_chkpnt on_chkpnt.call.run simp1 simp2 simp3

on_chkpnt: on_chkpnt.c ../common/common.o
	$(CC) $(INCS) $> -o $@ $(LIBS)

simp1: simp1.c 
	$(CC) $(INCS) $> -o $@ $(LIBS)

simp2: simp2.c 
	$(CC) $(INCS) $> -o $@ $(LIBS)

simp3: simp3.c 
	$(CC) $(INCS) $> -o $@ $(LIBS)

../common/common.o:
	( cd ../common; $(MAKE) )


clean:
	rm -f on_chkpnt on_chkpnt.run simp1 simp2 simp3
