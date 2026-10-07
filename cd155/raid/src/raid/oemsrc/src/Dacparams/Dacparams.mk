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
TOPDIR		= ..
# FROMTOP must be defined as the relative path FROM the top level source
# directory TO *this* subdir.  NOTE: Both FROMTOP and TOPDIR are redefined as
# make recurses the source directory hierarchy.  FROMTOP must always begin
# with a '.' in all make files.
FROMTOP		= ./Dacparams


MINCL		= ../Make.incl
MRULES		= ../Make.rules

include $(MINCL)
include $(MRULES)

#AR		= /bin/ar

INCDR1		= ./Include
INCDR2		= ../Common/Include
INCDR3		= ../Sysdep/$(SYS)/Include
INCLUDES	= -I$(INCDR1) -I$(INCDR2) -I$(INCDR3)
CFLAGS		= $(SWITCHES) $(DEFS) $(INCLUDES)

LIB		= ../../lib/$(SYS)
LIBRARY		= $(LIB)/Dacparams.a
THISMK		= ./Dacparams
C_SRCS		= dac_aad.c  dac_adr.c  dac_arre.c  dac_awre.c  dac_blksiz.c \
		  dac_dmw.c  dac_drvmap.c  dac_drvsts.c  dac_lunsts.c \
		  dac_nblks.c  dac_per.c  dac_raidlv.c  dac_rcamt.c \
		  dac_rcfreq.c  dac_rretry.c  dac_secsiz.c  dac_segsiz.c \
		  dac_sg0siz.c  dac_time.c  dac_wretry.c dac_inqdat.c \
		  dac_rcmde.c dac_arcmde.c

MEMBERS		= $(LIBRARY)(dac_aad.o) \
		  $(LIBRARY)(dac_adr.o) \
		  $(LIBRARY)(dac_arre.o) \
		  $(LIBRARY)(dac_awre.o) \
		  $(LIBRARY)(dac_blksiz.o) \
		  $(LIBRARY)(dac_dmw.o) \
		  $(LIBRARY)(dac_drvmap.o) \
		  $(LIBRARY)(dac_drvsts.o) \
		  $(LIBRARY)(dac_lunsts.o) \
		  $(LIBRARY)(dac_nblks.o) \
		  $(LIBRARY)(dac_per.o) \
		  $(LIBRARY)(dac_raidlv.o) \
		  $(LIBRARY)(dac_rcamt.o) \
		  $(LIBRARY)(dac_rcfreq.o) \
		  $(LIBRARY)(dac_rretry.o) \
		  $(LIBRARY)(dac_secsiz.o) \
		  $(LIBRARY)(dac_segsiz.o) \
		  $(LIBRARY)(dac_sg0siz.o) \
		  $(LIBRARY)(dac_time.o) \
		  $(LIBRARY)(dac_wretry.o) \
		  $(LIBRARY)(dac_inqdat.o) \
		  $(LIBRARY)(dac_mdepge.o) \
		  $(LIBRARY)(dac_rcmde.o) \
		  $(LIBRARY)(dac_arcmde.o)

.PRECIOUS:	$(LIBRARY)

all:		$(LIBRARY)

#  .DEFAULT rule is used to retrieve .c & .h source files from SCCS directories
#.DEFAULT:
#	$(GET) $(@F) >/dev/null 2>&1

$(LIBRARY):	$(MEMBERS)

$(MEMBERS):	$(MINCL)

dependlist: $(C_SRCS) $(MEMBERS)
	-gcc -MM $(INCLUDES) -I- $(C_SRCS) 2>/dev/null \
	  | sed -e "s/^\(.*\) :/$$(LIBRARY)(\1) :/"

depend:
	$(DEPEND)

#__DEPENDLIST__
$(LIBRARY)(dac_aad.o) : dac_aad.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ../Common/Include/dsw_param.h \
  ../Common/Include/dac_param.h ../Common/Include/dialog.h \
  ../Common/Include/object.h ../Common/Include/scsi_dev.h \
  ../Sysdep/$(SYS)/Include/sysio.h ./Include/dac_aad.h ../Common/Include/menu.h \
  ../Common/Include/object_set.h ../Common/Include/dau_err.h 
$(LIBRARY)(dac_adr.o) : dac_adr.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ../Common/Include/dsw_param.h \
  ../Common/Include/dac_param.h ../Common/Include/dialog.h \
  ../Common/Include/object.h ../Common/Include/scsi_dev.h \
  ../Sysdep/$(SYS)/Include/sysio.h ./Include/dac_adr.h ../Common/Include/menu.h \
  ../Common/Include/object_set.h ../Common/Include/dau_err.h 
