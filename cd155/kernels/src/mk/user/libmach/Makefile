# 
# $Copyright
# Copyright 1991 , 1994, 1995 Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
#
# @OSF_FREE_COPYRIGHT@
#
#
# HISTORY
# $Log: Makefile,v $
# Revision 1.13  1994/11/18  21:04:31  mtm
# Copyright additions/changes
#
# Revision 1.12  1994/07/12  19:31:21  andyp
# Merge of the NORMA2 branch back to the mainline.
#
# Revision 1.11.8.3  1994/03/02  18:43:58  andyp
# Added additional user-mode bindings for RDMA testing.
#
# Revision 1.11.8.2  1994/02/07  19:56:03  andyp
# Added user-mode bindings for RDMA testing.
#
# Revision 1.11.8.1  1994/02/04  07:58:55  andyp
# Purged some clutter found in the repository.  Added some temporary
# user-mode test points for RPC into libmach.
#
# Revision 1.11  1993/06/30  23:02:48  dleslie
# Adding copyright notices required by legal folks
#
# Revision 1.10  1993/06/18  19:18:53  andyp
# Needed another Makefile mod and two source files for the device_overwrite()
# suite of calls.
#
# Revision 1.9  1993/06/17  20:05:39  andyp
# Picked up the device_read_overwrite() family of calls.
#
# OSF Log as of 1993/06/17 included below:
# 	HISTORY
# 	Log: Makefile,v
# 	Revision 1.1.12.4  1993/04/22  19:39:30  rod
# 		Merge with NMK13 latest.
# 		[1993/04/22  19:07:08  rod]
#
# 		device_read_overwrite support.
# 		[1993/03/12  10:34:51  rod]
#
# 	Revision 1.1.12.3  1993/04/15  23:02:14  alanl
# 		Paging flow control (NORMA_VM).  Added new message,
# 		memory_object_data_write_completed.  [alanl, sjs]
# 		[1993/04/15  22:15:13  alanl]
# 
# 	Revision 1.1.12.2  1993/04/08  22:08:24  dwm
# 		Added device_read{_request} and device_write{_request} as
# 		traps.
# 		[1993/04/08  18:36:31  dwm]
# 
# 	Revision 1.1  1992/09/30  02:38:49  robert
# 		Initial revision
# 
# $EndLog$
#
# Revision 1.8  1993/06/08  21:38:04  regnier
# Generate i860paragon/mcmsg release area.
#
# Revision 1.7  1993/05/07  20:36:33  SSD
# Include Mach 2.5 compat routine xxx_host_info() for Mach 2.5 commands.
#
# Revision 1.6  1993/05/07  17:20:38  stans
# Added 'xxx_slot_info()', Mach 2.5 compat routine uutilized by some commands.
#
# Revision 1.5  1993/04/27  20:57:00  dleslie
# Copy of R1.0 sources onto main trunk
#
# Revision 1.2.6.3  1993/04/27  00:21:32  dleslie
# Patch release of April 23
#
# Revision 1.2  1993/04/12  18:08:00  SSD
# pager flow control fixes.
#
# Revision 1.3  1993/02/12  21:38:08  dleslie
# Added mkidinfo version string stuff
#
# Revision 1.2  1993/01/08  00:53:19  dleslie
# SSD value added files
#
# CMU_HIST 
#
# Revision 2.21.3.3  92/09/15  17:37:35  jeffreyh
# 	Added rules for prof.h prof_user.c prof_server.c
# 	Renamed mach_sample_thread and mach_sample_task
# 	[92/07/24            bernadat]
# 
# Revision 2.21.3.2  92/04/08  15:47:02  jeffreyh
# 	Add default_pager_object routines.
# 	[92/04/01            jeffreyh]
# 
# Revision 2.21.3.1  92/03/28  10:17:39  jeffreyh
# 	Put crto.o back into libmach_sa.a so that server Makefiles do not
# 	need to change.
# 	[92/03/25            jeffreyh]
# 
# 	Changes to make AT386 work as well as I386
# 	[92/03/23            jeffreyh]
# 	Added host_get_boot_info, mach_sample_thread, and
# 	mach_sample_task to MACH_HOST_ROUTINES
# 	[92/03/11            jeffreyh]
# 
# 	Removed default pager and bootstrap functionality.
# 	[92/03/06            jeffreyh]
# 
# Revision 2.30  92/03/05  22:46:21  rpd
# 	Added bootstrap functions.
# 	[92/03/02            rpd]
# 
# 	Changed to use double-colon rules for top-level targets.
# 	[92/02/28            rpd]
# 
# Revision 2.29  92/02/20  13:42:51  elf
# 	Added default_pager_paging_file.
# 	[92/02/19  17:37:12  af]
# 
# Revision 2.28  92/02/19  16:07:29  elf
# 	Added thread_depress_abort.
# 	Added i386/memcpy.s.
# 	[92/02/19            elf]
# 
# Revision 2.27  92/02/19  15:10:38  elf
# 	Added mach_error_string.o, htg_vm_map.o, panic.o,
# 	doprnt.o, printf.o, sprintf.o, get_privileged_ports.o.
# 	[92/02/11            rpd]
# 
# Revision 2.26  92/02/19  10:51:30  elf
# 	Added SA_LUNA88K_OBJS. Updated Copyright.
# 	[92/02/19            danner]
# 
# Revision 2.25  92/01/24  00:50:04  rpd
# 	Fixed exc_server.o to have type-checking.
# 	[92/01/23            rpd]
# 
# Revision 2.24  92/01/23  15:21:49  rpd
# 	Revised for new Makefile organization.
# 	[92/01/16            rpd]
# 
# CMU_ENDHIST 
#
# Mach Operating System
# Copyright (c) 1992,1991,1990,1989 Carnegie Mellon University
# All Rights Reserved.
# 
# Permission to use, copy, modify and distribute this software and its
# documentation is hereby granted, provided that both the copyright
# notice and this permission notice appear in all copies of the
# software, derivative works or modified versions, and any portions
# thereof, and that both notices appear in supporting documentation.
# 
# CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
# CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
# ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
# 
# Carnegie Mellon requests users of this software to return to
# 
#  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
#  School of Computer Science
#  Carnegie Mellon University
#  Pittsburgh PA 15213-3890
# 
# any improvements or extensions that they make and grant Carnegie Mellon
# the rights to redistribute these changes.
#  
#
#

