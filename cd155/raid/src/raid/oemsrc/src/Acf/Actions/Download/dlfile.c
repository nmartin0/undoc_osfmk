/*
 * 
 * $Copyright
 * Copyright 1992, 1993, 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/******************************************************************************
 ***				IDENTIFICATION				    ***
 ******************************************************************************
 Name:		dlfile.c
 Title:		Download file processor
 Version:	
 Revision:	$Revision: 1.2.4.1 $
 Update Date:	$Date: 1995/06/11 23:25:17 $ 
 Programmer:	rmj
 Documents:	1. UNIX V.4 Disk Array Utilities FS no. 348-0027726
		2. "Object-Oriented Programming in C," C Users Journal, 07/90


 COPYRIGHT 1991, NCR Corp.

 Description:	This module contains file processing for the downloading
		of disk array controller firmware.
 ******************************************************************************
 ***				              				    ***
 *****************************************************************************/

/*
Description:	Read from the given file pointer up to the number of bytes
requested in FDS.count and place the data in FDS.buffer.  All the data read
is required to be in contiquous memory.  If there is a gap in addresses,
only the contiquous memory will be returned.  Another call will be required
to get the next contiquous set of data.  This routine should be called
until an EOF is returned by this routine.  Do not use feof to check EOF.
Each call to read_file is treated as a brand new set of data and will always
return some data if any exists no matter if it is contiguous with the last
data returned or not.  This means that the caller must process/do something
with the data returned before calling this routine again.

Possible Errors:
	Number	=== Description ===
dl_file_err+0	Accessing (reading) file
	    1	Invalid Hex file, not Motorola-S or Intel-HEX	- what is char?
	    2	Invalid record format, not 0-9
	    3	Invalid checksum, line checksum mismatch    - what is value?
	    4	Invalid hex digit	- what is value?
	    5	Invalid status (not DATA,EOB,EOF)
	    6	File corrupted
	    7
	    8
	    9
	   10
	   
************************************************************************** */

/******************************************************************************
 ***				   INCLUDES				    ***
 *****************************************************************************/

#include "dlinclds.h"

/******************************************************************************
 ***				 VARIABLE DEFINITIONS			    ***
 *****************************************************************************/


#define	UT_DATA	0	/* Data returned */
#define	UT_EOB	1	/* Data returned but not contiguous, End-of-block */
#define	UT_EOR	2	/* End-of-record */
#define	UT_EOF	-1	/* End-of-file */

int get_data_block();
int get_data_line();
lword get_data_address();
byte get_data_byte();
byte atoh();

extern	int	verbose;
extern	lword	eval[];
static	byte sbuf[135];		/* local buffer for file reads */
static	byte scnt;		/* count of bytes in sbuf */
static	pointer sadr;		/* address where sbuf data is to be loaded */
static	int	ssts;		/* save status type */
static	int sflag = FALSE;	/* FALSE = empty, TRUE = data */

/* **************************************************************************
ENTRY:	FDS.count = max buffer size

EXIT:	FDS.buffer = data read
	FDS.count  = number of bytes in buffer
	sa         = starting address
************************************************************************** */

/******************************************************************************
 ***				   PROCEDURES				    ***
 *****************************************************************************/

read_block(fp, FDS)
FILE		*fp;		/* file pointer of open file */
struct dldata	*FDS;		/* file data structure */
{
	lword	mcount;	/* max data count, from FDS->count on entry */
	lword	dcount;	/* data count returned from get_data_block */
	lword	memadr;	/* memory address location of data */
	int	first_pass;	/* bool: flag to find first adress */
	int	sts;		/* status of read file */
	int	finished;	/* bool: flag to determine when while done */
	int	have_data;	/* bool: flag to determine if data present */
	
