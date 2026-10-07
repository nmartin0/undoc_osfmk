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
# $Log: Makefile,v $
# Revision 1.18  1995/02/01  23:42:03  bolsen
#  Reviewer(s): Jerry Toman
#  Risk: Medium (lots of files)
#  Module(s): Too many to list
#  Configurations built: STD, LITE, & RAMDISK
#
#  Added or Updated the Locus Copyright message.
#
# Revision 1.17  1994/11/19  03:07:46  mtm
# Copyright additions/changes
#
# Revision 1.16  1994/11/18  20:53:22  mtm
# Copyright additions/changes
#
# Revision 1.15  1994/01/12  10:54:35  stefan
#  Reviewer: none
#  Risk: very low - not used for transmittal builds
#  Benefit or PTS #: can build load leveling daemon
#  Testing: built
#  Module(s): Makefile
#
# Changed $MIGLPATH to look at $MIGCOMDIR first. Necesarry to build load
# leveling daemon in the server context with the new build environment.
#
# Revision 1.14  1993/11/19  17:58:18  bolsen
#  Reviewer: Mike Barnett, Stefan Tritscher
#  Risk: low
#  Benefit or PTS #: Part of 7176 - Can't build user tree.
#  Testing: Builds work
#  Module(s): user/etc/load_level/Makefile
# 	    Several other Makefiles will also be modified in the following
# 	    directories: user/test/tnc/VSTNC/*
#
# Revision 1.13  1993/09/30  18:32:26  stefan
# changed -lthreads to -lcthreads and use -lnx instead of -ltnc for onnode
#
# Revision 1.12  1993/09/30  18:06:57  stefan
# Made the load leveler multi-threaded
#
# Revision 1.11  1993/07/14  18:50:34  cfj
# OSF/1 AD 1.0.4 code drop from Locus.
#
# Revision 1.10  1993/06/21  12:01:33  stefan
# Use -lnx instead of -ltnc until the TNC system calls
# in libtnc.a are fixed (stack frame problem).
#
# Revision 1.1.1.3  1993/07/01  21:16:39  cfj
# Adding new code from vendor
#
# Revision 1.9  1993/05/21  20:19:05  stefan
# Removed -DNX from DEFINES.
#
# Revision 1.8  1993/05/13  09:17:43  stefan
# Integrated static load leveling support.
#
# Revision 1.7  1993/05/06  19:28:22  stefan
# ad103+tnc merged with Intel code.
#
# Revision 1.6  1993/02/23  12:32:25  stefan
# Renamed $MIGDEBUG to $DEBUG to avoid confusion with variables for mig.
#
# Revision 1.5  1993/02/18  12:24:33  stefan
# Removed -DSYSLOGBUG as the bug in syslog appears to be fixed.
#
# Revision 1.4  1993/01/28  16:48:27  dleslie
# New version string scheme, using mkidinfo
#
# Revision 1.3  1993/01/22  19:28:43  stefan
# Merged Locus 01-20-93 code drop.
#
# Revision 1.2  1992/11/30  23:01:25  dleslie
# Copy of NX branch back into main trunk
#
# Revision 1.1.2.1  1992/11/05  23:49:35  dleslie
# Local changes for NX through noon, November 5, 1992.
#
# Revision 4.1  1992/11/04  01:01:33  cfj
# Bump major revision number.
#
# Revision 3.10  1992/10/12  16:55:09  stefan
# added -DMIGRATELOG and -DSYSLOGBUG
#
# Revision 1.1.1.1  1993/05/03  17:57:30  cfj
# Initial 1.0.3 code drop
#
# Revision 3.11  1993/04/23  16:15:39  bolsen
# [SPE 0007] added onnode, fast and fastnode.
# 	fixed local header file dependancies.
#
# Revision 3.10  92/12/22  12:47:27  mbarnett
# Added defines __STDC__ and _NO_PROTO so that code would compile using i386
# compiler.
# 
# Revision 3.9  92/06/15  12:56:51  mbarnett
# Changed a log comment.
# 
# Revision 3.8  92/06/15  12:21:06  mbarnett
# Removed lines which build load_leveld_restart and removed all dependencies
# on load_leveld restart. Also, changed command line which links objects to
# link with libmach.a instead of libmach_sa.a.
# 
# Revision 3.7  92/05/13  08:25:47  mbarnett
# Added lines to accomodate additions of modules osf1_dep.o and 
# load_leveld_restart
# 
# Revision 3.6  92/05/01  15:36:48  yazz
# Re-groove to make both i386 and i860 work.
# Also defined $(SOURCECWD) which points to the current source
# directory -- handy for -I and other options.  (Bob Yazz)
# 
# Revision 3.5  92/04/22  14:34:14  mbarnett
# Added default target to Makefile.
# 
# Revision 3.4  92/04/22  10:47:21  mbarnett
# Explicitly set LPATH variable and removed unnecessary option -L/lib.
# 
# Revision 3.3  92/04/15  17:15:03  mbarnett
# Added another -I option to include OSF user/include.
# Set macro _NLCTYPE_H_.
# 
# Revision 3.2  92/04/14  16:14:13  mbarnett
# Added -ltnc option to pick up the objects from the libtnc library.
#
# Revision 3.1  92/04/02  13:22:44  mbarnett
# Added macro for C library.
# 

