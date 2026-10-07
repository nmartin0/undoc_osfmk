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
FROMTOP		= ./Sysdep/$(SYS)


MINCL		= ../../Make.incl
MRULES		= ../../Make.rules

include $(MINCL)
include $(MRULES)

#AR		= /bin/ar

INCDR1		= ./Include
INCDR2		= ../../Common/Include
INCLUDES	= -I$(INCDR1) -I$(INCDR2)
CFLAGS		= $(SWITCHES) $(DEFS) $(INCLUDES)

LIB		= ../../../lib/Uv4
LIBRARY		= $(LIB)/Uv4.a
DATAFILES	= data.hex  inquiry.doc  io_cmds.doc  logarray.doc \
		  phyarray.doc  read.doc  readcap.doc  recovery.doc sense.doc
THISMK		= ./Uv4
C_SRCS		= io_test.c sysio.c hc_ptol.c runlev.c diagnosis.c devopen.c \
		  getrel.c

MEMBERS		= $(LIBRARY)(io_test.o) \
		  $(LIBRARY)(hc_ptol.o) \
		  $(LIBRARY)(runlev.o) \
		  $(LIBRARY)(diagnosis.o) \
		  $(LIBRARY)(devopen.o) \
		  $(LIBRARY)(sysio.o) \
		  $(LIBRARY)(getrel.o)

.PRECIOUS:	$(LIBRARY)

all:	$(DATAFILES) $(LIBRARY)
	-rm /tmp/test
	ln -s `pwd` /tmp/test

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
$(LIBRARY)(io_test.o) : io_test.c ./Include/io_test.h ../../Common/Include/cdb_struct.h \
  ../../Common/Include/scsi_dk.h ../../Common/Include/stddefs.h \
  ./Include/sysdep.h ../../Common/Include/scsidefs.h \
  ../../Common/Include/scsi_dev.h ../../Common/Include/object.h \
  ./Include/sysio.h 
$(LIBRARY)(sysio.o) : sysio.c ./Include/sysio.h ../../Common/Include/stddefs.h \
  ./Include/sysdep.h ../../Common/Include/scsidefs.h \
  ../../Common/Include/cdb_struct.h ../../Common/Include/object.h 
$(LIBRARY)(hc_ptol.o) : hc_ptol.c ../../Common/Include/stddefs.h ./Include/sysdep.h \
  ../../Common/Include/scsidefs.h ../../Common/Include/cdb_struct.h 
$(LIBRARY)(getrel.o) : getrel.c ./Include/sysio.h \
  ../../Common/Include/dau_err.h ../../Common/Include/stddefs.h
$(LIBRARY)(runlev.o) : runlev.c 
$(LIBRARY)(diagnosis.o) : diagnosis.c 
$(LIBRARY)(devopen.o) : devopen.c 
