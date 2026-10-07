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
FROMTOP		= ./Ace


MINCL		= ../Make.incl
MRULES		= ../Make.rules

include $(MINCL)
include $(MRULES)

MAKEDEPEND	= makedepend
#AR		= /bin/ar

INCDR1		=
INCDR2		=
INCDR3		= ../Common/Include
INCDR5		= ../Sysdep/$(SYS)/Include
INCLUDES	= -I$(INCDR3) -I$(INCDR5)
CFLAGS		= -DUV4 $(SWITCHES) $(DEFS) $(INCLUDES)

LIB		= ../../lib/$(SYS)
ALIB		=
CLIB		= $(LIB)/Common.a
DLIB		=
ELIB		=
SLIB		= $(LIB)/$(SYS).a
LIBS		= $(SLIB) $(CLIB)
THISMK		= ./Ace
C_SRCS		= ace.c acomm.c acurses.c adebug.c create.c curview.c \
		  garinfo.c modify.c popwin.c laction.c daction.c zipdlog.c \
		  scsicmd.c monitor.c agetrel.c ace_err.c chkrel.c sysdname.c \
		  drvhlp.c lunhlp.c helpcmn.c # can.c 

OBJS		= ace.o acomm.o acurses.o adebug.o create.o curview.o \
		  garinfo.o modify.o popwin.o laction.o daction.o zipdlog.o \
		  scsicmd.o monitor.o agetrel.o ace_err.o chkrel.o sysdname.o \
		  drvhlp.o lunhlp.o  helpcmn.o 

all:	ace

#  .DEFAULT rule is used to retrieve .c & .h source files from SCCS directories
#.DEFAULT:
#	$(GET) $(@F) >/dev/null 2>&1

ace:	$(LIBS) $(OBJS)
	$(CC) $(CFLAGS) -o ace $(OBJS) $(LIBS) -lsys5 -lcurses ../../lib/Uv4/libio.a

$(OBJS): $(MINCL)

depend:
	$(MAKEDEPEND) -f $(THISMK).mk $(CFLAGS) *.c

clean:
	-rm -f *.o

# DO NOT DELETE THIS LINE -- make depend depends on it.

