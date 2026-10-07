/*
 *
 * $Copyright
 * Copyright 1992, 1993, 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 *
 */ 
static char *usage_descr =
"Usage:\n\tamd -z<hw-addr> -n<rf-window> -i<pl-interval> -r<rf-cycle> [-s]\n\t[-f<file>]\n\tamd -z<hw_addr> {-d|-k{message-number-list|all}}\n";
main()
{
	printf( "%s", usage_descr );
}