include ${MAKETOP}Makefile-common

# find machine-dependent files in machine subdirectory
# find mig source files, mach/syscall_sw.h on CPATH

VPATH = .:${cpu}:${CPATH}

# top-level targets

TARGETS = libmach.a libmach_sa.a crt0.o
ITARGETS = ${TARGETS;.*;${INSTALLDIR}/lib/&}
TTARGETS = ${TARGETS;.*;${TRELEASEDIR}/lib/&}

all :: ${TARGETS}

install :: ${ITARGETS}

release :: ${TTARGETS}

clean ::
	${RM} ${ITARGETS}

# installation rules

${INSTALLDIR}/lib/libmach.a : libmach.a
	${RM} $@
	${CP} $? $@
	${CHMOD_LIB} $@

${INSTALLDIR}/lib/libmach_sa.a : libmach_sa.a
	${RM} $@
	${CP} $? $@
	${CHMOD_LIB} $@

${INSTALLDIR}/lib/crt0.o : crt0.o
	${RM} $@
	${CP} $? $@
	${CHMOD_O} $@

# release rules

${TRELEASEDIR}/lib/libmach.a : ${FRELEASEDIR}/lib/libmach.a
	${RM} $@
	${CP} $? $@

${TRELEASEDIR}/lib/libmach_sa.a : ${FRELEASEDIR}/lib/libmach_sa.a
	${RM} $@
	${CP} $? $@

${TRELEASEDIR}/lib/crt0.o : ${FRELEASEDIR}/lib/crt0.o
	${RM} $@
	${CP} $? $@

# normal Mach system calls from mach/syscall_sw.h

MACH_TRAPS = evc_wait mach_msg_trap mach_reply_port mach_thread_self \
	mach_task_self mach_host_self swtch_pri swtch thread_switch

# dangerous RPC test traps from mach/syscall_sw.h
RPC_TEST_TRAPS = syscall_rpc_alloc \
	syscall_rpc_free \
	syscall_rpc_recv \
	syscall_rpc_reply \
	syscall_rpc_send

