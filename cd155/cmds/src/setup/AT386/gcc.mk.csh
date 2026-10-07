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
if (  ! -d ../tools/at386/i386cc ) then
    echo "Please create the path for ../tools/at386/i386cc"
    exit 1
endif
if ( ! -d ../tools/at386/lib ) then
    echo "Please create the path for ../tools/at386/lib"
    exit 1
endif
cp setup/AT386/bootstrap.gcc.sh usr/local/gnu-tools/gcc
(cd usr/local/gnu-tools/gcc; sh bootstrap.gcc.sh)
cp usr/local/gnu-tools/gcc/gcc ../tools/at386/i386cc/gcc
cp usr/local/gnu-tools/gcc/cpp ../tools/at386/i386cc/cpp
cp usr/local/gnu-tools/gcc/cc1 ../tools/at386/i386cc/cc1
gcc -c setup/AT386/dummy.c
ar rc ../tools/at386/i386cc/gnulib dummy.o


