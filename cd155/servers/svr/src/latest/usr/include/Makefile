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
# Revision 1.3  1994/11/18  21:09:04  mtm
# Copyright additions/changes
#
# Revision 1.2  1992/11/30  23:12:58  dleslie
# Copy of NX branch back into main trunk
#
# Revision 1.1.2.1  1992/11/05  23:56:25  dleslie
# Local changes for NX through noon, November 5, 1992.
#
# Revision 4.1  1992/11/04  01:14:33  cfj
# Bump major revision number.
#
# Revision 1.1  1992/07/20  16:17:07  nandy
# Initial revision
#
# Revision 1.18  92/04/13  14:34:08  stans
# "OSF-distribution-ad8.5.2"
# 
# Revision 1.18.8.2  91/06/21  13:04:54  ers
# 	Removed MACH_LINKS.  Programs should include these from <mach/xxx.h>
# 	[91/06/21  09:24:19  ers]
# 
# Revision 1.18.6.2  91/04/11  15:52:31  tmt
# 	Streams stropts.h belongs in sys/ - move it there. Leave symlink
# 	here to cover ambiguity between examples and actual.
# 	[91/04/11  15:51:30  tmt]
# 
# Revision 1.18  90/12/06  14:17:34  devrcs
# 	Changes for osc1.0 release.
# 	[90/12/01  18:39:15  gm]
# 
# Revision 1.17  90/11/03  14:06:23  devrcs
# 	Fixed ILIST to match DATAFILES
# 	[90/11/01  22:19:06  mckeen]
# 
# 	Add XTIFILES, STRFILES for new streams files locations.
# 	Add rules for copying them. Remove streams subdir.
# 	[90/10/11  12:58:56  tmt]
# 
# Revision 1.16  90/10/07  20:23:16  devrcs
# 	Added EndLog Marker.
# 	[90/09/28  20:39:15  gm]
# 
# Revision 1.15  90/09/23  16:42:41  devrcs
# 	Add loader subdirectory
# 	[90/09/19  17:48:33  lwa]
# 
# 	Add lib_lock.h
# 	[90/09/19  15:16:57  lwa]
# 
# Revision 1.14  90/08/24  13:51:53  devrcs
# 	Remove math-hdr from DATAFILES.  Fixup math.h rule.
# 	[90/08/18  09:51:29  gm]
# 
# 	Split machine-dependent math.h into machine independent
# 	and machine-dependent parts.
# 	[90/08/16  22:21:41  lehotsky]
# 
# Revision 1.13  90/08/09  14:25:45  devrcs
# 	Cleanup for export, install and post-freeze.
# 	[90/08/05  13:31:51  gm]
# 
# 	Add regex.h random.h crypt.h
# 	[90/08/02  10:31:36  sp]
# 
# 	Added cpio.h
# 	[90/07/30  13:19:45  glenn]
# 
# $EndLog$

VPATH			= ${TARGET_MACHINE}

COMMON_SUBDIRS		= arpa loader protocols rpc sdm tli
KERNEL_SUBDIRS		= sys net netimp netinet netns nfs s5fs ufs lvm
MACH_SUBDIRS		= mach mach_debug servers
SUBDIRS			= ${COMMON_SUBDIRS} ${KERNEL_SUBDIRS} ${MACH_SUBDIRS} \
			  ${${TARGET_MACHINE}_SUBDIRS}
EXPFILES		= AFdefs.h NLctype.h NLregexp.h NLxio.h \
			  acl.h ar.h assert.h cpio.h cpyrght.h \
			  crypt.h ctype.h curses.h curshdr.h dbm.h \
			  defenv.h dirent.h disktab.h dmpfmt.h \
			  dumprestor.h exc.defs fatal.h fcntl.h \
			  fp.h fstab.h ftw.h gmon.h grp.h jctype.h \
			  langinfo.h lastlog.h ldfcn.h loader.h \
			  locale.h login.h lprio.h mach.defs mach.h \
			  mach_debug.defs mach_error.h mach_exception.h \
			  mach_init.h mach_o_format.h mach_o_header.h \
			  mach_o_vals.h macros.h malloc.h mandatory.h \
			  mbstr.h memory.h mesg.h mon.h mp.h msg01.h \
			  msg04.h msg06.h msg08.h mtab.h ndbm.h \
			  netdb.h nl_types.h paths.h pcc.h poll.h \
			  prot.h protcmd.h pthread.h pwd.h random.h \
			  regex.h regexp.h resolv.h search.h sgtty.h \
			  stab.h standards.h std_labels.h stddef.h \
			  stdio.h stdlib.h string.h strings.h \
			  struct.h sysexits.h tar.h time.h \
			  ttyent.h ulimit.h unctrl.h unistd.h \
			  userpw.h utime.h utmp.h vfont.h wchar.h \
			  wcstr.h wstring.h \
			  ${${TARGET_MACHINE}_DATAFILES}
XTIFILES		= tiuser.h xti.h
DATAFILES		= ${EXPFILES} ${XTIFILES} lib_lock.h
SYS_LINKS		= NLchar.h errno.h limits.h signal.h syscall.h \
			  syslog.h termio.h termios.h ustat.h stropts.h
OTHERS			= machine math.h term.h \
			  ${MIG_HDRS} ${SYS_LINKS} ${MACH_LINKS} \
			  ${${TARGET_MACHINE}_OTHERS}
INCLUDES		= ${DATAFILES} ${OTHERS}

EXPINC_SUBDIRS		= ${SUBDIRS}

ILIST			= ${EXPFILES} ${OTHERS} ${XTIFILES}
IDIR			= /usr/include/
IMODE			= 644

include ${TARGET_MACHINE}/machdep.mk
include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/objects.mk

machine: ${ALWAYS}
	${RM} ${_RMFLAGS_} machine
	ln -s ${MACHINE} machine

math.h:	math-hdr ${TARGET_MACHINE}/math.h
	cat math-hdr ${TARGET_MACHINE}/math.h > $@

${SYS_LINKS}: ${ALWAYS}
	${RM} ${_RMFLAGS_} $@
	ln -s sys/$@ $@

${XTIFILES}: ${MAKETOP}/kernel/streamsm/$${@}
	${RM} ${_RMFLAGS_} $@
	${CP} $? $@

term.h: ${MAKETOP}/usr/ccs/lib/libcurses/caps \
	${MAKETOP}/usr/ccs/lib/libcurses/maketerm.ex
	${RM} ${_RMFLAGS_} caps.e
	${CP} ${MAKETOP}/usr/ccs/lib/libcurses/caps caps.e
	@ed < ${MAKETOP}/usr/ccs/lib/libcurses/maketerm.ex
	${RM} ${_RMFLAGS_} caps.e capnames.c
