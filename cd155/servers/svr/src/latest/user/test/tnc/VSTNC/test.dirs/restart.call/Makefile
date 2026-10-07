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
# Revision 1.3  1995/02/02  00:46:30  bolsen
#  Reviewer(s): Jerry Toman
#  Risk: Medium (lots of files)
#  Module(s): Too many to list
#  Configurations built: STD, LITE, & RAMDISK
#
#  Added or Updated the Locus Copyright message.
#
# Revision 1.2  1994/11/18  21:07:44  mtm
# Copyright additions/changes
#
# Revision 1.1  1994/03/14  17:56:46  slk
# Checkpoint Restart Code Drop
#  Reviewer: Chris Peak, chrisp@locus.com
#  Risk: Low
#  Benefit or PTS #: Enhancement
#  Testing: Locus VSTNC, individual checkpoint restart by hand
#  Module(s):
#
# Revision 2.2  93/11/10  12:18:21  slk
# Check into main tree for checkpoint restart merge.
# 
# Revision 2.1.1.1  93/08/27  14:32:58  hao
# 	Initial check-in.
# 
#
# Makefile for VSTNC restart.call
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

.INOBJECTDIR: restart.call.run
	rm -rf ./$<
	cp -p $< ./$<


LIBS = -ltnc


all: restart_call restart.call.run simp_pgrp

restart_call: restart_call.c ../common/common.o
	$(CC) -g $(INCS) $> -o $@ $(LIBS)

simp_pgrp: simp_pgrp.c 
	$(CC) $(INCS) $> -o $@ $(LIBS)

../common/common.o:
	( cd ../common; $(MAKE) )


clean:
	rm -f restart_call restart_call.run simp_pgrp
