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
 * INTEL CORPORATION PROPRIETARY INFORMATION 
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with Intel Corporation and may not be copied or
 * disclosed except in accordance with the terms of that agreement. 
 *
 * tdsopen.c 11.1 94/03/22 16:49:38 
 */
static char     tdsopen_ver[] = "@(#) sourcefile tdsopen.c 11.1 94/03/22 16:49:38";

/* 
 * Open a file on a 3480-type tape device. 
 *
 */
#define	CFS 0

#include <stdio.h>
#include <type.h>
#include "tapeio.h"
#include <sys/ioctl.h>
#include <mtio.h>
#if	CFS
#include "tape.h"
#endif
#include <sys/types.h>
#include "tape3480.h"
#include "extern.h"
#include <string.h>
#include <protos.h>
#include <defines.h>
#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include "tioproto.h"
#include <sys/errno.h>

int
tdsopen (int handle, DCB * dcb)
{
#define FIRST    0
#define NOTFIRST 1
#define END      2

#define ALPHA    0
#define NUMERIC  1
#define DOT      2
#define BLANK    3

    int    qualcnt = 0, count = 0, rc = 0, namelen = 0;
    int    state = FIRST, type = ALPHA;
    int    fd;				/* file descriptor used for display */
    char   charactr, readbuf[81];

    VOL1   vol1buf;
    REC1   hdr1buf;
    REC2   hdr2buf;
    LVCB  *lvcbptr;

#if	CFS
    LOADDISP        loaddisp;
#endif	
	load_display_t loaddisp; 
	struct mtop mt_com; 
	/* 
	 * Verify that the handle is valid (points to LVCB).
	 */
    lvcbptr = tapemain.lvcblist[handle];
    if (lvcbptr == NULL)
    {
	return (TERR_INV_HANDLE);
    }
    /*--------------------------------------------------------------------
     * Validity Checks 
     *--------------------------------------------------------------------*/
    if (dcb->volseqno > lvcbptr->volcount)
	return (TERR_INVALID_VOLSEQNO);
    if (dcb->volseqno == 0)
	return (TERR_INVALID_VOLSEQNO);
    if (lvcbptr->tdsopen == 1)
	return (TERR_ALREADY_TDSOPEND);
    /*--------------------------------------------------------------------
     * Validite Data Set Name
     *--------------------------------------------------------------------*/
    QT1 ("TDSOPEN Issued.  Verifying DSNAME ...");
    namelen = strlen (dcb->dsname);
    if (namelen > 17)
    {
	return (TERR_INVALID_DSNAME_LEN);
    }
    for (count = 0, qualcnt = 0; count < (sizeof (dcb->dsname) -1); count++, qualcnt++)
    {
	if (qualcnt >= 9)
	{
	    return (TERR_INVALID_DSNAME_QUALLEN);
	}
	charactr = dcb->dsname[count];
	if (((charactr >= 0x41) && (charactr <= 0x5a)) ||
	    ((charactr >= 0x61) && (charactr <= 0x7a)) ||
	    ((charactr == '#') || (charactr == '@') || (charactr == '$')))
	    type = ALPHA;
	else
	    if (((charactr >= 0x30) && (charactr <= 0x39)) ||
		(charactr == '-'))
	    type = NUMERIC;
	else
	if (charactr == '.')
	    type = DOT;
	else
	if (charactr == ' ')
	    type = BLANK;
	else
	{
	    QT1 ("Invalid Character found");
	    return (TERR_INVALID_DSNAME_CHAR);
	}
	if (state == FIRST)
	{
	    if (type == ALPHA)
	    {
		state = NOTFIRST;
	    } else
	    {
		return (TERR_INVALID_DSNAME_FRSTQUAL);
	    }
	} else
	if (state == NOTFIRST)
	{
	    if (type == DOT)
	    {
		state = FIRST;
		qualcnt = -1;
	    }
	    if (type == BLANK)
	    {
		state = END;
		qualcnt = -1;
	    }
	} else
	{			/* state == END */
	    if (type == BLANK)
	    {
		qualcnt = -1;
	    } else
	    {
		QT1 ("Invalid Character found");
		return (TERR_INVALID_DSNAME_CHAR);
	    }
	}
    }
    QT1 ("DSNAME Verified.");
    QT3 ("Get Correct Volume");
    /*----------------------------------------------------------------------
     *                        Get Correct Volume
     * Possible States:
     *   1. NEVER ALLOCATED
     *        Get correct volume loaded.  Read VOL1 and HDR1.
     *   2. ALLOCATED_VERIFIED & requested volume < current volume
     *        Error.  Can't move stack backwards.  
     *   3. ALLOCATED_VERIFIED & requested volume = current volume
     *        No processing.  Should be positioned before HDR1 or before 2nd TM.
     *   4. ALLOCATED_VERIFIED & requested volume > current volume
     *        Get correct volume loaded.  Read VOL1 and HDR1.
     * 
     *---------------------------------------------------------------------*/

    if ((lvcbptr->volfsm == ALLOCATED_VERIFIED) &&
	(dcb->volseqno < lvcbptr->cur_volseqno))
    {
	return (TERR_VOLSEQNO_ORDER);
    }
    if ((lvcbptr->volfsm == ALLOCATED_VERIFIED) &&
	(dcb->volseqno > lvcbptr->cur_volseqno))
    {
	QT1 ("Open done previously, but for a different volume");
	QT1 ("Issuing CLOSE");
	close (lvcbptr->fildes);
	lvcbptr->volfsm = ALLOCATED_NOT_VERIFIED;
    }
    if (lvcbptr->volfsm != ALLOCATED_VERIFIED)
    {
	if (lvcbptr->tapedev != 1)
	{			/* not 9-track */
	    loaddisp.message[0] = 0xE0;
	    strcpy (&loaddisp.message[1], "M");
	    strcpy (&loaddisp.message[2], lvcbptr->vollist[dcb->volseqno].serialno);
	    strcpy (&loaddisp.message[8], "S");
	    strncpy (&loaddisp.message[9], &loaddisp.message[1], 8);
	    /* load_display_tape (lvcbptr->fildes, &loaddisp); */
	}
	while (lvcbptr->cur_volseqno < dcb->volseqno)
	{
	    printf ("should not be in here yet\n");

	    /*
	     * for ( ; test_drive_ready(lvcbptr->path); ) { sleep(1); } 
	     */
	    QT1 ("Unloading tape (1)");

	    mt_com.mt_op = MTOFFL;
	    mt_com.mt_count = 1;

	    /*
	     * replace the fake ipsc860 unload_tape call with a true ioctl()
	     * call. 
	     * unload_tape(lvcbptr->path); 
	     */
	    ioctl (lvcbptr->fildes, MTIOCTOP, &mt_com);

	    if (lvcbptr->tapedev != 1)
	    {			/* not 9-track */
		loaddisp.message[0] = 0xE0;
		strcpy (&loaddisp.message[1], "M");
		strcpy (&loaddisp.message[2], lvcbptr->vollist[dcb->volseqno].serialno);
		strcpy (&loaddisp.message[8], "S");
		strncpy (&loaddisp.message[9], &loaddisp.message[1], 8);
		/* load_display_tape (lvcbptr->fildes, &loaddisp); */
		lvcbptr->cur_volseqno++;
	    }
	}
	open_load_disp (handle);
	load_display_tape (lvcbptr->fildes, &loaddisp); 
	lvcbptr->volfsm = ALLOCATED_NOT_VERIFIED;
    }
    if (lvcbptr->volfsm == ALLOCATED_NOT_VERIFIED)
    {
	/*--------------------------------------------------------------------
         *   Read VOL1 and HDR1 into temporary fields (vol1buf and hdr1buf), 
         *   and verify the VOL1 and HDR1 records.
         *--------------------------------------------------------------------*/
	QT1 ("Calling Verify_Vol");
	rc = verify_vol (handle, &vol1buf, &hdr1buf);
	QT2 ("After verify_vol");
	printf("vol1buf=%s\n",vol1buf);
	printf("hdr1buf=%s\n",hdr1buf);
	if (rc != 0)
	{
	    cleanup (handle);
	    printf ("return 1 rc %d\n", rc);
	    return (rc);
	} else
	{
	    lvcbptr->volfsm = ALLOCATED_VERIFIED;
	    lvcbptr->dsfsm = BOT;
	}
    }
    /*--------------------------------------------------------------------
     *                 DONE WITH VOLUME CHECKING !!
     *                 Should be positioned after HDR1.
     *-------------------------------------------------------------------*/


    /*--------------------------------------------------------------------
     *                 Data Set section
     * If Block ID specified, process it.
     * If EOT (at end of tape)
     *   If opened for Read
     *     If requested dsseqno <= last dsseqno
     *       Rewind.  (Search Later.)
     *     Else DATASET_NOT_FOUND.
     *   Else if requested dsseqno = last + 1
     *     Write.
     *   Else ERROR.
     * If BOT (Beginning of tape)
     *   If scratch tape
     *     If opened for Read ERROR.
     *     Else Write.
     *   Else (Search later).
     * If CLOSED (not BOT or EOT)
     *   If opened for Read
     *     If requested dsseqno <= last dsseqno
     *       Rewind.  (Search Later.)
     *     Else (Search later).
     *   Else ERROR.
     * SEARCH ROUTINE.
     *-------------------------------------------------------------------*/
    if ((dcb->blocknum != 0) && (lvcbptr->tapedev != 1))
    {
	QT1 ("Calling open_locate");
	rc = open_locate (handle, dcb, &hdr1buf, &hdr2buf);
	printf ("return 2, rc %d\n", rc);
	return (rc);
    }
    if (lvcbptr->dsfsm == EOT)
    {
	if (dcb->read == 1)
	{
	    if (dcb->dsseqno <= lvcbptr->last_dsseqno)
	    {
		QT1 ("Rewinding");
		t3480_rew (lvcbptr->fildes, 1);
#if CLREOF
		cleareof (lvcbptr->fildes);
#endif
		QT1 ("Reading VOL1");
		count = read (lvcbptr->fildes, readbuf, sizeof (readbuf));
		if (count != 80)
		{
		    cleanup (handle);
		    printf ("return NOT80\n");
		    return (TERR_HDRLEN_NOT80);
		}
		QT1 ("Reading HDR1");
		count = read (lvcbptr->fildes, readbuf, sizeof (readbuf));
		if (count != 80)
		{
		    cleanup (handle);
		    printf ("return NOT80\n");
		    return (TERR_HDRLEN_NOT80);
		}
		rec1fibm (&hdr1buf, &readbuf[0]);
	    } else
	    {
		cleanup (handle);
		return (TERR_DSSEQNO_NOT_FOUND);
	    }
	} else
	{
	    if (dcb->dsseqno == (lvcbptr->last_dsseqno + 1))
	    {
		QT1 ("Calling found_output");
		rc = found_output (handle, dcb);
		if (rc != 0)
		    return (rc);
		return (0);
	    } else
	    {
		cleanup (handle);
		return (TERR_CANNOT_APPEND);
	    }
	}
    }
    if (lvcbptr->dsfsm == BOT)
    {
	QT1 ("Reading tape label");
	count = read (lvcbptr->fildes, readbuf, sizeof (readbuf));
	printf("after read, count=%d\n",count);
	if (count == 0)
	{			/* If this is a scratch tape */
	    if (dcb->read == 1)
	    {
		cleanup (handle);
		return (TERR_INVALID_INPUT_TAPE_SCRATCH);
	    }
	    /*
            * If this is a scratch tape that is opened for OUTPUT, and dsseqno 
            * is zero or 1, back up to the dummy HDR1, write HDR1/2, User 
            * Labels and Tapemark.
            */
	    else
	    if ((dcb->dsseqno == 0) || (dcb->dsseqno == 1))
	    {
		QT1 ("Scratch tape opened for output, and DCB requested first DS on tape");
		QT1 ("BSF and BSR");
		t3480_bsf (lvcbptr->fildes, 1);
		t3480_bsr (lvcbptr->fildes, 1);

		/*
	         * FOUND !!!   SCRATCH TAPE OPENED FOR OUTPUT.
	         * Call found_output.
	         */
		QT1 ("Calling found_output");
		rc = found_output (handle, dcb);
		if (rc != 0)
		    return (rc);
		return (0);
	    } else
	    {
		cleanup (handle);
		return (TERR_INVALID_DSSEQNO);
	    }
	}
	rec2fibm (&hdr2buf, &readbuf[0]);	/* Save REC2 */


	/*
         * If dsseqno is zero, or the seqno of the first on the tape,
         *   If opened for output, return w/ error - cannot overwrite an
         *     existing dataset.
         */
	if ((dcb->dsseqno == 0) || (hdr1buf.dsseqno == dcb->dsseqno))
	{
	    if (dcb->read == 0)
	    {
		cleanup (handle);
		return (TERR_CANNOT_APPEND);
	    } else
	    {
		if ((strcmp (hdr1buf.dsname, dcb->dsname) != 0))
		{
		    cleanup (handle);
		    return (TERR_INVALID_DSNAME_SPECIFIED);
		}
		/*
	         * FOUND !!!   DSSEQNO=0 or first on tape.
	         */
		if ((strcmp (lvcbptr->vol1.volser, hdr1buf.dsserno) != 0))
		{
		    cleanup (handle);
		    return (TERR_NOT_START_OF_DATASET);
		}
		QT1 ("Calling found_input (1)");
		rc = found_input (dcb, lvcbptr->tindex, &hdr1buf, &hdr2buf);
		return (rc);
	    }
	}
    }
    if (lvcbptr->dsfsm == CLOSED)
    {
	if (dcb->dsseqno == (lvcbptr->last_dsseqno + 1))
	{
	    if (dcb->read == 0)
	    {
		cleanup (handle);
		return (TERR_CANNOT_APPEND);
		return (rc);
	    } else
	    {			/* read */
		QT1 ("Reading HDR1");
		count = read (lvcbptr->fildes, readbuf, sizeof (readbuf));
		if (count != 80)
		{
		    cleanup (handle);
		    printf ("return NOT80\n");
		    return (TERR_HDRLEN_NOT80);
		}
		rec1fibm (&hdr1buf, &readbuf[0]);
		if ((strcmp (hdr1buf.dsname, dcb->dsname) != 0))
		{
		    cleanup (handle);
		    return (TERR_INVALID_DSNAME_SPECIFIED);
		}
		if ((strcmp (lvcbptr->vol1.volser, hdr1buf.dsserno) != 0))
		{
		    printf ("hdr1buf.dsname=%s, hdr1buf.dsserno=%s, lvcbptr->vol1.volser=%s\n", hdr1buf.dsname, hdr1buf.dsserno, lvcbptr->vol1.volser);
		    QT2 ("WHY1");
		    QT3 ("WHY1");
		    cleanup (handle);
		    return (TERR_NOT_START_OF_DATASET);
		}
		QT1 ("Reading HDR2");
		count = read (lvcbptr->fildes, readbuf, sizeof (readbuf));
		if (count != 80)
		{
		    cleanup (handle);
		    printf ("return NOT80\n");
		    return (TERR_HDRLEN_NOT80);
		}
		rec2fibm (&hdr2buf, &readbuf[0]);

		QT1 ("Calling found_input (2)");
		rc = found_input (dcb, lvcbptr->tindex, &hdr1buf, &hdr2buf);
		return (rc);
	    }
	} else
	if (dcb->dsseqno < (lvcbptr->last_dsseqno + 1))
	{
	    QT1 ("Rewinding");
	    t3480_rew (lvcbptr->fildes, 1);
	    QT1 ("Reading VOL1");
	    count = read (lvcbptr->fildes, readbuf, sizeof (readbuf));
	    if (count != 80)
	    {
		cleanup (handle);
		printf ("return NOT80\n");
		return (TERR_HDRLEN_NOT80);
	    }
	    QT1 ("Reading HDR1");
	    count = read (lvcbptr->fildes, readbuf, sizeof (readbuf));
	    if (count != 80)
	    {
		cleanup (handle);
		printf ("return NOT80\n");
		return (TERR_HDRLEN_NOT80);
	    }
	    rec1fibm (&hdr1buf, &readbuf[0]);
	} else
	{			/* dcb->dsseqno > lvcbptr->last_dsseqno  */
	    QT1 ("Reading HDR1");
	    count = read (lvcbptr->fildes, readbuf, sizeof (readbuf));
	    if (count != 80)
	    {
		cleanup (handle);
		printf ("return NOT80\n");
		return (TERR_HDRLEN_NOT80);
	    }
	    rec1fibm (&hdr1buf, &readbuf[0]);

	    /*----------------------------------------------------------------
             * Doesn't look like next (commented out) section is necessary.
             *---------------------------------------------------------------*/

	    /*
	     * QT1("Reading HDR2"); count = read(lvcbptr->fildes , readbuf,
	     * sizeof(readbuf)); if (count != 80) { cleanup(handle);
	     * printf("return NOT80\n"); return(TERR_HDRLEN_NOT80); }
	     * rec2fibm(&hdr2buf, &readbuf[0]); 
	     */
	}
    }
    /*------------------------------------------------------------------
     *                   SEARCHING ...
     * When looking at a header file, jump over 2 Tapemarks.
     * The possibilities after the 2nd TM are:
     *   EOF1/2, (UTL1-n), TM, TM  (End)
     *   EOF1/2, (UTL1-n), TM, HDR (Continue on same tape)
     *   EOV, TM                   (Switch to next tape)
     *------------------------------------------------------------------*/
printf("dcb.dsseqno=%d\n",dcb->dsseqno);
    while ((hdr1buf.dsseqno < dcb->dsseqno))
    {
printf("hdr1buf.dsseqno=%d\n",hdr1buf.dsseqno);
	QT1 ("FSF");
	rc = t3480_fsf (lvcbptr->fildes, 2);
	if (rc != 0)
	{
	    cleanup (handle);
	    return (TERR_FSF_FAILED);
	}
	QT1 ("Reading EOF/EOV");
	read (lvcbptr->fildes, readbuf, sizeof (readbuf));
	rec1fibm (&hdr1buf, &readbuf[0]);
	if (strcmp (hdr1buf.labelid, "EOV") == 0)
	{
	    /* no switching volumes trying to find dataset. */
	    cleanup (handle);
	    return (TERR_DSSEQNO_NOT_FOUND);
	} else
	{
	    /*--------------------------------------------------------------
             * Must be EOF1.  FSF past EOF2 (and User labels).  The next
             * read will either be a new HDR1 record or a TM (end of tape).
             *--------------------------------------------------------------*/
	    QT1 ("FSF");
	    rc = t3480_fsf (lvcbptr->fildes, 1);
#if CLREOF
	    cleareof (lvcbptr->fildes);
#endif
	    QT1 ("Reading HDR1 or TM");
	    count = read (lvcbptr->fildes, readbuf, sizeof (readbuf));
	    if (count == 0)
	    {
		if (dcb->read == 1)
		{
		    cleanup (handle);
		    return (TERR_DSSEQNO_NOT_FOUND);
		} else
		if (hdr1buf.dsseqno == (dcb->dsseqno -1))
		{
		    /*------------------------------------------------------------
	             * The previous FSF positioned the tape between two TMs. 
	             * The next read positioned it after the TMs.  BSF puts it 
	             * back between the TMs.
	             *------------------------------------------------------------*/
		    QT1 ("BSF");
		    t3480_bsf (lvcbptr->fildes, 1);
		    /*----------------------------------------------------------
	             * SUCCESS !!!! Output data set will be the last one on the tape. 
	             * Call found_output.
	             *----------------------------------------------------------*/
		    QT1 ("Calling found_output");
		    rc = found_output (handle, dcb);
		    return (rc);
		} else
		{
		    cleanup (handle);
		    return (TERR_INVALID_DSSEQNO);
		}
	    } else
	    {			/* another HDR1 read */
		rec1fibm (&hdr1buf, &readbuf[0]);
	    }
	}
    }
    if (hdr1buf.dsseqno == dcb->dsseqno)
    {
	if (dcb->read == 0)
	{
	    cleanup (handle);
	    return (TERR_CANNOT_APPEND);
	} else
	{
	    if ((strcmp (hdr1buf.dsname, dcb->dsname) != 0))
	    {
		cleanup (handle);
		return (TERR_INVALID_DSNAME_SPECIFIED);
	    }
	    rec1fibm (&lvcbptr->rec1, &readbuf[0]);
	    QT1 ("Reading HDR2");
	    count = read (lvcbptr->fildes, readbuf, sizeof (readbuf));
	    if (count != 80)
	    {
		cleanup (handle);
		printf ("return NOT80\n");
		return (TERR_HDRLEN_NOT80);
	    }
	    rec2fibm (&hdr2buf, &readbuf[0]);

	    /*--------------------------------------------------------------
             * FOUND !!! after searching for input data set.
             --------------------------------------------------------------*/
	    QT1 ("Calling found_input (3)");
	    rc = found_input (dcb, lvcbptr->tindex, &hdr1buf, &hdr2buf);
	    return (rc);
	}
    }
    return (rc);
}

