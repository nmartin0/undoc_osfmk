# 
# $Copyright
# Copyright 1993, 1994, 1995  Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
#
# (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
#
# OSF/1 Release 1.0

VPATH		= ../libc:${TARGET_MACHINE}:../libc/${TARGET_MACHINE}:../common_nx_c:../libnx

OTHERS		= libc_r.a
MSGHDRS		= libc_msg.h

EXPLIB_TARGETS	= export_libc_r.a

ILIST		= libc_r.a
IDIR		= /usr/ccs/lib/
IMODE		= 644

INCFLAGS	= -I${TARGET_MACHINE} -I../libc

CFLAGS		= -DCOMPAT_43 -DLANGUAGE_C -DLIBC_SCCS -D_THREAD_SAFE \
		  ${${TARGET_MACHINE}CFLAGS}

OFILES		= ${STDOBJS} \
		  ${${TARGET_MACHINE}COBJS} \
		  ${${TARGET_MACHINE}SOBJS}

STDOBJS		= NLgetenv.o NLscanf.o abort.o alarm.o atoi.o atol.o \
		  clock.o closedir.o clrerr.o catgets.o catopen.o\
		  ctermid.o ctime.o tzset.o mktime.o cuserid.o data.o \
		  doprnt.o doscan.o ecvt.o exec_args.o execlp.o execvp.o \
		  exit.o fdopen.o feof.o ctimeutils.o \
		  ferror.o fgetc.o fgets.o fileno.o findiop.o flockfile.o \
		  flsbuf.o fopen.o fprintf.o fputc.o fputs.o fread.o fseek.o \
		  fstab.o ftell.o ftw.o fwrite.o gcvt.o getc.o getchar.o \
		  getcwd.o getwd.o getgrent.o malloc.o \
		  getpwent.o getpwnamuid.o gets.o getttyent.o getttynam.o \
		  getusershll.o getut.o getw.o getwc.o libc_locks.o \
		  mktemp.o mkstemp.o nice.o nl_langinfo.o opendir.o \
		  nano_timers.o pathconf.o perror.o printf.o putc.o putchar.o \
		  puts.o putw.o putwc.o rand.o random.o readdir.o rec_mutex.o \
		  regex.o rew.o rewinddir.o scanf.o setbuf.o \
		  setbuffer.o setlocale.o setvbuf.o signal.o \
		  sigops.o sprintf.o strerror.o strftime.o \
		  strtok.o strtol.o strtoul.o sysconf.o \
		  tempnam.o tmpfile.o tmpnam.o ttyname.o ulimit.o \
		  ungetc.o vfprintf.o vprintf.o vsprintf.o nx_waitall.o \
		  nx_port.o

include ${TARGET_MACHINE}/machdep.mk
include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/objects.mk

ctime.o data.o findiop.o getgrent.o getpwent.o setlocale.o: rec_mutex.h

NLscanf.o clrerr.o doprnt.o doscan.o fdopen.o feof.o ferror.o fgetc.o \
findiop.o flsbuf.o fopen.o fprintf.o fputc.o fputs.o fread.o fseek.o ftell.o \
fwrite.o getc.o getchar.o gets.o getw.o getwc.o printf.o putc.o putchar.o \
puts.o putw.o putwc.o rew.o scanf.o setbuf.o setbuffer.o setvbuf.o ungetc.o \

vfprintf.o vprintf.o:	stdio_lock.h

${STDOBJS}: $${@:.o=.c}
	${_CC_} ${_CCFLAGS_} -c $*.c
#       ${LD} ${_LDFLAGS_} -x -r $*.o
#       ${MV} -f a.out $*.o

.c.o:
	${_CC_} -c ${_CCFLAGS_} $*.c
#	${LD} ${_LDFLAGS_} -x -r $*.o
#	${MV} -f a.out $*.o

# i860 COFF doesn't use lorder or ranlib
#COFF_LORDER=`lorder *.o | tsort`
COFF_LORDER=`ls *.o`
A_OUT_LORDER=${OFILES}
MACHO_LORDER=${OFILES}

libc_r.a: libc_r.a(${OFILES})
	mkidinfo -f $@
	${_CC_} -c _cvs_id.c
	${AR} ${DEF_ARFLAGS} $@ _cvs_id.o $?
	${RM} -rf tmp
	mkdir tmp
	cd tmp && { \
		${AR} x ../libc_r.a; ${RM} -f __.SYMDEF __________ELELX; \
		${AR} cr libc_r.a ${${OBJECT_FORMAT}_LORDER}; }
	${MV} -f tmp/libc_r.a .
	${RM} -rf tmp
	${RANLIB} $@
	${RM} -f $?  _cvs_id.c _cvs_id.o ../libc_r.a
	ln -s libc_r/libc_r.a ../libc_r.a
include ${MAKEFILEPATH}/depend.mk
-include Makedep