$(LIBRARY)(dac_arre.o) : dac_arre.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ../Common/Include/dsw_param.h \
  ../Common/Include/dac_param.h ../Common/Include/dialog.h \
  ../Common/Include/object.h ../Common/Include/scsi_dev.h \
  ../Sysdep/$(SYS)/Include/sysio.h ./Include/dac_arre.h ../Common/Include/menu.h \
  ../Common/Include/object_set.h ../Common/Include/dau_err.h 
$(LIBRARY)(dac_awre.o) : dac_awre.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ../Common/Include/dsw_param.h \
  ../Common/Include/dac_param.h ../Common/Include/dialog.h \
  ../Common/Include/object.h ../Common/Include/scsi_dev.h \
  ../Sysdep/$(SYS)/Include/sysio.h ./Include/dac_awre.h ../Common/Include/menu.h \
  ../Common/Include/object_set.h ../Common/Include/dau_err.h 
$(LIBRARY)(dac_blksiz.o) : dac_blksiz.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ../Common/Include/dac_param.h \
  ../Common/Include/dialog.h ../Common/Include/object.h \
  ../Common/Include/scsi_dev.h ../Sysdep/$(SYS)/Include/sysio.h \
  ./Include/dac_blksiz.h ../Common/Include/menu.h \
  ../Common/Include/object_set.h ../Common/Include/dau_err.h 
$(LIBRARY)(dac_dmw.o) : dac_dmw.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ../Common/Include/dsw_param.h \
  ../Common/Include/dac_param.h ../Common/Include/dialog.h \
  ../Common/Include/object.h ../Common/Include/scsi_dev.h \
  ../Sysdep/$(SYS)/Include/sysio.h ./Include/dac_dmw.h ../Common/Include/menu.h \
  ../Common/Include/object_set.h ../Common/Include/dau_err.h 
$(LIBRARY)(dac_drvmap.o) : dac_drvmap.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ../Common/Include/dac_param.h \
  ../Common/Include/dialog.h ../Common/Include/object.h \
  ../Common/Include/scsi_dev.h ../Sysdep/$(SYS)/Include/sysio.h \
  ./Include/dac_drvmap.h ../Common/Include/menu.h \
  ../Common/Include/object_set.h ../Common/Include/dau_err.h \
  ../Common/Include/zip_list.h ../Common/Include/list_dlg.h \
  ../Common/Include/zipcod_dlg.h 
$(LIBRARY)(dac_drvsts.o) : dac_drvsts.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ../Common/Include/dac_param.h \
  ../Common/Include/dialog.h ../Common/Include/object.h \
  ../Common/Include/scsi_dev.h ../Sysdep/$(SYS)/Include/sysio.h \
  ./Include/dac_drvsts.h ../Common/Include/menu.h \
  ../Common/Include/object_set.h ../Common/Include/dau_err.h 
$(LIBRARY)(dac_lunsts.o) : dac_lunsts.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ../Common/Include/dac_param.h \
  ../Common/Include/dialog.h ../Common/Include/object.h \
  ../Common/Include/scsi_dev.h ../Sysdep/$(SYS)/Include/sysio.h \
  ./Include/dac_lunsts.h ../Common/Include/menu.h \
  ../Common/Include/object_set.h ../Common/Include/dau_err.h 
$(LIBRARY)(dac_nblks.o) : dac_nblks.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ../Common/Include/dac_param.h \
  ../Common/Include/dialog.h ../Common/Include/object.h \
  ../Common/Include/scsi_dev.h ../Sysdep/$(SYS)/Include/sysio.h \
  ./Include/dac_nblks.h ../Common/Include/menu.h \
  ../Common/Include/object_set.h ../Common/Include/dau_err.h 
$(LIBRARY)(dac_per.o) : dac_per.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ../Common/Include/dsw_param.h \
  ../Common/Include/dac_param.h ../Common/Include/dialog.h \
  ../Common/Include/object.h ../Common/Include/scsi_dev.h \
  ../Sysdep/$(SYS)/Include/sysio.h ./Include/dac_per.h ../Common/Include/menu.h \
  ../Common/Include/object_set.h ../Common/Include/dau_err.h 
$(LIBRARY)(dac_raidlv.o) : dac_raidlv.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ../Common/Include/dac_param.h \
  ../Common/Include/dialog.h ../Common/Include/object.h \
  ../Common/Include/scsi_dev.h ../Sysdep/$(SYS)/Include/sysio.h \
  ./Include/dac_raidlv.h ../Common/Include/menu.h \
  ../Common/Include/object_set.h ../Common/Include/dau_err.h 
$(LIBRARY)(dac_rcamt.o) : dac_rcamt.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ../Common/Include/dac_param.h \
  ../Common/Include/dialog.h ../Common/Include/object.h \
  ../Common/Include/scsi_dev.h ../Sysdep/$(SYS)/Include/sysio.h \
  ./Include/dac_rcamt.h ../Common/Include/menu.h \
  ../Common/Include/object_set.h ../Common/Include/dau_err.h 