void
cleanup (int handle)
{
    LVCB           *lvcbptr;

    /*--------------------------------------------------------------------
     * Issue a UNIX close and reset the lvcb and dcb fields.
     *--------------------------------------------------------------------*/

    lvcbptr = tapemain.lvcblist[handle];

    QT1 ("Rewinding - Cleanup");
    t3480_rew (lvcbptr->fildes, 1);
    lvcbptr->volfsm = NEVER_ALLOCATED;
    close (lvcbptr->fildes);
    lvcbptr->tindex = 0;
    lvcbptr->fildes = 0;
    lvcbptr->last_dsseqno = 0;
    memset ((char *) &lvcbptr->rec1, 0x00, sizeof (lvcbptr->rec1));
    memset ((char *) &lvcbptr->rec2, 0x00, sizeof (lvcbptr->rec2));
    lvcbptr->blockcnt = 0;
    lvcbptr->located = 0;
    lvcbptr->written = 0;
    lvcbptr->tdsopen = 0;
}


int
found_output (int handle, DCB * dcb)
{
    LVCB	*lvcbptr;
    int		rc, count, i;
    int		error;		/* store value of errno after read/write */
    char	wrtbuf[80];
    EXITPARM	*parmptr;
    struct tm	*timeptr;
    time_t	secsnow;

    /*
     * timezone = 8 * 60 * 60; 
     */
    time (&secsnow);
    timeptr = localtime (&secsnow);
    lvcbptr = tapemain.lvcblist[handle];

    /*
     * Not a scratch tape; a valid labeled tape.  If opened for output and
     * the the expiration date has not been reached, return with error. 
     */
    if (dcb->read == 0)
    {
	if (lvcbptr->expirec == ' ')
	    i = 0;
	else
	{
	    char            tempstr[2];

	    tempstr[0] = lvcbptr->expirec;
	    tempstr[1] = '\0';
	    i = 100 + (100 * atoi (tempstr));
	}
	if (((lvcbptr->expireyy + i) > timeptr->tm_year) ||
	    (((lvcbptr->expireyy + i) == timeptr->tm_year) &&
	     (lvcbptr->expiredd > (timeptr->tm_yday + 1))))
	{
	    QT1 ("Expire user exit ???");
	    if (lvcbptr->exlst != NULL)
	    {
		parmptr = (EXITPARM *) malloc (sizeof (EXITPARM));
		parmptr->volseqno = lvcbptr->cur_volseqno;
		strcpy (&parmptr->volser[0],
			lvcbptr->vollist[lvcbptr->cur_volseqno].serialno);
		strcpy (&parmptr->dsname[0], lvcbptr->rec1.dsname);
		parmptr->dsseqno = lvcbptr->rec1.dsseqno;
		memset (&parmptr->buffer[0], '\0',
			sizeof (parmptr->buffer));
		memset (&parmptr->bufferhd[0], '\0',
			sizeof (parmptr->bufferhd));
		QT1 ("Calling OVERWRITE_EXPIRE user exit");
		rc = lvcbptr->exlst (OVERWRITE_EXPIRE, parmptr);
		if (rc != 1)
		{
		    free (parmptr);
		    cleanup (handle);
		    return (TERR_EXPIRATION_DATE);
		}
	    }
	}
    }
    hdrsfdcb (handle, dcb);
    /*----------------------------------------------------------
      * Copy option fields from DCB to LVCB. 
      *----------------------------------------------------------*/
    lvcbptr->last_dsseqno = dcb->dsseqno;
    lvcbptr->blocknum = dcb->blocknum;
    lvcbptr->read = dcb->read;
    lvcbptr->buffered_mode = dcb->buffered_mode;
    lvcbptr->block_mode = dcb->block_mode;
    lvcbptr->num_tapeblk_bufs = dcb->num_tapeblk_bufs;
    QT2 ("Calling tape_io_init (1)");
    rc = tape_io_init (lvcbptr->tindex);
    if (rc != 0)
    {
	cleanup (lvcbptr->tindex);
	return (rc);
    }
    rec1tibm (&lvcbptr->rec1, &wrtbuf[0]);
    QT1 ("Writing HDR1 (found_output)");
    count = write (lvcbptr->fildes, wrtbuf, sizeof (wrtbuf));
    error = errno;
    if (count != sizeof (wrtbuf))
    {
#if TEOM
	/* replace tiseom with check of errno */
	if (!tiseom (lvcbptr->fildes))
#endif
	if ( errno == ENOSPC )
	    return (TERR_CWRITE_FAILED);
    }
    rec2tibm (&lvcbptr->rec2, &wrtbuf[0]);
    QT1 ("Writing HDR2");
    count = write (lvcbptr->fildes, wrtbuf, sizeof (wrtbuf));
    error = errno;
    if (count != sizeof (wrtbuf))
    {
#if TEOM
	/* replace tiseom with check of errno */
	if (!tiseom (lvcbptr->fildes))
#endif
	if ( errno == ENOSPC )
	    return (TERR_CWRITE_FAILED);
    }
    if (lvcbptr->exlst != NULL)
    {
	QT1 ("Calling OUTPUT_HDR_LABEL user exit");
	rc = userexit_output (lvcbptr->tindex, OUTPUT_HEADER_LABEL);
	if (rc != 0)
	    return (rc);
    }
    QT1 ("Writing tapemark");
    rc = t3480_weof (lvcbptr->fildes, 1);
    lvcbptr->tdsopen = 1;
    return (rc);
}

