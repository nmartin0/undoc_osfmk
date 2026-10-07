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
FROMTOP		= ./Arc/Dacparams


MINCL		= ../../Make.incl
MRULES		= ../../Make.rules

include $(MINCL)
include $(MRULES)

#AR		= /bin/ar

INCDR1		= ./Include
INCDR2		= ../../Common/Include
INCDR3		= ../../Dacparams/Include
INCDR4		= ../../Sysdep/$(SYS)/Include
INCLUDES	= -I$(INCDR1) -I$(INCDR2) -I$(INCDR3) -I$(INCDR4)
CFLAGS		= $(SWITCHES) $(DEFS) $(INCLUDES)

LIB		= ../../../lib/$(SYS)
LIBRARY		= $(LIB)/Arc.a
THISMK		= ./Dacparams
C_SRCS		= dac_rcprog.c dac_rcstrt.c

MEMBERS		= $(LIBRARY)(dac_rcstrt.o) \
		  $(LIBRARY)(dac_rcprog.o)

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

.c.o:
	$(CC) $(CFLAGS)  -c $*.c

.c.asm:
	$(CC) -S $(CFLAGS)  -c $*.c

#__DEPENDLIST__
$(LIBRARY)(dac_rcprog.o) : dac_rcprog.c ../../Common/Include/stddefs.h \
  ../../Sysdep/Uv4/Include/sysdep.h ../../Common/Include/scsidefs.h \
  ../../Common/Include/cdb_struct.h ../../Common/Include/dac_param.h \
  ../../Common/Include/dialog.h ../../Common/Include/object.h \
  ../../Common/Include/scsi_dev.h ../../Sysdep/Uv4/Include/sysio.h \
  ./Include/dac_rcprog.h ../../Common/Include/scsi_dk.h \
  ../../Common/Include/menu.h ../../Common/Include/object_set.h \
  ../../Common/Include/dau_err.h 
$(LIBRARY)(dac_rcstrt.o) : dac_rcstrt.c ../../Common/Include/stddefs.h \
  ../../Sysdep/Uv4/Include/sysdep.h ../../Common/Include/scsidefs.h \
  ../../Common/Include/cdb_struct.h ../../Common/Include/dac_param.h \
  ../../Common/Include/dialog.h ../../Common/Include/object.h \
  ../../Common/Include/scsi_dev.h ../../Sysdep/Uv4/Include/sysio.h \
  ../../Dacparams/Include/dac_drvsts.h ../../Common/Include/menu.h \
  ../../Common/Include/object_set.h ./Include/dac_rcstrt.h 
