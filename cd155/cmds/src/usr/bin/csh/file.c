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
static char rcsid[] = "@(#)$RCSfile: file.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:21:30 $";
#endif
/*
 * COMPONENT_NAME: CMDCSH  c shell(csh)
 *
 * FUNCTIONS: setup_tty, back_to_col_1, pushback, catn, copyn, filetype, 
 *            print_by_column, tilde, retype, beep, print_recognized_stuff, 
 *            extract_dir_and_name, getentry, free_items, search, recognize, 
 *	      is_prefix, is_suffix, tenex, ignored, sortscmp
 *
 * ORIGINS: 10,26,27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * (Copyright statements and/or associated legends of other
 * companies whose code appears in any part of this module must
 * be copied here.)
 */

/*
 * IBM CONFIDENTIAL
 * Copyright International Business Machines Corp. 1989
 * Unpublished Work
 * All Rights Reserved
 * Licensed Material - Property of IBM
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 

/*
#ifndef lint
static uchar_t *sccsid = "@(#)sh.file.c	5.6 (Berkeley) 5/18/86";
#endif
*/

#ifdef FILEC
/*
 * Tenex style file name recognition, .. and more.
 * History:
 *	Author: Ken Greer, Sept. 1975, CMU.
 *	Finally got around to adding to the Cshell., Ken Greer, Dec. 1981.
 */

#include "sh.h"
#include <dirent.h>
#include <pwd.h>
#include <string.h>

#define TRUE	1
#define FALSE	0
#define ON	1
#define OFF	0

#define ESC	'\033'

typedef enum {LIST, RECOGNIZE} COMMAND;

static recognize();
static is_prefix();
static ignored();
int  sortscmp();


/*
 * Put this here so the binary can be patched with adb to enable file
 * completion by default.  Filec controls completion, nobeep controls
 * ringing the terminal bell on incomplete expansions.
 */

/* bool filec = 0; */

static
setup_tty(on)
	int on;
{
        struct sgttyb sgtty;
        static struct tchars tchars;    /* INT, QUIT, XON, XOFF, EOF, BRK */

	if (on) {
                (void) ioctl(SHIN, TIOCGETC, (char *)&tchars);
                tchars.t_brkc = ESC;
                (void) ioctl(SHIN, TIOCSETC, (char *)&tchars);
                /*
                 * This must be done after every command: if
                 * the tty gets into raw or cbreak mode the user
                 * can't even type 'reset'.
                 */
                (void) ioctl(SHIN, TIOCGETP, (char *)&sgtty);
                if (sgtty.sg_flags & (RAW|CBREAK)) {
                         sgtty.sg_flags &= ~(RAW|CBREAK);
                         (void) ioctl(SHIN, TIOCSETP, (char *)&sgtty);
                }
	} else {
                tchars.t_brkc = -1;
                (void) ioctl(SHIN, TIOCSETC, (char *)&tchars);
	}
}

/*
 * Move back to beginning of current line
 */
static
back_to_col_1()
{
        struct sgttyb tty, tty_normal;
	int omask;

	omask = sigblock(sigmask(SIGINT));
        (void) ioctl(SHIN, TIOCGETP, (char *)&tty);
        tty_normal = tty;
        tty.sg_flags &= ~CRMOD;
        (void) ioctl(SHIN, TIOCSETN, (char *)&tty);
	(void) write(SHOUT, "\r", 1);
        (void )ioctl(SHIN, TIOCSETN, (char *)&tty_normal);
	(void) sigsetmask(omask);
}

/*
 * Push string contents back into tty queue
 */
static
pushback(string)
        uchar_t *string;
{
        register uchar_t *p;
        struct sgttyb tty, tty_normal;
        int omask;

        omask = sigblock(sigmask(SIGINT));
        (void) ioctl(SHOUT, TIOCGETP, (char *)&tty);
        tty_normal = tty;
        tty.sg_flags &= ~ECHO;
        (void) ioctl(SHOUT, TIOCSETN, (char *)&tty);

        for (p = string; *p; p++)
                (void) ioctl(SHOUT, TIOCSTI, p);
        (void) ioctl(SHOUT, TIOCSETN, (char *)&tty_normal);
        (void) sigsetmask(omask);
}



/*
 * Concatenate src onto tail of des.
 * Des is a string whose maximum length is count.
 * Always null terminate.
 */
static
catn(des, src, count)
	register uchar_t *des, *src;
	register count;
{

	while (--count >= 0 && *des)
		des++;
	while (--count >= 0)
		if ((*des++ = *src++) == 0)
			 return;
	*des = '\0';
}

/*
 * Like strncpy but always leave room for trailing \0
 * and always null terminate.
 */
