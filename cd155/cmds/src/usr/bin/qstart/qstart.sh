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
 

NOSCHED=`awk -F: '/nosched/{print $2}' /usr/spool/nqs/conf/sched_param | 
	 sed 's/[ 	]//g'`
# note that between the [ ] above are two characters: a space and a tab

if [ -z "$NOSCHED" -o "$NOSCHED" = "0" ];  

then

   QUEUE=`qstat -b | egrep AVAILBL | head -1 | awk '{print $1}'`

   if [ -z "$QUEUE" ];
   then
       echo "No queues available -- NQS scheduling not restarted."
       exit 1
   fi

   echo "sleep 1" | qsub -q $QUEUE -lP 1 -lT 1 -o /dev/null -eo /dev/null > /dev/null
   echo "NQS scheduling restarted"

else

   echo "The 'nosched' parameter in the file /usr/spool/nqs/conf/sched_param"
   echo "is set to '$NOSCHED'.  It must be set to 0, or removed, to restart NQS"
   echo "scheduling.  Please fix it and try again."
   exit 1

fi



