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

MACHO_SUBDIRS		= odump strings

COFF_SUBDIRS		= odump strings 

SUBDIRS			= cflow cb ctags cxref error indent \
			  lex lint lorder m4 make mig mkstr \
			  tsort xstr yacc \
			  ${${ARCHIVE_FORMAT}_SUBDIRS}
# notyet - gprof prof
#
# PGI supplies these: as ar ld icc ftn nm size strip dump

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/subdirs.mk
