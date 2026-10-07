/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: screen.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:30:47 $";
#endif
/*
 *	Copyright 1990, Eric Shienbrood
 *
 * This software may be freely copied, distributed, or modified, as long
 * this copyright notice is preserved.
 *
 *
 * screen.c : routines for performing operations on the terminal display
 */

#include <stdio.h>
#include <signal.h>
#include "globals.h"

/*
 * Erase the rest of the prompt, assuming we are starting at column col.
 */

erase (col)
register int col;
{
	register int linlen;

	linlen = line_width(cursor_line);
	line_width(cursor_line) = cursor_column = col;
	if (col >= linlen)
		return;
	if (hard) {
		putchar ('\n');
		line_width(cursor_line) = 0;
	}
	else {
		if (col == 0)
			putchar ('\r');
		if (!dumb && EraseLineStr)
			tputs (EraseLineStr, 1, putch);
		else
			for (col = linlen - col; col > 0; col--)
				putchar (' ');
	}
}

/*
 * Erase the current line entirely
 */

kill_line ()
{
	erase (0);
	if (!EraseLineStr || dumb)
		putchar ('\r');
}

/*
 * Go to a new line.  
 * If we are at the bottom, then a newline character
 * will cause us to scroll.
 */
newline()
{
	putchar('\n');
	nextline();
}

nextline()
{
	if (cursor_column > line_width(cursor_line))
		line_width(cursor_line) = cursor_column;
	cursor_column = 0;
	if (display_line < ScreenLength - 1)
		display_line++;
	if (cursor_line != screen_end) {
		if (cursor_line == ScreenLength - 1)
			cursor_line = 0;
		else
			cursor_line++;
	}
	else {
		if (screen_end == ScreenLength - 1)
			screen_end = 0;
		else
			screen_end++;
		cursor_line = screen_end;
		line_width(cursor_line) = 0;
		hidden_line.reschar = Screen[cursor_line].reschar;
		Screen[cursor_line].fline = -1;
	}
}

static struct screeninfo topline;

/*
 * Return the screeninfo structure of the first line on
 * the screen that displays a line from the input file.
 * If the state of the screen is unknown, and there is
 * a saved top line, then use that.
 */
struct screeninfo *
get_top_screen_line()
{
	register struct screeninfo *sp;
	struct screeninfo *startsp;

	if (screen_end == ScreenLength - 1)
		startsp = Screen;
	else
		startsp = &Screen[screen_end + 1];
	sp = startsp;
	while (sp->fline == -1) {
		sp++;
		if (sp == &Screen[ScreenLength])
			sp = Screen;
		/*
		 * If there are no known text lines
		 * on the screen, then retrieve the
		 * previously saved info for the top
		 * line.
		 */
		if (sp == startsp) {
			*startsp = topline;
			return(startsp);
		}
	}
	return(sp);
}

/*
 * Save enough screen state to allow redrawing after
 * the screen has been fouled, e.g., because a 
 * shell command has been run.  All we really need
 * to know is the state of the top screen line.
 */

save_screen_state()
{
	topline = *(get_top_screen_line());
}

/*
 * Scroll the screen backwards "nscroll" lines.
 * Returns -1 if no scroll is possible, 0 otherwise.
 */

reverse_scroll(nscroll)
int nscroll;
{
	register int i;
	register struct screeninfo *sp;
	int saved_display_line = display_line;
	int saved_column = cursor_column;
	int lines_occupied;
	int lines_to_show;
	int scrolled_so_far;
	int length;
	int reschar;
	int ocl;

	if (ScrollUpStr == NULL || HomeStr == NULL || nscroll == 0)
		return(-1);

	/* Get info for top screen line */

	if (screen_end == ScreenLength - 1)
		i = 0;
	else
		i = screen_end + 1;
	/* XXX FIX FOR fline == -1 */
	sp = &Screen[i];
	/*
	 * Don't attempt to scroll if there's
	 * junk at the top of the screen.
	 */
	if (sp->fline < 0 || (sp->fline == 0 && sp->fragnum == 0))
		return(-1);
	/*
	 * Find the file line just above the top one displayed
	 * on the screen.  Skip any lines marked as invisible.
	 */
	if (sp->fragnum > 0) {
		Currline = sp->fline;
		lines_occupied = sp->fragnum;
	}
	else {
		Currline = sp->fline - 1;
		while (Currline >= 0 && lineindex[Currline].width == -1)
			Currline--;
		if (Currline < 0)
			return;
		if (lineindex[Currline].width <= ScreenWidth)
			lines_occupied = 1;
		else
			lines_occupied = (lineindex[Currline].width-1)/ScreenWidth + 1;
	}
	/*
	 * lines_occupied is the number of screen lines occupied by
	 * this file line, if the line is entirely above the top of
	 * the screen.  Otherwise it is the number of lines the portion
	 * above the top of the screen would occupy.
	 */
	scrolled_so_far = 0;
	while (scrolled_so_far < nscroll) {
		goto_line(Currline);
		lines_to_show = lines_occupied;
		/*
		 * The current line may occupy more screen lines
		 * than we are supposed to scroll back.  Skip the
		 * portions we're not supposed to display.
		 */
		reschar = 0;
		if (lines_to_show > nscroll - scrolled_so_far) {
			lines_to_show = nscroll - scrolled_so_far;
			i = lines_occupied - lines_to_show;
			while (i-- > 0)
				(void)getline (&length, fold_opt);
		}
		hidden_line.reschar = Curresid;

		for (i = 0; i < lines_to_show; i++) {
			gohome();
			tputs(ScrollUpStr, 1, putch);
			screen_end--;
			/* The following works even if screen_end goes to -1 */
			cursor_line = screen_end + 1;
			line_width(cursor_line) = 0;
			if (screen_end == -1)
				screen_end += ScreenLength;
		}
		for (i = 0; i < lines_to_show; i++) {
			ocl = cursor_line;
			print_line_from_file();
		}
		scrolled_so_far += lines_to_show;
		/*
		 * First time through the loop we may have only displayed
		 * part of the top line.
		 */
		if (sp && sp->fragnum) {
			Currline--;
			sp = NULL;
		}
		else
			Currline -= 2;
		while (Currline >= 0 && lineindex[Currline].width == -1)
			Currline--;
		if (Currline < 0)
			break;
		if (lineindex[Currline].width <= ScreenWidth)
			lines_occupied = 1;
		else
			lines_occupied = (lineindex[Currline].width-1) / ScreenWidth + 1;
	}

	fflush(stdout);
	/*
	 * Calculate the new lowest line on the screen, and
	 * then derive cursor_line from that.
	 */
	display_line = saved_display_line + scrolled_so_far;
	if (display_line > ScreenLength - 1)
		display_line = ScreenLength - 1;
	cursor_line = screen_end - (ScreenLength - display_line - 1);
	if (cursor_line < 0)
		cursor_line += ScreenLength;
	cursor_column = saved_column;
	tputs(tgoto(CursorMotionStr, cursor_column, display_line), 1, putch);
	Currline = Screen[cursor_line].fline;
	Currfrag = Screen[cursor_line].fragnum;
	Fseek(Screen[cursor_line].seek_key);
	return(0);
}

