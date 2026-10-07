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
# Copyright (c) 1989 Carnegie-Mellon University
# All rights reserved.  The CMU software License Agreement specifies
# the terms and conditions for use and redistribution.
#  
# HISTORY
# $Log: Makefile,v $
# Revision 1.10  1994/11/18  20:21:57  mtm
# Copyright additions/changes
#
# Revision 1.9  1994/04/28  18:50:48  chrisp
# Re-instated the significance of DOI860 to determine
# whether the environment is setup for i860 versus i386.
# SOURCEDIR directory shadowing is now not enabled.
# 'make lint' now goes through the server/emulator Makefiles
# like compilations rather than proceeding directly to the
# CONFIG directory.
#
#  Reviewer: dleslie, cfj
#  Risk: M
#  Benefit or PTS #: 9188
#  Testing: Builds, builds, builds.
#  Module(s):
#
# Revision 1.8  1994/04/11  18:45:44  dleslie
#  Reviewer: John Litvin
#  Risk: low
#  Benefit or PTS #: support for configurable emulator builds
#  Testing: builds STD+WS and RAMDISK+WS emulators of same size as old way
# 	of building
#  Module(s): Makefile
#
# Revision 1.7  1994/01/03  20:46:06  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: allow 'lint' to be run on individual files or collections
# 	of files
#  Testing: built, ran 'lint' successfully
#  Module(s):
#
# Revision 1.6  1993/12/10  17:56:48  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: enable 'make lint' for server/emulator
#  Testing: ran 'make lint' successfully
#  Module(s): Makefile, emulator/Makefile, server/conf/Makefile.template,
# 	server/conf/make.template
#
# Revision 1.5  1993/07/14  17:29:54  cfj
# OSF/1 AD 1.0.4 code drop from Locus.
#
# Revision 1.1.1.3  1993/07/01  18:19:29  cfj
# Adding new code from vendor
#
# Revision 1.4  1993/05/06  22:21:12  cfj
# ad103+tnc merged with Intel code.
#
# Revision 1.1.1.1  1993/05/03  17:16:26  cfj
# Initial 1.0.3 code drop
#
# Revision 1.3  1993/02/24  19:52:20  cfj
# Remove User from default build.
#
# Revision 1.2  1992/11/30  22:14:13  dleslie
# Copy of NX branch back into main trunk
#
# Revision 1.1.2.1  1992/11/05  22:15:03  dleslie
# cal modifications for NX through noon, November 5, 1992ZZ
#
# Revision 4.1  1992/11/03  23:56:10  cfj
# Bump major revision number.
#
# Revision 2.8  1992/04/22  15:20:39  yazz
# Added main directory user to the existing server and emulator directories.
#
# Revision 2.7  92/03/12  12:33:45  klh
# Back to version 2.5 with not messing with symlinks (chrisp)
# 
# Revision 2.5  91/12/18  10:27:49  roy
# 	91/11/15  15:31:14  condict
# 	Eliminate checking and remaking the obsolete "machine" symbolic links 
# 	in the src tree.  Also, allow the building of just the server or just 
# 	the emulator, with automatic creation of the top-level object dirs.
# 
# 	91/11/07  15:14:58  condict
# 	Create the emulator and server object directories automatically.
# 
# Revision 2.4  91/10/11  13:53:51  sjs
# 	Revision 3.4  91/09/16  16:06:05  condict
# 	Delete test directory and files.  Was only needed for minimal servers.
# 
# Revision 2.3  91/10/08  11:26:06  rabii
# 	Removed include of Makefile.machine which is no longer used
# 
# Revision 2.2  91/08/30  16:39:20  rabii
# 	Initial V2 Checkin
# 
# Revision 3.3  91/08/30  10:23:29  condict
# Add targets to check and/or construct the machine-dependent sym links in
# the source dir tree.  Also, include new Makefile.machine for specifying
# target dependent stuff.
# 
# Revision 3.2  91/02/27  17:03:26  condict
# Change default configuration to STD+WS+second;  Also, add building
# of the new test directory (contains program for testing the server).
# 
# Revision 3.2  91/02/27  16:01:52  condict
# Change default configuration to STD+WS+second
# 
# Revision 3.1  91/01/21  17:23:36  condict
# Add default CONFIG=STD2VICE+WS
# 
# Revision 3.0  91/01/17  12:04:58  condict
# Unchanged copy from Mach 3.0 BSD UNIX server
# 
# Revision 3.0  91/01/17  11:58:52  condict
# Single Server Support File
# 
# Revision 3.0  91/01/17  11:54:32  condict
# *** empty log message ***
# 
# Revision 2.3  90/09/09  14:12:37  rpd
# 	Added install : all
# 	[90/08/20            rwd]
# 
# Revision 2.2  89/08/09  14:34:45  rwd
# 	Created
# 	[89/08/06            rwd]
# 

all:	Server Emulator 	# file "Server", etc must NOT exist

Server:	./server
	cd server;	$(MAKE)

Emulator: ./emulator
	cd emulator;	$(MAKE)

User:	./user
	cd user;	$(MAKE)

clean:	./emulator ./server ./user
	cd user;		$(MAKE) clean
	cd server;		$(MAKE) clean
	cd server/src/config;	$(MAKE) clean
	cd emulator;		$(MAKE) clean

./emulator ./server ./user:
	mkdir $@

lint: ./emulator ./server
	cd server;              $(MAKE) lint
	cd emulator;            $(MAKE) lint


.EXPORT: CONFIG LINTFILES

