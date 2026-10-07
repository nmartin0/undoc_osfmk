#!/sbin/sh
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
# @OSF_COPYRIGHT@
#
#
# HISTORY
# $Log: adduser.sh,v $
# Revision 1.12  1994/11/19  03:03:56  mtm
# Copyright additions/changes
#
# Revision 1.11  1993/10/15  20:22:08  shala
# Integrated fixes from OSF for bugs
#            4934 (can't interrupt script with cntl-C);
#            4943 (mode of output /etc/group dependent on root's umask);
#            5748 (failure if group name defined more than once in /etc/group).
#
# Revision 2.2  93/10/15  11:54:42  bolinger
# 	Fixes for Intel bugs 4934 (can't interrupt script with cntl-C);
# 	4943 (mode of output /etc/group dependent on root's umask); 5748
# 	(failure if group name defined more than once).
# 
# Revision 1.7.2.5  92/03/17  22:25:12  tom
# 	Try and make this a little more robust so people will stop
# 	reporting bugs against it:
# 	  Check to see if user is root.
# 	  Check to see if yellow pages is in use.
# 	  Ask for the users shell.
# 	  Create default home directory if it doesn't exist.
# 	  Call the passwd program to set the new users password.
# 	[1992/03/17  22:23:28  tom]
# 
# Revision 1.7.2.4  1992/01/13  22:48:25  whalen
# 	Added shell variables for default group and default home directory.
# 	Made default home directory be /home.  Bug #2596.
# 	[1992/01/13  22:47:23  whalen]
# 
# Revision 1.7.2.3  1992/01/09  15:53:10  tom
# 	Fix up quoting to be a little nicer. (bug #4039)
# 	Use vipw to edit the password file instead of crude locking.
# 	[1992/01/09  15:51:16  tom]
# 
# Revision 1.7.2.2  1991/11/06  08:47:20  kathyg
# 	Corrected messages with %n$s formats -> default dspmsg message must be
# 	in single quotes and msg catalog msg should be in double. (bug #3534)
# 	[91/11/06  08:46:26  kathyg]
# 
# Revision 1.7  91/08/29  17:08:08  devrcs
# 	      Changed 3 occurences of /bin to /usr/bin.  Two were invocations
# 	      of awk and the third was the definition of the default shell in
# 	      the /etc/passwd file.
# 	[91/08/15  14:16:24  whalen]
# 
# Revision 1.6  91/07/09  12:39:14  devrcs
# 	Fix up messages to use numbers, not symbols.
# 	Change default path to include /usr/bin and not /sbin and /usr/sbin.
# 	[91/06/17  12:54:40  tom]
# 
# Revision 1.5  90/10/07  22:04:19  devrcs
# 	Added EndLog Marker.
# 	[90/09/29  14:26:44  gm]
# 
# Revision 1.4  90/09/23  16:56:52  devrcs
# 	FIx the bug 0694
# 	[90/09/10  04:59:18  aster]
# 
# Revision 1.3  90/08/09  14:45:51  devrcs
# 	Changes for osc.19 build.
# 
# $EndLog$

#
# adduser - Adds new users
#
PATH=/usr/bin:/usr/sbin:/sbin
export PATH

tmppfx=/tmp/adu$$
trap "rm -rf $tmppfx.*; trap 0 1 2 3 14 15; exit 255" 1 2 3 14 15
trap "status=$?; rm -rf $tmppfx.*; trap 0 1 2 3 14 15; exit $status" 0

#
# The maximum uid should really be read from /usr/include/machine/machlimits.h 
#

uid_max=60000

#
# The default home directory group and shell
#

def_home=/home
def_group=users
def_shell=/usr/bin/csh

#
# Check to see if we are root
#
if test ! -w /etc/passwd
then
	dspmsg adduser.cat 25 'You must be root to add a user.\n'
	exit 1
fi

#
# Check to see if this computer uses NIS (yellow pages)
#
tail -1 /etc/passwd | grep -q "^[+-]:"
yp_used="$?"
tail -1 /etc/group | grep -q "^[+-]:"
if [ $? -eq 0 ] || [ $yp_used -eq 0 ]
then
	dspmsg adduser.cat 26 '\nNIS in use. Refer to network documentation for adding users.\n'
	exit 1
fi

#
# New login name
#

repeat=yes
while test $repeat
    do
    echo ""
    dspmsg adduser.cat 1 "New login name: "
    read newuser

    if test "${newuser}"
    then
#
# Check if the newuser already exists in /etc/passwd
#
	grep "^${newuser}:" /etc/passwd > /dev/null
	if test $? -eq 0
	then
	    echo ""
	    dspmsg adduser.cat 2 "%s already exists in /etc/passwd file.\n" $newuser
	    exit 5
	else
	    repeat=""
	fi
    else
	exit 5
    fi
    done

#
# Full name of the newuser
#

