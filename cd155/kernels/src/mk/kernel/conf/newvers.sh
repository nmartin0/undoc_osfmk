#!/bin/sh -
# 
# $Copyright
# Copyright 1991 , 1994, 1995 Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
#
# Mach Operating System
# Copyright (c) 1991,1990,1989 Carnegie Mellon University
# All Rights Reserved.
# 
# Permission to use, copy, modify and distribute this software and its
# documentation is hereby granted, provided that both the copyright
# notice and this permission notice appear in all copies of the
# software, derivative works or modified versions, and any portions
# thereof, and that both notices appear in supporting documentation.
# 
# CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
# CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
# ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
# 
# Carnegie Mellon requests users of this software to return to
# 
#  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
#  School of Computer Science
#  Carnegie Mellon University
#  Pittsburgh PA 15213-3890
# 
# any improvements or extensions that they make and grant Carnegie Mellon
# the rights to redistribute these changes.
#
#
# HISTORY
# $Log: newvers.sh,v $
# Revision 2.15  1994/11/18  20:28:39  mtm
# Copyright additions/changes
#
# Revision 2.14  1993/07/08  15:59:41  stans
#    Break MK version string with a newline; added who built the MK.
#
# Revision 2.13  1993/06/30  22:20:01  dleslie
# Adding copyright notices required by legal folks
#
# Revision 2.12  1993/04/27  20:17:53  dleslie
# Copy of R1.0 sources onto main trunk
#
# Revision 2.11.2.2  1993/04/22  18:17:44  dleslie
# First R1_0 release
#
# Revision 2.10  1993/02/16  21:53:58  dleslie
# Support for new mkidinfo
#
# Revision 2.9  1993/01/29  18:04:28  dleslie
# Changes for new 'mkidinfo' version string scheme.
#
# Revision 2.7  1993/01/09  22:33:38  stans
# norma13.16 version SSD Trans 7:
#   Added intel.version string.
#
# Revision 2.6.2.1  92/06/24  17:58:14  jeffreyh
# 	Fixed build of version string to accomodate both BSD and OSF/1
# 	/bin/echo commands. ("\n" can't be echoed with the same command)
# 	[92/06/09            bernadat]
# 
# Revision 2.6  91/05/08  12:25:38  dbg
# 	Added one more sed clause to deal with the "'s in the new 
# 	copyright.
# 	[91/05/07            mrt]
# 
# Revision 2.5  91/02/05  17:05:40  mrt
# 	Changed to new copyright
# 	[91/01/28  14:48:14  mrt]
# 
# Revision 2.4  90/09/14  12:56:01  rwd
# 	Dont printout major.minor in VERSION string.
# 	[90/09/14            rwd]
# 
# Revision 2.3  90/08/27  21:48:12  dbg
# 	Document that this file was created at CMU.
# 	[90/07/13            dbg]
# 
# Revision 2.2  89/11/29  14:08:47  af
# 	Use Mach version id.
# 
# Revision 2.1  89/08/03  17:38:33  rwd
# Created.
# 

#
# newvers.sh	copyright major minor variant edit patch
#

mkidinfo -f mach_kernel
relvers=`cat _cvs_id.c | sed -n -e 's/^.*"@/@/' -e 's/";$//p'`
rm -f _cvs_id.c
intel=`echo $relvers | awk '{print $3}'`
edit="$5"; major="$2"; minor="$3"; variant="$4"; patch="$6"; copyright="$1"
v="VERSION(${variant}${edit}${patch} ${intel})" d=`pwd` h=`hostname` t=`date` 
u=`whoami`
if [ -z "$d" -o -z "$h" -o -z "$t" ]; then
    exit 1
fi
CONFIG=`expr "$d" : '.*/\([^/]*\)$'`
d=`expr "$d" : '.*/\([^/]*/[^/]*\)$'`
(
  /bin/echo "char version_intel[]    = \"${relvers}\";" ;
  /bin/echo "int  version_major      = ${major};" ;
  /bin/echo "int  version_minor      = ${minor};" ;
  /bin/echo "char version_variant[]  = \"${variant}\";" ;
  /bin/echo "char version_patch[]    = \"${patch}\";" ;
  /bin/echo "int  version_edit       = ${edit};" ;
  /bin/echo "char version[] = \"Mach 3.0 ${v}: ${t};\\n\\t$d ($u@$h)" | sed -e 's?$?\\n\";?'
  /bin/echo "char cmu_copyright[] = \"\\" ;
  sed <$copyright -e '/^#/d' -e 's/"/\\"/g' -e 's;[ 	]*$;;' -e '/^$/d' -e 's;$;\\n\\;' ;
  /bin/echo "\";";
) > vers.c
if [ -s vers.suffix -o ! -f vers.suffix ]; then
    echo ".${variant}${edit}${patch}.${CONFIG}" >vers.suffix
fi
exit 0
