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
FROMTOP		= ./Acf/Menus


MINCL		= ../../Make.incl
MRULES		= ../../Make.rules

include $(MINCL)
include $(MRULES)

#AR		= /bin/ar

INCDR1		= ./Include
INCDR2		= ../../Common/Include
INCDR3		= ../../Sysdep/$(SYS)/Include
INCLUDES	= -I$(INCDR1) -I$(INCDR2) -I$(INCDR3)
CFLAGS		= $(SWITCHES) $(DEFS) $(INCLUDES)

LIB		= ../../../lib/$(SYS)
LIBRARY		= $(LIB)/Acf.a
THISMK		= ./Menus
C_SRCS		= dcf_menu.c ecf_menu.c ucf_menu.c ccf_menu.c

MEMBERS		= $(LIBRARY)(ecf_menu.o) \
		  $(LIBRARY)(ucf_menu.o) \
		  $(LIBRARY)(dcf_menu.o) \
		  $(LIBRARY)(ccf_menu.o)

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
$(LIBRARY)(dcf_menu.o) : dcf_menu.c ../../Common/Include/stddefs.h \
  ../../Sysdep/$(SYS)/Include/sysdep.h ../../Common/Include/scsidefs.h \
  ../../Common/Include/cdb_struct.h ../../Common/Include/menu.h \
  ../../Common/Include/dialog.h ../../Common/Include/object.h \
  ../../Common/Include/object_set.h ./Include/dcf_menu.h 
$(LIBRARY)(ecf_menu.o) : ecf_menu.c ../../Common/Include/stddefs.h \
  ../../Sysdep/$(SYS)/Include/sysdep.h ../../Common/Include/scsidefs.h \
  ../../Common/Include/cdb_struct.h ../../Common/Include/menu.h \
  ../../Common/Include/dialog.h ../../Common/Include/object.h \
  ../../Common/Include/object_set.h ./Include/ecf_menu.h 
$(LIBRARY)(ucf_menu.o) : ucf_menu.c ../../Common/Include/stddefs.h \
  ../../Sysdep/$(SYS)/Include/sysdep.h ../../Common/Include/scsidefs.h \
  ../../Common/Include/cdb_struct.h ../../Common/Include/menu.h \
  ../../Common/Include/dialog.h ../../Common/Include/object.h \
  ../../Common/Include/object_set.h ./Include/ucf_menu.h 
$(LIBRARY)(ccf_menu.o) : ccf_menu.c ../../Common/Include/stddefs.h \
  ../../Sysdep/$(SYS)/Include/sysdep.h ../../Common/Include/scsidefs.h \
  ../../Common/Include/cdb_struct.h ../../Common/Include/menu.h \
  ../../Common/Include/dialog.h ../../Common/Include/object.h \
  ../../Common/Include/object_set.h ./Include/ccf_menu.h 
