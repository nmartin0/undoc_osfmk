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

# The following modules contain locale information.  Archiving this
# information into libc.a involves several steps:
#
# 1) The locale's collating table information (ctab) is converted into
# binary form via the ctab utility.
# The new locale and datafile can be installed on a running system so
# that the new locale can be used.
#
# 2) The binary form of the ctab information and non-binary environment
# information are used by the libloc utility to create a C-language source
# module.
#
# 3) The C-language source module is compiled and archived into the
# library.
# 
# 4) Locinfo.c's string and structure pointer arrays for the
# locales are hand modified as locales are added/removed from the
# library  module usr/ccs/lib/libc/locinfo.c (see comments in locinfo.c
# for more comments).
# After you have completed 1,2,3, and 4 you need to rebuild usr/ccs/lib/libc
# and relink your application or command with the new libc.  The newly
# linked application does not need an installed locale or locale datafile
# in order to be found, it is built in.

#
#  Build rules
#

${CTABFILES}: $${@:=.ctab}
	${CTAB} -i ${@:=.ctab} -o $@.X
	${MV} $@.X $@
