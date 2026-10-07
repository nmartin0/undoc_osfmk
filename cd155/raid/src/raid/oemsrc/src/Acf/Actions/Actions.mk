# 
# $Copyright
# Copyright 1992, 1993, 1994, 1995  Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
# TOPDIR must be defined as the relative path FROM *this* directory TO the top
# level source directory.
TOPDIR		= ../..
# FROMTOP must be defined as the relative path FROM the top level source
# directory TO *this* subdir.  NOTE: Both FROMTOP and TOPDIR are redefined as
# make recurses the source directory hierarchy.  FROMTOP must always begin
# with a '.' in all make files.
FROMTOP		= ./Acf/Actions


MINCL		= ../../Make.incl
MRULES		= ../../Make.rules

include $(MINCL)
include $(MRULES)

#AR		= /bin/ar

INCDR1		= ./Include
INCDR2		= ../Menus/Include
INCDR3		= ../../Common/Include
INCDR4		= ../../Dacparams/Include
INCDR5		= ../../Sysdep/$(SYS)/Include
INCLUDES	= -I$(INCDR1) -I$(INCDR2) -I$(INCDR3) -I$(INCDR4) -I$(INCDR5)
CFLAGS		= $(SWITCHES) $(DEFS) $(INCLUDES)

LIB		= ../../../lib/$(SYS)
LIBRARY		= $(LIB)/Acf.a
THISMK		= ./Actions
C_SRCS		= cf_action.c dcf_action.c dld_action.c ecf_action.c \
		  ucf_action.c fmt_action.c inq_action.c ccf_action.c \
		  can_action.c

MEMBERS		= $(LIBRARY)(cf_action.o) \
		  $(LIBRARY)(ecf_action.o) \
		  $(LIBRARY)(ucf_action.o) \
		  $(LIBRARY)(dcf_action.o) \
		  $(LIBRARY)(dld_action.o) \
		  $(LIBRARY)(fmt_action.o) \
		  $(LIBRARY)(inq_action.o) \
		  $(LIBRARY)(ccf_action.o) \
		  $(LIBRARY)(can_action.o)

.PRECIOUS:	$(LIBRARY)

all:		Download $(LIBRARY)

#  .DEFAULT rule is used to retrieve .c & .h source files from SCCS directories
#.DEFAULT:
#	$(GET) $(@F) >/dev/null 2>&1

Download::
	cd $@; $(MAKE) -ef $@.mk

$(LIBRARY):	$(MEMBERS)

$(MEMBERS):	$(MINCL)

dependlist: $(C_SRCS) $(MEMBERS)
	-gcc -MM $(INCLUDES) -I- $(C_SRCS) 2>/dev/null \
	  | sed -e "s/^\(.*\) :/$$(LIBRARY)(\1) :/"

depend:
	$(DEPEND)
	cd Download; $(MAKE) -f Download.mk depend

#__DEPENDLIST__
$(LIBRARY)(cf_action.o) : cf_action.c ../../Common/Include/stddefs.h \
  ../../Sysdep/$(SYS)/Include/sysdep.h ../../Common/Include/scsidefs.h \
  ../../Common/Include/cdb_struct.h ./Include/cf_action.h \
  ../../Common/Include/action.h ../../Common/Include/object.h \
  ../../Common/Include/menu.h ../../Common/Include/dialog.h \
  ../../Common/Include/object_set.h ../../Common/Include/dac_param.h \
  ../../Common/Include/scsi_dev.h ../../Sysdep/$(SYS)/Include/sysio.h \
  ../../Common/Include/zip_dialog.h ../../Common/Include/zipcod_dlg.h 
$(LIBRARY)(dcf_action.o) : dcf_action.c ../../Common/Include/stddefs.h \
  ../../Sysdep/$(SYS)/Include/sysdep.h ../../Common/Include/scsidefs.h \
  ../../Common/Include/cdb_struct.h ./Include/dcf_action.h \
  ./Include/cf_action.h ../../Common/Include/action.h \
  ../../Common/Include/object.h ../../Common/Include/menu.h \
  ../../Common/Include/dialog.h ../../Common/Include/object_set.h \
  ../../Common/Include/dac_param.h ../../Common/Include/scsi_dev.h \
  ../../Sysdep/$(SYS)/Include/sysio.h ../Menus/Include/dcf_menu.h \
  ../../Dacparams/Include/dac_drvsts.h 
$(LIBRARY)(dld_action.o) : dld_action.c ../../Common/Include/stddefs.h \
  ../../Sysdep/$(SYS)/Include/sysdep.h ../../Common/Include/scsidefs.h \
  ../../Common/Include/cdb_struct.h ./Include/dld_action.h \
  ../../Common/Include/action.h ../../Common/Include/object.h \
  ../../Common/Include/menu.h ../../Common/Include/dialog.h \
  ../../Common/Include/object_set.h ../../Common/Include/zip_dialog.h \
  ../../Common/Include/zipcod_dlg.h ../../Common/Include/fnm_dialog.h \
  ../../Dacparams/Include/dac_drvsts.h ../../Common/Include/dac_param.h \
  ../../Common/Include/scsi_dev.h ../../Sysdep/$(SYS)/Include/sysio.h \
  ../../Common/Include/dau_err.h 
