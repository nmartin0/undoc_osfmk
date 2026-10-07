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
 ***				 IDENTIFICATION				    ***
 ******************************************************************************
  Name:		$RCSfile: acurses.h,v $
  Version:	$Id: acurses.h,v 1.1.4.1 1995/06/11 23:28:44 kat Exp $
  Title:	'ace' screen management header
  Revision:	$Revision: 1.1.4.1 $
  Update Date:	$Date: 1995/06/11 23:28:44 $
		(last change by: $Author: kat $)
  Programmer:	bem
  Documents:	UNIX V.4 RAID Manager Release 3.0 FS 348-0024272

  COPYRIGHT 1992, NCR Corporation

  Description:	This header contains the screen defines.
*/

/******************************************************************************
 ***				 CHANGE RECORD				    ***
 ******************************************************************************
 * $Log: acurses.h,v $
 * Revision 1.1.4.1  1995/06/11  23:28:44  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.1  1992/12/28  18:33:24  richardg
 * Initial revision
 *
 * Revision 1.5  1992/07/13  20:47:45  root
 * sdpr 140 Added WRK sharp defines - bem
 *
 * Revision 1.4  1992/05/26  15:09:15  root
 * sdpr u-120 - Changed several defines as result of user validation testing.
 *
 * Revision 1.3  1992/04/29  12:46:04  root
 * sdpr u-109 Changed sharp define W9R0 from row 20 to row 19 - bem
 *
 * Revision 1.2  1992/03/24  04:22:42  bchampag
 * Increased MAX_LINE_LENGTH to 78
 *
 * Revision 1.1  1992/03/18  19:40:33  root
 * Initial revision
 *
 */

#ifndef _ACURSES_H
#define _ACURSES_H

#include "acedefs.h"

#define	MAX_WINDOWS	5
#define MAX_LINE_LENGTH	78
#define BLANK		99
#define	ON		1
#define OFF		0

#define W1LINES		12
#define	W2LINES		( MAX_SCSI_ID + 1 ) * MAX_CHANNEL
#ifdef PARAGON860
#define	W3LINES		13
#else
#define	W3LINES		12
#endif
#define	W4LINES		1
#define W5LINES         6
#define	W6LINES		1
#define	W7LINES		16
#define	W8LINES		1
#define	W9LINES		6

#ifdef ASCII
#define	HORZL		( u_char ) '-'		/* Horizontal line */
#define	VERTL		( u_char ) '|'		/* Vertical line */
#define	ULC		( u_char ) '+'		/* Upper left corner */
#define	URC		( u_char ) '+'		/* Upper right corner */
#define	BLC		( u_char ) '+'		/* Bottom left corner */
#define	BRC		( u_char ) '+'		/* Bottom right corner */
#define LCON		( u_char ) '+'		/* Left connect */
#define RCON		( u_char ) '+'		/* Right connect */

#else

#define	HORZL		( u_char ) 196		/* Horizontal line */
#define	VERTL		( u_char ) 179		/* Vertical line */
#define	ULC		( u_char ) 218		/* Upper left corner */
#define	URC		( u_char ) 191		/* Upper right corner */
#define	BLC		( u_char ) 192		/* Bottom left corner */
#define	BRC		( u_char ) 217		/* Bottom right corner */
#define LCON		( u_char ) 180		/* Left connect */
#define RCON		( u_char ) 195		/* Right connect */
#endif

/*===========================================================================*/
/*                      Window Constant Defines                              */
/*===========================================================================*/

#ifdef ASCII
#define MAXROW		24
#define HIDECUR		23, 79
#else
#define MAXROW		25
#define HIDECUR		24, 79
#endif
#define	HORZ1		12
#define HORZ2		13
#define	HORZ3		16
#define HORZ4		17
#define VERT1		34

