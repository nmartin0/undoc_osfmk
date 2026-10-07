#!/bin/sh
# 
# $Copyright
# Copyright 1993, 1994 , 1995 Intel Corporation
# INTEL CONFIDENTIAL
# The technical data and computer software contained herein are subject
# to the copyright notices; trademarks; and use and disclosure
# restrictions identified in the file located in /etc/copyright on
# this system.
# Copyright$
# 
 
#
# Copyright (c) 1991, Locus Computing Corporation
# All rights reserved
#
# HISTORY
# $Log: mig_async.sh,v $
# Revision 1.5  1994/11/18  20:29:48  mtm
# Copyright additions/changes
#
# Revision 1.4  1993/07/14  17:54:26  cfj
# OSF/1 AD 1.0.4 code drop from Locus.
#
# Revision 1.1.1.3  1993/07/01  19:12:51  cfj
# Adding new code from vendor
#
# Revision 1.3  1993/05/06  19:14:54  cfj
# ad103+tnc merged with Intel code.
#
# Revision 1.1.1.1  1993/05/03  17:28:03  cfj
# Initial 1.0.3 code drop
#
# Revision 1.2  1992/11/30  22:19:27  dleslie
# Copy of NX branch back into main trunk
#
# Revision 1.1.2.1  1992/11/05  23:18:38  dleslie
# Local changes for NX through noon, November 5, 1992.
#
# Revision 2.2  1992/06/08  18:19:50  pjg
# 	Checked-in from Locus (pjg).
#
# 
#
# This script processes a given MIG-generated C file, <mig>.c, containing
# synchronous RPC functions to produce a set of asynchronous pairs of
# routines separately to send and receive reply for each RPC.
# Two output files are produced: <mig>_send.c, containing the send
# functions and <mig>_receive.c containing the receive functions.
#
# The input file is passed through sed once to generate each of the output
# files.
#

input_file=$1
mig_less_dot_c=`expr "$input_file" : "\(.*\)\.c"`

#
# First, generate the send routines.
# Scan looking for each function declaration; adding reply_port as the first
# parameter; substituting this in place of mig_get_reply_port(); change
# the parameters of the call to mach_msg() to perform a send with no receive;
# return with its return and delete all remaining lines in the function.  
#
sed -e "
	/^mig_external kern_return_t/{
		s/$/_msg_send/
	:STD
		n
		/^(/!b STD
		n
		a\\
		\	mach_port_t reply_port,
	:elseSTD
		n
		/#else/!b elseSTD
		n
		s/(server_port, /(server_port, reply_port, /
		n
		a\\
		\	mach_port_t reply_port;
	:mig
		n
		/mig_get_reply_port()/!b mig
		s/mig_get_reply_port()/reply_port/
	:msg
		n
		/msg_result = mach_msg(/!b msg
		s/(\([^,]*\),\([^,]*\),\([^,]*\),\([^,]*\),\([^,]*\),/(\1,MACH_SEND_MSG,\3,0,MACH_PORT_NULL,/
		a\\
		\	return (msg_result);
		n
	:delete
		/^}/b endfn
		s/^.*\$//
		N
		s/\n//
		b delete
	}
	:endfn
	" $input_file >${mig_less_dot_c}_send.c

#
# Second, generate the receive routines.
# Scan looking for each function declaration; adding reply_port as the first
# parameter; substituting this in place of mig_get_reply_port(); change
# the parameters of the call to mach_msg() to perform a receive only.
#
sed -e "
	/^mig_external kern_return_t/{
		s/$/_msg_receive/
	:STD
		n
		/^(/!b STD
		n
		a\\
		\	mach_port_t reply_port,
	:elseSTD
		n
		/#else/!b elseSTD
		n
		s/(server_port, /(server_port, reply_port, /
		n
		a\\
		\	mach_port_t reply_port;
	:mig
		n
		/mig_get_reply_port()/!b mig
		s/mig_get_reply_port()/reply_port/
	:msg
		n
		/msg_result = mach_msg(/!b msg
		s/(\([^,]*\),\([^,]*\),\([^,]*\),\([^,]*\),\([^,]*\),/(\1,MACH_RCV_MSG,0,\4,\5,/
	}
	" $input_file >${mig_less_dot_c}_receive.c

