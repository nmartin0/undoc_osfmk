# 
# $Copyright
# Copyright 1991, 1994, 1995 Intel Corporation
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
# Revision 1.10  1994/11/18  20:23:53  mtm
# Copyright additions/changes
#
# Revision 1.9  1994/09/08  19:51:43  bradf
# testing branch locking
#
# Revision 1.8  1994/06/22  22:54:06  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: remove dependence on setting CPATH and LPATH to
# 	local paths for Sun cross builds (PTS #9090)
#  Testing: built microkernel on Sun and Mach X86 platforms
#  Module(s):
#         Makeconf
#         Makefile
#         Makefile-subdirs
#         kernel/src/mig/Makefile
#         kernel/src/config/Makefile
#
# Revision 1.7  1994/05/02  16:49:38  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: cleaner 'make lint' implementation.  This incorporates
# 	Chris Peak's improved lint scheme from the server build.  We no
# 	longer rely on VPATH and CPATH files in the obj tree -- instead
# 	we actually go through the 'make' script in the obj tree for
# 	'make lint'.
#  Testing: built kernel, ran 'make lint' on entire kernel, tested using
# 	LINTFILES to make sure we can still lint individual files or
# 	collections of files
#  Module(s): Makefile, kernel/conf/Makefile.template, kernel/conf/make.template
#
# Revision 1.6  1994/01/03  20:50:35  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: allow 'lint' to be run on one file or on a collection
# 	of files
#  Testing: built, ran 'lint'
#  Module(s): Makefile
#
# Revision 1.5  1993/12/21  18:53:03  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: enable 'make lint'
#  Testing: built and ran lint
#  Module(s): Makefile
#
# Revision 1.4  1993/06/30  22:15:10  dleslie
# Adding copyright notices required by legal folks
#
# Revision 1.3  1993/04/27  20:12:45  dleslie
# Copy of R1.0 sources onto main trunk
#
# Revision 1.1.10.2  1993/04/22  18:11:36  dleslie
# First R1_0 release
#
# Revision 2.8.4.1  92/03/28  09:58:55  jeffreyh
# 	Changes from MK71
# 	[92/03/26  11:33:55  jeffreyh]
# 
# Revision 2.14  92/03/05  22:45:09  rpd
# 	Changed to use double-colon rules.
# 	[92/02/28            rpd]
# 
# Revision 2.13  92/02/19  15:05:46  elf
# 	Added Makefile-subdirs and Makefile-mkdirs.
# 	[92/02/10            rpd]
# 
# Revision 2.12  92/01/27  16:42:46  rpd
# 	Changed recursive make commands to use '+'.
# 	[92/01/27            rpd]
# 
# Revision 2.11  92/01/24  00:49:15  rpd
# 	Changed to use RECURSE_TEMPLATE.
# 	[92/01/23            rpd]
# 
# Revision 2.10  92/01/23  15:19:31  rpd
# 	Revised for new Makefile organization.
# 	[92/01/16            rpd]
# 

include ${MAKETOP}Makefile-common
# more definitions, for recursive makes
include ${MAKETOP}Makefile-subdirs
# more definitions, for mkodirs/mkidirs/mkrdirs
include ${MAKETOP}Makefile-mkdirs

# RECURSE_TEMPLATE normally creates subdirectories itself before cd'ing
# into them, but in our case it needs a little help because we do the
# directories in a strange order. The mkodirs target will create
# these if necessary.

MKODIRS = kernel kernel/src

# These subdirectories must exist in the install/release directories.
# The mkidirs and mkrdirs targets will create them.

MKIDIRS = include bin lib etc special man man/man1 man/man2 man/man3
MKRDIRS = ${MKIDIRS}

.INIT :: mkodirs

all :: install

# Order is important: we must install some include files, then do mig,
# finish the include files, then libraries, bootstrap, kernel.

# On the 860 Config must be built before user/libmach or else we will
# have problems with CRT0.o

EARLY = include

DIRS = include user \
	user/libmach user/threads bootstrap \
	kernel/src/doconf  kernel

LOCAL = kernel/src/mig kernel/src/config

# kernel/src/makeboot not built with new Makefiles yet

EARLY_RECURSE = ${RECURSE_TEMPLATE/RECURSE_DIRS/${EARLY}}
RECURSE = ${RECURSE_TEMPLATE/RECURSE_DIRS/${DIRS}}
LOCAL_RECURSE = ${LOCAL_RECURSE_TEMPLATE/RECURSE_DIRS/${LOCAL}}

install :: mkidirs
	+${EARLY_RECURSE/RECURSE_TARGET/preinstall}
	+${LOCAL_RECURSE/RECURSE_TARGET/install}
	+${RECURSE/RECURSE_TARGET/install}

release :: mkrdirs
	+${RECURSE/RECURSE_TARGET/release}

clean ::
	+${RECURSE/RECURSE_TARGET/clean}

lint:
	cd kernel; make lint

.EXPORT: LINTFILES

-include Makedep
