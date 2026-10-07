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
# Reset the BOOT_NODE_LIST bootmagic if necessary because of failed nodes
#

# Call reset_boot_node_list() to remove nodes in /etc/nx/.badnodes
/sbin/reset_bnl

# Shutdown if command failed because of unrecoverable error
if [ $? -eq 1 ]
then
	/sbin/sync;/sbin/sync;/sbin/sync
	/sbin/halt
fi
