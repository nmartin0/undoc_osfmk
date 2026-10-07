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
FROMTOP		= ./Amd


MINCL		= ../Make.incl
MRULES		= ../Make.rules

include $(MINCL)
include $(MRULES)

#AR		= /bin/ar

INCDR1		= ./Include
INCDR2		= ../Common/Include
INCDR3		= ../Dacparams/Include
INCDR4		= ../Sysdep/$(SYS)/Include
INCLUDES	= -I$(INCDR1) -I$(INCDR2) -I$(INCDR3) -I$(INCDR4)
CFLAGS		= $(SWITCHES) $(DEFS) $(INCLUDES)

LIB		= ../../lib/$(SYS)
CLIB		= $(LIB)/Common.a
DLIB		= $(LIB)/Dacparams.a
ELIB		= $(LIB)/Dacevents.a
SLIB		= $(LIB)/$(SYS).a
LIBS		= $(DLIB) $(CLIB) $(ELIB) $(SLIB) $(DLIB)
THISMK		= ./Amd
C_SRCS		= amd.c console.c usage.c # can.c zipdlog.c
OBJS		= amd.o console.o usage.o # can.o zipdlog.o

all:	amd	

#  .DEFAULT rule is used to retrieve .c & .h source files from SCCS directories
#.DEFAULT:
#	$(GET) $(@F) >/dev/null 2>&1

amd:	$(LIBS) $(OBJS)
	$(CC) $(CFLAGS) -o amd $(OBJS) $(LIBS) -lnx -L../../lib/Uv4 -lio 
	$(MAKE) -f $(THISMK).mk clean

$(OBJS): $(MINCL)

dependlist: $(C_SRCS) $(OBJS)
	-gcc -MM $(INCLUDES) -I- $(C_SRCS) 2>/dev/null

depend:
	$(DEPEND)

clean:
	if [ ! -d dotOsave ]; then mkdir dotOsave; fi
	-mv $(OBJS) dotOsave/
	-rm -f *.o
	-(cd dotOsave; mv $(OBJS) ..)
	-rmdir dotOsave

#__DEPENDLIST__
amd.o : amd.c ../Common/Include/stddefs.h ../Sysdep/Uv4/Include/sysdep.h \
  ../Common/Include/scsidefs.h ../Common/Include/cdb_struct.h \
  ./Include/console.h ../Common/Include/sink.h ../Common/Include/object.h \
  ../Common/Include/object_set.h ../Dacparams/Include/dac_time.h \
  ../Common/Include/dac_param.h ../Common/Include/dialog.h \
  ../Common/Include/scsi_dev.h ../Sysdep/Uv4/Include/sysio.h \
  ../Common/Include/menu.h ../Common/Include/dac_event.h \
  ../Common/Include/zip_dialog.h ../Common/Include/zipcod_dlg.h \
  ../Common/Include/dau_err.h 
console.o : console.c ./Include/console.h ../Common/Include/stddefs.h \
  ../Sysdep/Uv4/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h ../Common/Include/sink.h \
  ../Common/Include/object.h ../Common/Include/object_set.h \
  ../Common/Include/dac_event.h 
usage.o : usage.c ../Common/Include/stddefs.h \
  ../Sysdep/Uv4/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h 
