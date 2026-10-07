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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/* ldr_errno.c
 * error message handling for loader
 *
 * OSF/1 Release 1.0
 */

#include <sys/types.h>
#include <loader.h>

#include "ldr_types.h"
#include "ldr_malloc.h"
#include "ldr_sys_int.h"
#include "ldr_errno.h"


#define		LDR_ERROR_BUFFER_SIZE	4096
#define		LDR_PRINTF_BUFFER_SIZE	8192

static char *ldr_error_buffer;
static char *ldr_printf_buffer;

static char *ldr_status_strings[] = {
	"",				/* 0 */
	"no such module name",		/* LDR_ENOMODULE */
	"module has no entry point",	/* LDR_ENOMAIN */
	"allocation failure",		/* LDR_EALLOC */
};


static void prf(char *buf, int len, const char *fmt, va_list apx);
static int printn(char *bpstart, char *be, u_long n, int b, int zf, int fld_size);
static char *status_to_string(int rc);


static char *
status_to_string(int rc)

/* Return a pointer to a (static) character string describing the specified
 * loader error status.  Uses a string from the sytem error list if possible;
 * otherwise returns a static string.
 */
{
	static	char	buf[64];

	if (rc >= LDR_SUCCESS)
		return("");
	else if (rc >= -SYSTEM_ERRNO_MAX) {
		rc = -rc;
		if (rc >= sys_nerr) {
			ldr_sprintf(buf, sizeof(buf), "unknown error %d", rc);
			return(buf);
		}
		return(sys_errlist[rc]);
	} else {
		rc = -rc - SYSTEM_ERRNO_MAX;
		if (rc >= LDR_MAXSTATUS) {
			ldr_sprintf(buf, sizeof(buf), "unknown loader error %d",
				    rc);
			return(buf);
		}
		return(ldr_status_strings[rc]);
	}
}
	

void
ldr_log(const char *fmt, ...)

/* Log information about the current error to the static loader
 * error logging buffer.  Information from the buffer can be
 * retrieved later by using the %B format character to ldr_error().
 * Arguments are like simplified printf (supports %x, %d, %u, %o,
 * %c, %s, field widths), plus loader-specific format-characters:
 *   %B		include error log buffer as a string (don't use this
 *		in ldr_log())
 *   %E		format loader error message string
 */
{
	va_list		ap;

	if (ldr_error_buffer == NULL) {

		/* No error buffer yet; try to allocate one */

		if (ldr_malloc(LDR_ERROR_BUFFER_SIZE, LDR_STRING_T,
			       (univ_t *)&ldr_error_buffer) < 0)
			return;
		*ldr_error_buffer = '\0';
	}

	va_start(ap, fmt);
	prf(ldr_error_buffer, LDR_ERROR_BUFFER_SIZE, fmt, ap);
	va_end(ap);
}


void
ldr_msg(const char *fmt, ...)

/* Display a message to the user.  Currently this simply opens
 * /dev/tty and blurts the message out.  In the future we may try to
 * do something more graceful.  Arguments are same as ldr_log() above.
 */
{
	va_list		ap;
	ldr_file_t	fd;

	if (ldr_printf_buffer == NULL) {

		/* No error buffer yet; try to allocate one */

		if (ldr_malloc(LDR_PRINTF_BUFFER_SIZE, LDR_STRING_T,
			       (univ_t *)&ldr_printf_buffer) < 0)
			return;
		*ldr_printf_buffer = '\0';
	}

	va_start(ap, fmt);
	prf(ldr_printf_buffer, LDR_PRINTF_BUFFER_SIZE, fmt, ap);
	va_end(ap);

	if ((fd = ldr_open("/dev/tty", LDR_O_RDWR)) < 0)
		return;
	(void)ldr_write(fd, ldr_printf_buffer, strlen(ldr_printf_buffer));
	(void)ldr_close(fd);
}


void
ldr_sprintf(char *buffer, int bufsize, const char *fmt, ...)

/* Simple-minded sprintf.  Supports the same formats as ldr_log.
 * No return value.  Unlike real sprintf, it's safe (takes a
 * buffer size).
 */
{
	va_list		ap;

	va_start(ap, fmt);
	prf(buffer, bufsize, fmt, ap);
	va_end(ap);
}


static void
prf(char *buf, int len, const char *fmt, va_list adx)

/* Simple-minded printf directly into a fixed-size buffer.  Anything
 * overflowing the end of the buffer is just dropped.  Supports basic
 * kernel printf characters (%duxocs) and field widths, plus loader-
 * specific characters: %B (print error buffer); %E (print loader
 * error message string).
 */

{
	register int b, c, i;
	char *s;
	int any;
	int base;
	int zf, fld_size;
	char *bp, *be;

#define putchar(c, buffer, bufend) MACRO_BEGIN \
	if ((buffer) < (bufend))  \
		*(buffer)++ = (c); \
MACRO_END

	bp = buf;
	be = &buf[len];

loop:
	while ((c = *fmt++) != '%') {
		putchar(c, bp, be);
		if (c == '\0')
			return;
	}
again:
	zf = 0;
	fld_size = 0;
	c = *fmt++;
	if (c == '0')
		zf = '0';
	for (;c <= '9' && c >= '0'; c = *fmt++)
		fld_size = fld_size * 10 + c - '0';
	
	/* THIS CODE IS VAX DEPENDENT IN HANDLING %l %c */
	switch (c) {

	case 'l':
		goto again;
	case 'x': case 'X':
		b = 16;
		goto number;
	case 'd': case 'D':
	case 'u':		/* what a joke */
		b = 10;
		goto number;
	case 'o': case 'O':
		b = 8;
number:
		bp += printn(bp, be, (u_long)va_arg(adx, int), b, zf, fld_size);
		break;
	case 'c': case 'C':
		b = va_arg(adx, int);
		c = (char)b;
		putchar(c, bp, be);
		break;

	case 's':
		s = va_arg(adx, char *);
		while (c = *s++)
			putchar(c, bp, be);
		break;

	case 'B':
		if (ldr_error_buffer == NULL || buf == ldr_error_buffer)
			break;
		for (s = ldr_error_buffer; (c = *s++) != '\0'; )
			putchar(c, bp, be);
		break;

	case 'E':
		b = va_arg(adx, int);
		s = status_to_string(b);
		while (c = *s++)
			putchar(c, bp, be);
		break;

	case '%':
		putchar('%', bp, be);
		break;
	}
	goto loop;
}

static int
printn(char *bpstart, char *be, u_long n, int b, int zf, int fld_size)

/*
 * Printn prints a number n in base b.
 * We don't use recursion to avoid deep kernel stacks.
 */
{
	char prbuf[11];
	register char *cp;
	char	*bp = bpstart;

	if (b == 10 && (int)n < 0) {
		putchar('-', bp, be);
		n = (unsigned)(-(int)n);
	}
	cp = prbuf;
	do {
		*cp++ = "0123456789abcdef"[n%b];
		n /= b;
	} while (n);
	if (fld_size) {
		for (fld_size -= cp - prbuf; fld_size > 0; fld_size--)
			if (zf)
				putchar('0', bp, be);
			else
				putchar(' ', bp, be);
	}
	do
		putchar(*--cp, bp, be);
	while (cp > prbuf);
	return(bp - bpstart);
}
