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
# $Log: vprocgen.mk,v $
# Revision 1.4  1995/02/10  18:57:19  toman
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
# Revision 1.3  1994/11/18  20:51:18  mtm
# Copyright additions/changes
#
# Revision 1.2  1994/05/13  21:42:01  chrisp
# Don't retain permissions when copying files to object tree.
#
#  Reviewer: None
#  Risk: L
#  Benefit or PTS #: 9429
#  Testing: None
#  Module(s): vproc/vprocgen.mk tnc/tncgen.mk
#
# Revision 1.1  1994/04/28  19:21:05  chrisp
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
# Makefile for the following TNC generated files:
#
GEN_FILES =	vproc/bvproc_protos_gen.h \
		vproc/vproc_macros_gen.h \
		vproc/vproc_struct_gen.h \
		vproc/bvps_protos_gen.h \
		vproc/vps_macros_gen.h \
		vproc/vps_struct_gen.h \
		vproc/bvpop_tables_gen.c

default:	./vproc $(GEN_FILES)

./vproc:
		mkdir $@
#
# The files are generated using transformation scripts:
#
TNCGEN =	vproc/tncgen

GEN_SCRIPTS =	vproc/gen_dispatch_table.sh \
		vproc/gen_op_macros.sh \
		vproc/gen_prototypes.sh \
		vproc/gen_table_struct.sh
#
# driven by input from operations definition files:
#
TNC_OPS =	vproc/vp.ops \
		vproc/vps.ops

#
# Copy scripts and their input into the object tree,
#	ensuring that the master script has execute permissions.
#
$(TNCGEN):	$(SOURCE_DIR)/$$@
		rm -f ./$@
		cp $(SOURCE_DIR)/$@ ./$@
		chmod +x ./$@
		chmod u+w ./$@

$(TNC_OPS) $(GEN_SCRIPTS):	$(SOURCE_DIR)/$$@
		rm -f ./$@
		cp $(SOURCE_DIR)/$@ ./$@
		chmod u+w ./$@

#
# Prototypes for operation functions:
#
vproc/bvproc_protos_gen.h:	$(TNCGEN) vproc/gen_prototypes.sh vproc/vp.ops
	$(TNCGEN) $(IDENT) -p -x bvp -o $@ vproc/vp.ops

vproc/bvps_protos_gen.h:	$(TNCGEN) vproc/gen_prototypes.sh vproc/vps.ops
	$(TNCGEN) $(IDENT) -p -x bvps -o $@ vproc/vps.ops


#
# struct declarations for operations tables:
#
vproc/vproc_struct_gen.h:	$(TNCGEN) vproc/gen_table_struct.sh vproc/vp.ops
	$(TNCGEN) $(IDENT) -t -n vproc_ops -o $@ vproc/vp.ops

vproc/vps_struct_gen.h:		$(TNCGEN) vproc/gen_table_struct.sh vproc/vps.ops
	$(TNCGEN) $(IDENT) -t -n vps_ops -o $@ vproc/vps.ops

#
# Dispatch table instantiation for operations ("base" => bvp):
#
vproc/bvpop_tables_gen.c:	$(TNCGEN) vproc/gen_dispatch_table.sh \
				vproc/vp.ops vproc/vps.ops
	$(TNCGEN) $(IDENT) -d -n vproc_ops -q base  vproc/vp.ops >$@
	$(TNCGEN) $(IDENT) -d -n vps_ops -q base vproc/vps.ops >>$@

#
# Macros for operation invocation:
#
vproc/vproc_macros_gen.h:	$(TNCGEN) vproc/gen_op_macros.sh vproc/vp.ops
	$(TNCGEN) $(IDENT) -m -o $@ vproc/vp.ops

vproc/vps_macros_gen.h:		$(TNCGEN) vproc/gen_op_macros.sh vproc/vps.ops
	$(TNCGEN) $(IDENT) -m -o $@ vproc/vps.ops
