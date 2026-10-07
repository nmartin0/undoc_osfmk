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
# $Log: gen_mig.sh,v $
# Revision 1.6  1995/02/01  21:44:12  bolsen
#  Reviewer(s): Jerry Toman
#  Risk: Medium (lots of files)
#  Module(s): Too many to list
#  Configurations built: STD, LITE, & RAMDISK
#
#  Added or Updated the Locus Copyright message.
#
# Revision 1.5  1994/11/18  20:43:29  mtm
# Copyright additions/changes
#
# Revision 1.4  1994/08/31  22:47:13  mtm
#    This commit is part of the R1_3 branch -> mainline collapse. This
#    action was approved by the R1.X meeting participants.
#
#    Reviewer:        None
#    Risk:            Something didn't get merged properly, or something
#                     left on the mainline that wasn't approved for RTI
#                     (this is VERY unlikely)
#    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
#                     developers will not have to make sure their
#                     changes get onto two separate branches.
#    Testing:         R1_3 branch will be compared (diff'd) with the new
#                     main. (Various tags have been set incase we have to
#                     back up)
#    Modules:         Too numerous to list.
#
# Revision 1.2.2.1  1994/08/03  14:06:15  chrisp
# Remove changes for PTS-9688, tncgen skip facility.
# Modified Files: dvp_pvpsops.c gen_mig.sh pvps.ops
#
# Revision 1.2  1994/07/27  18:43:19  chrisp
# Implement a skip facility in tncgen. Without SLL (for LITE server),
# table_set is skipped.
#
#  Reviewer: yazz@locus.com
#  Risk: M
#  Benefit or PTS #: 9688
#  Testing: Built and run for STD and LITE servers
#  Module(s): dvp_pvpsops.c gen_mig.sh pvps.ops
#
# Revision 1.1  1994/04/28  19:14:21  chrisp
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
# Generate the OSF/1AD-specific MIG definitions for the
# remote TNC operations (tnc_gen.defs) to be included in tnc.defs.
# Note that ASYNC operations are written instead to file tnc_async_gen.defs
# ready for splitting into send and receive segments.
#
# Note that this transformation is riddled with the following assumptions
# and special cases:
#	- Parameters of the form "struct xxx" are converted into types
#	  xxx_t (example: struct rusage_dev becomes rusage_dev_t) under
#	  the assumption that MIG will require such a type definition for
#	  its own purposes.
#	- Handling of variable length buffers: parameters defined as:
#		OUT RPC_VAR_ARRAY xxx_t xxx,
#		INOUT RPC_VAR_ARRAY_LENGTH int *xxxlen
#	  are translated to
#		out xxx : xxx_t;
#	  and, for out-of-line data:
#		OUT RPC_VAR_ARRAY_OUT_OF_LINE xxx_t *xxx,
#		INOUT RPC_VAR_ARRAY_LENGTH int *xxxlen
#	  are translated to
#		out xxx : xxx_t;
cat <<EOF >$TMPDIR/awk.mig
	BEGIN {
		option = "$QUALIFIER";
		prefix = "$PREFIX";
		if (prefix == "pvp")
			destination_decl="\t\tvproc_port\t: mach_port_t";
		if (prefix == "pvps")
			destination_decl="\t\tserver_port\t: mach_port_t";
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
		n_parms = 0;
		op_type = \$3;
		if ((option == "async" && op_type != "ASYNC") || \
		    (option != "async" && op_type == "ASYNC"))
			next;
		if (op_type == "NO_REPLY") {
			printf("simpleroutine r%s(\n", op);
			printf("%s;\n", destination_decl);
			printf("\tin\trval\t\t: rval_t");
		} else {
			printf("routine r%s(\n", op);
			printf("%s;\n", destination_decl);
			printf("\tout\trval\t\t: int");
		}
	}

	\$1 == "parameter" {
		if ((option == "async" && op_type != "ASYNC") || \
		    (option != "async" && op_type == "ASYNC"))
			next;
		if (\$2 ~ /VAR_ARRAY_LENGTH/)
			next;
		n_parms++;
		if (n_parms == 1)
			next;
		parm_name = \$NF;
		parm_type = \$3;
		if (parm_type == "struct")
			parm_type = \$4 "_t";
		while (substr(parm_name,1,1) == "*") {
			parm_name = substr(parm_name, 2, length(parm_name));
		}
		if (\$2 ~ /^INOUT/)
			in_or_out = "inout";
		else if (\$2 ~ /^IN/)
			in_or_out = "in";
		else
			in_or_out = "out";
		if (\$2 ~ /^IN/ && parm_type == "vproc_t")
			printf(";\n\tin\t%-16s: pid_t", \
				sprintf("%s_pid", parm_name));
		else
			printf(";\n\t%s\t%-16s: %s", \
				in_or_out, parm_name, parm_type);
		if (\$2 ~ /VAR_ARRAY_OUT_OF_LINE/ && in_or_out ~ /out/)
			printf(", dealloc");
	}

	\$1 == "}" {
		if ((option == "async" && op_type != "ASYNC") || \
		    (option != "async" && op_type == "ASYNC"))
			next;
		printf(");\n\n");
	}
EOF
$AWK -f $TMPDIR/awk.mig
