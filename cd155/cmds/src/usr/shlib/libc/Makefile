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
# (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
#
# OSF/1 Release 1.0.3

LIBC			= ${MAKETOP}usr/ccs/lib/libc
VPATH			= ${TARGET_MACHINE}:${LIBC}:${LIBC}/${TARGET_MACHINE}

OTHERS			= libc.so
MSGHDRS			= libc_msg.h

USE_SHARED_LIBRARIES	=
LDFLAGS			= -S -x
EXPORTS			= -export libc:
LIBS			= -lloader

EXPSHLIB_TARGETS	= export_libc.so

ILIST			= libc.so
IDIR			= /usr/shlib/
IMODE			= 644

GLINE			=
INCFLAGS		= -I${TARGET_MACHINE} \
			  -I${LIBC} -I${LIBC}/${TARGET_MACHINE}
CFLAGS			= -DCOMPAT_43 -DLANGUAGE_C -DLIBC_SCCS \
			  ${${TARGET_MACHINE}CFLAGS}
scandir.o_CFLAGS	= -D_BSD ${CFLAGS}
wait3.o_CFLAGS		= -D_BSD ${CFLAGS}
kls_ipc.o_CFLAGS	= -D_KLS_IPC_SOCK_DGRAM ${CFLAGS}

ldr_xproc.o_CFLAGS	= -D_USE_PTRACE ${CFLAGS}
ldr_xproc.o_INCFLAGS	= -I${MAKETOP}sbin/loader/include \
			  -I${MAKETOP}sbin/loader/include/${TARGET_MACHINE} \
			  -I${MAKETOP}sbin/loader/ldr_include

OFILES			= ${STDOBJS} \
			  ${${TARGET_MACHINE}SOBJS} \
			  ${${TARGET_MACHINE}COBJS}
GARBAGE			= *.S *.as tags

