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
FROMTOP		= ./Dacevents


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
LIBRARY		= $(LIB)/Dacevents.a
THISMK		= ./Dacevents
C_SRCS		= dac_abtcmd.c  dac_datpro.c  dac_hrderr.c  dac_illreq.c \
		  dac_mederr.c  dac_miscmp.c  dac_nosens.c  dac_notrdy.c \
		  dac_rcverr.c  dac_untatn.c

MEMBERS		= $(LIBRARY)(dac_abtcmd.o) \
		  $(LIBRARY)(dac_datpro.o) \
		  $(LIBRARY)(dac_hrderr.o) \
		  $(LIBRARY)(dac_illreq.o) \
		  $(LIBRARY)(dac_mederr.o) \
		  $(LIBRARY)(dac_miscmp.o) \
		  $(LIBRARY)(dac_nosens.o) \
		  $(LIBRARY)(dac_notrdy.o) \
		  $(LIBRARY)(dac_rcverr.o) \
		  $(LIBRARY)(dac_untatn.o)

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
$(LIBRARY)(dac_abtcmd.o) : dac_abtcmd.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ./Include/dac_abtcmd.h \
  ../Common/Include/dac_event.h ../Common/Include/object.h 
$(LIBRARY)(dac_datpro.o) : dac_datpro.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ./Include/dac_datpro.h \
  ../Common/Include/dac_event.h ../Common/Include/object.h 
$(LIBRARY)(dac_hrderr.o) : dac_hrderr.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ./Include/dac_hrderr.h \
  ../Common/Include/dac_event.h ../Common/Include/object.h 
$(LIBRARY)(dac_illreq.o) : dac_illreq.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ./Include/dac_illreq.h \
  ../Common/Include/dac_event.h ../Common/Include/object.h 
$(LIBRARY)(dac_mederr.o) : dac_mederr.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ./Include/dac_mederr.h \
  ../Common/Include/dac_event.h ../Common/Include/object.h 
$(LIBRARY)(dac_miscmp.o) : dac_miscmp.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ./Include/dac_miscmp.h \
  ../Common/Include/dac_event.h ../Common/Include/object.h 
$(LIBRARY)(dac_nosens.o) : dac_nosens.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ./Include/dac_nosens.h \
  ../Common/Include/dac_event.h ../Common/Include/object.h 
$(LIBRARY)(dac_notrdy.o) : dac_notrdy.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ./Include/dac_notrdy.h \
  ../Common/Include/dac_event.h ../Common/Include/object.h 
$(LIBRARY)(dac_rcverr.o) : dac_rcverr.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ./Include/dac_rcverr.h \
  ../Common/Include/dac_event.h ../Common/Include/object.h 
$(LIBRARY)(dac_untatn.o) : dac_untatn.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ./Include/dac_untatn.h \
  ../Common/Include/dac_event.h ../Common/Include/object.h 
