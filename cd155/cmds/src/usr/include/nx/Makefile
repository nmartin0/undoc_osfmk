# 
# $Copyright
# Copyright 1993, 1994, 1995  Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
#
# (c) Copyright 1992, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
#
# Intel - Release 1.0
# History
# $Log: Makefile,v $
# Revision 1.19  1994/11/19  02:44:33  mtm
# Copyright additions/changes
#
# Revision 1.18  1993/12/20  19:44:33  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.17  1993/11/18  19:19:03  dleslie
#  Reviewer: shala
#  Risk: low
#  Benefit or PTS #: put nx headers in the export tree; find mcmsg and nx
# 	headers in the right place in alloc.h
#  Testing: built, used in libnx, showpart, etc. builds
#  Module(s): Makefile alloc.h
#
# Revision 1.16  1993/07/13  22:25:08  carbajal
# Removed create.h
#
# Revision 1.15  1993/07/13  21:18:46  carbajal
# Added parsepart.h, parsesched.h, create.h
#
# Revision 1.14  1993/05/18  19:48:31  carbajal
# Removed allocsys.h and moved it to ..
#
# Revision 1.13  1993/05/11  21:13:54  carbajal
# Added allocsys.h and writepart.h
#
# Revision 1.12  1993/01/27  19:28:04  carbajal
# Added partprint.h prototype file
#
# Revision 1.11  1993/01/25  22:47:26  carbajal
# Added nodeparser.h
#
# Revision 1.10  1992/12/17  18:30:10  shala
# Added more include files.
#
# Revision 1.9  1992/12/17  15:00:41  shala
# Added utils.h .
#
# Revision 1.8  1992/11/03  00:27:14  carbajal
# Added partlock.h
#
#
DATAFILES		=	allocator.h \
			 	bitmap.h \
			 	defines.h \
				h.h \
			 	hash.h \
			 	list.h \
			 	mkpart.h \
			 	alloc.h \
			 	alloc_types.h \
			 	schedule.h \
			 	gops.h \
				assert.h \
				debugxmsg.h \
				nxacct.h \
				mcmsg_lib.h \
				nx_getopt.h \
				nx_types.h \
				partlock.h \
				partprint.h \
				partutils.h \
				procinfo.h \
				utils.h \
				nodeparser.h \
				writepart.h \
				parsepart.h \
				parsesched.h \
				smd.h


INCLUDES		= ${DATAFILES}
EXPFILES		= ${DATAFILES}
EXPDIR			= /usr/include/nx/

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
