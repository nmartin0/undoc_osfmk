# Makefile for mplinpack in /usr/lib/sat/parallel.
# (c) Copyright 1993, Intel, Inc.
# ALL RIGHTS RESERVED
#

SUBDIRS			= blacs lapack plapack

DATAFILES		= README run mplinpack.mk mainc.c mainf.f \
		 	  mainf0.f mainf1.f matrix.f rand.c vector.f 

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
