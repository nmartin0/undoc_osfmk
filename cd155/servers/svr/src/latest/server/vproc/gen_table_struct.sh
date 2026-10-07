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
# $Log: gen_table_struct.sh,v $
# Revision 1.2  1994/11/18  20:51:08  mtm
# Copyright additions/changes
#
# Revision 1.1  1994/04/28  19:20:55  chrisp
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
# Generate a structure declaration for an operation table in the form:
#	struct <table_name>_ops {
#		int (*<op_name>)
#			(<type info> <parameter_name>,
#			...
#			<type info> <parameter_name>);
#		...
#	}
#
cat >$TMPDIR/awk.table_struct <<EOF
	BEGIN {
		prefix = "$PREFIX";
		name = "$NAME";
		printf("struct %s {\n", name);
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
		printf("\tint (*%s%s%s) (\n", prefix, "op_", \$2);
		n_parms = 0;
	}

	\$1 == "parameter" {
		if (n_parms != 0)
			printf(",\n");
		n_parms++;
		printf("\t\t");
		for (i=3; i<=NF; i++) {
			if (i == 3)
				printf("%s", \$i);
			else
				printf(" %s", \$i);
		}
	}

	\$1 == "}" {
		printf("\n\t);\n");
	}

	END {
		printf("};\n");
	}
EOF
$AWK -f $TMPDIR/awk.table_struct
