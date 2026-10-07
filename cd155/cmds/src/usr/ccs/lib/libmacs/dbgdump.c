/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
#include <stdio.h>
#include <sys/types.h>
#include "mac.h"
/* This function here for use while debugging */

void dbgdump(data)
  struct cpu_ctrl *data;
{
	printf("struct cpu_ctrl:\n");
	printf("\tuid=%d\n",data->uid);
	printf("\tagid=%d\n",data->agid);
	printf("\ttimestamp=%d\n",data->info.timestamp);
	printf("\tstruct cpu_lblk info:\n");
	printf("\t\tid=%d\n",data->info.id);
	printf("\t\tweight=%d\n",data->info.weight);
	printf("\t\tinhibit=%d\n",data->info.inhibit);
	printf("\t\tmodify=%d\n",data->info.modify);
	printf("\t\ttransfer=%d\n",data->info.transfer);
	printf("\t\tuse=%d\n",data->info.use);
	printf("\t\tkilljobs=%d\n",data->info.killjobs);
	printf("\t\tlockjobs=%d\n",data->info.lockjobs);
	printf("\t\tunlimit=%d\n",data->info.unlimit);
	printf("\t\tpercent=%d\n",data->info.percent);
	printf("\t\tauthorized=%f\n",data->info.authorized);
	printf("\t\tused=%d\n",data->info.used_time);
	printf("\tseq=%d\n",data->seq);
}