int
found_input (DCB * dcb, int handle, REC1 * hdr1buf, REC2 * hdr2buf)
{
    LVCB           *lvcbptr;
    int             rc;

    lvcbptr = tapemain.lvcblist[handle];
    /*--------------------------------------------------------------------
     * Routine entered after reading HDR1 and HDR2.  It returns after
     * having read any possible user header labels and the following
     * tapemark.
     --------------------------------------------------------------------*/

    /*-------------------------------------------------------------
    * GENERATE REC1 and REC2.
    *-------------------------------------------------------------*/
    lvcbptr->rec1 = *hdr1buf;
    lvcbptr->rec2 = *hdr2buf;

    /*-------------------------------------------------------------
     * Copy fields from tape to user's DCB.
     *------------------------------------------------------------*/
    dcb->expirec = lvcbptr->rec1.expirec;
    dcb->expireyy = lvcbptr->rec1.expireyy;
    dcb->expiredd = lvcbptr->rec1.expiredd;
    dcb->recfm = lvcbptr->rec2.recfm;
    dcb->blocklen = lvcbptr->rec2.blocklen;
    dcb->reclen = lvcbptr->rec2.reclen;
    dcb->blkattr = lvcbptr->rec2.blkattr;

    rc = userexit_input (lvcbptr->tindex, INPUT_HEADER_LABEL, "UHL");
    if (rc != 0)
	return (rc);
    /*----------------------------------------------------------
     * Copy option fields from DCB to LVCB. 
     *----------------------------------------------------------*/
    lvcbptr->last_dsseqno = dcb->dsseqno;
    lvcbptr->blocknum = dcb->blocknum;
    lvcbptr->read = dcb->read;
    lvcbptr->buffered_mode = dcb->buffered_mode;
    lvcbptr->block_mode = dcb->block_mode;
    lvcbptr->num_tapeblk_bufs = dcb->num_tapeblk_bufs;
    QT2 ("Calling tape_io_init (2) ");
    rc = tape_io_init (lvcbptr->tindex);
    if (rc != 0)
    {
	cleanup (lvcbptr->tindex);
	return (rc);
    }
    lvcbptr->tdsopen = 1;
    return (0);
}

