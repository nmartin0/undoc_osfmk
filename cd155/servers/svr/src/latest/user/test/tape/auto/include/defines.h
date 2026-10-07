/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 *		INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *	defines.h 11.1 94/03/22 16:48:55
 */
static char	definesh_ver[] = "@(#) sourcefile defines.h 11.1 94/03/22 16:48:55";

/*
 *
 */

#define DEbug
/*
#define QT1(a) ltrace(1, a, lvcbptr,         __FILE__, __LINE__)
#define QT2(a) ltrace(2, a, lvcbptr,         __FILE__, __LINE__)
#define QT3(a) ltrace(3, a, lvcbptr, dcb, __FILE__, __LINE__)
*/
#define QT1(a) printf(a)
#define QT2(a) printf(a)
#define QT3(a) printf(a)

#ifdef DEbug
#define LTRACE  ltrace
#else
#define LTRACE
#endif

#define NEW_VOL 1

#define t3480_weof(f, c) t3480_interface(f, c, MTWEOF)
#define t3480_fsf(f, c)  t3480_interface(f, c, MTFSF)
#define t3480_bsf(f, c)  t3480_interface(f, c, MTBSF)
#define t3480_fsr(f, c)  t3480_interface(f, c, MTFSR)
#define t3480_bsr(f, c)  t3480_interface(f, c, MTBSR)
#define t3480_rew(f, c)  t3480_interface(f, c, MTREW)
#define t3480_offline(f, c)  t3480_interface(f, c, MTOFFL)
#define t3480_reten(f, c)  t3480_interface(f, c, MTRETEN)
#define t3480_erase(f, c)  t3480_interface(f, c, MTERASE)


/* operations */
#define MTWEOF	0	/* write an end-of-file record */
#define MTFSF	1	/* forward space file */
#define MTBSF	2	/* backward space file */
#define MTFSR	3	/* forward space record */
#define MTBSR	4	/* backward space record */
#define MTREW	5	/* rewind */
#define MTOFFL	6	/* rewind and put the drive offline */
#define MTNOP	7	/* no operation, sets status only */
#define MTRETEN	8	/* retension the tape */
#define MTERASE	9	/* erase the entire tape */
#define MTEOM	10	/* position to end of media (SCSI only) */
#define MTGAP	11	/* iSC: erase 3.5" */



/*----------------------------------------------------------------------
 * Values for Volume Checking FSM (volfsm)
 *---------------------------------------------------------------------*/ 
#define NEVER_ALLOCATED        0
#define ALLOCATED_NOT_VERIFIED 1
#define ALLOCATED_VERIFIED     2 



/*----------------------------------------------------------------------
 * Values for Data Set Checking FSM (dsfsm)
 *---------------------------------------------------------------------*/ 
#define BOT       0
#define CLOSED    1
#define EOT       2