$(LIBRARY)(dac_rcfreq.o) : dac_rcfreq.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ../Common/Include/dac_param.h \
  ../Common/Include/dialog.h ../Common/Include/object.h \
  ../Common/Include/scsi_dev.h ../Sysdep/$(SYS)/Include/sysio.h \
  ./Include/dac_rcfreq.h ../Common/Include/menu.h \
  ../Common/Include/object_set.h ../Common/Include/dau_err.h 
$(LIBRARY)(dac_rretry.o) : dac_rretry.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ../Common/Include/dac_param.h \
  ../Common/Include/dialog.h ../Common/Include/object.h \
  ../Common/Include/scsi_dev.h ../Sysdep/$(SYS)/Include/sysio.h \
  ./Include/dac_rretry.h ../Common/Include/menu.h \
  ../Common/Include/object_set.h ../Common/Include/dau_err.h 
$(LIBRARY)(dac_secsiz.o) : dac_secsiz.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ../Common/Include/dac_param.h \
  ../Common/Include/dialog.h ../Common/Include/object.h \
  ../Common/Include/scsi_dev.h ../Sysdep/$(SYS)/Include/sysio.h \
  ./Include/dac_secsiz.h ../Common/Include/menu.h \
  ../Common/Include/object_set.h ../Common/Include/dau_err.h 
$(LIBRARY)(dac_segsiz.o) : dac_segsiz.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ../Common/Include/dac_param.h \
  ../Common/Include/dialog.h ../Common/Include/object.h \
  ../Common/Include/scsi_dev.h ../Sysdep/$(SYS)/Include/sysio.h \
  ./Include/dac_segsiz.h ../Common/Include/menu.h \
  ../Common/Include/object_set.h ../Common/Include/dau_err.h 
$(LIBRARY)(dac_sg0siz.o) : dac_sg0siz.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ../Common/Include/dac_param.h \
  ../Common/Include/dialog.h ../Common/Include/object.h \
  ../Common/Include/scsi_dev.h ../Sysdep/$(SYS)/Include/sysio.h \
  ./Include/dac_sg0siz.h ../Common/Include/menu.h \
  ../Common/Include/object_set.h ../Common/Include/dau_err.h 
$(LIBRARY)(dac_time.o) : dac_time.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ../Common/Include/dac_param.h \
  ../Common/Include/dialog.h ../Common/Include/object.h \
  ../Common/Include/scsi_dev.h ../Sysdep/$(SYS)/Include/sysio.h \
  ./Include/dac_time.h ../Common/Include/menu.h \
  ../Common/Include/object_set.h ../Common/Include/dau_err.h 
$(LIBRARY)(dac_wretry.o) : dac_wretry.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ../Common/Include/dac_param.h \
  ../Common/Include/dialog.h ../Common/Include/object.h \
  ../Common/Include/scsi_dev.h ../Sysdep/$(SYS)/Include/sysio.h \
  ./Include/dac_wretry.h ../Common/Include/menu.h \
  ../Common/Include/object_set.h ../Common/Include/dau_err.h 
$(LIBRARY)(dac_inqdat.o) : dac_inqdat.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ../Common/Include/dac_param.h \
  ../Common/Include/dialog.h ../Common/Include/object.h \
  ../Common/Include/scsi_dev.h ../Sysdep/$(SYS)/Include/sysio.h \
  ./Include/dac_inqdat.h ../Common/Include/menu.h \
  ../Common/Include/object_set.h ../Common/Include/dau_err.h 
$(LIBRARY)(dac_adr.o) : dac_adr.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ../Common/Include/dsw_param.h \
  ../Common/Include/dac_param.h ../Common/Include/dialog.h \
  ../Common/Include/object.h ../Common/Include/scsi_dev.h \
  ../Sysdep/$(SYS)/Include/sysio.h ./Include/dac_adr.h ../Common/Include/menu.h \
  ../Common/Include/object_set.h ../Common/Include/dau_err.h 
$(LIBRARY)(dac_rcmde.o) : dac_rcmde.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ../Common/Include/dac_param.h \
  ../Common/Include/dialog.h ../Common/Include/object.h \
  ../Common/Include/scsi_dev.h ../Sysdep/$(SYS)/Include/sysio.h \
  ./Include/dac_rcmde.h ../Common/Include/menu.h \
  ../Common/Include/object_set.h ../Common/Include/dau_err.h 
$(LIBRARY)(dac_arcmde.o) : dac_arcmde.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ../Common/Include/dac_param.h \
  ../Common/Include/dialog.h ../Common/Include/object.h \
  ../Common/Include/scsi_dev.h ../Sysdep/$(SYS)/Include/sysio.h \
  ./Include/dac_arcmde.h ../Common/Include/menu.h \
  ../Common/Include/object_set.h ../Common/Include/dau_err.h 