$(LIBRARY)(ecf_action.o) : ecf_action.c ../../Common/Include/stddefs.h \
  ../../Sysdep/$(SYS)/Include/sysdep.h ../../Common/Include/scsidefs.h \
  ../../Common/Include/cdb_struct.h ./Include/ecf_action.h \
  ./Include/cf_action.h ../../Common/Include/action.h \
  ../../Common/Include/object.h ../../Common/Include/menu.h \
  ../../Common/Include/dialog.h ../../Common/Include/object_set.h \
  ../../Common/Include/dac_param.h ../../Common/Include/scsi_dev.h \
  ../../Sysdep/$(SYS)/Include/sysio.h ../Menus/Include/ecf_menu.h \
  ../../Dacparams/Include/dac_rretry.h ../../Dacparams/Include/dac_wretry.h \
  ../../Dacparams/Include/dac_arre.h ../../Common/Include/dsw_param.h \
  ../../Dacparams/Include/dac_awre.h ../../Dacparams/Include/dac_per.h 
$(LIBRARY)(ucf_action.o) : ucf_action.c ../../Common/Include/stddefs.h \
  ../../Sysdep/$(SYS)/Include/sysdep.h ../../Common/Include/scsidefs.h \
  ../../Common/Include/cdb_struct.h ./Include/ucf_action.h \
  ./Include/cf_action.h ../../Common/Include/action.h \
  ../../Common/Include/object.h ../../Common/Include/menu.h \
  ../../Common/Include/dialog.h ../../Common/Include/object_set.h \
  ../../Common/Include/dac_param.h ../../Common/Include/scsi_dev.h \
  ../../Sysdep/$(SYS)/Include/sysio.h ../Menus/Include/ucf_menu.h \
  ../../Dacparams/Include/dac_lunsts.h ../../Dacparams/Include/dac_drvmap.h \
  ../../Dacparams/Include/dac_raidlv.h ../../Dacparams/Include/dac_secsiz.h \
  ../../Dacparams/Include/dac_blksiz.h ../../Dacparams/Include/dac_segsiz.h \
  ../../Dacparams/Include/dac_sg0siz.h ../../Dacparams/Include/dac_nblks.h \
  ../../Dacparams/Include/dac_rcamt.h ../../Dacparams/Include/dac_rcfreq.h \
  ../../Dacparams/Include/dac_adr.h ../../Common/Include/dsw_param.h \
  ../../Dacparams/Include/dac_dmw.h ../../Dacparams/Include/dac_aad.h 
$(LIBRARY)(fmt_action.o) : fmt_action.c ../../Common/Include/stddefs.h \
  ../../Sysdep/$(SYS)/Include/sysdep.h ../../Common/Include/scsidefs.h \
  ../../Common/Include/cdb_struct.h ./Include/fmt_action.h \
  ../../Common/Include/action.h ../../Common/Include/object.h \
  ../../Common/Include/menu.h ../../Common/Include/dialog.h \
  ../../Common/Include/object_set.h ../../Common/Include/dau_err.h \
  ../../Common/Include/scsi_dk.h ../../Common/Include/scsi_dev.h \
  ../../Sysdep/$(SYS)/Include/sysio.h ../../Common/Include/zip_dialog.h \
  ../../Common/Include/zipcod_dlg.h 
$(LIBRARY)(inq_action.o) : inq_action.c ../../Common/Include/stddefs.h \
  ../../Sysdep/$(SYS)/Include/sysdep.h ../../Common/Include/scsidefs.h \
  ../../Common/Include/cdb_struct.h ./Include/inq_action.h \
  ../../Common/Include/action.h ../../Common/Include/object.h \
  ../../Common/Include/menu.h ../../Common/Include/dialog.h \
  ../../Common/Include/object_set.h ../../Dacparams/Include/dac_inqdat.h \
  ../../Common/Include/dac_param.h ../../Common/Include/scsi_dev.h \
  ../../Sysdep/$(SYS)/Include/sysio.h ../../Common/Include/zip_dialog.h \
  ../../Common/Include/zipcod_dlg.h 
$(LIBRARY)(ccf_action.o) : ccf_action.c ../../Common/Include/stddefs.h \
  ../../Sysdep/$(SYS)/Include/sysdep.h ../../Common/Include/scsidefs.h \
  ../../Common/Include/cdb_struct.h ../Menus/Include/ccf_menu.h \
  ../../Common/Include/menu.h ../../Common/Include/dialog.h \
  ../../Common/Include/object.h ../../Common/Include/object_set.h \
  ./Include/ccf_action.h ../../Common/Include/action.h ./Include/can_action.h \
  ../../Common/Include/dau_err.h ../../Common/Include/zip_dialog.h \
  ../../Common/Include/zipcod_dlg.h 
$(LIBRARY)(can_action.o) : can_action.c ../../Common/Include/stddefs.h \
  ../../Sysdep/$(SYS)/Include/sysdep.h ../../Common/Include/scsidefs.h \
  ../../Common/Include/cdb_struct.h ../../Common/Include/scsi_dev.h \
  ../../Common/Include/object.h ../../Sysdep/$(SYS)/Include/sysio.h \
  ./Include/can_action.h ../../Common/Include/action.h \
  ../../Common/Include/dau_err.h ../../Common/Include/zip_dialog.h \
  ../../Common/Include/dialog.h ../../Common/Include/zipcod_dlg.h 
