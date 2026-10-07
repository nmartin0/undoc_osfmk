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
# Mach Operating System
# Copyright (c) 1990 Carnegie-Mellon University
# All rights reserved.  The CMU software License Agreement specifies
# the terms and conditions for use and redistribution.
#
# HISTORY
# $Log: Makefile,v $
# Revision 1.5  1994/11/18  20:53:12  mtm
# Copyright additions/changes
#
# Revision 1.4  1993/07/14  18:50:22  cfj
# OSF/1 AD 1.0.4 code drop from Locus.
#
# Revision 1.3  1993/05/06  19:28:01  stefan
# ad103+tnc merged with Intel code.
#
# Revision 1.1.1.3  1993/07/01  21:15:58  cfj
# Adding new code from vendor
#
# Revision 1.2  1992/11/30  23:01:02  dleslie
# Copy of NX branch back into main trunk
#
# Revision 1.1.2.1  1992/11/05  23:49:10  dleslie
# Local changes for NX through noon, November 5, 1992.
#
# Revision 4.1  1992/11/04  01:01:17  cfj
# Bump major revision number.
#
# Revision 1.1.1.1  1993/05/03  17:57:26  cfj
# Initial 1.0.3 code drop
#
# Revision 2.5  1992/05/01  15:36:34  yazz
# This is no longer built.
#
# Revision 2.4  91/11/27  11:48:38  rabii
# 	Removed extra stuff, and change cc to $CC
# 
# Revision 2.3  91/11/27  11:29:03  rabii
# 	Put in temporary libmach (what is the right libmach to use?)
# 
# Revision 2.2  91/11/27  11:14:57  rabii
# 	Initial check-in
# 
# $EndLog$
# 

#
# i860 Support
#
CC = $(DOI860?icc:cc)
LD = $(DOI860?ld860:ld)
AS = $(DOI860?as860:as)
AR = $(DOI860?ar860:ar)


LIBS = -lmach
#

LIBDIR = -L/afs/ri/$(target_sys)/usr/mach3/lib
#LIBDIR = -L../../../../../../mk/release/$(target_sys)/latest/lib

#
# directory where the mk sources reside.
#
#MKDIR = /afs/ri/project/mach3/xm/mk/src/MK66/kernel
#MKDIR = ../../../../../../mk/src/latest/kernel/
MKDIR = $(MKINC)

#
# Add the src dir for the mk since some of the norma include files are
# not yet exported.  Note that $(target_sys) is i386_mach or i860_mach .
INCDIR = -I$(MKDIR)  -I/afs/ri/$(target_sys)/usr/mach3/include

CFLAGS = $(INCDIR) $(CDEBUGFLAGS) $(CDEFS) $(LIBDIR)

# Bind the norma support with the lnsvr since libmach does not support
# norma calls as yet.  Once libmach does, mach_normaUser.o can be deleted
# from the following rule
#

OBJS = lnsvr.o nprocs.o mach_normaUser.o

all:	lnsvr

mach_normaUser.c mach_normaServer.c mach_norma.h: $(MKDIR)/mach/mach_norma.defs
	mig $(MKDIR)/mach/mach_norma.defs $(INCDIR)

lnsvr:	$(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS) $(LIBDIR)

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
