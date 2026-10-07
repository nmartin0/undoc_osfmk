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
FROMTOP		= ./Special/Can


MINCL		= $(TOPDIR)/Make.incl
MRULES		= $(TOPDIR)/Make.rules

include $(MINCL)
include $(MRULES)

#AR		= /bin/ar

INCDR1		= $(TOPDIR)/Common/Include
INCDR2		= $(TOPDIR)/Sysdep/$(SYS)/Include
INCLUDES	= -I$(INCDR1) -I$(INCDR2)
CFLAGS		= $(SWITCHES) $(DEFS) $(INCLUDES)

LIB		= ../../../lib/$(SYS)
CLIB		= $(LIB)/Common.a
DLIB		= $(LIB)/Dacevents.a
PLIB		= $(LIB)/Dacparams.a
ULIB		= $(LIB)/Uv4.a
SLIB		= $(LIB)/$(SYS).a
LIBS		= $(SLIB) $(DLIB) $(PLIB) $(ULIB) $(CLIB)
THISMK		= ./Can
C_SRCS		= can.c usage.c
OBJS		= can.o usage.o

all:	can	

#  .DEFAULT rule is used to retrieve .c & .h source files from SCCS directories
#.DEFAULT:
#	$(GET) $(@F) >/dev/null 2>&1

can:	$(LIBS) $(OBJS)
	$(CC) $(CFLAGS) -o can $(OBJS) $(LIBS) -lsys5 -L../../../lib/Uv4 -lio
	-$(MAKE) -f $(THISMK).mk clean

$(OBJS): $(MINCL)

dependlist: $(C_SRCS) $(OBJS)
	-gcc -MM $(INCLUDES) -I- $(C_SRCS) 2>/dev/null

depend:
	$(DEPEND)

realclean: clean
	-rm -f $(OBJS) apr

clean:
	if [ ! -d dotOsave ]; then mkdir dotOsave; fi
	-mv $(OBJS) dotOsave/
	-rm -f *.o
	-(cd dotOsave; mv $(OBJS) ..)
	-rmdir dotOsave

#__DEPENDLIST__
can.o : can.c ../../Common/Include/stddefs.h \
  ../../Sysdep/Uv4/Include/sysdep.h ../../Common/Include/scsidefs.h \
  ../../Common/Include/cdb_struct.h ../../Common/Include/dau_err.h \
  ../../Sysdep/Uv4/Include/getrel.h
usage.o : usage.c ../../Common/Include/stddefs.h \
  ../../Sysdep/Uv4/Include/sysdep.h ../../Common/Include/scsidefs.h \
  ../../Common/Include/cdb_struct.h 
