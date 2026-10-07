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

LIBRARIES		= libksh.a
MSGHDRS			= ksh_msg.h

EXPLIB_TARGETS		=

INCFLAGS		= -I../include -I..
CFLAGS                  = -DKSHELL -DKSH_88D
HFILES			= ksh_msg.h
OFILES			= adjust.o assign.o assnum.o cannon.o chkid.o \
			  convert.o findnod.o gettree.o growaray.o \
			  gsort.o linknod.o msgcat.o namscan.o optget.o \
			  rjust.o strdata.o streval.o strmatch.o tilde.o \
			  unassign.o utos.o valup.o 

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/libs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