default: all

#
# Include file and library search paths.  Defined in Makeconf.
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
# It's a hassle getting a macro that defines the pathname of the
# actual source directory, since make doesn't define one.  (Why?)
# We construct one using the pathname of the Makeconf file, taking
# its head (directory part including slash, less trailing filename),
# then appending $(MAKESUB) which is the relative pathname from the
# Makeconf file to the current source directory.
#
SOURCECWD = $(MAKECONF:h)$(MAKEDIR)

#
#  Compilation components
#
LN = /bin/ln

CPATH2INCLIST=`echo $(CPATH) | sed -e 's/:/ -I/g'`

MIG_LPATH=$(MIGCOMDIR):$(MKLIB):$(LOCAL_LPATH)	# mig requires a local cpp in $LPATH
MIGINC= -I$(MKINC)
SRVINC= \
	-I. \
	-I$(SOURCECWD)/. \
	-I$/../../../server \
	-I$(SOURCECWD)/../../../server \
	-I$(SOURCECWD)/../../../server/include
USRINC= \
	-I$(CPATH2INCLIST) \
	-I$(SOURCECWD)/../../../usr/include \
	-I$(SOURCECWD)/../../../usr/include/$(TARGET_MODEL)
MACHLIB= -L$(MKLIB)
DEPENDS=  -MD
DEPENDS1= -DOSF -DCOMPAT_43 -DOSF1_ADFS -DOSF1_SERVER -DSECOND_SERVER
DEPENDS2= -DSUN_RPC -DTNC -D__STDC__  -D_NO_PROTO
INCLUDES= $(USRINC) $(MIGINC) $(SRVINC)
DEFINES= -UCMUCS -D_BSD -D_NLCTYPE_H_ -DMACH_IPC_COMPAT=0 -DSLL
DEBUG= -DMIGRATELOG

#
#  Common C-compilation flags
#
#OPTIMIZE= -O
OPTIMIZE= -g
CFLAGS=  $(DEPENDS) $(DEPENDS1) $(DEPENDS2) $(OPTIMIZE) $(INCLUDES) $(DEFINES) \
	$(DEBUG)
MIGFLAGS = $(DEPENDS) $(DEPENDS1) $(DEPENDS2) $(DEFINES) $(MIGINC)
OBJS = osf1_dep.o load_leveld.o load_level_mig.o sll_osf1_dep.o sll_load_leveld.o

CLEANLIST = \
	$(OBJS) \
	load_level_server.c \
	load_level_user.c \
	load_leveld \
	onnode fast fastnode

all:	load_leveld onnode fast fastnode

load_level_server.c load_level_user.c : load_level.defs
	LPATH=$(MIG_LPATH) mig $(MIGFLAGS) -header /dev/null \
		-user load_level_user.c -server load_level_server.c \
		load_level.defs

load_level_mig.o: load_level_mig.c load_level_server.c load_level_user.c
	$(CC) -c $(CFLAGS) load_level_mig.c

load_leveld.o: load_leveld.c load_level_com.h load_level_types.h
	$(CC) -c $(CFLAGS) load_leveld.c

osf1_dep.o: osf1_dep.c load_level_com.h load_level_types.h
	$(CC) -c ${CFLAGS} osf1_dep.c

sll_osf1_dep.o: sll_osf1_dep.c sll_load_level_types.h load_level_com.h \
		load_level_types.h
	$(CC) -c ${CFLAGS} sll_osf1_dep.c

sll_load_leveld.o: sll_load_leveld.c sll_load_level_types.h load_level_com.h \
		load_level_types.h
	$(CC) -c ${CFLAGS} sll_load_leveld.c

load_leveld: $(OBJS)
	rm -f _cvs_id.c _cvs_id.o
	mkidinfo -f load_leveld
	$(CC) -c _cvs_id.c
	$(CC) $(MACHLIB) _cvs_id.o osf1_dep.o load_leveld.o load_level_mig.o \
			sll_osf1_dep.o sll_load_leveld.o \
			-lnx -lcthreads -lmach -o load_leveld
#			-ltnc -lcthreads -lmach -o load_leveld

onnode: onnode.c load_level_com.h
#	$(CC) $(CFLAGS) onnode.c -o onnode -ltnc
	$(CC) $(CFLAGS) onnode.c -o onnode -lnx -lmach

fast: onnode
	$(LN) onnode fast

fastnode: onnode
	$(LN) onnode fastnode

clean:
	rm -rf $(CLEANLIST)

print_env:
	@echo environment LPATH is $$LPATH
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
