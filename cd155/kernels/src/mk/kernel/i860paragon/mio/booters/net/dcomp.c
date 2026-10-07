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
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1991 Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mio/booters/net/dcomp.c,v 0.5 1994/11/18 20:45:32 mtm Exp $
 *
 * $Log: dcomp.c,v $
 * Revision 0.5  1994/11/18  20:45:32  mtm
 * Copyright additions/changes
 *
 * Revision 0.4  1993/06/30  22:40:04  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 0.3  1993/04/27  20:32:02  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 0.2.6.2  1993/04/22  18:31:48  dleslie
 * First R1_0 release
 *
 * Revision 0.2  1992/07/17  17:21:53  rkl
 * Added decompression progress monitoring.
 *
 * Revision 1.3  92/03/05  12:12:33  shala
 * Remove rcsid.
 * 
 * Revision 1.2  92/03/03  09:36:07  shala
 * Add Header and Log markers.
 * 
 *
 */

/*
 * dcomp.c
 *
 * Decompression code from public domain compress
 */


#define CLEAR	256	/* table clear output code */
#define FIRST	257	/* first free entry */
#define BITS	 16
#define INIT_BITS 9	/* initial number of bits/code */

/* Defines for third byte of header */

#define BIT_MASK	0x1f
#define BLOCK_MASK	0x80
#define MAXCODE(n_bits)	(((code_int) 1 << (n_bits)) - 1)

#define TSIZE (1 << BITS)

typedef	unsigned char	char_type;
typedef long int	code_int;

static char_type magic_header[] = { "\037\235" };/* 1F 9D */
static int block_compress = BLOCK_MASK;
static int clear_flg = 0;
static int n_bits;			/* number of bits/code */
static int maxbits = BITS;		/* user settable max # bits/code */
static code_int maxcode;		/* maximum code, given n_bits */
static code_int maxmaxcode = (code_int)1 << BITS;
static code_int free_ent = 0;		/* first unused entry */
static char_type rmask[9] =
	{0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};
static char_type suffixtab[TSIZE];
static unsigned short prefixtab [TSIZE];
static char_type *inp;
static long int incnt;
static	int	orig, cur_percent = 0;

/*
 * Read one code from the input.  If EOF, return -1.
 */

static code_int
getcode()
{
	register code_int code;
	static int offset = 0, size = 0;
	static char_type *bufp;
	register int r_off, bits;
	register char_type *bp = bufp;
	int	tmp;

	if ( clear_flg > 0 || offset >= size || free_ent > maxcode ) {

		/*
		 * If the next entry will be too big for the current code
		 * size, then we must increase the size.  This implies reading
		 * a new buffer full, too.
		 */

		if ( free_ent > maxcode ) {
			n_bits++;
			if ( n_bits == maxbits ) {
				maxcode = maxmaxcode;	/* won't get bigger */
			} else {
				maxcode = MAXCODE(n_bits);
			}
		}
		if ( clear_flg > 0) {
			maxcode = MAXCODE (n_bits = INIT_BITS);
			clear_flg = 0;
		}

		size = incnt;
		if ( size <= 0 )
			return -1;			/* end of file */
		if (size > n_bits)
			size = n_bits;

		bp = bufp = inp;
		incnt -= size;

		/*
		 *  Let them know we are making progress
		 */
		tmp = 100 - ((incnt*100)/orig);
		if (tmp > cur_percent) {
			printf("\r%d%%",  tmp);
			cur_percent = tmp;
		}

		inp += size;

		offset = 0;
		/* Round size down to integral number of codes */
		size = (size << 3) - (n_bits - 1);
	}
	r_off = offset;
	bits = n_bits;

	/*
	 * Get to the first byte.
	 */

	bp += (r_off >> 3);
	r_off &= 7;

	/* Get first part (low order bits) */
	code = (*bp++ >> r_off);
	bits -= (8 - r_off);
	r_off = 8 - r_off;		/* now, offset into code word */

	/* Get any 8 bit parts in the middle (<=1 for up to 16 bits). */
	if ( bits >= 8 ) {
		code |= *bp++ << r_off;
		r_off += 8;
		bits -= 8;
	}

	/* high order bits. */
	code |= (*bp & rmask[bits]) << r_off;
	offset += n_bits;

	return code;
}

/*
 * Decompress in to out. This routine adapts to the codes in the
 * file building the "string" table on-the-fly; requiring no table to
 * be stored in the compressed file.
 */

decompress(inbuf, insize, outbuf, outsize)
	char_type	*inbuf;
	int		insize;
	char_type	*outbuf;
	int		outsize;
{
	register char_type *stackp;

	register int finchar;
	register code_int code, oldcode, incode;
	register char_type *outp = outbuf;

	/* Check the magic number */
	if ((*inbuf++ != (magic_header[0] & 0xFF)) ||
		(*inbuf++ != (magic_header[1] & 0xFF))) {
		return -1;
	}

	/*
	 *  Save for progress reporting
	 */
	orig = insize;

	maxbits = *inbuf++;	/* set -b from file */
	inp = inbuf;
	incnt = insize - 3;
	block_compress = maxbits & BLOCK_MASK;
	maxbits &= BIT_MASK;
	maxmaxcode = (code_int) 1 << maxbits;
	if(maxbits > BITS) {
		return -1;
	}

	/*
	 * As above, initialize the first 256 entries in the table.
	 */

	maxcode = MAXCODE(n_bits = INIT_BITS);
	for ( code = 255; code >= 0; code-- ) {
		prefixtab[code] = 0;
		suffixtab[code] = (char_type)code;
	}
	free_ent = ((block_compress) ? FIRST : 256 );

	finchar = oldcode = getcode();
	if(oldcode == -1)		/* EOF already? */
		return outp - outbuf;	/* Get out of here */

	*outp++ = finchar;		/* first code must be 8 bits = char */
	stackp = &outbuf[outsize];

	while ( (code = getcode()) > -1 ) {

		if ( (code == CLEAR) && block_compress ) {
			for ( code = 255; code >= 0; code-- ) {
				prefixtab[code] = 0;
			}
			clear_flg = 1;
			free_ent = FIRST - 1;
			if ( (code = getcode()) == -1 )
				break;
		}
		incode = code;

		/*
		 * Special case for KwKwK string.
		 */

		if ( code >= free_ent ) {
			*--stackp = finchar;
			code = oldcode;
		}

		/*
		 * Generate output characters in reverse order
		 */

		while ( code >= 256 ) {
			*--stackp = suffixtab[code];
			code = prefixtab[code];
		}
		*--stackp = finchar = suffixtab[code];

		/*
		 * And put them out in forward order
		 */

		do {
			*outp++ = *stackp++;
		} while ( stackp < &outbuf[outsize]);

		/*
		 * Generate the new entry.
		 */

		if ( (code=free_ent) < maxmaxcode ) {
			prefixtab[code] = (unsigned short)oldcode;
			suffixtab[code] = finchar;
			free_ent = code+1;
		} 

		/*
		 * Remember previous code.
		 */

		oldcode = incode;
	}
	return outp - outbuf;
}
