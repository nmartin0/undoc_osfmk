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
# Read the configuration information for this host
#
if [ -r setup/IPSC860/host.sh ]
then
    . setup/IPSC860/host.sh
else
    . setup/IPSC860/osf1.sh
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
obj=${base}/hostobj
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
exp=${base}/hostexp
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

#
# Constrain search paths
#
PATH="${hostbindir}:${bindir}:/usr/ucb:/bin:/usr/bin"
MIGCOM="${libdir}/migcom"
LEXER="${libdir}/ncform"
YACCPAR="${libdir}/yaccpar"

export PATH MIGCOM LEXER YACCPAR

CCTYPE=host

export CCTYPE

#
# Site/Environment stuff
#
CENV="-UMSG -D_BLD"
SITE="OSF"
OWNER="root"
GROUP="system"

export CENV SITE OWNER GROUP

#
# New build environment definitions
#
MAKEFILEPATH='${MAKETOP}usr/lib/makefiles'
EXPORTBASE=${expdir}
SOURCEBASE=${srcdir}
SOURCEDIR=""
OBJECTDIR=../hostobj/${target_machine}

export MAKEFILEPATH EXPORTBASE SOURCEBASE SOURCEDIR OBJECTDIR

#
# Sandbox include files
#
(cd usr/include/sdm; make -ckEF export_all)

#
# Sandbox library
#
NEED_FOR_PORTING=
PORTING_CARGS=
if [ -n "$NO_VFPRINTF" ]
then
    NEED_FOR_PORTING="vfprintf.o vsprintf.o"
    PORTING_CARGS="-D_DOPRNT_IS_VISIBLE"
fi
if [ -n "$NO_SETENV" ]
then
    NEED_FOR_PORTING="$NEED_FOR_PORTING environment.o"
fi
(cd usr/ccs/lib/libsb; \
 make -ckEF \
   INCARGS="-I. -I${expdir}/usr/include" \
   par_rc_file.o_CARGS=-DMACHINE=\\\"${MACHINE}\\\" \
   NEED_FOR_PORTING="${NEED_FOR_PORTING}" \
   CARGS="${PORTING_CARGS}" \
   export_all)

#
# create native version of sdm tools
#
CENV=
if [ -n "$NO_PW_STAYOPEN" ]
then
    CENV="-DNO_PW_STAYOPEN"
fi
if [ -n "$NO_DIRENT" ]
then
    CENV="$PORTING_CENV -DNO_DIRENT"
fi
INCENV="-I. -I${expdir}/usr/include"
LIBENV="-L${expdir}/usr/ccs/lib"
export CENV INCENV LIBENV

(cd usr/local/sdm/bin; make -ckEF build_all)
cp ${objdir}/usr/local/sdm/bin/bcs/bci ${bindir}
cp ${objdir}/usr/local/sdm/bin/bcs/bco ${bindir}
cp ${objdir}/usr/local/sdm/bin/bcs/bcreate ${bindir}
cp ${objdir}/usr/local/sdm/bin/bcs/bcs ${bindir}
cp ${objdir}/usr/local/sdm/bin/bcs/bdiff ${bindir}
cp ${objdir}/usr/local/sdm/bin/bcs/blog ${bindir}
cp ${objdir}/usr/local/sdm/bin/bcs/bmerge ${bindir}
cp ${objdir}/usr/local/sdm/bin/bcs/bstat ${bindir}
cp ${objdir}/usr/local/sdm/bin/bsubmit/bsubmit ${bindir}
cp ${objdir}/usr/local/sdm/bin/bsubmit/sadmin ${bindir}
cp ${objdir}/usr/local/sdm/bin/build/build ${bindir}
cp ${objdir}/usr/local/sdm/bin/currentsb/currentsb ${bindir}
cp ${objdir}/usr/local/sdm/bin/find/find ${bindir}
cp ${objdir}/usr/local/sdm/bin/genpath/genpath ${bindir}
cp ${objdir}/usr/local/sdm/bin/makepath/makepath ${bindir}
cp ${objdir}/usr/local/sdm/bin/md/md ${bindir}
cp ${objdir}/usr/local/sdm/bin/mklinks/mklinks ${bindir}
cp ${objdir}/usr/local/sdm/bin/mksb/mksb ${bindir}
cp ${objdir}/usr/local/sdm/bin/resb/resb ${bindir}
cp ${objdir}/usr/local/sdm/bin/sbinfo/sbinfo ${bindir}
cp ${objdir}/usr/local/sdm/bin/uptodate/uptodate ${bindir}
cp ${objdir}/usr/local/sdm/bin/wh/wh ${bindir}
cp ${objdir}/usr/local/sdm/bin/workon/workon ${bindir}

CENV=
INCENV=
LIBENV=
export CENV INCENV LIBENV

PORTING_CARGS=
if [ -n "$NO_VFPRINTF" ]
then
    PORTING_CARGS="-DNO_VFPRINTF"
fi
(cd usr/local/rcs/bin; \
 make -ckEF INCARGS="-I. -I${expdir}/usr/include" \
   CARGS="${PORTING_CARGS}" \
   build_all)
cp ${objdir}/usr/local/rcs/bin/branchmerge ${bindir}
cp ${objdir}/usr/local/rcs/bin/ci ${bindir}
cp ${objdir}/usr/local/rcs/bin/co ${bindir}
cp ${objdir}/usr/local/rcs/bin/coconfig ${bindir}
cp ${objdir}/usr/local/rcs/bin/ident ${bindir}
cp ${objdir}/usr/local/rcs/bin/merge ${bindir}
cp ${objdir}/usr/local/rcs/bin/name_version ${bindir}
cp ${objdir}/usr/local/rcs/bin/rcs ${bindir}
cp ${objdir}/usr/local/rcs/bin/rcsclean ${bindir}
cp ${objdir}/usr/local/rcs/bin/rcsdiff ${bindir}
cp ${objdir}/usr/local/rcs/bin/rcsdiff3 ${bindir}
cp ${objdir}/usr/local/rcs/bin/rcsfreeze ${bindir}
cp ${objdir}/usr/local/rcs/bin/rcsit ${bindir}
cp ${objdir}/usr/local/rcs/bin/rcsmerge ${bindir}
cp ${objdir}/usr/local/rcs/bin/rcsstat ${bindir}
cp ${objdir}/usr/local/rcs/bin/rcstime ${bindir}
cp ${objdir}/usr/local/rcs/bin/rlog ${bindir}
cp ${objdir}/usr/local/rcs/bin/sccstorcs ${bindir}
cp ${objdir}/usr/local/rcs/bin/snapshot ${bindir}

(cd usr/local/sdm/lib; \
 make -ckEF INCARGS="-I. -I${expdir}/usr/include" \
   LIBARGS="-L${expdir}/usr/ccs/lib" \
   rcsauth rcsacl srcacl)
cp ${objdir}/usr/local/sdm/lib/rcsauth ${libdir}
rm -f ${libdir}/srcauth
ln ${libdir}/rcsauth ${libdir}/srcauth
cp ${objdir}/usr/local/sdm/lib/rcsacl ${libdir}
cp ${objdir}/usr/local/sdm/lib/srcacl ${libdir}

exit 0
