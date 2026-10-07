# 
# $Copyright
# Copyright 1994, 1995  Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
# $Copyright
# Copyright 1994 Intel Corporation.
#        INTEL CONFIDENTIAL
# Permission to use, copy, modify, and distribute this software and 
# its online documentation for any purpose is hereby granted.
# Intel Coprporation, Inc makes no representations about 
# the suitability of this software for any purpose.
# It is provided "as is" without express or implied warranty.
#
# INTEL CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD 
# TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF 
# MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL INTEL CORPORATION.
# BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
# OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
# CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
# Copyright$
#
  
#  Check for and make if necessary MACS working directories.
#
#  MACSROOT is set on the make line.  If not set, / is assumed.
#
MACS_OWNER	= root
MACS_GROUP	= system
MACS_USR	= $(MACSROOT)/usr
MACS_ETC	= $(MACSROOT)/etc
MACS_USRLIB	= $(MACS_USR)/lib
MACS_USRSPOOL	= $(MACS_USR)/spool
MACS_LOGD	= $(MACS_SPOOL)/log.d
MACS_SPOOL	= $(MACS_USRSPOOL)/macs
MACS_LIBEXE	= $(MACS_USRLIB)/macs
MACS_USREXE	= $(MACS_USR)/bin
MACS_SUSREXE	= $(MACS_USR)/sbin

#
#	MACS directory installation.
#
directories:
	@if [ ! -d $(MACSROOT) ]      ; then mkdir $(MACSROOT)      ; fi
	@if [ ! -d $(MACS_USR) ]      ; then mkdir $(MACS_USR)      ; fi
	@if [ ! -d $(MACS_USREXE) ]   ; then mkdir $(MACS_USREXE)   ; fi
	@if [ ! -d $(MACS_USRLIB) ]   ; then mkdir $(MACS_USRLIB)   ; fi
	@if [ ! -d $(MACS_USRSPOOL) ] ; then mkdir $(MACS_USRSPOOL) ; fi
	@if [ ! -d $(MACS_ETC) ]      ; then mkdir $(MACS_ETC)      ; fi
	@if [ ! -d $(MACS_SPOOL) ]    ; then mkdir $(MACS_SPOOL)    ; fi
	@chmod 755 $(MACS_SPOOL)
	@chown $(MACS_OWNER) $(MACS_SPOOL)
	@chgrp $(MACS_GROUP) $(MACS_SPOOL)
	@if [ ! -d $(MACS_SPOOL)/log.d ] ; then mkdir $(MACS_SPOOL)/log.d ; fi
	@chmod 755 $(MACS_SPOOL)/log.d
	@chown $(MACS_OWNER) $(MACS_SPOOL)/log.d
	@chgrp $(MACS_GROUP) $(MACS_SPOOL)/log.d

	@if [ ! -d $(MACS_LOGD)/macupdlog.d ] ; then mkdir $(MACS_LOGD)/macupdlog.d ; fi
	@chmod 755 $(MACS_LOGD)/macupdlog.d
	@chown $(MACS_OWNER) $(MACS_LOGD)/macupdlog.d
	@chgrp $(MACS_GROUP) $(MACS_LOGD)/macupdlog.d

	@if [ ! -d $(MACS_LOGD)/macdlog.d ] ; then mkdir $(MACS_LOGD)/macdlog.d ; fi
	@chmod 755 $(MACS_LOGD)/macdlog.d
	@chown $(MACS_OWNER) $(MACS_LOGD)/macdlog.d
	@chgrp $(MACS_GROUP) $(MACS_LOGD)/macdlog.d

	@if [ ! -d  $(MACS_SPOOL)/private ] ; then mkdir $(MACS_SPOOL)/private ; fi
	@chmod 700 $(MACS_SPOOL)/private
	@chown $(MACS_OWNER) $(MACS_SPOOL)/private
	@chgrp $(MACS_GROUP) $(MACS_SPOOL)/private
	@if [ ! -d  $(MACS_LIBEXE) ] ; then mkdir $(MACS_LIBEXE) ; fi
	@chmod 775 $(MACS_LIBEXE)
	@chown $(MACS_OWNER) $(MACS_LIBEXE)
	@chgrp $(MACS_GROUP) $(MACS_LIBEXE)
	@if [ ! -d $(MACS_SPOOL)/conf ] ; then mkdir $(MACS_SPOOL)/conf; fi
	@chmod 755 $(MACS_SPOOL)/conf
	@chown $(MACS_OWNER) $(MACS_SPOOL)/conf
	@chgrp $(MACS_GROUP) $(MACS_SPOOL)/conf

