# $Copyright
# Copyright 1992, 1993, 1994, 1995 Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
SCRIPT	=	mk_script
ENT	=	mk_ent

TOOLS	=	$(SCRIPT) $(ENT)
CODE	=	siop0_script.c siop1_script.c siop_ent.h

all:		tools code install

tools:		$(TOOLS)

code:		$(CODE)

install:	$(CODE)
	sync

siop0_script.c:	script.out $(SCRIPT) makefile
	rm -f $@
	echo "#include <siop.h>" >> $@
	echo "#if	NSIOP > 0" >> $@
	echo >> $@
	echo "#define SIOP0" >> $@
	echo >> $@
	echo "#include <scsi/compat_30.h>" >> $@
	echo "#include <scsi/scsi_defs.h>" >> $@
	echo "#include <i860paragon/sdb/sdb.h>" >> $@
	echo "#include <i860paragon/sdb/siop_script.h>" >> $@
	echo "#include <scsi/adapters/scsi_53C720.h>" >> $@
	echo >> $@
	cat script.out | mk_script >> $@
	echo >> $@
	echo "ULONG	SIZEOF_SCRIPT	= sizeof(SCRIPT);" >> $@
	echo >> $@
	echo "#endif	NSIOP > 0" >> $@

siop1_script.c:	script.out $(SCRIPT) makefile
	rm -f $@
	echo "#include <siop.h>" >> $@
	echo "#if	NSIOP > 0" >> $@
	echo >> $@
	echo "#define SIOP1" >> $@
	echo >> $@
	echo "#include <scsi/compat_30.h>" >> $@
	echo "#include <scsi/scsi_defs.h>" >> $@
	echo "#include <i860paragon/sdb/sdb.h>" >> $@
	echo "#include <i860paragon/sdb/siop_script.h>" >> $@
	echo "#include <scsi/adapters/scsi_53C720.h>" >> $@
	echo >> $@
	cat script.out | mk_script >> $@
	echo >> $@
	echo "ULONG	SIZEOF_SCRIPT	= sizeof(SCRIPT);" >> $@
	echo >> $@
	echo "#endif	NSIOP > 0" >> $@

siop_ent.h:	script.out $(ENT) makefile
	rm -f $@
	echo "#ifndef	_SIOP_ENT_H_" >> $@
	echo "#define	_SIOP_ENT_H_" >> $@
	cat script.out | mk_ent >> $@
	echo "#endif	_SIOP_ENT_H_" >> $@

clean_all:
	rm -f $(TOOLS) $(CODE)

clean:
	rm -f $(CODE)
