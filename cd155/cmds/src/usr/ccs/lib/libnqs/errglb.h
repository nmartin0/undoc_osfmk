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
 
/**********************************************************************
***********************************************************************
**                                                                   **
**  NOTICE:                                                          **
**                                                                   **
**  Significant portions of UniTree Release 1 software were          **
**  developed using private DISCOS funding, however, part of this    **
**  program resulted from work developed under a United States       **
**  Government contract and the following legend applies to the      **
**  government developed portions of the code.                       **
**                                                                   **
**  The Government retains for itself and others acting on its       **
**  behalf a paid-up, non-exclusive, irrevocable worldwide license   **
**  in such copyrighted data to reproduce, prepare derivative works, **
**  distribute copies to the public, and perform publicly and        **
**  display publicly, by or on behalf of the Government.             **
**                                                                   **
**  Neither the Government, nor any of its employees, makes any      **
**  warranty, expressed or implied, or assumes any legal liability   **
**  or responsibilty for the accuracy, completeness, or usefulness   **
**  of any information, apparatus, product, or process disclosed,    **
**  or represents that its use would not infringe privately owned    **
**  rights.                                                          **
**                                                                   **
***********************************************************************
**********************************************************************/

/**********************************************************************
**                                                                   **
**        (C) Copyright 1990, 1991  General Atomics                  **
**                    All Rights Reserved.                           **
**                                                                   **
**********************************************************************/

#if !defined(lint)
/* static char	sccs_id[] = "@(#)errglb.h	162.7 (UNITREE) 1/20/92"; */
#endif

/*
 * Variables used for error logging
 */
                                      /* APST error numbers */
#define  APSTERR_1           1
#define  APSTERR_2           2
#define  APSTERR_3           3
#define  APSTERR_4           4
#define  APSTERR_5           5
#define  APSTERR_6           6
#define  APSTERR_7           7
#define  APSTERR_8           8
#define  APSTERR_9           9
#define  APSTERR_10         10
#define  APSTERR_11         11
#define  APSTERR_12         12
#define  APSTERR_13         13
#define  APSTERR_14         14
#define  APSTERR_15         15
#define  APSTERR_16         16
#define  APSTERR_17         17
                                      /* diskmovr error numbers */
#define  DISKMOVRERR_1       1
#define  DISKMOVRERR_2       2
#define  DISKMOVRERR_3       3
#define  DISKMOVRERR_4       4
#define  DISKMOVRERR_5       5
#define  DISKMOVRERR_6       6
#define  DISKMOVRERR_7       7
                                      /* filesrvr error numbers */
#define  FILESRVRERR_1       1
#define  FILESRVRERR_2       2
#define  FILESRVRERR_3       3
#define  FILESRVRERR_4       4
#define  FILESRVRERR_5       5
                                      /* disksrvr error numbers */
#define  DISKSRVRERR_1       1
#define  DISKSRVRERR_2       2
#define  DISKSRVRERR_3       3
#define  DISKSRVRERR_4       4
#define  DISKSRVRERR_5       5
#define  DISKSRVRERR_6       6
#define  DISKSRVRERR_7       7
#define  DISKSRVRERR_8       8
#define  DISKSRVRERR_9       9
                                      /* tapesrvr error numbers */
#define  TAPESRVRERR_1       1
#define  TAPESRVRERR_2       2
#define  TAPESRVRERR_3       3
#define  TAPESRVRERR_4       4
#define  TAPESRVRERR_5       5
#define  TAPESRVRERR_6       6
#define  TAPESRVRERR_7       7
#define  TAPESRVRERR_8       8
#define  TAPESRVRERR_9       9
#define  TAPESRVRERR_10       10
                                      /* migsrvr error numbers */
#define  MIGSRVRERR_1        1
#define  MIGSRVRERR_2        2
#define  MIGSRVRERR_3        3
#define  MIGSRVRERR_4        4
                                      /* pdmsrvr error numbers */
#define  PDMSRVRERR_1        1
#define  PDMSRVRERR_2        2
#define  PDMSRVRERR_3        3
#define  PDMSRVRERR_4        4
#define  PDMSRVRERR_5        5
#define  PDMSRVRERR_6        6
                                      /* namesrvr error numbers */
