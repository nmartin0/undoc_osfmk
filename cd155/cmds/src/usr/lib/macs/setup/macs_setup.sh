#!/bin/sh
# 
# $Copyright
# Copyright 1994, 1995  Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
# 
# $Copyright
# Copyright 1994 Intel Corporation.
#        INTEL CONFIDENTIAL
# Permission to use, copy, modify, and distribute this software and 
# its online documentation for any purpose is hereby granted.
# Intel Coprporation, Inc makes no representations about 
# the suitability of this software for any purpose.
# It is provided "as is" without express or implied warranty.
#
# INTEL CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD 
# TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF 
# MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL INTEL CORPORATION.
# BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
# OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
# CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
# Copyright$
#
  
MACSROOT=/ ; export MACSROOT
MACS_LIBEXE=$ROOT/usr/lib/macs
MACS_SETUP=$MACS_LIBEXE/setup

if [ "x`id | grep uid=0`" = "x" ]
then
	echo "You must be root to setup MACS"
	exit 1
fi 

echo "Making MACS support directories."
make -f $MACS_SETUP/macs_dirs.mk MACSROOT=$MACSROOT

$MACS_SETUP/macs_params


echo "Should MACS be started automatically at system boot (y/n)? \c"

if [ "x`line`" = "xy" ]
then
	if [ -f /sbin/rc3.d/S88nqs ]; then
		rm -f /sbin/rc3.d/S88nqs
		ln -s /sbin/init.d/nqs /sbin/rc3.d/S89nqs
	fi
	if [ -f /sbin/rc0.d/K12nqs ]; then
		rm -f /sbin/rc0.d/K12nqs
		ln -s /sbin/init.d/nqs /sbin/rc0.d/K11nqs
	fi
        if [ -f /sbin/rc2.d/K07nqs ]; then     
                rm -f /sbin/rc2.d/K07nqs       
                ln -s /sbin/init.d/nqs /sbin/rc2.d/K06nqs      
        fi
	if [ -f /sbin/rc3.d/S60inetd ]; then
		mv /sbin/rc3.d/S60inetd /sbin/rc3.d/XS60inetd
		ln -s /sbin/init.d/inetd /sbin/rc3.d/S91inetd
	fi
	if [ -f /sbin/rc0.d/K25inetd ]; then
		mv /sbin/rc0.d/K25inetd /sbin/rc0.d/XK25inetd
		ln -s /sbin/init.d/inetd /sbin/rc0.d/K01inetd
	fi
        if [ -f /sbin/rc2.d/K20inetd ]; then     
		mv /sbin/rc2.d/K20inetd /sbin/rc2.d/XK20inetd
                ln -s /sbin/init.d/inetd /sbin/rc2.d/K01inetd      
        fi
	ln -s $MACSROOT/sbin/init.d/macs $MACSROOT/sbin/rc3.d/S88macs
	ln -s $MACSROOT/sbin/init.d/macs $MACSROOT/sbin/rc0.d/K12macs
	ln -s $MACSROOT/sbin/init.d/macs $MACSROOT/sbin/rc2.d/K07macs
else
	echo "MACS may be started or stopped manually with the following command:"
	echo
	echo "/sbin/init.d/macs start"
	echo "and"
	echo "/sbin/init.d/macs stop"
fi

