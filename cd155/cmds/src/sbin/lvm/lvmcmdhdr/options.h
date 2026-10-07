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
 */

/*
 *   options.h
 *   
 *   Contents:
 *	Command line parsing has been highly automatised, in the sense
 *	that all options and arguments legal to each command have been
 *	extracted by the same source of information from which the manual
 *	pages were generated. Here are the functions to handle this
 *	information.
 *	See the LVM commands library module "options.c".
 */

int parse_args(int *argcp, char ***argvp, char *opt_without_val,
   	       char *opt_with_val, unsigned int required_args);
int used_opt(char key);
char *value_of_opt(char key);
char *program_name();
char *next_arg();
char **left_arg(int *numptr);
int bad_int_arg_value(char *flagptr, int *valptr, char flag, char *flagmeaning);
int bad_char_arg_value(char *flagptr, char *valptr, char flag,
		       char *flagmeaning, char *goodvalues);
int usage_error(char *bad_inp, char *err_msg);
int print_arg_error();
int print_usage(register char *usage_msg);