# dangerous RDMA test traps from mach/syscall_sw.h
RDMA_TEST_TRAPS = syscall_rdma_alloc \
	syscall_rdma_free \
	syscall_rdma_token \
	syscall_rdma_accept \
	syscall_rdma_connect \
	syscall_rdma_send \
	syscall_rdma_recv \
	syscall_rdma_disconnect \
	syscall_rdma_send_busy \
	syscall_rdma_send_ready \
	syscall_rdma_send_done \
	syscall_rdma_send_complete \
	syscall_rdma_recv_busy \
	syscall_rdma_recv_ready \
	syscall_rdma_recv_done \
	syscall_rdma_recv_complete

# system calls from mach/syscall_sw.h which the kernel doesn't implement

UNIXOID_TRAPS = task_by_pid pid_by_task init_process map_fd \
	rfs_make_symlink htg_syscall

# routines from mach/mach.defs which have fast syscall versions

MACH_SYSCALLS = vm_allocate vm_map vm_deallocate task_create \
	task_terminate task_suspend task_set_special_port \
	thread_depress_abort

DEVICE_SYSCALLS = device_read device_write

DEVICE_REQ_SYSCALLS = device_read_request device_write_request

DEVICE_OVERWRITE_SYSCALLS = device_read_overwrite_request device_read_overwrite
DEVICE_OVERWRITE_REPLY = device_read_reply_overwrite
 
# other routines from mach/mach.defs

MACH_ROUTINES = task_threads thread_terminate vm_protect vm_inherit \
	vm_read vm_write vm_copy vm_region vm_statistics \
	mach_ports_register mach_ports_lookup \
	memory_object_data_provided memory_object_data_unavailable \
	memory_object_get_attributes vm_set_default_memory_manager \
	memory_object_lock_request task_get_emulation_vector \
	task_set_emulation_vector task_resume task_get_special_port \
	task_info thread_create thread_suspend thread_resume \
	thread_abort thread_get_state thread_set_state \
	thread_get_special_port thread_set_special_port thread_info \
	task_set_emulation memory_object_data_error \
	memory_object_set_attributes memory_object_destroy \
	memory_object_data_supply memory_object_ready \
	memory_object_change_attributes vm_machine_attribute \
	memory_object_data_write_completed \
	xxx_slot_info xxx_host_info

# routines from mach/mach_port.defs which have fast syscall versions

MACH_PORT_SYSCALLS = mach_port_allocate mach_port_allocate_name \
	mach_port_deallocate mach_port_insert_right

# other routines from mach/mach_port.defs

MACH_PORT_ROUTINES = mach_port_names mach_port_type mach_port_rename \
	mach_port_destroy mach_port_get_refs mach_port_mod_refs \
	old_mach_port_get_receive_status mach_port_set_qlimit \
	mach_port_set_mscount mach_port_get_set_status \
	mach_port_move_member mach_port_request_notification \
	mach_port_extract_right mach_port_get_receive_status \
	mach_port_set_seqno

# routines from device/device.defs

DEVICE_ROUTINES = device_open device_close \
	device_write_inband device_read_inband \
	device_set_status device_get_status device_set_filter device_map

# routines from device/device_request.defs

DEVICE_REQ_ROUTINES = device_open_request \
	device_write_request_inband \
	device_read_request_inband

# routines from mach/mach_host.defs

MACH_HOST_ROUTINES = host_processors host_info processor_info \
	processor_start processor_exit processor_control \
	processor_set_default xxx_processor_set_default_priv \
	processor_set_create processor_set_destroy processor_set_info \
	processor_assign processor_get_assignment thread_assign \
	thread_assign_default thread_get_assignment task_assign \
	task_assign_default task_get_assignment host_kernel_version \
	thread_priority thread_max_priority task_priority \
	processor_set_max_priority thread_policy \
	processor_set_policy_enable processor_set_policy_disable \
	processor_set_tasks processor_set_threads host_processor_sets \
	host_processor_set_priv host_set_time host_adjust_time \
	host_get_time host_reboot vm_wire thread_wire\
	host_get_boot_info thread_sample task_sample


# routines from mach_debug/mach_debug.defs

MACH_DEBUG_ROUTINES = host_zone_info mach_port_get_srights \
	host_ipc_hash_info host_ipc_marequest_info mach_port_space_info \
	mach_port_dnrequest_info \
	host_stack_usage processor_set_stack_usage \
	host_virtual_physical_table_info host_load_symbol_table \
	mach_port_kernel_object mach_vm_region_info vm_mapped_pages_info
