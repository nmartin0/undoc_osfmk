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
# $Log: tncgen.mk,v $
# Revision 1.5  1995/02/10  18:18:33  toman
# Added "rm -f" before and "chmod u+w" after "cp" commands in the makefiles
# so rebuilds do not fail because of permissions.
#
#  Reviewer: Bob Yasi, John Litvin, Suri Brahmaroutu
#  Risk: Low
#  Benefit or PTS #: 12406
#  Testing: Built, touched .ops files and scripts, rebuilt
#  Module(s): server/tnc/tncgen.mk
#             server/vproc/vprocgen.mk
#
# Revision 1.4  1995/02/01  21:59:40  bolsen
#  Reviewer(s): Jerry Toman
#  Risk: Medium (lots of files)
#  Module(s): Too many to list
#  Configurations built: STD, LITE, & RAMDISK
#
#  Added or Updated the Locus Copyright message.
#
# Revision 1.3  1994/11/18  20:44:36  mtm
# Copyright additions/changes
#
# Revision 1.2  1994/05/13  21:42:24  chrisp
# Don't retain permissions when copying files to object tree.
#
#  Reviewer: None
#  Risk: L
#  Benefit or PTS #: 9429
#  Testing: None
#  Module(s): vproc/vprocgen.mk tnc/tncgen.mk
#
# Revision 1.1  1994/04/28  19:15:15  chrisp
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
# Makefile for the following TNC generated files:
#
GEN_FILES =	tnc/dvproc_protos_gen.h \
		tnc/dvps_protos_gen.h \
		tnc/dpvproc_protos_gen.h \
		tnc/dpvproc_struct_gen.h \
		tnc/dpvproc_macros_gen.h \
		tnc/dpvps_protos_gen.h \
		tnc/dpvps_struct_gen.h \
		tnc/dpvps_macros_gen.h \
		tnc/dvpop_tables_gen.c \
		tnc/pvpops_gen.c \
		tnc/rvp_pvpops_gen.c \
		tnc/rvp_pvpops_server_gen.c \
		tnc/rvp_pvpsops_gen.c \
		tnc/rvp_pvpsops_server_gen.c \
		tnc/tnc_gen.defs \
		tnc/tnc_async_gen.defs

default:	./tnc $(GEN_FILES)

./tnc:
		mkdir $@

#
# The files are generated using transformation scripts:
#
TNCGEN =	vproc/tncgen

GEN_SCRIPTS =	vproc/gen_client_stubs.sh \
		vproc/gen_mig.sh \
		vproc/gen_migrate_wrappers.sh \
		vproc/gen_server_stubs.sh
#
# driven by input from operations definition files:
#
TNC_OPS =	tnc/pvp.ops \
		tnc/pvps.ops

#
# Copy scripts and their input into the object tree,
#	ensuring the master script has execute permissions,
#	and the tnc-specific scripts are put into the vproc obj dir.
#
$(TNCGEN):	$(SOURCE_DIR)/$$@
		rm -f ./$@
		cp $(SOURCE_DIR)/$@ ./$@
		chmod +x ./$@
		chmod u+w ./$@

$(TNC_OPS):	$(SOURCE_DIR)/$$@
		rm -f ./$@
		cp $(SOURCE_DIR)/$@ ./$@
		chmod u+w ./$@

$(GEN_SCRIPTS):	$(SOURCE_DIR)/tnc/$$(@F)
		rm -f ./$@
		cp $> ./$@
		chmod u+w ./$@

#
# Prototypes for operation functions:
#	Note that remote operations require a 2nd pass.
#
tnc/dvproc_protos_gen.h:	$(TNCGEN) vproc/gen_prototypes.sh vproc/vp.ops
	$(TNCGEN) $(IDENT) -p -x dvp -o $@ vproc/vp.ops

tnc/dvps_protos_gen.h:		$(TNCGEN) vproc/gen_prototypes.sh vproc/vps.ops
	$(TNCGEN) $(IDENT) -p -x dvps -o $@ vproc/vps.ops

tnc/dpvproc_protos_gen.h:	$(TNCGEN) vproc/gen_prototypes.sh tnc/pvp.ops
	$(TNCGEN) $(IDENT) -p -x dpvp -o $@ tnc/pvp.ops
	$(TNCGEN) $(IDENT) -p -x rpvp tnc/pvp.ops >>$@

tnc/dpvps_protos_gen.h:	$(TNCGEN) vproc/gen_prototypes.sh tnc/pvps.ops
	$(TNCGEN) $(IDENT) -p -x dpvps -o $@ tnc/pvps.ops
	$(TNCGEN) $(IDENT) -p -x rpvps tnc/pvps.ops >>$@


#
# struct declarations for operations tables:
#
tnc/dpvproc_struct_gen.h:	$(TNCGEN) vproc/gen_table_struct.sh tnc/pvp.ops
	$(TNCGEN) $(IDENT) -t -n pvproc_ops -o $@ tnc/pvp.ops

tnc/dpvps_struct_gen.h:		$(TNCGEN) vproc/gen_table_struct.sh tnc/pvps.ops
	$(TNCGEN) $(IDENT) -t -n pvps_ops -o $@ tnc/pvps.ops

