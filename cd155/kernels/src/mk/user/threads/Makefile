# 
# $Copyright
# Copyright 1991 , 1994, 1995 Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
#
# Mach Operating System
# Copyright (c) 1991,1990,1989 Carnegie Mellon University
# All Rights Reserved.
# 
# Permission to use, copy, modify and distribute this software and its
# documentation is hereby granted, provided that both the copyright
# notice and this permission notice appear in all copies of the
# software, derivative works or modified versions, and any portions
# thereof, and that both notices appear in supporting documentation.
# 
# CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
# CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
# ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
# 
# Carnegie Mellon requests users of this software to return to
# 
#  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
#  School of Computer Science
#  Carnegie Mellon University
#  Pittsburgh PA 15213-3890
# 
# any improvements or extensions that they make and grant Carnegie Mellon
# the rights to redistribute these changes.
#
#
# HISTORY
# $Log: Makefile,v $
# Revision 1.6  1994/11/18  21:06:44  mtm
# Copyright additions/changes
#
# Revision 1.5  1993/06/30  23:05:59  dleslie
# Adding copyright notices required by legal folks
#
# Revision 1.4  1993/04/27  21:00:08  dleslie
# Copy of R1.0 sources onto main trunk
#
# Revision 1.1.10.2  1993/04/22  19:07:04  dleslie
# First R1_0 release
#
# Revision 1.2  1993/02/12  21:38:05  dleslie
# Added mkidinfo version string stuff
#
# Revision 1.1  1993/01/06  23:55:58  dleslie
# Initial revision
#
# Revision 2.18.2.1  92/03/28  10:36:29  jeffreyh
# 	Pick up changes from MK71
# 	[92/03/20  13:37:27  jeffreyh]
# 
# Revision 2.20  92/03/05  22:46:34  rpd
# 	Changed to use double-colon rules for top-level targets.
# 	[92/02/28            rpd]
# 
# Revision 2.19  92/01/23  15:22:53  rpd
# 	Revised for new Makefile organization.
# 	[92/01/16            rpd]
# 

include ${MAKETOP}Makefile-common

# find machine-dependent files in machine subdirectory

VPATH = .:${cpu}

all :: libthreads.a

install :: ${INSTALLDIR}/lib/libthreads.a

release :: ${TRELEASEDIR}/lib/libthreads.a

clean ::
	${RM} ${INSTALLDIR}/lib/libthreads.a

# installation rules

${INSTALLDIR}/lib/libthreads.a : libthreads.a
	${RM} $@
	${CP} $? $@
	${CHMOD_LIB} $@

# release rules

${TRELEASEDIR}/lib/libthreads.a : ${FRELEASEDIR}/lib/libthreads.a
	${RM} $@
	${CP} $? $@

# build rules

OBJS = cprocs.o cthreads.o malloc.o \
       mig_support.o stack.o sync.o \
       thread.o lock.o csw.o

libthreads.a : ${OBJS}
	${RM} $@ _cvs_id.c _cvs_id.o
	mkidinfo -f libthreads.a
	${CC} -c _cvs_id.c
	${AR} cq $@ _cvs_id.o ${OBJS}
	${RANLIB} $@

-include Makedep
