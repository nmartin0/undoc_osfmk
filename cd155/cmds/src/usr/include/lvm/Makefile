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

OSF1AD_SYS      = ${MAKETOP}/osf1ad/svr/src/latest/server/lvm
VPATH           = ${OSF1AD_SYS}:${MAKETOP}/kernel/lvm

DATAFILES		= ltg.h lv_defect.h lv_pvq.h lv_q.h lv_qmacros.h \
			  lvm.h lvmd.h pvres.h vgres.h vgsa.h
INCLUDES		= ${DATAFILES}

ILIST			= ${INCLUDES}
IDIR			= /usr/include/lvm/
IMODE			= 644

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