static
copyn(des, src, count)
	register uchar_t *des, *src;
	register count;
{

	while (--count >= 0)
		if ((*des++ = *src++) == 0)
			return;
	*des = '\0';
}

static uchar_t
filetype(dir, file)
	uchar_t *dir, *file;
{
	uchar_t path[PATH_MAX];
	struct stat statb;

	catn((uchar_t *)strcpy((char *)path, (char *)dir), file, sizeof path);
	if (lstat((char *)path, &statb) == 0) {
		switch(statb.st_mode & S_IFMT) {
		    case S_IFDIR:
			return ('/');

		    case S_IFLNK:
			if (stat((char *)path, &statb) == 0 && /* follow it */
			   (statb.st_mode & S_IFMT) == S_IFDIR)
				return ('>');
			else
				return ('@');

		    case S_IFSOCK:
			return ('=');

		    default:
			if (statb.st_mode & 0111)
				return ('*');
		}
	}
	return (' ');
}

static struct winsize win;

/*
 * Print sorted down columns
 */
static
print_by_column(dir, items, count)
	uchar_t *dir, *items[];
{
	register int i, rows, r, c, maxwidth = 0, columns;

	if (ioctl(SHOUT, TIOCGWINSZ, (char *)&win) < 0 || win.ws_col == 0)
		win.ws_col = 80;
	for (i = 0; i < count; i++)
		maxwidth = maxwidth > (r = strlen((char *)items[i])) ? maxwidth : r;
	maxwidth += 2;			/* for the file tag and space */
	columns = win.ws_col / maxwidth;
	if (columns == 0)
		columns = 1;
	rows = (count + (columns - 1)) / columns;
	for (r = 0; r < rows; r++) {
		for (c = 0; c < columns; c++) {
			i = c * rows + r;
			if (i < count) {
				register int w;

				printf("%s", items[i]);
				putchar(dir ? filetype(dir, items[i]) : ' ');
				if (c < columns - 1) {	/* last column? */
					w = strlen((char *)items[i]) + 1;
					for (; w < maxwidth; w++)
						putchar(' ');
				}
			}
		}
		putchar('\n');
	}
}

/*
 * Expand file name with possible tilde usage
 *	~person/mumble
 * expands to
 *	home_directory_of_person/mumble
 */
static uchar_t *
tilde(new, old)
	uchar_t *new, *old;
{
	register uchar_t *o, *p;
	register struct passwd *pw;
	static uchar_t person[MAX_LOG_NAMLEN];

	if (old[0] != '~')
		return ((uchar_t *)strcpy((char *)new, (char *)old));

	for (p = person, o = &old[1]; *o && *o != '/'; *p++ = *o++)
		;
	*p = '\0';
	if (person[0] == '\0')
		(void) strcpy((char *)new, (char *)value("home"));
	else {
		pw = getpwnam((char *)person);
		if (pw == NULL)
			return (NULL);
		(void) strcpy((char *)new, (char *)pw->pw_dir);
	}
	(void) strcat((char *)new, (char *)o);
	return (new);
}

/*
 * Cause pending line to be printed
 */
static
retype()
{
        int pending_input = LPENDIN;

        (void) ioctl(SHOUT, TIOCLBIS, (char *)&pending_input);
}

static
beep()
{

	if (adrof("nobeep") == 0)
		(void) write(SHOUT, "\007", 1);
}

/*
 * Erase that silly ^[ and
 * print the recognized part of the string
 */
static
print_recognized_stuff(recognized_part)
	uchar_t *recognized_part;
{

	/* An optimized erasing of that silly ^[ */
	switch (strlen((char *)recognized_part)) {

	case 0:				/* erase two character: ^[ */
		printf("\b\b  \b\b");
		break;

	case 1:				/* overstrike the ^, erase the [ */
		printf("\b\b%s \b", recognized_part);
		break;

	default:			/* overstrike both character ^[ */
		printf("\b\b%s", recognized_part);
		break;
	}
	flush();
}

/*
 * Parse full path in file into 2 parts: directory and file names
 * Should leave final slash (/) at end of dir.
 */
static
extract_dir_and_name(path, dir, name)
	uchar_t *path, *dir, *name;
{
	register uchar_t  *p;

	p = (uchar_t *)rindex((char *)path, '/');
	if (p == NULL) {
		copyn(name, path, PATH_MAX);
		dir[0] = '\0';
	} else {
		copyn(name, ++p, PATH_MAX);
		copyn(dir, path, p - path);
	}
}

