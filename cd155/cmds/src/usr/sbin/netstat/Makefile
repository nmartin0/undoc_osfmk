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
# Revision 1.6  1994/11/19  03:12:36  mtm
# Copyright additions/changes
#
# Revision 1.5  1994/06/17  15:27:25  paul
# Added changes for TNC. -I <unit> now supports TNC style interface names
# such as <201>el0 where the node number which has the interface is included
# in the name. At the same time, the server changed to return the long form
# of the interface name, and because these names are longer, the hippi
# interface name no longer fits in the 7 byte wide print field, so now this
# utility does two passes in order to determine the maximum width interface
# name. It then makes the field wider than 7 if necessary.
#
# Also, added a -D switch which in conjunction with the -r switch produces
# routes including all the _mi invisible routes. This is probably only of
# interest to developers, but may prove useful to people tuning applications.
#
#  Reviewer: Bernie Keany
#  Risk: L
#  Benefit or PTS #: 8808 9892
#  Testing: 1, 2, & 3 netserver systems, on boot/nonboot configurations
#  Module(s): if.c main.c route.c Makefile
#
# Revision 1.4  1993/12/20  19:54:16  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.3  1993/06/07  16:21:11  shala
# Made unix.o with the same CFLAGS as the rest of the objects.
#
# Revision 1.2  1992/12/11  01:39:05  shala
# Support New table call.
#
# Revision 2.2  92/11/18  14:34:57  loverso
# 	From 1.0.4
# 
# Revision 1.10  90/10/07  22:34:18  devrcs
# 	Added EndLog Marker.
# 	[90/09/29  15:50:34  gm]
# 
# Revision 1.9  90/08/09  14:58:13  devrcs
# 	Cleanup for export, install and post-freeze.
# 	[90/08/05  13:42:53  gm]
# 
# $EndLog$

PROGRAMS		= netstat

ILIST			= netstat
IDIR			= /usr/sbin/
IGROUP			= kmem
IMODE			= 2755

CFLAGS                  = -DOSF1_ADFS -DOSF1AD -DTNC

OFILES			= host.o inet.o if.o main.o mbuf.o route.o \
			  unix.o ns.o iso.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

include ${MAKEFILEPATH}/depend.mk
-include Makedep
