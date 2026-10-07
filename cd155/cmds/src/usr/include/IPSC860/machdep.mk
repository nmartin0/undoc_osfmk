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
# OSF/1 Release 1.0.1

IPSC860_SUBDIRS		= i860

# Mach O format definitions; should be purged. Needed by libld.a
#
MACH_O_FILES		= mach_o_format.h mach_o_header.h mach_o_header_md.h \
			  mach_o_types.h mach_o_vals.h

IPSC860_DATAFILES	= a.out.h aouthdr.h coff.a.out.h coff.nlist.h \
			  filehdr.h float.h linenum.h nan.h ranlib.h \
			  nlist.h reloc.h scnhdr.h setjmp.h varargs.h \
			  stdarg.h va_list.h storclass.h \
			  syms.h values.h ieeefp.h \
			  ${MACH_O_FILES}
