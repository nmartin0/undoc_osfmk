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

#include <cpus.h>
#include <mach_assert.h>

#include <kern/cpu_number.h>

#include <i860paragon/lbus.h>
#include <i860paragon/led.h>

extern char *getbootenv();
extern long ipsc_physnode;

/*
 * this default used if BOOT_GREEN_LED not specified
 */
int	green_led_flag =	(1 << GREEN_DEBUG) |
				(1 << GREEN_COMEUP) |
				(1 << GREEN_INTERVAL) |
				(1 << GREEN_WAKE);

/*
 * this default used if BOOT_RED_LED not specified
 */
int	red_led_flag =		(1 << RED_DEBUG) |
				(1 << RED_COMEUP) |
				(1 << RED_ECC) |
				(1 << RED_GETC);

#if	MACH_ASSERT
led_green_on_debug(x)
{
	GREEN_ON(GREEN_DEBUG);
	return x;
}


led_green_off_debug(x)
{
	GREEN_OFF(GREEN_DEBUG);
	return x;
}

led_red_on_debug(x)
{
        RED_ON(RED_DEBUG);
	return x;
}


led_red_off_debug(x)
{
        RED_OFF(RED_DEBUG);
	return x;
}
#endif	MACH_ASSERT

/*
 * Pretty lights
 *
 * Called once per clock tick (100/sec)
 */

#define UHIST	20
extern unsigned long	previous_user_count[NCPUS];
unsigned long		previous_user_shadow[NCPUS];
unsigned long		user_history;
unsigned long		green_idle_time;
unsigned long		wake_cycle;

#define WAKE_TIME	200

unsigned long	wake_pattern[4];

int
shuffle(i, n)
	int	i;
	int	n;
{
	int	j;
	int	m;
	int	k;

	if (n == 0) {
		return 0;
	}
	if (n < 100) {
		k = 100/n;
	} else {
		k = 1;
	}
	j = 0;
	while (n >= 5) {
		j += (n/5)*("03142"[i%5] - '0');
		i /= 5;
		n /= 5;
	}
	if (n == 4) {
		i = (i >> 1) + ((i & 1) << 1);
	}
	return k*(i + j);
}

char *
parse_movement(s, i, n, mp)
	char	*s;
	int	n;
	int	*mp;
{
	int	m;
	int	k;

	if (*s == 'r' || *s == 'R') {
		*mp = shuffle(i, n);
		return s+1;
	}
	if (*s == '+') {
		k = 1;
		s++;
	} else if (*s == '-') {
		k = -1;
		s++;
	} else {
		k = 1;
	}
	m = 0;
	while (*s >= '0' && *s <= '9') {
		m = 10*m + *s - '0';
		s++;
	}
	if (m == 0) {
		*mp = 0;
		return s;
	}
	if (m > 100) {
		*mp = k;
		return s;
	}
	*mp = k*100/m;
	return s;
}

int test_wake_x;
int test_wake_y;
init_lights()
{
	char	*s;
	int	x;
	int	y;
	int	mx;
	int	my;

	/*
	 * BOOT_WAKE_PATTERN is an (up to) 25-digit hex number where
	 * each of the 100 bits controls one 10ms interval in each second.
	 * A one bit turns the light on. The progression is LSB to MSB.
	 */

	s = getbootenv("BOOT_WAKE_PATTERN");
	if (s != 0) {
		wake_pattern[0] = 0;
		wake_pattern[1] = 0;
		wake_pattern[2] = 0;
		wake_pattern[3] = 0;
		while (*s) {
			int c = *s++;

			if (c >= '0' && c <= '9') {
				c = c - '0';
			} else {
				c = (c & 7) + 9;
			}
			wake_pattern[3] = wake_pattern[3] << 4 |
					  wake_pattern[2] >> 28;
			wake_pattern[2] = wake_pattern[2] << 4 |
					  wake_pattern[1] >> 28;
			wake_pattern[1] = wake_pattern[1] << 4 |
					  wake_pattern[0] >> 28;
			wake_pattern[0] = wake_pattern[0] << 4 | c;
		}
	} else {
		wake_pattern[0] = 3 | 15 << 16;
		wake_pattern[1] = 0;
		wake_pattern[2] = 0;
		wake_pattern[3] = 0;
	}

	/*
	 * BOOT_WAKE_MOVEMENT is a string of the form x,y where
	 * x and y are signed integers or "r".
	 * x and y control the apparent movement of the pattern across the
	 * nodes within the one-second period.
	 * A value of 2 or +2 generates movement at the rate of 2 nodes/sec
	 * in the left-to-right (x) or top-to-bottom (y) direction.
	 * A negative value generates movement in the opposite direction.
	 * Values or 0 or 1 generate no movement along the axis.
	 * An "r" causes random activation along the axis.
	 */

	wake_cycle = 0;
	s = getbootenv("BOOT_WAKE_MOVEMENT");
	if (s != 0) {
		mx = getbootint("BOOT_MESH_X", 4);
		my = getbootint("BOOT_MESH_Y", 4);
		s = parse_movement(s, ipsc_physnode % mx, mx, &x);
		if (*s++ == ',') {
			parse_movement(s, ipsc_physnode / mx, my, &y);
		} else {
			x = y = 0;
		}
		wake_cycle = x + y;
test_wake_x=x;
test_wake_y=y;
	}
}

