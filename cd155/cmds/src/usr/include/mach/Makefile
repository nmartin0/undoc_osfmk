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
# (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
#
# OSF/1 Release 1.0.2
#
# Mach 3.0 conversion

VPATH			= ${MKREL}/include/mach

SUBDIRS			= ${MACHINE}

EXPINC_SUBDIRS		= ${SUBDIRS}

DATAFILES               = boolean.h default_pager_object.h error.h \
                          exc.h exception.h host_info.h \
                          kern_return.h mach.h mach_host.h \
                          mach_interface.h mach_param.h \
                          mach_port.h mach_traps.h mach_types.h \
                          machine.h mcmsg_info.h memory_object.h \
                          memory_object_default.h memory_object_user.h \
                          message.h mig_errors.h msg_type.h \
                          norma_special_ports.h notify.h notify_user.h \
                          policy.h port.h processor_info.h \
                          prof_types.h profilparam.h std_types.h \
                          syscall_sw.h task_info.h task_special_ports.h \
                          thread_info.h thread_special_ports.h \
                          thread_status.h thread_switch.h \
                          time_value.h vm_attributes.h vm_inherit.h \
                          vm_param.h vm_prot.h vm_statistics.h \
			  std_types.defs mach.defs mach_types.defs \
			  mach_host.defs mach_norma.defs mach_port.defs \
			  norma_task.defs notify.defs prof.defs exc.defs

MIG_HDRS                = ${MIG_DEFS:.defs=.h}

OTHERS			= machine

INCLUDES		= ${DATAFILES} ${OTHERS} ${MIG_HDRS}

ILIST			= ${INCLUDES}
IDIR			= /usr/include/mach/
IMODE			= 644

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk
include ${MAKEFILEPATH}/datafiles.mk

#
# [ Wed Sep  9 16:02:24 PDT 1992 ] shala & Stan
# When $OSFMK was introduced a real "machine" directory is created. We do NOT
# understand why at this time but it needs to go away and be a symbolic
# link. Here we force the removal of the real directory "machine" so the
# symlink can proceed.
#
machine: ${ALWAYS}
	${RM} -rf machine
	ln -s ${MACHINE} machine

include ${MAKEFILEPATH}/depend.mk
-include Makedep
