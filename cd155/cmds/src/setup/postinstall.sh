#! /bin/sh
#  
#  $Copyright
#  Copyright 1993, 1994, 1995  Intel Corporation
#  INTEL CONFIDENTIAL
#  The technical data and computer software contained herein are subject
#  to the copyright notices; trademarks; and use and disclosure
#  restrictions identified in the file located in /etc/copyright on
#  this system.
#  Copyright$
#  
 
#
# (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
# $Id: postinstall.sh,v 1.84.2.2 1995/06/15 17:21:16 kat Exp $
#
# OSF/1 Release 1.0.2
#
# HISTORY
# $Log: postinstall.sh,v $
#  Revision 1.84.2.2  1995/06/15  17:21:16  kat
#  removed execution of raidinstall
#
#  Revision 1.84.2.1  1995/06/13  17:00:08  kat
#  Removed instructions for removing crypt.o from libc.a and libc_r.a
#
#  Revision 1.84  1995/03/21  01:05:04  stans
#   Symlink /sbin/init.d/mklog to ./sbin/rc3.d/S09mklog for starting remote
#   Micro-kernel console logger.
#
#   Reviewer:cfleck
#   Risk:medium
#   Benefit or PTS #:12747
#   Testing:        WW10 sats
#          Developer tests:
#                  Verified mark feature of syslog is working.
#                  Verified syslog, without mark feature, is working.
#                  Verified console logging is not broken.
#
#  Revision 1.83  1994/11/21  16:48:43  mtm
#  Copyright additions/changes
#
#  Revision 1.82  1994/09/06  21:55:41  shane
#   Reviewer: None
#   Risk: Low
#   Benefit or PTS #: 10688
#   Testing: None
#   Module(s):Postinstall.sh
#
#  Revision 1.81  1994/06/16  17:57:12  dleslie
#   Reviewer: Shala Arshi
#   Risk: low
#   Benefit or PTS #: autoinit.o and __denormal.o are installed in the
#  	correct place by the appropriate Makefiles.  postinstall.sh
#  	no longer has to move them. This will help the patch process
#  	know where these files are supposed to go.
#   Testing: verified with 'cmake -n install_all' that the files will be
#  	installed correctly
#   Module(s):
#  	setup/postinstall.sh
#  	usr/ccs/lib/libc/Makefile
#  	usr/ccs/lib/libnx/Makefile
#
#  Revision 1.80  1994/06/09  18:57:59  hobbes
#  Removed the link for /sbin/init.d/hippi ..
#  this is no longer needed for R1.3
#
#  Revision 1.79  1994/05/31  23:20:14  shala
#   Reviewer: NONE
#   Risk: Low
#   Benefit or PTS #: Bug #8932
#   Testing:
#   Module(s): cmds_libs/setup/postinstall.sh
#
#  Roll in the fix from R1.2 into main trunk. Fixed the links for spv startup.
#
#  Revision 1.78  1994/05/31  21:17:23  shala
#   Reviewer: NONE
#   Risk: Low
#   Benefit or PTS #: Bug #9426
#   Testing:
#   Module(s): cmds_libs/src/setup/postinstall.sh
#
#  Since "install" script creates directory "/pfs", there is no need to
#  include it in the root tar file.
#
#  Revision 1.77  1994/03/19  00:22:05  shala
#   Reviewer: Charlie Johnson
#   Risk: Low.
#   Benefit or PTS #: Bugs # 8450 - The startup of "ntpd" daemon should not be
#               default. It should be configuration option and be selected
#               during "postboot". The "ntpd" has impact on the system,
#               since it sends message to all the nodes in the system every
#               5 seconds. This should be communicated to the user before
#               making "ntpd" started on the system.
#   Testing: Used the new postinstall and postboot to make sure the ntpd
#               does not started automatically and can be configured as an
#               option during "postboot".
#   Module(s): cmds_libs/src/setup/postinstall.sh
#              cmds_libs/src/setup/postboot
#
#  Revision 1.76  1994/01/10  20:26:11  shala
#   Reviewer: NONE
#   Risk: Low
#   Benefit or PTS #: Regular user be able to see bootmagic.
#   Testing: Tested "getmagic" with new permissions as regular user and root.
#   Module(s): src/setup/postinstall.sh
#
#  Set effective user id of root for getmagic command.
#
#  Revision 1.75  1993/11/30  23:21:44  shala
#   Reviewer: NONE
#   Risk: LOW
#   Benefit or PTS #: Security bug. Bug #7184.
#   Testing: Ran the script.
#   Module(s): src/setup/postinstall.sh
#
#  Change owner from "bin" to "root" and group from "bin" to "system".
#
#  Revision 1.74  1993/11/30  17:33:10  shala
#   Reviewer: NONE
#   Risk: LOW
#   Benefit or PTS #: Cleanup
#   Testing: Ran nm860 to make sure extras are gone.
#   Module(s): src/postinstall.sh
#
#  Revision 1.73  1993/11/03  19:48:48  stans
#     Replace lost edits: 1.67 thru 1.70
#     Removed:
#  	'/paging' directory; not used.
#  	rc2.d/S05paging	now done in bcheckrc
#     Added:
#  	rc2.d/S05amd
#  	CVS History and Id.
#
#    Reviewer(s):  Shala
#    Risk:         Low
#    Benefit:      "amd" monitor(s) startup as soon as possible in the boot
#  		process. Vnode pagers started ASAP from bcheckrc instead
#  		of "rc2.d/".
#    Testing:      booted two Paragons.
#    Module(s):    rc2.d/
#
#

