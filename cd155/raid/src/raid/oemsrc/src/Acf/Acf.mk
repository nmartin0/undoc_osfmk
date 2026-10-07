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
FROMTOP		= ./Acf


MINCL		= ../Make.incl
MRULES		= ../Make.rules

include $(MINCL)
include $(MRULES)

#AR		= /bin/ar

INCDR1		= ./Actions/Include
INCDR2		= ./Menus/Include
INCDR3		= ../Common/Include
INCDR4		= ../Dacparams/Include
INCDR5		= ../Sysdep/$(SYS)/Include
INCLUDES	= -I$(INCDR1) -I$(INCDR2) -I$(INCDR3) -I$(INCDR4) -I$(INCDR5)
CFLAGS		= $(SWITCHES) $(DEFS) $(INCLUDES)

LIB		= ../../lib/$(SYS)
ALIB		= $(LIB)/Acf.a
CLIB		= $(LIB)/Common.a
DLIB		= $(LIB)/Dacparams.a
ELIB		= $(LIB)/Dacevents.a
SLIB		= $(LIB)/$(SYS).a
LIBS		= $(ALIB) $(DLIB) $(ELIB) $(CLIB) $(SLIB)
THISMK		= ./Acf
C_SRCS		= acf.c usage.c
OBJS		= acf.o usage.o

all:	Menus Actions acf Ccfiles

#  .DEFAULT rule is used to retrieve .c & .h source files from SCCS directories
#.DEFAULT:
#	$(GET) $(@F) >/dev/null 2>&1

Menus::
	cd $@; $(MAKE) -ef $@.mk

Actions::
	cd $@; $(MAKE) -ef $@.mk

Ccfiles::
	cd $@; $(MAKE) -ef $@.mk

acf:	$(LIBS) $(OBJS)
	$(CC) $(CFLAGS) -o $(ACF) *.o $(LIBS) -lsys5 $(LIBRARIES)
	$(MAKE) -f $(THISMK).mk clean

$(OBJS): $(MINCL)

dependlist: $(C_SRCS) $(OBJS)
	-gcc -MM $(INCLUDES) -I- $(C_SRCS) 2>/dev/null

depend:
	$(DEPEND)
	cd Actions; $(MAKE) -f Actions.mk depend
	cd Menus; $(MAKE) -f Menus.mk depend

clean:
	if [ ! -d dotOsave ]; then mkdir dotOsave; fi
	-mv $(OBJS) dotOsave/
	-rm -f *.o
	-(cd dotOsave; mv $(OBJS) ..)
	-rmdir dotOsave

#__DEPENDLIST__
acf.o : acf.c ../Common/Include/stddefs.h ../Sysdep/$(SYS)/Include/sysdep.h \
  ../Common/Include/scsidefs.h ../Common/Include/cdb_struct.h \
  ../Common/Include/main_menu.h ../Common/Include/menu.h \
  ../Common/Include/dialog.h ../Common/Include/object.h \
  ../Common/Include/object_set.h ./Actions/Include/ucf_action.h \
  ./Actions/Include/cf_action.h ../Common/Include/action.h \
  ../Common/Include/dac_param.h ../Common/Include/scsi_dev.h \
  ../Sysdep/$(SYS)/Include/sysio.h ./Actions/Include/ecf_action.h \
  ./Actions/Include/dcf_action.h ./Actions/Include/dld_action.h \
  ./Actions/Include/fmt_action.h ../Common/Include/dau_err.h \
  ./Actions/Include/inq_action.h ./Actions/Include/ccf_action.h \
  ../Common/Include/zip_dialog.h ../Common/Include/zipcod_dlg.h 
usage.o : usage.c ../Common/Include/stddefs.h \
  ../Sysdep/$(SYS)/Include/sysdep.h ../Common/Include/scsidefs.h \
  ../Common/Include/cdb_struct.h 
