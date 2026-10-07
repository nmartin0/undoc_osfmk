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
# @OSF_COPYRIGHT@
#
#
# HISTORY
# $Log: Makefile,v $
# Revision 1.9  1995/02/25  00:31:10  hobbes
#  Reviewer: Nina Lepak
#  Risk: Low
#  Benefit or PTS #: 12438
#  Testing: verified the fix by testing the steps that produced it
#  Module(s): sbin/mount/Makefile usr/sbin/mount/Makefile
# 	    usr/sbin/mount/mount.c usr/sbin/mountd/mountd.c
#
# Revision 1.8  1994/11/19  03:09:28  mtm
# Copyright additions/changes
#
# Revision 1.7  1994/01/13  17:36:25  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: replace hardcoded relative paths for libraries (libesize,
# 	libmacs, librin, libnqs) so sandboxes can find libraries in
# 	shadow obj trees
#  Testing: built
#  Module(s): Makefile
#
# Revision 1.6  1993/12/20  19:53:48  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.5  1993/09/28  22:35:32  brad
# Removed link with libnx ... no longer needed for large file support.
# Now link with libesize to get extended arithmetic functions.
#
# Revision 1.4  1993/06/29  15:48:36  shala
# Removed mach3 dependencies.
#
# Revision 1.3  1993/04/02  22:22:11  brad
# Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
# Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
#
# Revision 1.2.2.1  1993/01/12  02:14:23  brad
# Added support for PFS, including the stripeunit and stripedirs options.
#
# Revision 1.2  1992/10/12  22:13:53  shala
# New version to support maj, min and node numbers.
#
# Revision 1.4  90/10/31  15:36:55  devrcs
# 	removed UNIX_DOMAIN
# 	[90/10/10  22:06:36  gmf]
# 
# 	Added pathnames.h.
# 	[90/10/10  21:14:28  gmf]
# 
# Revision 1.3  90/10/07  22:21:09  devrcs
# 	Added EndLog Marker.
# 	[90/09/29  15:29:13  gm]
# 
# Revision 1.2  90/09/23  16:58:45  devrcs
# 	Moved components into their own subdirectories.
# 	[90/09/22  15:52:20  gm]
# 
# $EndLog$

SEC_PROGRAMS		= mount
PROGRAMS		= mount

SEC_ILIST		= mount
ILIST			= mount
IDIR			= /usr/sbin/

CFLAGS			= ${SEC_DEFS} -DOSF1_ADFS -DPFS -DTNC
mount.o_CFLAGS		= -DNFS ${CFLAGS}
OFILES			= mount.o ${SEC_DEFS?mount_sec.o:}
HFILES			= pathnames.h
LIBS			= -lrpc ${SEC_LIBS} -lesize

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
