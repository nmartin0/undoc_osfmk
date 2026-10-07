/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
#include <stdio.h>

main()
{
	char * temp_string;

	temp_string = (char *) getenv("sleep");
	if (temp_string == NULL){
		printf("Error: variable sleep is not set\n");
		exit(0);
	}
	printf("sleep_time is %d\n", atoi(temp_string));
}
