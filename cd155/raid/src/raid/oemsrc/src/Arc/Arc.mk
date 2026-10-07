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
FROMTOP		= ./Arc


MINCL		= ../Make.incl
MRULES		= ../Make.rules

include $(MINCL)
include $(MRULES)

#AR		= /bin/ar

INCDR1		= ./Dacparams/Include
INCDR2		= ../Common/Include
INCDR3		= ../Dacparams/Include
INCDR4		= ../Sysdep/$(SYS)/Include
INCLUDES	= -I$(INCDR1) -I$(INCDR2) -I$(INCDR3) -I$(INCDR4)
CFLAGS		= $(SWITCHES) $(DEFS) $(INCLUDES)

LIB		= ../../lib/$(SYS)
ALIB		= $(LIB)/Arc.a
CLIB		= $(LIB)/Common.a
DLIB		= $(LIB)/Dacparams.a
ELIB		= $(LIB)/Dacevents.a
SLIB		= $(LIB)/$(SYS).a
LIBS		= $(ALIB) $(CLIB) $(DLIB) $(ELIB) $(SLIB)
THISMK		= ./Arc
C_SRCS		= arc.c usage.c
OBJS		= arc.o usage.o

all:	Dacparams arc	

#  .DEFAULT rule is used to retrieve .c & .h source files from SCCS directories
#.DEFAULT:
#	$(GET) $(@F) >/dev/null 2>&1

Dacparams::
	cd $@; $(MAKE) -ef $@.mk

arc:	$(LIBS) $(OBJS)
	$(CC) $(CFLAGS) -o arc $(OBJS) $(LIBS) -lsys5 -L../../lib/Uv4 -lio
	$(MAKE) -f $(THISMK).mk clean

$(OBJS): $(MINCL)

dependlist: $(C_SRCS) $(OBJS)
	-gcc -MM $(INCLUDES) -I- $(C_SRCS) 2>/dev/null \
	  | sed -e "s/^\(.*\) :/$$(LIBRARY)(\1) :/"

depend:
	$(DEPEND)
	cd Dacparams; $(MAKE) -f Dacparams.mk depend

clean:
	if [ ! -d dotOsave ]; then mkdir dotOsave; fi
	-mv $(OBJS) dotOsave/
	-rm -f *.o
	-(cd dotOsave; mv $(OBJS) ..)
	-rmdir dotOsave

#__DEPENDLIST__
