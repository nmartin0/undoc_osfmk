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

DATAFILES		= C.en da_DK.88591.en de_CH.88591.en \
			  de_DE.88591.en en_GB.88591.en en_US.88591.en \
			  es_ES.88591.en fi_FI.88591.en fr_BE.88591.en \
			  fr_CA.88591.en fr_CH.88591.en fr_FR.88591.en \
			  it_IT.88591.en ja_JP.SJIS.en nl_BE.88591.en \
			  nl_NL.88591.en no_NO.88591.en pt_PT.88591.en \
			  sv_SE.88591.en
CTABFILES		= C da_DK.88591 de_DE.88591 en_GB.88591 \
			  en_US.88591 es_ES.88591 fi_FI.88591 \
			  fr_CA.88591 fr_FR.88591 it_IT.88591 \
			  ja_JP.SJIS nl_NL.88591 no_NO.88591 \
			  pt_PT.88591 sv_SE.88591
OTHERS			= ${CTABFILES}

ILIST			= ${DATAFILES} ${CTABFILES}
IDIR			= /usr/lib/nls/loc/

de_DE.88591_ILINKS	= ${IDIR}de_CH.88591
fr_FR.88591_ILINKS	= ${IDIR}fr_BE.88591 ${IDIR}fr_CH.88591
nl_NL.88591_ILINKS	= ${IDIR}nl_BE.88591

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/locales.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
