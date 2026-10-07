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
#
# derived from PMAX machdep.mk

IPSC860TNCOBJS = ${IPSC860COBJS} ${IPSC860SOBJS}

IPSC860TNCSOBJS = get_tnc_port.o kill3.o migrate.o node_self.o \
	rexecve.o rfork.o rforkmulti.o set_tnc_port.o table_node.o

IPSC860COBJS = rexecl.o rexecle.o rexeclp.o rexecv.o rexecvp.o

IPSC860PFSSOBJS = __esize.o _iodone.o _iomode.o _iowait.o _iread.o _ireadv.o \
		_iseof.o _iwrite.o _iwritev.o _lsize.o

IPSC860SOBJS = _nx_init.o nx_allocator.o _gop_sqrt.o mcmsglib.o _hwclock.o \
		${IPSC860TNCSOBJS} ${IPSC860PFSSOBJS} \
		fork_rmt_ctl.o part_attr.o part_bitmap.o node_list.o \
		chpart_rpc.o mkpart_rpc.o rmpart_rpc.o reg_daemon.o \
		pspart_rpc.o nx_mypart.o app_nodes.o app_info.o free_nodes.o \
		alloc_exit.o match_bitmap.o

${IPSC860SOBJS:.o=.S}: $${@:.S=.s}
	${RM} -f $@
	${CP} ${@:.S=.s} $@

${IPSC860SOBJS}: $${@:.o=.S}
	${CC} -Ui386 -Di860 ${_CCFLAGS_} -ES $*.S | as860 -o $@