	finished = FALSE;
	have_data = FALSE;
	mcount = FDS->count;
	FDS->count = 0;
	FDS->address = 0;
	memadr = 0;	/* get any data */
	first_pass = TRUE;
	while (FDS->count < (mcount-80))    /* while room in dest. buffer */
	{
		sts = get_data_block(&FDS->buffer[FDS->count],
				(pointer *)&memadr,(long *)&dcount,&FDS->cs,fp);
		switch( sts )
		{
		case UT_DATA:
			FDS->count += dcount;
			if (first_pass)
			{
				have_data = TRUE;
				first_pass = FALSE;
				FDS->address = memadr;
			}
			break;
		case UT_EOB:	/* end of this block, may be more data */
			if (have_data)	finished = TRUE;
			break;
		case UT_EOR:	/* no more data should exist, EOF next */
			FDS->address = memadr;
		case UT_EOF:
			finished = TRUE;
			break;
		}
		if (finished)	break;	/* from while */
		memadr += dcount;		/* set expected address */
	}
}


/* **************************************************************************
Entry:	buffer address at buf
	next LPF address at adr
	
Exit:	count of data
	beginning address of where data belongs
	status:	0 = UT_DATA, ok, have data
		1 = UT_EOB
		2 = UT_EOR
		-1= UT_EOF
************************************************************************** */
static int get_data_block(buf, adr, cnt, gcs,fp)
byte	*buf;		/* buffer to place data */
pointer	*adr;		/* address (0 = get data, else match addresses */
long	*cnt;		/* byte count of data in buf */
Lint	*gcs;		/* address of global check sum */
FILE    *fp;
{
	int	sts;
	
	*cnt = 0;
	if (sflag)	/* data already in Save buffer */
	{
		movmem(sbuf, scnt, buf);
		*cnt = scnt;
		*adr = sadr;
		sts  = ssts;
		sflag = FALSE;
	}
	else		/* get next input line */
	{
		sts = get_data_line(&sbuf[0],&sadr,&scnt,gcs,fp);
		switch( sts )
		{
		case UT_DATA:
			if ((*adr == 0) || (*adr == sadr))
			{
				movmem(sbuf, scnt, buf);
				*cnt = scnt;
				*adr = sadr;
			}
			else
			{
				ssts = UT_DATA;
				sts = UT_EOB;
				sflag = TRUE;
			}
			break;
		case UT_EOR:
		case UT_EOF:
			ssts = sts;
			sts = UT_EOB;
			sflag = TRUE;
			break;
		default:
			eval[dle_status_offset] = sts;
			udl_error(dl_file_err+5, dle_status+1, eval);
		}
	}
	return(sts);
}

