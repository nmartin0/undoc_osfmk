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
 * therest.c 11.1 94/03/22 16:49:41 
 */
static char     therest_ver[] = "@(#) sourcefile therest.c 11.1 94/03/22 16:49:41";

/* 
 */

#include <stdio.h>

#include <type.h>
#include "tapeio.h"
#include <sys/ioctl.h>
#include <mtio.h>
#include "tape3480.h"
#include "extern.h"
#include <string.h>
#include "protos.h"
#include "defines.h"
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <sys/errno.h>

int
tdsclose (int handle)
{
    LVCB	*lvcbptr;
    char	readbuf[80], writebuf[80];
    int		rc, count;
    int		error;	/* store errno after read or write */
    /*
     * Verify that the handle is valid (points to LVCB). Clear the DCB, rec1
     * and rec2. 
     */
    lvcbptr = tapemain.lvcblist[handle];
    if (lvcbptr == NULL)
    {
	return (TERR_INV_HANDLE);
    }
    if (lvcbptr->fildes == 0)
    {
	return (TERR_NO_DATASET_OPENED);
    }
    QT1 ("TDSCLOSE Entered");
    QT2 ("Calling tape_io_exit");
    rc = tape_io_exit (handle);
    if (rc != 0)
    {
	return (rc);
    }
    /*--------------------------------------------------------------------
     * If TDSCLOSE was issued for an INPUT data set without hitting EOD, 
     *   FSF, Call EOD, and continue.
     *--------------------------------------------------------------------*/
    if (lvcbptr->read == 1)
    {
	if (lvcbptr->eodhit == 0)
	{
	    lvcbptr->eodhit = 1;
	    eod (lvcbptr->tindex);
	}
    }
    /*--------------------------------------------------------------------
     * For OUTPUT files, TDSCLOSE writes the EOF1/2 records,
     * drives the user exit to write any User Labels, and writes
     * two tapemarks.  
     * 
     * For INPUT and OUTPUT files, it clears the dcb, rec1 and rec2 and
     * issues a UNIX close for the device.
     *--------------------------------------------------------------------*/
    if (lvcbptr->read == 0)
    {
	QT1 ("Writing Tapemark");
	t3480_weof (lvcbptr->fildes, 1);
	strcpy (lvcbptr->rec1.labelid, "EOF");
	strcpy (lvcbptr->rec2.labelid, "EOF");
	lvcbptr->rec1.blockcnt = lvcbptr->blockcnt;
	rec1tibm (&lvcbptr->rec1, &writebuf[0]);
	QT1 ("Writing EOF1");
	count = write (lvcbptr->fildes, writebuf, sizeof (writebuf));
	error = errno;
	if (count != sizeof (writebuf))
	{
#if TEOM    /* replace tiseom with errno check */
	    if (!tiseom (lvcbptr->fildes))
#endif
	    if ( error == ENOSPC )
		return (TERR_CWRITE_FAILED);
	}
	rec2tibm (&lvcbptr->rec2, &writebuf[0]);
	QT1 ("Writing EOF2");
	count = write (lvcbptr->fildes, writebuf, sizeof (writebuf));
	error = errno;
	if (count != sizeof (writebuf))
	{
#if TEOM    /* replace tiseom with errno check */
	    if (!tiseom (lvcbptr->fildes))
#endif
	    if ( error == ENOSPC )
		return (TERR_CWRITE_FAILED);
	}
	if (lvcbptr->exlst != NULL)
	{
	    rc = userexit_output (lvcbptr->tindex, OUTPUT_TRAILER_LABEL);
	    if (rc != 0)
		return (rc);
	}
	QT1 ("Writing 2 tapemarks");
	t3480_weof (lvcbptr->fildes, 1);
	t3480_weof (lvcbptr->fildes, 1);
	/*------------------------------------------------------------------
         * Leave it positioned between the two TM's, just as for read.
         *-----------------------------------------------------------------*/
	QT1 ("BSF");
	t3480_bsf (lvcbptr->fildes, 1);
	lvcbptr->dsfsm = EOT;
    }
    /*--------------------------------------------------------------------
     * For a data set we've just read, read one more record to see if
     * we're at the end of the tape.  If we read a TM, we're at end of tape.
     *-------------------------------------------------------------------*/
    else
    {
	QT1 ("Reading to see if we're at the end of the tape");
	/* Already read one TM.  Clear EOF field before reading second */
#if CLREOF
	cleareof (lvcbptr->fildes);
#endif
	count = read(lvcbptr->fildes, readbuf, sizeof (readbuf));
	if (count == 0)
	{
	    QT1 ("BSF");
	    t3480_bsf (lvcbptr->fildes, 1);
#if CLREOF
	    cleareof (lvcbptr->fildes);
#endif
	    lvcbptr->dsfsm = EOT;
	} else
	{
	    QT1 ("BSR");
	    t3480_bsr (lvcbptr->fildes, 1);
	    lvcbptr->dsfsm = CLOSED;
	}
    }
    memset ((char *) &lvcbptr->rec1, 0x00, sizeof (lvcbptr->rec1));
    memset ((char *) &lvcbptr->rec2, 0x00, sizeof (lvcbptr->rec2));
    lvcbptr->blockcnt = 0;
    lvcbptr->eodhit = 0;
    lvcbptr->located = 0;
    lvcbptr->written = 0;
    lvcbptr->tdsopen = 0;

    return (0);
}



