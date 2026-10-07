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
# Revision 1.3  1994/11/18  21:11:35  mtm
# Copyright additions/changes
#
# Revision 1.2  1992/11/30  23:18:29  dleslie
# Copy of NX branch back into main trunk
#
# Revision 1.1.2.1  1992/11/06  00:01:24  dleslie
# Local changes for NX through noon, November 5, 1992.
#
# Revision 4.1  1992/11/04  01:20:36  cfj
# Bump major revision number.
#
# Revision 1.16  1992/04/13  14:33:17  stans
# "OSF-distribution-ad8.5.2"
#
# Revision 1.16.8.2  91/09/12  17:50:46  coren
# 	Restored (uncommented) rule for building auditsysc.h, but made it
# 	conditional on SEC_LEVEL being set. Removed auditsysc.h from
# 	DATAFILES. All to enable fix to bug 2617.
# 	[91/09/11  14:27:53  coren]
# 
# Revision 1.16.6.3  91/04/11  15:52:37  tmt
# 	Streams stropts.h belongs here.
# 	[91/04/11  15:51:53  tmt]
# 
# Revision 1.16.6.2  91/03/11  13:10:47  jeffc
# 	Remove obsolete files.
# 	[91/03/01  13:02:51  jeffc]
# 
# Revision 1.16  90/10/31  15:16:06  devrcs
# 	Add STREAMS files to list of exports.
# 	[90/10/11  13:00:48  tmt]
# 
# Revision 1.15  90/10/07  20:37:28  devrcs
# 	Removed defunct header files.
# 	[90/10/04  07:55:47  gm]
# 
# 	Added EndLog Marker.
# 	[90/09/28  21:00:24  gm]
# 
# Revision 1.14  90/08/27  13:50:37  devrcs
# 	Removed syscontext.h.
# 	[90/08/27  12:18:32  mckeen]
# 
# Revision 1.13  90/08/09  14:28:41  devrcs
# 	Cleanup for export, install and post-freeze.
# 	[90/08/05  13:34:14  gm]
# 
# 	Added sysconfig.h.
# 	[90/07/11  11:36:50  knight]
# 
# $EndLog$

VPATH			= ${MAKETOP}/kernel/sys

DATAFILES		= NLchar.h access.h acct.h addrconf.h \
			  audit.h auxv.h bkmac.h buf.h \
			  callout.h clist.h clock.h cmap.h cmupty.h \
			  conf.h disklabel.h dk.h dkbad.h domain.h \
			  errno.h exec.h fcntl.h file.h filsys.h \
			  flock.h fpsconf.h fpsreg.h fs.h fullstat.h \
			  gprof.h id.h ioctl.h ioctl_compat.h \
			  ipc.h jctype0.h jctype1.h jioctl.h kernel.h \
			  kloadcall.h ldr_exec.h limits.h localedef.h \
			  lock.h lock_types.h lpio.h map.h mbuf.h \
			  mman.h mode.h mount.h msg.h msgbuf.h \
			  mtio.h namei.h param.h poll.h proc.h \
			  protosw.h ptrace.h quota.h reboot.h \
			  resource.h secdefines.h secioctl.h \
			  secpolicy.h security.h sem.h shm.h signal.h \
			  socket.h socketvar.h stat.h \
			  stream.h strlog.h stropts.h strstat.h cmn_err.h \
			  sysconfig.h sysinfo.h syslimits.h \
			  syslog.h systm.h table.h termio.h \
			  termios.h time.h timeb.h timers.h times.h \
			  trace.h ttmap.h tty.h ttychars.h \
			  ttydefaults.h ttydev.h ttyloc.h types.h \
			  ucred.h uio.h un.h unix_defs.h unpcb.h \
			  user.h ustat.h utsname.h vadvise.h \
			  version.h vlimit.h \
			  vm.h vmmac.h vmparam.h vnode.h \
			  vtimes.h wait.h
OTHERS			= dir.h syscall.h ${SEC_LEVEL?auditsysc.h:}

INCLUDES		= ${DATAFILES} ${OTHERS}

ILIST			= ${INCLUDES}
IDIR			= /usr/include/sys/
IMODE			= 644

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/datafiles.mk

dir.h:	${ALWAYS}
	${RM} ${_RMFLAGS_} $@
	ln -s ../ufs/$@ $@

syscall.h: ${MAKETOP}/kernel/conf/makesyscalls.sh \
	   ${MAKETOP}/kernel/conf/syscalls.master
	sh ${MAKETOP}/kernel/conf/makesyscalls.sh -h \
	   ${MAKETOP}/kernel/conf/syscalls.master


auditsysc.h: ${MAKETOP}/kernel/conf/makesyscalls.sh \
             ${MAKETOP}/kernel/conf/syscalls.master \
             ${MAKETOP}/kernel/conf/maketable.sh \
             ${MAKETOP}/kernel/conf/auderrent \
             ${MAKETOP}/kernel/conf/auderrmsg \
             ${MAKETOP}/kernel/conf/audit.master
	rm -f auderrent auderrmsg audit.syscalls
	sh ${MAKETOP}/kernel/conf/makesyscalls.sh -a ${MAKETOP}/kernel/conf/syscalls.master
	sh ${MAKETOP}/kernel/conf/maketable.sh -h ${MAKETOP}/kernel/conf/audit.master <audit.syscalls
	rm -f auderrent auderrmsg audit.syscalls
