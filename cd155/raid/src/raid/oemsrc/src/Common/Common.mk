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
FROMTOP		= ./Common


MINCL		= ../Make.incl
MRULES		= ../Make.rules

include $(MINCL)
include $(MRULES)

#AR		= /bin/ar

INCDR1		= ./Include
INCDR2		= ../Dacevents/Include
INCDR3		= ../Dacparams/Include
INCDR4		= ../Sysdep/$(SYS)/Include
INCLUDES	= -I$(INCDR1) -I$(INCDR2) -I$(INCDR3) -I$(INCDR4)
CFLAGS		= $(SWITCHES) $(DEFS) $(INCLUDES)

LIB		= ../../lib/$(SYS)
LIBRARY		= $(LIB)/Common.a
THISMK		= ./Common
C_SRCS		= action.c  common.c  dac_event.c  dac_param.c \
		  dialog.c  dsw_param.c  fnm_dialog.c  list_dlg.c \
		  main_menu.c  menu.c  mk_dacev.c  object.c \
		  object_set.c  scsi_dev.c  scsi_dk.c  sink.c \
		  stat_tbl.c  zip_dialog.c  zip_list.c  zipcod_dlg.c error.c \
		  cvzip.c

MEMBERS		= $(LIBRARY)(action.o) \
		  $(LIBRARY)(common.o) \
		  $(LIBRARY)(dac_event.o) \
		  $(LIBRARY)(dac_param.o) \
		  $(LIBRARY)(dialog.o) \
		  $(LIBRARY)(dsw_param.o) \
		  $(LIBRARY)(fnm_dialog.o) \
		  $(LIBRARY)(list_dlg.o) \
		  $(LIBRARY)(main_menu.o) \
		  $(LIBRARY)(menu.o) \
		  $(LIBRARY)(mk_dacev.o) \
		  $(LIBRARY)(object.o) \
		  $(LIBRARY)(object_set.o) \
		  $(LIBRARY)(scsi_dev.o) \
		  $(LIBRARY)(scsi_dk.o) \
		  $(LIBRARY)(sink.o) \
		  $(LIBRARY)(stat_tbl.o) \
		  $(LIBRARY)(zip_dialog.o) \
		  $(LIBRARY)(zip_list.o) \
		  $(LIBRARY)(zipcod_dlg.o) \
		  $(LIBRARY)(error.o) \
		  $(LIBRARY)(cvzip.o)

.PRECIOUS:	$(LIBRARY)

all:		$(LIBRARY)

#  .DEFAULT rule is used to retrieve .c & .h source files from SCCS directories
#.DEFAULT:
#	$(GET) $(@F) >/dev/null 2>&1

$(LIBRARY):	$(MEMBERS)

$(MEMBERS):	$(MINCL)

#  Here's a special dependency for stat_tbl.c which gets generated from
#  senscode.lst using senscode.sh and senscode.awk
stat_tbl.c:	senscode.lst senscode.awk senscode.sh
	chmod +x senscode.sh
	-$(SCCSCMD) edit stat_tbl.c
	copy -movr stat_tbl.c stat_tbl.cO
	cat stat_tbl.c | sed -n -e "1,/^\/\*__STARTMAGIC__\*\//p" > st.1
	cat stat_tbl.c | sed -n -e "/^\/\*__ENDMAGIC__\*\//,\$$p" > st.2
	/bin/sh ./senscode.sh -doit
	mv stat_tbl.c stat_tbl.c~
	cat st.1 senscode._c st.2 >> stat_tbl.c
	rm st.1 st.2 senscode._c
	-diff stat_tbl.cO stat_tbl.c >/dev/null 2>&1			; \
	if [ $$? -eq 0 ]; then						  \
	  $(SCCSCMD) unedit stat_tbl.c					; \
	else								  \
	  echo "Automatic delta due to changes in senscode.lst file"	  \
	    | $(SCCSCMD) delget stat_tbl.c				; \
	fi

dependlist: $(C_SRCS) $(MEMBERS)
	-gcc -MM $(INCLUDES) -I- $(C_SRCS) 2>/dev/null \
	  | sed -e "s/^\(.*\) :/$$(LIBRARY)(\1) :/"

depend:
	$(DEPEND)

.c.o:
	$(CC) $(CFLAGS)  -c $*.c

.c.asm:
	$(CC) -S $(CFLAGS)  -c $*.c

#__DEPENDLIST__
$(LIBRARY)(action.o) : action.c ./Include/stddefs.h ../Sysdep/$(SYS)/Include/sysdep.h \
  ./Include/scsidefs.h ./Include/cdb_struct.h ./Include/object.h \
  ./Include/action.h ./Include/menu.h ./Include/dialog.h \
  ./Include/object_set.h 
$(LIBRARY)(common.o) : common.c ./Include/stddefs.h ../Sysdep/$(SYS)/Include/sysdep.h \
  ./Include/scsidefs.h ./Include/cdb_struct.h ./Include/dau_err.h \
  ../Dacparams/Include/dac_raidlv.h ./Include/dac_param.h ./Include/dialog.h \
  ./Include/object.h ./Include/scsi_dev.h ../Sysdep/$(SYS)/Include/sysio.h \
  ./Include/menu.h ./Include/object_set.h 
$(LIBRARY)(dac_event.o) : dac_event.c ./Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ./Include/scsidefs.h ./Include/cdb_struct.h \
  ./Include/dac_event.h ./Include/object.h 
