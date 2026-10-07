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
 * Copyright (c) 1990 San Diego Supercomputer Center.
 * All rights reserved.  The SDSC software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * This header file along with mac_def.h contains all definitions and 
 * mappings for the system-set-up of a menu-driven user interface used 
 * by MACS utilities.
 *
 * This header is only to be included in the main routine of
 * menu driver source file mac_ui.c.  It must have #include mac_def.h
 * proceeds its inclusion.
 */
 
#ifndef MAC_UI_H
#define MAC_UI_H

/*
 * Corresponding command-line option and arguments for main menu (function)
 * items.  The index here must match above main menu index definitions
 */
char menu_option = 'm';
char *menu_arg[N_MENU_ITEM] = {
	"la",	/* list accounts info */
	"aa",	/* add a new account */
	"da",	/* delete  accounts */
	"ca",	/* change accounts */
	"ta",	/* transfer between accounts */
	"lu",	/* list user accounts info */
	"au",	/* add user accounts */
	"du",	/* delete user accounts */
	"cu"};	/* change user accounts */

/* 
 * what skip-input means under each menu selection 
 * The index are correspond to the main menu item index
 */

int menu_def[N_MENU_ITEM] = {	
	SKIP_DEFAULT,
	SKIP_DEFAULT,
	SKIP_UNCHANGE,
	SKIP_UNCHANGE,
	SKIP_TRANSFER,
	SKIP_DEFAULT,
	SKIP_DEFAULT,
	SKIP_UNCHANGE,
	SKIP_UNCHANGE,
};
					
/*
 * Menu item descriptions which will be
 * displayed with index number as menu
 * This index must correspond to
 * the main menu item index
 */

char *menu_line[N_MENU_ITEM] = {
	"List Account Information",
	"Add New Accounts",
	"Delete Accounts From Database",
	"Change Account Settings",
	"Transfer Account Allocation",
	"List User Account Information",
	"Add Users To Accounts",
	"Delete Users From Accounts",
	"Change User Account Settings",
};

/*
 * Bitmaps to indicate what are the input items for each menu (function)
 * selection.  This index must correspond to main menu item index
 */

int menu_info_mask[N_MENU_ITEM] = {
	/* list account */
	(ACCTS_NAME_OR_ID),
	/* add a new account */
	(ACCT_NAME_AND_ID | ACCOUNT_WEIGHT | ACCOUNT_KILL | ACCOUNT_LOCK | ALLOC_MINUTES | MAX_NODES),
	/* delete accounts */
	(ACCTS_NAME_OR_ID),
	/* change accounts setting */
	(ACCTS_NAME_OR_ID | ACCOUNT_WEIGHT | ACCOUNT_KILL | ACCOUNT_LOCK | ALLOC_MINUTES | MAX_NODES),
	/* transfer between accounts */
	(TWO_ACCT_NAME_OR_ID | USER_PERCENT | ALLOC_MINUTES),
	/* list users' accounts */
	(ACCTS_USERS_NAME_OR_ID),
	/* add users accounts */
	(ACCTS_NEW_USERS_NAME_OR_ID | USER_ACCESS | USER_PERCENT | ALLOC_MINUTES | MAX_NODES),
	/* delete users' accounts */
	(ACCTS_USERS_NAME_OR_ID),
	/* change users' accounts */
	(ACCTS_USERS_NAME_OR_ID | USER_ACCESS | USER_PERCENT | ALLOC_MINUTES | MAX_NODES)
};

/*
 * command-line option code
 * this index must correspond to
 * command-line option index definition
 */

char op_code[N_INFO_OPTION] = {
	'A',	/* new account group name,id,description */
	'a',	/* account group name or id list */
	'u',	/* user name or id list */
	'F',	/* from a single account name or id */
	'T',	/* to a single account name of id */
	'w',	/* weight flag */
	'N',	/* No_kill flag */
	'l',	/* lock flag */
	'p',	/* user permission */
	'%',	/* allocation percentage */
	't',	/* allocation time */
	'n',	/* maximum number of nodes */
	'r'	/* unlimit allocation time */
};

/*
 * Corresponding functional unit to each input item.  This index must be
 * consistent with command-line option index
 */
int (*check_info[])() = {
	accts_name_or_id,
	acct_name_and_id,
	accts_users_name_or_id,
	accts_new_users_name_or_id,
	two_acct_name_or_id,
	acctweight,
	acctkill,
	acctlock,
	useraccess,
	userpercent,
	allocminutes,
	maxnodes
};

#ifdef MACADMIN

/* 
 * Mapping for application macadmin
 */

#define TITLE_LINE "\n\n\tMACS ADMINISTRATION MENU\n\t========================\n\n"

/*
 * macadmin menu mapping 
 */

#define N_APPL_MENU 9
static int appl_menu[N_APPL_MENU] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };

#endif /* MACADMIN */

#ifdef MACALLOC

/* 
 * Mapping for application macalloc
 */

#define TITLE_LINE "\n\n\tMACS ACCOUNT MANAGEMENT MENU\n\t============================\n\n"

/*
 * macalloc menu mapping 
 */

#define N_APPL_MENU 5
static int appl_menu[N_APPL_MENU] = { 0, 3, 4, 5, 8 };

#endif	/* ifdef MACALLOC */

#endif	/* ifdef MAC_UI_H */