/*
 * Go to home position
 */
gohome()
{
	tputs(HomeStr,1,putch);
	cursor_column = 0;
	display_line = 0;
	if (screen_end == ScreenLength - 1)
		cursor_line = 0;
	else
		cursor_line = screen_end + 1;
}

/*
 * force clear to end of line
 */
cleareol()
{
	tputs(EraseLineStr, 1, putch);
}

/*
 * clear to end of screen
 */
clreos()
{
	tputs(EodClrStr, 1, putch);
}

/*
 *  Clear the screen
 */
doclear()
{
	register int i;

	if (ClearScreenStr && !hard) {
		tputs(ClearScreenStr, 1, putch);
		/*
		 * Put out carriage return so that system doesn't
		 * get confused by escape sequences when expanding tabs
		 */
		cursor_column = 0;
		putchar ('\r');
		display_line = 0;
		if (screen_end == ScreenLength - 1)
			cursor_line = 0;
		else
			cursor_line = screen_end + 1;
		promptlen = 0;
		for (i = 0; i < ScreenLength; i++)
			line_width(i) = 0;
	}
}

/*
 * After the screen has been fouled, forget what we know
 * about the state of each line on the screen, by assuming
 * that each line occupies the full width of the screen.
 * This will force us to clear to the end of every line
 * after writing over it.
 */
forget_screen_state()
{
	register struct screeninfo *sp;

	for (sp = Screen; sp < &Screen[ScreenLength]; sp++) {
		sp->fline = -1;
		sp->width = ScreenWidth;
	}
	screen_end = cursor_line = ScreenLength - 1;
	display_line = ScreenLength - 1;	/* ???? initially unknown */
}

/*
 * Come here to redraw the screen or if a
 * signal for a window size change is received.
 */
void
redraw()
{
	register struct screeninfo *sp;
#ifdef TIOCGWINSZ
	struct winsize win;
	int oldscreenlength;
	int oldscreenwidth;
#endif

#ifdef TIOCGWINSZ
	(void) signal(SIGWINCH, SIG_IGN);
	oldscreenlength = ScreenLength;
	oldscreenwidth = ScreenWidth;
	save_screen_state();
	if (ioctl(fileno(stdout), TIOCGWINSZ, &win) != -1) {
		if (win.ws_row != 0) {
			ScreenLength = win.ws_row;
			lines_to_display = ScreenLength - 1;
			lines_to_scroll = lines_to_display / 2;
			if (lines_to_display <= 0)
				lines_to_display = lines_to_scroll = 1;
		}
		if (win.ws_col != 0)
			ScreenWidth = win.ws_col;
	}
	(void) signal(SIGWINCH, redraw);
	/*
	 * Realloc the screeninfo array if necessary
	 */
	if (ScreenLength != oldscreenlength) {
		extern char *realloc();

		Screen = (struct screeninfo *)
			realloc(Screen,ScreenLength*sizeof(struct screeninfo));
		if (Screen == NULL) {
			perror(MSGSTR(NOMEM, "Not enough memory for screeninfo array"));
			exit(1);
		}
		forget_screen_state();
	}
	else if (ScreenWidth != oldscreenwidth)
		forget_screen_state();
#endif /* TIOCGWINSZ */

	/* Now redraw the screen */
	doclear ();
	sp = get_top_screen_line();
	Currline = sp->fline;
	Currfrag = sp->fragnum;
	Curresid = hidden_line.reschar;
	Fseek(sp->seek_key);
	show_lines(lines_to_display, NULL);
}
