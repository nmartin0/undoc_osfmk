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
# $Log: gen_server_stubs.sh,v $
# Revision 1.3  1995/02/01  21:44:56  bolsen
#  Reviewer(s): Jerry Toman
#  Risk: Medium (lots of files)
#  Module(s): Too many to list
#  Configurations built: STD, LITE, & RAMDISK
#
#  Added or Updated the Locus Copyright message.
#
# Revision 1.2  1994/11/18  20:43:31  mtm
# Copyright additions/changes
#
# Revision 1.1  1994/04/28  19:14:29  chrisp
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
# Generate the remote operations server stubs (tnc/rvp_pvpops_server_gen.c
# and tnc/rvp_pvpsops_server_gen.c) for the pvproc and pvps ops.
# Each operation server stub is of the form:
#	DECL_R<pvp>_SERVER_OP(r<op_name>)
#		DECL_R<pvp>_DESTINATION,
#		<parm_type_2> <parm_name_2>,
#			... repeated 
#		<parm_type_n> <parm_name_n>
#	END_DECL
#	{
#		START_R<pvp>_SERVER_OP("<op_name>");
#		R<pvp>_SERVER_OP(<OP_NAME>(<pvp>_DESTINATION,
#				<parm_name_2>,...,<parm_name_n>));
#		END_R<pvp>_SERVER_OP;
#	}
#	where <pvp> is "PVP" for pvproc operations and "PVPS" for private
#	virtual process system operations.
#	IN parameters of type struct vproc are specially treated with the
#	following transformations being performed:
#	    o	the formal declaration becomes "<parm_name>_pid"
#	    o	"LOCATE_VALID_VPROC_PID(<parm_name>_pid,<parm_name>)"
#		appears before the R<pvp>_SERVER_OP() invocation
#	    o	"VALID_VPROC_RELEASE(<parm_name>)"
#		appears following the R<pvp>_SERVER_OP() invocation
#
cat <<EOF >$TMPDIR/awk.server_stubs
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
		esc_type = \$2
		next;
	}

	\$1 == "end_escape" {
		in_escape = 0;
		next;
	}

	{
		if (in_escape) {
			if (esc_type ~ /SERVER/)
				escape[esc_type] = escape[esc_type] \$0 "\n";
			next;
		}
	}

	\$1 == "operation" {
		op = pvp "op_" \$2;
		OP = "";
		for (i=1;i<=length(op);i++)
			OP = OP to_upper[substr(op,i,1)];
		n_parms = 0;
		n_vp_parms = 0;
		escape["SERVER_ENTRY"] = "";
		escape["SERVER_PRE-OP"] = "";
		escape["SERVER_POST-OP"] = "";
	}

	\$1 == "parameter" {
		n_parms++;
		parm_name = \$NF;
		parm_type = \$3;
		for (i=4; i<NF; i++)
			parm_type = parm_type " " \$i;
		while (substr(parm_name,1,1) == "*") {
			parm_name = substr(parm_name, 2, length(parm_name));
			parm_type = parm_type " *";
		}
		if (n_parms == 1) {
			printf("DECL_R%s_SERVER_OP(r%s)\n", PVP, op);
			printf("\tDECL_R%s_DESTINATION", PVP);
		} else if (\$2 ~ /^IN/ && \$3 == "struct" && \$4 == "vproc") {
			printf(",\n\tpid_t %s_pid", parm_name);
			vp_parm[++n_vp_parms] = n_parms;
		}
		else
			printf(",\n\t%s %s", parm_type, parm_name);
		parm[n_parms] = parm_name;
	}

	\$1 == "}" {
		printf("\nEND_DECL\n");
		other_parms = "";
		for (i=2; i<=n_parms; i++)
			other_parms = sprintf("%s,%s", other_parms, parm[i]);
		printf("{\n");
		if (n_vp_parms > 0) for (i=1; i<=n_vp_parms; i++)
			printf("\tstruct vproc *%s = NULL;\n", \
				parm[vp_parm[i]]); 
		printf("%s", escape["SERVER_ENTRY"]);
		printf("\tSTART_R%s_SERVER_OP(\"%s\");\n", PVP, op);
		if (n_vp_parms > 0) for (i=1; i<=n_vp_parms; i++)
			printf("\tLOCATE_VALID_VPROC_PID(%s_pid,%s);\n", \
				parm[vp_parm[i]], parm[vp_parm[i]]); 
		printf("%s", escape["SERVER_PRE-OP"]);
		printf("\tR%s_SERVER_OP(%s(%s_DESTINATION%s));\n", \
			PVP, OP, PVP, other_parms);
		printf("%s", escape["SERVER_POST-OP"]);
		if (n_vp_parms > 0) for (i=1; i<=n_vp_parms; i++)
			printf("\t%VALID_VPROC_RELEASE(%s);\n", \
				parm[vp_parm[i]]); 
		printf("\tEND_R%s_SERVER_OP;\n", PVP);
		printf("}\n");
	}
EOF
$AWK -f $TMPDIR/awk.server_stubs
