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

LOADERS		= print_hex_flags_loader print_dec_flags_loader \
		  print_file_loader print_loader_loader \
		  print_argc_loader print_argv_loader print_env_loader \
		  user_loader echo_loader failure_loader

RANDOMS		= dec_random hex_random

CPROGS		= execve exec_with_loader env \
		  getuid geteuid getsetuid getsetgid \
		  setuidprogram setgidprogram \
		  test04 test05 test06 test11 test12 test13 test18 test19 \
		  cprog25

PROGRAMS	= $(LOADERS) $(RANDOMS) $(CPROGS)

SCRIPTS		= prog loop test01 test02 test03 test07 test08 test09 \
		  test14 test15 test16 test17 test20 test21 test22 test23 \
		  test24 test25 test26 test27 test28 test29 test30 \
		  test31 test32

dec_random_OFILES		= dec_random.o
hex_random_OFILES		= hex_random.o
hex_random.o_CFLAGS		= -DHEX

print_dec_flags_loader_OFILES	= print_dec_flags_loader.o auxv.o
print_hex_flags_loader_OFILES	= print_hex_flags_loader.o auxv.o
print_hex_flags_loader.o_CFLAGS	= -DHEX

print_loader_loader_OFILES	= print_loader_loader.o auxv.o
print_file_loader_OFILES	= print_file_loader.o auxv.o
echo_loader_OFILES		= echo_loader.o auxv.o

getsetuid_OFILES		= getsetuid.o
getsetgid_OFILES		= getsetgid.o
getsetuid.o_CFLAGS		= -DSETUID

LIBLT_DIR	= ../liblt

test04_LIBS		= $(LIBLT_DIR)/liblt.a
test05_LIBS		= $(LIBLT_DIR)/liblt.a
test06_LIBS		= $(LIBLT_DIR)/liblt.a
test11_LIBS		= $(LIBLT_DIR)/liblt.a
test12_LIBS		= $(LIBLT_DIR)/liblt.a
test13_LIBS		= $(LIBLT_DIR)/liblt.a
test18_LIBS		= $(LIBLT_DIR)/liblt.a
test19_LIBS		= $(LIBLT_DIR)/liblt.a
cprog25_LIBS		= $(LIBLT_DIR)/liblt.a

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/programs.mk
include ${MAKEFILEPATH}/objects.mk
include ${MAKEFILEPATH}/scripts.mk

dec_random.c: random.c
	cp random.c dec_random.c
hex_random.c: random.c
	cp random.c hex_random.c

print_dec_flags_loader.c: print_flags_loader.c
	cp print_flags_loader.c print_dec_flags_loader.c
print_hex_flags_loader.c: print_flags_loader.c
	cp print_flags_loader.c print_hex_flags_loader.c

getsetuid.c: getsetid.c
	cp getsetid.c getsetuid.c

getsetgid.c: getsetid.c
	cp getsetid.c getsetgid.c
include ${MAKEFILEPATH}/depend.mk
-include Makedep