ace.o: /usr/include/stdio.h ../Common/Include/acedefs.h
ace.o: ../Common/Include/scsidefs.h ../Sysdep/Uv4/Include/sysdep.h
ace.o: /usr/include/sys/types.h ../../include/sys/scsi/cs.h
ace.o: ../../include/sys/scsi/SCSI.h ../Common/Include/cdb_struct.h
ace.o: ../Common/Include/dau_err.h ../Sysdep/Uv4/Include/sysio.h
ace.o: ../Common/Include/stddefs.h ../Common/Include/object.h
ace.o: ../../include/sys/libio.h /usr/include/fcntl.h /usr/include/curses.h
ace_err.o: /usr/include/stdio.h /usr/include/ctype.h
ace_err.o: ../Common/Include/stddefs.h ../Sysdep/Uv4/Include/sysdep.h
ace_err.o: /usr/include/sys/types.h ../../include/sys/scsi/cs.h
ace_err.o: ../../include/sys/scsi/SCSI.h ../Common/Include/scsidefs.h
ace_err.o: ../Common/Include/cdb_struct.h ../Common/Include/dau_err.h
ace_err.o: /usr/include/curses.h
acomm.o: /usr/include/stdio.h /usr/include/string.h #/usr/include/stdarg.h
acomm.o: ../Common/Include/stddefs.h ../Sysdep/Uv4/Include/sysdep.h
acomm.o: /usr/include/sys/types.h ../../include/sys/scsi/cs.h
acomm.o: ../../include/sys/scsi/SCSI.h ../Common/Include/scsidefs.h
acomm.o: ../Common/Include/cdb_struct.h ../Common/Include/dau_err.h
acomm.o: ../Common/Include/acurses.h ../Common/Include/acedefs.h
acomm.o: /usr/include/curses.h
acurses.o: /usr/include/stdio.h ../Common/Include/acurses.h
acurses.o: ../Common/Include/acedefs.h ../Common/Include/scsidefs.h
acurses.o: ../Sysdep/Uv4/Include/sysdep.h /usr/include/sys/types.h
acurses.o: ../../include/sys/scsi/cs.h ../../include/sys/scsi/SCSI.h
acurses.o: ../Common/Include/cdb_struct.h /usr/include/curses.h
adebug.o: /usr/include/stdio.h ../Common/Include/acurses.h
adebug.o: ../Common/Include/acedefs.h ../Common/Include/scsidefs.h
adebug.o: ../Sysdep/Uv4/Include/sysdep.h /usr/include/sys/types.h
adebug.o: ../../include/sys/scsi/cs.h ../../include/sys/scsi/SCSI.h
adebug.o: ../Common/Include/cdb_struct.h ../Common/Include/stddefs.h
adebug.o: /usr/include/curses.h
agetrel.o: ../Sysdep/Uv4/Include/getrel.h /usr/include/stdio.h
agetrel.o: /usr/include/fcntl.h ../Common/Include/dau_err.h
agetrel.o: ../Common/Include/stddefs.h ../Sysdep/Uv4/Include/sysdep.h
agetrel.o: /usr/include/sys/types.h ../../include/sys/scsi/cs.h
agetrel.o: ../../include/sys/scsi/SCSI.h ../Common/Include/scsidefs.h
agetrel.o: ../Common/Include/cdb_struct.h
chkrel.o: /usr/include/stdio.h ../Common/Include/acedefs.h
chkrel.o: ../Common/Include/scsidefs.h ../Sysdep/Uv4/Include/sysdep.h
chkrel.o: /usr/include/sys/types.h ../../include/sys/scsi/cs.h
chkrel.o: ../../include/sys/scsi/SCSI.h ../Common/Include/cdb_struct.h
chkrel.o: /usr/include/string.h /usr/include/curses.h
create.o: /usr/include/stdio.h ../Common/Include/acurses.h
create.o: ../Common/Include/acedefs.h ../Common/Include/scsidefs.h
create.o: ../Sysdep/Uv4/Include/sysdep.h /usr/include/sys/types.h
create.o: ../../include/sys/scsi/cs.h ../../include/sys/scsi/SCSI.h
create.o: ../Common/Include/cdb_struct.h ../Common/Include/stddefs.h
create.o: ../Common/Include/dau_err.h /usr/include/curses.h
curview.o: /usr/include/stdio.h ../Common/Include/stddefs.h
curview.o: ../Sysdep/Uv4/Include/sysdep.h /usr/include/sys/types.h
curview.o: ../../include/sys/scsi/cs.h ../../include/sys/scsi/SCSI.h
curview.o: ../Common/Include/scsidefs.h ../Common/Include/cdb_struct.h
curview.o: ../Common/Include/acurses.h ../Common/Include/acedefs.h
curview.o: /usr/include/curses.h
daction.o: /usr/include/stdio.h ../Common/Include/acurses.h
daction.o: ../Common/Include/acedefs.h ../Common/Include/scsidefs.h
daction.o: ../Sysdep/Uv4/Include/sysdep.h /usr/include/sys/types.h
daction.o: ../../include/sys/scsi/cs.h ../../include/sys/scsi/SCSI.h
daction.o: ../Common/Include/cdb_struct.h ../Common/Include/stddefs.h
daction.o: ../Common/Include/dau_err.h /usr/include/curses.h
drvhlp.o: /usr/include/stdio.h /usr/include/curses.h
drvhlp.o: ../Common/Include/acurses.h ../Common/Include/acedefs.h
drvhlp.o: ../Common/Include/scsidefs.h ../Sysdep/Uv4/Include/sysdep.h
drvhlp.o: /usr/include/sys/types.h ../../include/sys/scsi/cs.h
drvhlp.o: ../../include/sys/scsi/SCSI.h ../Common/Include/cdb_struct.h
garinfo.o: /usr/include/stdio.h ../Common/Include/acedefs.h
garinfo.o: ../Common/Include/scsidefs.h ../Sysdep/Uv4/Include/sysdep.h
garinfo.o: /usr/include/sys/types.h ../../include/sys/scsi/cs.h
garinfo.o: ../../include/sys/scsi/SCSI.h ../Common/Include/cdb_struct.h
garinfo.o: /usr/include/curses.h
gmstub.o: /usr/include/stdio.h /usr/include/curses.h /usr/include/fcntl.h
gmstub.o: ../Common/Include/acurses.h ../Common/Include/acedefs.h
gmstub.o: ../Common/Include/scsidefs.h ../Sysdep/Uv4/Include/sysdep.h
gmstub.o: /usr/include/sys/types.h ../../include/sys/scsi/cs.h
gmstub.o: ../../include/sys/scsi/SCSI.h ../Common/Include/cdb_struct.h
helpcmn.o: /usr/include/stdio.h /usr/include/fcntl.h /usr/include/malloc.h
helpcmn.o: ../Common/Include/acedefs.h ../Common/Include/scsidefs.h
helpcmn.o: ../Sysdep/Uv4/Include/sysdep.h /usr/include/sys/types.h
helpcmn.o: ../../include/sys/scsi/cs.h ../../include/sys/scsi/SCSI.h
helpcmn.o: ../Common/Include/cdb_struct.h ../Common/Include/acurses.h
helpcmn.o: /usr/include/curses.h
laction.o: /usr/include/stdio.h ../Common/Include/acurses.h
laction.o: ../Common/Include/acedefs.h ../Common/Include/scsidefs.h
laction.o: ../Sysdep/Uv4/Include/sysdep.h /usr/include/sys/types.h
laction.o: ../../include/sys/scsi/cs.h ../../include/sys/scsi/SCSI.h
laction.o: ../Common/Include/cdb_struct.h ../Common/Include/stddefs.h
laction.o: ../Common/Include/dau_err.h /usr/include/curses.h
lunhlp.o: /usr/include/stdio.h /usr/include/curses.h
lunhlp.o: ../Common/Include/acurses.h ../Common/Include/acedefs.h
lunhlp.o: ../Common/Include/scsidefs.h ../Sysdep/Uv4/Include/sysdep.h
lunhlp.o: /usr/include/sys/types.h ../../include/sys/scsi/cs.h
lunhlp.o: ../../include/sys/scsi/SCSI.h ../Common/Include/cdb_struct.h
modify.o: /usr/include/stdio.h ../Common/Include/acurses.h
modify.o: ../Common/Include/acedefs.h ../Common/Include/scsidefs.h
modify.o: ../Sysdep/Uv4/Include/sysdep.h /usr/include/sys/types.h
modify.o: ../../include/sys/scsi/cs.h ../../include/sys/scsi/SCSI.h
modify.o: ../Common/Include/cdb_struct.h ../Common/Include/dau_err.h
modify.o: ../Common/Include/stddefs.h /usr/include/curses.h
monitor.o: /usr/include/time.h ../Common/Include/acedefs.h
monitor.o: ../Common/Include/scsidefs.h ../Sysdep/Uv4/Include/sysdep.h
monitor.o: /usr/include/sys/types.h ../../include/sys/scsi/cs.h
monitor.o: ../../include/sys/scsi/SCSI.h ../Common/Include/cdb_struct.h
monitor.o: /usr/include/curses.h
popwin.o: /usr/include/stdio.h ../Common/Include/acurses.h
popwin.o: ../Common/Include/acedefs.h ../Common/Include/scsidefs.h
popwin.o: ../Sysdep/Uv4/Include/sysdep.h /usr/include/sys/types.h
popwin.o: ../../include/sys/scsi/cs.h ../../include/sys/scsi/SCSI.h
popwin.o: ../Common/Include/cdb_struct.h /usr/include/curses.h
scsicmd.o: ../Common/Include/stddefs.h ../Sysdep/Uv4/Include/sysdep.h
scsicmd.o: /usr/include/sys/types.h ../../include/sys/scsi/cs.h
scsicmd.o: ../../include/sys/scsi/SCSI.h ../Common/Include/scsidefs.h
scsicmd.o: ../Common/Include/cdb_struct.h ../Common/Include/scsi_dk.h
scsicmd.o: ../Common/Include/scsi_dev.h ../Common/Include/object.h
scsicmd.o: /usr/include/stdio.h ../Sysdep/Uv4/Include/sysio.h
scsicmd.o: ../../include/sys/libio.h /usr/include/fcntl.h
scsicmd.o: ../Common/Include/dau_err.h ../Common/Include/acedefs.h
sysdname.o: ../Common/Include/stddefs.h ../Sysdep/Uv4/Include/sysdep.h
sysdname.o: /usr/include/sys/types.h ../../include/sys/scsi/cs.h
sysdname.o: ../../include/sys/scsi/SCSI.h ../Common/Include/scsidefs.h
sysdname.o: ../Common/Include/cdb_struct.h
zipdlog.o: /usr/include/stdio.h /usr/include/ctype.h /usr/include/string.h
zipdlog.o: /usr/include/sys/types.h ../Common/Include/stddefs.h
zipdlog.o: ../Sysdep/Uv4/Include/sysdep.h ../../include/sys/scsi/cs.h
zipdlog.o: ../../include/sys/scsi/SCSI.h ../Common/Include/scsidefs.h
zipdlog.o: ../Common/Include/cdb_struct.h ../Common/Include/dau_err.h