/* **************************************************************************
Entry:	buffer
Return:	address
	data
	count
	status, /data/end
************************************************************************** */
static int get_data_line(pbuf, addr, cnt, fcs,fp)
byte	*pbuf;
pointer	*addr;
byte	*cnt;
Lint	*fcs;
FILE    *fp;
{
	byte lbuf[135];
	byte cp;
	byte csum;
	byte type;
	byte ch;
	int i;
	int have_data;
	int sts;
	
	do
	{
		/* read data */
		if (fgets ((char *)lbuf, sizeof(lbuf), fp) == (char *) NULL)
		{
			if (feof(fp)) return(UT_EOF);
			if (eval[dle_status_offset] = ferror(fp))
				udl_error(dl_file_err+0, dle_status+1, eval);
		}
		
		/* convert data */
		cp = 1;
		switch( lbuf[0] )
		{
		case 'S':	/* Motorola-S */
			csum = 1;
			type = lbuf[cp++] & 0xf;
			*cnt = get_data_byte(&lbuf[0], &csum, &cp);
			*addr = (pointer) get_data_address(&lbuf[0], &csum,
							 &cp, type);
			if (type == 0)	(*cnt)--;
			*cnt -= ((type+1)+1);	/* get data count */
			break;
		case ':':	/* Intel-HEX */
			csum = 0;
			type = 0;  /* set type for get_data_address routine */
			*cnt = get_data_byte(&lbuf[0], &csum, &cp);
			*addr = (pointer) get_data_address(&lbuf[0], &csum,
							&cp, type);
			get_data_byte(&lbuf[0], &csum, &cp); /* unused byte */
			if (*cnt == 0)	type = 9;	/* end record */
			else		type = 1;	/* data record */
			break;
		default:
			eval[dle_expecting_offset] = 'S';
			eval[dle_or_offset] = ':';
			eval[dle_found_offset] = lbuf[0];
			udl_error(dl_file_err+1, dle_eof+3, eval);
		}

		/* figure out what to return */
	
		have_data = TRUE;
		switch(type)
		{
		case 0: /* print name in header */
			printname(&lbuf[0], &csum, &cp, *cnt);
			have_data = FALSE;
			break;
		case 1:	/* copy data into new buffer */
		case 2:
		case 3:
#ifdef PARAGON860 /* a cast to make the compiler happy */
			for (i=0; i < (int)*cnt; i++)
				pbuf[i] = get_data_byte(&lbuf[0], &csum, &cp);
#else
			for (i=0; i < (lword *)*cnt; i++)
				pbuf[i] = get_data_byte(&lbuf[0], &csum, &cp);
#endif
			sts = UT_DATA;
			break;
		case 7: /* return end record address */
		case 8:
		case 9:
			*cnt = 0;
			sts = UT_EOR;
			break;
		default:
			eval[dle_expecting_offset] = 0;
			eval[dle_range_offset] = 9;
			eval[dle_found_offset] = type;
			udl_error(dl_file_err+2, dle_erf+3, eval);
		}
		
		/* get checksum character */
		ch = get_data_byte(&lbuf[0], &csum, &cp);
		if (csum)
		{
			eval[dle_expecting_offset] = ch;
			eval[dle_found_offset] = (csum-ch)&0xff;
			udl_error(dl_file_err+3, dle_ef+2, eval);
		}
		if ( lbuf[cp] >= ' ' )
			udl_error(dl_file_err+6, 0, 0);
		*fcs += ch;
	}
	while( !(have_data || feof(fp)) );
	return (sts);
}

/* **************************************************************************
************************************************************************** */
static printname(buf, cs, cp, cnt)
byte	*buf;
byte	*cs;
byte	*cp;
byte	cnt;
{
	byte	lbuf[80];
	int	i;
	
	for (i=0;i < (lword)cnt;i++)
		lbuf[i] = get_data_byte(buf,cs,cp);
	lbuf[i] = 0;
	if (verbose)
	{
		printf(" Program name = %s ",lbuf);
		fflush(stdout);
	}
}

/* **************************************************************************
************************************************************************** */
static lword get_data_address(c, cs, cp, t)
byte	*c;		/* character buffer */
byte	*cs;		/* check sum */
byte	*cp;		/* character pointer */
byte	t;		/* address type (32,24,16 bit address) */
{
	lword	a;
	
	a = 0;
	switch(t)
	{
	default:	a = (unsigned long) get_data_byte(&c[0], cs, cp)<<24;
	case 8:
	case 2:		a |= (unsigned long) get_data_byte(&c[0], cs, cp)<<16;
	case 9:
	case 1:
	case 0:		a |= get_data_byte(&c[0], cs, cp)<<8;
			a |= get_data_byte(&c[0], cs, cp);
	}
	return(a);
}

/* **************************************************************************
************************************************************************** */
static byte get_data_byte(c,cs,cp)
byte *c;
byte *cs;
byte *cp;
{
	byte	b;
	
	b = atoh(c[(*cp)++])<<4;
	b |= atoh(c[(*cp)++]);
	*cs += b;
	return(b);
}


/* **************************************************************************
************************************************************************** */
static byte atoh(ch)
byte	ch;
{
	if ((ch >= (byte) '0') && (ch <= (byte) '9'))
		return((byte) (ch-'0'));
	ch = toupper(ch);
	if ((ch >= (byte) 'A') && (ch <= (byte) 'F'))
		return((byte) (ch-'7'));
	eval[dle_char_offset] = ch;
	udl_error(dl_file_err+4, dle_char+1, eval);
	return((byte) 0);
}
