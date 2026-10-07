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

DATAFILES		= tmac.a tmac.an tmac.an.new tmac.an6n \
			  tmac.an6t tmac.ayday tmac.bib tmac.cp tmac.e \
			  tmac.os tmac.r tmac.s tmac.scover tmac.sdisp \
			  tmac.skeep tmac.srefs tmac.vcat tmac.vgrind

ILIST			= ${DATAFILES}
IDIR			= /usr/share/lib/tmac/
IMODE			= 644

INOBJECTDIR_OPTIONS	= -e '/%beginstrip%/{' \
			  -e 'h' \
			  -e 's/.*/.\\" This version has had comments stripped; an unstripped version is available./p' \
			  -e 'g' \
			  -e '}' \
			  -e '/%beginstrip%/,$$s/[. 	][ 	]*\\".*//' \
			  -e '/^$$/d' \
			  -e '/\\n@/d'

include ${MAKEFILEPATH}/standard.mk
include ${MAKEFILEPATH}/datafiles.mk
include ${MAKEFILEPATH}/depend.mk
-include Makedep