int
userexit_input (int handle, int type, char label[5])
{
    LVCB           *lvcbptr;
    int             i, count;
    char            tempstr[5], readbuf[81];
    EXITPARM       *parmptr;

    /*--------------------------------------------------------------------
     * Verify that the handle is valid (points to LVCB).
     *--------------------------------------------------------------------*/
    lvcbptr = tapemain.lvcblist[handle];
    if (lvcbptr == NULL)
    {
	return (TERR_INV_HANDLE);
    }
    /*----------------------------------------------------------------
     * User Exit to read Header Labels
     *----------------------------------------------------------------*/
    for (i = 0; i < 8; i++)
    {
	count = read (lvcbptr->fildes, readbuf, sizeof (readbuf));
	if (count == 0)
	{
	    return (0);
	} else
	{
	    if (lvcbptr->exlst != NULL)
	    {
		stretoa (&readbuf[0], &tempstr[0], 3);
		if (strcmp (&tempstr[0], label) == 0)
		{
		    if (lvcbptr->exlst != NULL)
		    {
			parmptr = (EXITPARM *) malloc (sizeof (EXITPARM));
			parmptr->volseqno = lvcbptr->cur_volseqno;
			strcpy (&parmptr->volser[0], lvcbptr->vollist[lvcbptr->cur_volseqno].serialno);
			strcpy (&parmptr->dsname[0], lvcbptr->rec1.dsname);
			parmptr->dsseqno = lvcbptr->rec1.dsseqno;
			memcpy (&parmptr->bufferhd[0], &readbuf[0], 4);
			memcpy (&parmptr->buffer[0], &readbuf[4], 76);
			QT1 ("Calling user exit");
			lvcbptr->exlst (type, parmptr);
			free (parmptr);
		    }
		}
	    }
	}
    }
    if (i == 8)
    {				/* read all the user labels, haven't read
				 * tapemark. */
	count = read (lvcbptr->fildes, readbuf, sizeof (readbuf));
	if (count != 0)
	{
	    return (TERR_NOTM_USRLABELS);
	}
    }
    return (0);
}

