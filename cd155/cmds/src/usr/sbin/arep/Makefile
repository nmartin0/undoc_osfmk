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
 

#              INTEL CORPORATION PROPRIETARY INFORMATION
#
#  This software is supplied under the terms of a license
#  agreement or nondisclosure agreement with Intel Corporation
#  and may not be copied or disclosed except in accordance
#  with the terms of that agreement.
#
#
#      Copyright 1992  Intel Corporation.
#
#
# HISTORY
# $Log: Makefile,v $
# Revision 1.2  1994/11/19  03:04:36  mtm
# Copyright additions/changes
#
# Revision 1.1  1993/12/15  05:06:17  jkearns
# Initial revision
#
# Revision 1.0  1993/10/19  10:40:00 jk
# Initial revision
#
#

PROGRAMS		= arep

IDIR		= /usr/bin/
IOWNER		= root
IMODE		= 6111

IPATH			= ../../include
INCFLAGS                = -I$(IPATH) -I$(IPATH)/macs
CFLAGS			= $(DBG) -DIPSC ${${TARGET_MACHINE}CFLAGS} 
MACS_LIBS		= ../../ccs/lib/libmacs/libmac.a \
			  ../../ccs/lib/libmacs/librin.a

arep_OFILES		= arep.o $(MACS_LIBS) 


include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/datafiles.mk

