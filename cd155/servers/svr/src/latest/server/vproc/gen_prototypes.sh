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
# $Log: gen_prototypes.sh,v $
# Revision 1.2  1994/11/18  20:51:07  mtm
# Copyright additions/changes
#
# Revision 1.1  1994/04/28  19:20:50  chrisp
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
#	Generate ANSI C prototype declarations for operations.
#
cat <<EOF >$TMPDIR/awk.prototypes
	BEGIN {
		prefix = "$PREFIX";
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
		op = "op_" \$2;
		op_type = \$3;
		n_parms = 0;
		parm_string = "";
	}

	\$1 == "parameter" {
		if (n_parms == 0)
			parm_string = \$3;
		else
			parm_string = parm_string ", " \$3;
		n_parms++;
		for (i=4; i<=NF; i++)
			parm_string = parm_string " " \$i;
	}
	\$1 == "}" {
		printf("extern int %s%s(%s);\n", \
			prefix, op, parm_string);
		if (op_type != "ASYNC")
			next;
		printf("extern int %s%s_send(%s);\n", \
			prefix, op, parm_string);
		printf("extern int %s%s_receive(%s);\n", \
			prefix, op, parm_string);
	}
EOF
$AWK -f $TMPDIR/awk.prototypes
