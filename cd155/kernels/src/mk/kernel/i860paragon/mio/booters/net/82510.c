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
 
/*----------------------------------------------------------------------;
;    Name:		82510.c
;    System:	paragon diagnostics
;    Author:	Pat Mason
;
;    Synopsis:	MIO 82510 driver
;
;
;        INTEL CORPORATION PROPRIETARY INFORMATION
;
;  This software is supplied under the terms of a license
;  agreement or nondisclosure agreement with Intel Corporation
;  and may not be copied or disclosed except in accordance
;  with the terms of that agreement.
;
;
;----------------------------------------------------------------------*/
/*
#include "types.h"
*/

/* Registers (8250/16450 mode)
 */
#define BASE	0x91400000
#define RXD		(BASE + (0<<5))		/* Read data */
#define TXD		(BASE + (0<<5))		/* Write data */
#define IER		(BASE + (1<<5))		/* Interrupt Enable Register */
#define BAL		(BASE + (0<<5))		/* BRGA LSB */
#define BAH		(BASE + (1<<5))		/* BRGA MSB */
#define IIR		(BASE + (2<<5))		/* Interrupt ID Register */
#define LCR		(BASE + (3<<5))		/* Line Control Register */
#define MCR		(BASE + (4<<5))		/* Modem Control Register */
#define LSR		(BASE + (5<<5))		/* Line Status Register */
#define MSR		(BASE + (6<<5))		/* Modem Status Register */
#define ACR0	(BASE + (7<<5))		/* */


#define _ESC_ 0x1b

extern	int	software_refresh;

/*----------------------------------------------------------------------;
; read_reg()
;----------------------------------------------------------------------*/
int read_reg(reg)
unsigned reg;
{
	return (inb(reg));
}

/*----------------------------------------------------------------------;
; write_reg()
;----------------------------------------------------------------------*/
write_reg(reg, val)
unsigned long reg;
unsigned char val;
{
	outb(reg, val);
}

/*----------------------------------------------------------------------;
; init_82510()
;----------------------------------------------------------------------*/
init_82510()
{
#define BPS		19200
#define DIVISOR	(115200L/BPS)
#define LCR_VAL	0x03		/* N81 */

	/* Set DLAB
	 */
	outb(LCR, 0x80);

	/* Set Speed
	 */
	outb(BAL,  DIVISOR     & 0xFF);
	outb(BAH, (DIVISOR>>8) & 0xFF);

	outb(LCR, LCR_VAL);
}

/*----------------------------------------------------------------------;
; putc()
;----------------------------------------------------------------------*/
void
putc(c)
int c;
{
	/* Wait for tx empty status 
	 */
	while( (inb(LSR)&0x20) != 0x20 ) {
		if (software_refresh)
			mem_refresh();
		;
	}

	/* Send char
	 */
	outb(TXD, c);

	if( c == '\n')
		putc('\r') ;
}

/*----------------------------------------------------------------------;
; getstat()
;----------------------------------------------------------------------*/
int getstat()
{
	/* Wait for char ready status 
	 */
	if( (inb(LSR)&0x01) == 0x01 )
		return 1;

	return 0;
}

/*----------------------------------------------------------------------;
; getc()
;----------------------------------------------------------------------*/
unsigned char getc(reg)
unsigned reg;
{
	unsigned char data;

	/* Wait for char ready status 
	 */
	while( !getstat() ) {
		if (software_refresh)
			mem_refresh();
		;
	}

	/* Read char
	 */
	data = inb(TXD);
	return data;
}

/*----------------------------------------------------------------------;
; gets()
;----------------------------------------------------------------------*/
gets(str, maxlen)
	char *str;
	int  maxlen;
{
	register char *lp;
	register int c;
	char *strmax = str + maxlen - 1; /* allow space for trailing 0 */

	lp = str;
	for (;;) {
		c = getc();
		switch (c) {
		case '\n':
		case '\r':
			printf("\n");
			*lp++ = 0;
			return;
			
		case '\b':
		case '#':
		case '\177':
			if (lp > str) {
				printf("\b \b");
				lp--;
			}
			continue;

		case '@':
		case 'u'&037:
			lp = str;
			printf("\n\r");
			continue;

		default:
			if (c >= ' ' && c < '\177') {
				if (lp < strmax) {
					*lp++ = c;
					printf("%c", c);
				}
				else {
					printf("%c", '\007'); /* beep */
				}
			}
		}
	}
}
