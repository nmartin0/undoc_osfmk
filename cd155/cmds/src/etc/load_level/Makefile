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
#      $Header: /afs/ssd/i860/CVS/cmds_libs/src/etc/load_level/Makefile,v 1.7 1994/11/21 16:38:07 mtm Exp $
#
#

OSF1AD_LL	= ${MAKETOP}/usr/sbin/load_level/src
VPATH		= ${OSF1AD_LL}

SUBDIRS			= shm_keys
DATAFILES		= migrate_commands parameters

ILIST			= ${DATAFILES}
IDIR			= /etc/load_level/
IMODE			= 644

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
