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
# (c) Copyright 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
# ALL RIGHTS RESERVED 
#
#
# OSF/1 Release 1.0.4

# @(#)Makefile	2.1 16:15:31 4/20/90 SecureWare

LIBRARIES	= libsecurity.a
MSGHDRS		= libsec_msg.h
CATFILES	= libsec.cat

EXPLIB_TARGETS	= export_libsecurity.a

ILIST		= libsecurity.a
IDIR		= /usr/ccs/lib/
IMODE		= 644

CFLAGS		= ${SEC_DEFS}
HFILES		= ${MSGHDRS} libsecurity.h
OFILES		= accept_pw.o acllib.o auditlib.o authaudit.o \
		  authcap.o chacl.o chilabel.o chslabel.o compress.o copyacl.o \
		  data.o discr.o \
		  disk.o fields.o getclrnce.o getdvagent.o getilabel.o \
		  getpasswd.o getprdfent.o getprfient.o getprlpent.o \
		  getprpwent.o getprtcent.o getslabel.o identity.o \
		  lchilabel.o lchslabel.o lstatilabel.o lstatslabel.o \
		  mand_init.o mand_tag.o mandlib.o map_ids.o mld.o \
		  msg_acl.o msg_ilabel.o msg_slabel.o passlen.o printbuf.o \
		  privileges.o psecerror.o randomword.o sec_errlst.o \
		  security.o seed.o sem_acl.o \
		  sem_ilabel.o sem_slabel.o setclrnce.o setilabel.o \
		  setslabel.o shm_acl.o shm_ilabel.o shm_slabel.o \
		  statacl.o statilabel.o statslabel.o std_labels.o \
		  strdup.o subsystems.o tod.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/libs.mk
include ${MAKEFILEPATH}/objects.mk






include ${MAKEFILEPATH}/depend.mk
-include Makedep
