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
# (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
#
# OSF/1 Release 1.0.1

# @(#)$RCSfile: lint.sh,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:53:42 $

# COMPONENT_NAME: (CMDPROG) Programming Utilities
#
# FUNCTIONS: 
#
# ORIGINS: 00 03 10 27 32
#
# (C) COPYRIGHT International Business Machines Corp. 1985, 1989
# All Rights Reserved
# Licensed Materials - Property of IBM
#
# US Government Users Restricted Rights - Use, duplication or
# disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
#
# lint.sh	1.20  com/cmd/prog/lint,3.1.2,9049base 10/4/90 12:41:23
#
# Lint shell script.  Interacts with options from cc(1).
#
LLDIR=/usr/ccs/lib			# location of lint executables
LCPP=/lib/cpp
LINT1=$LLDIR/lint1
LINT2=$LLDIR/lint2
TMP=/usr/tmp/tlint.$$		# preprocessor scratch file
TOUT=				# combined input for second pass
LIBA=$LLDIR/llib-lansi.ln	# standard ANSI library
LIBE=$LLDIR/llib-lc.ln		# default EXTD library

POPT="-E -C -Dlint -DLINT"
# options for the preprocessor
L1OPT=				# options for the lint passes
L2OPT=				# options for lint pass2 only
CFILES=				# the *.c files in order
LFILES=				# the *.ln files in order
LLIB=				# lint library file to create
WARNS="-wA"			# lint warning level
CONLY=				# set for ``compile only''
MANSI=				# set if ANSI mode
LOOK=				# set for echo commands only
RC=0				# Value of 2 to signal failure of lint1
CF=0				# C files specified on input line
				# GH 09/14/90
trap "rm -f $TMP $TOUT; exit 2" 1 2 3 15

#
# Process each of the arguments, building lists of options.
#
# jrw  16/05/90 modified control loop from for to while to allow shift
#               usage on -o option so that -o filename and -ofilename are
#		valid syntax. Meets X/Open standard (Vol1 Version 3).
#		   PTM 40555

# GH - 09/21/90 modified while loop to explicitly check for "" in order		
# that the TEST function does not treat bad flag to LINT as its own and give	
# error messages. P48838 
while [ "" != "$*" ] 

do
        OPT=$1  # kludge to avoid changing the usage of OPT
	case "$OPT" in
	*.c)	CFILES="$CFILES $OPT";
		TOUT=/usr/tmp/olint.$$;;	# combined input for second pass
	*.ln)	LFILES="$LFILES $OPT";;
	-*)	OPT=`echo $OPT | sed s/-//p`
		while [ "" != "$OPT" ] # GH fix
		do
			O=`echo $OPT | sed 's/\\(.\\).*/\\1/p'`	# option letter
			OPT=`echo $OPT | sed s/.//p`	# option argument
			case $O in
			\#)	LOOK=1;;		# echo commands only
			c)	CONLY=1;;	# lint1 only, make .ln files
			p)	L2OPT="$L2OPT -$O";
				LIBE="$LLDIR/llib-port.ln";;	# extreme portability
			n)	LIBA=				# no libraries
				LIBE=;;
			v)	L1OPT="$L1OPT -v"	# parameter usage check
				;;
			a)	WARNS="${WARNS}l"     # warning message options
				;;
			w)	WARNS="$WARNS$OPT"    # warning message options
				break;;
			b)	WARNS="${WARNS}R"    # warning message options
				;;
			[hu])	WARNS="$WARNS$O"    # warning message options
				;;
			x)	WARNS="${WARNS}D"    # warning message options
				;;
			[NX])	L1OPT="$L1OPT -$O$OPT"	# valid cc(1) options
				break;;
			M)	L1OPT="$L1OPT -$O$OPT"	# valid cc(1) options
				MANSI=1
				break;;
			l)	if [ "" != "$OPT" ]	# include a lint library
				then
					LFILES="$LLDIR/llib-l$OPT.ln $LFILES"
				else
					dspmsg lint.cat 1 "improper usage of option: %s\n" $O
				fi
				break;;
			o)	if [ "" != "$OPT" ]		# make a lint library
				then
					OPT=`basename $OPT`
					LLIB="llib-l$OPT.ln" # GH fix
				else if [ "" != "$2" ] # pick up next arg as lib
				     then
					OPT=`basename $2`
					LLIB="llib-l$OPT.ln" # GH fix
					shift
				     else
					# situation of -o with no more args
					dspmsg lint.cat 1 "improper usage of option: %s\n" $O
				     fi
				fi
				break;;
			[IDU])	if [ "" != "$OPT" ]		# preprocessor options
				then
					POPT="$POPT -$O$OPT"
				else
					dspmsg lint.cat 1 "improper usage of option: %s\n" $O
				fi
				break;;
			*)	dspmsg lint.cat 2 "lint: bad option ignored: %s\n" $O;;
			esac
		done;;
	*)	dspmsg lint.cat 3 "lint: file with unknown suffix ignored: %s\n" $OPT;;
	esac
	shift
done
#
# Check for full ANSI library.
#
if [ "" != "$MANSI" ]
then
	LFILES="$LIBA $LFILES"		# standard ANSI library
else
	LFILES="$LFILES $LIBE"		# standard EXTD library
fi

#
# Run the file through lint1 (lint2).
#
if [ "" != "$CONLY" ]		# run lint1 on *.c's only producing *.ln's
then
	for i in $CFILES
	do
		T=`basename $i .c`.ln
		rm -f $TMP $T
		if [ "" != "$LOOK" ]
		then	
			echo "( $LCPP $POPT $i > $TMP"
			echo "$LINT1 $WARNS $L1OPT $TMP -L$T ) 2>&1"
		else
			( $LCPP $POPT $i > $TMP
			$LINT1 $WARNS $L1OPT $TMP -L$T ) 2>&1
		fi
	done
else			# send all *.c's through lint1 run all through lint2
	if [ "" != "$CFILES" ]
	then
		CF=1
		rm -f $TOUT; touch $TOUT
	fi
	for i in $CFILES
	do	
		if [ $RC -eq 2 ]
		then
			rm -f $TMP $TOUT;
			exit $RC;
		fi
		rm -f $TMP
		if [ "" != "$LOOK" ]
		then	
			echo "( $LCPP $POPT $i > $TMP"
			echo "$LINT1 $WARNS $L1OPT $TMP -L$TOUT ) 2>&1"
		else
			( $LCPP $POPT $i > $TMP
			$LINT1 $WARNS $L1OPT $TMP -L$TOUT ) 2>&1
			RC=$?
		fi
	done

	if [ ! $RC -eq 2 ]	# 2 means lint1 failed without recovering
	then
		if [ "" != "$LOOK" ]
		then	
			echo "$LINT2 $WARNS $L1OPT $L2OPT $TOUT $LFILES"
		else
			$LINT2 $WARNS $L1OPT $L2OPT $TOUT $LFILES
		fi

		# GH 09/21/90 P48838, making sure mv is not called 
		# unless C files were supplied
		if [ "" != "$LLIB" ]	# make a library of lint1 results
		then
			if [ $CF -eq 1 ] # GH fix
			then
				mv $TOUT $LLIB
			fi
		fi
	else
		rm -f $TMP $TOUT;
		exit $RC;
	fi
fi
rm -f $TMP $TOUT
