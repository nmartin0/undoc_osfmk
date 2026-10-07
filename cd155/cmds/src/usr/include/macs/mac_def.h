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
 * File:	mac_def.h
 *
 * This header file and the header file mac_ui.h 
 * together contains all definitions for MACS 
 * administration applications (macalloc, macadmin)
 * user interface set-up.  The set-up allows 
 * applications for both command-line execution or 
 * interactive menu-driven session.
 *
 * All MACS administration functional units are 
 * defined in mac_adm.c and user-menu-interface
 * functional unit is defined in mac_ui.c and libmac.a
 *
 * User input collected through the interactive
 * menu-driven session is translated into command-
 * line options by mac_ui and sent to mac_adm for
 * execution.
 *
 * To add new functional units, you need only to
 * define the function in mac_adm.c, add it into menu-function
 * mapping definitions in this and mac_ui.h header
 * files then recompile the applications.
 *
 * To create a new application with a sub-set of
 * the defined menu-items here, you need only to
 * add a new definition for the new menu selection
 * in mac_ui.h and add a line of instruction for
 * the target application in the makefile
 */

#ifndef MAC_DEF_H
#define MAC_DEF_H

/*
 * maximum number of account ids or user ids allowed on
 * MACS utilities command line
 */
#define MAX_AGIDS (512)
#define MAX_UIDS  (1024)

/*
 * default values for account attributes
 */

#define DEF_ACCESS	"U"
#define DEF_PERCENT	"100"
#define DEF_MINUTES	"0"
#define DEF_WEIGHT	"1"
#define DEF_KILL	"0"
#define DEF_LOCK	"0"
#define DEF_UNLIMIT	"0"

/*
 * string-field length
 */

#define MAX_ACCT_NAME_LEN	(16)
#define MAX_ACCT_ID_LEN		(8)
#define MAX_ACCT_DESC_LEN	(64)
#define MAX_ACCT_LIST_LEN	(256)
#define MAX_USER_LOGIN_LEN	(16)
#define MAX_USER_ID_LEN		(8)
#define MAX_USER_LIST_LEN	(256)
#define MAX_USER_LEN		(16)
#define MAX_ACCT_WT_LEN		(2)
#define MAX_ACCT_KILL_LEN	(2)
#define MAX_ACCT_LOCK_LEN	(2)
#define MAX_USER_ACCESS_LEN	(5)
#define MAX_USER_PCNT_LEN	(6)
#define MAX_MINUTES_LEN		(16)
#define MAX_NODES_LEN		(5)

#define DEF_NODES		(1)

/*
 * bit map for input items
 */

#define ACCTS_NAME_OR_ID	(1)
#define ACCT_NAME_AND_ID	(1<<1)
#define ACCTS_USERS_NAME_OR_ID	(1<<2)
#define ACCTS_NEW_USERS_NAME_OR_ID	(1<<3)
#define TWO_ACCT_NAME_OR_ID	(1<<4)
#define ACCOUNT_WEIGHT		(1<<5)
#define ACCOUNT_KILL		(1<<6)
#define ACCOUNT_LOCK		(1<<7)
#define USER_ACCESS		(1<<8)
#define USER_PERCENT		(1<<9)
#define ALLOC_MINUTES		(1<<10)
#define MAX_NODES		(1<<11)
#define N_FUNC_MASK		(12)

/*
 * main menu (function) item index
 * Note:  Be careful.  Many other definitions
 * in this file use this index, it's indicated
 * in paragraph-comments.  Preserve the
 * consistency among them are critical.
 */

#define LIST_ACCT_IDX	0	/* list accounts info */
#define ADD_ACCT_IDX	1	/* add a new account */
#define DEL_ACCT_IDX	2	/* delete  accounts */
#define SET_ACCT_IDX	3	/* change accounts */
#define MOV_ALLOC_IDX	4	/* transfer between accounts */
#define LIST_USER_IDX	5	/* list user accounts info */
#define ADD_USER_IDX	6	/* add user accounts */
#define DEL_USER_IDX	7	/* delete user accounts */
#define CHG_USER_IDX	8	/* change user accounts */
#define N_MENU_ITEM	9

/*
 * meanings of skipping an input
 */

#define SKIP_DEFAULT	0
#define SKIP_UNCHANGE	1
#define SKIP_TRANSFER	2

/* 
 * command-line option index 
 */

