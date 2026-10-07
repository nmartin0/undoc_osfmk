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
# OSF/1 Release 1.0.1

SEC_SUBDIRS		= auth policy
SUBDIRS			= copyright load_level magic nls nx termcap \
			  ${SEC_DEFS?${SEC_SUBDIRS}:}
DATAFILES		= disktab gettydefs inittab fstab rc.config hosts \
			  inetd.conf lib_admin.conf ntp.conf networks \
			  motd pfstab printcap profile rpc services shells \
                          protocols syslog.conf ttys TIMEZONE \
			  ${SEC_DEFS?:group passwd}
SEC_OTHERS		= ${SEC_DEFS?group passwd:}
OTHERS			= ${SEC_OTHERS}

SEC_ILIST		= ${SEC_OTHERS}
ILIST			= ${DATAFILES} ${OTHERS}
IDIR			= /etc/
IMODE            = 644

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk
include ${MAKEFILEPATH}/datafiles.mk

${SEC_OTHERS}: $${@:=.${SEC_LEVEL}} ${ALWAYS}
	${RM} ${_RMFLAGS_} $@
	${CP} ${@:=.${SEC_LEVEL}} $@

include ${MAKEFILEPATH}/depend.mk
-include Makedep
