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

#
#  Definitions for .INOBJECTDIR rule
#
_INOBJECTDIR_OPTIONS_=${INOBJECTDIR_OPTIONS?${INOBJECTDIR_OPTIONS}:-e ''}
_INOBJECTDIR_=${INOBJECTDIR?${INOBJECTDIR}:${DATAFILES}}

#
#  Default update rule
#
.INOBJECTDIR: ${_INOBJECTDIR_}
	${SED} ${${@}_SED_OPTIONS?${${@}_SED_OPTIONS}:${SED_OPTIONS}} ${_INOBJECTDIR_OPTIONS_} <$@ >./$@.X
	${MV} -f ./$@.X ./$@
