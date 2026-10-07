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
# @(#) $RCSfile: ptecms.awk,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:03:24 $
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
# 	ptecms.awk	1.2  com/cmd/acct,3.1,8943 10/24/89 10:58:22
#

BEGIN {
	MAXCPU = 20.0		# report if cpu usage greater than this
	MAXKCORE = 1000.0	# report if KCORE usage is greater than this
	}
NF == 4		{
			print "\t\t\t\t" $1 " Time Exception Command Usage Summary"
		}

NF == 3		{
			print "\t\t\t\tCommand Exception Usage Summary"
		}

NR == 1		{
			MAXCPU = MAXCPU + 0.0
			MAXKCORE = MAXKCORE + 0.0
			print "\t\t\t\tTotal CPU > " MAXCPU " or Total KCORE > " MAXKCORE
		}

NF <= 4 && length != 0	{
				next
			}

$1 == "COMMAND" || $1 == "NAME"		{
						print
						next
					}

NF == 10 && ( $4 > MAXCPU || $3 > MAXKCORE ) && $1 != "TOTALS"

NF == 13 && ( $5 + $6 > MAXCPU || $4 > MAXKCORE ) && $1 != "TOTALS"

length == 0


