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
 *		INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *	uitoa.c 11.1 94/03/22 16:49:54
 */
static char	uitoa_ver[] = "@(#) sourcefile uitoa.c 11.1 94/03/22 16:49:54";

/*
 *
 *  ******************  ENGINEERING CHANGE HISTORY ***********************
 *
 *    Date      Engineer                  Description
 *  
 *  2/25/92     G. Haycox       Module creation release to BETA
 *
 *
 */

/*--------------------------------------------------
 * uitoa()
 *
 * INPUT:   uint_var:	unsigned integer to be converted
 *          ascii:	   	pointer to buffer for the ascii result
 *
 * OUTPUT:  int:     the number of zoned characters created

 * Description:
 *
 *      Convert an intel 32 bit integer (unsigned) into a null
 *		terminated ascii string.
 *
 --------------------------------------------------*/

void uitoa(unsigned int uintel_int, char ascii[]) {

	unsigned int temp, j, q;
	int i;
	unsigned char work[10];

	if(uintel_int) {

		for (i=0; uintel_int; ++i) {
			work[i] = '0' + (uintel_int % 10);
			uintel_int /= 10;
		}
		for (j=0, --i; i >= 0; --i, ++j){
			ascii[j] = work[i];
		}
        ascii[j] = 0;
	}
	else {   /* ZERO */
		ascii[0] = '0';
        ascii[1] = 0;
	}
}

