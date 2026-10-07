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

VPATH			= ..

dofile.o_CFLAGS		= -DPDA

MSGHDRS			= cm_msg.h co_msg.h delta_msg.h

INCFLAGS		= -I..

LIBRARIES		= libsccs.a
OFILES			= \
			  auxf.o\
			  chkid.o\
			  chksid.o\
			  date_ab.o\
			  date_ba.o\
			  del_ab.o\
			  del_ba.o\
			  dodelt.o\
			  dofile.o\
			  dohist.o\
			  doie.o\
			  dolist.o\
			  eqsid.o\
			  flushto.o\
			  fmalloc.o\
			  fmterr.o\
			  getline.o\
			  logname.o\
			  newstats.o\
			  pf_ab.o\
			  putline.o\
			  rdmod.o\
			  setup.o\
			  sid_ab.o\
			  sid_ba.o\
			  sidtoser.o\
			  sinit.o\
			  permiss.o\
			  stats_ab.o ${NEED_FOR_PORTING}

HFILES			= ${MSGHDRS}

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/libs.mk
include ${MAKEFILEPATH}/objects.mk

auxf.o: defines.h
dohist.o: delta_msg.h
include ${MAKEFILEPATH}/depend.mk
-include Makedep