int
tapedealloc (int handle)
{
    LVCB           *lvcbptr;
    int             rc;
    struct mtop     mt_com;
    int             fd;

    /*
     * Verify that the handle is valid (points to LVCB). Clear the DCB, rec1
     * and rec2. 
     */

    lvcbptr = tapemain.lvcblist[handle];
    if (lvcbptr == NULL)
    {
	return (TERR_INV_HANDLE);
    }
    QT1 ("Tapedealloc issued");

#if 0
    unload_tape (lvcbptr->path);
#endif

    if (lvcbptr->debug_nounload != 1)
    {
	mt_com.mt_op = MTOFFL;
	mt_com.mt_count = 1;
	ioctl (lvcbptr->fildes, MTIOCTOP, &mt_com);
    }
    rc = close (lvcbptr->fildes);
    if (rc != 0)
    {
	return (TERR_CLOSE_FAILED);
    }
#if RELTAPE
    rc = release_tape ((char *) lvcbptr->path);
    if (rc != 0)
    {
	return (TERR_RELEASE_TAPE_FAILED);
    }
#endif

    tapemain.lvcblist[handle] = NULL;
    free (lvcbptr);
    return (0);
}


int
eod (int handle)
{

    LVCB           *lvcbptr;
    int             rc, count;
    int		error;		/* store value of errno after read or write */
    char            readbuf[81], writebuf[80];
    REC1            hdr1buf;
    VOL1            vol1buf;
    EXITPARM       *parmptr;

    /*
     * LOADDISP loaddisp; 
     */
    load_display_t  loaddisp;
    int             fd;
    struct mtop     mt_com;

    /*------------------------------------------------------------------
     * Verify that the handle is valid (points to LVCB).
     * Clear the DCB, rec1 and rec2.
     *------------------------------------------------------------------*/

    lvcbptr = tapemain.lvcblist[handle];
    if (lvcbptr == NULL)
    {
	return (TERR_INV_HANDLE);
    }
    QT1 ("EOD Entered");

    /*------------------------------------------------------------------
     * If eod is being called as a result of FEOV, have to call
     *   tape_io_exit to allow read/write code to clean up and get 
     *   positioned on the EOT side of the TM.
     *
     *------------------------------------------------------------------*/
    if (lvcbptr->feov == 1)
    {
	QT2 ("Calling tape_io_exit");
	rc = tape_io_exit (handle);
	if (rc != 0)
	{
	    return (rc);
	}
    }
    if (lvcbptr->read == 1)
    {
	/*--------------------------------------------------
          INPUT PROCESSING
         --------------------------------------------------*/

	/*------------------------------------------------------------------
         * If eod is being called as a result of FEOV, have to call
         *   tape_io_exit to allow read/write code to clean up and get 
         *   positioned on the EOT side of the TM.
         *
         *
         *  NOTE - following section of code deleted for test14a
         *------------------------------------------------------------------*/

	/*
	 * if ( lvcbptr->feov == 1 ) { QT2("Calling tape_io_exit"); rc =
	 * tape_io_exit(handle); if ( rc != 0 ) { return(rc); } }  
	 */
	if (lvcbptr->feov == 1)
	{
	    printf ("eod: lvcbptr->feov=1\n");
	    printf ("skipped tape_io_exit.\n");
	}
	QT1 ("Reading EOF or EOV");
	count = read (lvcbptr->fildes, readbuf, sizeof (readbuf));
	if (count != 80)
	{
	    printf ("count not 80 count=%d\n", count);
	    printf ("sizeof ( readbuf ) = %d\n", sizeof (readbuf));
	    return (TERR_HDRLEN_NOT80);
	}
	rec1fibm (&hdr1buf, &readbuf[0]);
	if (((strcmp (hdr1buf.labelid, "EOF") != 0) &&
	     (strcmp (hdr1buf.labelid, "EOV") != 0)) ||
	    (hdr1buf.labelno != '1'))
	{
	    return (TERR_REC1_NOT_FOUND);
	}
	/*------------------------------------------------------------------
         * EODHIT: To TDSCLOSE, it means EOD was driven before TDSCLOSE.
         *         To EOD, it means EOD is being driven from TDSCLOSE, and
         *           EOD should not check the block counts.
         *
         *         Also don't check block count if a TLOCATE was done.
         *------------------------------------------------------------------*/
	if ((lvcbptr->eodhit == 0) && (lvcbptr->located == 0) &&
	    (lvcbptr->feov == 0))
	{
	    if (lvcbptr->blockcnt != hdr1buf.blockcnt)
	    {
		return (TERR_BLKCNTS_DONT_MATCH);
	    }
	}
	printf ("got here in eod 2\n");
	lvcbptr->eodhit = 1;
	if (strcmp (hdr1buf.labelid, "EOF") == 0)
	{
	    /*------------------------------------------------------------------
             * Read EOF2 (Don't do anything with it, just read it.)
             *------------------------------------------------------------------*/
	    QT1 ("Reading EOF2");
	    count = read (lvcbptr->fildes, readbuf, sizeof (readbuf));
	    if (count != 80)
	    {
		return (TERR_HDRLEN_NOT80);
	    }
	    printf ("got here in eod 3\n");
	    /*----------------------------------------------------------------
             * User Exit for trailer labels   
             *----------------------------------------------------------------*/

	    rc = userexit_input (lvcbptr->tindex, INPUT_TRAILER_LABEL, "UTL");
	    if (rc != 0)
		return (rc);

	    if (lvcbptr->feov == 1)
	    {
		lvcbptr->feov = 0;
		return (TINFO_EOF_ENCOUNTERED);
	    }
	    return (0);
	} else
	{
	    /*-------------------------------------------------- 
             * Must be EOV.
             * Drive User Exit for EOV 
             * Close device, load_display, open quick, open forever
             * 
             *--------------------------------------------------*/
	    /*------------------------------------------------------------------
             * Read EOV2 (Don't do anything with it, just read it.)
             *------------------------------------------------------------------*/
	    QT1 ("Reading EOV2");
	    count = read (lvcbptr->fildes, readbuf, sizeof (readbuf));
	    if (count != 80)
	    {
		return (TERR_HDRLEN_NOT80);
	    }
	    /*----------------------------------------------------------------
             * User Exit for trailer labels   
             *----------------------------------------------------------------*/

	    rc = userexit_input (lvcbptr->tindex, INPUT_TRAILER_LABEL, "UTL");
	    if (rc != 0)
		return (rc);


	    /*----------------------------------------------------------------
             * User Exit for EOV
              *----------------------------------------------------------------*/
	    if (lvcbptr->exlst != NULL)
	    {
		parmptr = (EXITPARM *) malloc (sizeof (EXITPARM));
		parmptr->volseqno = lvcbptr->cur_volseqno;
		strcpy (&parmptr->volser[0], lvcbptr->vollist[lvcbptr->cur_volseqno].serialno);
		strcpy (&parmptr->dsname[0], lvcbptr->rec1.dsname);
		parmptr->dsseqno = lvcbptr->rec1.dsseqno;
		memset (&parmptr->bufferhd[0], '\0', sizeof (parmptr->bufferhd));
		memset (&parmptr->buffer[0], '\0', sizeof (parmptr->buffer));
		rc = lvcbptr->exlst (EOV_USEREXIT, parmptr);
		free (parmptr);
	    }
	    lvcbptr->cur_volseqno++;
	    if (lvcbptr->cur_volseqno > lvcbptr->volcount)
	    {
		if (lvcbptr->cur_volseqno > MAX_VOLUMES)
		    return (TERR_MAX_VOLS_EXCEEDED);
		/*--------------------------------------------------------------
	         * Drive User Exit to request next volser.
	         *--------------------------------------------------------------*/

		if (lvcbptr->exlst != NULL)
		{
		    parmptr = (EXITPARM *) malloc (sizeof (EXITPARM));
		    parmptr->volseqno = lvcbptr->cur_volseqno;
		    strcpy (&parmptr->volser[0], NULL);
		    strcpy (&parmptr->dsname[0], lvcbptr->rec1.dsname);
		    parmptr->dsseqno = lvcbptr->rec1.dsseqno;
		    memset (&parmptr->bufferhd[0], '\0', sizeof (parmptr->bufferhd));
		    memset (&parmptr->buffer[0], '\0', sizeof (parmptr->buffer));
		    rc = lvcbptr->exlst (NEXT_VOLSER, parmptr);
		    if (rc != 1)
		    {
			free (parmptr);
			return (TERR_NO_NEXT_VOLSER);
		    } else
		    {
			strcpy (lvcbptr->vollist[lvcbptr->cur_volseqno].serialno, &parmptr->volser[0]);
			lvcbptr->volcount++;
			free (parmptr);
		    }
		} else
		    return (TERR_NO_NEXT_VOLSER);
	    }

	    /*
	     * In order to use ioctl to load the display, we need to
	     * translate the lvcbptr->path to a file descriptor. 
	     */
	    fd = open (lvcbptr->path, O_RDWR);
	    if (fd < 0)
	    {
		perror ("open");
		exit (1);
	    }
	    if (lvcbptr->tapedev != 1)
	    {
		loaddisp.message[0] = 0xE0;
		strncpy (&loaddisp.message[1], "M", 1);
		strncpy (&loaddisp.message[2], lvcbptr->vollist[lvcbptr->cur_volseqno].serialno, 6);
		strncpy (&loaddisp.message[8], "S", 1);
		strncpy (&loaddisp.message[9], &loaddisp.message[1], 8);
		ioctl (fd, MTIODISPLY, &loaddisp);
	    }
	    QT1 ("Closing device (1)");
	    rc = close (lvcbptr->fildes);
	    if (rc == -1)
		return (TERR_CLOSE_FAILED);
	    QT1 ("Unload tape");
	    mt_com.mt_op = MTOFFL;
	    mt_com.mt_count = 1;
	    rc = ioctl (fd, MTIOCTOP, &mt_com);

	    /*
	     * unload_tape(lvcbptr->path); 
	     */
	    close (fd);
	    /*----------------------------------------------------------------
             * Open and Load_Display
             *----------------------------------------------------------------*/
	    open_load_disp (handle);
	    QT1 ("Calling verify_vol");
	    rc = verify_vol (handle, &vol1buf, &hdr1buf);
	    if (rc != 0)
	    {
		return (rc);
	    }
	    /*----------------------------------------------------------------
             * The volume has been verified that the first two records are 80
             * bytes and contain "VOL1" and HDR1".  If there is no HDR2 record,
             * by definition this is a scratch tape, and cannot be opened for 
             * input.  
             * 
             * Drive User Exit for User Labels.
             *----------------------------------------------------------------*/
	    QT1 ("Reading HDR2 (or TM for scratch tape)");
	    count = read (lvcbptr->fildes, readbuf, sizeof (readbuf));
	    if (count == 0)
		return (TERR_INVALID_INPUT_TAPE_SCRATCH);
	    else
	    if (count != 80)
		return (TERR_HDRLEN_NOT80);

	    if ((strcmp (hdr1buf.dsname, lvcbptr->rec1.dsname) != 0))
	    {
		return (TERR_INVALID_DSNAME_SPECIFIED);
	    }
	    /*----------------------------------------------------------------
             * Looks like the volume switch occurred correctly.
             * Update REC1 and REC2.
             * 
             * 
             *----------------------------------------------------------------*/

	    lvcbptr->rec1 = hdr1buf;
	    rec2fibm (&lvcbptr->rec2, &readbuf[0]);
	    lvcbptr->last_dsseqno = lvcbptr->rec1.dsseqno;
	    rc = userexit_input (lvcbptr->tindex, INPUT_HEADER_LABEL, "UHL");
	    if (rc != 0)
		return (rc);
	    /* Following lines added for test13a */
	    if (lvcbptr->feov == 1)
	    {
		QT2 ("Calling tape_io_init (3) ");
		rc = tape_io_init (lvcbptr->tindex);
		if (rc != 0)
		{
		    return (rc);
		}
	    }
	    /* Above lines added for test13a */

	    lvcbptr->blockcnt = 0;	/* ##gary */
	    lvcbptr->located = 0;
	    lvcbptr->written = 0;
	    lvcbptr->feov = 0;
	    return (NEW_VOL);
	}
    } else
    {
	/*--------------------------------------------------
         * Opened for OUTPUT.
         * Write EOV1/2
         * If User Label processing, call User exit 
         * Write one TM
         * 2 exception conditions
         --------------------------------------------------*/
	QT1 ("Writing tapemark");
	t3480_weof (lvcbptr->fildes, 1);
	lvcbptr->rec1.blockcnt = lvcbptr->blockcnt;
	strcpy (lvcbptr->rec1.labelid, "EOV");
	strcpy (lvcbptr->rec2.labelid, "EOV");
	rec1tibm (&lvcbptr->rec1, &writebuf[0]);
	QT1 ("Writing EOV1");
	count = write (lvcbptr->fildes, writebuf, sizeof (writebuf));
	error = errno;
	if (count != sizeof (writebuf))
	{
#if TEOM    /* replace tiseom with errno check */
	    if (!tiseom (lvcbptr->fildes))
#endif
	    if ( error == ENOSPC )
		return (TERR_CWRITE_FAILED);
	}
	rec2tibm (&lvcbptr->rec2, &writebuf[0]);
	QT1 ("Writing EOV2");
	count = write (lvcbptr->fildes, writebuf, sizeof (writebuf));
	error = errno;
	if (count != sizeof (writebuf))
	{
#if TEOM    /* replace tiseom with errno check */
	    if (!tiseom (lvcbptr->fildes))
#endif
	    if ( error == ENOSPC )
		return (TERR_CWRITE_FAILED);
	}
	if (lvcbptr->exlst != NULL)
	{
	    rc = userexit_output (lvcbptr->tindex, OUTPUT_TRAILER_LABEL);
	    if (rc != 0)
		return (rc);
	}
	lvcbptr->cur_volseqno++;
	if (lvcbptr->cur_volseqno > lvcbptr->volcount)
	{
	    if (lvcbptr->cur_volseqno > MAX_VOLUMES)
		return (TERR_MAX_VOLS_EXCEEDED);
	    /*--------------------------------------------------------------
              * Drive User Exit to request next volser.
              *--------------------------------------------------------------*/
	    if (lvcbptr->exlst != NULL)
	    {
		parmptr = (EXITPARM *) malloc (sizeof (EXITPARM));
		parmptr->volseqno = lvcbptr->cur_volseqno;
		strcpy (&parmptr->volser[0], NULL);
		strcpy (&parmptr->dsname[0], lvcbptr->rec1.dsname);
		parmptr->dsseqno = lvcbptr->rec1.dsseqno;
		memset (&parmptr->bufferhd[0], '\0', sizeof (parmptr->bufferhd));
		memset (&parmptr->buffer[0], '\0', sizeof (parmptr->buffer));
		QT1 ("Calling Next_Volser user exit");
		rc = lvcbptr->exlst (NEXT_VOLSER, parmptr);
		if (rc != 1)
		{
		    free (parmptr);
		    return (TERR_NO_NEXT_VOLSER);
		}
		strcpy (lvcbptr->vollist[lvcbptr->cur_volseqno].serialno, &parmptr->volser[0]);
		lvcbptr->volcount++;
		free (parmptr);
	    } else
		return (TERR_NO_NEXT_VOLSER);
	}
	QT1 ("Closing device (2)");
	rc = close (lvcbptr->fildes);
	if (rc == -1)
	    return (TERR_CLOSE_FAILED);

	/*
	 * In order to use ioctl to load the display, we need to translate
	 * the lvcbptr->path to a file descriptor. 
	 */
	fd = open (lvcbptr->path, O_RDWR);
	if (fd < 0)
	{
	    perror ("open");
	    exit (1);
	}
	if (lvcbptr->tapedev != 1)
	{
	    loaddisp.message[0] = 0xE0;
	    strncpy (&loaddisp.message[1], "M", 1);
	    strncpy (&loaddisp.message[2], lvcbptr->vollist[lvcbptr->cur_volseqno].serialno, 6);
	    strncpy (&loaddisp.message[8], "S", 1);
	    strncpy (&loaddisp.message[9], &loaddisp.message[1], 8);
	    ioctl (fd, MTIODISPLY, &loaddisp);
	}
	/*------------------------------------------------------------------
         * Unload Tape
         *------------------------------------------------------------------*/
	QT1 ("Unloading tape (2)");
	mt_com.mt_op = MTOFFL;
	mt_com.mt_count = 1;
	rc = ioctl (fd, MTIOCTOP, &mt_com);

	/*
	 * unload_tape(lvcbptr->path); 
	 */
	close (fd);
	/*------------------------------------------------------------------
         * USER EXIT - EOV
         *------------------------------------------------------------------*/
	if (lvcbptr->exlst != NULL)
	{
	    parmptr = (EXITPARM *) malloc (sizeof (EXITPARM));
	    parmptr->volseqno = lvcbptr->cur_volseqno;
	    strcpy (&parmptr->volser[0], lvcbptr->vollist[lvcbptr->cur_volseqno].serialno);
	    strcpy (&parmptr->dsname[0], lvcbptr->rec1.dsname);
	    parmptr->dsseqno = lvcbptr->rec1.dsseqno;
	    memset (&parmptr->buffer[0], '\0', sizeof (parmptr->buffer));
	    memset (&parmptr->bufferhd[0], '\0', sizeof (parmptr->bufferhd));
	    rc = lvcbptr->exlst (EOV_USEREXIT, parmptr);
	    free (parmptr);
	}
	/*------------------------------------------------------------------
         * Open file.  User Exit for "waiting for open", if necessary.
         *------------------------------------------------------------------*/
	open_load_disp (handle);
	QT1 ("Calling verify_vol");
	rc = verify_vol (handle, &vol1buf, &hdr1buf);
	if (rc != 0)
	{
	    return (rc);
	}
	/*----------------------------------------------------------------
         * The volume has been verified that the first two records are 80
         * bytes and contain "VOL1" and HDR1".  If there is no HDR2 record,
         * by definition this is a scratch tape, and cannot be opened for 
         * input.  
         * 
         * Drive User Exit for User Labels.
         *----------------------------------------------------------------*/
	count = read (lvcbptr->fildes, readbuf, sizeof (readbuf));
	if (count == 80)
	    return (TERR_INVALID_DSSEQNO);
	QT1 ("BSF, BSR");
	t3480_bsf (lvcbptr->fildes, 1);
	t3480_bsr (lvcbptr->fildes, 1);

	/*----------------------------------------------------------------
         * Reset for new volume.
         *----------------------------------------------------------------*/
	strcpy (lvcbptr->rec1.labelid, "HDR");
	strcpy (lvcbptr->rec2.labelid, "HDR");
	lvcbptr->rec1.volseqno = lvcbptr->cur_volseqno;
	lvcbptr->rec1.blockcnt = 0;
	lvcbptr->blockcnt = 0;
	lvcbptr->located = 0;
	lvcbptr->written = 0;
	lvcbptr->feov = 0;

	QT2 ("Calling tape_io_init (4) ");
	rc = tape_io_init (lvcbptr->tindex);
	if (rc != 0)
	{
	    return (rc);
	}
	/*----------------------------------------------------------------
         * Write new HDRs.
         *----------------------------------------------------------------*/
	rec1tibm (&lvcbptr->rec1, &writebuf[0]);
	QT1 ("Writing HDR1");
	count = write (lvcbptr->fildes, writebuf, sizeof (writebuf));
	error = errno;
	if (count != sizeof (writebuf))
	{
#if TEOM    /* replace tiseom with errno check */
	    if (!tiseom (lvcbptr->fildes))
#endif	
	    if ( error == ENOSPC )
		return (TERR_CWRITE_FAILED);
	}
	rec2tibm (&lvcbptr->rec2, &writebuf[0]);
	QT1 ("Writing HDR2");
	count = write (lvcbptr->fildes, writebuf, sizeof (writebuf));
	error = errno;
	if (count != sizeof (writebuf))
	{
#if TEOM    /* replace tiseom with errno check */
	    if (!tiseom (lvcbptr->fildes))
#endif	
	    if ( error == ENOSPC )
		return (TERR_CWRITE_FAILED);
	}
	if (lvcbptr->exlst != NULL)
	{
	    rc = userexit_output (lvcbptr->tindex, OUTPUT_HEADER_LABEL);
	    if (rc != 0)
		return (rc);
	}
	QT1 ("Writing tapemark");
	rc = t3480_weof (lvcbptr->fildes, 1);
	/*----------------------------------------------------------------
         * Looks like the volume switch occurred correctly.
         *----------------------------------------------------------------*/
	lvcbptr->last_dsseqno = lvcbptr->rec1.dsseqno;
	return (NEW_VOL);
    }
}

