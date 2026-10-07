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
# Boot mesh nodes
#	boot the mesh
#	log failed nodes for allocator
#	log to booting logfile
#

# validate operation for paragons only
arch=`/sbin/getmagic BOOT_ARCH`
if [ "$arch" != "paragon" ]
then
	exit
fi

# Check to make sure mesh is not already booted
if [ -f /tmp/when_bootmesh_ran ]
then
	boot_time=`/sbin/getmagic BOOT_TIME`
	when_bootmesh_ran=`cat /tmp/when_bootmesh_ran`
	if [ "$when_bootmesh_ran" -gt "$boot_time" ]
	then
		echo "bootmesh already run, skipping mesh booting."
		exit
	fi
fi

if [ ! -f /etc/.forcebootmesh ]
then
	skipit=`/sbin/getmagic DISABLE_BOOTMESH`
	if [ "$skipit" -ne 0 ]
	then
		echo "Skipping mesh booting."
		exit
	fi
fi

########################################################################
# Begin mesh booting
########################################################################
echo "Mesh booting started.  Please wait..."
. /etc/TIMEZONE
echo "Bootmesh " `date` >>/etc/bootmesh.log
epoch_time >/tmp/when_bootmesh_ran
chmod 644 /tmp/when_bootmesh_ran
sync

########################################################################
# Look for the magic string DEBUG_NODE. If set, then this system is being
# setup for server debug. This means we need to disable broadcast and
# boot the nodes 1 at a time. The utility 'bootpp' has already setup the
# the BOOT_NODE_LIST with the debug node removed from it.
########################################################################
DEBUG_BOOTMAGIC=/mach_servers/bootmagic.db
debug_node=`getmagic DEBUG_NODE`
DEBUG_KERNEL=`getmagic BOOT_KERNEL_NAME`
if [ $debug_node ]; then
        echo "*** Booting Debug Node: $debug_node"
        /sbin/bootmesh -P -v -n $debug_node \
                -m $DEBUG_BOOTMAGIC -k $DEBUG_KERNEL -S -E
        echo ""
        echo "*** Booting rest of the mesh."
        bad=`/sbin/bootmesh -v`
else
	bad=`/sbin/bootmesh -b`
fi

########################################################################
# Store the bad nodes into the file "/etc/nx/.badnodes
########################################################################
echo "$bad" >/etc/nx/.badnodes
if [ -z "$bad" ]
then
  echo "Successful" >>/etc/bootmesh.log
else
  echo "Failed:" >>/etc/bootmesh.log
  echo "$bad" >>/etc/bootmesh.log
fi
echo "Mesh booting done."
/sbin/sync
