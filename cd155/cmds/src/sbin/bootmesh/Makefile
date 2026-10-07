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
#      $Header: /afs/ssd/i860/CVS/cmds_libs/src/sbin/bootmesh/Makefile,v 1.16 1994/11/21 16:39:43 mtm Exp $
#
#

PROGRAMS		= bootmesh epoch_time 
DATAFILES               = bootmesh.sh

MSGHDRS			= 
CATFILES		= 

ILIST			= ${DATAFILES} bootmesh epoch_time
IDIR			= /sbin/

SVRDIR                        = ${SVROBJ}/${SVRCONFIG}

HFILES			= bootmesh.h nic.h coff.h

INCFLAGS                = 

CFLAGS 			= -O -DBUMPER -DSLOWDOWN #-DTEST

LIBS                    = -lnx -lmach -lnetname 

SVR_OBJS              = nx_create_attr.o

bootmesh_OFILES         = main.o boot.o kernops.o coff.o magic.o nic.o \
			  boot_send.o ${SVR_OBJS}

epoch_time_OFILES       = epoch_time.o

${SVR_OBJS}: ${SVRDIR}/$$@
	for i in ${SVR_OBJS}; do cp ${SVRDIR}/$$i .; done

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