int
verify_vol (int handle, VOL1 * vol1buf, REC1 * hdr1buf)
{
    LVCB           *lvcbptr;
    int             count;
    char            readbuf[81];

    lvcbptr = tapemain.lvcblist[handle];
    if (lvcbptr == NULL)
    {
	return (TERR_INV_HANDLE);
    }
    /*--------------------------------------------------------------------
     * Read VOL1 and HDR1.
     * Verify that the serial number of the volume is the one we were
     *   expecting.
     * Verify that the data set name is the one we were expecting.
     *--------------------------------------------------------------------*/
    count = read (lvcbptr->fildes, readbuf, sizeof (readbuf));
    printf ("after read for VOL1 count=%d\n", count);
    if (count != 80)
    {
	return (TERR_HDRLEN_NOT80);
    }
    vol1fibm (vol1buf, &readbuf[0]);
    if ((strcmp (vol1buf->labelid, "VOL") != 0) ||
	(vol1buf->labelno != '1'))
	return (TERR_VOL1_NOT_FOUND);

    if (strcmp (vol1buf->volser, lvcbptr->vollist[lvcbptr->cur_volseqno].serialno) != 0)
    {
	printf ("INVALID_VOLUME: volser=%s ", vol1buf->volser);
	printf (" lvcbptr->vollist[%d].serialno=%s\n",
		lvcbptr->cur_volseqno,
		lvcbptr->vollist[lvcbptr->cur_volseqno].serialno);
	return (TERR_INVALID_VOLUME);
    }
    memcpy (&lvcbptr->vol1, vol1buf, 80);


    count = read (lvcbptr->fildes, readbuf, sizeof (readbuf));
    printf ("after read for HDR1 count=%d\n", count);
    if (count != 80)
    {
	return (TERR_HDRLEN_NOT80);
    }
    rec1fibm (hdr1buf, &readbuf[0]);
    if ((strcmp (hdr1buf->labelid, "HDR") != 0) ||
	(hdr1buf->labelno != '1'))
	return (TERR_REC1_NOT_FOUND);
    /*--------------------------------------------------------------------
     * Save the Volume's expiration date
     *--------------------------------------------------------------------*/
    lvcbptr->expirec = hdr1buf->expirec;
    lvcbptr->expireyy = hdr1buf->expireyy;
    lvcbptr->expiredd = hdr1buf->expiredd;
    QT2 ("Returning from verify_vol");
    return (0);
}

