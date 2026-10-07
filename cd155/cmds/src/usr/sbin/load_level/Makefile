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
# Copyright (c) 1993, Intel Corporation
# All rights reserved
#
# $Log: Makefile,v $
# Revision 1.13  1994/11/19  03:07:39  mtm
# Copyright additions/changes
#
# Revision 1.12  1993/12/20  19:53:24  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: create accurate make dependencies for commands/libraries
#  Testing: built all commands and libraries
#  Module(s): zillions of Makefiles
#
# Revision 1.11  1993/11/18  20:25:53  dleslie
#  Reviewer:shala
#  Risk: low
#  Benefit or PTS #: new cmds/libs build scheme
# 	change path to load_leveld sources
#  Testing: built on Suns and 486
#  Module(s): Makefile
#
# Revision 1.10  1993/10/14  16:55:50  shala
# Made the include files in the obj directory of the cmds/libs be searched
# before include files from kernel. This is to fix problem with the R1.2
# build of the load_level in cmds/libs.
#
# Revision 1.9  1993/09/30  18:34:40  stefan
# Include -lcthreads in LIBS for multi-threaded load leveler.
#
# Revision 1.8  1993/06/29  15:47:46  shala
# Removed mach3 dependencies.
#
# Revision 1.7  1993/05/21  20:58:59  stefan
# Integrated static load leveling support.
#
#

PROGRAMS		= load_leveld

ILIST			= load_leveld
IDIR			= /usr/sbin/
IMODE			= 755

OFILES                  = osf1_dep.o load_leveld.o load_level_mig.o \
			  sll_osf1_dep.o sll_load_leveld.o

HFILES			= load_level_com.h load_level_types.h osf1_dep_types.h \
			  load_level_server.c load_level_user.c \
			  sll_load_level_types.h

OPTIMIZE		= -O
DEPENDS			= -MD
DEPENDS1		= -DOSF -DCOMPAT_43 -DOSF1_ADFS -DOSF1_SERVERSRC \
			  -DSECOND_SERVERSRC
DEPENDS2		= -DSUN_RPC -DTNC -D__STDC__  -D_NO_PROTO
DEFINES			= -UCMUCS -D_BSD -D_NLCTYPE_H_ -DMACH_IPC_COMPAT=0 -DSLL
DEBUG			= -DMIGRATELOG

CFLAGS			= $(DEPENDS) $(DEPENDS1) $(DEPENDS2) $(OPTIMIZE) \
			  $(DEFINES) $(DEBUG)

MKINC			= -I../../include ${INCDIRS} -I${MKREL}/include
MKLIB			= ${MKREL}/lib

INCFLAGS		= $(MKINC)

LIBS			= -lnx -lcthreads -lmach

SERVERSRC		= src



MIGINC			= $(MKINC)

MIGFLAGS = $(DEPENDS) $(DEPENDS1) $(DEPENDS2) $(DEFINES) $(MIGINC)

load_level.defs: $(SERVERSRC)/load_level.defs
	rm -f $@ 
	cp $? $@

load_level_mig.c: $(SERVERSRC)/load_level_mig.c
	rm -f $@ 
	cp $? $@

load_level_com.h: $(SERVERSRC)/load_level_com.h
	rm -f $@
	cp $? $@

load_level_types.h: $(SERVERSRC)/load_level_types.h
	rm -f $@ 
	cp $? $@

load_leveld.c: $(SERVERSRC)/load_leveld.c
	rm -f $@ 
	cp $? $@

osf1_dep_types.h: $(SERVERSRC)/osf1_dep_types.h
	rm -f $@ 
	cp $? $@

osf1_dep.c: $(SERVERSRC)/osf1_dep.c
	rm -f $@ 
	cp $? $@

sll_load_level_types.h: $(SERVERSRC)/sll_load_level_types.h
	rm -f $@ 
	cp $? $@

sll_load_leveld.c: $(SERVERSRC)/sll_load_leveld.c
	rm -f $@ 
	cp $? $@

sll_osf1_dep.c: $(SERVERSRC)/sll_osf1_dep.c
	rm -f $@ 
	cp $? $@


load_level_server.c load_level_user.c : load_level.defs
	mig $(MIGFLAGS) -header /dev/null \
	-user load_level_user.c -server load_level_server.c \
		load_level.defs

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk

include ${MAKEFILEPATH}/depend.mk
-include Makedep
