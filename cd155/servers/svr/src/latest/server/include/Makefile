# 
# $Copyright
# Copyright 1993, 1994 , 1995 Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
#
# @OSF_COPYRIGHT@
#
#
# HISTORY
# $Log: Makefile,v $
# Revision 1.9  1994/11/18  20:32:24  mtm
# Copyright additions/changes
#
# Revision 1.8  1994/06/29  19:21:35  johannes
# allocinfo.h and core.h added to be exported to usr/include/sys
#
#  Initial check-in of parallel core dumping
#  Reviewer: stefan, jlitvin
#  Risk: Medium
#  Benefit or PTS #: OS support for Postmortem Debugging
#  Testing: developer tests
#  Module(s):
# 	svr/server/conf/MASTER
# 	svr/server/conf/MASTER.i860
# 	svr/server/conf/files.i860
# 	svr/server/paracore/core_types.h
# 	svr/server/paracore/allocinfo.c
# 	svr/server/paracore/core.c
# 	svr/server/paracore/dump.c
# 	svr/server/paracore/dvp_pvpcore.c
# 	svr/server/sys/allocinfo.h
# 	svr/server/sys/core.h
# 	svr/server/sys/user.h
# 	svr/server/nx/nx.defs
# 	svr/server/nx/nx.c
# 	svr/server/bsd/kern_exit.c
# 	svr/server/bsd/kern_fork.c
# 	svr/server/bsd/kern_sig.c
# 	svr/server/tnc/dpvproc.h
# 	svr/server/tnc/dvp_init.c
# 	svr/server/tnc/dvp_pvpops.c
# 	svr/server/tnc/pvp.ops
# 	svr/server/uxkern/fsvr_msg.c
# 	cmds_libs/src/usr/sbin/allocator/alloc.defs
# 	cmds_libs/src/usr/sbin/allocator/misc_rpcs.c
# 	cmds_libs/src/usr/sbin/allocator/Makefile
# 	cmds_libs/src/usr/include/README.locate
# 	cmds_libs/src/usr/include/sys/Makefile
#
# Revision 1.7  1994/06/14  23:15:09  yazz
#  Reviewer: Dave Leslie, Shala Arshi
#  Risk: very lo
#  Benefit or PTS #: #9867 L-*
#  Testing: Builds of STD+WS, LITE+WS and RAMDISK+WS
#  Module(s): server/include/Makefile
#
# Delete references to unused OSF include file "uxkern/credential.h".  Note
# that include file "uxkern/credentials.h" (plural) is correct as-is.
#
# Revision 1.6  1993/12/29  19:10:32  dleslie
#  Reviewer: none
#  Risk: low
#  Benefit or PTS #: no longer run 'cmp` on all the headers every time
# 	we run 'make' -- will greatly speed up sandbox work
#  Testing: builds successfully
#  Module(s): Makefile
#
# Revision 1.5  1993/11/18  17:36:55  dleslie
#  Reviewer: shala
#  Risk: low
#  Benefit or PTS #: copy header files needed by the cmds/libs build to the
# 	obj tree, so cmds builds won't have to look for anything in the
# 	server source tree
#  Testing: built on 486 and Sun
#  Module(s): make.template
#
# Revision 1.4  1993/07/14  18:00:49  cfj
# OSF/1 AD 1.0.4 code drop from Locus.
#
# Revision 1.1.1.3  1993/07/01  19:19:31  cfj
# Adding new code from vendor
#
# Revision 1.3  1993/05/06  19:15:38  cfj
# ad103+tnc merged with Intel code.
#
# Revision 1.1.1.1  1993/05/03  17:30:24  cfj
# Initial 1.0.3 code drop
#
# Revision 1.2  1992/11/30  22:22:35  dleslie
# Copy of NX branch back into main trunk
#
# Revision 1.1.2.1  1992/11/05  23:21:38  dleslie
# Local changes for NX through noon, November 5, 1992.
#
# Revision 2.4  1992/04/22  15:21:04  yazz
# Added a print_env: target.
#
# Revision 2.3  91/11/13  13:15:15  rabii
# 	Added i860 include files
# 
# Revision 2.2  91/08/31  13:37:07  rabii
# 	Initial V2.0 Checkin
# 
# Revision 3.4  91/08/30  10:30:33  condict
# Change make built-in var TARGET_MACHINE to TARGET_CPU, allowing cross-compiles.
# 
# Revision 3.3  91/08/28  14:02:12  condict
# Update to OSF 1.0.2.
# 
# Revision 1.13.4.3  90/12/20  14:52:06  devsrc
# 	Merge 1.01 sandbox to osc1.0
# 
# Revision 1.13.4.2  90/12/13  11:13:11  kwallace
# 	Merged changes from latest branch.
# 	Revision 1.13.2.2  90/11/29  15:16:40  hermi
# 		Added bsd.a.out.h to AT386_INCFILES.
# 		[90/11/29  15:15:20  hermi]
# 	[90/12/13  10:15:50  kwallace]
# 
# Revision 1.13  90/10/31  13:57:24  devrcs
# 	Added - before if test to suppress Ultrix errors.
# 	[90/10/28  15:18:46  gm]
# 
# 	Only copy files from /usr/include if they changed.  Abstract out
# 	MACH_O machine dependent headers.
# 	[90/10/23  13:31:37  brezak]
# 
# Revision 1.12  90/10/07  13:48:50  devrcs
# 	Added EndLog Marker.
# 	[90/09/28  09:46:52  gm]
# 
# Revision 1.11  90/08/24  12:02:14  devrcs
# 	Added a.out.h to HP_M68K_INCFILES.
# 	[90/08/22  09:37:30  sharpe]
# 
# 	HP/Apollo M68K
# 	[90/08/13  17:36:15  mcg]
# 
# Revision 1.10  90/08/09  13:19:54  devrcs
# 	Cleanup for export, install and post-freeze.
# 	[90/08/05  13:11:09  gm]
# 
# $EndLog$

