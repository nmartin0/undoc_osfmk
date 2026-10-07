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
# OSF/1 Release 1.0.3

#
#
OSF1AD_SYS	= ${SVROBJ}/include/sys

VPATH		= ${OSF1AD_SYS}

DATAFILES		= NLchar.h access.h acct.h addrconf.h allocinfo.h \
			  audit.h auxv.h bkmac.h buf.h \
			  callout.h clist.h clock.h cmap.h cmupty.h conf.h \
			  core.h dbglib.h disklabel.h dk.h dkbad.h domain.h \
			  errno.h estat.h exec.h fcntl.h file.h filsys.h \
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
			  synch.h sysconfig.h sysinfo.h syslimits.h \
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

# These are the symbols we want to have defined in sys/syscall.h for
# commands/libraries builds, which aren't defined in server builds.
# Used in 'munge_syscall'.
ADD_DEFINES             = NX TNC PFS SLL

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/datafiles.mk

dir.h:	${ALWAYS}
	${RM} ${_RMFLAGS_} $@
	ln -s ../ufs/$@ $@

syscall.h: ${SVROBJ}/include/sys/syscall.h
	${SOURCEBASE}/usr/include/sys/munge_syscall \
		${SVROBJ}/include/sys/syscall.h ${ADD_DEFINES} > syscall.h


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
include ${MAKEFILEPATH}/depend.mk
-include Makedep