int
open_load_disp (int handle)
{
    EXITPARM       *parmptr;

    /*
     * LOADDISP loaddisp; 
     */
    LVCB           *lvcbptr;
    int             i;
    load_display_t  loaddisp;
    int             fd;

    lvcbptr = tapemain.lvcblist[handle];
    QT2 ("open_load_disp");

#if TESTRDY
    for (i = 0; ((i < 25) && (test_drive_ready (lvcbptr->path))); i++)
    {
	sleep (1);
    }
    if (i >= 25)
    {
	/*----------------------------------------------------------------
         * User Exit for WAITING FOR OPEN
         * Not needed with the unix asynchronous open
         *----------------------------------------------------------------*/
	if (lvcbptr->exlst != NULL)
	{
	    parmptr = (EXITPARM *) malloc (sizeof (EXITPARM));
	    parmptr->volseqno = lvcbptr->cur_volseqno;
	    strcpy (&parmptr->volser[0], lvcbptr->vollist[lvcbptr->cur_volseqno].serialno);
	    strcpy (&parmptr->dsname[0], lvcbptr->rec1.dsname);
	    parmptr->dsseqno = lvcbptr->rec1.dsseqno;
	    memset (&parmptr->buffer[0], '\0', sizeof (parmptr->buffer));
	    memset (&parmptr->bufferhd[0], '\0', sizeof (parmptr->bufferhd));
	    lvcbptr->exlst (WAITING_FOR_OPEN, parmptr);
	    free (parmptr);
	}
    }
    for (; test_drive_ready (lvcbptr->path);)
    {
	sleep (1);
    }
#endif

    lvcbptr->fildes = open (lvcbptr->path, O_RDWR);

    if (lvcbptr->fildes == -1)
	return (TERR_CANNOT_OPEN);

#if 0
    if (lvcbptr->tapedev != 1)
    {
	loaddisp.message[0] = 0x20;
	strncpy (&loaddisp.message[1], " ", 1);
	strncpy (&loaddisp.message[2], lvcbptr->vollist[lvcbptr->cur_volseqno].serialno, 6);
	strncpy (&loaddisp.message[8], "S", 1);
	strcpy (&loaddisp.message[9], NULL);
	load_display_tape (lvcbptr->fildes, &loaddisp);
    }
#endif
    return 0;
}

int
t3480_interface (fd, cnt, cmd)
    int             fd, cnt, cmd;
{
    struct mtget    scsi_err;	/* will hold tape status on error */
    struct mtop     s;
    int             e = 0;

    s.mt_op = cmd;
    s.mt_count = cnt;
    if (ioctl (fd, MTIOCTOP, &s) == -1)
    {
	e = errno;
	ioctl (fd, MTIOCGET, &scsi_err);

	/*
	 * (void) mt_status(fd, 1); 
	 */
	errno = e;
    }
    return e;
}
