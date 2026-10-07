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
# (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
#
# OSF/1 Release 1.0
# HISTORY
#
# $Log: Makefile,v $
# Revision 1.12  1994/11/19  02:33:41  mtm
# Copyright additions/changes
#
# Revision 1.11  1994/07/27  23:34:02  shane
#  Reviewer:None
#  Risk:Low
#  Benefit or PTS #:Allows use of -lpthreads with 4.5 comp. in 1.3 environment
#  Testing:Successful link with new libpthreads and 4.5 compilers.
#  Module(s):init_dummy.o added to object lisT
#
# Revision 1.10  1994/06/27  23:15:15  shala
#  Reviewer: None
#  Risk: Medium
#  Benefit or PTS #: Bug #10012
#  Testing: Build new libc, libc_r, libpthreads. Tested application. Ran
#           pthreads EATs.
#  Module(s): libc/malloc.c
#             libc_r/Makefile
#             libc_r/libc_locks.c
#             libpthreads/Makefile
#             libpthreads/cond.c
#             libpthreads/lib_lock.c
#             libpthreads/mutex.c
#             libpthreads/pthread.c
#             libpthread/pthread_attr.c
#             libpthread/specific.c
#             libpthread/vp.c
#             libpthread/malloc.c     removed
#             libpthread/pmalloc.     added
#
# Made reentrant version of libc malloc(). It gives much better performance
# than pthread version of malloc(). Renamed pthread malloc() to pmalloc() and
# used that for internal pthread calls.
#
# Revision 1.9  1993/12/20  19:42:12  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.8  1993/10/22  17:04:36  stans
#    Still some funn problems with dpendencies; removed include Makedep.
#
# Revision 1.7  1993/10/22  16:56:51  stans
#    Added dependency file inclusion 'Makedep' and processing.
#
# Revision 1.6  1993/08/05  22:33:27  stans
#    Created pthreads versions of rfork() and rforkmulti(); "remoteFork.c".
#    The rfork*() rtns depend on pthread versions of the rfork*.s syscall
#    interfaces; see libpthreads/IPSC860/machdep.mk and libnx/IPSC860/rfork*.s
#
# Revision 1.5  1993/08/03  18:05:15  stans
#    Added/commented debug flavors.
#
# Revision 1.4  1993/05/04  00:44:41  stans
# Mach 3.0 conversion
#
#

VPATH		= ${TARGET_MACHINE}

LIBRARIES	= libpthreads.a

EXPLIB_TARGETS	= export_libpthreads.a

INCFLAGS	= -I.

# Debug flags: -DDEBUG for FULL on debug output, -DDEBUG2 for slim version
#CFLAGS		= -DDEBUG2

ILIST		= libpthreads.a
IDIR		= /usr/ccs/lib/
IMODE		= 644

OFILES		= cond.o evt.o fork.o remoteFork.o lib_lock.o mutex.o \
		  pmalloc.o pthread.o pthread_attr.o sched.o signal.o \
		  specific.o spin.o stack.o vp.o init_dummy.o\
		  ${${TARGET_MACHINE}_OFILES}

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/libs.mk
include ${MAKEFILEPATH}/objects.mk
include ${TARGET_MACHINE}/machdep.mk

include ${MAKEFILEPATH}/depend.mk

-include Makedep