#define  NAMESRVRERR_1       1
#define  NAMESRVRERR_2       2
#define  NAMESRVRERR_3       3
#define  NAMESRVRERR_4       4
#define  NAMESRVRERR_5       5
#define  NAMESRVRERR_6       6
#define  NAMESRVRERR_7       7
#define  NAMESRVRERR_8       8
#define  NAMESRVRERR_9       9
#define  NAMESRVRERR_10     10
#define  NAMESRVRERR_11     11
#define  NAMESRVRERR_12     12
#define  NAMESRVRERR_13     13
#define  NAMESRVRERR_14     14
#define  NAMESRVRERR_15     15
#define  NAMESRVRERR_16     16
#define  NAMESRVRERR_17     17
#define  NAMESRVRERR_18     18
#define  NAMESRVRERR_19     19
#define  NAMESRVRERR_20     20
#define  NAMESRVRERR_21     21
#define  NAMESRVRERR_22     22
#define  NAMESRVRERR_23     23
                                        /* tdmovr error numbers */
#define  TDMOVRERR_1         1
#define  TDMOVRERR_2         2
#define  TDMOVRERR_3         3
#define  TDMOVRERR_4         4
#define  TDMOVRERR_5         5
#define  TDMOVRERR_6         6
#define  TDMOVRERR_7         7
#define  TDMOVRERR_8         8
#define  TDMOVRERR_9         9
#define  TDMOVRERR_10       10
#define  TDMOVRERR_11       11
#define  TDMOVRERR_12       12
#define  TDMOVRERR_13       13
#define  TDMOVRERR_14       14
#define  TDMOVRERR_15       15
#define  TDMOVRERR_16       16
#define  TDMOVRERR_17       17
#define  TDMOVRERR_18       18
#define  TDMOVRERR_19       19
#define  TDMOVRERR_20       20
#define  TDMOVRERR_21       21
#define  TDMOVRERR_22       22
                                      /* tapemovr error numbers */
#define  TAPEMOVRERR_1       1
#define  TAPEMOVRERR_2       2
#define  TAPEMOVRERR_3       3
#define  TAPEMOVRERR_4       4
#define  TAPEMOVRERR_5       5
#define  TAPEMOVRERR_6       6
#define  TAPEMOVRERR_7       7
#define  TAPEMOVRERR_8       8
#define  TAPEMOVRERR_9       9
#define  TAPEMOVRERR_10      10
#define  TAPEMOVRERR_11       11
#define  TAPEMOVRERR_12       12
#define  TAPEMOVRERR_13       13
#define  TAPEMOVRERR_14       14
#define  TAPEMOVRERR_15       15
#define  TAPEMOVRERR_16       16
#define  TAPEMOVRERR_17       17
#define  TAPEMOVRERR_18       18
#define  TAPEMOVRERR_19       19
#define  TAPEMOVRERR_20       20
#define  TAPEMOVRERR_21       21
#define  TAPEMOVRERR_22       22
#define  TAPEMOVRERR_23       23
#define  TAPEMOVRERR_24       24
#define  TAPEMOVRERR_25       25
#define  TAPEMOVRERR_26       26
#define  TAPEMOVRERR_27       27
#define  TAPEMOVRERR_28       28
#define  TAPEMOVRERR_29       29
#define  TAPEMOVRERR_30       30
                                      /* statsrvr error numbers */
#define  STATSRVRERR_1       1
#define  STATSRVRERR_2       2
#define  STATSRVRERR_3       3
#define  STATSRVRERR_4       4
                                      /* Unitree error numbers */
#define  UTERR_1             1
#define  UTERR_2             2


#define  ENDOFTABLE       NULL    /* Last entry in error tables */

#define  LOG_ONLY            1    /* Output to Log File only */ 
#define  CON_ONLY            2    /* Output to System Console only */
#define  LOG_CON             3    /* Output to both */

#define  LOG_OUTPUT     (0x01)    /* Test flags for LOG/CON output */
#define  CONS_OUTPUT    (0x02)


/*
 *  Structure for each entry in the error tables
 */
#if OSF
struct tabentry {
#else
static struct tabentry {
#endif
    int tabindex;
    int fmttype;
    char *errmsg[7];
};