#
# Dispatch table instantiation for operations:
#	Multiple passes are needed to build all instances
#	of local and remote, synchronous and asynchronous tables.
#	Include explicit declarations of indirect asynchronous operations
#	-- these should be moved into sources including this file.
#
tnc/dvpop_tables_gen.c:	$(TNCGEN) vproc/gen_dispatch_table.sh \
			vproc/vp.ops vproc/vps.ops tnc/pvp.ops tnc/pvps.ops
	@echo >$@  "int dpvpop_nop() { return(ESUCCESS); }"
	$(TNCGEN) $(IDENT) -d -n vproc_ops vproc/vp.ops >>$@
	$(TNCGEN) $(IDENT) -d -n vps_ops vproc/vps.ops >>$@
	$(TNCGEN) $(IDENT) -d -n pvproc_ops -q "local sync" tnc/pvp.ops >>$@
	$(TNCGEN) $(IDENT) -d -n pvproc_ops -q "local send" tnc/pvp.ops >>$@
	$(TNCGEN) $(IDENT) -d -n pvproc_ops -q "local receive" tnc/pvp.ops >>$@
	$(TNCGEN) $(IDENT) -d -n pvproc_ops -q "remote sync" tnc/pvp.ops >>$@
	$(TNCGEN) $(IDENT) -d -n pvproc_ops -q "remote send" tnc/pvp.ops >>$@
	$(TNCGEN) $(IDENT) -d -n pvproc_ops -q "remote receive" tnc/pvp.ops >>$@
	$(TNCGEN) $(IDENT) -d -n pvps_ops -q "local sync" tnc/pvps.ops >>$@
	$(TNCGEN) $(IDENT) -d -n pvps_ops -q "local send" tnc/pvps.ops >>$@
	$(TNCGEN) $(IDENT) -d -n pvps_ops -q "local receive" tnc/pvps.ops >>$@
	$(TNCGEN) $(IDENT) -d -n pvps_ops -q "remote sync" tnc/pvps.ops >>$@
	$(TNCGEN) $(IDENT) -d -n pvps_ops -q "remote send" tnc/pvps.ops >>$@
	$(TNCGEN) $(IDENT) -d -n pvps_ops -q "remote receive" tnc/pvps.ops >>$@
	@echo >>$@ "struct pvproc_ops_vector dpvproc_ops_table = {"
	@echo >>$@ "	&dpvproc_ops_table_sync,"
	@echo >>$@ "	&dpvproc_ops_table_send,"
	@echo >>$@ "	&dpvproc_ops_table_receive"
	@echo >>$@ "};"
	@echo >>$@ "struct pvproc_ops_vector rpvproc_ops_table = {"
	@echo >>$@ "	&rpvproc_ops_table_sync,"
	@echo >>$@ "	&rpvproc_ops_table_send,"
	@echo >>$@ "	&rpvproc_ops_table_receive"
	@echo >>$@ "};"
	@echo >>$@ "struct pvps_ops_vector dpvps_ops_table = {"
	@echo >>$@ "	&dpvps_ops_table_sync,"
	@echo >>$@ "	&dpvps_ops_table_send,"
	@echo >>$@ "	&dpvps_ops_table_receive"
	@echo >>$@ "};"
	@echo >>$@ "struct pvps_ops_vector rpvps_ops_table = {"
	@echo >>$@ "	&rpvps_ops_table_sync,"
	@echo >>$@ "	&rpvps_ops_table_send,"
	@echo >>$@ "	&rpvps_ops_table_receive"
	@echo >>$@ "};"

#
# Macros for operation invocation:
#
tnc/dpvproc_macros_gen.h:	$(TNCGEN) vproc/gen_op_macros.sh tnc/pvp.ops
	$(TNCGEN) $(IDENT) -m -o $@ tnc/pvp.ops

tnc/dpvps_macros_gen.h:		$(TNCGEN) vproc/gen_op_macros.sh tnc/pvps.ops
	$(TNCGEN) $(IDENT) -m -o $@ tnc/pvps.ops

#
# Migration interlock wrappers for pvproc operations:
#
tnc/pvpops_gen.c:	$(TNCGEN) vproc/gen_migrate_wrappers.sh tnc/pvp.ops
	$(TNCGEN) $(IDENT) -w -o $@ tnc/pvp.ops

#
# Client stubs for remote operations:
#
tnc/rvp_pvpops_gen.c:	$(TNCGEN) vproc/gen_client_stubs.sh tnc/pvp.ops
	$(TNCGEN) $(IDENT) -c -o $@ tnc/pvp.ops

tnc/rvp_pvpsops_gen.c:	$(TNCGEN) vproc/gen_client_stubs.sh tnc/pvps.ops
	$(TNCGEN) $(IDENT) -c -o $@ tnc/pvps.ops

#
# Server stubs for remote operations:
#
tnc/rvp_pvpops_server_gen.c:	$(TNCGEN) vproc/gen_server_stubs.sh tnc/pvp.ops
	$(TNCGEN) $(IDENT) -s -o $@ tnc/pvp.ops

tnc/rvp_pvpsops_server_gen.c:	$(TNCGEN) vproc/gen_server_stubs.sh tnc/pvps.ops
	$(TNCGEN) $(IDENT) -s -o $@ tnc/pvps.ops

#
# MIG definitions for remote operations:
#	Normal, synchronous operations followed by async versions.
#
tnc/tnc_gen.defs:	$(TNCGEN) vproc/gen_mig.sh tnc/pvp.ops tnc/pvps.ops
	$(TNCGEN) $(IDENT) -i -o $@ tnc/pvp.ops
	$(TNCGEN) $(IDENT) -i tnc/pvps.ops >> $@

tnc/tnc_async_gen.defs:	$(TNCGEN) vproc/gen_mig.sh tnc/pvp.ops tnc/pvps.ops
	$(TNCGEN) $(IDENT) -i -q async -o $@ tnc/pvp.ops
	$(TNCGEN) $(IDENT) -i -q async tnc/pvps.ops >> $@
