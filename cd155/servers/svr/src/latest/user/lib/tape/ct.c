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
 
/*------------------------------------------------------------------------------
 *  ct.c
 *
 *   Copy a tape.
 *
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license agreement
 *  or nondisclosure agreement with Intel Corporation and may not be
 *  copied nor disclosed except in accordance with the terms of that
 *  agreement.
 *
 *	ct.c 11.1 94/03/22 16:49:20
 */
static char	ct_ver[] = "@(#) sourcefile ct.c 11.1 94/03/22 16:49:20";

/*
 *
 *  Usage: ct [ -n ] from_tapedevice to_tapedevice
 *
 *  History:
 *  10-30-91   g t  Created
 *
 *----------------------------------------------------------------------------*/
#define DEBUG 0
#define COUNT (34*1024)
#define CFS 0

#include <fcntl.h>
#include <stdio.h>
/*
#include <malloc.h>
*/
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include "mtio.h"
#include <nodedef.h>
#include <msgtype.h>
#include <pid.h>
#include <cfs.h>

#if	CFS
#include "tape.h"
#include <diskproc.h> 
#include <nameproc.h>
#endif	/* CFS */

#include <fio.h>
#include <iod.h>
#include <scsi.h>

  struct mtop s;

Usage() {

fprintf(stderr,"Usage: ct [-n] from_tapedevice to_device\n");
fprintf(stderr,"\t-n  specifies not an IBM-format tape (double tape mark at EOT)\n");

}

#if	CFS
void cleareof(int fd) {
    fds[fd].fs_pid &= ~FEOF;
}
#endif	/* CFS */

main(ac, av)
    int ac;
    char **av;

{
	char	nmode;
	char	*program;
	char	*ptr;
	int		num,
			len,
			tapemode;			/* 1 = HDR, 2 = data, 3 = EOF/V */
	char	itape[80],
			otape[80];
	int		ifd,
			ofd;
	char	tm,
			eov1,
			eov2,
			utln;


/*PAGE*/
    /*
    **   Deal with command line arguments.
    */
    program = *av;                     /* First argument is program name . . . */
    av++;
    ac--;

    if ((ac < 2) || (ac > 3)) {
        fprintf(stderr, "%s: incorrect number of arguments = %d\n", program, ac);
        Usage();
        exit(1);
    }

    /*
    **
    */
	nmode = 0;
    if (strcmp(*av, "-n") == 0) {      /* -n if specified, is first */
        nmode = 1;
#if DEBUG
        printf("ctspider: n = %s\n",*av);
#endif
        av++;
        ac--;
    }

    if (ac != 2) {
        fprintf(stderr, "%s: incorrect arguments\n", program);
        Usage();
        exit(1);
    }

	/*
	**  From Tapedevice.
	*/
    strncpy(itape, *av, 80);
	av++;
    ac--;
#if DEBUG
    printf("ctspider: from_tapedevice=%s\n", itape);
#endif

	if (itape[0] == '-') {
        fprintf(stderr, "%s: incorrect arguments\n", program);
        Usage();
        exit(1);
    }

	/*
	**  To Tapedevice.
	*/
    strncpy(otape, *av, 80);
#if DEBUG
    printf("ctspider: to_tapedevice=%s\n", otape);
#endif

	if (otape[0] == '-') {
        fprintf(stderr, "%s: incorrect arguments\n", program);
        Usage();
        exit(1);
    }


/*PAGE*/
    /*
    **   Get memory.
    */
#if DEBUG
    printf("ctspider: initializing buffer\n");
#endif
    if ((ptr = (char *) malloc(COUNT)) == 0x00) {
        fprintf(stderr, "%s: malloc failed\n", program);
        exit(1);
    }

    /*
    **   Open tape devices.
    */
#if DEBUG
    printf("ctspider: opening input tape device %s . . . ",itape);
#endif
    if ((ifd = open(itape, O_RDONLY)) == -1) {
        perror(itape);
        exit(1);
    }
#if DEBUG
    else
        printf("successful - fd=%08lX\n", ifd);
#endif

#if DEBUG
    printf("ctspider: opening output tape device %s . . . ",otape);
#endif
    if ((ofd = open(otape, O_WRONLY)) == -1) {
        perror(otape);
        exit(1);
    }
#if DEBUG
    else
        printf("successful - fd=%08lX\n", ofd);
#endif


/*PAGE*/
    /*
    ** Read and copy the tape.
    */
    printf("\n");
	tm=eov1=eov2=utln=0;
	tapemode = 1;
    do {
        num = read(ifd, ptr, COUNT);
        if (num < 0) {
           	fprintf(stderr,"%s: read failure = %d\n", program, num);
         	exit(1);
		}
        else if (num > 0) {          /* Got data */
            len = write(ofd, ptr, num);
            if (len != num) {
            	fprintf(stderr,"%s: write failure = %d\n", program, len);
            	exit(1);
			}
			if (tapemode == 3) {
				if (eov2) {
					if ((len == 80) &&
					   (strncmp(strnetoa(ptr,ptr,3), "UTL", 3))) {
						utln += 1;
						if (utln > 8) {
							eov2 = 0;
							}
						}
					}
					else {
						eov2 = 0;
					}
				if (eov1) {
					if ((len == 80) &&
					(strncmp(strnetoa(ptr,ptr,4), "EOV2", 4))) {
						eov2 = 1;
						utln = 0;
					}
					eov1 = 0;
				}
				if (tm) {
					if ((len == 80) &&
					(strncmp(strnetoa(ptr,ptr,4), "EOV1", 4))) {
						eov1 = 1;
					}
				}
            }
			tm = 0;
        }	

        else if (num == 0) {			/* Got tape mark */
#if	CFS
			cleareof(ifd);
#endif	/* CFS */
			s.mt_op = MTWEOF;
    		s.mt_count = 1;
			len = ioctl(ofd, MTIOCTOP, &s);
			if (len < 0) {
      			fprintf(stderr,"a%s: ioctl failed: %d\n", program, len);
    			return(-1);
			}
			if (nmode || 
			   (!nmode && (tapemode != 2)))
				tm++;
    		if ((tapemode+=1) == 4)
    			tapemode=1;
    		eov1=eov2=utln=0;
		}
		if (!nmode && tm==1 && eov2==1)
			 tm++;
	}	while(tm < 2);

	close(ifd);
	close(ofd);
	free(ptr);
    printf("\nTape copy complete\n");
    exit(0);

}
