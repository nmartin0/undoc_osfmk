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

OSFMK = ${MKREL}/include/device:${MKREL}/include/ipi:${MKREL}/include/i860paragon/hippi:${MKREL}/include/scsi

VPATH = ${OSFMK}

DATAFILES		= device.h device_reply.h device_request.h \
			  device_types.h net_status.h tape_status.h \
			  tty_status.h disk_status.h ipi_status.h \
			  hippi_status.h scsi_status.h

INCLUDES		= ${DATAFILES}

ILIST			= ${INCLUDES}
IDIR			= /usr/include/device/
IMODE			= 644

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
