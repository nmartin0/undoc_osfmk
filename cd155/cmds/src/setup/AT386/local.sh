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

#
# Setup variables describing the current environment
#
MACHINE="i386"
TARGET_MACHINE="AT386"
target_machine="at386"
CPUTYPE="I386"
cputype="i386"

export MACHINE TARGET_MACHINE target_machine CPUTYPE cputype
#
# Setup immediate directory heirarchy
#
srcdir=`pwd`
base=`expr "${srcdir}" : "\(.*\)/.*"`
if [ `basename "${srcdir}"` != "src" ]
then
    echo "Please move source tree into a \"src\" subdirectory"
    exit 1
fi
obj=${base}/obj
if [ -d ${obj} ]
then
    true
else
    echo "Please create the path for ${obj}"
    exit 1
fi
objdir=${obj}/${target_machine}
if [ -d ${objdir} ]
then
    true
else
    echo "Please create the path for ${objdir}"
    exit 1
fi
tools=${base}/tools
if [ -d ${tools} ]
then
    true
else
    echo "Please create the path for ${tools}"
    exit 1
fi
toolsdir=${tools}/${target_machine}
if [ -d ${toolsdir} ]
then
    true
else
    echo "Please create the path for ${toolsdir}"
    exit 1
fi
bindir=${toolsdir}/bin
if [ -d ${bindir} ]
then
    true
else
    echo "Please create the path for ${bindir}"
    exit 1
fi
libdir=${toolsdir}/lib
if [ -d ${libdir} ]
then
    true
else
    echo "Please create the path for ${libdir}"
    exit 1
fi
gccdir=${toolsdir}/gcc
if [ -d ${gccdir} ]
then
    true
else
    echo "Please create the path for ${gccdir}"
    exit 1
fi

#
# Constrain search paths
#
PATH="${bindir}:${gccdir}:/usr/ucb:/bin:/usr/bin"
CPATH="/usr/cs/include:/usr/include"
LPATH="/usr/cs/lib:/lib:/usr/lib"
INCDIRS="-I${objdir}/usr/include"
LIBDIRS="-L${objdir}/usr/ccs/lib/libc"
MIGCOM=${libdir}/migcom
YACCPAR=${libdir}/yaccpar

export PATH CPATH LPATH INCDIRS LIBDIRS MIGCOM YACCPAR

GCC_EXEC_PREFIX="${gccdir}/"

export GCC_EXEC_PREFIX

#
# Site/Environment stuff
#
SITE="OSF"
OWNER="bin"
GROUP="bin"

export SITE OWNER GROUP

#
# New build environment definitions
#
MAKEFILE_COMMON='${MAKETOP}/Makefile-common'
SOURCEBASE=${srcdir}
SOURCEDIR=""
OBJECTDIR=../obj/at386

export MAKEFILE_COMMON SOURCEBASE SOURCEDIR OBJECTDIR

#
# Define this if your machine uses COFF instead of a.out
#
USES_COFF="COFF"

export USES_COFF

#
# build local directories
#
for dir in usr/local/*
do
    if [ -d $dir  ]
    then
	(cd $dir; make -cF build_all)
    fi
done

exit 0
