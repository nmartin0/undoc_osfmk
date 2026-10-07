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
TOPDIR		= ../../..
# FROMTOP must be defined as the relative path FROM the top level source
# directory TO *this* subdir.  NOTE: Both FROMTOP and TOPDIR are redefined as
# make recurses the source directory hierarchy.  FROMTOP must always begin
# with a '.' in all make files.
FROMTOP		= ./Acf/Actions/Download


MINCL		= ../../../Make.incl
MRULES		= ../../../Make.rules

include $(MINCL)
include $(MRULES)

#AR		= /bin/ar

INCDR1		= ./Include
INCDR2		= ../../../Common/Include
INCDR3		= ../../../Sysdep/$(SYS)/Include
INCLUDES	= -I$(INCDR1) -I$(INCDR2) -I$(INCDR3)
CFLAGS		= $(SWITCHES) $(DEFS) $(INCLUDES)

LIB		= ../../../../lib/$(SYS)
LIBRARY		= $(LIB)/Acf.a
THISMK		= ./Download
C_SRCS		= dlerror.c dlfile.c dlmain.c dlxfer.c

MEMBERS		= $(LIBRARY)(dlmain.o) \
		  $(LIBRARY)(dlfile.o) \
		  $(LIBRARY)(dlxfer.o) \
		  $(LIBRARY)(dlerror.o)

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
$(LIBRARY)(dlerror.o) : dlerror.c ./Include/dlinclds.h ./Include/dlerror.h \
  ./Include/ccdrv.h ./Include/ccioctl.h ./Include/ccerrors.h \
  ../../../Common/Include/stddefs.h ../../../Sysdep/$(SYS)/Include/sysdep.h \
  ../../../Common/Include/scsidefs.h ../../../Common/Include/cdb_struct.h \
  ../../../Common/Include/scsi_dev.h ../../../Common/Include/object.h \
  ../../../Sysdep/$(SYS)/Include/sysio.h 
$(LIBRARY)(dlfile.o) : dlfile.c ./Include/dlinclds.h ./Include/dlerror.h \
  ./Include/ccdrv.h ./Include/ccioctl.h ./Include/ccerrors.h \
  ../../../Common/Include/stddefs.h ../../../Sysdep/$(SYS)/Include/sysdep.h \
  ../../../Common/Include/scsidefs.h ../../../Common/Include/cdb_struct.h \
  ../../../Common/Include/scsi_dev.h ../../../Common/Include/object.h \
  ../../../Sysdep/$(SYS)/Include/sysio.h 
$(LIBRARY)(dlmain.o) : dlmain.c ./Include/dlinclds.h ./Include/dlerror.h \
  ./Include/ccdrv.h ./Include/ccioctl.h ./Include/ccerrors.h \
  ../../../Common/Include/stddefs.h ../../../Sysdep/$(SYS)/Include/sysdep.h \
  ../../../Common/Include/scsidefs.h ../../../Common/Include/cdb_struct.h \
  ../../../Common/Include/scsi_dev.h ../../../Common/Include/object.h \
  ../../../Sysdep/$(SYS)/Include/sysio.h 
$(LIBRARY)(dlxfer.o) : dlxfer.c ./Include/dlinclds.h ./Include/dlerror.h \
  ./Include/ccdrv.h ./Include/ccioctl.h ./Include/ccerrors.h \
  ../../../Common/Include/stddefs.h ../../../Sysdep/$(SYS)/Include/sysdep.h \
  ../../../Common/Include/scsidefs.h ../../../Common/Include/cdb_struct.h \
  ../../../Common/Include/scsi_dev.h ../../../Common/Include/object.h \
  ../../../Sysdep/$(SYS)/Include/sysio.h 