#XXX Not in NORMA yet 
# mach_vm_region_info mach_vm_object_info mach_vm_object_pages 

# routines from mach/default_pager.defs /*XXX not in NORMA yet */

#DPAGER_ROUTINES = default_pager_object_create default_pager_info \
	default_pager_objects default_pager_object_pages \
	default_pager_paging_file
DPAGER_ROUTINES = default_pager_object_create default_pager_info  

# routines from mach/bootstrap.defs /*XXX not in NORMA yet */

#BOOTSTRAP_ROUTINES = bootstrap_privileged_ports

# all system calls from mach/syscall_sw.h

PURE_TRAPS = ${MACH_TRAPS} \
	${RPC_TEST_TRAPS} \
	${RDMA_TEST_TRAPS} \
	${MACH_SYSCALLS/.*/syscall_&} \
	${MACH_PORT_SYSCALLS/.*/syscall_&} \
	${DEVICE_REQ_SYSCALLS/.*/syscall_&} \
	${DEVICE_SYSCALLS/.*/syscall_&} \
	${DEVICE_OVERWRITE_SYSCALLS/.*/syscall_&}

ALL_TRAPS = ${PURE_TRAPS} ${UNIXOID_TRAPS}

# mig rules to generate intermediate source files in the object directory

${ALL_TRAPS/.*/&.cs} : mach/syscall_sw.h
	{ echo '#include <mach/machine/syscall_sw.h>'; \
	  egrep '^kernel_trap\($*,' mach/syscall_sw.h; } > $@

MIG_SYSCALL_SRCS = ${MACH_SYSCALLS/.*/mig_&.c} \
	${MACH_PORT_SYSCALLS/.*/mig_&.c} \
	${DEVICE_REQ_SYSCALLS/.*/mig_&.c} \
	${DEVICE_SYSCALLS/.*/mig_&.c} \
	${DEVICE_OVERWRITE_REPLY/.*/mig_&.c}

${MIG_SYSCALL_SRCS} : $${@/mig_(.*)\.c/\1.c}
	name=${@/mig_(.*)\.c/\1}; \
	sed -e "s/$${name}/mig_$${name}/g" < $${name}.c > mig_$${name}.c

MACH_USER_SRCS = ${MACH_ROUTINES/.*/&.c} ${MACH_SYSCALLS/.*/&.c}
MACH_SRCS = mach_interface.h ${MACH_USER_SRCS} mach_server.c

${MACH_SRCS} : mach/mach.defs
	${MIG} ${MIGFLAGS} -i ./ -user '$${MACH_USER_SRCS}' -header mach_interface.h -server mach_server.c mach/mach.defs

MACH_PORT_USER_SRCS = ${MACH_PORT_ROUTINES/.*/&.c} ${MACH_PORT_SYSCALLS/.*/&.c}
MACH_PORT_SRCS = mach_port.h ${MACH_PORT_USER_SRCS} mach_port_server.c

${MACH_PORT_SRCS} : mach/mach_port.defs
	${MIG} ${MIGFLAGS} -i ./ -user '$${MACH_PORT_USER_SRCS}' -server mach_port_server.c mach/mach_port.defs

DEVICE_USER_SRCS = ${DEVICE_ROUTINES/.*/&.c} ${DEVICE_SYSCALLS/.*/&.c}
DEVICE_SRCS = device.h ${DEVICE_USER_SRCS} device_server.c

${DEVICE_SRCS} : device/device.defs
	${MIG} ${MIGFLAGS} -i ./ -user '$${DEVICE_USER_SRCS}' -server device_server.c device/device.defs

MACH_HOST_USER_SRCS = ${MACH_HOST_ROUTINES/.*/&.c}
MACH_HOST_SRCS = mach_host.h ${MACH_HOST_USER_SRCS} mach_host_server.c

${MACH_HOST_SRCS} : mach/mach_host.defs
	${MIG} ${MIGFLAGS} -i ./ -user '$${MACH_HOST_USER_SRCS}' -server mach_host_server.c mach/mach_host.defs

MACH_DEBUG_USER_SRCS = ${MACH_DEBUG_ROUTINES/.*/&.c}
MACH_DEBUG_SRCS = mach_debug.h ${MACH_DEBUG_USER_SRCS} mach_debug_server.c

