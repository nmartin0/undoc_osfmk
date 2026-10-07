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
FROMTOP		= ./Apc


MINCL		= ../Make.incl
MRULES		= ../Make.rules

include $(MINCL)
include $(MRULES)

#AR		= /bin/ar

INCDR1		= ../Common/Include
INCDR2		= ../Dacevents/Include
INCDR3		= ../Sysdep/$(SYS)/Include
INCLUDES	= -I$(INCDR1) -I$(INCDR2) -I$(INCDR3)
CFLAGS		= $(SWITCHES) $(DEFS) $(INCLUDES)

LIB		= ../../lib/$(SYS)
CLIB		= $(LIB)/Common.a
SLIB		= $(LIB)/$(SYS).a
DLIB		= $(LIB)/Dacevents.a
PLIB		= $(LIB)/Dacparams.a
LIBS		= $(CLIB) $(SLIB) $(DLIB) $(PLIB) $(CLIB)
LIBIO		= /usr/lib/libio.a
THISMK		= ./Apc
C_SRCS		= apc.c usage.c
OBJS		= apc.o usage.o

all:	apc	

#  .DEFAULT rule is used to retrieve .c & .h source files from SCCS directories
#.DEFAULT:
#	$(GET) $(@F) >/dev/null 2>&1

apc:	$(LIBS) $(OBJS)
	$(CC) $(CFLAGS) -o apc $(OBJS) $(LIBS) -lsys5 -L../../lib/Uv4 -lio
	-$(MAKE) -f $(THISMK).mk clean

$(OBJS): $(MINCL)

dependlist: $(C_SRCS) $(OBJS)
	-gcc -MM $(INCLUDES) -I- $(C_SRCS) 2>/dev/null

depend:
	$(DEPEND)

realclean: clean
	-rm -f $(OBJS) apc

clean:
	if [ ! -d dotOsave ]; then mkdir dotOsave; fi
	-mv $(OBJS) dotOsave/
	-rm -f *.o
	-(cd dotOsave; mv $(OBJS) ..)
	-rmdir dotOsave

#__DEPENDLIST__
apc.o : apc.c ../Common/Include/stddefs.h ../Sysdep/Uv4/Include/sysdep.h \
  ../Common/Include/scsidefs.h ../Common/Include/cdb_struct.h \
  ../Common/Include/scsi_dk.h ../Common/Include/scsi_dev.h \
  ../Common/Include/object.h ../Sysdep/Uv4/Include/sysio.h \
  ../Common/Include/zip_dialog.h ../Common/Include/dialog.h \
  ../Common/Include/zipcod_dlg.h ../Common/Include/fnm_dialog.h \
  ../Common/Include/dac_event.h ../Common/Include/dau_err.h 
usage.o : usage.c ../Common/Include/stddefs.h \
  ../Sysdep/Uv4/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h 
