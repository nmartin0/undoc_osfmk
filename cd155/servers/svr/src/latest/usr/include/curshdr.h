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
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: curshdr.h,v $
 * Revision 1.3  1994/11/18  21:09:23  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:13:39  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:56:56  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:15:12  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:17:52  nandy
 * Initial revision
 *
 * Revision 1.4  92/04/13  14:34:53  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.4  90/10/07  20:26:38  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:44:12  gm]
 * 
 * Revision 1.3  90/04/27  23:06:24  devrcs
 * 	Removed NONSTANDARD ifdefs. Updated to March AIX drop.
 * 	[90/04/16  13:47:40  tom]
 * 
 * Revision 1.2  90/03/13  21:21:55  mbrown
 * 	AIX merge first cut - new file.
 * 	[90/02/12  18:08:23  tom]
 * 
 * $EndLog$
 */
/* @(#)curshdr.h        1.7  com/lib/curses,3.1,9013 12/14/89 10:58:18 */
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   curshdr.h
 *
 * ORIGINS: 3, 10, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1988
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
#ifndef _CURSHDR_H_
#define _CURSHDR_H_


/*
 * IC and IL overheads and costs should be set to this
 * value if the corresponding feature is missing
 */
#define INFINITY 500

struct line
{
	int     hash;		/* hash value for this line, 0 if not known */
	struct line *next;      /* pointer to the next line in list of lines*/
	short   bodylen;	/* the cost of redrawing this line */
	short   length;		/* the number of valid characters in line */
	chtype  *body;		/* the actual text of the line */
};

struct costs {
	short Cursor_address;
	short Cursor_home;
	short Carriage_return;
	short Tab;
	short Back_tab;
	short Cursor_left;
	short Cursor_right;	/* current cost, which is 1 unless in insert
				 * mode, since you can just output the
				 * character you want to move over */
	short Right_base;	/* actual cost to do cursor_right */
	short Cursor_down;
	short Cursor_up;
	short Parm_left_cursor;
	short Parm_right_cursor;
	short Parm_up_cursor;
	short Parm_down_cursor;
	short Column_address;
	short Row_address;
	unsigned ilvar;		/* Insert line varying part * 32 */
	int   ilfixed;		/* Insert line fixed overhead */
	unsigned dlvar;		/* Delete line varying part * 32 */
	int   dlfixed;		/* Delete line fixed overhead */
	unsigned icvar;		/* Insert char varying part * 32 */
	int   icfixed;		/* Insert char fixed overhead */
	unsigned dcvar;		/* Delete char varying part * 32 */
	int   dcfixed;		/* Delete char fixed overhead */
	/* Should have costs for delete char/delete line here too. */
};
#define _cost(field) (SP->term_costs.field)

struct map {
	char label[16];		/* Text the key is labelled with */
	char sends[16];		/* Characters sent when key is pressed */
	short keynum;		/* "char" we pass back to program */
};

struct screen {
	unsigned fl_nonl	: 1;	/* we shouldn't output \n */
	unsigned fl_uppercase	: 1;	/* upper case terminal */
	unsigned fl_normtty	: 1;	/* currently in normal tty mode */
	unsigned fl_my_term	: 1;	/* user names his terminal type */
	unsigned fl_echoit	: 1;	/* in software echo mode */
	unsigned fl_rawmode	: 1;	/* in raw or cbreak mode */
	unsigned fl_endwin	: 1;	/* has called endwin */
	unsigned phys_irm	: 1;	/* physically in insert char mode */
	unsigned virt_irm	: 1;	/* want to be in insert char mode */
	unsigned fl_nodelay	: 1;	/* tty is in nodelay mode */
	struct line **cur_body;	/* physical screen image */
	struct line **std_body;	/* desired screen image */
	struct line *freelist;	/* free space list */
	short phys_x;		/* physical cursor X position */
	short phys_y;		/* physical cursor Y position */
	short virt_x;		/* virtual cursor X position */
	short virt_y;		/* virtual cursor Y position */
	short phys_top_mgn;	/* physical top margin of scrolling region */
	short phys_bot_mgn;	/* physical bottom margin of scrolling region */
	short des_top_mgn;	/* desired top margin of scrolling region */
	short des_bot_mgn;      /* desired bottom margin of scrolling region*/
	chtype *curptr;		/* pointer to cursor */
	chtype virt_gr;		/* desired highlight state */
	chtype phys_gr;		/* current highlight state */
	short winsize;		/* no. of lines on which ID operations done */
	short doclear;		/* flag to say whether screen garbaged */
	short baud;		/* baud rate of this tty */
	short check_input;	/* flag for input check */
	short check_fd;		/* file descriptor for input check */
	struct _win_st *std_scr;/* primary output screen */
	struct _win_st *cur_scr;/* what's physically on the screen */
	struct term *tcap;	/* TERMCAP strings and storage space */
	struct costs term_costs;/* costs of various capabilities */
	short kp_state;		/* 1 iff keypad is on, else 0 */
	short ml_above;		/* # memory lines above top of screen */
	SGTTY	save_tty_buf;	/* current state of this tty */
	FILE *term_file;	/* File to write on for output. */
#ifdef		KEYPAD
	struct map *kp;		/* keypad map */
#endif		/* KEYPAD */
	short *input_queue;	/* a place to put stuff ungetc'ed */
	FILE *input_file;	/* Where to get keyboard input */
};

extern struct screen *SP;

#ifdef DEBUG
extern	FILE	*outf;
#endif

#endif /* _CURSHDR_H_ */
