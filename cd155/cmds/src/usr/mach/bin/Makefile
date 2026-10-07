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
# @OSF_FREE_COPYRIGHT@
#
#
# HISTORY
# $Log: Makefile,v $
# Revision 1.10  1995/03/15  17:41:54  stans
#  Added building 'pgs' utility.
#
#  Reviewer:self
#  Risk:low
#  Benefit or PTS #:12113
#
# Revision 1.9  1994/11/19  03:01:16  mtm
# Copyright additions/changes
#
# Revision 1.8  1993/12/20  19:49:20  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.7  1993/07/20  01:24:00  stans
#    Moved 'waitfor' to 'sbin' to be utilized during the startup of mach
#    servers.
#
# Revision 1.6  1993/07/16  20:42:32  stans
#    Removed 'xptest' again as OSF implies it will not work until extensive
#    MK work has been accomplished.
#
# Revision 1.5  1993/07/07  20:32:54  stans
# Added xptest to the Mach/bin stew
#
# Revision 1.4  1993/05/26  23:30:52  stans
# 'vmstat' is now the new version on '/usr/bin/vm_stat'. No need to build it
# here.
#
# Revision 1.3  1993/05/21  02:07:58  stans
# Mach 3.0 widgets
#
# Revision 1.2  1993/05/19  15:19:03  stans
# Moved machid and snames to mach_servers to reflect where they are
# installed.
#
# Revision 1.1  1993/05/05  19:50:40  stans
# Initial revision
#
# Revision 1.3  1992/10/21  13:13:08  devrcs
# 	 Added ipc_test thstate vmread
# 	xptest hash_info machipc zprint
# 	[1992/10/01  17:10:23  david]
#
# 	Added waitfor.
# 	[1992/09/23  13:05:08  barbou]
#
# 	Merged Charles Silvers's version with 1.0.4.1b1
# 	[1992/06/25  15:04:18  emcmanus]
#
# 	Charles Silvers changes from CMU
# 	[1992/06/25  13:13:33  emcmanus]
#
# 	Added mach3 program.
# 	[1992/06/23  19:20:23  emcmanus]
#
# Revision 1.2  1992/05/12  14:32:37  devrcs
# 	Created for OSF/1 MK
# 	[1992/05/04  05:34:09  condict]
# 
# $EndLog$


SUBDIRS			= hash_info ipc_test machipc md mkill ms pinfo thstate \
			  vminfo vmread wh zprint pgs
# xptest

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk
