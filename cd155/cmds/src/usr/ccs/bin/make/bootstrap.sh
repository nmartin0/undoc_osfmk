#!/bin/sh
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
if [ "${NO_VFPRINTF}" = "NO_VFPRINTF" ]
then
    MISC_DEFS=-DNO_VFPRINTF
else
    MISC_DEFS=
fi
LIBS_DEFS=-D${ARCHIVE_FORMAT}
MAIN_DEFS=-DMACHINE=\"${MACHINE}\"
RULES_DEFS="-Dtarget_MACHINE=\"TARGET_MACHINE=${TARGET_MACHINE}\" -Dtarget_machine=\"target_machine=${target_machine}\" -Dthis_CPUTYPE=\"CPUTYPE=${CPUTYPE}\" -Dthis_cputype=\"cputype=${cputype}\""
CFLAGS="${HOST_FLAGS} -D_BLD -D_BSD -O"
${CC=cc} -c ${CFLAGS} ${MAIN_DEFS} main.c
${CC} -c ${CFLAGS} doname.c
${CC} -c ${CFLAGS} ${MISC_DEFS} misc.c
${CC} -c ${CFLAGS} files.c
${CC} -c ${CFLAGS} dirs.c
${CC} -c ${CFLAGS} ${LIBS_DEFS} libs.c
${CC} -c ${CFLAGS} ${RULES_DEFS} rules.c
${CC} -c ${CFLAGS} dosys.c
yacc gram.y
mv y.tab.c gram.c
${CC} -c ${CFLAGS} gram.c
${CC} -c ${CFLAGS} dyndep.c
${CC} -c ${CFLAGS} environment.c
${CC} -c ${CFLAGS} rewinddir.c
${CC} main.o doname.o misc.o files.o dirs.o libs.o rules.o dosys.o gram.o dyndep.o environment.o rewinddir.o ${LIBS} -o make