INCFILES		= standards.h dirent.h \
			  mach_o_format.h mach_o_header.h mach_o_vals.h
TLI_INCFILES		= tli/tihdr.h tli/timod.h tli/xtiso_config.h
SYS_FSH			= sys/fs.h
SYS_QUOTAH		= sys/quota.h
SYS_LOCKH		= sys/lock.h
SYS_INCFILES		=         sys/access.h sys/acct.h sys/addrconf.h sys/allocinfo.h sys/auxv.h sys/buf.h \
	sys/callout.h sys/clist.h sys/cmn_err.h sys/conf.h sys/core.h sys/dbglib.h sys/disklabel.h sys/dk.h sys/domain.h \
	sys/errno.h sys/estat.h sys/exec.h sys/fcntl.h sys/file.h sys/flock.h  sys/gprof.h sys/ioctl.h \
	sys/ioctl_compat.h sys/ipc.h sys/kernel.h sys/kloadcall.h sys/ldr_exec.h \
	sys/limits.h sys/lock_types.h sys/map.h sys/mbuf.h sys/mman.h sys/mode.h sys/mount.h \
	sys/msg.h sys/msgbuf.h sys/namei.h sys/param.h sys/poll.h sys/proc.h sys/protosw.h sys/ptrace.h \
	sys/reboot.h sys/resource.h sys/secdefines.h sys/security.h sys/sem.h sys/shm.h \
	sys/signal.h sys/socket.h sys/socketvar.h sys/stat.h sys/stream.h sys/strlog.h sys/stropts.h \
	sys/strstat.h sys/synch.h sys/sysconfig.h sys/syslimits.h sys/syslog.h \
	sys/systm.h sys/table.h sys/termio.h sys/termios.h sys/time.h sys/timeb.h sys/times.h \
	sys/trace.h sys/ttmap.h sys/tty.h sys/ttychars.h sys/ttydefaults.h sys/ttydev.h sys/ttyloc.h \
	sys/types.h sys/ucred.h sys/uio.h sys/un.h sys/unix_defs.h sys/unpcb.h \
	sys/user.h sys/utsname.h sys/ux_exception.h sys/vadvise.h sys/version.h sys/vnode.h sys/wait.h
NET_INCFILES		= net/if.h net/if_dl.h net/if_llc.h net/if_types.h \
	net/radix.h net/route.h \
	net/if_arp.h net/if_hippi.h net/if_slvar.h net/netisr.h net/raw_cb.h
NETIMP_INCFILES		= netimp/if_imp.h netimp/if_imphost.h
NETINET_INCFILES	= netinet/icmp_var.h netinet/if_ether.h netinet/in.h netinet/in_pcb.h netinet/in_systm.h \
	netinet/in_var.h netinet/inet_config.h netinet/ip.h netinet/ip_icmp.h netinet/ip_var.h netinet/tcp.h \
	netinet/tcp_debug.h netinet/tcp_fsm.h netinet/tcp_seq.h netinet/tcp_timer.h netinet/tcp_var.h \
	netinet/tcpip.h netinet/udp.h netinet/udp_var.h
NETNS_INCFILES		= netns/ns_config.h netns/idp.h netns/idp_var.h netns/ns.h netns/ns_error.h \
	netns/ns_if.h netns/ns_pcb.h netns/sp.h netns/spidp.h netns/spp_debug.h netns/spp_timer.h netns/spp_var.h
