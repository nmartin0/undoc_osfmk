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
# OSF/1 Release 1.0.2

# @(#)$RCSfile: install.sh,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 01:26:18 $
#
# COMPONENT_NAME: (CMDFILES) commands that manipulate files
#
# FUNCTIONS: install
#
# ORIGINS: 3, 27
#
# (C) COPYRIGHT International Business Machines Corp. 1985, 1989
# All Rights Reserved
# Licensed Materials - Property of IBM
#
# US Government Users Restricted Rights - Use, duplication or
# disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
#
# install.sh	1.5  com/cmd/files, 9104320c, bos320 10/15/90 18:08:45

# NAME: usage
#
# FUNCTION: displays the usage statement
#
#
# RETURNS: 0 if successful; 2 if unsuccessful.
# 
usage()
{
  dspmsg -s 2 install.cat 1 "Usage: install [-c dira] [-f dirb] [-i] [-M] [-m mode] [-u owner]\n\
\t       [-g group] [-S] [-n dirc] [-o] [-s] file [dirx ...]\n" 1>&2
  exit 2
}

#
# NAME: install (shell script)
#
# FUNCTION: installs a command
#
# EXECUTION ENVIRONMENT: Shell 
#
# NOTES:
# 	Possible flags:
#	-c dira		installs a new command file in dira only if that
#			file does not already exist there.
#	-f dirb		installs a command file in dirb whether or not file   
#			already exists.
#	-i		ignores the default directory list and searches only
#			thoes directories specified on the command line.
# 	-M 		moves a file instead of being copied.
#	-n dirc		installs file in dirx if it is not in any of the     
#			searched directories.
#	-m mode		specifies the mode of the destination file
#	-u mode		specifies the owner of the destination file
#	-g mode		specifies the group of the destination file
#	-o		saves the old copy of file.
#	-s		displays error messages only.
#	-S		strippes the binary after installation.
#
# RETURNS: 0 if it is successful; 2 if it is unsuccessful.
#
 
#
# initializes some shell variables
#
FLIST=/etc/syslist
DEFAULT="/bin /usr/bin /etc /lib /usr/lib" 
FOUND="" MOVOLD="" MVFILE=""
ECHO=echo MODE=755 OWNER=bin GROUP=bin
MFLAG="" OFLAG="" GFLAG="" STRIP=""
MVERR=1 CPERR=1
dspmsg="/usr/bin/dspmsg"
DSPMSG=$dspmsg

#
# Process options
set -- `getopt g:m:u:Sc:f:iMn:os $*`
if [ $? != 0 ]
then usage
fi
while [ $# -gt 0 ]
do
  case $1 in
  -c )	cflg=c
	direct=$2
	ARG=-c
	shift
	;;
  -f )	fflg=f
	direct=$2
	ARG=-f
	shift
	;;
  -i )	iflg=i
	DEFAULT=""
	flag=-i
	;;
  -M )	mflg=m
	MVFILE=yes
	flag=-m
	;;
  -n )	nflg=n
	LASTRES=$2
	FOUND=n
	flag=-n
	shift
	;;
  -o )	oflg=o
	MOVOLD=yes
	;;
  -s )	# Disable echo and dspmsg to standard output
	DSPMSG=:
	ECHO=:
	;;
  -m )
	case $2 in
	 	[0-7][0-7][0-7]) MODE=$2;; 
		[0-7][0-7][0-7][0-7]) MODE=$2;;
		*)
		dspmsg -s 2 install.cat 10 \
'install: 0653-249 Specify permissions for -m option in absolute mode.\n' 1>&2
		exit 2
	esac	
	MFLAG=on
	shift
	;;
  -u )
	OWNER=$2
	OFLAG=on
	shift
	;;
  -g )
	GROUP=$2
	GFLAG=on
	shift
	;;
  -S )
	STRIP=yes
	;;
  -- )	shift
	break
	;;
  *)	break
	;;
  esac
  shift
done

# Check for invalid option combinations:
coth="$fflg$iflg$oflg$nflg$mflg"
if [ .$cflg = .c -a .$coth != . ]
then
  bad1=-$cflg
  bad2=`echo "$coth" | awk '{ print substr($0,1,1) }'`
else
  foth="$iflg$mflg$nflg"
  if [ .$fflg = .f -a .$foth != . ]
  then
    bad1=-$fflg
    bad2=`echo "$foth" | awk '{ print substr($0,1,1) }'`
  elif [ .$iflg = .i -a .$mflg = .m ]
  then
    bad1=-$iflg
    bad2=$mflg
  elif [ .$mflg = .m -a .$nflg = .n ]
  then
    bad1=-$mflg
    bad2=$nflg
  fi
