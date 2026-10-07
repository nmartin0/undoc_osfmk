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
# OSF/1 Release 1.0.3

VPATH			= ${SVROBJ}/include/uxkern

DATAFILES		= bsd_msg.h bsd_types.h \
			  credentials.h device.h device_reply_hdlr.h \
			  device_utils.h fsvr_generic.h fsvr_types.h \
			  import_mach.h mf.h port_hash.h proc_to_port.h \
			  proc_to_task.h rmtnode.h rproc.h server_mach_msg.h \
			  sthread.h syscall_subr.h syscalltrace.h vm_param.h

INCLUDES		= ${DATAFILES}

ILIST			= ${INCLUDES}
IDIR			= /usr/include/uxkern/
IMODE			= 644

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