${MACH_DEBUG_SRCS} : mach_debug/mach_debug.defs
	${MIG} ${MIGFLAGS} -i ./ -user '$${MACH_DEBUG_USER_SRCS}' -server mach_debug_server.c mach_debug/mach_debug.defs

DPAGER_USER_SRCS = ${DPAGER_ROUTINES/.*/&.c}
#DPAGER_SRCS = default_pager.h ${DPAGER_USER_SRCS}default_pager_server.c
DPAGER_SRCS = ${DPAGER_USER_SRCS}

#${DPAGER_SRCS} : mach/default_pager.defs
#	${MIG} ${MIGFLAGS} -i ./ -user '$${DPAGER_USER_SRCS}' -server default_pager_server.c mach/default_pager.defs

${DPAGER_SRCS} : mach/default_pager_object.defs
	${MIG} ${MIGFLAGS} -i ./ -user '$${DPAGER_USER_SRCS}' -server /dev/null mach/default_pager_object.defs
#BOOTSTRAP_USER_SRCS = ${BOOTSTRAP_ROUTINES/.*/&.c}
#BOOTSTRAP_SRCS = bootstrap.h ${BOOTSTRAP_USER_SRCS} bootstrap_server.c

#${BOOTSTRAP_SRCS} : mach/bootstrap.defs
#	${MIG} ${MIGFLAGS} -i ./ -user '$${BOOTSTRAP_USER_SRCS}' -server bootstrap_server.c mach/bootstrap.defs

EXC_SRCS = exc.h exc_user.c exc_server.c

${EXC_SRCS} : mach/exc.defs
	${MIG} ${MIGFLAGS} -user exc_user.c -server exc_server.c mach/exc.defs

PROF_SRCS = prof.h prof_user.c prof_server.c

${PROF_SRCS} : mach/prof.defs
	${MIG} ${MIGFLAGS} -user prof_user.c -server prof_server.c mach/prof.defs

NOTIFY_SRCS = notify_user.h notify_user.c notify_server.c

${NOTIFY_SRCS} : mach/notify.defs
	${MIG} ${MIGFLAGS} -header notify_user.h -user notify_user.c -server notify_server.c mach/notify.defs

seqnos_notify_server.c : mach/notify.defs
	${MIG} ${MIGFLAGS} -DSEQNOS -header /dev/null -user /dev/null -server seqnos_notify_server.c mach/notify.defs

MEMORY_OBJECT_SRCS = memory_object_user.h \
	memory_object_user.c memory_object_server.c

${MEMORY_OBJECT_SRCS} : mach/memory_object.defs
	${MIG} ${MIGFLAGS} -header memory_object_user.h -user memory_object_user.c -server memory_object_server.c mach/memory_object.defs

seqnos_memory_object_server.c : mach/memory_object.defs
	${MIG} ${MIGFLAGS} -DSEQNOS -header /dev/null -user /dev/null -server seqnos_memory_object_server.c mach/memory_object.defs

MEMORY_OBJECT_DEFAULT_SRCS = memory_object_default.h \
	memory_object_default_user.c memory_object_default_server.c

${MEMORY_OBJECT_DEFAULT_SRCS} : mach/memory_object_default.defs
	${MIG} ${MIGFLAGS} -header memory_object_default.h -user memory_object_default_user.c -server memory_object_default_server.c mach/memory_object_default.defs

seqnos_memory_object_default_server.c : mach/memory_object_default.defs
	${MIG} ${MIGFLAGS} -DSEQNOS -header /dev/null -user /dev/null -server seqnos_memory_object_default_server.c mach/memory_object_default.defs

DEVICE_USER_REQ_SRCS = ${DEVICE_REQ_ROUTINES/.*/&.c} ${DEVICE_REQ_SYSCALLS/.*/&.c}
DEVICE_REQUEST_SRCS = device_request.h ${DEVICE_USER_REQ_SRCS}

${DEVICE_REQUEST_SRCS} : device/device_request.defs
	${MIG} ${MIGFLAGS} -i ./ -user '$${DEVICE_USER_REQ_SRCS}' -server /dev/null device/device_request.defs

DEVICE_OVERWRITE_REPLY_SRCS = ${DEVICE_OVERWRITE_REPLY/.*/&.c}
DEVICE_REPLY_SRCS = device_reply_server.c ${DEVICE_OVERWRITE_REPLY_SRCS}