NFS_INCFILES		= nfs/nfs.h nfs/nfsiom.h nfs/nfsm_subs.h nfs/nfsmount.h nfs/nfsnode.h \
	nfs/nfsrvcache.h nfs/nfsv2.h nfs/rpcv2.h nfs/xdr_subs.h
S5FS_INCFILES		= s5fs/filsys.h s5fs/s5dir.h s5fs/s5fblk.h s5fs/s5ino.h s5fs/s5inode.h \
	s5fs/s5mount.h s5fs/s5param.h
UFS_INCFILES		= ufs/dinode.h ufs/dir.h ufs/fs.h ufs/inode.h ufs/mfsnode.h ufs/quota.h \
	ufs/ufsmount.h
PFS_INCFILES		= pfs/pfs.h
KERN_INCFILES		= kern/assert.h kern/event.h kern/kalloc.h kern/lock.h kern/macro_help.h \
	kern/mfs.h kern/parallel.h kern/queue.h kern/sched_prim.h kern/threadcall.h kern/xpr.h kern/zalloc.h
UXKERN_INCFILES		= uxkern/bsd_msg.h uxkern/bsd_types.h uxkern/credentials.h \
	uxkern/device.h uxkern/device_reply_hdlr.h uxkern/device_utils.h uxkern/fsvr_generic.h \
	uxkern/fsvr_types.h uxkern/import_mach.h uxkern/mf.h uxkern/port_hash.h uxkern/proc_to_port.h \
	uxkern/proc_to_task.h uxkern/rmtnode.h uxkern/rproc.h uxkern/server_mach_msg.h uxkern/sthread.h \
	uxkern/syscall_subr.h uxkern/syscalltrace.h uxkern/vm_param.h
SLL_INCFILES		= sll/sll.h








MACH_O_INCFILES		= mach_o_types.h mach_o_header_md.h

PMAX_INCFILES		= syms.h varargs.h symconst.h sym.h stsupport.h \
			  ${MACH_O_INCFILES}
MMAX_INCFILES		= syms.h varargs.h ${MACH_O_INCFILES}
AT386_INCFILES		= a.out.h bsd.a.out.h stab.h varargs.h \
			  ${MACH_O_INCFILES}
I386_INCFILES		= machlimits.h
HP_M68K_INCFILES        = a.out.h varargs.h aouthdr.h filehdr.h scnhdr.h
I860_INCFILES		= varargs.h varargs.pgi.h va_list.h filehdr.h scnhdr.h

# In the OSF/1 build, all of these are defined externally:
MAKETOP=../..
RM=/bin/rm
_RMFLAGS_= -f

all:	dirs ${INCFILES} ${${TARGET_CPU}_INCFILES} \
		${${TARGET_MODEL}_INCFILES} ${TLI_INCFILES} ${SYS_INCFILES} \
		${SYS_FSH} ${SYS_QUOTAH} ${SYS_LOCKH} ${NET_INCFILES} \
		${NETINET_INCFILES} ${NETNS_INCFILES} ${NFS_INCFILES} \
		${KERN_INCFILES} ${UXKERN_INCFILES} ${NETIMP_INCFILES} \
		${S5FS_INCFILES} ${UFS_INCFILES} ${PFS_INCFILES} \
		${SLL_INCFILES}

dirs: sys tli net netinet netns nfs kern uxkern netimp s5fs ufs pfs sll

sys:
	mkdir sys

tli:
	 mkdir tli

net:
	mkdir net

netinet:
	mkdir netinet 

netns:
	mkdir netns

nfs:
	mkdir nfs

kern:
	mkdir kern

uxkern:
	mkdir uxkern

netimp:
	mkdir netimp

s5fs:
	mkdir s5fs

ufs:
	mkdir ufs

pfs:
	mkdir pfs

sll:
	mkdir sll

${INCFILES}: ${MAKETOP}/usr/include/$${@}
	-if [ -f $@ ] && cmp -s $? $@; then :; \
	 else ${RM} ${_RMFLAGS_} $@; ${CP} $? $@; fi

tli/tihdr.h: ${MAKETOP}/server/streamsm/tihdr.h
	-if [ -f $@ ] && cmp -s $? $@; then :; \
	 else ${RM} ${_RMFLAGS_} $@; ${CP} $? $@; fi

tli/timod.h: ${MAKETOP}/server/streamsm/timod.h
	-if [ -f $@ ] && cmp -s $? $@; then :; \
	 else ${RM} ${_RMFLAGS_} $@; ${CP} $? $@; fi

tli/xtiso_config.h: ${MAKETOP}/server/streamsm/xtiso_config.h
	-if [ -f $@ ] && cmp -s $? $@; then :; \
	 else ${RM} ${_RMFLAGS_} $@; ${CP} $? $@; fi

