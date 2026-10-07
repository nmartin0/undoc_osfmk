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
# echo_loader that will.

PATH=:$PATH

testpurpose="test that call to default loader works"

status="STATUS($0:$$)"
error="ERROR($0:$$)"

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

exec_with_loader 0 0 /bin/ls | grep -s -i default

if [ $? = 0 ]
then 
    if [ "$vflag" = 1 ]
    then
	echo "$status: PASSED on `date`"
    fi
    exit 0
else
    echo "$error: FAILED on `date`" 
    exit 1
fi