$(LIBRARY)(dac_param.o) : dac_param.c ./Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ./Include/scsidefs.h ./Include/cdb_struct.h \
  ./Include/dac_param.h ./Include/dialog.h ./Include/object.h \
  ./Include/scsi_dev.h ../Sysdep/$(SYS)/Include/sysio.h ./Include/dau_err.h 
$(LIBRARY)(dialog.o) : dialog.c ./Include/dialog.h ./Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ./Include/scsidefs.h ./Include/cdb_struct.h \
  ./Include/object.h ./Include/dau_err.h 
$(LIBRARY)(dsw_param.o) : dsw_param.c ./Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ./Include/scsidefs.h ./Include/cdb_struct.h \
  ./Include/dac_param.h ./Include/dialog.h ./Include/object.h \
  ./Include/scsi_dev.h ../Sysdep/$(SYS)/Include/sysio.h ./Include/dsw_param.h 
$(LIBRARY)(fnm_dialog.o) : fnm_dialog.c ./Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ./Include/scsidefs.h ./Include/cdb_struct.h \
  ./Include/fnm_dialog.h ./Include/dialog.h ./Include/object.h \
  ./Include/dau_err.h 
$(LIBRARY)(list_dlg.o) : list_dlg.c ./Include/list_dlg.h ./Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ./Include/scsidefs.h ./Include/cdb_struct.h \
  ./Include/dialog.h ./Include/object.h ./Include/dau_err.h 
$(LIBRARY)(main_menu.o) : main_menu.c ./Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ./Include/scsidefs.h ./Include/cdb_struct.h \
  ./Include/menu.h ./Include/dialog.h ./Include/object.h \
  ./Include/object_set.h ./Include/main_menu.h 
$(LIBRARY)(menu.o) : menu.c ./Include/stddefs.h ../Sysdep/$(SYS)/Include/sysdep.h \
  ./Include/scsidefs.h ./Include/cdb_struct.h ./Include/object.h \
  ./Include/menu.h ./Include/dialog.h ./Include/object_set.h \
  ./Include/action.h ./Include/dau_err.h 
$(LIBRARY)(mk_dacev.o) : mk_dacev.c ./Include/stddefs.h ../Sysdep/$(SYS)/Include/sysdep.h \
  ./Include/scsidefs.h ./Include/cdb_struct.h ./Include/scsi_dev.h \
  ./Include/object.h ../Sysdep/$(SYS)/Include/sysio.h \
  ../Dacevents/Include/dac_abtcmd.h ./Include/dac_event.h \
  ../Dacevents/Include/dac_datpro.h ../Dacevents/Include/dac_hrderr.h \
  ../Dacevents/Include/dac_illreq.h ../Dacevents/Include/dac_mederr.h \
  ../Dacevents/Include/dac_miscmp.h ../Dacevents/Include/dac_nosens.h \
  ../Dacevents/Include/dac_notrdy.h ../Dacevents/Include/dac_rcverr.h \
  ../Dacevents/Include/dac_untatn.h 
$(LIBRARY)(object.o) : object.c ./Include/stddefs.h ../Sysdep/$(SYS)/Include/sysdep.h \
  ./Include/scsidefs.h ./Include/cdb_struct.h ./Include/object.h 
$(LIBRARY)(object_set.o) : object_set.c ./Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ./Include/scsidefs.h ./Include/cdb_struct.h \
  ./Include/object.h ./Include/object_set.h 
$(LIBRARY)(scsi_dev.o) : scsi_dev.c ./Include/scsi_dev.h ./Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ./Include/scsidefs.h ./Include/cdb_struct.h \
  ./Include/object.h ../Sysdep/$(SYS)/Include/sysio.h 
$(LIBRARY)(scsi_dk.o) : scsi_dk.c ./Include/scsi_dk.h ./Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ./Include/scsidefs.h ./Include/cdb_struct.h \
  ./Include/scsi_dev.h ./Include/object.h ../Sysdep/$(SYS)/Include/sysio.h 
$(LIBRARY)(sink.o) : sink.c ./Include/sink.h ./Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ./Include/scsidefs.h ./Include/cdb_struct.h \
  ./Include/object.h 
$(LIBRARY)(stat_tbl.o) : stat_tbl.c ./Include/stddefs.h ../Sysdep/$(SYS)/Include/sysdep.h \
  ./Include/scsidefs.h ./Include/cdb_struct.h ./Include/dac_event.h \
  ./Include/object.h 
$(LIBRARY)(zip_dialog.o) : zip_dialog.c ./Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ./Include/scsidefs.h ./Include/cdb_struct.h \
  ./Include/zipcod_dlg.h ./Include/dialog.h ./Include/object.h \
  ./Include/zip_dialog.h 
$(LIBRARY)(zip_list.o) : zip_list.c ./Include/zip_list.h ./Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ./Include/scsidefs.h ./Include/cdb_struct.h \
  ./Include/list_dlg.h ./Include/dialog.h ./Include/object.h \
  ./Include/zipcod_dlg.h 
$(LIBRARY)(zipcod_dlg.o) : zipcod_dlg.c ./Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ./Include/scsidefs.h ./Include/cdb_struct.h \
  ./Include/zipcod_dlg.h ./Include/dialog.h ./Include/object.h \
  ./Include/dau_err.h 
$(LIBRARY)(error.o) : error.c ./Include/stddefs.h ../Sysdep/$(SYS)/Include/sysdep.h \
  ./Include/scsidefs.h ./Include/cdb_struct.h ./Include/dau_err.h 
$(LIBRARY)(cvzip.o) : cvzip.c ./Include/stddefs.h ../Sysdep/$(SYS)/Include/sysdep.h \
  ./Include/scsidefs.h ./Include/cdb_struct.h 