${DEVICE_REPLY_SRCS} : device/device_reply.defs
	${MIG} ${MIGFLAGS} -i ./ -server device_reply_server.c -header /dev/null -user '$${DEVICE_OVERWRITE_REPLY_SRCS}' device/device_reply.defs

NORMA_SRCS = mach_norma.h mach_norma_user.c mach_norma_server.c

${NORMA_SRCS} : mach/mach_norma.defs
	${MIG} ${MIGFLAGS} -user mach_norma_user.c -server mach_norma_server.c mach/mach_norma.defs

MACH_I386_SRCS = mach_i386.h mach_i386_user.c mach_i386_server.c

${MACH_I386_SRCS} : mach/i386/mach_i386.defs
	${MIG} ${MIGFLAGS} -user mach_i386_user.c -server mach_i386_server.c mach/i386/mach_i386.defs

MCMSG_SRCS = mcmsg_info_user.c mcmsg_info.h

${MCMSG_SRCS} : ../../kernel/i860paragon/mcmsg/mcmsg_info.defs
	${MIG} ${MIGFLAGS} -user mcmsg_info_user.c -server /dev/null -header mcmsg_info.h  ../../kernel/i860paragon/mcmsg/mcmsg_info.defs
	${CP} mcmsg_info.h ${INSTALLDIR}/include/mach/mcmsg_info.h
	${CP} mcmsg_info.h ${INSTALLDIR}/include/i860paragon/mcmsg/mcmsg_info.h

# This is a funny one.  We do the mig into a temporary directory and
# then pick out the one file that we actually want.  We use a
# temporary directory to avoid interfering with the files normally
# generated from mach/mach.defs.  We use htg_vm_map.defs instead of
# using mach/mach.defs directly so that the dependencies show up in
# htg_vm_map.defs.d instead of overwriting mach.defs.d.

htg_vm_map.c : htg_vm_map.defs
	mkdir tmp
	${MIG} ${MIGFLAGS} -DEMULATOR -i tmp/ -user htg_vm_map.c -header tmp/mach_interface.h -server /dev/null htg_vm_map.defs
	mv tmp/htg_vm_map.c htg_vm_map.c
	${RM} -r tmp

htg_vm_map.defs : mach/mach.defs
	ln -s $? $@

htg_vm_map.o : mach_interface.h

# mig generated code normally does stringent type-checking of received
# messages.  We disable this type-checking (for performance reasons)
# on messages that are received from the kernel, because the kernel
# is trusted to send correct messages.  Exceptions:
#	mach_debug user stubs - the mach_debug interface changes often,
#		so type-checking is useful to catch version skew
#	exc_server.c - users also have access to exception ports and
#		can make exception RPCs directly
#	Add mcmsg_info_user.c - 10/16/92 Greg Regnier

NO_TYPE_CHECK_SRCS = ${MACH_USER_SRCS} ${MACH_PORT_USER_SRCS} \
	${MIG_SYSCALL_SRCS} ${DEVICE_USER_SRCS} ${MACH_HOST_USER_SRCS} \
	notify_server.c seqnos_notify_server.c \
	memory_object_server.c seqnos_memory_object_server.c \
	memory_object_default_server.c \
	seqnos_memory_object_default_server.c \
	device_reply_server.c mach_norma_user.c mach_i386_user.c \
	htg_vm_map.c \
	mcmsg_info_user.c

# bootstrap_server.c ${BOOTSTRAP_USER_SRCS}

NO_TYPE_CHECK_OBJS = ${NO_TYPE_CHECK_SRCS/(.*)\.c/\1.o}

${NO_TYPE_CHECK_OBJS} : $${@/(.*)\.o/\1.c}
	${CC} -c ${CFLAGS} -DTypeCheck=0 ${@/(.*)\.o/\1.c}

# rules to generate the libraries

MACH_OBJS = mach_init.o mach_msg.o mach_msg_send.o mach_msg_receive.o \
	mach_msg_destroy.o  mach_msg_server.o

MIG_OBJS = mig_support.o mig_strncpy.o \
	mig_allocate.o mig_deallocate.o mig_reply_setup.o

USER_OBJS = slot_name.o error_codes.o mach_error_string.o

LIBC_OBJS = sbrk.o fork.o Ovfork.o

PURE_I386_OBJS = mach_i386_user.o
PURE_AT386_OBJS = mach_i386_user.o

