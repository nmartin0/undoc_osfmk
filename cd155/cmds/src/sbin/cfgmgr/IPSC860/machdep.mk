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
# OSF/1 Release 1.0.1
#

IPSC860_CFLAGS= -DINET_METHOD -DSTREAMS_METHOD -DXTI_METHOD -DXNS_METHOD -DFILSYS_METHOD 

IPSC860_OFILES=  method_streams.o	\
	  	method_filsys.o 	\
	  	method_xti.o 		\
	  	method_xns.o 		\
	  	method_inet.o		\
		cm_method_defs.o