repeat=yes
while test $repeat
    do
    dspmsg adduser.cat 3 "User full name : "
    read name

    if test "$name"
    then
	repeat=""
    else
	repeat=yes
    fi
    done

#
# Set up the new uid, check for the values < uid_max and sort the
# /etc/passwd file
#

uid=`fgrep -v 4294967294 /etc/passwd | sort -nt: +2 -3 | tail -1 | awk -F: '{print $3}' | sed -n '/[0-9][0-9]*/p'`

#
# See if the uid < uid_max
#

if test $uid
then
    ok=true
else
    echo "^G"
    dspmsg adduser.cat 15 "The /etc/passwd file may be corrupt. Operation aborted!\n"
    exit 1
fi

if test $uid -lt $uid_max
then
    ok=true
else
    echo "^G"
    dspmsg adduser.cat 16 "The uid is greater than the maximum allowed, %s. Operation aborted.\n" $uid_max
    exit 1
fi
uid=`expr $uid + 1`

repeat=yes
while test $repeat
    do
    dspmsg adduser.cat 3 "User id number [$uid] ? "
    read tuid

    repeat=""
    if test "$tuid"
    then
        uid=$tuid
    fi
    done

#
# Login group for the new user
#
rm -f $tmppfx.group
cp /etc/group $tmppfx.group
repeat=yes
while test $repeat
	do
	dspmsg adduser.cat 4 "User login group [%s] ? " ${def_group}
	read usergrp
	dspmsg adduser.cat 5 "Working..."

	if test \! "$usergrp"
	then
		usergrp=${def_group}
	fi

	#
	# GID for the new user
	#
	gid=`grep "^$usergrp:" $tmppfx.group | awk -F: '{print $3}'`
	occurs="`echo $gid | wc -w | sed -e 's; ;;g'`" 
	case $occurs in
	1)
		repeat=
		;;
	0)
		echo ""
		dspmsg adduser.cat 6 "\nGroup '%1\$s' unknown. Known groups are:\n" $usergrp
		/bin/awk -F: '{print $1}' $tmppfx.group | pr -t -l1 -4 2> /dev/null
		dspmsg adduser.cat 7 "The group '%s' must be added to the /etc/group file.\n" $usergrp
		exit 5
		;;
	*)
		dspmsg adduser.cat 30 "\nGroup '%1\$s' occurs %2\$s times in /etc/group. Operation aborted!\n" $usergrp $occurs
		exit 5
		;;
	esac
	done
loggid=$gid

#
# Add the user to the specified groups
#
    if test "$newuser"
    then
	if grep "^$usergrp:" /etc/group | grep "[:|,]$newuser[,|\$]" > /dev/null
	then 
	    echo ""
	    dspmsg adduser.cat 10 "\nUser '%1\$s' is already a member of group %2\$s.\n" $newuser $usergrp
	else
	    ed $tmppfx.group > /dev/null << EOF
	    /^$usergrp:/s/\$/,$newuser/
	    g/:,/s//:/
	    w
	    q
EOF
	fi
    fi


#

#
# Other group which the newuser may be member of.
#

dspmsg adduser.cat 8 "\nAnother group that '%s' should be a member of. <CR> for none: " $newuser
read another

while test "$another"
	do
	dspmsg adduser.cat 5 "Working ..."
	addnewuser=
	#
	# Check if the group is in the /etc/group
	#
	gid=`grep "^$another:" $tmppfx.group | awk -F: '{print $3}' `
	occurs="`echo $gid | wc -w | sed -e 's; ;;g'`" 
	case $occurs in
	1)
		addnewuser=y
		;;
	0)
		echo ""
		dspmsg adduser.cat 6 "\nGroup '%1\$s' unknown. Known groups are:\n" $another
		/bin/awk -F: '{print $1}' $tmppfx.group | pr -t -l1 -4 2> /dev/null
		dspmsg adduser.cat 7 "The group '%s' must be added to the /etc/group file.\n" $another
	dspmsg adduser.cat 9 "<CR> to continue "
		read dummy
		;;
	*)
		dspmsg adduser.cat 30 "\nGroup '%1\$s' occurs %2\$s times in /etc/group. Operation aborted!\n" $another $occurs
		exit 5
		;;
	esac
	if test $addnewuser
	then
		#
		# Add the user to the specified groups
		#
		if grep "^$another:" $tmppfx.group | grep "[:|,]$newuser$" > /dev/null
		then 
			echo ""
			dspmsg adduser.cat 10 "\nUser '%1\$s' is already a member of group %2\$s.\n" $newuser $another
		else
			ed $tmppfx.group > /dev/null << EOF
			/^$another:/s/\$/,$newuser/
			g/:,/s//:/
			w
			q
EOF
		fi
	fi

	#
	# See if there are more groups for the newuser
	#
	dspmsg adduser.cat 8 "\nAnother group that '%s' should be a member of. <CR> for none: " $newuser
	read another
	done

