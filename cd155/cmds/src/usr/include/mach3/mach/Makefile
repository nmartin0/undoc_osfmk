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
# (c) Copyright 1992, INTEL, CORP.
# ALL RIGHTS RESERVED
#
#
# INTEL Release 1.0
#
# $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/mach3/mach/Makefile,v 1.7 1994/11/19 02:42:56 mtm Exp $
#
MK_I860			= ${MAKETOP}/mk/release/i860_mach/include/mach

VPATH			= ${MK_I860}

SUBDIRS                 = ${MACHINE}
EXPINC_SUBDIRS          = ${SUBDIRS}

DATAFILES		= boolean.h default_pager_object.h error.h \
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
			  default_pager_object.defs exc.defs mach.defs \
		  	  mach_host.defs mach_norma.defs mach_port.defs \
			  mach_types.defs memory_object.defs \
			  memory_object_default.defs norma_task.defs \
			  notify.defs prof.defs std_types.defs

OTHERS                  = machine

INCLUDES		= ${DATAFILES} ${OTHERS}

ILIST			= ${INCLUDES}
IDIR			= /usr/include/mach3/mach/
IMODE			= 644

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk
include ${MAKEFILEPATH}/datafiles.mk

machine: ${ALWAYS}
	${RM} -rf machine
	ln -s ${MACHINE} machine
include ${MAKEFILEPATH}/depend.mk
-include Makedep
