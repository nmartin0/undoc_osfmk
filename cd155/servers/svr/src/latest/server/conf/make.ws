#! /bin/sh
# 
# $Copyright
# Copyright 1993, 1994 , 1995 Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
# HISTORY
# $Log: make.ws,v $
# Revision 1.5  1994/11/18  20:29:46  mtm
# Copyright additions/changes
#
# Revision 1.4  1993/07/14  17:54:16  cfj
# OSF/1 AD 1.0.4 code drop from Locus.
#
# Revision 1.1.1.3  1993/07/01  19:12:36  cfj
# Adding new code from vendor
#
# Revision 1.3  1993/05/06  19:14:41  cfj
# ad103+tnc merged with Intel code.
#
# Revision 1.1.1.1  1993/05/03  17:27:48  cfj
# Initial 1.0.3 code drop
#
# Revision 1.2  1992/11/30  22:19:22  dleslie
# Copy of NX branch back into main trunk
#
# Revision 1.1.2.1  1992/11/05  23:18:31  dleslie
# Local changes for NX through noon, November 5, 1992.
#
#Revision 2.3  1991/10/18  18:31:08  chrisp
#	RCS Source keyword removed.
#
#Revision 2.2  91/08/31  13:30:42  rabii
#	Initial V2.0 Checkin
#
#	Revision 3.0  91/01/17  12:06:58  condict
#	Unchanged copy from Mach 3.0 BSD UNIX server
#	
# Revision 2.1  89/08/04  15:28:58  rwd
# Created.
# 
# Revision 2.4  88/07/20  16:23:26  rpd
# Don't change PATH.  Instead, pick up omerge & makesym as arguments.
# (Default location is ../bin/.)  Note new version of makesym is needed
# for longer names in the symbol table.
# 
# Revision 2.3  88/07/15  15:56:05  mja
# Import ../conf/fixsyms in $4 and ../mach/rdb.h on $5 so that they can be
# sought by make along VPATH when necessary.
# 
# 11-Jun-88  Michael Young (mwyoung) at Carnegie-Mellon University
#	Take source directory name as a fifth argument.
#
# 23-Jun-87  William Bolosky (bolosky) at Carnegie-Mellon University
#	Changed completion message.
#
# 27-Apr-87  Bill Bolosky (bolosky) at Carnegie-Mellon University
#	Removed useless acis "cross-compile" code, and also now look
#	at the contents of "romp_rdb.h" to decide if we really need
#	to do a debugger merge.
#
# 11-Nov-86  Robert Baron (rvb) at Carnegie-Mellon University
#	set kernel back to segment e
#
#set -x$-
#
# shell script to generate a workstation binary with the debugger and
# symbol table merged in.
#
#
# this version deals with only the native compile case.
#
input=${1-vmunix}
output=${3-$input.ws}
fixsyms=${4-../conf/fixsyms}
rdbh=${5-../machine/rdb.h}
omerge=${6-../bin/omerge}
makesym=${7-../bin/makesym}

rm -f $output

case `cat romp_rdb.h` in
*0*)
		# no debugger merge needed
	ln $input $output
	echo "Debugger not configured, so not merged."
	exec true
	;;
*)		# native compile
	VMRDB=${2-../standca/rdb.out}
	nm -n $input > $input.nm || exit 1
	;;

esac

	echo "symbol table produced in $input.nm"

	rm -f $input.out
	cp $input $input.out
#
# get the offsets and positions of the debugger 
#
eval `awk ' $1 == "#define" { print $2 "=" substr($3,3) } ' $rdbh `
# make a vmunix.out that is -N format (vax compatible)
	$omerge -p2048 -o0x${RDB_RELOC?} -e0x${RDB_END?} ${VMRDB} $input.out || exit 1
# convert NCS symbols if necessary
$fixsyms $input.nm $input.snm
# following sed makes the 'int' routines visable and also changes the
# virtual kernel addresses back into real addresses
	( sed -e 's;^e;0;' -e 's; int; _int;' $input.snm ) |
		$makesym -h > $input.sym
	$omerge -p2048 -o0x${RDB_SYMTAB?} -l0x${RDB_SYMLEN?} $input.sym $input.out || exit 1
	echo "debugger (from ${VMRDB}) merged into $input.out"

	ln $input.out $output

	echo "Mach kernel with debugger now in $output"