# iPSC860 with MCMSG
MCMSG_OBJS = mcmsg_info_user.o

PURE_OBJS = ${MACH_OBJS} ${MIG_OBJS} ${USER_OBJS} \
	${PURE_${TARGET_MACHINE}_OBJS} htg_vm_map.o \
	device_reply_server.o \
	exc_user.o exc_server.o mach_norma_user.o \
	memory_object_server.o seqnos_memory_object_server.o \
	memory_object_default_server.o seqnos_memory_object_default_server.o \
	notify_user.o notify_server.o seqnos_notify_server.o \
	mach_server.o mach_port_server.o mach_host_server.o \
	mach_debug_server.o mach_norma_server.o device_server.o \
	memory_object_user.o memory_object_default_user.o \
	prof_user.o prof_server.o ${PURE_TRAPS/.*/&.o} \
	${MACH_SYSCALLS/.*/ms_&.o} ${MACH_SYSCALLS/.*/mig_&.o} \
	${MACH_PORT_SYSCALLS/.*/ms_&.o} ${MACH_PORT_SYSCALLS/.*/mig_&.o} \
	${DEVICE_SYSCALLS/.*/ms_&.o} ${DEVICE_SYSCALLS/.*/mig_&.o} \
	${DEVICE_REQ_SYSCALLS/.*/ms_&.o} ${DEVICE_REQ_SYSCALLS/.*/mig_&.o} \
	${DEVICE_OVERWRITE_SYSCALLS/.*/ms_&.o} \
	${DEVICE_OVERWRITE_REPLY/.*/mig_&.o} \
	${MACH_ROUTINES/.*/&.o} ${MACH_PORT_ROUTINES/.*/&.o} \
	${DEVICE_ROUTINES/.*/&.o} ${DEVICE_REQ_ROUTINES/.*/&.o} \
	${MACH_HOST_ROUTINES/.*/&.o} \
	${MACH_DEBUG_ROUTINES/.*/&.o} ${DPAGER_ROUTINES/.*/&.o} 
#	default_pager_server.o ${DPAGER_ROUTINES/.*/&.o} \
#	bootstrap_server.o ${BOOTSTRAP_ROUTINES/.*/&.o}

UNIXOID_OBJS = mach_init_ports.o mach_error.o \
	${LIBC_OBJS} ${UNIXOID_TRAPS/.*/&.o}

SA_VAX_OBJS     = urem.o udiv.o
SA_SUN3_OBJS    = start_float.o
SA_I386_OBJS    = gcc.o memcpy.o
SA_AT386_OBJS    = gcc.o memcpy.o
SA_I860_OBJS    = pre_crt0.o
SA_LUNA88K_OBJS = gcc.o

SA_OBJS = mach_init_sa.o ${SA_${TARGET_MACHINE}_OBJS} \
	exit.o _setjmp.o bzero.o bcopy.o \
	strcmp.o strlen.o strncpy.o strcat.o strcpy.o \
	doprnt.o printf.o sprintf.o panic.o \
	get_privileged_ports.o

CRT0_OBJS = crt0.o

LIBMACH_OBJS = ${PURE_OBJS} ${UNIXOID_OBJS} ${MCMSG_OBJS}

libmach.a : ${LIBMACH_OBJS}
	${RM} $@ _cvs_id.c _cvs_id.o
	mkidinfo -f libmach.a
	${CC} -c _cvs_id.c
	${AR} cq $@ _cvs_id.o ${LIBMACH_OBJS}
	${RANLIB} $@

LIBMACH_SA_OBJS = ${CRT0_OBJS} ${PURE_OBJS} ${SA_OBJS} ${MCMSG_OBJS}

libmach_sa.a : ${LIBMACH_SA_OBJS}
	${RM} $@ _cvs_id.c _cvs_id.o
	mkidinfo -f libmach_sa.a
	${CC} -c _cvs_id.c
	${AR} cq $@ _cvs_id.o ${LIBMACH_SA_OBJS}
	${RANLIB} $@

# Special '.cc' suffix for putting crt0 thru cpp twice.

.SUFFIXES: .cc

.cc.o:
	${CP} $*.cc $*.c
	${CC} -S -DCRT0 $*.c
	${CPP} $*.s > $*.i
	${AS} ${ASFLAGS} -o $*.o $*.i

-include Makedep
