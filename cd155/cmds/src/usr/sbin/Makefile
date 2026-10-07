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
# (c) Copyright 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
# ALL RIGHTS RESERVED 
#
#
# OSF/1 Release 1.0.4

SEC_SUBDIRS		= ${${TARGET_MACHINE}_SEC_SUBDIRS} \
			  acct arp chroot clri cron dcheck diskpart dumpfs \
			  fastboot fasthalt fsdb getty halt \
			  hippi_setmap hippi_showmap hostid \
			  icheck ifconfig killall link load_level lpd mknod \
			  mkpasswd mount ncheck newfs ping quot \
			  reboot \
			  renice route rshd savecore shutdown swapon \
			  syslogd tunefs ufs_fsck umount vipw wall

MACS_SUBDIRS		= ${${TARGET_MACHINE}_MACS_SUBDIRS} \
			  dbconvert dbcreate macadmin macupdate si

SUBDIRS			= ${SEC_SUBDIRS} ${MACS_SUBDIRS} \
			  ac adduser allocator badsect catman comsat \
			  dmesg dump exportpaging fingerd fsck gated \
			  grpck inetd kgmon mklost+found mkproto mountd mvdir \
			  named netstat nfsd nfsiod ntpd \
			  portmap pwck restore rlogind rmt \
			  routed rwhod s5fsck s5mkfs sa sendmail \
			  smd strace sync talkd telnetd tftpd \
			  timed trpt traceroute update \
			  ${${TARGET_MACHINE}_SUBDIRS}
# notyet - install pstat

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk
-include ${TARGET_MACHINE}/machdep.mk