#define NEW_ACCT_IDX		(0)	/* new account group name,id,description */
#define ACCTS_NAME_OR_ID_IDX	(1)	/* account group name or id list */
#define USERS_LOGIN_OR_ID_IDX   (2)	/* user name or id list */
#define FROM_ACCT_IDX		(3)	/* from a single account name or id */
#define TO_ACCT_IDX		(4)	/* to a single account name of id */
#define ACCT_WEIGHT_IDX		(5)	/* weight flag */
#define ACCT_KILL_IDX		(6)	/* kill flag */
#define ACCT_LOCK_IDX		(7)	/* lock flag */
#define USER_ACCESS_IDX		(8)	/* user permission */
#define USER_PERCENT_IDX	(9)	/* allocation percentage */
#define ALLOC_MINUTES_IDX	(10)	/* allocation time */
#define MAX_NODES_IDX		(11)	/* maximum number of nodes */
#define N_INFO_OPTION		(13)
#define UNLIMIT_IDX		(12)	/* unlimit allocation time */

/* 
 * Each function unit above returns the number of command-line
 * options it translated, normally 1 or 2.  OVERSIGN is set
 * to a number greater then all the normal returns.
 * Return value of all the above routine should be less than
 * this value except when it wants to signal caller to skip
 * soliciting for next input item.  For example, if an
 * user has specified percentage of allocation, then
 * userpercent() will return #-of-options + OVERSIGN
 * and telling the caller that allocminutes()
 * should be skipped.
 */

#define OVERSIGN	3

/* 
 * M T U masks 
 */

#define MODIFY_MASK     (1<<2)
#define TRANSFER_MASK   (1<<1)
#define USE_MASK        1

/*
 * Tell mac_adm if input arguments have been checked
 * or not.  If the options are translated by the
 * interactive menu-driven user interface, then they
 * have been checked, mac_adm then can assume so to
 * save some work; otherwise, mac_adm need to validate
 * all the option arguments
 */

#define NOT_CHECKED	0
#define CHECKED		1

/*
 * functional units that gets the input item
 * from user and translate to command-line
 * options
 */
#ifdef __STDC__
#   define  __(args) args
#else
#   define  __(args) ()
#   define const
#endif


/*---------------------------------------------------------------------------*
 *	calls defined in checkinfo.c
 *---------------------------------------------------------------------------*/
extern int	acct_name_and_id __((char **, int));
extern int	accts_name_or_id __((char **, int));
extern int	accts_users_name_or_id __((char **, int));
extern int	accts_new_users_name_or_id __((char **, int));
extern int	two_acct_name_or_id __((char **, int));
extern int	acctweight __((char **, int));
extern int	acctkill __((char **, int));
extern int	acctlock __((char **, int));
extern int	useraccess __((char **, int));
extern int	userpercent __((char **, int));
extern int	allocminutes __((char **, int));
extern int	maxnodes __((char **, int));

/*---------------------------------------------------------------------------*
 *	calls defined in exist.c
 *---------------------------------------------------------------------------*/
extern int	acct_exist_ok __((char *));
extern int	accts_exist_ok __((char **));
extern int	accts_users_exist_ok __((char **, char **));
extern int	users_exist_ok(char **users);

/*---------------------------------------------------------------------------*
 *	calls defined in get.c
 *---------------------------------------------------------------------------*/
extern int	get_acct __((int));
extern char	**get_accts __((int));
extern char	*get_adesc __((void));
extern char	*get_aid __((void));
extern char	*get_aname __((void));
extern int	get_line __((char *, int));
extern int	get_menu_item __((int));
extern char	**get_users __((char **, int));
extern int	get_yes_no __((char *));

extern char	**get_new_users __((void));

/*---------------------------------------------------------------------------*
 *	calls defined in syntax.c
 *---------------------------------------------------------------------------*/
extern int	acct_desc_syntex_ok __((char *));
extern int	acct_id_syntex_ok __((char *));
extern int	acct_name_syntex_ok __((char *));

extern int	user_id_syntex_ok __((char *));
extern int	user_name_syntex_ok __((char *));

extern int	percent_syntex_ok __((char *));


/*---------------------------------------------------------------------------*
 * External variables containing the menus
 *---------------------------------------------------------------------------*/
extern char	menu_option;
extern char	*menu_arg[];
extern int	menu_def[];
extern char	*menu_line[];
extern int	menu_info_mask[];
extern char	op_code[];
extern int	(*check_info[])();

#endif /* MAC_DEF_H */
