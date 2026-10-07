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

EXPLIBC_SUBDIRS		= libc libg
EXPCOMMON_SUBDIRS	= common_nx_c libesize
EXPLIBLD_SUBDIRS	= libld
EXPLIB_SUBDIRS		= libPW libbsd libhippi libsys5 \
			  libcurses libdbm libl \
			  libmach libmp libpthreads \
			  libresolv librpc libsb libtermlib libtli \
			  libutil libxti liby ${SEC_LEVEL?libsecurity:} \
			  libAF libc_r libnx libmach_sa \
			  libnetname libpm libcs libcthreads libmachid \
			  libservice libenv 
SUBDIRS			= ${EXPCOMMON_SUBDIRS} libc libld libg \
			  ${EXPLIB_SUBDIRS} cflow migcom xpass libnqs libmacs
DATAFILES		= ncform nrform yaccpar

ILIST			= ncform nrform yaccpar
IDIR			= /usr/ccs/lib/
ncform_IMODE		= 644
nrform_IMODE		= 644
yaccpar_IMODE		= 644

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk
include ${MAKEFILEPATH}/datafiles.mk
