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
 
/****************************************************************************
 **                                                                        **
 **                (C) Copyright 1992 Intel Corporation                    **
 **                        All rights reserved                             **
 **                                                                        **
 **              INTEL CORPORATION PROPRIETARY INFORMATION                 **
 **                                                                        **
 **  This software is supplied under the terms of a license agreement or   **
 **  nondisclosure agreement with Intel Corporation and may not be copied  **
 **  or disclosed except in accordance with the terms of that agreement.   **
 **                                                                        **
 **                                                                        **
 ****************************************************************************/

/***************************************************************************
 *
 *    Title:
 *        guard.c
 *        $Id: guard.c,v 1.5 1994/11/19 02:11:53 mtm Exp $
 *
 *    Description:
 *        Routine to be placed at the end of user code to delimit it
 *        from system code.
 *
 **************************************************************************/

/***************************************************************************
 *
 *    Calling Sequence:
 *        __user_code_end_delimiter__ ()
 *
 *    Description:
 *        Not intended to be called. This routine is placed between user and
 *        system code by the linker. Debuggers and other programming tools
 *        can easily differentiate user code and system code by finding the
 *        symbol for this routine.
 *
 *    Parameters:
 *        none.
 *
 *    Returns:
 *        Does not return unless there is a SIGABRT handler, in which case
 *        it returns -1.
 *
 **************************************************************************/

/*
 *  declaring _____perfmon will force the linker to pull in libpm.a. It is
 *  not declared "extern" so that if the user explicitly says don't link in
 *   libpm.a the external reference will resolve.
 */
int _____perfmon;

int
__user_code_end_delimiter__ ()
{
    (void)abort();
    return -1;
}
