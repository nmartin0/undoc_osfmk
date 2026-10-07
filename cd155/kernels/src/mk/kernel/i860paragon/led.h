/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1992 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */

#include <i860paragon/lbus.h>

extern int	green_led_flag;
extern int	red_led_flag;

/*
 * Green and Red LED options are listed here.
 * They can be set via bootmagic, but their defaults are:
 * BOOT_GREEN_LED=Dciw
 * BOOT_RED_LED=DCge
 */

#define GREEN_LED_STRING "Dckhibwa"
#define	GREEN_DEBUG	0 /* D */
	/*
	 * Intended for for kernel debugging
	 * eg. set BOOT_GREEN_LED=D and call led_green_[on,off]_debug()
	 * so that debug events can be observed on the LEDs w/o interference.
	 * No production code should use GREEN_DEBUG
	 */
#define GREEN_COMEUP	1 /* c */
	/*
	 * GREEN ON at start of i860_init()
	 * and GREEN OFF at end of machine_startup().
	 */
#define GREEN_KDB	2 /* k */
	/*
	 * GREEN shows KDB input focus.
	 * ie OFF on entry into KDB, ON if cpu acquires the kdb lock,
	 * and OFF on exit from KDB.
	 */
#define	GREEN_HIT	3 /* h */
#define GREEN_INTERVAL	4 /* i */
#define GREEN_BAR	5 /* b */
	/*
	 * pretty_lights() (called by master_cpu every clock tick)
	 * has a mechanism to use all 5 LED's to measure the magnitude
	 * of something with a scheme such that it doesn't change faster
	 * than the eye can see.  These three pretty_lights() options use
	 * different twists to measure user activity on a node.
	 *
	 * if (GREEN_BAR)
	 *		???
	 * else if (GREEN_INTERVAL)
	 * 		measures user activity by counting user ctraps
	 *		with previous_user_count[] & previous_user_shadow[].
	 * 		unclear if user trap count is interesting
	 * else if (GREEN_HIT)
	 * 		measures user activity by seeing if this clock tick
	 *		on cpu0 "hit" user mode code.  makes no sense on a
	 *		MP node (10259)
	 */
#define GREEN_WAKE	6 /* w */
	/*
	 * if none of the pretty_lights() options above are enabled
	 * this will cause a "heart-beat" on the middle GREEN.
	 */
#define GREEN_ACTIVE	7 /* a */
	/*
	 * 1 GREEN LED for each CPU
	 * ON if the CPU is not in the IDLE loop upon clock tick,
	 * OFF otherwise.  Thus it is ON for both user and kernel activity.
	 */


#define RED_LED_STRING "DcsiglXRme"
#define	RED_DEBUG	0 /* D */
	/*
	 * Intended for for kernel debugging
	 * eg. set BOOT_RED_LED=D and call led_red_[on,off]_debug()
	 * so that debug events can be observed on the LEDs w/o interference.
	 * No production code should use RED_DEBUG
	 */
#define RED_COMEUP	1 /* c */
	/*
	 * if set, green on at start of i860_init()
	 * and green off at end of machine_startup().
	 */
#define RED_SCAN	2 /* s */
	/*
	 * used for scan driver debugging
	 */
#define RED_INT		3 /* i */
	/*
	 * ON during Interrupt service, off otherwise.
	 */
#define RED_GETC	4 /* g */
	/*
	 * ON if KDB is doing input.
	 * redundant uses in the various console drivers should be deteled.
	 */
#define RED_LIGHTPEN	5 /* l */
	/*
	 * called only by roll_led_out(), which is only called
	 * by the mesh "multicomputer" console driver.
	 */
#define RED_XMIP	6 /* X */
	/*
	 * called by mcmsg_nic_check() (called by master_cpu every clock tick)
	 * if NIC transmit-in-progress flag is set.
	 */
#define RED_RMIP	7 /* R */
	/*
	 * called by mcmsg_nic_check() (called by master_cpu every clock tick)
	 * if NIC receive-in-progress flag is set.
	 */
#define RED_MSG		8 /* m */
	/*
	 * Various uses in the MCP code.
	 */
#define RED_ECC		9 /* e */
	/*
	 * ON upon ECC Correctable Errors.
	 * Also can be used for other handled error conditions
	 * in conjunction with additional bootmagic.
	 */

#define GREEN_ON(x) \
	if (green_led_flag & (1 << x)) {\
	   switch(cpu_number()) { \
	     case 0:	node_control_register_set(LB_NODE_CTRL_MIDDLE_LED);\
			break;\
	     case 1:	node_control_register_set(LB_NODE_CTRL_TOP_LED);\
			break;\
	     case 2:	node_control_register_set(LB_NODE_CTRL_BOTTOM_LED);\
			break;\
	   }\
	}


#define GREEN_OFF(x) \
	if (green_led_flag & (1 << x)) {\
	   switch(cpu_number()) { \
	     case 0:	node_control_register_clear(LB_NODE_CTRL_MIDDLE_LED);\
			break;\
	     case 1:	node_control_register_clear(LB_NODE_CTRL_TOP_LED);\
			break;\
	     case 2:	node_control_register_clear(LB_NODE_CTRL_BOTTOM_LED);\
			break;\
	   }\
	}

#define RED_ON(x) if (red_led_flag & (1 << x)) \
        node_control_register_set(LB_NODE_CTRL_RED_LED);

#define RED_OFF(x) if (red_led_flag & (1 << x)) \
        node_control_register_clear(LB_NODE_CTRL_RED_LED);