int
userexit_output (int handle, int type)
{
    LVCB	*lvcbptr;
    char	wrtbuf[80];
    EXITPARM	*parmptr;
    int		rc, flag, count;
    int		error;		/* store errno, if set by write */

    /*--------------------------------------------------------------------
     * Verify that the handle is valid (points to LVCB).
     *--------------------------------------------------------------------*/
    lvcbptr = tapemain.lvcblist[handle];
    if (lvcbptr == NULL)
    {
	return (TERR_INV_HANDLE);
    }
    /*----------------------------------------------------------------
     * User Exit to write Header Labels
     *----------------------------------------------------------------*/
    parmptr = (EXITPARM *) malloc (sizeof (EXITPARM));
    parmptr->volseqno = lvcbptr->cur_volseqno;
    strcpy (&parmptr->volser[0], lvcbptr->vollist[lvcbptr->cur_volseqno].serialno);
    strcpy (&parmptr->dsname[0], lvcbptr->rec1.dsname);
    parmptr->dsseqno = lvcbptr->rec1.dsseqno;
    memset (&parmptr->bufferhd[0], '\0', sizeof (parmptr->bufferhd));
    memset (&parmptr->buffer[0], '\0', sizeof (parmptr->buffer));
    if (type == OUTPUT_HEADER_LABEL)
	stratoe ("UHL", &parmptr->bufferhd[0]);
    else
	stratoe ("UTL", &parmptr->bufferhd[0]);
    for (rc = 1, flag = 0; ((rc != 0) && (flag < 8)); flag++)
    {
	itoz (flag + 1, &parmptr->bufferhd[3], 1);
	QT1 ("Calling user exit");
	rc = lvcbptr->exlst (type, parmptr);
	if (rc == 1)
	{
	    memcpy (&wrtbuf[0], &parmptr->bufferhd[0], 4);
	    memcpy (&wrtbuf[4], &parmptr->buffer[0], 76);
	    count = write (lvcbptr->fildes, &wrtbuf[0], sizeof (wrtbuf));
	    error = errno;
	    if (count != sizeof (wrtbuf))
	    {
#if TEOM
		/* replace tiseom with check of errno */
		if (!tiseom (lvcbptr->fildes))
#endif
		if ( error == ENOSPC )
		{
		    free (parmptr);
		    return (TERR_CWRITE_FAILED);
		}
	    }
	}
    }
    free (parmptr);
    return (0);
}

