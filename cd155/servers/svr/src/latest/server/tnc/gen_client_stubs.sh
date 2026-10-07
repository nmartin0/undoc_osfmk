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
# $Log: gen_client_stubs.sh,v $
# Revision 1.3  1995/02/01  21:43:48  bolsen
#  Reviewer(s): Jerry Toman
#  Risk: Medium (lots of files)
#  Module(s): Too many to list
#  Configurations built: STD, LITE, & RAMDISK
#
#  Added or Updated the Locus Copyright message.
#
# Revision 1.2  1994/11/18  20:43:29  mtm
# Copyright additions/changes
#
# Revision 1.1  1994/04/28  19:14:18  chrisp
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
# Generate the remote operations client stubs (tnc/rvp_pvpops_gen.c
# and tnc/rvp_pvpsops_gen.c) for the pvproc and pvps ops.
# Each operation client stub is of the form:
#	int r<op_name><mode>(
#		<parm_type_1> <parm_name_1>,
#		<parm_type_2> <parm_name_2>,
#			... repeated 
#		<parm_type_n> <parm_name_n>)
#	{
#		START_R<pvp>_CLIENT_OP<MODE>(<parm_name_1>,"<op_name>");
#		R<pvp>_CLIENT_OP<MODE>(
#			r<op_name>,(R<pvp>_DEST<MODE>(<parm_name_1>,
#				<parm_name_2>,...,<parm_name_n>));
#		END_R<pvp>_CLIENT_OP<MODE>;
#	}
#	where <pvp> is "PVP" for pvproc operations and "PVPS" for private
#	virtual process system operations and <mode> is "" for all operations
#	but, in addition, the stub is regenerated with values of "_send"
#	and "_receive" for ASYNC operations.
#	Note, in addition, that OUT parameters appear in the client RPC
#	in the form "<parm_name> ? <parm_name> : (<parm_type> *)&dummy" to
#	avoid null pointers being passed to the RPC. Furthermore, IN
#	parameters of type struct vproc are transformed to be:
#	"<parm_name> ? <parm_name>->vp_pid : 0".
#
cat <<EOF >$TMPDIR/awk.client_stubs
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
			if (esc_type ~ /CLIENT/)
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
		op_type = \$3;
		escape["CLIENT_ENTRY"] = "";
		escape["CLIENT_PRE-RPC"] = "";
		escape["CLIENT_POST-RPC"] = "";
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
		last_parm = parm_name;
		if (n_parms == 1) {
			decl_list = parm_type " " parm_name;
			target_parm = parm_name;
			other_parms = "";
			next;
		}
		decl_list = decl_list ",\n\t" parm_type " " parm_name;
		other_parms = other_parms ",";
		if (\$2 ~ /^OUT|^INOUT/ && \$2 !~ /VAR_ARRAY/) 
			other_parms = other_parms parm_name " ? " \
					parm_name " : (" parm_type ")&dummy";
		else if (\$2 ~ /^IN/ && \$3 == "struct" && \$4 == "vproc")
			other_parms = other_parms parm_name " ? " \
					parm_name "->vp_pid : 0";
		else
			other_parms = other_parms parm_name;
	}

	\$1 == "}" {
		parm_list = target_parm other_parms;
		printf("int r%s(\n\t%s)\n", op, decl_list);
		printf("{\n");
		if (parm_list ~ /dummy/)
			printf("\tint dummy;\n");
		printf("%s", escape["CLIENT_ENTRY"]);
		printf("\tSTART_R%s_CLIENT_OP(%s,\"%s\");\n", \
			PVP, target_parm, op);
		printf("%s", escape["CLIENT_PRE-RPC"]);
		printf("\tR%s_CLIENT_OP(r%s,(R%s_DESTINATION(%s)%s));\n", \
			PVP, op, PVP, target_parm, other_parms);
		printf("%s", escape["CLIENT_POST-RPC"]);
		printf("\tEND_R%s_CLIENT_OP;\n", PVP);
		printf("}\n");
		if (op_type != "ASYNC")
			next;
		handle_parm = last_parm;
		printf("int r%s_send(\n\t%s)\n", op, decl_list);
		printf("{\n");
		if (parm_list ~ /dummy/)
			printf("\tint dummy;\n");
		printf("%s", escape["CLIENT_ENTRY"]);
		printf("\tSTART_R%s_CLIENT_OP_SEND(%s,%s,\"%s\");\n", \
			PVP, target_parm, handle_parm, op);
		printf("%s", escape["CLIENT_PRE-RPC"]);
		printf("\tR%s_CLIENT_OP_SEND(r%s,(R%s_DEST_SEND(%s)%s));\n", \
			PVP, op, PVP, target_parm, other_parms);
		printf("%s", escape["CLIENT_POST-RPC"]);
		printf("\tEND_R%s_CLIENT_OP_SEND;\n", PVP);
		printf("}\n");
		printf("int r%s_receive(\n\t%s)\n", op, decl_list);
		printf("{\n");
		if (parm_list ~ /dummy/)
			printf("\tint dummy;\n");
		printf("%s", escape["CLIENT_ENTRY"]);
		printf("\tSTART_R%s_CLIENT_OP_RECEIVE(%s,%s,\"%s\");\n", \
			PVP, target_parm, handle_parm, op);
		printf("%s", escape["CLIENT_PRE-RPC"]);
		printf("\tR%s_CLIENT_OP_RECEIVE(r%s,(R%s_DEST_RECEIVE(%s)%s));\n", \
			PVP, op, PVP, target_parm, other_parms);
		printf("%s", escape["CLIENT_POST-RPC"]);
		printf("\tEND_R%s_CLIENT_OP_RECEIVE;\n", PVP);
		printf("}\n");
	}
EOF
$AWK -f $TMPDIR/awk.client_stubs
