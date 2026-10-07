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
# $Log: gen_dispatch_table.sh,v $
# Revision 1.2  1994/11/18  20:51:05  mtm
# Copyright additions/changes
#
# Revision 1.1  1994/04/28  19:20:44  chrisp
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
# Generate the declarations of the dispatch tables to match
# the declarations cracked from the headers. This produces
# dvpop_tables_gen.c which is included in dvp_init.c.
#
cat <<EOF >$TMPDIR/awk.dispatch_table
	BEGIN {
		table_type = "$QUALIFIER";
		name = "$NAME";
		prefix = "$PREFIX";
		if (table_type ~ /sync/)
			suffix = "_sync";
		else if (table_type ~ /send/)
			suffix = "_send";
		else if (table_type ~ /receive/)
			suffix = "_receive";
		if (table_type ~ /remote/)
			printf("struct %s r%s_table%s = {\n", \
				name, name, suffix);
		else if (table_type ~ /base/)
			printf("struct %s b%s_table%s = {\n", \
				name, name, suffix);
		else
			printf("struct %s d%s_table%s = {\n", \
				name, name, suffix);
		n_ops = 0;
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
		op = prefix "op_" \$2;
		op_type = \$3;
		if (n_ops != 0)
			printf(",\n");
		n_ops++;
		if (table_type !~ /remote/) {
			if (table_type ~ /send/)
				printf("\tdpvpop_nop");
			else if (table_type ~ /base/)
				printf("\tb%s", op);
			else
				printf("\td%s", op);
		} else {
			if (table_type ~ /send/) {
				if (op_type == "ASYNC")
					printf("\tr%s_send", op);
				else
					printf("\tdpvpop_nop");
				}
			else if (table_type ~ /receive/) {
				if (op_type == "ASYNC")
					printf("\tr%s_receive", op);
				else
					printf("\tr%s", op);
				}
			else
				printf("\tr%s", op);
		}
	}

	END {
		printf("\n};\n");
	}
EOF
$AWK -f $TMPDIR/awk.dispatch_table
