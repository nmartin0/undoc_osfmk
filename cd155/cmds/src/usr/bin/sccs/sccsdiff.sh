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
# (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
# 
# 
# OSF/1 Release 1.0.3

#
# "@(#)$RCSfile: sccsdiff.sh,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:37:54 $"
#
# COMPONENT_NAME: CMDSCCS      Source Code Control System (sccs)
#
# FUNCTIONS: N/A
#
# ORIGINS: 3, 10, 27
#
# This module contains IBM CONFIDENTIAL code. -- (IBM
# Confidential Restricted when combined with the aggregated
# modules for this product)
# OBJECT CODE ONLY SOURCE MATERIALS
# (C) COPYRIGHT International Business Machines Corp. 1985, 1989
# All Rights Reserved
#
# US Government Users Restricted Rights - Use, duplication or
# disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
#
# sccsdiff.sh    1.5 1/4/90 18:11:10
#

sid1= sid2= num= pipe= files=

for i in $@
do
	case $i in

	-*)
		case $i in

		-r*)
			if [ ! "$sid1" ]
			then
				sid1=$i
			elif [ ! "$sid2" ]
			then
				sid2=$i
			fi
			;;
		-s*)
			num=`expr "$i" : '-s\(.*\)'`
			;;
		-p*)
			pipe=yes
			;;
		*)
			files=
			break
			;;
		esac
		;;
	*s.*)
		files="$files $i"
		;;
	*)
		dspmsg sccsdiff.cat 1 '%1$s: %2$s is not an SCCS file.\n\tSpecify an SCCS file.\n' $0 $i 1>&2
#		echo "$0: $i not an SCCS file" 1>&2
		;;
	esac
done

if [ -z "$files" ]
then
	dspmsg sccsdiff.cat 2 'usage: %s -rSID1 -rSID2 \[-p\] \[-sNumber\] SCCSFile...\n\tCompares two versions of a Source Code Control System (SCCS) file.\n' $0 1>&2
#	echo "usage: $0 -r<sid1> -r<sid2> [-p] [-s<num-arg>] sccsfile ..." 1>&2
	exit 1
fi

trap "rm -f /tmp/get[abc]$$;exit 1" 0 1 2 3 15

for i in $files
do
	if get -s -p -k $sid1 $i > /tmp/geta$$
	then
		if get -s -p -k $sid2 $i > /tmp/getb$$
		then
			bdiff /tmp/geta$$ /tmp/getb$$ $num > /tmp/getc$$
		fi
	fi
	if [ ! -s /tmp/getc$$ ]
	then
		if [ -f /tmp/getc$$ ]
		then
                  echo '%s: No differences.\n' $i >/tmp/getc$$
		else
		exit 1
		fi
	fi
	if [ "$pipe" ]
	then
		msg=`dspmsg sccsdiff.cat 3 '%1$s: %2$s versus %3$s\n' $i $sid1 $sid2`
		pr -h "$msg" /tmp/getc$$
	else
		cat /tmp/getc$$
	fi
done

trap 0
rm -f /tmp/get[abc]$$