static uchar_t *
getentry(dir_fd, looking_for_lognames)
	DIR *dir_fd;
{
	register struct passwd *pw;
	register struct direct *dirp;

	if (looking_for_lognames) {
		if ((pw = getpwent()) == NULL)
			return (NULL);
		return ((uchar_t *)pw->pw_name);
	}
	if (dirp = readdir(dir_fd))
		return ((uchar_t *)dirp->d_name);
	return (NULL);
}

static
free_items(items)
	register uchar_t **items;
{
	register int i;

	for (i = 0; items[i]; i++)
		free(items[i]);
	free((char *)items);
}

#define FREE_ITEMS(items) { \
	int omask;\
\
	omask = sigblock(sigmask(SIGINT));\
	free_items(items);\
	items = NULL;\
	(void) sigsetmask(omask);\
}

/*
 * Perform a RECOGNIZE or LIST command on string "word".
 */
static
tenex_search(word, command, max_word_length)
	uchar_t *word;
	COMMAND command;
{
	static uchar_t **items = NULL;
	register DIR *dir_fd;
	register numitems = 0, ignoring = TRUE, nignored = 0;
	register name_length, looking_for_lognames;
	uchar_t tilded_dir[PATH_MAX + 1], dir[PATH_MAX + 1];
	uchar_t name[PATH_MAX + 1], extended_name[PATH_MAX+1];
	uchar_t *entry;
	extern int NLstrcmp();
	uchar_t *org_word;
#define MAXITEMS 1024

	if (items != NULL)
		FREE_ITEMS(items);
        org_word = word;
        if (index ((char *)word,'$') != NULL) {
                uchar_t    *p;

                if ((p = (uchar_t *)rindex((char *)word, '/')) == NULL || index((char *)p, '$') != NULL)
                        return (0);     /* leave if searching only $foo */
                word = Dfix1(word);
        }

	looking_for_lognames = (*word == '~') && (index((char *)word, '/') == NULL);
	if (looking_for_lognames) {
		(void) setpwent();
		copyn(name, &word[1], PATH_MAX);	/* name sans ~ */
	} else {
		extract_dir_and_name(word, dir, name);
		if (tilde(tilded_dir, dir) == 0)
			return (0);
		dir_fd = opendir(*tilded_dir ? (char *)tilded_dir : ".");
		if (dir_fd == NULL)
			return (0);
	}

again:	/* search for matches */
	name_length = strlen((char *)name);
	for (numitems = 0; entry = getentry(dir_fd, looking_for_lognames); ) {
		if (!is_prefix(name, entry))
			continue;
		/* Don't match . files on null prefix match */
		if (name_length == 0 && entry[0] == '.' &&
		    !looking_for_lognames)
			continue;
		if (command == LIST) {
			if (numitems >= MAXITEMS) {
				if(looking_for_lognames)
					printf (MSGSTR(YIKES1,"\nYikes!! Too many names in password file!!\n"));
				else
					printf (MSGSTR(YIKES2,"\nYikes!! Too many files!!\n"));
				break;
			}
			if (items == NULL)
				items = (uchar_t **) calloc(sizeof (items[1]),
				    MAXITEMS);
			items[numitems] = calloc(1,(unsigned)strlen((char *)entry)+1);
			copyn(items[numitems], entry, PATH_MAX);
			numitems++;
		} else {			/* RECOGNIZE command */
			if (ignoring && ignored(entry))
				nignored++;
			else if (recognize(extended_name,
			    entry, name_length, ++numitems))
				break;
		}
	}
	if (ignoring && numitems == 0 && nignored > 0) {
		ignoring = FALSE;
		nignored = 0;
		if (looking_for_lognames)
			(void) setpwent();
		else
			rewinddir(dir_fd);
		goto again;
	}

	if (looking_for_lognames)
		(void) endpwent();
	else
		closedir(dir_fd);
	if (numitems == 0)
		return (0);
	if (command == RECOGNIZE) {
		/* add extended name */
                catn(org_word, extended_name+name_length, max_word_length);
/*
		catn(word, extended_name, max_word_length);
*/
		return (numitems);
	}
	else { 				/* LIST */
		qsort((char *)items, numitems, sizeof(items[1]), sortscmp);
		print_by_column(looking_for_lognames ? NULL : tilded_dir,
		    items, numitems);
		if (items != NULL)
			FREE_ITEMS(items);
	}
	return (0);
}

/*
 * Object: extend what user typed up to an ambiguity.
 * Algorithm:
 * On first match, copy full entry (assume it'll be the only match) 
 * On subsequent matches, shorten extended_name to the first
 * character mismatch between extended_name and entry.
 * If we shorten it back to the prefix length, stop searching.
 */
