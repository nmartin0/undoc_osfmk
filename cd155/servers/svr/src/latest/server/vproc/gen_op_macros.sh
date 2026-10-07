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
# Copyright (c) 1993, Locus Computing Corporation
# All rights reserved
#
# HISTORY
# $Log: gen_op_macros.sh,v $
# Revision 1.2  1994/11/18  20:51:06  mtm
# Copyright additions/changes
#
# Revision 1.1  1994/04/28  19:20:47  chrisp
# Changes for introduction of tncgen and support of i386 builds.
# Generated prototype header files changed for consistency with tnc/.
#
#  Reviewer: dleslie, cfj
#  Risk: M
#  Benefit or PTS #: 9188
#  Testing: Builds and tested on i386 platform.
#  Module(s):
#      Modified Files:
#      	bvp_init.c bvp_vpops.c bvp_vpsops.c
#      Added Files:
#      	gen_dispatch_table.sh gen_op_macros.sh gen_prototypes.sh
#      	gen_table_struct.sh tncgen vp.ops vprocgen.mk vps.ops
#      Removed Files:
#      	maketables.sh
#
# $EndLog$
#

#
# Generate the remote operation macros (tnc/<table>_macros_gen.h).
# Note that this file is included in dpvproc.h.
# Each operations wrapper is of the form:
#	#define <OP_NAME>(x, <parm_name_1>, ... <parm_name_n>) \
#		_<PVP>OP_(<op_name>, _x, _<parm_name_1>, ... _<parm_name_n>) \
#			"<fmt_string>" \
#	where <PVP> is "PVP" for pvproc operations and "PVPS" for private
#	virtual process system operations.
#
cat <<EOF >$TMPDIR/awk.op_macros
	BEGIN {
		for (i=32;i<128;i++) {
			ch = sprintf("%c", i);
			if (ch ~/[a-z]/)
				to_upper[ch] = sprintf("%c", i-32);
			else
				to_upper[ch] = ch;
		}
		pvp = "$PREFIX";
		PVP = "";
		for (i=1;i<=length(pvp);i++)
			PVP = PVP to_upper[substr(pvp,i,1)];
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
		parm_type = "value";
		while (substr(parm_name,1,1) == "*") {
			parm_name = substr(parm_name, 2, length(parm_name));
			parm_type = "pointer";
		}
		if (n_parms == 1) {
			op_parms = "_" parm_name;
			target_parm = op_parms;
			dbg_str = "";
		} else {
			op_parms = op_parms ", _" parm_name;
			dbg_str = dbg_str ", ";
		}
		if (\$2 ~ /OUT|INOUT/ || parm_type == "pointer")
			dbg_str = dbg_str parm_name "=&0x%x";
		else
			dbg_str = dbg_str parm_name "=%d";
	}

	\$1 == "}" {
		printf("#define %s(%s) \\\\\n", OP, op_parms);
		printf("\t_%sOP_(%s, %s, (%s), \\\\\n", \
			PVP, op, target_parm, op_parms);
		printf("\t\t\"(%s)\")\n", dbg_str);
		if (op_type != "ASYNC")
			next;
		printf("#define %s_SEND(%s) \\\\\n", OP, op_parms);
		printf("\t_%sOP_SEND_(%s, %s, (%s), \\\\\n", \
			PVP, op, target_parm, op_parms);
		printf("\t\t\"(%s)\")\n", dbg_str);
		printf("#define %s_RECEIVE(%s) \\\\\n", OP, op_parms);
		printf("\t_%sOP_RECEIVE_(%s, %s, (%s), \\\\\n", \
			PVP, op, target_parm, op_parms);
		printf("\t\t\"(%s)\")\n", dbg_str);
	}
EOF
$AWK -f $TMPDIR/awk.op_macros
