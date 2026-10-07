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
 *	conv_dat.c 11.1 94/03/22 16:49:17
 */
static char	conv_dat_ver[] = "@(#) sourcefile conv_dat.c 11.1 94/03/22 16:49:17";

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
 *  CONV_DATA.C --  ASCII <-> EBCDIC  Conversion tables
 *
 *
 *
 *
 --------------------------------------------------*/

#include "convec.h"

#define     NOTR    0xFF  /* Characters that have no translation
                             EBCDIC <->ASCII */



const unsigned char ebcdic_to_ascii[] = {

    0x00, 0x01, 0x02, 0x03, NOTR, 0x09, NOTR, 0x7F,   /* 00 - 07 */
    NOTR, NOTR, NOTR, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,   /* 08 - 0F */
    0x10, 0x11, 0x12, 0x13, NOTR, NOTR, 0x08, NOTR,   /* 10 - 17 */
    0x18, 0x19, NOTR, NOTR, 0x1C, 0x1D, 0x1E, 0x1F,   /* 18 - 1F */
    NOTR, NOTR, NOTR, NOTR, NOTR, 0x0A, 0x17, 0x1B,   /* 20 - 27 */
    NOTR, NOTR, NOTR, NOTR, NOTR, 0x05, 0x06, 0x07,   /* 28 - 2F */
    NOTR, NOTR, 0x16, NOTR, NOTR, NOTR, NOTR, 0x04,   /* 30 - 37 */
    NOTR, NOTR, NOTR, NOTR, 0x14, 0x15, NOTR, 0x1A,   /* 38 - 3F */
    ' ' , NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR,   /* 40 - 47 */
    NOTR, NOTR, NOTR, '.' , '<' , '(' , '+' , '|' ,   /* 48 - 4F */
    '&' , NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR,   /* 50 - 57 */
    NOTR, NOTR, '!' , '$' , '*' , ')' , ';' , '^' ,   /* 58 - 5F */
    '-' , '/' , NOTR, NOTR, NOTR, NOTR, NOTR, NOTR,   /* 60 - 67 */
    NOTR, NOTR, NOTR, ',' , '%' , '_' , '>' , '?' ,   /* 68 - 6F */
    NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR,   /* 70 - 77 */
    NOTR, '`' , ':' , '#' , '@' , 0x27, '=' , '"' ,   /* 78 - 7F */
    NOTR, 'a' , 'b' , 'c' , 'd' , 'e' , 'f' , 'g' ,   /* 80 - 87 */
    'h' , 'i' , NOTR, NOTR, NOTR, NOTR, NOTR, NOTR,   /* 88 - 8F */
    NOTR, 'j' , 'k' , 'l' , 'm' , 'n' , 'o' , 'p' ,   /* 90 - 97 */
    'q' , 'r' , NOTR, NOTR, NOTR, NOTR, NOTR, NOTR,   /* 98 - 9F */
    NOTR, '~' , 's' , 't' , 'u' , 'v' , 'w' , 'x' ,   /* A0 - A7 */
    'y' , 'z' , NOTR, NOTR, NOTR, NOTR, NOTR, NOTR,   /* A8 - AF */
    NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR,   /* B0 - B7 */
    NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR,   /* B8 - BF */
    0x7B, 'A' , 'B' , 'C' , 'D' , 'E' , 'F' , 'G' ,   /* C0 - C7 */
    'H' , 'I' , NOTR, NOTR, NOTR, NOTR, NOTR, NOTR,   /* C8 - CF */
    0x7D, 'J' , 'K' , 'L' , 'M' , 'N' , 'O' , 'P' ,   /* D0 - D7 */
    'Q' , 'R' , NOTR, NOTR, NOTR, NOTR, NOTR, NOTR,   /* D8 - DF */
    0x5C, NOTR, 'S' , 'T' , 'U' , 'V' , 'W' , 'X' ,   /* E0 - E7 */
    'Y' , 'Z' , NOTR, NOTR, NOTR, NOTR, NOTR, NOTR,   /* E8 - EF */
    '0' , '1' , '2' , '3' , '4' , '5' , '6' , '7' ,   /* F0 - F7 */
    '8' , '9' , NOTR, NOTR, NOTR, NOTR, NOTR, NOTR    /* F8 - FF */

};

