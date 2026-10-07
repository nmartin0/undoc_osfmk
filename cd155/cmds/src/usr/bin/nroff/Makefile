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

VPATH			= ../troff

PROGRAMS		= nroff

ILIST			= nroff
IDIR			= /usr/bin/

INCFLAGS		= -I../troff
CFLAGS			= -DNROFF -DVMUNIX
#hytab.o_CFLAGS		= -R
#ntab.o_CFLAGS		= -R
#suftab.o_CFLAGS	= -R
HFILES			= d.h tw.h v.h tdef.h sdef.h
OFILES			= ni.o nii.o n1.o n2.o n3.o n4.o n5.o n6.o n7.o \
			  n8.o n9.o n10.o hytab.o suftab.o ntab.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
