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
# Revision 1.6  1995/02/02  00:51:38  bolsen
#  Reviewer(s): Jerry Toman
#  Risk: Medium (lots of files)
#  Module(s): Too many to list
#  Configurations built: STD, LITE, & RAMDISK
#
#  Added or Updated the Locus Copyright message.
#
# Revision 1.5  1994/11/18  21:08:16  mtm
# Copyright additions/changes
#
# Revision 1.4  1994/03/14  17:57:42  slk
# Checkpoint Restart Code Drop
#  Reviewer: Chris Peak, chrisp@locus.com
#  Risk: Low
#  Benefit or PTS #: Enhancement
#  Testing: Locus VSTNC, individual checkpoint restart by hand
#  Module(s):
#
# Revision 3.3  92/12/02  16:39:34  jpaul
# Remove files before copying.
# 
# Revision 3.2  92/11/14  16:51:46  yazz
# Copyright added.
# 
#
# Makefile for VSTNC rfork.call
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
CPATH2INCLIST=`echo $(CPATH) | sed -e 's/:/ -I/g'`
.EXPORT: CPATH LPATH CPATH2INCLIST

INCS = -I. -I$(CPATH2INCLIST) -I$(MKINC) -I$(SVRINC) -I$(SVRINC_MACHINE)

LIBS = -ltnc


all: rfork.call rfork.call.run

rfork.call: rfork.call.c ../common/common.o
	$(CC) $(INCS) $> -o $@ $(LIBS)

../common/common.o:
	( cd ../common; $(MAKE) )


clean:
	rm -f rfork.call rfork.call.run
