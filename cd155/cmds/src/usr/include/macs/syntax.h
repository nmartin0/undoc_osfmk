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
 * File:        syntax.h
 *
 * Abstract:
 *****************************************************************************/
#ifdef __STDC__
#   define  __(_A) _A
#else
#   define  __(_A) ()
#   define const
#endif

extern int	acct_desc_syntex_ok __((char *));
extern int	acct_id_syntex_ok __((char *));
extern int	acct_name_syntex_ok __((char *));

extern int	user_id_syntex_ok __((char *));
extern int	user_name_syntex_ok __((char *));

extern int	percent_syntex_ok __((char *));

