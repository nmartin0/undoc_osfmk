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

SEC_SUBDIRS		= arp chown clri date fsdb fsck halt hostid hostname \
			  ifconfig init kill killall mknod mount \
			  named newfs ping ps \
			  reboot route savecore syslogd swapon tar \
			  ufs_fsck umount bootmesh cbs getmagic initpart kt \
			  stack \
			  mkdevtab reset_boot_node_list watchdog settime gps

SUBDIRS			= ${SEC_SUBDIRS} \
			  cat chmod cp cfgmgr create_pf dd devstat df ed expr \
			  hostinfo init.d ln ls mkdir mv rm restore waitfor \
			  rmknod s5fsck s5mkfs sh sleep stty sync update who \
			  mklogd patches \
			  ${${TARGET_MACHINE}_SUBDIRS}

DATAFILES		= bcheckrc rc0 rc2 rc3

EXPLOADER_SUBDIRS	= 

ILIST			= ${DATAFILES}
IDIR			= /sbin/

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk
include ${MAKEFILEPATH}/datafiles.mk
-include ${TARGET_MACHINE}/machdep.mk
