#! /bin/sh
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

# Hope is that default loader will echo "default" and other loaders won't.
# So you'll have to install a default loader in /etc/loader, such as the
# echo_loader, that will.

PATH=:$PATH

testpurpose="make sure set gid programs forced to use default loader"

status="STATUS($0:$$)"
error="ERROR($0:$$)"

program=setgidprogram

while [ "$1" != "" ]
do
    case $1 in
    -v)		vflag=1;;
    esac
    shift
done

if [ "$vflag" = 1 ]
then
    echo "$status: $testpurpose"
fi

# We must not be running as root, so test our uid
if [ `getuid` = 0 ]
then
    echo "$error: $0 cannot be run as root"
    echo "$error: FAILED on `date`" 
    exit 1
fi

# Test our effective uid
if [ `geteuid` = 0 ]
then
    echo "$error: $0 cannot be run as root"
    echo "$error: FAILED on `date`" 
    exit 2
fi

# File must be set gid root
setuid="`getsetgid $program`"
if [ $? != 0 -o "$setuid" != 0 ]
then
    echo "$error: file \"$program\" is not set gid root"
    echo "$error: chmod(1) it to set gid root"
    echo "$error: FAILED on `date`" 
    exit 3
fi

exec_with_loader 0 failure_loader $program | grep -s -i default

if [ $? = 0 ]
then 
    if [ "$vflag" = 1 ]
    then
	echo "$status: PASSED on `date`"
    fi
    exit 0
else
    echo "$error: FAILED on `date`" 
    exit 4
fi