#${TLI_INCFILES}: ${MAKETOP}/server/streamsm/$${@}
#	-if [ -f tli/$@ ] && cmp -s $? tli/$@; then :; \
#	 else ${RM} ${_RMFLAGS_} tli/$@; ${CP} $? tli/$@; fi

${SYS_INCFILES}: ${MAKETOP}/server/$${@}
	-if [ -f $@ ] && cmp -s $? $@; then :; \
	 else ${RM} ${_RMFLAGS_} $@; ${CP} $? $@; fi

${SYS_FSH}: ${MAKETOP}/server/$${@}
	-if [ -f $@ ] && cmp -s $? $@; then :; \
	 else ${RM} ${_RMFLAGS_} $@; ${CP} $? $@; fi

${SYS_QUOTAH}: ${MAKETOP}/server/$${@}
	-if [ -f $@ ] && cmp -s $? $@; then :; \
	 else ${RM} ${_RMFLAGS_} $@; ${CP} $? $@; fi

${SYS_LOCKH}: ${MAKETOP}/server/$${@}
	-if [ -f $@ ] && cmp -s $? $@; then :; \
	 else ${RM} ${_RMFLAGS_} $@; ${CP} $? $@; fi

${NET_INCFILES}: ${MAKETOP}/server/$${@}
	-if [ -f $@ ] && cmp -s $? $@; then :; \
	 else ${RM} ${_RMFLAGS_} $@; ${CP} $? $@; fi

${NETIMP_INCFILES}: ${MAKETOP}/server/$${@}
	-if [ -f $@ ] && cmp -s $? $@; then :; \
	 else ${RM} ${_RMFLAGS_} $@; ${CP} $? $@; fi

${NETINET_INCFILES}: ${MAKETOP}/server/$${@}
	-if [ -f $@ ] && cmp -s $? $@; then :; \
	 else ${RM} ${_RMFLAGS_} $@; ${CP} $? $@; fi

${NETNS_INCFILES}: ${MAKETOP}/server/$${@}
	-if [ -f $@ ] && cmp -s $? $@; then :; \
	 else ${RM} ${_RMFLAGS_} $@; ${CP} $? $@; fi

${NFS_INCFILES}: ${MAKETOP}/server/$${@}
	-if [ -f $@ ] && cmp -s $? $@; then :; \
	 else ${RM} ${_RMFLAGS_} $@; ${CP} $? $@; fi

${S5FS_INCFILES}: ${MAKETOP}/server/$${@}
	-if [ -f $@ ] && cmp -s $? $@; then :; \
	 else ${RM} ${_RMFLAGS_} $@; ${CP} $? $@; fi

${UFS_INCFILES}: ${MAKETOP}/server/$${@}
	-if [ -f $@ ] && cmp -s $? $@; then :; \
	 else ${RM} ${_RMFLAGS_} $@; ${CP} $? $@; fi

${PFS_INCFILES}: ${MAKETOP}/server/$${@}
	-if [ -f $@ ] && cmp -s $? $@; then :; \
	 else ${RM} ${_RMFLAGS_} $@; ${CP} $? $@; fi

${KERN_INCFILES}: ${MAKETOP}/server/$${@}
	-if [ -f $@ ] && cmp -s $? $@; then :; \
	 else ${RM} ${_RMFLAGS_} $@; ${CP} $? $@; fi

${UXKERN_INCFILES}: ${MAKETOP}/server/$${@}
	-if [ -f $@ ] && cmp -s $? $@; then :; \
	 else ${RM} ${_RMFLAGS_} $@; ${CP} $? $@; fi

${SLL_INCFILES}: ${MAKETOP}/server/$${@}
	-if [ -f $@ ] && cmp -s $? $@; then :; \
	 else ${RM} ${_RMFLAGS_} $@; ${CP} $? $@; fi



${${TARGET_CPU}_INCFILES}: ${MAKETOP}/usr/include/${target_cpu}/$${@}
	-if [ -f $@ ] && cmp -s $? $@; then :; \
	 else ${RM} ${_RMFLAGS_} $@; ${CP} $? $@; fi

${${TARGET_MODEL}_INCFILES}: ${MAKETOP}/usr/include/${TARGET_MODEL}/$${@}
	-if [ -f $@ ] && cmp -s $? $@; then :; \
	 else ${RM} ${_RMFLAGS_} $@; ${CP} $? $@; fi

print_env:
	@echo TARGET_CPU is $(TARGET_CPU)
	@echo TARGET_MODEL is $(TARGET_MODEL)
	@echo MAKETOP is $(MAKETOP)

