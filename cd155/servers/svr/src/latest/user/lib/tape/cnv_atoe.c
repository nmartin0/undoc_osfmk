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
 *	cnv_atoe.c 11.1 94/03/22 16:49:14
 */
static char	cnv_atoe_ver[] = "@(#) sourcefile cnv_atoe.c 11.1 94/03/22 16:49:14";

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
 * cnv_atoe.c
 *
 * Description:
 *
 *  This module contains the following character
 *  conversion routines and translation table
 *  redirection routines for ASCII to EBCDIC:
 *
 *      strnatoe()
 *      stratoe()
 *      set_atoe_trans()
 *
 --------------------------------------------------*/

extern char *cnv_atoe_p;
extern char ascii_to_ebcdic[];

/*--------------------------------------------------
 * atoe()
 *
 * INPUT:   ascii:    ASCII character
 *
 * OUTPUT:            EBCDIC character
 *
 * Description:
 *
 *      Convert an ASCII character a EBCDIC character
 *
 *      A 0xFF will be returned if there is no EBCDIC
 *      translation for the ASCII character.
 *
 --------------------------------------------------*/

char atoe(char ascii) {

    return (cnv_atoe_p[(unsigned char)ascii] & 0xFF);
}




/*--------------------------------------------------
 * strnatoe()
 *
 * INPUT:   ascii_p:    pointer to an array of ASCII characters
 *          ebcdic_p:   pointer to where converted EBCDIC characters
 *                      are stored
 *
 *          length:     number of characters to convert
 *
 * OUTPUT:   0:     if all characters are convertable
 *          -1:     if any character can not be converted
 *
 * Description:
 *
 *      Convert an array of ASCII characters to an array of
 *      EBCDIC characters, the number of charactes converted is
 *      determined by the length parameter.
 *
 *      The entire buffer will be translated with all non-EBCDIC
 *      equivalent characters translated to 0xFF in the EBCDIC
 *      buffer.
 *
 --------------------------------------------------*/

int strnatoe(char *ascii_p, char *ebcdic_p, int length) {

	int return_val = 0;
	while(length--) {
        	*ebcdic_p = atoe(*ascii_p);
        	if ((unsigned char)*ebcdic_p == 0xFF) {
			return_val = -1;
		}
		++ebcdic_p;
		++ascii_p;
	}
	return (return_val);
}

/*--------------------------------------------------
 * stratoe()
 *
 * INPUT:   ascii_p:    pointer to an array of ASCII characters
 *          ebcdic_p:   pointer to where converted EBCDIC characters
 *                      are stored
 *
 *
 * OUTPUT:   0:     if all characters are convertable
 *          -1:     if any character can not be converted
 *
 * Description:
 *
 *      Convert a null (0x00) terminated ASCII string of characters
 *      to an array of EBCDIC characters.  The EBCDIC array will
 *      contain 1 less byte after the conversion as the null (0x00)
 *      character is stripped.
 *
 *      The entire buffer will be translated with all non-EBCDIC
 *      equivalent characters translated to 0xFF in the EBCDIC
 *      buffer.
 *
 --------------------------------------------------*/

int stratoe(char *ascii_p, char *ebcdic_p) {

	int return_val = 0;
	while(*ascii_p) {
        	*ebcdic_p = atoe(*ascii_p);
        	if ((unsigned char)*ebcdic_p == 0xFF) {
			return_val = -1;
		}
		++ebcdic_p;
		++ascii_p;
	}
	return (return_val);
}

/*--------------------------------------------------
 * set_atoe_trans()
 *
 * INPUT:   new_table_p:    pointer to a new ASCII to
 *                          EBCDIC translation table to use.
 *                          If a null (0x00) pointer is passed
 *                          then the default conversion table
 *                          will be used.
 *
 * OUTPUT:  cur_table_p:    pointer to the current ASCII to
 *                          EBCDIC translation table.
 *
 * Description:
 *
 *
 *      The function allows the ASCII to EBCDIC translation
 *      table to be changed.
 *
 --------------------------------------------------*/

char *set_atoe_trans(char *new_table_p) {

	char *cur_table_p = cnv_atoe_p;

	if ((cnv_atoe_p = new_table_p) == (char *)0) {
		cnv_atoe_p = &ascii_to_ebcdic[0];
	}
	return (cur_table_p);
}
