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

MK_INC                  = ${MKREL}/include
OSF1AD_INC              = ${SVROBJ}
VPATH			= ${TARGET_MACHINE}:${MK_INC}:${OSF1AD_INC}/include:${OSF1AD_INC}/${SVRCONFIG}

COMMON_SUBDIRS		= arpa loader protocols rpc sdm tli nx nqs macs

KERNEL_SUBDIRS		= sys net netimp netinet netns nfs s5fs ufs pfs \
			  kern uxkern device i860paragon sll

MACH_SUBDIRS		= mach mach_debug servers mcmsg 

SUBDIRS			= ${COMMON_SUBDIRS} ${KERNEL_SUBDIRS} ${MACH_SUBDIRS} \
			  ${${TARGET_MACHINE}_SUBDIRS}

EXPFILES		= AFdefs.h NLctype.h NLregexp.h NLxio.h \
			  acl.h ar.h assert.h cpio.h cpyrght.h \
			  ctype.h curses.h curshdr.h dbm.h \
			  defenv.h dirent.h disktab.h dmpfmt.h \
			  dumprestor.h estat.h fatal.h fcntl.h \
			  fp.h fstab.h ftw.h gmon.h grp.h jctype.h \
			  langinfo.h lastlog.h ldfcn.h loader.h \
			  locale.h login.h lprio.h mach.h \
			  macros.h malloc.h mandatory.h \
			  mbstr.h memory.h mesg.h mon.h mp.h msg01.h \
			  msg04.h msg06.h msg08.h mtab.h ndbm.h \
			  netdb.h nl_types.h paths.h pcc.h poll.h \
			  prot.h protcmd.h pthread.h pwd.h random.h \
			  regex.h regexp.h remote_proc.h resolv.h search.h \
			  sgtty.h stab.h standards.h std_labels.h stddef.h \
			  stdio.h stdlib.h string.h strings.h \
			  struct.h sysexits.h tar.h time.h \
			  ttyent.h ulimit.h unctrl.h unistd.h \
			  userpw.h utime.h utmp.h vfont.h wchar.h \
			  wcstr.h wstring.h nx.h fnx.h allocsys.h \
			  mach_error.h mach_init.h errorlib.h cthreads.h \
			  vfs_vio.h raw_hippi.h tiuser.h xti.h\
			  ${${TARGET_MACHINE}_DATAFILES}


DATAFILES		= ${EXPFILES} lib_lock.h

SYS_LINKS		= NLchar.h errno.h limits.h signal.h syscall.h \
			  syslog.h termio.h termios.h ustat.h stropts.h dbglib.h

MACH_LINKS		= exc.defs mach.defs

MACH_DEBUG_LINKS	= mach_debug.defs

OTHERS			= machine math.h term.h \
			  ${MIG_HDRS} ${SYS_LINKS} ${MACH_LINKS} \
			  ${MACH_DEBUG_LINKS} \
			  ${${TARGET_MACHINE}_OTHERS}

INCLUDES		= ${DATAFILES} ${OTHERS}

EXPINC_SUBDIRS		= ${SUBDIRS}

ILIST			= ${EXPFILES} ${OTHERS} lib_lock.h
IDIR			= /usr/include/
IMODE			= 644

include ${TARGET_MACHINE}/machdep.mk
include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/objects.mk

machine: ${ALWAYS}
	${RM} -rf machine
	ln -s ${MACHINE} machine

math.h:	math-hdr ${TARGET_MACHINE}/math.h
	cat math-hdr ${TARGET_MACHINE}/math.h > $@

${SYS_LINKS}: ${ALWAYS}
	${RM} ${_RMFLAGS_} $@
	ln -s sys/$@ $@

${MACH_LINKS}: ${ALWAYS}
	${RM} ${_RMFLAGS_} $@
	ln -s mach/$@ $@

${MACH_DEBUG_LINKS}: ${ALWAYS}
	${RM} ${_RMFLAGS_} $@
	ln -s mach_debug/$@ $@

term.h: ${MAKETOP}/usr/ccs/lib/libcurses/caps \
	${MAKETOP}/usr/ccs/lib/libcurses/maketerm.ex
	${RM} ${_RMFLAGS_} caps.e
	${CP} ${MAKETOP}/usr/ccs/lib/libcurses/caps caps.e
	@ed < ${MAKETOP}/usr/ccs/lib/libcurses/maketerm.ex
	${RM} ${_RMFLAGS_} caps.e capnames.c
include ${MAKEFILEPATH}/depend.mk
-include Makedep
