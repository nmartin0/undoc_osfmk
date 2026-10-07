# 
# $Copyright
# Copyright 1991 , 1994, 1995 Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
#       Copyright (c) 1989, Intel Corporation.
#       All rights reserved.
#
#         INTEL CORPORATION PROPRIETARY INFORMATION
#
#  This software is supplied under the terms of a license
#  agreement or nondisclosure agreement with Intel Corp.
#  and may not be copied or disclosed except in accordance
#  with the terms of that agreement.
#

# @(#)fpe.mk	3.0  

INCRT = ..
CFLAGS = -I$(INCRT) -D_KERNEL -O $(MORECPP)
CPP = $(CCSROOT)/lib/cpp
OFILE = $(CONF)/pack.d/fpe/Driver.o

FRC =

FILES =\
	fpe_main.o \
	fpe_source.o \
	fpe_result.o \
	fpe_deno.o \
	fpe_utils.o \
	fpe_chk.o \
	fpe_fadd.o \
	fpe_fmul.o \
	fpe_ops.o \
	fpe_vars.o \
	fpe_debug.o \
	fpe_prims.o

all:	$(OFILE) 

$(OFILE): $(FILES)
	$(LD) -r $(FILES) -o $(OFILE)

install: all

clean:
	rm -f $(FILES) $(OFILE)

clobber:
	rm -f $(FILES) $(OFILE)