MACDMODE=`awk '/MACDMODE/{print $2}' $MACSROOT/usr/spool/macs/conf/macs.conf`
if [ "$MACDMODE" = "macwatch" ]; then
    echo "MACS is configured in 'macwatch' mode.  In order to prevent users from"
    echo "running when they have exceeded their allocation, the allocator must be"
    echo "configured to enforce MACS limits (by adding 'USE_MACS=1' to the"
    echo "allocator.config file).  "
    echo ""
    echo "Should the allocator be configured to enforce MACS limits (y/n)? \c"
    if [ "x`line`" = "xy" ]; then
        ALLOC_CFG="/etc/nx/allocator.config"
        ALLOC_OLD="/etc/nx/allocator.config.`date +%y.%m.%d`"
        RESTART_ALLOC="no"

        grep -sq 'USE_MACS' $ALLOC_CFG
        case $? in
        0)  # USE_MACS found in file, change it
            cp $ALLOC_CFG $ALLOC_OLD
            sed 's/USE_MACS.*/USE_MACS=1/' < $ALLOC_OLD > $ALLOC_CFG
            diff $ALLOC_CFG $ALLOC_OLD > /dev/null
            if [ $? -eq 0 ]; then
                rm $ALLOC_OLD
                RESTART_ALLOC="no"
                echo ""
                echo "The allocator is already configured to enforce MACS limits."
                echo "The $ALLOC_CFG file was not changed."
            else
                RESTART_ALLOC="yes"
                echo ""
                echo "The allocator has been configured to enforce MACS limits."
                echo "The previous $ALLOC_CFG file has been saved as"
                echo "$ALLOC_OLD."
            fi;;
        1)  # USE_MACS not found in file, add it
            cp $ALLOC_CFG $ALLOC_OLD
            echo "USE_MACS=1" >> $ALLOC_CFG
            RESTART_ALLOC="yes"
            echo ""
            echo "The allocator has been configured to enforce MACS limits."
            echo "The previous $ALLOC_CFG file has been saved as"
            echo "$ALLOC_OLD.";;
        2)  # file not found, create it
            echo "USE_MACS=1" >> $ALLOC_CFG
            RESTART_ALLOC="yes"
            echo ""
            echo "The allocator has been configured to enforce MACS limits."
            echo "A new $ALLOC_CFG file was created.";;
        esac

        if [ "$RESTART_ALLOC" = "yes" ]; then
            if [ `ps -ef | grep -ic 'nqs '` -gt 0 ]; then
                NQS_IS_RUNNING="yes"
            else
                NQS_IS_RUNNING="no"
            fi

            if [ "$NQS_IS_RUNNING" = "yes" ]; then
                echo ""
                echo "The allocator configuration has been changed.  The allocator, SMD, and"
                echo "NQS must be restarted for the change to take effect.  THIS WILL KILL"
                echo "ALL RUNNING APPLICATIONS.  (Running NQS jobs will be restarted from the"
                echo "beginning.)"
                echo ""
                echo "Would you like to restart the allocator, SMD, and NQS now (y/n)? \c"
            else
                echo ""
                echo "The allocator configuration has been changed.  The allocator and SMD"
                echo "must be restarted for the change to take effect.  THIS WILL KILL"
                echo "ALL RUNNING APPLICATIONS."
                echo ""
                echo "Would you like to restart the allocator and SMD now (y/n)? \c"
            fi

            if [ "x`line`" = "xy" ]; then
                if [ "$NQS_IS_RUNNING" = "yes" ]; then
                    /sbin/init.d/nqs stop
                fi
                /sbin/init.d/smd stop
                /sbin/init.d/allocator stop
                /sbin/init.d/allocator start
                /sbin/init.d/smd start
                if [ "$NQS_IS_RUNNING" = "yes" ]; then
                    /sbin/init.d/nqs start
                fi
            else
                echo ""
                echo "The change in the allocator configuration will not take effect until"
                echo "the next time the allocator is restarted.  Until then, users who"
                echo "exceed their allocations will NOT be prevented from running applications."
            fi
        fi
    else
        echo ""
        echo "The allocator has NOT been configured to enforce MACS limits.  Users"
        echo "who exceed their allocations will NOT be prevented from running applications."
    fi
    echo ""

fi

echo "Do you want to start MACS now (y/n)? \c"

if [ "x`line`" = "xy" ]
then
	/sbin/init.d/macs start
fi

echo
echo "MACS setup is complete."
exit 0
