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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
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
 *      Copyright 1992 Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/svr/server/uxkern/rpm_clock.c,v 1.12 1995/02/18 01:41:21 yazz Exp $
 *
 * HISTORY
 * $Log: rpm_clock.c,v $
 * Revision 1.12  1995/02/18  01:41:21  yazz
 *  Reviewer: John Litvin
 *  Risk: Med
 *  Benefit or PTS #: 12240, including emul console logging cleanup
 *  Testing: EATs controlc, sched
 *  Module(s):
 * 	svr/emulator/bsd_user_side.c
 * 	svr/emulator/emul_chkpnt.c
 * 	svr/emulator/emul_init.c
 * 	svr/emulator/emul_mapped.c
 * 	svr/emulator/fsvr_user_side.c
 * 	svr/server/bsd/kern_sig.c
 * 	svr/server/bsd/mach_signal.c
 * 	svr/server/bsd/subr_prf.c
 * 	svr/server/tnc/dvp_vpops.c
 * 	svr/server/uxkern/boot_config.c
 * 	svr/server/uxkern/bsd_server_side.c
 * 	svr/server/uxkern/credentials.c
 * 	svr/server/uxkern/rpm_clock.c
 *
 * General cleanup of emulator console logging.  Added bootnode_printf()
 * routine to server.  Added server bootmagic variable ENABLE_RPM_TIMESTAMP
 * so printf() and bootnode_printf() messages are timestamped with the
 * 56-bit RPM global clock value.  This enables very fine timings to be
 * observable in console log output.
 *
 * Revision 1.11  1995/02/01  22:27:55  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.10  1994/11/18  20:49:20  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1994/10/05  18:00:19  yazz
 *  Reviewer: Karen Hackett
 *  Risk: Lo
 *  Benefit or PTS #: 11120
 *  Testing: By inspection, plus controlc EAT
 *  Module(s): server/uxkern/rpm_clock.c
 *
 * Set default for bootmagic CHECK_FOR_RPMFAIL true.  (And leave default
 * for bootmagic REBOOT_ON_RPMFAIL false.)  The latest decision on these
 * defaults was reached the first week of 10/1994.
 *
 * Revision 1.8  1994/09/06  20:33:48  yazz
 *  Reviewer: John Litvin
 *  Risk: Lo
 *  Benefit or PTS #: #70732 H-1
 *  Testing: controlc EATS, with new bootmagic CHECK_FOR_RPMFAIL true & false
 *  Module(s): server/uxkern/rpm_clock.c, .../boot_config.c
 * Don't check for & report on RPM failures if bootmagic CHECK_FOR_RPMFAIL is
 * true, the present default.  But when true, print a message once on boot node.
 *
 * Revision 1.7  1994/08/31  22:47:48  mtm
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.4.2.2  1994/08/23  00:43:17  yazz
 *  Author of fix: Paul Cantrell (paul@locus.com)
 *  Reviewer: Bob Yasi, Mike Leibensperger, Dave Minturn
 *  Risk: Lo
 *  Benefit or PTS #: 10647 C-0
 *  Testing: Specific testcase
 *  Module(s): server/uxkern/boot_config.c, server/uxkern/rpm_clock.c
 * RPM failure error messages now contain more info for RPM diagnosis, and
 * appear on the boot node regardless of the failing node.  New bootmagic
 * REBOOT_ON_RPMFAIL defaults to false; only if set true will the system reboot
 * when an RPM failure occurs.
 *
 * Revision 1.4.2.1  1994/08/13  01:48:39  paul
 * Reviewer: mjl jlitvin
 * Risk:M
 * Benefit or PTS #: 8836
 * Testing: tested on Plymouth across diag station reboots
 * Module(s):
 *     sys/time.h
 *     uxkern/rpm_clock.c
 *     uxkern/boot_config.c
 *
 * Changes to fix bug 8836 9521 RTI - Handle an RPM which stops counting
 *     This change halts a system when the RPM stops counting either due
 *     to a hardware failure, or a reboot of the diagnostic station. It
 *     also includes a boot magic variable to override use of the RPM.
 *
 * Revision 1.4  1994/01/26  12:40:22  paul
 *  Reviewer: none (cfg for changes to 1.2 tree)
 *  Risk: low
 *  Benefit or PTS #: 7698
 *  Testing:
 *  Module(s):
 * 	uxkern/misc.c
 * 	uxkern/rpm_clock.c
 * 	bsd/kern_time.c
 * 	bsd/kern_utctime.c
 * 	afs/afs_osi.c
 *
 *
 * Merge of bug fixes for RPM from 1.2 tree - Moved the actual sending of the
 * RPM offset outside of a TIME lock.
 *
 * Revision 1.3  1994/01/12  17:47:39  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: Reduce lint complaints.
 *  Testing: compiled server
 *  Module(s):
 * 	uxkern/vm_unix.c
 * 	uxkern/ux_server_loop.c
 * 	uxkern/tty_io.c
 * 	uxkern/syscall.c
 * 	uxkern/server_init.c
 * 	uxkern/raw_hippi.c
 * 	uxkern/misc.c
 * 	uxkern/mf.c
 * 	uxkern/inittodr.c
 * 	uxkern/hippi_io.c
 * 	uxkern/fsvr_subr.c
 * 	uxkern/fsvr_server_side.c
 * 	uxkern/fsvr_rmtspec_ops.c
 * 	uxkern/fsvr_port.c
 * 	uxkern/fsvr_msg.c
 * 	uxkern/ether_io.c
 * 	uxkern/disk_io.c
 * 	uxkern/device_reply_hdlr.c
 * 	uxkern/credentials.c
 * 	uxkern/cons.c
 * 	uxkern/bsd_server_side.c
 * 	uxkern/boot_config.c
 * 	uxkern/block_io.c
 * 	uxkern/rpm_clock.c
 * 	i386/conf.c
 * 	i860/conf.c
 *
 * Revision 1.2  1993/12/03  19:28:13  paul
 * Various fixes to RPM support, plus support for global setting of Timezone
 *
 * Moved the if defined(__i860__) && defined(TNC) so that the #includes are
 * inside of the if
 *
 * Removed some leading underscores from some symbol names. (Sorry, I know
 * this screws up the diffs.
 *
 * Added a check to rpm_get_unixtime and rpm_update_time_var so that if the
 * rpm just got initialized (and thus rpm_present is now non-zero) still get
 * the time of day from mach if the rpm offset hasn't been set yet. This
 * prevents us from seeing dates in the 1970's before the RPM has been set.
 *
 * Added code into rpm_adjust_time and rpm_set_time to call VPSOP_RPMOFFSET()
 * where this used to be done out in misc.c (which was a little ugly).
 *
 *  Reviewer: John Litvin (jlitvin@ssd.intel.com) Brent Olsen (bolsen@locus.com)
 *  Risk: Moderate
 *  Benefit or PTS #:Fixes bug #s 3503 5303 6029 7299
 *  Testing: functionality checked on olympus.sd.locus.com w/RPM support
 *  Module(s): server/uxkern/rpm_clock.c
 *
 * Revision 1.1.2.2  1994/01/18  20:32:08  paul
 *   Reviewer:cfj
 *   Risk:M
 *   Benefit or PTS #:7698
 *   Testing:
 *   Module(s):
 *       uxkern/misc.c
 *       uxkern/rpm_clock.c
 *       bsd/kern_time.c
 *       afs/afs_osi.c
 *
 * Changes to fix bug 7698 - Time Lock hang
 *    Moved the RPM's offsett transmittal RPC outside of the TIME lock to
 *    prevent deadlocks.
 *
 * rpm_clock.c:
 *         Put rpm_get_unixtime() inside a false ifdef since it's not
 *         currently being used.
 *
 *         Moved the actual sending of the offset to a separate routine,
 *         rpm_transmit_offset(), so that the calculation of the new
 *         offset, and the sending of it can be separated. The calculation
 *         gets done under the TIME lock, but the sending is done after
 *         the return from resettodr
 *
 *         The setting of the time in pps_rpmoffset is now done under the
 *         TIME write lock.
 *
 * Revision 1.1.2.1  1993/12/07  16:26:11  cfj
 * The final timezone fix from Locus.  This is just a merge from
 * the main-stem into the R1_2 branch.
 *
 *  Reviewer:jlitvin,cfj
 *  Risk:M
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 * 	uxkern/misc.c
 * 	uxkern/rpm_clock.c
 * 	bsd/kern_xxx.c
 * 	sys/vproc.h
 * 	tnc/dvp_pvpsops.c
 * 	tnc/dvp_vpsops.c
 * 	vproc/bvp_vpsops.c
 * 	tnc/dpvproc_struct.h
 * 	bsd/kern_time.c
 * 	uxkern/server_init.c
 *
 * Revision 1.2  1993/12/03  19:28:13  paul
 * Various fixes to RPM support, plus support for global setting of Timezone
 *
 * Moved the if defined(__i860__) && defined(TNC) so that the #includes are
 * inside of the if
 *
 * Removed some leading underscores from some symbol names. (Sorry, I know
 * this screws up the diffs.
 *
 * Added a check to rpm_get_unixtime and rpm_update_time_var so that if the
 * rpm just got initialized (and thus rpm_present is now non-zero) still get
 * the time of day from mach if the rpm offset hasn't been set yet. This
 * prevents us from seeing dates in the 1970's before the RPM has been set.
 *
 * Added code into rpm_adjust_time and rpm_set_time to call VPSOP_RPMOFFSET()
 * where this used to be done out in misc.c (which was a little ugly).
 *
 *  Reviewer: John Litvin (jlitvin@ssd.intel.com) Brent Olsen (bolsen@locus.com)
 *  Risk: Moderate
 *  Benefit or PTS #:Fixes bug #s 3503 5303 6029 7299
 *  Testing: functionality checked on olympus.sd.locus.com w/RPM support
 *  Module(s): server/uxkern/rpm_clock.c
 *
 * Revision 1.1  1993/10/29  11:53:59  paul
 * Add support for setting and using the RPM distributed time-of-day clock.
 *
 * Revision 1.0  1993/07/14  18:42:32  paul
 * Initial version of the file
 *
 */

#if defined(__i860__) && defined(TNC)

#include <i860paragon/rpm.h>
#include <sys/time.h>
#include <sys/vproc.h>
#include <sys/reboot.h>

/*
 * This file is a noop except on an Intel PARAGON running TNC
 *
 * This file derived from dclock.c - an Intel library file. It has
 * been modified for server access to the RPM clock. The RPM clock
 * is essentially a global counter, present on every node of the
 * Paragon, guarenteed to have the same number, and update at the
 * same time, across all the nodes. It's essentially a single clock,
 * with distributed read access. The clock is not writable, so in
 * order to set or adjust the time of day, we transmit a difference
 * to all the nodes. This difference gets added to the current contents
 * of the RPM, and the result is a floating point number representing
 * time of day in seconds. This generally gets converted to standard
 * unix seconds/microseconds, but entry points have been left in so
 * that low level code which wants either the raw 64 bit counter or
 * the raw 64 bit floating point number can still get it.
 */

/*
 * Global Variables.
 */
int		rpm_present		= 0;	/* boolean */
int		rpm_initialized		= 0;	/* boolean */
int		rpm_offset_has_been_set = 0;	/* boolean */
int		rpm_error 		= 0;	/* boolean */
boolean_t	rpm_avoid_using_global_clock = 0;
boolean_t	rpm_reboot_on_fail	 = 0;
boolean_t	rpm_check_for_fail	 = 1;

extern int	vnode_pager_is_set;		/* != 0 when bootmesh done */
extern int	root_fs_node;
extern int	this_node;

double		rpm_offset_value;	/* offset to true timeofday */
double		rpm_new_offset_value;	/* new offset not yet sent */
double		rpm_hwhz;		/* timer frequency */
long		rpm_hz;			/* frequency (integer) */
unsigned long	rpm_timer_addr;		/* timer address */

double		rpm_bad_counter_value0;	/* we keep these around */
double		rpm_bad_counter_value1;	/* we keep these around */

#define RPM_HWHZ	10000000	/* PARAGON RPM  */
#define	PARAGON_HWHZ	50000000	/* FAB 7 GPNODE */

#define HW_TIMER	0xFFFF4630
#define RPM_TIMER	(RPM_BASE_VADDR+RPM_GLOBAL_TIME)

void rpm_check_for_error();

/*
 * Function:
 *
 *	This routine is called before using the RPM clock when the
 *	initialized variable is zero. It gives us a chance to test
 *	whether we are running on a Paragon or not. Some Paragons
 *	don't have RPMs, which explains the call to check_for_rpm.
 *	In this case, that Paragon has no system wide global clock.
 */
void
_rpm_init()
{

	rpm_initialized = 1;

	rpm_present = check_for_rpm();
	if (rpm_present) {
		/* PARAGON with RPM */
		rpm_timer_addr = RPM_TIMER;
		rpm_hz = RPM_HWHZ;
		rpm_hwhz = 1.0/RPM_HWHZ;
	} else {
		/* PARAGON without RPM */
		rpm_timer_addr = HW_TIMER;
		rpm_hz = PARAGON_HWHZ;
		rpm_hwhz = 1.0/PARAGON_HWHZ;
	}

	/*
	 * If the RPM clock is not being checked for failures, say so once
	 * on the boot node and that's all.
	 */
	if (rpm_check_for_fail == 0 && this_node == root_fs_node) {
		printf("NOTE: RPM CLOCK FAILURES WILL BE IGNORED as "
				"bootmagic CHECK_FOR_RPMFAIL=false\n");
	}
}

/*
 * Function:
 *
 *	This routine returns the contents of the RPM as seconds in
 *	double floating point format.  And if the caller supplies a
 *	non-NULL pointer then the raw integer PRM clock value is
 *	stored as two longs.
 */
double
rpm_contents(longptr)
long	*longptr;
{
	union {
		unsigned short	wordwise[4];
		long		longwise[2];
		double		value;
	} counter0, counter1;			/* read RPM twice */

	if (!rpm_initialized) {
		_rpm_init();
	}

	/*
	 *	Get the integral number of RPM ticks and convert that to
	 *	a floating point number of ticks for our comparisons.
	 *	Only when we're about to return to our caller do we
	 *	convert this value to a number of seconds.
	 */
	_read_timer(rpm_timer_addr, &counter0);
	if (longptr) {
		longptr[0] = counter0.longwise[1];	/* hi order 32 bits */
		longptr[1] = counter0.longwise[0];	/* lo order 32 bits */
	}
	counter0.wordwise[3] = (counter0.wordwise[3] & 0x000F) | 0x4330;
							/* hi order 16 bits */

	/*
	 * Read the RPM clock a second time to be sure it's incrementing.
	 * If not, cause warnings to be printed and the clock to be abandoned.
	 * The RPM clock has resolution fine enough that two successive reads
	 * are guaranteed to be different.
	 *
	 * Don't even check for such failures if the bootmagic so specifies.
	 */

	if (rpm_check_for_fail) {
		_read_timer(rpm_timer_addr, &counter1);
		counter1.wordwise[3] = (counter1.wordwise[3] & 0x000F) | 0x4330;

		if(counter0.value >= counter1.value){
			rpm_error = 1;
			rpm_offset_has_been_set = 0;
			rpm_bad_counter_value0 = counter0.value;
			rpm_bad_counter_value1 = counter1.value;
		} else {
			counter0 = counter1;	/* return newest value read */
		}
	}

	counter0.value = rpm_hwhz*(counter0.value - 4503599627370496.0);

	return(counter0.value);
}

#if 0
/*
 * Function:
 *
 *	This routine takes a pointer to a unix timeval structure and
 *	fills it in with the current time of day. If the RPM is present,
 *	that is used. If there is no RPM, we use what the microkernel
 *	thinks the time of day is. In this case, however, the time is not
 *	a global, system-wide time, but is just a per-node time of day.
 *	It's not currently used in the server, but seems like a valuable
 *	function that might get used sometime.
 */
void
rpm_get_unixtime(tvp)
register struct timeval *tvp;
{
	extern struct timeval time;
	double rawtime;

	if (!rpm_initialized) {
		_rpm_init();
	}

	if(rpm_present == 0 || rpm_offset_has_been_set == 0){
		update_time_var_from_mach();
		*tvp = time;
		return;
	}

	rawtime = rpm_contents(NULL) + rpm_offset_value;
	tvp->tv_sec = rawtime;
	tvp->tv_usec = (rawtime - (double)tvp->tv_sec) * 1000000;
	return;
}
#endif

/*
 * Function:
 *
 *	This routine returns the time of day in the "time" shared structure.
 *	If the RPM is present, that value is used, otherwise we just use
 *	the time that the microkernel thinks it is.
 */
void
rpm_update_time_var()
{
	extern struct timeval time;
	double rawtime;

	if (!rpm_initialized) {
		_rpm_init();
	}

	if(rpm_present == 0 || rpm_offset_has_been_set == 0){
		update_time_var_from_mach();
		return;
	}

	rawtime = rpm_contents(NULL) + rpm_offset_value;
	if(rpm_error){
		update_time_var_from_mach();
		return;
	}
	time.tv_sec = rawtime;
	time.tv_usec = (rawtime - (double)time.tv_sec) * 1000000;
	return;
}

/*
 * Function:
 *
 *	This routine is called to adjust the RPM time. Since we can't
 *	actually set the clock, we keep a difference between the raw
 *	RPM value, and the actual time of day. To adjust the time of
 *	day, we simply manipulate this value. Code outside of this
 *	module takes care of broadcasting the new difference to all
 *	the nodes in the system.
 */
void
rpm_adjust_time(ndelta)
register int ndelta;
{
	rpm_new_offset_value = rpm_offset_value + ((double)ndelta / 1000000.0);
}

/*
 * Function:
 *
 *	Similar to the previous routine, this sets the time of day
 *	to a new value, rather than adjusting it up and down. Again,
 *	outside functions are responsible for broadcasting the new
 *	value to all the nodes in the system.
 */
void
rpm_set_time(tvp)
register struct timeval *tvp;
{
	double new_time,current_time;

	if (!rpm_initialized) {
		_rpm_init();
	}

	current_time = rpm_contents(NULL);
	new_time = tvp->tv_sec + ((double)tvp->tv_usec / 1000000.0);
	rpm_new_offset_value = new_time - current_time;

}

/*
 * Function:
 *
 *	Transmit the current global offset to the other nodes in the
 *	system. This is done after the lock is released so that we
 *	don't RPC off node with a simple lock held.
 */
void
rpm_transmit_offset()
{
	union {
		unsigned int longwise[2];
		double		value;
	} rpmval;

/*
 * Don't try to talk to other nodes if bootmesh hasn't run yet... It
 * hangs the Paragon.
 */
	if(this_node != root_fs_node || vnode_pager_is_set){
		if(rpm_offset_value != rpm_new_offset_value){
			rpmval.value = rpm_new_offset_value;
			VPSOP_RPMOFFSET(rpmval.longwise[0],rpmval.longwise[1]);
		}
	}
}

/*
 * Function:
 *
 *	Kinda dumb, this function simply allows the RPM difference
 *	value to be set. This is used when another node is sending
 *	us a new difference value because time of day was set or
 *	adjusted.
 */
void
pps_rpmoffset(new_offset)
double new_offset;
{
	TIME_WRITE_LOCK();
	rpm_offset_value = new_offset;
	rpm_offset_has_been_set = 1;
	TIME_WRITE_UNLOCK();

}

#if 0
/*
 * Function:
 *
 *	This routine is left over from the dclock.c code. It's not
 *	currently used in the server, but seems like a valuable
 *	function that might get used sometime.
 */
void
rpm_timerinfo(global, frequency, address)
int				*global;		/* boolean */
long			*frequency;		/* frequency of timer */
unsigned long	*address;		/* address of timer */
{
	if (!rpm_initialized) {
		_rpm_init();
	}

	*global    =   rpm_present;
	*frequency =   rpm_hz;
	*address   =   rpm_timer_addr;

	return;
}
#endif

/*
 * Function:
 *
 *	Returns the RPM contents (or whatever r16 points at) as a
 *	double.
 */
static int
_read_timer(timer_addr, timer_result)
unsigned long timer_addr;
double *timer_result;
{
	asm (" fld.d   0(r16), f16   // Read counter into f16-17");
	asm (" fst.l   f16, 0(r17)   // Store at pointer as two");
	asm (" fst.l   f17, 4(r17)   // longs to avoid traps");
}

/*
 *	check_for_rpm
 *
 *	return 1 if rpm present
 *	       0 if not
 */
int
check_for_rpm()
{

	double timestamp;

	if (!rpm_initialized) {
		_rpm_init();
	}

	if(rpm_avoid_using_global_clock){
		printf("bootmagic DONT_USE_RPM_GLOBAL_CLOCK true - NO RPM!\n");
		return(0);
	}

	_read_timer(RPM_TIMER, &timestamp);
	if (timestamp != (double)0.0) {
		return(1);
	}
	return(0);
}

void
rpm_check_for_error()
{
#define	BSD_PRINT	0x01
#define PRINTF_BUFSIZE    4096
        char buf[PRINTF_BUFSIZE];
extern	mach_port_t	root_fs_port;
	kern_return_t	ret;

	union {
		unsigned long longwise[2];
		double		value;
	} val0,val1;

	if(rpm_error == 1){
		val0.value = rpm_bad_counter_value0;
		val1.value = rpm_bad_counter_value1;
		sprintf(buf,
			"RPM Hardware Clock error node %d (%x,%x) (%x,%x)!\n",
			this_node,
			val0.longwise[1],val0.longwise[0],
			val1.longwise[1],val1.longwise[0]);
		if (this_node != root_fs_node &&
		 MACH_PORT_VALID(root_fs_port)) {
			/*
			 * We overload the root_fs_port for remote printing.
			 */
		        ux_server_thread_blocking();
			ret = ubsd_remote_print(root_fs_port, BSD_PRINT, 
						0, 0, 0,buf, strlen(buf));
			ux_server_thread_unblocking();
			if (ret != KERN_SUCCESS) {
				printf("rpm_check_for_error: ubsd_remote_print failed 0x%x\n",ret);
			}
		}
		printf(buf);
		rpm_error = 2;
		if(rpm_reboot_on_fail != 0){
			(void) VPSOP_REBOOT(RB_BOOT, RB_HALT);
		}
	}
}

#endif /* defined(__i860__) && defined(TNC) */
