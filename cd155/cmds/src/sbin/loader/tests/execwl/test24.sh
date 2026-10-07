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

PATH=:$PATH

testpurpose="test #! argument processing with symbolic link to shell"

status="STATUS($0:$$)"
error="ERROR($0:$$)"

script=script.$$
expected=expected.$$
results=results.$$
symlink=symlink.$$

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

# Build the symbolic link
ln -s /bin/sh $symlink

# Build the shell script
> $script
echo '#!' $symlink >> $script
chmod +x $script

#Build expected argv
> $expected
echo "sh" >> $expected
echo "$script" >> $expected
echo "second" >> $expected
echo "third" >> $expected
echo "fourth" >> $expected

exec_with_loader 0 print_argv_loader $script first second third fourth > $results
cmp $expected $results

if [ $? = 0 ]
then 
    rm -f $script $expected $results $symlink
    if [ "$vflag" = 1 ]
    then
	echo "$status: PASSED on `date`"
    fi
    exit 0
else
    echo "$error: FAILED on `date`" 
    exit 1
fi