pretty_lights(user_hit)
	int	user_hit;
{
	int	user_interval;
	int	i;
	int	n;

	if ((green_led_flag & ((1 << GREEN_INTERVAL) |
			       (1 << GREEN_HIT) |
			       (1 << GREEN_BAR) |
			       (1 << GREEN_WAKE))) == 0) {
		return;
	}

	user_interval = 0;
	for (i = 0; i < NCPUS; i++) {
		if (previous_user_count[i] !=
		    previous_user_shadow[i]) {
			user_interval++;
			previous_user_shadow[i] =
				previous_user_count[i];
		}
	}
	wake_cycle++;
	if (wake_cycle == 100) {
		wake_cycle = 0;
	}

	user_history = (user_history << 1) & ((1 << UHIST) - 1);
	if (green_led_flag & (1 << GREEN_INTERVAL)) {
		if (user_interval != 0) {
			user_history |= 1;
		}
	} else {
		if (user_hit != 0) {
			user_history |= 1;
		}
	}
	n = popcnt(user_history);
	i = n / (UHIST/4);
	switch (i) {
		case 4:	node_control_register_set(LB_NODE_CTRL_TOP_LED);
		case 3:	node_control_register_set(LB_NODE_CTRL_BOTTOM_LED);
		case 2:	node_control_register_set(LB_NODE_CTRL_UNDER_LED);
		case 1:	node_control_register_set(LB_NODE_CTRL_OVER_LED);
	}
	switch (i) {
		case 0:	node_control_register_clear(LB_NODE_CTRL_OVER_LED);
		case 1:	node_control_register_clear(LB_NODE_CTRL_UNDER_LED);
		case 2:	node_control_register_clear(LB_NODE_CTRL_BOTTOM_LED);
		case 3:	node_control_register_clear(LB_NODE_CTRL_TOP_LED);
	}

	if (green_led_flag & (1 << GREEN_BAR)) {
		/* n = popcnt(user_history) already */
	} else if (green_led_flag & (1 << GREEN_INTERVAL)) {
		n = user_interval;
	} else if (green_led_flag & (1 << GREEN_HIT)) {
		n = user_hit;
	} else if (green_led_flag & (1 << GREEN_WAKE)) {
		n = 0;
	} else {
		n = -1;
	}
	if (n > 0) {
		green_idle_time = 0;
		node_control_register_set(LB_NODE_CTRL_MIDDLE_LED);
	} else if (n == 0) {
		green_idle_time++;
		if (green_led_flag & (1 << GREEN_WAKE) &&
		    green_idle_time >= WAKE_TIME &&
		    (wake_pattern[wake_cycle/32] & (1 << (wake_cycle % 32)))) {
			green_idle_time = WAKE_TIME;
			node_control_register_set(LB_NODE_CTRL_MIDDLE_LED);
		} else {
			node_control_register_clear(LB_NODE_CTRL_MIDDLE_LED);
		}
	}
}
