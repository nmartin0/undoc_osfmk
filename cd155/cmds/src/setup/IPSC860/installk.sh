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

#
# Read the configuration information for this host
#
if [ -r setup/PMAX/host.sh ]
then
    . setup/PMAX/host.sh
else
    . setup/PMAX/osf1.sh
fi

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
exp=${base}/export
if [ -d ${exp} ]
then
    true
else
    echo "Please create the path for ${exp}"
    exit 1
fi
expdir=${exp}/${target_machine}
if [ -d ${expdir} ]
then
    true
else
    echo "Please create the path for ${expdir}"
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
hostbindir=${toolsdir}/hostbin
if [ -d ${hostbindir} ]
then
    true
else
    echo "Please create the path for ${hostbindir}"
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
gccdir=${toolsdir}/${gccsuffix}
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
PATH="${hostbindir}:${bindir}:${gccdir}:/usr/ucb:/bin:/usr/bin"

export PATH

#
# Site/Environment stuff
#
SITE="OSF"
OWNER="root"
GROUP="system"

export SITE OWNER GROUP

#
# New build environment definitions
#
MAKEFILEPATH='${MAKETOP}usr/lib/makefiles'
EXPORTBASE=${expdir}
SOURCEBASE=${srcdir}
SOURCEDIR=""
OBJECTDIR=../obj/${target_machine}

export MAKEFILEPATH EXPORTBASE SOURCEBASE SOURCEDIR OBJECTDIR

#
# install targets
#

USAGE="installk.sh <targetroot> <configuration_name>"

if [ $# != 2 ]
then
    echo "Incorrect number of command line options."
    echo $USAGE
    exit 1
fi
TOSTAGE=$1
CONFIG=$2
CONFIGDIR=${OBJECTDIR}/kernel/${CONFIG}

if [ ! -d $TOSTAGE ]
then
    echo "Directory ${TOSTAGE} not found.  Please correct and rerun."
    echo $USAGE
    exit 1
fi

if [ ! -d ${CONFIGDIR} ]
then
    echo "Configuration ${CONFIG} not found.  Please correct and rerun."
    echo $USAGE
    exit 1
fi


echo "---------------------------------------------------------------"
echo ""

echo "Copying ${CONFIG}/vmunix to ${TOSTAGE}/vmunix.OSF1.${CONFIG}"
cp ${CONFIGDIR}/vmunix ${TOSTAGE}/vmunix.OSF1.${CONFIG}
echo "Linking ${TOSTAGE}/vmunix.OSF1.${CONFIG} to vmunix"
ln ${TOSTAGE}/vmunix.OSF1.${CONFIG} ${TOSTAGE}/vmunix


echo "---------------------------------------------------------------"
echo ""
echo " copying kernel modules"
echo ""

if [ -f $CONFIGDIR/*_kmod ]
then
  if [ ! -d ${TOSTAGE}/sbin/subsys ]
  then
    makepath ${TOSTAGE}/sbin/subsys/
  fi
  cp ${CONFIGDIR}/*_kmod ${TOSTAGE}/sbin/subsys
fi

echo ""
echo "Kernel install complete.  Use vmunix when you reboot."
exit 0
