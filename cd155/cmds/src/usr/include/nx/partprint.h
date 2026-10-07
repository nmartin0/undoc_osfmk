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
 
extern void print_part_info(int from_lspart,int restrictflg,
		int rflg,int *print_name,
                char *part_name,char *name,char *indent_str,
                nx_part_info_t *part, nx_node_list_t lp_map);

extern void print_part_info_ext(int from_lspart,int restrictflg,
		int rflg,int *print_name,
                char *part_name,char *name,char *indent_str,
                nx_part_info_t *part, nx_node_list_t lp_map,
		int lflg, int physflg);

extern void print_heading(int from_lspart,char *indent_str, int lflg);
