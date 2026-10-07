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

LIBRARIES		= libsb.a

EXPLIB_TARGETS		= export_libsb.a
EXPDIR			= /usr/ccs/lib/

CFLAGS			= -D_BSD
run.o_CFLAGS		= ${CFLAGS} -D${${TARGET_MACHINE}_VA_ARGV}
run_p.o_CFLAGS		= ${CFLAGS} -D${${TARGET_MACHINE}_VA_ARGV}
OFILES			= mklinks.o par_cmd_line.o par_rc_file.o \
			  print_usage.o rc_file_ext.o runcmd.o uquit.o \
			  abspath.o concat.o editor.o errmsg.o fdate.o \
			  ffilecopy.o filecopy.o getbool.o getstab.o \
			  getstr.o makepath.o nxtarg.o path.o quit.o \
			  run.o salloc.o searchp.o sindex.o stablk.o \
			  fold.o skipto.o stlmatch.o prstab.o \
			  ${NEED_FOR_PORTING}

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/libs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
