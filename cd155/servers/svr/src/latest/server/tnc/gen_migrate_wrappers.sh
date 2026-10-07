#!/bin/ksh -
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
# Copyright (c) 1993-1995, Locus Computing Corporation
# All rights reserved
#
# HISTORY
# $Log: gen_migrate_wrappers.sh,v $
# Revision 1.3  1995/02/01  21:44:33  bolsen
#  Reviewer(s): Jerry Toman
#  Risk: Medium (lots of files)
#  Module(s): Too many to list
#  Configurations built: STD, LITE, & RAMDISK
#
#  Added or Updated the Locus Copyright message.
#
# Revision 1.2  1994/11/18  20:43:30  mtm
# Copyright additions/changes
#
# Revision 1.1  1994/04/28  19:14:25  chrisp
# tncgen: autugenerated code now derived from *.ops definition files
# and processed by tncgen and associated gen*.sh scripts. #ifdefs
# introduced where previously they were ignored.
#
#  Reviewer: dleslie, cfj
#  Risk: M
#  Benefit or PTS #: 9188
#  Testing: Builds, builds, builds.
#  Module(s):
#      Modified Files:
#  	dpvproc.h dvp_pvpsops.c dvp_vpops.c rvp_pvpops.c
#  	rvp_pvpops_server.c rvp_pvpsops.c rvp_pvpsops_server.c
#  	tnc.defs tnc_mig.c
#      Added Files:
#  	gen_client_stubs.sh gen_mig.sh gen_migrate_wrappers.sh
#  	gen_server_stubs.sh pvp.ops pvps.ops tncgen.mk
#      Removed Files:
#  	dpvproc_struct.h makeTNCtables.sh
#
# $EndLog$
#

#
# Generate the wrapper routines (tnc/pvpops_gen.c) for the pvproc ops.
# Note that this file is included in pvpops.c.
# Each operations wrapper is of the form:
#	int <op_name>_<mode>(
#		struct vproc *<parm_name_1>,
#		<parm_type_2> <parm_name_2>,
#			... repeated 
#		<parm_type_n> <parm_name_n>)
#	{
#		PVP_OP(<parm_name_1>,
#			<OP_NAME>_<MODE>(<parm_name_1>,...),"<op_name>");
#	}
# where <mode> is "sync" for standard operations. For operations declared as
# ASYNC, in addition, wrappers are generated for modes "send" and "receive".
#
cat >$TMPDIR/awk.wrappers <<EOF
	BEGIN {
		for (i=32;i<128;i++) {
			ch = sprintf("%c", i);
			if (ch ~/[a-z]/)
				to_upper[ch] = sprintf("%c", i-32);
			else
				to_upper[ch] = ch;
		}
	}

	\$1 == "" { next }

	\$1 == "escape" {
		in_escape = 1;
		next;
	}

	\$1 == "end_escape" {
		in_escape = 0;
		next;
	}

	{
		if (in_escape)
			next;
	}

	\$1 == "operation" {
		op = "$PREFIX" "op_" \$2;
		OP = "";
		for (i=1;i<=length(op);i++)
			OP = OP to_upper[substr(op,i,1)];
		n_parms = 0;
		op_type = \$3;
	}

	\$1 == "parameter" {
		n_parms++;
		parm_name = \$NF;
		while (substr(parm_name,1,1) == "*")
			parm_name= substr(parm_name, 2, length(parm_name));
		if (n_parms == 1) {
			decl_list = "";
			parm_list = parm_name;
			target_parm = parm_name;
		} else {
			decl_list = decl_list ",\n\t";
			parm_list = parm_list ", " parm_name;
		}
		for (i=3; i<=NF; i++)
			decl_list = decl_list " " \$i;
	}

	\$1 == "}" {
		printf("int %s_sync(\n\t%s)\n", op, decl_list);
		printf("{\n");
		printf("\tPVP_OP(%s,%s(%s),\"%s\");\n", \
			target_parm, OP, parm_list, op);
		printf("}\n");
		if (op_type != "ASYNC")
			next;
		printf("int %s_send(\n\t%s)\n", op, decl_list);
		printf("{\n");
		printf("\tPVP_OP(%s,%s_SEND(%s),\"%s\");\n", \
			target_parm, OP, parm_list, op);
		printf("}\n");
		printf("int %s_receive(\n\t%s)\n", op, decl_list);
		printf("{\n");
		printf("\tPVP_OP(%s,%s_RECEIVE(%s),\"%s\");\n", \
			target_parm, OP, parm_list, op);
		printf("}\n");
	}
EOF
$AWK -f $TMPDIR/awk.wrappers
