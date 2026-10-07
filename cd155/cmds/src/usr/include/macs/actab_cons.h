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
 
char *sort_key_grp[SORT_LEVELS] = { "al", "q", "pciut" };
char *sort_key_name[SORT_KEYS] = {
	/* sequence corresponding to DEFAULT_SORT */
	"account", 
	"user",
	"job type",
	"average partition size",
	"total cpu-time",
	"total idle cpu-time",
	"total under-used cpu-time",
	"total charge"
};

/* corresponding names of queue types */
char *qtype_name[3] = { "INTERACT", "BATCH", "DEDICATE" };