STDOBJS=NCchrlen.o NCcollate.o NCcoluniq.o NCcolval.o \
	NCdec.o NCdechr.o NCdecode.o NCdecstr.o NCenc.o \
	NCencode.o NCencstr.o NCeqvmap.o NCesc.o NCflatchr.o \
	NCgetbuf.o NCisNLchar.o NCisalnum.o NCisalpha.o NCiscntrl.o \
	NCisdigit.o NCisgraph.o NCislower.o NCisprint.o NCispunct.o \
	NCisshift.o NCisspace.o NCisupper.o NCisxdigit.o NCstrdup.o \
	NCtoNLchar.o NCtolower.o NCtoupper.o NCunesc.o NCwunesc.o \
	NLcatgets.o NLchrlen.o NLcsv.o NLctime.o NLescstr.o \
	NLesctab.o NLfcatgets.o NLflatstr.o NLflattab.o NLfprintf.o \
	NLgetamsg.o NLgetenv.o \
	NLisNLcp.o NLprintf.o NLscanf.o NLsprintf.o NLstrcat.o \
	NLstrchr.o NLstrcmp.o NLstrcpy.o NLstrcspn.o NLstrlen.o \
	NLstrncat.o NLstrncmp.o NLstrncpy.o NLstrpbrk.o NLstrrchr.o \
	NLstrspn.o NLstrtok.o NLtmtime.o NLunescstr.o \
	NLunesctab.o NLvfprintf.o NLvprintf.o NLvsprintf.o NLxcol.o \
	NLxio.o NLyesno.o _NCtolower.o _NCtoupper.o \
	_tolower.o _toupper.o a64l.o abort.o alarm.o \
	assert.o atof.o atoi.o atol.o auditwrite.o \
	basename.o bcopy.o break.o bsearch.o calloc.o \
	catgetmsg.o catgets.o catopen.o cfspeed.o clock.o \
	closedir.o clrerr.o copysign.o creadir.o crt0msg.o crypt.o \
	ctermid.o ctime.o cuserid.o data.o \
	difftime.o dirname.o disklabel.o divide.o doprnt.o dordmsg.o \
	doscan.o drand48.o dsto2fp.o ecvt.o errlst.o ether_addr.o \
	exec_args.o execlp.o execvp.o exit.o expacct.o \
	fcatgetmsg.o fcatgets.o fdopen.o feof.o ferror.o fgetc.o \
	fgetpos.o fgets.o fgetwc.o filbuf.o fileno.o \
	findiop.o floor.o flsbuf.o fopen.o fprintf.o \
	fputc.o fputs.o fputwc.o fread.o fseek.o \
	fsetpos.o fstab.o ftell.o ftok.o ftw.o \
	fwrite.o gcvt.o getc.o getchar.o getcwd.o \
	getenv.o getgrent.o gethostnmad.o getmbcurmax.o getmntinfo.o \
	getnetent.o getopt.o getpass.o getpw.o getpwent.o \
	getpwnamuid.o getrpcent.o gets.o getservent.o getttyent.o \
	getttynam.o getusershll.o getut.o getw.o getwc.o \
	getwchar.o getwd.o gnetgrent.o gprotoent.o gtty.o \
	herror.o hsearch.o inet_addr.o inet_lnaof.o inet_mkadr.o \
	inet_netof.o inet_netwk.o inet_ntoa.o initgroups.o innetgr.o \
	isalnum.o isalpha.o isascii.o isatty.o iscntrl.o \
	isdigit.o isgraph.o islower.o isprint.o ispunct.o \
	isspace.o isupper.o isxdigit.o itrunc.o killpg.o \
	kls_client.o kls_ipc.o kls_message.o \
	l3.o l64a.o labs.o \
	ldr_atexit.o ldr_status.o ldr_syscalls.o ldr_xcalls.o ldr_xproc.o \
	lfind.o liblocale.o linkaddr.o localeconv.o \
	lockf.o lsearch.o malloc.o mblen.o mbs.o mbscat.o mbscmp.o \
	mbscpy.o mbsncmp.o mbspbrk.o mbstowcs.o mbtowc.o \
	memccpy.o memchr.o memcmp.o memcpy.o memmove.o \
	memset.o mkfifo.o mk_query.o mkstemp.o mktemp.o \
	nano_timers.o ndbm.o nice.o ns_addr.o ns_ntoa.o \
	nl_langinfo.o opendir.o pathconf.o \
	pause.o perror.o popen.o printf.o psignal.o putc.o \
	putchar.o putenv.o putgr.o putgrent.o putgrsent.o \
	putpw.o putpwent.o puts.o putw.o putwc.o \
	putwchar.o qsort.o quota_init.o raise.o rand.o random.o \
	rcmd.o readdir.o regex.o remove.o res_comp.o \
	res_debug.o res_init.o res_mkquery.o res_query.o res_send.o \
	rew.o rewinddir.o rexec.o rint.o ruserpass.o \
	scandir.o scanf.o seekdir.o setbuf.o setbuffer.o setegid.o \
	setenv.o seteuid.o sethostent.o setjmper.o setlocale.o setrgid.o \
	setruid.o setvbuf.o sigcompat.o sighold.o siglist.o \
	signal.o sigops.o sigsetjmp.o sleep.o sprintf.o ssignal.o\
	stime.o strcasecmp.o strchr.o strcoll.o strcpyn.o strcspn.o \
	strdup.o streamio.o strerror.o strftime.o strncat.o strncmp.o \
	strncpy.o strpbrk.o strrchr.o strspn.o strstr.o strtod.o \
	strtok.o strtol.o strtoul.o strxfrm.o stty.o \
	swab.o sysconf.o syslog.o system.o tcdrain.o \
	tcflow.o tcflush.o tcgetattr.o tcgetpgrp.o tcsendbreak.o \
	tcsetattr.o tcsetpgrp.o tell.o telldir.o tempnam.o \
	time.o times.o tmpfile.o tmpnam.o toascii.o \
	tolower.o toupper.o tsearch.o ttylock.o ttyname.o \
	ttyslot.o ualarm.o uitrunc.o ulimit.o ungetc.o ungetwc.o \
	usleep.o ustat.o utime.o vfprintf.o vprintf.o vsprintf.o \
	wait.o wait3.o waitpid.o wcscat.o wcschr.o \
	wcscspn.o wcslen.o wcsncat.o wcsncpy.o wcspbrk.o \
	wcsrchr.o wcsspn.o wcstombs.o wcswcs.o wctomb.o creat.o \
	atojis.o isj1kana.o isjparen.o jistouj.o ujtojis.o \
	cjistosj.o isjalnum.o isjprint.o kutentojis.o ujtosj.o \
	cjistouj.o isjalpha.o isjpunct.o putws.o watof.o \
	csjtojis.o isjdigit.o isjspace.o sjtojis.o watoi.o \
	csjtouj.o isjgraph.o isjupper.o sjtouj.o watol.o \
	cujtojis.o isjhira.o isjxdigit.o tojhira.o wdsto2fp.o \
	cujtosj.o isjis.o jctype_.o tojkata.o wsto2fp.o \
	fgetws.o isjkanji.o jistab.o tojlower.o wstrtod.o \
	fputws.o isjkata.o jistoa.o tojupper.o wstrtol.o \
	getws.o isjlower.o jistosj.o toujis.o

-include ${TARGET_MACHINE}/machdep.mk
include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/others.mk
include ${MAKEFILEPATH}/objects.mk

NLgetctab.o NLinit.o assert.o crt0msg.o getopt.o getwd.o initgroups.o \
perror.o psignal.o random.o regex.o setjmper.o syslog.o: libc_msg.h

${LOCALE}: ${LOCALEPATH}/$${@:=.ctab}
	${CTAB} -i ${LOCALEPATH}/${@:=.ctab} -o $@.X
	${MV} $@.X $@

liblocale.c: ${LOCALEPATH}/$${LOCALE}.en ${LOCALE}
	${LIBLOC} ${LOCALE} ${LOCALE} ${LOCALEPATH}/${LOCALE}.en > $@

${STDOBJS}: $${@:.o=.c}
	${_CC_} ${_CCFLAGS_} -c $*.c
#	${LD} -x -r $*.o
#	${MV} -f a.out $*.o

.c.o:
	${_CC_} -c ${_CCFLAGS_} $*.c
#	${LD} ${_LDFLAGS_} -x -r $*.o
#	${MV} -f a.out $*.o

libc.so: ${_OFILES_}
	${_LD_} ${_SHLDFLAGS_} -o $@.X ${_OFILES_} ${_LIBS_}
	${MV} $@.X $@
include ${MAKEFILEPATH}/depend.mk
-include Makedep
