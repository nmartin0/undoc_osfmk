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
 
/*****************************************************************************
 *              Copyright (c) 1990 San Diego Supercomputer Center.
 *              All rights reserved.  The SDSC software License Agreement
 *              specifies the terms and conditions for redistribution.
 *
 * File:        get.h
 *
 * Author:
 *      Max Okumoto
 *	Thu Sep 17 18:33:02 PDT 1992
 *
 * Abstract:
 *****************************************************************************/

#ifdef __STDC__
#   define  __(_A) _A
#else
#   define  __(_A) ()
#   define const
#endif

extern int	get_acct __((int));
extern char	**get_accts __((int));
extern char	*get_adesc __((void));
extern char	*get_aid __((void));
extern char	*get_aname __((void));
extern int	get_line __((char *, int));
extern int	get_menu_item __((int));
#if 0 /* THIS CODE IS NOT USED */
extern char	*get_uid __((void));
extern char	*get_uname __((void));
extern char	*get_user __((char *, char *, char *));
#endif /* THIS CODE IS NOT USED */
extern char	**get_users __((char **, int));
extern int	get_yes_no __((char *));

extern char	**get_new_users __((void));
