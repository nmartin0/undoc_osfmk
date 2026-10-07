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

# @(#)$RCSfile: spell.sh,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:40:01 $

# COMPONENT_NAME: (CMDTEXT) Text Formatting Services
# 
# FUNCTIONS:
# 
# ORIGINS: 3,10,27
# 
# (C) COPYRIGHT International Business Machines Corp. 1989
# All Rights Reserved
# Licensed Materials - Property of IBM
# 
# US Government Users Restricted Rights - Use, duplication or
# disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
# 
# spell program
# Variables from environment (else defaulted):
#   D_SPELL dictionary, S_SPELL stop, H_SPELL history, P_SPELL program
# Local variables:
#   B flags
#   F input files
#   V data for -v
#   D -l/-i handling for forming deroff arguments
#   M non-null if multiple -i/-l flags
#   LOCAL file containing locally acceptable words (via + argument)
#   POSTPROC post-processing commands (for LOCAL list and history)
#
#   spell.sh	1.6  3/3/90 14:40:46
# 
H_SPELL=${H_SPELL-/usr/lbin/spell/spellhist}
P_SPELL=${P_SPELL-/usr/lbin/spell/spellprog}
# 
#   Only deroff V5.5 and up know about -u flag.
DEROFF="deroff -u"
V=/dev/null
F=
B=
D=
M=

next="F="
trap "rm -f /tmp/spell.$$; exit" 0 1 2 13 15
for A in $*
do
	case $A in
	-v)     if [ -x /bin/pdp11 ] && /bin/pdp11
		then	ERRMSG=`dspmsg spell.cat 1 "-v option not supported on pdp11"`
			if [ "$ERRMSG" = "" ]
			then	echo -v option not supported on pdp11 >&2
			else	echo $ERRMSG >&2
			fi
			EXIT_SPELL=exit
		else    B="$B -v"
			V=/tmp/spell.$$
		fi ;;
	-a)	: ;;
	-b) 	D_SPELL=${D_SPELL-/usr/lbin/spell/hlistb}
		B="$B -b" ;;
	-x)     B="$B -x" ;;
	+*)	if [ "$FIRSTPLUS" = "+" ]
		then	ERRMSG=`dspmsg spell.cat 2 "multiple + options in spell, all but the last are ignored"`
			if [ "$ERRMSG" = "" ]
			then	echo "multiple + options in spell, all but the last are ignored" >&2
			else	echo $ERRMSG >&2
			fi
		fi;
		FIRSTPLUS="$FIRSTPLUS"+
		if  LOCAL=`expr $A : '+\(.*\)' 2>/dev/null`;
		then if test ! -r $LOCAL;
			then ERRMSG=`dspmsg spell.cat 3 "spell cannot read %s" $LOCAL`
			if [ "$ERRMSG" = "" ]
			then echo "spell cannot read $LOCAL" >&2; EXIT_SPELL=exit;
			else	echo $ERRMSG >&2; EXIT_SPELL=exit;
			fi
		     fi
		else ERRMSG=`dspmsg spell.cat 4 "spell cannot identify local spell file"`
			if [ "$ERRMSG" = "" ]
			then echo "spell cannot identify local spell file" >&2; EXIT_SPELL=exit;
			else	echo $ERRMSG >&2; EXIT_SPELL=exit;
			fi
		fi ;;
	-l|-i)  M="$D"
		D="$A" ;;
	-d)	# alternate dictionary (not american or british)
		next="D_SPELL="
		;;
	-s)	# alternate stoplist (must be hashed!)
		next="S_SPELL="
		;;
	-h)	# alternate histlist
		next="H_SPELL="
		;;
	-*)	# bad option
		ERRMSG=`dspmsg spell.cat 5 "spell: unknown flag '%s'" $A`
		if [ "$ERRMSG" = "" ]
		then	echo "spell: unknown flag '$A'" 2>&1
		else	echo $ERRMSG 2>&1
		fi
		ERRMSG=`dspmsg spell.cat 6 "usage: spell [-v] [-a|-b] [-x] [-l] [-i] [+hlist] [-d hlist] [-s hstop] [-h sphist] [file ...]"`
		if [ "$ERRMSG" = "" ]
		then	echo "usage: spell [-v] [-a|-b] [-x] [-l] [-i] [+hlist] [-d hlist] [-s hstop] [-h sphist] [file ...]" 2>&1
		else	echo $ERRMSG 2>&1
		fi
		exit 1
		;;
	*)	# option arguments or filenames
		eval $next"$A"
		next="F=$F@"
		;;
	esac
	done
${EXIT_SPELL-:}
IFS="${IFS}@"

if [ -n "$M" ]
then  ERRMSG=`dspmsg spell.cat 7 "multiple -i/-l options in spell, all but the last are ignored"`
	if [ "$ERRMSG" = "" ]
	then	echo "multiple -i/-l options in spell, all but the last are ignored" >&2
	else	echo $ERRMSG 2>&1
	fi
fi

# -l flag to spell means no special flags to deroff (follow ALL .so/.nx)
# default (no -l or -i flag to spell) means don't include /usr/lbin*: deroff -l
# -i flag to spell (follow NO .so/.nx) is passed through to deroff
case "$D" in
	-l)     D="" ;;
	"")     D="-l" ;;
esac

POSTPROC=
if [ -n "$LOCAL" ]
then
	POSTPROC=" | sort | comm -23 - $LOCAL"
fi

if [ -w "$H_SPELL" ]
then
	POSTPROC="$POSTPROC |tee -a $H_SPELL; who am i >>$H_SPELL 2>/dev/null"
fi

eval "$DEROFF -w $D $F |  sort -u +0 |  \
 $P_SPELL ${S_SPELL-/usr/lbin/spell/hstop} 1 | \
 $P_SPELL ${D_SPELL-/usr/lbin/spell/hlista} $V $B \
 $POSTPROC "
case $V in
/dev/null)	exit
esac
sed '/^\./d' $V | sort -u +1f +0
