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
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1991  Intel Corporation.
 */

/*
 * _flick.c
 *
 * _flick routine
 */

static int _flick_count    = 0;
static int _flick_interval = 10000;

_flick()
{
	extern mcmsg_msgp;	/* boolean */
	int other_threads;	/* boolean */

	if (mcmsg_msgp) {
		/*
		 * Message Processor Active
		 */
		if (_flick_count++ >= _flick_interval) {
			_flick_count = 0;
			/*
			 * Schedule next thread
			 */
			other_threads = swtch();
			/*
			 * If other threads are runnable
			 * switch more often.
			 */
			if (other_threads) {
				_flick_interval = 1000;
			} else {
				_flick_interval = 10000;
			}
		}
	} else {
		/*
		 * Message Processor Not Active
		 */
		mcmsg_flick();
	}
	return 0;
}
