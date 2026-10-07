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
 *		Copyright (c) 1990 San Diego Supercomputer Center.
 *		All rights reserved.  The SDSC software License Agreement
 *		specifies the terms and conditions for redistribution.
 *
 * File:	exist.h
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

extern int	acct_exist_ok __((char *));
extern int	accts_exist_ok __((char **));
#if 0 /* THIS CODE IS NOT USED */
extern char	*user_exist_ok __((char *, char *, char *));
#endif /* THIS CODE IS NOT USED */
extern int	users_exist_ok __((char **, char **));

extern int	users_id_ok(char **users);

