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

CCSBIN			= ${MAKETOP}/usr/ccs/bin
VPATH			= ${CCSBIN}/common:${CCSBIN}/cflow

SUBDIRS			= cflow1
PROGRAMS		= dag flip lpfx nmf
#
# cflow.msg comes from usr/ccs/bin/cflow
#
MSGHDRS			= cflow_msg.h ctools_msg.h
CATFILES		= cflow.cat ctools.cat

ILIST			= dag flip lpfx nmf
IDIR			= /usr/ccs/lib/

INCFLAGS		= -I${CCSBIN}/common -I${CCSBIN}/cflow
CFLAGS			= -DHOSTIEEE -DCFLOW2

lpfx_OFILES		= lpfx.o tftn.o xdefs.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/subdirs.mk
include ${MAKEFILEPATH}/objects.mk

dag.o:	cflow_msg.h
lpfx.o: mfile1.h error.h ctools_msg.h
tftn.o:	ctools_msg.h
include ${MAKEFILEPATH}/depend.mk
-include Makedep
