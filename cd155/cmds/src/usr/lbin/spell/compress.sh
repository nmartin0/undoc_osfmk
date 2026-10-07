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
 
# (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
# 
# 
# OSF/1 Release 1.0

# @(#)$RCSfile: compress.sh,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:48:07 $

# 
# IBM CONFIDENTIAL
# Copyright International Business Machines Corp. 1989
# Unpublished Work
# All Rights Reserved
# Licensed Material - Property of IBM
# 
# US Government Users Restricted Rights - Use, duplication or
# disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
#
# compress.sh	1.4 3/3/90 14:40:26
# 
# COMPONENT_NAME: (CMDTEXT) Text Formatting Services
#
# FUNCTIONS:
# 
# ORIGINS: 3
# 
# compress - compress the spell program log
# 

trap 'rm -f /usr/tmp/spellhist;exit' 1 2 3 15
echo "COMPRESSED `date`" > /usr/tmp/spellhist
grep -v ' ' /usr/lbin/spell/spellhist | sort -fud >> /usr/tmp/spellhist
cp /usr/tmp/spellhist /usr/lbin/spell
rm -f /usr/tmp/spellhist
