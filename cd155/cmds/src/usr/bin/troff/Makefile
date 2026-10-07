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
# $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/troff/Makefile,v 1.3 1994/11/19 01:43:52 mtm Exp $
#
# OSF/1 Release 1.0

PROGRAMS		= troff

ILIST			= troff
IDIR			= /usr/bin/

CFLAGS			= -DVMUNIX
#hytab.o_CFLAGS		= -R
#suftab.o_CFLAGS	= -R
HFILES			= d.h tw.h v.h tdef.h sdef.h
OFILES			= ni.o nii.o n1.o n2.o n3.o n4.o n5.o t6.o n7.o \
			  n8.o n9.o t10.o hytab.o suftab.o tab3.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
