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
 
# Copyright (C) 1991 Free Software Foundation, Inc.
# This file is part of the GNU C Library.

# The GNU C Library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public License
# as published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.

# GNU Make is distributed "AS IS" in the hope that it will be useful,
# but WITHOUT ANY WARRANTY of any kind; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU Library General Public
# License along with the GNU C Library; see the file COPYING.LIB.  If
# not, write to the Free Software Foundation, Inc., 675 Mass Ave,
# Cambridge, MA 02139, USA.

############################################################################
#
#     Copyright (c) 1988-1992 Intel Corporation
#
#     This file is part of pmake, an extension of GNU Make.
#     Pmake is distributed by the Intel Corporation under the
#     terms of the GNU Library General Public License.
#
#     Title:
#         $Id: Makefile,v 1.3 1994/11/19 01:34:21 mtm Exp $
#
#     Description:
#        Makefile for standalone distribution of libglob.a (fnmatch, glob).
#
############################################################################

LIBRARIES               = libglob.a
#MSGHDRS                 = ksh_msg.h

EXPLIB_TARGETS		=

INCFLAGS                = -I.
CFLAGS                  = -D_MACHINE_=\"PARAGON\" -D_machine_=\"paragon\" \
			  -D_CPUTYPE_=\"I860\" -D_cputype_=\"i860\"       \
			  -DPMAKE -DPOSIX -DPARAGON -D_NO_PROTO -DV8REGEXP

HFILES                  = glob.h fnmatch.h
OFILES                  = glob.o fnmatch.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/libs.mk
include ${MAKEFILEPATH}/objects.mk

include ${MAKEFILEPATH}/depend.mk
-include Makedep
