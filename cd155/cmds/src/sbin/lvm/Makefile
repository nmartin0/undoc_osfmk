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

VPATH			= lvmcmdhdr:${MAKETOP}/usr/sbin/newfs

SUBDIRS			= lvmcmdlib
PROGRAMS 		= lvchange lvcreate lvdisplay lvextend lvreduce \
			  lvremove lvsync pvchange pvcreate pvdisplay \
			  pvmove vgchange vgcreate vgdisplay vgextend \
			  vgreduce vgremove vgsync
MSGHDRS			= lvm_msg.h
CATFILES		= lvm.cat

ILIST			= ${PROGRAMS}
IDIR			= /usr/sbin/

USE_STATIC_LIBRARIES	=

INCFLAGS		= -Ilvmcmdhdr -I${MAKETOP}/usr/sbin/newfs
CFLAGS			=
HFILES			= debug.h dump.h logvol.h lvmcmds.h lvmdefmsg.h \
			  lvmtab.h maps.h options.h user_interf.h utilities.h \
			  ${MSGHDRS}
LIBS			= lvmcmdlib/lvmcmdlib.a

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