#define MR0		0
#define MR1		MR0 + 1
#define MR2		MR1 + 1
#define MR3		MR2 + 1
#define MR4		MR3 + 1
#define MR5		MR4 + 1
#define MR6		MR5 + 1
#define MR7		MR6 + 1
#define MR8		MR7 + 1
#define MR9		MR8 + 1
#define MR10		MR9 + 1
#define MR11		MR10 + 1
#define MR12		MR11 + 1
#define MR13		MR12 + 1
#define MR14		MR13 + 1
#define MR15		MR14 + 1
#define MR16		MR15 + 1
#define MR17		MR16 + 1
#define MR18		MR17 + 1
#define MR19		MR18 + 1
#define MR20		MR19 + 1
#define MR21		MR20 + 1
#define MR22		MR21 + 1
#define MR23		MR22 + 1
#define MR24		MR23 + 1
#define MCOL		57
#define MPCOL		42
#define MSCOL		4

#define	W1R0		0
#define W1R1		W1R0 + 1
#define W1R2		W1R1 + 1
#define W1R3		W1R2 + 1
#define W1R4		W1R3 + 1
#define W1R5		W1R4 + 1
#define W1R6		W1R5 + 1
#define W1R7		W1R6 + 1
#define W1R8		W1R7 + 1
#define W1R9		W1R8 + 1
#define W1R10		W1R9 + 1
#define W1R11		W1R10 + 1
#define W1COL		9

#define W2R0		4
#define W2COL		11

#ifdef PARAGON860
#define	W3R0		12
#else
#define	W3R0		13
#endif
#define W3R1		W3R0 + 1
#define W3R2		W3R1 + 1
#define W3R3		W3R2 + 1
#define W3R4		W3R3 + 1
#define W3R5		W3R4 + 1
#define W3R6		W3R5 + 1
#define W3R7		W3R6 + 1
#define W3R8		W3R7 + 1
#define W3R9		W3R8 + 1
#define W3R10		W3R9 + 1
#define W3R11		W3R10 + 1
#define W3COL		0

#define W4R0		12
#define W4COL		10

#ifdef PARAGON860
#define	W5R0		13
#else
#define	W5R0		14
#endif
#define W5R1		W5R0 + 1
#define W5R2		W5R1 + 1
#define W5R3		W5R2 + 1
#define W5R4		W5R3 + 1
#define W5R5		W5R4 + 1
#define W5R6		W5R5 + 1
#define W5R7		W5R6 + 1
#ifdef PARAGON860
#define W5R8		W5R7 + 1
#define W5R9		W5R8 + 1
#else
#endif
#define W5COL		18

#define	W6R0		0
#define W6COL		36

#define	W7R0		1
#define W7R1		W7R0 + 1
#define W7R2		W7R1 + 1
#define W7R3		W7R2 + 1
#define W7R4		W7R3 + 1
#define W7R5		W7R4 + 1
#define W7R6		W7R5 + 1
#define W7R7		W7R6 + 1
#define W7R8		W7R7 + 1
#define W7R9		W7R8 + 1
#define W7R10		W7R9 + 1
#define W7R11		W7R10 + 1
#define W7R12		W7R11 + 1
#define W7R13		W7R12 + 1
#define W7R14		W7R13 + 1
#define W7R15		W7R14 + 1
#define W7R16		W7R15 + 1
#define W7COL		36

#define	W8R0		17
#define W8COL		44

#ifdef PARAGON860
#define	W9R0		18		/* sdpr u-109   was row 20 */
#else
#define	W9R0		19		/* sdpr u-109   was row 20 */
#endif
#define W9R1		W9R0 + 1
#define W9R2		W9R1 + 1
#define W9R3		W9R2 + 1
#define W9COL		42

#define DRVROW		4
#define DRVCOL		11
#define WRKROW		23
#define WRKCOL		21

#define	WLUNSTAT_0	"optimal     "
#define WLUNSTAT_1	"degraded    "
#define WLUNSTAT_2	"reconstruct "
#define WLUNSTAT_4	"dead        "
#define	WUNKNOWN	"unknown"
/*===========================================================================*/
/*                      Screen Control Structures                            */
/*===========================================================================*/

typedef struct line_ent {
	int	row;
	int	col;
	int	bold;
	int	grp;
	int	lun;
	int	status;
	char	data[MAX_LINE_LENGTH];
} line_ent_t;

#endif 
