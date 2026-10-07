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
# @OSF_COPYRIGHT@
#
#
# HISTORY
# $Log: Makefile,v $
# Revision 1.3  1994/11/18  21:11:25  mtm
# Copyright additions/changes
#
# Revision 1.2  1992/11/30  23:18:14  dleslie
# Copy of NX branch back into main trunk
#
# Revision 1.1.2.1  1992/11/06  00:01:04  dleslie
# Local changes for NX through noon, November 5, 1992.
#
# Revision 4.1  1992/11/04  01:20:02  cfj
# Bump major revision number.
#
# Revision 1.1  1992/07/20  16:28:44  nandy
# Initial revision
#
# Revision 1.7  92/04/13  14:28:42  stans
# "OSF-distribution-ad8.5.2"
# 
# Revision 1.7.4.2  90/12/13  11:27:32  kwallace
# 	Merged changes from latest branch.
# 	Revision 1.7.2.2  90/11/29  13:39:11  hermi
# 		Added addrconf.h
# 		[90/11/29  11:08:36  hermi]
# 	[90/12/13  10:33:33  kwallace]
# 
# Revision 1.7  90/10/31  15:15:23  devrcs
# 	fix thread. => thread.h, vtrap.h m_tuning.h => trap.h vm_tuning.h
# 	[90/10/19  08:34:10  jeffc]
# 
# 	Nosupport aout functionality for the '386
# 	[90/10/15  09:52:04  kevins]
# 
# 	Added machtime.h
# 	[90/10/10  06:20:43  kevins]
# 
# Revision 1.6  90/10/07  20:28:03  devrcs
# 	Added EndLog Marker.
# 	[90/09/28  20:46:17  gm]
# 
# Revision 1.5  90/09/13  12:32:11  devrcs
# 	Updated to remove references to deleted include files.
# 	[90/08/27  09:32:50  gm]
# 
# Revision 1.4  90/08/09  14:26:22  devrcs
# 	Cleanup for export, install and post-freeze.
# 	[90/08/05  13:32:01  gm]
# 
# $EndLog$

VPATH	= ${MAKETOP}/kernel/i386:${MAKETOP}/kernel/i386/${TARGET_MACHINE}

DATAFILES		= asm.h clock.h cpu.h debug.h disk.h endian.h fpreg.h \
			  ipl.h machlimits.h machtime.h machparam.h pcb.h \
			  pic.h pit.h pmap.h psl.h reg.h seg.h thread.h\
			  trap.h vm_tuning.h vmparam.h xpr.h addrconf.h
INCLUDES		= ${DATAFILES}

ILIST			= ${INCLUDES}
IDIR			= /usr/include/i386/
IMODE			= 644

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/datafiles.mk
