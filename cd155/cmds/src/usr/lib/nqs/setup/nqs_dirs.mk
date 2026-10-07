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
 
#  Check for and make if necessary NQS working directories.
#
#  NQSROOT is set on the make line.  If not set, / is assumed.
#
NQS_OWNER	= root
NQS_GROUP	= system
NQS_USR		= $(NQSROOT)/usr
NQS_ETC		= $(NQSROOT)/etc
NQS_USRLIB	= $(NQS_USR)/lib
NQS_USRSPOOL	= $(NQS_USR)/spool
NQS_SPOOL	= $(NQS_USRSPOOL)/nqs
NQS_LIBEXE	= $(NQS_USRLIB)/nqs
NQS_USREXE	= $(NQS_USR)/bin
QMGR_HELPFILE	= $(NQS_LIBEXE)/qmgr.hlp
MAP_DIRECTORY	= $(NQS_ETC)/nmap

#
#	NQS directory installation.
#
directories:
	@if [ ! -d $(NQSROOT) ]      ; then mkdir $(NQSROOT)      ; fi
	@if [ ! -d $(NQS_USR) ]      ; then mkdir $(NQS_USR)      ; fi
	@if [ ! -d $(NQS_USREXE) ]   ; then mkdir $(NQS_USREXE)   ; fi
	@if [ ! -d $(NQS_USRLIB) ]   ; then mkdir $(NQS_USRLIB)   ; fi
	@if [ ! -d $(NQS_USRSPOOL) ] ; then mkdir $(NQS_USRSPOOL) ; fi
	@if [ ! -d $(NQS_ETC) ]      ; then mkdir $(NQS_ETC)      ; fi
	@if [ ! -d $(NQS_SPOOL) ]    ; then mkdir $(NQS_SPOOL)    ; fi
	@chmod 755 $(NQS_SPOOL)
	@chown $(NQS_OWNER) $(NQS_SPOOL)
	@chgrp $(NQS_GROUP) $(NQS_SPOOL)
	@if [ ! -d $(NQS_SPOOL)/log.d ] ; then mkdir $(NQS_SPOOL)/log.d ; fi
	@chmod 755 $(NQS_SPOOL)/log.d
	@chown $(NQS_OWNER) $(NQS_SPOOL)/log.d
	@chgrp $(NQS_GROUP) $(NQS_SPOOL)/log.d
	@if [ ! -d  $(NQS_SPOOL)/new ] ; then mkdir $(NQS_SPOOL)/new ; fi
	@chmod 700 $(NQS_SPOOL)/new
	@chown $(NQS_OWNER) $(NQS_SPOOL)/new
	@chgrp $(NQS_GROUP) $(NQS_SPOOL)/new
	@if [ ! -d  $(NQS_SPOOL)/new/requests ] ; then mkdir $(NQS_SPOOL)/new/requests ; fi
	@chmod 777 $(NQS_SPOOL)/new/requests
	@chown $(NQS_OWNER) $(NQS_SPOOL)/new/requests
	@chgrp $(NQS_GROUP) $(NQS_SPOOL)/new/requests
	@if [ ! -d  $(NQS_SPOOL)/private ] ; then mkdir $(NQS_SPOOL)/private ; fi
	@chmod 700 $(NQS_SPOOL)/private
	@chown $(NQS_OWNER) $(NQS_SPOOL)/private
	@chgrp $(NQS_GROUP) $(NQS_SPOOL)/private
	@if [ ! -d  $(NQS_SPOOL)/private/root ] ; then mkdir $(NQS_SPOOL)/private/root ; fi
	@chmod 777 $(NQS_SPOOL)/private/root
	@chown $(NQS_OWNER) $(NQS_SPOOL)/private/root
	@chgrp $(NQS_GROUP) $(NQS_SPOOL)/private/root
	@if [ ! -d  $(NQS_SPOOL)/private/root/control ] ; then mkdir $(NQS_SPOOL)/private/root/control ; fi
	@chmod 777 $(NQS_SPOOL)/private/root/control
	@chown $(NQS_OWNER) $(NQS_SPOOL)/private/root/control
	@chgrp $(NQS_GROUP) $(NQS_SPOOL)/private/root/control
	@if [ ! -d  $(NQS_SPOOL)/private/root/data ] ; then mkdir $(NQS_SPOOL)/private/root/data ; fi
	@chmod 777 $(NQS_SPOOL)/private/root/data
	@chown $(NQS_OWNER) $(NQS_SPOOL)/private/root/data
	@chgrp $(NQS_GROUP) $(NQS_SPOOL)/private/root/data
	@if [ ! -d  $(NQS_SPOOL)/private/root/database ] ; then mkdir $(NQS_SPOOL)/private/root/database ; fi
	@chmod 777 $(NQS_SPOOL)/private/root/database
	@chown $(NQS_OWNER) $(NQS_SPOOL)/private/root/database
	@chgrp $(NQS_GROUP) $(NQS_SPOOL)/private/root/database
	@if [ ! -d  $(NQS_SPOOL)/private/root/database_qa ] ; then mkdir $(NQS_SPOOL)/private/root/database_qa ; fi
	@chmod 777 $(NQS_SPOOL)/private/root/database_qa
	@chown $(NQS_OWNER) $(NQS_SPOOL)/private/root/database_qa
	@chgrp $(NQS_GROUP) $(NQS_SPOOL)/private/root/database_qa
	@if [ ! -d  $(NQS_SPOOL)/private/root/database_qo ] ; then mkdir $(NQS_SPOOL)/private/root/database_qo ; fi
	@chmod 777 $(NQS_SPOOL)/private/root/database_qo
	@chown $(NQS_OWNER) $(NQS_SPOOL)/private/root/database_qo
	@chgrp $(NQS_GROUP) $(NQS_SPOOL)/private/root/database_qo
	@if [ ! -d  $(NQS_SPOOL)/private/root/failed ] ; then mkdir $(NQS_SPOOL)/private/root/failed ; fi
	@chmod 777 $(NQS_SPOOL)/private/root/failed
	@chown $(NQS_OWNER) $(NQS_SPOOL)/private/root/failed
	@chgrp $(NQS_GROUP) $(NQS_SPOOL)/private/root/failed
	@if [ ! -d  $(NQS_SPOOL)/private/root/interproc ] ; then mkdir $(NQS_SPOOL)/private/root/interproc ; fi
	@chmod 777 $(NQS_SPOOL)/private/root/interproc
	@chown $(NQS_OWNER) $(NQS_SPOOL)/private/root/interproc
	@chgrp $(NQS_GROUP) $(NQS_SPOOL)/private/root/interproc
	@if [ ! -d  $(NQS_SPOOL)/private/root/output ] ; then mkdir $(NQS_SPOOL)/private/root/output ; fi
	@chmod 777 $(NQS_SPOOL)/private/root/output
	@chown $(NQS_OWNER) $(NQS_SPOOL)/private/root/output
	@chgrp $(NQS_GROUP) $(NQS_SPOOL)/private/root/output
	@if [ ! -d  $(NQS_SPOOL)/private/root/transact ] ; then mkdir $(NQS_SPOOL)/private/root/transact ; fi
	@chmod 777 $(NQS_SPOOL)/private/root/transact
	@chown $(NQS_OWNER) $(NQS_SPOOL)/private/root/transact
	@chgrp $(NQS_GROUP) $(NQS_SPOOL)/private/root/transact
	@if [ ! -d  $(NQS_SPOOL)/scripts ] ; then mkdir $(NQS_SPOOL)/scripts ; fi
	@chmod 755 $(NQS_SPOOL)/scripts
	@chown $(NQS_OWNER) $(NQS_SPOOL)/scripts
	@chgrp $(NQS_GROUP) $(NQS_SPOOL)/scripts
	@if [ ! -d  $(NQS_LIBEXE) ] ; then mkdir $(NQS_LIBEXE) ; fi
	@chmod 775 $(NQS_LIBEXE)
	@chown $(NQS_OWNER) $(NQS_LIBEXE)
	@chgrp $(NQS_GROUP) $(NQS_LIBEXE)
	@if [ ! -d  $(MAP_DIRECTORY) ] ; then mkdir $(MAP_DIRECTORY) ; fi
	@chown $(NQS_OWNER) $(MAP_DIRECTORY)
	@chgrp $(NQS_GROUP) $(MAP_DIRECTORY)
	@chmod 755 $(MAP_DIRECTORY)
	@if [ ! -d $(NQS_SPOOL)/conf ] ; then mkdir $(NQS_SPOOL)/conf; fi
	@chmod 755 $(NQS_SPOOL)/conf
	@chown $(NQS_OWNER) $(NQS_SPOOL)/conf
	@chgrp $(NQS_GROUP) $(NQS_SPOOL)/conf
