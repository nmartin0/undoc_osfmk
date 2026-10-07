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
# Copyright (c) 1992, Intel Corporation
# All rights reserved
#
# Copyright (c) 1991, Locus Computing Corporation
# All rights reserved
#
# HISTORY
# $Log: pfs_async.sh,v $
# Revision 1.3  1994/11/18  20:30:02  mtm
# Copyright additions/changes
#
# Revision 1.2  1993/04/03  03:05:45  brad
# Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
# Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
#
# Revision 1.1.2.1  1993/02/23  04:32:28  brad
# Added ability to handle MiG SimpleRoutines as well as Routines.
# E.g. this allows this script to be used on fsvr_user.c.
#
# Revision 1.1  1992/12/16  06:10:33  brad
# Initial revision
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
# This script handles MiG SimpleRoutines as well as Routines.
#
sed -e "
	s/mach_port_t mig_get_reply_port();/mach_port_t pfs_get_reply_port();/
	s/void mig_dealloc_reply_port();/void pfs_dealloc_reply_port();/
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
		s/_port, /_port, reply_port, /
		n
		a\\
		\	mach_port_t reply_port;
	:mig
		n
		/Head.msgh_reply_port = /!b mig
		s/mig_get_reply_port()/reply_port/
	:msg
		n
		/mach_msg(&InP->Head,/!b msg
		s/(\([^,]*\),\([^,]*\),\([^,]*\),\([^,]*\),\([^,]*\),/(\1,MACH_SEND_MSG,\3,0,MACH_PORT_NULL,/
		/return mach_msg(&InP->Head,/b skip
		a\\
		\	return (msg_result);
	:skip
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
	s/mach_port_t mig_get_reply_port();/mach_port_t pfs_get_reply_port();/
	s/void mig_dealloc_reply_port();/void pfs_dealloc_reply_port();/
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
		s/_port, /_port, reply_port, /
		n
		a\\
		\	mach_port_t reply_port;
	:mig
		n
		/Head.msgh_reply_port = /!b mig
		s/mig_get_reply_port()/reply_port/
	:msg
		n
		/mach_msg(&InP->Head,/!b msg
		s/(\([^,]*\),\([^,]*\),\([^,]*\),\([^,]*\),\([^,]*\),/(\1,MACH_RCV_MSG,0,\4,\5,/
		/return mach_msg(&InP->Head,/b endfn
	:pfs
		n
		/mig_dealloc_reply_port()/!b pfs
		s/mig_dealloc_reply_port()/pfs_dealloc_reply_port()/
	:endfn
	}
	" $input_file >${mig_less_dot_c}_receive.c
