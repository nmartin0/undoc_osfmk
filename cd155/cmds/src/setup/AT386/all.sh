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
# (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
#
# OSF/1 Release 1.0

if [ $# != 2 ]
then
    echo "all.sh <script dir> <log dir>"
    exit 1
fi
/bin/sh -x $1/setup.sh  > $2/setup.log  2>&1
/bin/sh -x $1/kernel.sh > $2/kernel.log 2>&1 &
sleep 60
/bin/sh -x $1/cmds.sh   > $2/cmds.log   2>&1 &
sleep 60
/bin/sh -x $1/tools.sh  > $2/tools.log  2>&1 &

exit 0