static
recognize(extended_name, entry, name_length, numitems)
	uchar_t *extended_name, *entry;
{

	if (numitems == 1)			/* 1st match */
		copyn(extended_name, entry, PATH_MAX);
	else {					/* 2nd & subsequent matches */
		register uchar_t *x, *ent;
		register int len = 0;

		x = extended_name;
		for (ent = entry; *x && *x == *ent++; x++, len++)
			;
		*x = '\0';			/* Shorten at 1st uchar_t diff */
		if (len == name_length)		/* Ambiguous to prefix? */
			return (-1);		/* So stop now and save time */
	}
	return (0);
}

/*
 * Return true if check matches initial uchar_ts in template.
 * This differs from PWB imatch in that if check is null
 * it matches anything.
 */
static
is_prefix(check, template)
	register uchar_t *check, *template;
{

	do
		if (*check == 0)
			return (TRUE);
	while (*check++ == *template++);
	return (FALSE);
}

/*
 *  Return true if the uchar_ts in template appear at the
 *  end of check, I.e., are it's suffix.
 */
static
is_suffix(check, template)
	uchar_t *check, *template;
{
	register uchar_t *c, *t;

	for (c = check; *c++;)
		;
	for (t = template; *t++;)
		;
	for (;;) {
		if (t == template)
			return 1;
		if (c == check || *--t != *--c)
			return 0;
	}
}

tenex(inputline, inputline_size)
	uchar_t *inputline;
	int inputline_size;
{
	register int numitems, num_read;
	setup_tty(ON);
	while ((num_read = read(SHIN, inputline, inputline_size)) > 0) {
		static uchar_t delims[] = {  /* WORD DELIMITERS */
			' ',		/* space 		*/
			'\'',		/* apostrophe		*/
			'"',		/* quote		*/
			'\t',		/* tab			*/
			';',		/* semicolon		*/
			'&',		/* ampersand		*/
			'<',		/* left angle		*/
			'>',		/* right angle		*/
			'(',		/* left paren		*/
			')',		/* right paren		*/
			'|',		/* vertical bar		*/
			'^',		/* circumflex		*/
			'%',		/* percent		*/
#ifdef KJI
			0x81, 0x40,	/* KJI space		*/
#endif
			'\0'
		};
		register uchar_t *str_end, *word_start, *last_char, should_retype;
		register int space_left;
		COMMAND command;

		last_char = inputline;
		while (last_char + NLchrlen(last_char) < inputline + num_read)
			last_char += NLchrlen(last_char);
		if (*last_char == '\n' || num_read == inputline_size)
			break;
		command = (*last_char == ESC) ? RECOGNIZE : LIST;
		if (command == LIST)
			putchar('\n');
		str_end = &inputline[num_read];
		if (*last_char == ESC)
			--str_end;	/* wipeout trailing cmd uchar_t */
		*str_end = '\0';
		/*
		 * Find LAST occurence of a delimiter in the inputline.
		 * The word start is one character past it.
		 */
		for(word_start = last_char = inputline; 
                    last_char < str_end; 
                    last_char += NLchrlen(last_char)) 
			if (NLstrchr(delims, *last_char))
			{
				word_start = last_char;
				word_start += NLchrlen(word_start);
			}

		space_left = inputline_size - (word_start - inputline) - 1;
		numitems = tenex_search(word_start, command, space_left);

		if (command == RECOGNIZE) {
			/* print from str_end on */
			print_recognized_stuff(str_end);
			if (numitems != 1)	/* Beep = No match/ambiguous */
				beep();
		}

		/*
		 * Tabs in the input line cause trouble after a pushback.
		 * tty driver won't backspace over them because column
		 * positions are now incorrect. This is solved by retyping
		 * over current line.
		 */
		should_retype = FALSE;
		if (NLstrchr(inputline, '\t')) {  /* tab in input line */
			back_to_col_1();
			should_retype = TRUE;
		}
		if (command == LIST)	/* Always retype after a LIST */
			should_retype = TRUE;
		if (should_retype)
			printprompt();
		pushback(inputline);
		if (should_retype)
			retype();
	}
	setup_tty(OFF);
	return (num_read);
}

static
ignored(entry)
	register uchar_t *entry;
{
	struct varent *vp;
	register uchar_t **cp;

	if ((vp = adrof("fignore")) == NULL || (cp = vp->vec) == NULL)
		return (FALSE);
	for (; *cp != NULL; cp++)
		if (is_suffix(entry, *cp))
			return (TRUE);
	return (FALSE);
}

/*
 * String compare for qsort.
 */
sortscmp(a1, a2)
        uchar_t **a1, **a2;
{

         return (NLstrcmp(*a1, *a2));
}

#endif FILEC