int
open_locate (int handle, DCB * dcb, REC1 * hdr1buf, REC2 * hdr2buf)
{
    LVCB           *lvcbptr;
    char            readbuf[81];
    int             rc = 0, count = 0;


    /*--------------------------------------------------------------------
     * Verify that the handle is valid (points to LVCB).
     *--------------------------------------------------------------------*/
    lvcbptr = tapemain.lvcblist[handle];
    if (lvcbptr == NULL)
    {
	return (TERR_INV_HANDLE);
    }
#if CLREOF
    cleareof (lvcbptr->fildes);
#endif

    /*--------------------------------------------------------------------
     * If the request is for a write with blocknum specified, ERROR.
     *--------------------------------------------------------------------*/

    if (dcb->read == 0)
    {
	return (TERR_BLOCKNUM_ON_WRITE);
    }
printf("locate_tape_id: from tdsopen\n");
    rc = locate_tape_id (lvcbptr->fildes, dcb->blocknum);
    if (rc != 0)
    {
	return (rc);
    }
    count = read (lvcbptr->fildes, readbuf, sizeof (readbuf));
    if (count != 80)
    {
	QT1 ("Open w/ locate failed - count not 80 bytes");
	open_locate_failed (handle);
	return (TERR_INVALID_BLOCKNUM);
    }
    rec1fibm (hdr1buf, &readbuf[0]);
    if ((strcmp (hdr1buf->labelid, "HDR") != 0) ||
	(hdr1buf->labelno != '1'))
    {
	QT1 ("Open w/ locate failed - not HDR1");
	open_locate_failed (handle);
	return (TERR_INVALID_BLOCKNUM);
    }
    if ((hdr1buf->dsseqno != dcb->dsseqno) ||
	(strcmp (hdr1buf->dsname, dcb->dsname) != 0))
    {
	QT1 ("Open w/ locate failed - dsnames don't match");
	open_locate_failed (handle);
	return (TERR_INVALID_BLOCKNUM);
    }
    count = read (lvcbptr->fildes, readbuf, sizeof (readbuf));
    if (count != 80)
    {
	QT1 ("Open w/ locate failed - count not 80");
	open_locate_failed (handle);
	return (TERR_INVALID_BLOCKNUM);
    }
    rec2fibm (hdr2buf, &readbuf[0]);

    if ((strcmp (hdr2buf->labelid, "HDR") != 0) ||
	(hdr2buf->labelno != '2'))
    {
	QT1 ("Open w/ locate failed - not HDR2");
	open_locate_failed (handle);
	return (TERR_INVALID_BLOCKNUM);
    }
    /*--------------------------------------------------------------
      * FOUND !!! after searching for input data set.
      --------------------------------------------------------------*/
    rc = found_input (dcb, lvcbptr->tindex, hdr1buf, hdr2buf);
    return (rc);
}


