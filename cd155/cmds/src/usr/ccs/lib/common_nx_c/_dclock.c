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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation 
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/common_nx_c/_dclock.c,v 1.4 1994/11/19 01:56:21 mtm Exp $
 *
 */
#include <i860paragon/rpm.h>

/*
 * Global Variables.
 */
int		_machine_is_ipsc860 = 0;	/* boolean */
int		_rpm_present        = 0;	/* boolean */

double			_dclock_hwhz;			/* timer frequency */
long			_dclock_hz;				/* frequency (integer) */	
unsigned	long _dclock_timer_addr;	/* timer address */

#define	IPSC860_HWHZ	10000000
#define RPM_HWHZ		10000000	/* PARAGON RPM  */
#define	PARAGON_HWHZ	50000000	/* FAB 7 GPNODE */

#define HW_TIMER	0xFFFF4630
#define RPM_TIMER	(RPM_BASE_VADDR+RPM_GLOBAL_TIME)

double
_dclock()
{
	static int called = 0;
	union {
		unsigned short	wordwise[4];
		double		value;
	} counter;

	if (!called) {
		called++;
		if (_cpu_type() == 1) {
			/* IPSC860 */
			_machine_is_ipsc860 = 1;
			_rpm_present = 0;
			_dclock_timer_addr = HW_TIMER;
			_dclock_hz = IPSC860_HWHZ;
			_dclock_hwhz = 1.0/IPSC860_HWHZ;
		} else {
			_machine_is_ipsc860 = 0;
			_rpm_present = _check_for_rpm();
			if (_rpm_present) {
				/* PARAGON with RPM */
				_dclock_timer_addr = RPM_TIMER;
				_dclock_hz = RPM_HWHZ;
				_dclock_hwhz = 1.0/RPM_HWHZ;
			} else {
				/* PARAGON without RPM */
				_dclock_timer_addr = HW_TIMER;
				_dclock_hz = PARAGON_HWHZ;
				_dclock_hwhz = 1.0/PARAGON_HWHZ;
			}
		}
	}

	_read_timer(_dclock_timer_addr, &counter);
	counter.wordwise[3] = (counter.wordwise[3] & 0x000F) | 0x4330;
	return _dclock_hwhz*(counter.value - 4503599627370496.0);
}

_timerinfo(global, frequency, address)
int				*global;		/* boolean */
long			*frequency;		/* frequency of timer */
unsigned long	*address;		/* address of timer */
{
	/* call _dclock to ensure init */
	_dclock();

	*global    =   _rpm_present;
	*frequency =   _dclock_hz;
	*address   =   _dclock_timer_addr;

	return;
}

/*
 *	_cpu_type returns 1 for 860XR, 2 for 860XP.
 */
static int 
_cpu_type()
{
	asm(" ld.c epsr,r16");
	asm(" and  0xff,r16,r16");
}

/*
 *	_read_timer reads a 64 bit timer (as a double)
 */
int
_read_timer(timer_addr, timer_result)
unsigned long timer_addr;
double *timer_result;
{
	asm (" fld.d   0(r16), f16   // Read counter into f16-17");
	asm (" fst.l   f16, 0(r17)   // Store at pointer as two");
	asm (" fst.l   f17, 4(r17)   //  longs to avoid traps");
}

/*
 *	_check_for_rpm
 *
 *	return 1 if rpm present
 *	       0 if not
 */
int
_check_for_rpm()
{

	int timestamp[2];

	_read_timer(RPM_TIMER, timestamp);
	if ((timestamp[0] == 0) && (timestamp[1] == 0)) {
		_read_timer(RPM_TIMER, timestamp);
	}
	if ((timestamp[0] != 0) || (timestamp[1] != 0)) {
		return 1;
	}
	return 0;
}