dspmsg adduser.cat 11 "Base directory for '%s' [%s]: " ${newuser} ${def_home}
read base
case ${base} in
"") base=${def_home}
	if test ! -d "${base}"
	then
		mkdir "${base}"
	fi
	;;
esac
while test \! -d "${base}"
do
	dspmsg adduser.cat 12 "%s not found.\n" ${base}
	dspmsg adduser.cat 13 "Base directory for user or <exit> to exit [%s] :" ${def_home}
	read base
	case ${base} in
	"")
		base=${def_home}
		;;
	exit)
		exit 5
		;;
	*)
		;;
	esac
done

#
# get the users login shell
#
while true
do
	dspmsg adduser.cat 28 "Login shell [%s]: " ${def_shell}
	read X
	case "${X}" in
		"") 	X="${def_shell}"
			;;
		*/*) 	grep -q "^${X}\$" /etc/shells
			if [ $? -ne 0 ]
			then
				X=""
			fi
			;;
		*) 	X=`grep "/${X}$" /etc/shells | sed 1q`
			;;
	esac
	if [ -n "${X}" ]
	then
		SHELL="${X}"
		break
	else
		dspmsg adduser.cat 29 "The shells are:\n\n"
		pr -a -3 -t /etc/shells
		echo ""
	fi
done

dspmsg adduser.cat 14 "\nAdding new user...\n"

trap "" 1 2 3 14 15

#
# Lock /etc/passwd and add the user to the password file 
# and update the hash table.
#

EDITOR=/usr/bin/ed; export EDITOR

/usr/sbin/vipw > /dev/null << EOFX
\$a
$newuser:*:$uid:$loggid:$name:${base}/${newuser}:${SHELL}
.
w
q
EOFX

if [ $? -ne 0 ]
then
	dspmsg adduser.cat 31 "\n/usr/sbin/vipw failed. Operation aborted!\n"
	exit 5
fi

#
# Only now move new group file into place.
#
mv $tmppfx.group /etc/group
chmod 0644 /etc/group

#
# Set-up and change owner, group and mode for the directories ${HOME}/bin
# /usr/spool/mail/${newuser} 
#

if test \! -d /usr/spool/mail
then
    mkdir /usr/spool/mail
fi

> /usr/spool/mail/${newuser}

mkdir ${base}/${newuser} ${base}/${newuser}/bin

chmod 0755 ${base}/${newuser} ${base}/${newuser}/bin
chmod 0600 /usr/spool/mail/${newuser} 2> /dev/null
chgrp $usergrp ${base}/${newuser} ${base}/${newuser}/bin /usr/spool/mail/${newuser}
chown ${newuser} ${base}/${newuser} ${base}/${newuser}/bin /usr/spool/mail/${newuser}

#
# Set-up home directory files .profile .cshrc .login and change owener
# group and mode, if the files are not under /usr/skel it creates them
# with few entries
#

if test -f /usr/skel/.profile 
then
    dspmsg adduser.cat 21 ".....Copying .profile\n"
    cp /usr/skel/.profile* ${base}/${newuser}/.profile
else
    dspmsg adduser.cat 18 ".....Initialializing .profile\n"
    cat >>${base}/${newuser}/.profile <<!
    HOME=${base}/${newuser}
    TERM=
    MAIL=/usr/spool/mail/${newuser}
    PATH=/usr/bin:.
    export TERM MAIL HOME PATH
!
fi

if test -f  /usr/skel/.cshrc 
then
    dspmsg adduser.cat 22 ".....Copying .cshrc\n"
    cp /usr/skel/.cshrc* ${base}/${newuser}/.cshrc
else
    dspmsg adduser.cat 19 ".....Initialializing .cshrc\n"
    cat >> ${base}/${newuser}/.cshrc <<!
    setenv HOME ${base}/${newuser}
    setenv MAIL /usr/spool/mail/${newuser}
!
fi


if test -f  /usr/skel/.login 
then
    dspmsg adduser.cat 23 ".....Copying .login\n"
    cp /usr/skel/.login* ${base}/${newuser}/.login
else
    dspmsg adduser.cat 20 ".....Initialializing .login\n"
    cat >> ${base}/${newuser}/.login <<!
    stty crt new
    tset -I -Q
    umask 022
    set path=(${base}/${newuser}/bin /usr/bin .)
    set prompt="`hostname`> "
    date
!
fi

chmod 0755 ${base}/${newuser}/.profile ${base}/${newuser}/.cshrc ${base}/${newuser}/.login
chgrp $usergrp ${base}/${newuser}/.profile ${base}/${newuser}/.cshrc ${base}/${newuser}/.login
chown ${newuser} ${base}/${newuser}/.profile ${base}/${newuser}/.cshrc ${base}/${newuser}/.login

dspmsg adduser.cat 24 "Until the password is set for '%s' they will not be able to login.\n" ${newuser}

if passwd ${newuser}
then
	:
else
	dspmsg adduser.cat 27 "Warning, user's password not set.\n"
fi

exit 0