int
open_locate_failed (int handle)
{
    LVCB           *lvcbptr;

    /*--------------------------------------------------------------------
     * Verify that the handle is valid (points to LVCB).
     * Clear the DCB, rec1 and rec2.
     *--------------------------------------------------------------------*/
    lvcbptr = tapemain.lvcblist[handle];
    if (lvcbptr == NULL)
    {
	return (TERR_INV_HANDLE);
    }
    QT1 ("Rewinding - open_locate_failed");
    t3480_rew (lvcbptr->fildes, 1);
    lvcbptr->last_dsseqno = 0;
    lvcbptr->volfsm = NEVER_ALLOCATED;
    memset ((char *) &lvcbptr->rec1, 0x00, sizeof (lvcbptr->rec1));
    memset ((char *) &lvcbptr->rec2, 0x00, sizeof (lvcbptr->rec2));
    lvcbptr->blockcnt = 0;
    lvcbptr->written = 0;
    lvcbptr->located = 0;
    return (0);
}

int
feov (int handle)
{
    int             rc;
    LVCB           *lvcbptr;

    /*--------------------------------------------------------------------
     * Verify that the handle is valid (points to LVCB).
     * Clear the DCB, rec1 and rec2.
     *--------------------------------------------------------------------*/
    lvcbptr = tapemain.lvcblist[handle];
    if (lvcbptr == NULL)
    {
	return (TERR_INV_HANDLE);
    }
    if (lvcbptr->fildes == 0)
    {
	return (TERR_NO_DATASET_OPENED);
    }
    /*---------------------------------------------------------------
     * Call eod to process FEOV.
     *---------------------------------------------------------------*/
    lvcbptr->feov = 1;
    rc = eod (handle);
    if (rc == NEW_VOL)
    {
	return (0);
    }
    return (rc);
}
