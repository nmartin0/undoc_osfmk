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
 
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 * 
 */

/*
 * rose_namesmd.h -- version for AT386
 * Contains name strings for the machine dependent types and flags in the 
 * OSF/ROSE object format, along with the corresponding values of the 
 * types and flags.
 */

type_desc_t regc_usage_type_array[] = {
	{"REG_TEXT_T", REG_TEXT_T},
	{"REG_DATA_T", REG_DATA_T},
	{"REG_BSS_T", REG_BSS_T},
};
type_desc_t regc_usage_type_stripped_array[] = {
	{"TEXT", REG_TEXT_T},
	{"DATA", REG_DATA_T},
	{"BSS", REG_BSS_T},
};
int regc_usage_type_count = array_count(regc_usage_type_array);

type_desc_t ri_size_type_array[] = {
	{"R_RELBYTE", R_RELBYTE},
	{"R_RELWORD", R_RELWORD},
	{"R_RELLONG", R_RELLONG},
	{"R_PCRBYTE", R_PCRBYTE},
	{"R_PCRWORD", R_PCRWORD},
	{"R_PCRLONG", R_PCRLONG},
	{"R_RRELBYTE", R_RRELBYTE},
	{"R_RRELWORD", R_RRELWORD},
	{"R_RRELLONG", R_RRELLONG},
	{"R_RPCRBYTE", R_RPCRBYTE},
	{"R_RPCRWORD", R_RPCRWORD},
	{"R_RPCRLONG", R_RPCRLONG},
};
type_desc_t ri_size_type_stripped_array[] = {
	{"RELBYTE", R_RELBYTE},
	{"RELWORD", R_RELWORD},
	{"RELLONG", R_RELLONG},
	{"PCRBYTE", R_PCRBYTE},
	{"PCRWORD", R_PCRWORD},
	{"PCRLONG", R_PCRLONG},
	{"RRELBYTE", R_RRELBYTE},
	{"RRELWORD", R_RRELWORD},
	{"RRELLONG", R_RRELLONG},
	{"RPCRBYTE", R_RPCRBYTE},
	{"RPCRWORD", R_RPCRWORD},
	{"RPCRLONG", R_RPCRLONG},
};
int ri_size_type_count = array_count(ri_size_type_array);

