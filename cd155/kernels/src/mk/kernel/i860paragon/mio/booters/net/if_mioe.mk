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
 
#
# HISTORY
# $Log: if_mioe.mk,v $
# Revision 0.4  1994/11/18  20:45:42  mtm
# Copyright additions/changes
#
# Revision 0.3  1993/06/30  22:40:17  dleslie
# Adding copyright notices required by legal folks
#
# Revision 0.2  1992/07/17  17:36:56  rkl
# Brought up to date with working version.
#
#	Revision 1.3  92/03/05  11:39:10  rkl
#	Fixes after receive simulation.
#	
#	Revision 1.2  92/02/27  17:06:29  rkl
#	Changes after personal code review.
#	
#	Revision 1.1  92/02/21  12:33:41  rkl
#	Initial revision
#	
 
AS	= as860
CC	= cc860
LD	= ld860
DEBUG	= -DDEBUG -DMIOE_STATS #-DSW_MEM_REFRESH
DEFS	= -DMACH -DMACH_KERNEL -DKERNEL -DNMIOE=1 -DNET_BOOT -Ddp_printf=printf
INC	= -Iinc-1 -Iinc-2 -Iinc-3 -I.
#
# PGI FLAGS
#
LDFLAGS		= -e _start -Mx
HEX_LOC		= -T 0xc0001000 -d 0xc000c000
FLASH_LOC	= -T 0xc0001800 -d 0xc000c000
OBJ_LOC		= -T 0xc0080000 -d 0xc00a0000
HELLO_LOC	= -T 0xc00d0000 -d 0xc00e0000
SPECIAL		= -i860
LIBS		= /usr/pgi/i860/lib/libpgc.a /usr/pgi/i860/lib/libc.a
#VOLATILE	= -Dvolatile=
CFLAGS		= $(SPECIAL) $(DEFS) $(INC) $(DEBUG) $(VOLATILE)

MIOE_OBJ  = if_mioe.o mio_support.o
MIOE_SRC  = if_mioe.c mio_support.c
IO_OBJ	  = 82510.o io.o printf.o
FLASH_OBJ = start.o net_boot.o ethernet.o flash.o stubs.o dcomp.o msgload.o
#FLASH_OBJ = net_boot.o ethernet.o flash.o stubs.o dcomp.o msgload.o
FLASH_SRC = net_boot.c ethernet.c flash.c stubs.c dcomp.c msgload.c
QUICK_OBJ = quicktest.o 82510.o io.o printf.o
HELLO_OBJ = hello.o 82510.o io.o printf.o

.SUFFIXES:	.lst

.c.lst:
	$(CC) $(CFLAGS) -S $<
	$(AS) -l $*.s > $*.lst

.c.s:
	$(CC) $(CFLAGS) -S $<

all:	if_mioe.o mio_support.o

loader:	loader.o
	$(LD) $(LDFLAGS) -k -T 0xc0001000 -d 0xc0001800 -o loader.out \
					loader.o  > loader.map
	hex loader.out tmp.hex
	cat tmp.hex hex.EOF > loader.hex

flash.tftp:	$(FLASH_OBJ) $(MIOE_OBJ) $(IO_OBJ)
	$(LD) $(LDFLAGS) $(FLASH_LOC) -o flash.tftp.out $(FLASH_OBJ) \
							$(MIOE_OBJ)  \
							$(IO_OBJ)    \
							$(LIBS) > flash.tftp.map

flash.obj:	$(FLASH_OBJ) $(MIOE_OBJ) $(IO_OBJ)
	$(LD) $(LDFLAGS) $(OBJ_LOC) -o flash.obj.out $(FLASH_OBJ) \
						      $(MIOE_OBJ)  \
						      $(IO_OBJ)    \
						      $(LIBS) > flash.obj.map

flash:	$(FLASH_OBJ) $(MIOE_OBJ) $(IO_OBJ)
	$(LD) $(LDFLAGS) -k $(HEX_LOC) -o flash.out $(FLASH_OBJ) \
						      $(MIOE_OBJ)  \
						      $(IO_OBJ)    \
						      $(LIBS) > flash.map
	hex flash.out tmp.hex
	cat tmp.hex hex.EOF > flash.hex

hello:	$(HELLO_OBJ)
	$(LD) $(LDFLAGS) $(HELLO_LOC) -o hello.out $(HELLO_OBJ) $(LIBS) >hello.map

quicktest:	$(QUICK_OBJ)
	$(LD) $(LDFLAGS) -k $(HEX_LOC)  -o quicktest.out $(QUICK_OBJ) \
							 $(LIBS) > quicktest.map
	hex quicktest.out tmp.hex
	cat tmp.hex hex.EOF > quicktest.hex

if_mioe.o:	if_mioe.h

lint:
	lint $(INC) -Dvolatile= $(DEBUG) $(DEFS) if_mioe.c > lint.out

tftp.lint:
	lint $(INC) -Dvolatile= $(DEBUG) $(DEFS) $(TFTP_SRC) > lint.out

flash.lint:
	lint $(INC) -Dvolatile= $(DEBUG) $(DEFS) $(FLASH_SRC) > lint.out

tftp.tags:
	ctags $(TFTP_SRC)