echo 
echo "======================================================================="
echo "                  OSF/1 Rev 1.0 Postinstall Process"
echo

if [ $# = 0 ]
then
  echo "ERROR: Incorrect number of arguments:"
  echo $USAGE
  exit 1
fi

INSTALLBASE=$1

if [ "$INSTALLBASE" = ""  ]
then
  echo "ERROR: no <targetroot> specified"
  echo $USAGE
  exit 1
fi

cd ${INSTALLBASE}

rm -rf usr/include/nqs
rm -rf usr/ccs/lib/libnqs
cp usr/bin/awk sbin/awk
cp usr/bin/sed sbin/sed
chmod 755 sbin/bootmesh.sh
chown 0 sbin/bootmesh.sh
chgrp 0 sbin/bootmesh.sh
chmod 4755 sbin/getmagic
#
#  This is now run during the raid install phase of the build.
#./raidinstall

if [ ! -f etc/exports ]
then
  touch etc/exports
fi

echo "Making additional directories..."

dirlist="var/mail var/news var/preserve var/tmp \
	 var/adm/crash var/spool/crontab var/named var/spool/cron/atjobs \
	 var/spool/lpd var/spool/mail var/spool/mqueue var/spool/uucp \
	 var/spool/uucp/.Workspace var/run var/rwho var/adm/syslog \
	 usr/share/man home mnt tmp usr/tmp sbin/rc0.d sbin/rc2.d sbin/rc3.d"

for d in $dirlist
do
  if [ ! -d $d ]
  then
    mkdir $d
  fi
done

echo "Symbolic links will now be created..."

echo " - Links for usr/... to var/... files"
ln -s ../var/adm usr/adm
ln -s ../var/mail usr/mail
ln -s ../var/news usr/news
ln -s ../var/preserve usr/preserve
ln -s ../var/spool usr/spool
#ln -s ../tmp usr/tmp

#
# Links for init/rc files
#
echo " - Links for init/rc files"
ln -s ../init.d/enlogin  	./sbin/rc0.d/K00enlogin
ln -s ../init.d/lpd  		./sbin/rc0.d/K05lpd
ln -s ../init.d/mach  		./sbin/rc0.d/K07mach
ln -s ../init.d/loadlevel    	./sbin/rc0.d/K8loadlevel
ln -s ../init.d/amd 		./sbin/rc0.d/K10amd
# place holder for nqs          ./sbin/rc0.d/K11nqs
# place holder for macs         ./sbin/rc0.d/K12nqs
ln -s ../init.d/smd 		./sbin/rc0.d/K13smd
ln -s ../init.d/spv  	  	./sbin/rc0.d/K14spv
ln -s ../init.d/allocator 	./sbin/rc0.d/K15allocator
ln -s ../init.d/inetd  		./sbin/rc0.d/K25inetd
ln -s ../init.d/snmpd  		./sbin/rc0.d/K30snmpd
ln -s ../init.d/ntpd  		./sbin/rc0.d/XK35ntpd
ln -s ../init.d/sendmail  	./sbin/rc0.d/K40sendmail
ln -s ../init.d/nfs  		./sbin/rc0.d/K45nfs
ln -s ../init.d/nfsmount  	./sbin/rc0.d/K50nfsmount
ln -s ../init.d/named  		./sbin/rc0.d/K55named
ln -s ../init.d/syslog  	./sbin/rc0.d/K60syslog
ln -s ../init.d/inet  		./sbin/rc0.d/K65inet
ln -s ../init.d/cron 		./sbin/rc0.d/K70cron
#
ln -s ../init.d/lpd  		./sbin/rc2.d/K00lpd
ln -s ../init.d/mach  		./sbin/rc2.d/K03mach
ln -s ../init.d/loadlevel    	./sbin/rc2.d/K4loadlevel
ln -s ../init.d/amd 		./sbin/rc2.d/K05amd
# place holder for nqs          ./sbin/rc2.d/K06nqs
# place holder for macs         ./sbin/rc2.d/K07nqs
ln -s ../init.d/smd 		./sbin/rc2.d/K08smd
ln -s ../init.d/spv             ./sbin/rc2.d/K09spv
ln -s ../init.d/allocator 	./sbin/rc2.d/K10allocator
ln -s ../init.d/inetd  		./sbin/rc2.d/K20inetd
ln -s ../init.d/snmpd  		./sbin/rc2.d/K25snmpd
ln -s ../init.d/ntpd  		./sbin/rc2.d/XK30ntpd
ln -s ../init.d/cron    	./sbin/rc2.d/K35cron
ln -s ../init.d/sendmail  	./sbin/rc2.d/K40sendmail
ln -s ../init.d/nfs  		./sbin/rc2.d/K45nfs
ln -s ../init.d/nfsmount  	./sbin/rc2.d/K50nfsmount
ln -s ../init.d/named  		./sbin/rc2.d/K55named
ln -s ../init.d/syslog  	./sbin/rc2.d/K60syslog
ln -s ../init.d/inet  		./sbin/rc2.d/K65inet
#ln -s ../init.d/savecore  	./sbin/rc2.d/S00savecore
ln -s ../init.d/amd    		./sbin/rc2.d/S05amd
ln -s ../init.d/recpasswd  	./sbin/rc2.d/S10recpasswd
ln -s ../init.d/uucp  		./sbin/rc2.d/S15uucp
ln -s ../init.d/enlogin  	./sbin/rc2.d/S20enlogin
#
ln -s ../init.d/inet    	./sbin/rc3.d/S00inet
ln -s ../init.d/settime    	./sbin/rc3.d/S05settime
ln -s ../init.d/mklog    	./sbin/rc3.d/S09mklog
ln -s ../init.d/syslog    	./sbin/rc3.d/S10syslog
ln -s ../init.d/named    	./sbin/rc3.d/S15named
ln -s ../init.d/nfsmount  	./sbin/rc3.d/S20nfsmount
ln -s ../init.d/loader  	./sbin/rc3.d/S25loader
ln -s ../init.d/preserve  	./sbin/rc3.d/S30preserve
ln -s ../init.d/rmtmpfiles  	./sbin/rc3.d/S35rmtmpfiles
ln -s ../init.d/nfs   	 	./sbin/rc3.d/S40nfs
ln -s ../init.d/sendmail  	./sbin/rc3.d/S45sendmail
ln -s ../init.d/ntpd    	./sbin/rc3.d/XS50ntpd
ln -s ../init.d/snmpd    	./sbin/rc3.d/S55snmpd
ln -s ../init.d/inetd    	./sbin/rc3.d/S60inetd
ln -s ../init.d/cron    	./sbin/rc3.d/S65cron
#ln -s ../init.d/motd    	./sbin/rc3.d/S70motd
ln -s ../init.d/lpd    		./sbin/rc3.d/S75lpd
ln -s ../init.d/mach  		./sbin/rc3.d/S77mach
ln -s ../init.d/initpart    	./sbin/rc3.d/S80initpart
ln -s ../init.d/allocator    	./sbin/rc3.d/S85allocator
ln -s ../init.d/spv             ./sbin/rc3.d/S86spv
ln -s ../init.d/smd	    	./sbin/rc3.d/S87smd
# place holder for nqs          ./sbin/rc3.d/S88nqs
# place holder for nqs          ./sbin/rc3.d/S89nqs
ln -s ../init.d/loadlevel    	./sbin/rc3.d/S95loadlevel
ln -s /postboot1    		./sbin/rc3.d/S99postboot1
#
# Link compiler components to common locations from /usr/ccs
#
echo " - Links for compile tools"
ln -s ../ccs/bin/ar     ./usr/bin/ar
ln -s ../ccs/bin/ar     ./usr/bin/ar860
ln -s ../ccs/bin/as     ./usr/bin/as
ln -s ../ccs/bin/as     ./usr/bin/as860
ln -s ../ccs/bin/cb     ./usr/bin/cb
ln -s ../ccs/bin/cc     ./usr/bin/cc
ln -s ../ccs/bin/cc     ./usr/bin/icc
ln -s ../ccs/bin/cflow     ./usr/bin/cflow
ln -s  ../ccs/bin/cpp  ./usr/lib/cpp
ln -s ../ccs/bin/cxref     ./usr/bin/cxref
ln -s ../ccs/bin/dump860     ./usr/bin/dump860
ln -s ../ccs/bin/error     ./usr/bin/error
ln -s ../ccs/bin/f77     ./usr/bin/f77
ln -s ../ccs/bin/f77     ./usr/bin/if77
ln -s ../ccs/bin/ic    ./usr/bin/ic
ln -s ../ccs/bin/ic_ex    ./usr/bin/ic_ex
ln -s ../ccs/bin/ifixlib     ./usr/bin/ifixlib
ln -s ../ccs/bin/iftn     ./usr/bin/iftn
ln -s ../ccs/bin/iftn_ex     ./usr/bin/iftn_ex
ln -s ../ccs/bin/indent   ./usr/bin/indent
ln -s ../ccs/bin/ld     ./usr/bin/ld
ln -s ../ccs/bin/ld     ./usr/bin/ld860
ln -s ../ccs/bin/lex     ./usr/bin/lex
ln -s ../ccs/bin/lint     ./usr/bin/lint
ln -s ../ccs/bin/lorder   ./usr/bin/lorder
ln -s ../ccs/bin/m4     ./usr/bin/m4
ln -s ../ccs/bin/mac     ./usr/bin/mac
ln -s ../ccs/bin/mac     ./usr/bin/mac860
ln -s ../ccs/bin/make     ./usr/bin/make
ln -s ../ccs/bin/mig     ./usr/bin/mig
ln -s ../ccs/bin/mkstr     ./usr/bin/mkstr
ln -s ../ccs/bin/nm     ./usr/bin/nm
ln -s ../ccs/bin/nm     ./usr/bin/nm860
ln -s ../ccs/bin/size     ./usr/bin/size
ln -s ../ccs/bin/size     ./usr/bin/size860
ln -s ../ccs/bin/strings   ./usr/bin/strings
ln -s ../ccs/bin/strip   ./usr/bin/strip
ln -s ../ccs/bin/strip   ./usr/bin/strip860
ln -s ../ccs/bin/tsort     ./usr/bin/tsort
#ln -s ../ccs/bin/xpass     ./usr/bin/xpass
ln -s ../ccs/bin/xstr     ./usr/bin/xstr
ln -s ../ccs/bin/yacc     ./usr/bin/yacc
#
# Link /usr/lib/* to /usr/ccs/lib/*
#
echo " - Link libraries in /usr/ccs/lib to /usr/lib"
if [ ! -d ${INSTALLBASE}/usr/ccs/lib/options ]; then
   mkdir ${INSTALLBASE}/usr/ccs/lib/options
fi
if [ ! -d ${INSTALLBASE}/usr/ccs/lib/noieee ]; then
   mkdir ${INSTALLBASE}/usr/ccs/lib/noieee
fi
if [ ! -d ${INSTALLBASE}/usr/lib/noieee ]; then
   mkdir ${INSTALLBASE}/usr/lib/noieee
fi
mkdir ${INSTALLBASE}/etc/load_level/shm_keys
chmod 755 ${INSTALLBASE}/etc/load_level/shm_keys
chown 0 ${INSTALLBASE}/etc/load_level/shm_keys
chgrp 0 ${INSTALLBASE}/etc/load_level/shm_keys
ln -s ../ccs/lib/options ./usr/lib/options
ln -s ../../ccs/lib/noieee/__denormal.o ./usr/lib/noieee/__denormal.o
ln -s ../ccs/lib/crt0.o ./usr/lib/crt0.o
ln -s ../ccs/lib/guard.o ./usr/lib/guard.o
ln -s ../ccs/lib/libic.a ./usr/lib/libic.a
ln -s ../ccs/lib/ifmain.o ./usr/lib/ifmain.o
ln -s  ../ccs/lib/libAF.a  ./usr/lib/libAF.a
ln -s  ../ccs/lib/libPW.a  ./usr/lib/libPW.a
ln -s  ../ccs/lib/libbsd.a  ./usr/lib/libbsd.a
ln -s  ../ccs/lib/libc.a  ./usr/lib/libc.a
ln -s  ../ccs/lib/libc_r.a  ./usr/lib/libc_r.a
ln -s  ../ccs/lib/libcs.a  ./usr/lib/libcs.a
ln -s  ../ccs/lib/libcthreads.a  ./usr/lib/libcthreads.a
ln -s  ../ccs/lib/libcurses.a  ./usr/lib/libcurses.a
ln -s  ../ccs/lib/libdbm.a  ./usr/lib/libdbm.a
ln -s  ../ccs/lib/libenv.a  ./usr/lib/libenv.a
ln -s  ../ccs/lib/libf.a  ./usr/lib/libf.a
ln -s  ../ccs/lib/libg.a  ./usr/lib/libg.a
ln -s  ../ccs/lib/libhippi.a  ./usr/lib/libhippi.a
ln -s  ../ccs/lib/libkmath.a  ./usr/lib/libkmath.a
ln -s  ../ccs/lib/libl.a  ./usr/lib/libl.a
#ln -s  ../ccs/lib/libc_g.a  ./usr/lib/libc_g.a
ln -s  ../ccs/lib/libld.a  ./usr/lib/libld.a
ln -s  ../ccs/lib/libln.a  ./usr/lib/libln.a
ln -s  ../ccs/lib/libm.a  ./usr/lib/libm.a
ln -s  ../ccs/lib/libmach.a  ./usr/lib/libmach.a
ln -s  ../ccs/lib/libmach.a  ./usr/lib/libmach3.a
ln -s  ./libmach.a  ./usr/ccs/lib/libmach3.a
ln -s  ../ccs/lib/libmachid.a  ./usr/lib/libmachid.a
ln -s  ../ccs/lib/libmp.a  ./usr/lib/libmp.a
ln -s  ../ccs/lib/libnetname.a  ./usr/lib/libnetname.a
ln -s  ../ccs/lib/libnx.a  ./usr/lib/libnx.a
ln -s  ../ccs/lib/libpm.o  ./usr/lib/libpm.o
ln -s  ../ccs/lib/pgfalt.o  ./usr/lib/pgfalt.o
ln -s  ../ccs/lib/libpthreads.a  ./usr/lib/libpthreads.a
ln -s  ../ccs/lib/libresolv.a  ./usr/lib/libresolv.a
ln -s  ../ccs/lib/librpc.a  ./usr/lib/librpc.a
ln -s  ../ccs/lib/libservice.a  ./usr/lib/libservice.a
#ln -s  ../ccs/lib/libsecurity.a  ./usr/lib/libsecurity.a
ln -s  ../ccs/lib/libsys5.a  ./usr/lib/libsys5.a
ln -s  ../ccs/lib/libtermcap.a  ./usr/lib/libtermcap.a
ln -s  ../ccs/lib/libtermlib.a  ./usr/lib/libtermlib.a
ln -s  ../ccs/lib/libtli.a  ./usr/lib/libtli.a
ln -s  ../ccs/lib/libutil.a  ./usr/lib/libutil.a
ln -s  ../ccs/lib/libxti.a  ./usr/lib/libxti.a
ln -s  ../ccs/lib/liby.a  ./usr/lib/liby.a
ln -s  ../ccs/lib/mcrt0.o  ./usr/lib/mcrt0.o
ln -s  ../ccs/lib/yaccpar  ./usr/lib/yaccpar
#
# Set up mail links and set directory protection
#
chgrp 6 ./usr/spool/mail
chmod 775 ./usr/spool/mail
echo " - Link mail program"
ln -s  ../sbin/sendmail  ./usr/lib/sendmail
#
# Shared directory structure
#
echo " - Link to /usr/shared"
#ln -s  ../share/lib/adb    ./usr/lib/adb
ln -s  ../share/lib/lib.b  ./usr/lib/lib.b
#ln -s  ../share/lib/macros  ./usr/lib/macros
ln -s  ../share/lib/me    ./usr/lib/me
ln -s  ../share/lib/more.help  ./usr/lib/more.help
ln -s  ../share/lib/ms    ./usr/lib/ms
#ln -s  ../share/lib/nterm  ./usr/lib/nterm
ln -s  ../share/lib/term  ./usr/lib/term
ln -s  ../share/lib/terminfo  ./usr/lib/terminfo
ln -s  ../share/lib/tabset  ./usr/lib/tabset
ln -s  ../usr/share/lib/termcap  ./etc/termcap
ln -s  ../share/lib/tmac  ./usr/lib/tmac
ln -s  share/man    ./usr/man

#
# Links from root directories to /usr directories
#

ln -s  usr/lib    ./lib
ln -s  usr/bin    ./bin
ln -s  bin        usr/ucb
#ln -s  ../stand   usr/mdec

touch var/adm/sendmail/sendmail.cw
touch var/adm/sendmail/sendmail.st
touch var/adm/wtmp

echo "************************************************************************"
echo "Post Install done.  Please remember to do the following before trying"
echo "to reboot."
echo
echo "  - review and edit if needed etc/rc.config and etc/fstab"
echo "  - run installk.sh to install a kernel if not already done"
echo "  - Install the boot program for your computer on the root partition"
echo ""
echo "After you boot OSF/1 into multiuser mode you will need to run"
echo "/postboot program to create the databases that can only be created when"
echo "OSF/1 is running.  The postboot program only has to be run once and"
echo "then can be deleted."
echo 
exit 0
