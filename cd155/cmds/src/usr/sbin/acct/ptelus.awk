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
# (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
# ALL RIGHTS RESERVED
#
#
# OSF/1 Release 1.0

# 
# 	@(#) $RCSfile: ptelus.awk,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:03:25 $
# 
# 	 COMPONENT_NAME: (CMDACCT) Command Accounting
# 
# 	 FUNCTIONS: none
# 
# 	 ORIGINS: 3, 9, 27
# 
# 	 (C) COPYRIGHT International Business Machines Corp. 1985, 1989
# 	 All Rights Reserved
# 	 Licensed Materials - Property of IBM
# 
# 	 US Government Users Restricted Rights - Use, duplication or
# 	 disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
# 
# 	ptelus.awk	1.2  com/cmd/acct,3.1,8943 10/24/89 10:58:37
#

BEGIN	{
	MAXCPU = 20.		# report if cpu usage is greater than this
	MAXKCORE = 500.		# report is Kcore usage is greater than this
	MAXCONNECT = 120.	# report if connect time is greater than this
	}
NR == 1	 {
	MAXCPU = MAXCPU + 0
	MAXKCORE = MAXKCORE + 0
	MAXCONNECT = MAXCONNECT + 0
	printf "Logins with exceptional Prime/Non-prime Time Usage\n"
	printf ( "CPU > %d or KCORE > %d or CONNECT > %d\n\n\n", MAXCPU, MAXKCORE, MAXCONNECT)
	printf "\tLogin\t\tCPU (mins)\tKCORE-mins\tCONNECT-mins\tdisk"
	printf "\t# of\t# of\t# Disk\tfee\n"
	printf "UID\tName\t\tPrime\tNprime\tPrime\tNprime\t"
	printf "Prime\tNprime\tBlocks\tProcs\tSess\tSamples\n\n"
	}
$3 > MAXCPU || $4 > MAXCPU || $5 > MAXKCORE || $6 > MAXKCORE || $7 > MAXCONNECT || $8 > MAXCONNECT {
				printf("%d\t%-8.8s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", $1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13)
				}
