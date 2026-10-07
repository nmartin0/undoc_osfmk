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
# (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
#
# OSF/1 Release 1.0

# use data encryption
#CRYPT_FILES		= crypt.o crypt_multperm.o crypt_newdes.o crypt_xor.o \
#			  crypt_des.o  des.o 
# turn off data encryption
CRYPTFLAGS		= -D_NOCRYPT

VPATH			= ../conf

PROGRAMS		= netmsgserver
MIG_DEFS		= netname.defs km.defs logstat.defs

ILIST			= netmsgserver
IDIR			= /usr/sbin/

USE_STATIC_LIBRARIES	=

INCFLAGS		= -I../conf
CFLAGS			= -D_BSD ${CRYPTFLAGS}
OFILES			= nmserver.o dispatcher.o lock_queue.o mem.o \
			  nm_extra.o rwlock.o sbuf.o timer.o uid.o \
			  ipc.o ipc_block.o ipc_msg.o ipc_rpc.o \
			  ipc_exception.o ipc_swap.o \
			  keyman.o km_procs.o km_utils.o multperm.o \
			  netname.o netnameServer.o nn_handler.o nn_procs.o \
			  portrec.o portcheck.o pc_handler.o \
			  pc_startup.o portsearch.o ps_auth.o \
			  portops.o po_message.o po_notify.o po_utils.o \
			  datagram.o deltat.o deltat_handler.o \
			  deltat_send.o deltat_utils.o \
			  srr.o srr_handler.o srr_utils.o netipc.o \
			  network.o transport.o \
			  nm_init.o logstat.o logstatServer.o ls_rec.o \
			  vmtp2.o nm_enable.o  \
			  tcp.o cam_stuff.o \
			  stabsearch.o fold.o srchscore.o getbool.o \
			  prstab.o getint.o  ${CRYPT_FILES}
LIBS			= -lpthreads -lmach -lbsd -lc_r

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/programs.mk

kmUser.c netnameServer.c logstatServer.c: config.h
include ${MAKEFILEPATH}/depend.mk
-include Makedep
