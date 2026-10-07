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
 
# Copyright (C) 1988, 1989, 1991 Free Software Foundation, Inc.
# This file is part of GNU Make.
#
# GNU Make is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# GNU Make is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Make; see the file COPYING.  If not, write to
# the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

############################################################################
#
#    Copyright (c) 1988-1992 Intel Corporation
#
#    This file is part of pmake, an extension of GNU Make.
#    Pmake is distributed by the Intel Corporation under the
#    terms of the GNU General Public License.
#
#    Title:
#        $Id: Makefile,v 1.4 1994/11/19 01:33:30 mtm Exp $
#
#    Description:
#        Makefile for Paragon version of pmake.
#
###########################################################################

SUBDIRS                 = glob 
PROGRAMS                = pmake

ILIST                   = ${PROGRAMS}
IDIR                    = /usr/bin/

INCFLAGS                = -I.
CFLAGS                  = -D_MACHINE_=\"PARAGON\" -D_machine_=\"paragon\" \
			  -D_CPUTYPE_=\"I860\" -D_cputype_=\"i860\"       \
			  -DPMAKE -DPOSIX -DPARAGON -D_NO_PROTO -DV8REGEXP

HFILES                  = commands.h dep.h file.h job.h make.h regexp.h \
			  regexpdefs.h regmagic.h rule.h variable.h
OFILES                  = commands.o default.o dir.o expand.o file.o	\
			  function.o implicit.o job.o load.o misc.o	\
			  main.o makeconf.o nx.o read.o regexp.o	\
			  regsub.o remake.o remote.o rule.o variable.o	\
			  vpath.o version.o arscan.o ar.o

LIBFLAGS                = -Lglob 
LIBS                    = -lglob -lnx -lmach

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

include ${MAKEFILEPATH}/depend.mk
-include Makedep