fi
if [ .$bad1 != . ]
then
  bad2=-$bad2
  dspmsg -s 2 install.cat 2 'install: 0653-244 The %1$s and %2$s flags may not be used together.\n' $bad1 $bad2 1>&2
  usage
fi

# Check for File argument:
if [ $# -lt 1 ]
then usage
fi
# Get File name
#
FILEP=$1 FILE=`echo $1 | sed -e "s/.*\///"`

# Search specified directory if -c or -f option was used.
if [ x$ARG = x-c -o x$ARG = x-f ]
then
	case $2 in
		-*) usage ;;
		"") :	;;
	esac
	if test -f $direct/$FILE -o -f $direct/$FILE/$FILE
	then
		case $ARG in
			-c)
			    dspmsg -s 2 install.cat 3 \
'0653-241 File %1$s already exists in directory %2$s.\n' $FILE $direct 1>&2
			    exit 2;;
			-f) if [ -k $direct/$FILE ]
			    then
				chmod -t $direct/$FILE
				$direct/$FILE < /dev/null > /dev/null
				tbit=on
			    fi
			    if [ "$MOVOLD" = yes ]
			    then
				mv $direct/$FILE $direct/OLD$FILE
				cp $direct/OLD$FILE $direct/$FILE
				if [ $? = 0 ]
				then
				   $DSPMSG -s 2 install.cat 4 \
'install: 0653-246 File %1$s is moved to %2$s .\n' $FILE $direct/OLD$FILE
				   chgrp $GROUP $direct/$FILE
				else
				   dspmsg -s 2 install.cat 5 \
'install: 0653-247 The command  cp %1$s %2$s  failed.\n' $direct/OLD$FILE $direct/$FILE 1>&2
				   exit 2
				fi
			    fi
			    LS=`ls -l $direct/$FILE`
			    OLDWMODE=`expr "$LS" : '..\(.\).*'`
			    chmod u+w $direct/$FILE
			    if cp $FILEP $direct/$FILE
			    then
				$DSPMSG -s 2 install.cat 6 \
'File %1$s is installed as %2$s .\n' $FILEP $direct/$FILE
			    fi
			    if [ "$MFLAG" = on ]
				then
			  	if chmod $MODE $direct/$FILE
				then
					$ECHO "chmod $MODE $direct/$FILE"
				else
					exit 2
				fi
			    else
				if [ $OLDWMODE = - ]
			    	then
					chmod u-w $direct/$FILE
			    	fi
			        if [ "$tbit" = on ]
			        then
					chmod +t $direct/$FILE
			        fi
			    fi
			    if [ "$GFLAG" = on ]
			    then
				if chgrp $GROUP $direct/$FILE
				then
					$ECHO "chgrp $GROUP $direct/$FILE"
				else
					exit 2
				fi
			    fi
			    if [ "$OFLAG" = on ]
			    then
				if chown $OWNER $direct/$FILE
				then
					$ECHO "chown $OWNER $direct/$FILE"
				else 
					exit 2
				fi
			    fi
			    if [ "$STRIP" = yes ]
			    then
				if /bin/strip $direct/$FILE 
				then
					$ECHO "strip $direct/$FILE"
				else
					exit 2
				fi
			    fi
			    exit;;
		esac
	else
# file does not exist
		cp $FILEP $direct/$FILE
		if [ $? = 0 ]
		then
			$DSPMSG -s 2 install.cat 6 \
'File %1$s is installed as %2$s .\n' $FILEP $direct/$FILE
# if -m , -u or -g flag is not on, file will get the default values
			chmod $MODE $direct/$FILE       
			if [ $? != 0 ] 
			then exit 2
			fi
			chgrp $GROUP $direct/$FILE
			if [ $? != 0 ]
			then exit 2
			fi
 			chown $OWNER $direct/$FILE
			if [ $? != 0 ]
			then exit 2
			fi
			if [ "$MFLAG" = on ]
			then
				$ECHO "chmod $MODE $direct/$FILE"
			fi
			if [ "$GFLAG" = on ]
			then
				$ECHO "chgrp $GROUP $direct/$FILE"
			fi
			if [ "$OFLAG" = on ]
			then
				$ECHO "chown $OWNER $direct/$FILE"
			fi
			if [ "$STRIP" = yes ]
			then 
				if /bin/strip $direct/$FILE
				then 
					$ECHO "strip $direct/$FILE"
				else
					exit 2
 				fi
			fi
		fi
	fi
	exit
# done with the command if the file is installed by -c or -f 
fi

shift