const unsigned char ascii_to_ebcdic[] = {

    0x00, 0x01, 0x02, 0x03, 0x37, 0x2D, 0x2E, 0x2F,   /* 00 - 07 */
    0x16, 0x05, 0x25, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,   /* 08 - 0F */
    0x10, 0x11, 0x12, 0x13, 0x3C, 0x3D, 0x32, 0x26,   /* 10 - 17 */
    0x18, 0x19, 0x3F, 0x27, 0x1C, 0x1D, 0x1E, 0x1F,   /* 18 - 1F */
    0x40, 0x5A, 0x7F, 0x7B, 0x5B, 0x6C, 0x50, 0x7D,   /* 20 - 27 */
    0x4D, 0x5D, 0x5C, 0x4E, 0x6B, 0x60, 0x4B, 0x61,   /* 28 - 2F */
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,   /* 30 - 37 */
    0xF8, 0xF9, 0x7A, 0x5E, 0x4C, 0x7E, 0x6E, 0x6F,   /* 38 - 3F */
    0x7C, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,   /* 40 - 47 */
    0xC8, 0xC9, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6,   /* 48 - 4F */
    0xD7, 0xD8, 0xD9, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6,   /* 50 - 57 */
    0xE7, 0xE8, 0xE9, 0xAD, 0xE0, 0xBD, 0x5F, 0x6D,   /* 58 - 5F */
    0x79, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,   /* 60 - 67 */
    0x88, 0x89, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96,   /* 68 - 6F */
    0x97, 0x98, 0x99, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,   /* 70 - 77 */
    0xA7, 0xA8, 0xA9, 0xC0, 0x4F, 0xD0, 0xA1, 0x07,   /* 78 - 7F */

    NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR,   /* 80 - 87 */
    NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR,   /* 88 - 8F */
    NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR,   /* 90 - 97 */
    NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR,   /* 98 - 9F */
    NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR,   /* A0 - A7 */
    NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR,   /* A8 - AF */
    NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR,   /* B0 - B7 */
    NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR,   /* B8 - BF */
    NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR,   /* C0 - C7 */
    NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR,   /* C8 - CF */
    NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR,   /* D0 - D7 */
    NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR,   /* D8 - DF */
    NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR,   /* E0 - E7 */
    NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR,   /* E8 - EF */
    NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR,   /* F0 - F7 */
    NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR, NOTR    /* F8 - FF */


};

char *cnv_etoa_p = (char *)&ebcdic_to_ascii[0];
char *cnv_atoe_p = (char *)&ascii_to_ebcdic[0];


struct CERR_INFO {
  int   err_code;
  char  *err_str;
} cerr_info[] = {

/* Data conversion Errors */

/* 000 */  {CERR_INVALID_ZONED_LEN, ""},
/* 001 */  {CERR_INVALID_ZONED_NUM, ""},
/* 002 */  {CERR_INVALID_SIGN, ""},
/* 003 */  {CERR_INVALID_PACKED_LEN, ""},
/* 004 */  {CERR_INVALID_PACKED_NUM, ""},
/* 005 */  {CERR_OVERFLOW_ZONE_LEN, ""},
/* 006 */  {CERR_OVERFLOW_PACKED_LEN, ""},
/* 007 */  {CERR_INVALID_ZONE_DIGIT, ""},

/* 008 */  {CERR_LOSS_OF_PRECISION, ""},
/* 009 */  {CERR_UNDERFLOW, ""},
/* 010 */  {CERR_OVERFLOW, ""},


};

