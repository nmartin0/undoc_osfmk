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
 *	cnv_etoa.c 11.1 94/03/22 16:49:17
 */
static char	cnv_etoa_ver[] = "@(#) sourcefile cnv_etoa.c 11.1 94/03/22 16:49:17";

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
 * cnv_etoa.c
 *
 * Description:
 *
 *  This module contains the following character
 *  conversion routines and translation table
 *  redirection routines for EBCDIC to ASCIi:
 *
 *      strnetoa()
 *      stretoa()
 *      set_etoa_trans()
 *
 *
 --------------------------------------------------*/

extern char *cnv_etoa_p;
extern char ebcdic_to_ascii[];

/*--------------------------------------------------
 * etoa()
 *
 * INPUT:   ebcdic:    EBCDIC character
 *
 * OUTPUT:             ASCII character
 *
 * Description:
 *
 *      Convert a EBCDIC character an ASCII character
 *
 *      A 0xFF will be returned if there is no ASCII
 *      translation for the EBCDIC character.
 *
 --------------------------------------------------*/

char etoa(char ebcdic) {

    return (cnv_etoa_p[(unsigned char)ebcdic] & 0xFF);
}


/*--------------------------------------------------
 * strnetoa()
 *
 * INPUT:   ebcdic_p:   pointer to an array of EBCDIC characters
 *          ascii_p:    pointer to where converted ASCII characters
 *                      are stored
 *          length:     number of characters to convert
 *
 * OUTPUT:   0:     if all characters are convertable
 *          -1:     if any character can not be converted
 *
 * Description:
 *
 *      Convert an array of EBCDIC characters to an array of
 *      ASCII characters, the number of charactes converted is
 *      determined by the length parameter.
 *
 *      The entire buffer will be translated with all non-ASCII
 *      equivalent characters translated to 0xFF in the ASCII
 *      buffer.
 *
 --------------------------------------------------*/

int strnetoa(char *ebcdic_p, char *ascii_p, int length) {

    int return_val = 0;
    while(length--) {
        *ascii_p = etoa(*ebcdic_p);
        if ((unsigned char)*ascii_p == 0xFF) {
            return_val = -1;
        }
        ++ebcdic_p;
        ++ascii_p;
    }
    return (return_val);
}


/*--------------------------------------------------
 * stretoa()
 *
 * INPUT:   ebcdic_p:   pointer to an array of EBCDIC characters
 *          ascii_p:    pointer to where converted ASCII characters
 *                      are stored
 *
 * OUTPUT:   0:     if all characters are convertable
 *          -1:     if any character can not be converted
 *
 * Description:
 *
 *
 *      Convert an array of EBCDIC characters to a null (0x00)
 *      terminated array of ASCII characters, the number of
 *      charactes converted is determined by the length parameter.
 *
 *      The entire buffer will be translated with all non-ASCII
 *      equivalent characters translated to 0xFF in the ASCII
 *      buffer.
 *
 *      Ensure that the ASCII array is one byte longer than
 *      number of characters to convert.
 --------------------------------------------------*/

int 
stretoa(char *ebcdic_p, char *ascii_p, int length) 
{
    int return_val = 0;

    while(length--) {
        *ascii_p = etoa(*ebcdic_p);
        if ((unsigned char)*ascii_p == 0xFF) {
            return_val = -1;
        }
        ++ebcdic_p;
        ++ascii_p;
    }
    *ascii_p = 0;
    return (return_val);
}



/*--------------------------------------------------
 * set_etoa_trans()
 *
 * INPUT:   new_table_p:    pointer to a new EBCDIC to
 *                          ASCII translation table to use.
 *                          If a null (0x00) pointer is passed
 *                          then the default conversion table
 *                          will be used.
 *
 * OUTPUT:  cur_table_p:    pointer to the current EBCDIC to
 *                          ASCII translation table.
 *
 * Description:
 *
 *
 *      The function allows the EBCDIC to ASCII translation
 *      table to be changed.
 *
 --------------------------------------------------*/

char *set_etoa_trans(char *new_table_p) {

    char *cur_table_p = cnv_etoa_p;

    if ((cnv_etoa_p = new_table_p) == (char *)0) {
        cnv_etoa_p = &ebcdic_to_ascii[0];
    }
    return (cur_table_p);
}

