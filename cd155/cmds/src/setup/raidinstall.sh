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
 

# installation script for the RAID utilities. sets ownership and permission.

ROOTDIR=`pwd`

#raid utility binaries
cd ${ROOTDIR}/sbin
chown root ace acf arc apc apr can inquire
chgrp 0 ace acf arc apc apr can inquire
chmod 700 ace acf arc apc apr can
chmod 755 inquire

cd ${ROOTDIR}/sbin/init.d
chown root amd
chgrp 0 amd
chmod 755 amd

cd ${ROOTDIR}/usr/sbin
chown root amd
chgrp 0 amd
chmod 700 amd

#raid utility log files
cd ${ROOTDIR}/var
chown root array
chgrp 0 array
chmod 755 array

cd ${ROOTDIR}/var/array
chown root logs
chgrp 0 logs
chmod 755 logs


#raid utility configuration files
cd ${ROOTDIR}/etc
chown root default array
chgrp 0 default array
chmod 755 default array

cd ${ROOTDIR}/etc/default
chown root acf 
chgrp 0 acf 
chmod 644 acf 

cd ${ROOTDIR}/etc/array
chown root cc acehelp
chgrp 0 cc acehelp
chmod 755 cc acehelp

cd ${ROOTDIR}/etc/array/cc
chown root ccf_?
chgrp 0 ccf_?
chmod 700 ccf_?

#raid utility help files
cd ${ROOTDIR}/etc/array/acehelp
chown root *.HLP
chgrp 0 *.HLP
chmod 644 *.HLP