#
# starts searching the dirx directories or the default directories
#
PUTHERE=""
for i in $*
do
	case $i in
		-*) usage ;;
	esac
	PUTHOLD=`find $i -name $FILE -type f -print`
	PUTHERE=`expr "\`echo $PUTHOLD\`" : '\([^ ]*\)'`
	if [ "$PUTHERE" != "" ]
	then break
	fi
done
if [ -r $FLIST -a "$PUTHERE" = "" ]
then
	PUTHERE=`grep "/${FILE}$" $FLIST | sed  -n -e '1p'`
fi
if [ "$PUTHERE" = "" ]
then
	for i in $DEFAULT
	do
		PUTHOLD=`find $i -name $FILE -type f -print`
		PUTHERE=`expr "\`echo $PUTHOLD\`" : '\([^ ]*\)'`
		if [ "$PUTHERE" != "" ]
		then break
		fi
	done
fi
if [ "$PUTHERE" != "" ]
then
		    if [ -k $PUTHERE ]
		    then
			chmod -t $PUTHERE
			$PUTHERE < /dev/null > /dev/null
			tbit=on
		    fi
		    if [ "$MOVOLD" = yes ]
		    then
			old=`echo $PUTHERE | sed -e "s/\/[^\/]*$//"`
			mv $PUTHERE $old/OLD$FILE
			cp $old/OLD$FILE $PUTHERE
			if [ $? = 0 ]
			then
			  chgrp $GROUP $PUTHERE
			  $DSPMSG -s 2 install.cat 7 \
'The old file %1$s is moved to %2$s .\n' $FILE $old/OLD$FILE
			else
			  dspmsg -s 2 install.cat 5 \
'install: 0653-247 The command  cp %1$s %2$s  failed.\n' $old/OLD$FILE $PUTHERE 1>&2
			  exit 2
			fi
		    fi
		    FOUND=y
		    LS=`ls -l $PUTHERE`
		    OLDWMODE=`expr "$LS" : '..\(.\).*'`
		    chmod u+w $PUTHERE
		    if [ "$MVFILE" = yes ]
		    then 
		    	mv $FILEP $PUTHERE
			MVERR=$?
		    else
			cp $FILEP $PUTHERE
		        CPERR=$?
		    fi
		    if [ $CPERR = 0 -o $MVERR = 0 ]
		    then
			$DSPMSG -s 2 install.cat 6 \
'File %1$s is installed as %2$s .\n' $FILEP $direct/$PUTHERE
		    else
			exit 2
		    fi
		    if [ "$MFLAG" = on ]
		    then
		  	if chmod $MODE $PUTHERE
			then
				$ECHO "chmod $MODE $PUTHERE"
			else
				exit 2
			fi
		    else 
			if [ $OLDWMODE = - ]
		        then
				chmod u-w $PUTHERE
		    	fi
		        if [ "$tbit" = on ]
		    	then
			     	chmod +t $PUTHERE
		     	fi
		    fi
		    if [ "$GFLAG" = on ]
		    then
			if chgrp $GROUP $PUTHERE
			then
				$ECHO "chgrp $GROUP $PUTHERE"
			else 
  				exit 2
			fi
		    fi
		    if [ "$OFLAG" = on ]
		    then
			if chown $OWNER $PUTHERE
			then 
				$ECHO "chown $OWNER $PUTHERE"
			else
				exit 2
			fi
		    fi
		    break
fi

case $FOUND in
	"") dspmsg -s 2 install.cat 8 \
'install: 0653-248 File %s was not found.\n' $FILE 1>&2
	    exit 2;;
	 y) :  ;; 
	 n) cp $FILEP $LASTRES/$FILE
	    if [ $? = 0 ]
	    then
		$DSPMSG -s 2 install.cat 9 \
'File %1$s is installed as %2$s by default!\n' $FILEP $LASTRES/$FILE
# if -m , -u or -g flag is not on, file will get the default values
		cd $LASTRES
 		chmod $MODE $FILE
		if [ $? != 0 ]
		then exit 2
		fi
		chgrp $GROUP $FILE
		if [ $? != 0 ]
		then exit 2
		fi
		chown $OWNER $FILE
		if [ $? != 0 ]
		then exit 2
		fi
		if [ "$MFLAG" = on ]
		then
			$ECHO "chmod $MODE $PUTHERE"
		fi
		if [ "$GFLAG" = on ]
		then
			$ECHO "chgrp $GROUP $PUTHERE"
		fi
		if [ "$OFLAG" = on ]
		then
			$ECHO "chown $OWNER $PUTHERE"
		fi
	    fi;;
esac
