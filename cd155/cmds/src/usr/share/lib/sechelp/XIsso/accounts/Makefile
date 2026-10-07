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

SEC_DATAFILES		= DefAuth DefClear DefCom DefHours DefLogin \
			  DefPass DefSub GetIssoName GetName GetSysName \
			  Lock MakeGroup  MakeUser Retire SinUserCl \
			  Unlock UserAudit UserAuth UserClear UserCom \
			  UserGroup UserHours UserLogin UserPass UserSub 
DATAFILES		= ${SEC_DATAFILES}

SEC_ILIST		= ${DATAFILES}
ILIST			= ${SEC_ILIST}

IDIR			= /usr/share/lib/sechelp/XIsso/accounts/
IMODE			= 644

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
