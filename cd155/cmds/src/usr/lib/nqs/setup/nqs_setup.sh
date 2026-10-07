#!/bin/sh
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
#  Setup script for NQS
#
NQSROOT=/ ; export NQSROOT
NQS_LIBEXE=$ROOT/usr/lib/nqs
NQS_SETUP=$NQS_LIBEXE/setup

#
#  Make sure we are root
#
if [ "x`id | grep uid=0`" = "x" ]
then
	echo "You must be root to setup NQS"
	exit 1
fi 

#
#  Add the NQS service to /etc/services if it isn't already there
#
if [ "x`grep 'nqs.*607/tcp' /etc/services`" = "x" ]
then
	echo "nqs\t\t607/tcp\t\tNQS\t#NQS network daemon" >> /etc/services
fi

#
#  Make the basic NQS support directories
#
echo "Making NQS support directories."
make -f $NQS_SETUP/nqs_dirs.mk NQSROOT=$NQSROOT

#
#  Make the spool directories
#
echo "Making NQS spool directories.  This will take a few minutes"
$NQS_SETUP/nqsmkdirs $NQSROOT > /dev/null
$NQS_SETUP/nqsmktrans $NQSROOT > /dev/null

#
#  Set the NQS scheduling parameters
#
$NQS_SETUP/nqs_params

#
#  Let them know they need to run nmapmgr(1M) to use NQS with remote systems.
#
echo "
The nmapmgr(1M) utility must be run to create a database that holds the
names and machine ID numbers of every machine that will be interacting
in the NQS environment.

Would you like the nmapmgr(1M) utility be run now (y/n)? \c"

if [ x"`line`" = "xy" ]
then
	$NQSROOT/usr/bin/nmapmgr
fi

#
#  See if they want to NQS started at boot time.
#
echo "Should NQS be started automatically at system boot (y/n)? \c"

if [ "x`line`" = "xy" ]
then
	ln -s $NQSROOT/sbin/init.d/nqs $NQSROOT/sbin/rc3.d/S89nqs
	ln -s $NQSROOT/sbin/init.d/nqs $NQSROOT/sbin/rc0.d/K11nqs
	ln -s $NQSROOT/sbin/init.d/nqs $NQSROOT/sbin/rc2.d/K06nqs
else
	echo "NQS may be started or stopped manually with the following command:"
	echo
	echo "/sbin/init.d/nqs start"
	echo "and"
	echo "/sbin/init.d/nqs stop"
fi

#
#  See if they want to start NQS now.
#
echo "Do you want to start NQS now (y/n)? \c"

if [ "x`line`" = "xy" ]
then
	/sbin/init.d/nqs start
fi

#
#  Let then know we are done.
#
echo
echo "NQS setup is complete."
exit 0
