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

LIBRARIES	= librpc.a

EXPLIB_TARGETS	= export_librpc.a

ILIST		= librpc.a
IDIR		= /usr/ccs/lib/
IMODE		= 644

OFILES		= auth_none.o auth_unix.o authunix_prot.o \
		  bindresvport.o clnt_generic.o clnt_perror.o \
		  clnt_raw.o clnt_simple.o clnt_tcp.o clnt_udp.o \
		  get_myaddress.o getrpcent.o getrpcport.o \
		  pmap_clnt.o pmap_getmaps.o pmap_getport.o pmap_prot.o \
		  pmap_prot2.o pmap_rmt.o \
		  rpc_callmsg.o rpc_dtablesize.o rpc_prot.o \
		  svc.o svc_auth.o svc_auth_unix.o svc_raw.o svc_run.o \
		  svc_simple.o svc_tcp.o svc_udp.o xdr.o xdr_array.o \
		  xdr_float.o xdr_mem.o \
		  xdr_rec.o xdr_reference.o xdr_stdio.o \
		  rpc_commondata.o

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/libs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
