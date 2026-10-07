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
 
/* Copyright (C) 1988-1991 Free Software Foundation, Inc.
This file is part of GNU Make.

GNU Make is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Make is distributed "AS IS" in the hope that it will be useful,
but WITHOUT ANY WARRANTY of any kind; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Make; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/***************************************************************************
 *
 *    Copyright (c) 1988-1992 Intel Corporation
 *
 *    This file is part of pmake, an extension of GNU Make.
 *    Pmake is distributed by the Intel Corporation under the
 *    terms of the GNU General Public License.
 *
 *    Title:
 *        $Id: main.c,v 1.7 1994/11/19 01:33:53 mtm Exp $
 *
 *    Description:
 *        Main GNU make routine.
 *
 **************************************************************************/

#include "make.h"
#include "commands.h"
#include "dep.h"
#include "file.h"
#include "variable.h"
#include "job.h"
#include <time.h>

#ifdef PMAKE
extern char *cvs_version;
#else
extern char *version_string;
#endif

extern int fatal_error_signal ();

extern struct dep *read_all_makefiles ();

extern void print_variable_data_base ();
extern void print_dir_data_base ();
extern void print_rule_data_base ();
extern void print_file_data_base ();
extern void print_vpath_data_base ();

#if	!defined(__GNU_LIBRARY__) && !defined(POSIX)
extern int chdir ();
extern void exit ();
extern time_t time ();
extern double atof ();
#endif
extern char *mktemp ();

static void log_working_directory ();
static void print_data_base (), print_version ();
static void decode_switches (), decode_env_switches ();
static void define_makeflags ();


#if 0 /* dummy tag */
flags () {}
#endif
/* Flags:
 *	-b ignored for compatibility with System V Make
 *	-C change directory
 *	-d debug
 *	-e env_overrides
 *	-f makefile
 *	-i ignore_errors
 *	-j job_slots
 *	-k keep_going
 *	-l max_load_average
 *	-m ignored for compatibility with something or other
 *	-n just_print
 *	-o consider file old
 *	-p print_data_base
 *	-q question
 *	-r no_builtin_rules
 *	-s silent
 *	-S turn off -k
 *	-t touch
 *	-v print version information
 *	-w log working directory
 *	-W consider file new (with -n, `what' if effect)
 *
 * When built with -DPMAKE, the following flags are added/changed:
 *
 *	-c if a file does not exist, do not check it out from SCCS or RCS
 *      -F the absense of a Makefile/makefile is a fatal error
 *	-m search machine-specific subdirectories before cwd or vpath
 *	-N disable Makeconf processing
 *	-P run as a parallel application in a specified partition
 *	-u do not unlink files checked out with RCS or SCCS
 *	-U ignored for compatibility with other makes
 */

/* The structure that describes an accepted command switch.  */

struct command_switch
  {
    char c;		/* The switch character.  */
    enum		/* Type of the value.  */
      {
	flag,		/* Turn int flag on.  */
	flag_off,	/* Turn int flag off.  */
	string,		/* One string per switch, may be in the same word.  */
	positive_int,	/* A positive integer.  */
	floating,	/* A floating-point number (double).  */
	ignore		/* Ignored.  */
      } type;

    char *value_ptr;	/* Pointer to the value-holding variable.  */

    unsigned int env:1;		/* Can come from MAKEFLAGS.  */
    unsigned int toenv:1;	/* Should be put in MAKEFLAGS.  */
    unsigned int no_makefile:1;	/* Don't propagate when remaking makefiles.  */

    char *noarg_value;	/* Pointer to value used if no argument is given.  */
    char *default_value;/* Pointer to default value.  */
#ifdef PMAKE
    unsigned int *origin; /* Origin of the switch value. */
#endif
  };

/* The structure used to hold the list of strings given
   in command switches of a type that takes string arguments.  */

struct stringlist
  {
    char **list;	/* Nil-terminated list of strings.  */
    unsigned int idx;	/* Index into above.  */
    unsigned int max;	/* Number of pointers allocated.  */
  };


/* The recognized command switches.  */

/* Nonzero means do not print commands to be executed (-s).  */

int silent_flag;

/* Nonzero means just touch the files
   that would appear to need remaking (-t)  */

int touch_flag;

#ifdef PMAKE
/* Nonzero means try to check out files from SCCS or RCS archives. */

int source_control_flag = 1;

/* Nonzero means absence of Makefile/makefile is fatal error. */

int makefile_nesc_flag = 0;

/* Nonzero means search machine-specific subdirectories before cwd or vpath */

int machine_specific_flag = 0;

/* Nonzero means process Makeconf files. */

int makeconf_flag = 1;

/* Nonzero means unlink source files automatically retrieved from SCCS or RCS */

int unlink_source_flag = 1;

/* Partition name */

static struct stringlist *partitions = 0;

/* Partition to use if -P is given without a partition name */

char nx_dflt_part[] = "NX_DFLT_PART";
#endif

/* Nonzero means just print what commands would need to be executed,
   don't actually execute them (-n).  */

int just_print_flag;

/* Print debugging trace info (-d).  */

int debug_flag = 0;

/* Environment variables override makefile definitions.  */

int env_overrides = 0;

/* Nonzero means ignore status codes returned by commands
   executed to remake files.  Just treat them all as successful (-i).  */

int ignore_errors_flag = 0;

/* Nonzero means don't remake anything, just print the data base
   that results from reading the makefile (-p).  */

int print_data_base_flag = 0;

/* Nonzero means don't remake anything; just return a nonzero status
   if the specified targets are not up to date (-q).  */

int question_flag = 0;

/* Nonzero means do not use any of the builtin rules (-r).  */

int no_builtin_rules_flag = 0;

/* Nonzero means keep going even if remaking some file fails (-k).  */

int keep_going_flag;
int default_keep_going_flag = 0;

/* Nonzero means print working directory
   before starting and after finishing make. */

int print_directory_flag = 0;

/* Nonzero means print version information.  */

int print_version_flag = 0;

/* List of makefiles given with -f switches.  */

static struct stringlist *makefiles = 0;

/* Number of job slots (commands that can be run at once).  */

unsigned int job_slots = 1;
unsigned int default_job_slots = 1;

#ifdef PMAKE
/* Origin of number of job slots (e.g., command line, environment, default). */
unsigned int job_slots_origin = 0;
#endif

/* Value of job_slots that means no limit.  */

static unsigned int inf_jobs = 0;

/* Maximum load average at which multiple jobs will be run.
   Negative values mean unlimited, while zero means limit to
   zero load (which could be useful to start infinite jobs remotely
   but one at a time locally).  */

double max_load_average = -1.0;
double default_load_average = -1.0;

/* List of directories given with -c switches.  */

static struct stringlist *directories = 0;

/* List of include directories given with -I switches.  */

static struct stringlist *include_directories = 0;

/* List of files given with -o switches.  */

static struct stringlist *old_files = 0;

/* List of files given with -W switches.  */

static struct stringlist *new_files = 0;

/* The table of command switches.  */

#ifdef PMAKE
static struct command_switch switches[] =
  {
    { 'b', ignore, 0, 0, 0, 0, 0, 0, 0 },
    { 'c', flag_off, (char *) &source_control_flag, 1, 1, 0, 0, 0, 0 },
    { 'C', string, (char *) &directories, 0, 0, 0, 0, 0, 0 },
    { 'd', flag, (char *) &debug_flag, 1, 1, 0, 0, 0, 0 },
    { 'e', flag, (char *) &env_overrides, 1, 1, 0, 0, 0, 0 },
    { 'f', string, (char *) &makefiles, 0, 0, 0, 0, 0, 0 },
    { 'F', flag, (char *) &makefile_nesc_flag, 1, 1, 0, 0, 0, 0 },
    { 'i', flag, (char *) &ignore_errors_flag, 1, 1, 0, 0, 0, 0 },
    { 'I', string, (char *) &include_directories, 0, 0, 0, 0, 0, 0 },
    { 'j', positive_int, (char *) &job_slots, 1, 1, 0,
	(char *) &inf_jobs, (char *) &default_job_slots, &job_slots_origin },
    { 'k', flag, (char *) &keep_going_flag, 1, 1, 0,
	0, (char *) &default_keep_going_flag, 0 },
    { 'l', floating, (char *) &max_load_average, 1, 1, 0,
	(char *) &default_load_average, (char *) &default_load_average, 0 },
    { 'm', flag, (char *) &machine_specific_flag, 1, 1, 0, 0, 0, 0 },
    { 'n', flag, (char *) &just_print_flag, 1, 1, 1, 0, 0, 0 },
    { 'N', flag_off, (char *) &makeconf_flag, 1, 1, 0, 0, 0, 0 },
    { 'o', string, (char *) &old_files, 0, 0, 0, 0, 0, 0 },
    { 'p', flag, (char *) &print_data_base_flag, 1, 1, 0, 0, 0, 0 },
    { 'P', string, (char *) &partitions, 0, 0, 0, nx_dflt_part, 0, 0 },
    { 'q', flag, (char *) &question_flag, 1, 1, 1, 0, 0, 0 },
    { 'r', flag, (char *) &no_builtin_rules_flag, 1, 1, 0, 0, 0, 0 },
    { 's', flag, (char *) &silent_flag, 1, 1, 0, 0, 0, 0 },
    { 'S', flag_off, (char *) &keep_going_flag, 1, 1, 0,
	0, (char *) &default_keep_going_flag, 0 },
    { 't', flag, (char *) &touch_flag, 1, 1, 1, 0, 0, 0 },
    { 'u', flag_off, (char *) &unlink_source_flag, 1, 1, 0, 0, 0, 0 },
    { 'U', ignore, 0, 0, 0, 0, 0, 0, 0 },
    { 'v', flag, (char *) &print_version_flag, 0, 0, 0, 0, 0, 0 },
    { 'w', flag, (char *) &print_directory_flag, 1, 1, 0, 0, 0, 0 },
    { 'W', string, (char *) &new_files, 0, 0, 0, 0, 0, 0 },
    { '\0', ignore, 0, 0, 0, 0, 0, 0 }
  };
#else
static struct command_switch switches[] =
  {
    { 'b', ignore, 0, 0, 0, 0, 0, 0 },
    { 'C', string, (char *) &directories, 0, 0, 0, 0, 0 },
    { 'd', flag, (char *) &debug_flag, 1, 1, 0, 0, 0 },
    { 'e', flag, (char *) &env_overrides, 1, 1, 0, 0, 0 },
    { 'f', string, (char *) &makefiles, 0, 0, 0, 0, 0 },
    { 'i', flag, (char *) &ignore_errors_flag, 1, 1, 0, 0, 0 },
    { 'I', string, (char *) &include_directories, 0, 0, 0, 0, 0 },
    { 'j', positive_int, (char *) &job_slots, 1, 1, 0,
	(char *) &inf_jobs, (char *) &default_job_slots },
    { 'k', flag, (char *) &keep_going_flag, 1, 1, 0,
	0, (char *) &default_keep_going_flag },
    { 'l', floating, (char *) &max_load_average, 1, 1, 0,
	(char *) &default_load_average, (char *) &default_load_average },
    { 'm', ignore, 0, 0, 0, 0, 0, 0 },
    { 'n', flag, (char *) &just_print_flag, 1, 1, 1, 0, 0 },
    { 'o', string, (char *) &old_files, 0, 0, 0, 0, 0 },
    { 'p', flag, (char *) &print_data_base_flag, 1, 1, 0, 0, 0 },
    { 'q', flag, (char *) &question_flag, 1, 1, 1, 0, 0 },
    { 'r', flag, (char *) &no_builtin_rules_flag, 1, 1, 0, 0, 0 },
    { 's', flag, (char *) &silent_flag, 1, 1, 0, 0, 0 },
    { 'S', flag_off, (char *) &keep_going_flag, 1, 1, 0,
	0, (char *) &default_keep_going_flag },
    { 't', flag, (char *) &touch_flag, 1, 1, 1, 0, 0 },
    { 'v', flag, (char *) &print_version_flag, 0, 0, 0, 0, 0 },
    { 'w', flag, (char *) &print_directory_flag, 1, 1, 0, 0, 0 },
    { 'W', string, (char *) &new_files, 0, 0, 0, 0, 0 },
    { '\0', ignore, 0, 0, 0, 0, 0 }
  };
#endif

/* List of non-switch arguments.  */

struct stringlist *other_args = 0;


/* The name we were invoked with.  */

char *program;

/* Value of the MAKELEVEL variable at startup (or 0).  */

unsigned int makelevel;

/* First file defined in the makefile whose name does not
   start with `.'.  This is the default to remake if the
   command line does not specify.  */

struct file *default_goal_file;

/* Pointer to structure for the file .DEFAULT
   whose commands are used for any file that has none of its own.
   This is zero if the makefiles do not define .DEFAULT.  */

struct file *default_file;

/* Mask of signals that are being caught with fatal_error_signal.  */
#ifdef POSIX
sigset_t fatal_signal_mask;
sigset_t fatal_and_child_signal_mask;
#else
int fatal_signal_mask;
#endif

#ifdef PMAKE
/* Location of object directory */

char *objectdir = 0;

/* Flag to indicate whether we are already in the separate OBJECTDIR. */

int  in_objectdir = 0;

/* Location of makeconf file to read */

char *makeconf = 0;

/* Machine-specific subdirectory */

char *machinedir = 0;

/* .EXPORT file */

struct file *export_file;

/* Current directory */

PATH_VAR (currentdir);

#endif


int
main (argc, argv, envp)
     int argc;
     char **argv;
     char **envp;
{
#if	(defined(USG) && !defined(HAVE_SIGLIST)) || defined(DGUX)
  extern void init_siglist ();
#endif
  extern int child_handler ();
  register struct file *f;
  register unsigned int i;
  register char *cmd_defs;
  register unsigned int cmd_defs_len, cmd_defs_idx;
  char **p;
  time_t now;
  struct dep *goals = 0;
  register struct dep *lastgoal;
  struct dep *read_makefiles;
  PATH_VAR (starting_directory);

  default_goal_file = 0;
  reading_filename = 0;
  reading_lineno_ptr = 0;

#ifdef TEST
{
int i;
printf("===========================================================\n");
printf("PMAKE");
for (i=1; i<argc; i++) printf(" %s", argv[i]);
printf("\n");
}
#endif

  /* Set up signal handling */
  
#if	(defined(USG) && !defined(HAVE_SIGLIST)) || defined(DGUX)
  init_siglist ();
#endif

#ifdef POSIX
  sigemptyset (&fatal_signal_mask);
  sigemptyset (&fatal_and_child_signal_mask);
  sigaddset (&fatal_and_child_signal_mask, SIGCHLD);
#else
  fatal_signal_mask = 0;
#endif

#ifdef POSIX
#define	FATAL_SIG(sig)							      \
  if (SIGNAL ((sig), (SIGHANDLER) fatal_error_signal)			      \
      == (SIGHANDLER) SIG_IGN)						      \
    (void) SIGNAL ((sig), SIG_IGN);					      \
  else {								      \
    sigaddset (&fatal_signal_mask, sig);				      \
    sigaddset (&fatal_and_child_signal_mask, sig);			      \
  }
#else
#define	FATAL_SIG(sig)							      \
  if (SIGNAL ((sig), (SIGHANDLER) fatal_error_signal)			      \
      == (SIGHANDLER) SIG_IGN)						      \
    (void) SIGNAL ((sig), SIG_IGN);					      \
  else									      \
    fatal_signal_mask |= sigmask (sig);
#endif

  FATAL_SIG (SIGHUP);
  FATAL_SIG (SIGQUIT);
  FATAL_SIG (SIGINT);
  FATAL_SIG (SIGILL);
  FATAL_SIG (SIGTRAP);
  FATAL_SIG (SIGIOT);
#ifdef	SIGEMT
  FATAL_SIG (SIGEMT);
#endif
#ifdef	SIGDANGER
  FATAL_SIG (SIGDANGER);
#endif
  FATAL_SIG (SIGFPE);
  FATAL_SIG (SIGBUS);
  FATAL_SIG (SIGSEGV);
  FATAL_SIG (SIGSYS);
  FATAL_SIG (SIGTERM);
#ifdef SIGXCPU
  FATAL_SIG (SIGXCPU);
#endif
#ifdef SIGXFSZ
  FATAL_SIG (SIGXFSZ);
#endif

#undef	FATAL_SIG

  /* Make sure stdout is line-buffered.  */

#if	(defined (USGr3) || defined (HPUX) || defined (hpux) \
	 || defined (M_XENIX) || defined (APOLLO))
  setvbuf (stdout, (char *) 0, _IOLBF, BUFSIZ);
#else	/* Not USGr3 and not HPUX et al.  */
#ifdef	USG
#ifdef	sgi
  setvbuf (stdout, _IOLBF, gmalloc (BUFSIZ), BUFSIZ);
#else
  setvbuf (stdout, _IOLBF, (char *) 0, BUFSIZ);
#endif
#else	/* Not USG.  */
  setlinebuf (stdout);
#endif	/* USG.  */
#endif	/* USGr3.  */

  /* Set up to access user data (files).  */
  user_access ();

  /* Figure out where this program lives.  */

  if (argv[0] == 0)
    argv[0] = "";
  if (argv[0][0] == '\0')
    program = "make";
  else 
    {
      program = rindex (argv[0], '/');
      if (program == 0)
	program = argv[0];
      else
	++program;
    }

  /* Figure out where we are.  */

#ifdef	USG
  /* In some System V's, `getcwd' spawns a child running /bin/pwd.  */
  push_signals_blocked_p (1);
#endif
  if (getwd (starting_directory) == 0)
    {
#ifdef	USG
      perror_with_name ("getcwd: ", "");
#else
      error ("getwd: %s", starting_directory);
#endif
      starting_directory[0] = '\0';
    }
#ifdef PMAKE
  /* Keep the cwd in a global so that we don't have to do so many getwds */
  strcpy (currentdir, starting_directory);
#endif

#ifdef	USG
  pop_signals_blocked_p ();
#endif

  /* Read in variables from the environment.  It is important that this be
     done before `MAKE' and `MAKEOVERRIDES' are figured out so their
     definitions will not be ones from the environment.  */

  for (i = 0; envp[i] != 0; ++i)
    {
      register char *ep = envp[i];
      while (*ep++ != '=')
	;
#ifdef PMAKE
      (void) define_variable (envp[i], ep - envp[i] - 1, ep, o_env, 1, 1);
#else
      (void) define_variable (envp[i], ep - envp[i] - 1, ep, o_env, 1);
#endif
    }
#ifdef PMAKE
    (void) define_variable ("CORE_ACTION_OTHER", 17, "CONT", o_env, 1 );
#endif

  /* Decode the switches.  */

  decode_switches (argc, argv);

  /* Print version information.  */

  if (print_version_flag || print_data_base_flag || debug_flag)
    print_version ();

  /* Search for command line arguments that define variables,
     and do the definitions.  Also save up the text of these
     arguments in CMD_DEFS so we can put them into the values
     of $(MAKEOVERRIDES) and $(MAKE).  */

  cmd_defs_len = 200;
  cmd_defs = (char *) gmalloc (cmd_defs_len);
  cmd_defs_idx = 0;

  for (i = 1; other_args->list[i] != 0; ++i)
    {
      if (other_args->list[i][0] == '\0')
	/* Ignore empty arguments, so they can't kill enter_file.  */
	continue;

      /* Try a variable definition.  */
      if (try_variable_definition (other_args->list[i], o_command))
	{
	  /* It succeeded.  The variable is already defined.
	     Backslash-quotify it and append it to CMD_DEFS, then clobber it
	     to 0 in the list so that it won't be taken for a goal target.  */
	  register char *p = other_args->list[i];
	  unsigned int l = strlen (p);
	  if (cmd_defs_idx + (l * 2) + 1 > cmd_defs_len)
	    {
	      if (l * 2 > cmd_defs_len)
		cmd_defs_len += l * 2;
	      else
		cmd_defs_len *= 2;
	      cmd_defs = (char *) xrealloc (cmd_defs, cmd_defs_len);
	    }
	  
	  while (*p != '\0')
	    {
	      if (index (";'\"*?[]$<>(){}|&~`\\ \t\r\n\f\v", *p) != 0)
		cmd_defs[cmd_defs_idx++] = '\\';
	      cmd_defs[cmd_defs_idx++] = *p++;
	    }
	  cmd_defs[cmd_defs_idx++] = ' ';
	}
      else
	{
	  /* It was not a variable definition, so it is a target to be made.
	     Enter it as a file and add it to the dep chain of goals.  */
	  f = enter_file (other_args->list[i]);
	  f->cmd_target = 1;
	  
	  if (goals == 0)
	    {
	      goals = (struct dep *) gmalloc (sizeof (struct dep));
	      lastgoal = goals;
	    }
	  else
	    {
	      lastgoal->next = (struct dep *) gmalloc (sizeof (struct dep));
	      lastgoal = lastgoal->next;
	    }
	  lastgoal->name = 0;
#ifdef PMAKE
          lastgoal->origname = 0;
#endif
	  lastgoal->file = f;
	}
    } /* for */

  if (cmd_defs_idx > 0)
    {
      cmd_defs[cmd_defs_idx - 1] = '\0';
#ifdef PMAKE
      (void) define_variable ("MAKEOVERRIDES", 13, cmd_defs, o_override, 0, 1);
#else
      (void) define_variable ("MAKEOVERRIDES", 13, cmd_defs, o_override, 0);
#endif
    }

  /* Set the "MAKE" variable to the name we were invoked with.
     (If it is a relative pathname with a slash, prepend our directory name
     so the result will run the same program regardless of the current dir.
     If it is a name with no slash, we can only hope that PATH did not
     find it in the current directory.)

     Append the command-line variable definitions gathered above
     so sub-makes will get them as command-line definitions.  */

  if (starting_directory[0] != '\0'
      && argv[0] != 0 && argv[0][0] != '/' && index (argv[0], '/') != 0)
    argv[0] = concat (starting_directory, "/", argv[0]);

  if (cmd_defs_idx > 0)
    {
      char *str = concat (argv[0], " ", cmd_defs);
#ifdef PMAKE
      (void) define_variable ("MAKE", 4, str, o_env, 0, 1);
#else
      (void) define_variable ("MAKE", 4, str, o_env, 0);
#endif
      free (str);
    }
  else
#ifdef PMAKE
    (void) define_variable ("MAKE", 4, argv[0], o_env, 0, 1);
#else
    (void) define_variable ("MAKE", 4, argv[0], o_env, 0);
#endif

  free (cmd_defs);

  /* If there were -c flags, move ourselves about.  */

  if (directories != 0) {
    for (i = 0; directories->list[i] != 0; ++i)
      {
	char *dir = directories->list[i];
	if (chdir (dir) < 0)
	  pfatal_with_name (dir);
      }
#ifdef PMAKE
    if (getwd (currentdir) == 0)
      {
        fatal ("getwd: %s", starting_directory);
      }
  }
#else
  }
#endif

  /* Figure out the level of recursion.  */
  {
    struct variable *v = lookup_variable ("MAKELEVEL", 9);
    if (v != 0 && *v->value != '\0' && *v->value != '-')
      makelevel = (unsigned int) atoi (v->value);
    else
      makelevel = 0;
  }

  /* Construct the list of include directories to search.  */

  construct_include_path (include_directories == 0 ? (char **) 0
			  : include_directories->list);

  /* Tell the user where he is.  */

  if (print_directory_flag)
    log_working_directory (1);

#ifdef PMAKE
  /* Figure out the path to the source directory here in case we are
     invoked recursively. */
  if (makeconf_flag)
    (void) set_makeconf_environment();
#endif

  /* Read any stdin makefiles into temporary files.  */

  if (makefiles != 0)
    {
      register unsigned int i;
      for (i = 0; i < makefiles->idx; ++i)
	if (makefiles->list[i][0] == '-' && makefiles->list[i][1] == '\0')
	  {
	    /* This makefile is standard input.  Since we may re-exec
	       and thus re-read the makefiles, we read standard input
	       into a temporary file and read from that.  */
	    static char name[] = "/tmp/GmXXXXXX";
	    FILE *outfile;

	    /* Free the storage allocated for "-".  */
	    free (makefiles->list[i]);

	    /* Make a unique filename.  */
	    (void) mktemp (name);

	    outfile = fopen (name, "w");
	    if (outfile == 0)
	      pfatal_with_name ("fopen (temporary file)");
	    while (!feof (stdin))
	      {
		char buf[2048];
		int n = fread (buf, 1, sizeof(buf), stdin);
		if (n > 0 && fwrite (buf, 1, n, outfile) != n)
		  pfatal_with_name ("fwrite (temporary file)");
	      }
	    /* Try to make sure we won't remake the temporary
	       file when we are re-exec'd.  Kludge-o-matic!  */
	    fprintf (outfile, "%s:;\n", name);
	    (void) fclose (outfile);

	    /* Replace the name that read_all_makefiles will
	       see with the name of the temporary file.  */
	    makefiles->list[i] = savestring (name, sizeof name - 1);

	    /* Make sure the temporary file will not be remade.  */
	    f = enter_file (savestring (name, sizeof name - 1));
	    f->updated = 1;
	    f->update_status = 0;
	    /* Let it be removed when we're done.  */
	    f->intermediate = 1;
	    /* But don't mention it.  */
	    f->dontcare = 1;
	  }
    }

#ifdef PMAKE
  /* Become a gang-scheduled parallel application.  This must be
     done before setting up the SIGCHLD signal handler. */
  if (partitions) 
#ifdef PARAGON
    init_parallel (partitions->list[0], &job_slots, job_slots_origin );  
#else
    error ("-P <partition> option only supported for Paragon\n");
#endif
#endif

  /* Set up to handle children dying.  This must be done before
     reading in the makefiles so that `shell' function calls will work.  */

#if	defined (SIGCHLD) && !defined (USG)
  (void) SIGNAL (SIGCHLD, child_handler);
#else
  (void) SIGNAL (SIGCLD, child_handler);
#endif

  /* Define the initial list of suffixes for old-style rules.  */

  set_default_suffixes ();

  /* Define some internal and special variables (SHELL & MAKELEVEL).  */

  define_automatic_variables ();

  /* Set up the MAKEFLAGS and MFLAGS variables so makefiles can look 
     at them.  Don't include -j and -l options since we don't want
     to pass them down to recursive makefiles. */

  define_makeflags (0, 0);

  /* Define the default variables.  */
  define_default_variables ();

  /* Create a target to hold default rules */

  default_file = enter_file (".DEFAULT");

#ifdef PMAKE
  /* Create a target to hold .EXPORT variables */
  export_file = enter_file (".EXPORT");
#endif

  /* Read all the makefiles.  */

  read_makefiles
    = read_all_makefiles (makefiles == 0 ? (char **) 0 : makefiles->list);

#ifdef PMAKE
  (void) set_export_flags();

  if (machine_specific_flag)
    set_machine_path (&machinedir);

  if (makeconf_flag) { 
    if (makeconf) 
      (void) reset_makeconf_environment();
    fix_vpath();
  }
#endif

  decode_env_switches ("MAKEFLAGS", 9);
  decode_env_switches ("MFLAGS", 6);

  /* Set up MAKEFLAGS and MFLAGS again, including -j and -l options. */

  define_makeflags (1, 0);

  ignore_errors_flag |= lookup_file (".IGNORE") != 0;

  silent_flag |= lookup_file (".SILENT") != 0;

#ifdef PMAKE
  /* Check for and expand dynamic dependency references ($$@). */
  dynamic_dep ();
#endif

  /* Make each `struct dep' point at the
     `struct file' for the file depended on.  */
  snap_deps ();

  /* Install the default implicit rules.
     This used to be done before reading the makefiles.
     But in that case, built-in pattern rules were in the chain
     before user-defined ones, so they matched first.  */

  install_default_implicit_rules ();

  /* Convert old-style suffix rules to pattern rules.  */

  convert_to_pattern ();

  /* Compute implicit rule limits.  */

  count_implicit_rule_limits ();

  /* Construct the listings of directories in VPATH lists.  */

  build_vpath_lists ();

  /* Mark files given with -o flags as very old (00:00:01.00 Jan 1, 1970)
     and as having been updated already, and files given with -W flags
     as brand new (time-stamp of now).  */

  if (old_files != 0)
    for (p = old_files->list; *p != 0; ++p)
      {
	f = enter_file (*p);
	f->last_mtime = (time_t) 1;
	f->updated = 1;
	f->update_status = 0;
      }

  if (new_files != 0)
    {
      now = time ((time_t *) 0);
      for (p = new_files->list; *p != 0; ++p)
	{
	  f = enter_file (*p);
	  f->last_mtime = now;
	}
    }

  /* First loop through the makefiles and update any that are out of
     date (i.e., this will grab a new copy of a makefile if necessary).i
     We must do this before updating targets. */
#ifdef TEST

  if (read_makefiles != 0)
    {
      time_t *makefile_mtimes = 0;
      unsigned int mm_idx = 0;

      if (debug_flag)
	puts ("Updating makefiles....");

      /* Remove any makefiles we don't want to try to update.
	 Also record the current modtimes so we can compare them later.  */
      {
	register struct dep *d, *last;
	last = 0;
	d = read_makefiles;
	while (d != 0)
	  {
	    register struct file *f = d->file;
	    if (f->double_colon)
	      {
		do
		  {
		    if (f->deps == 0 && f->cmds != 0)
		      {
			/* This makefile is a :: target with commands, but
			   no dependencies.  So, it will always be remade.
			   This might well cause an infinite loop, so don't
			   try to remake it.  */

			if (debug_flag)
			  printf ("Makefile `%s' might loop; not remaking it\n",
				  f->name);

			if (last == 0)
			  read_makefiles = d->next;
			else
			  last->next = d->next;

			/* Free the storage.  */
			free ((char *) d);

			d = last == 0 ? 0 : last->next;

			break;
		      }
		    f = f->prev;
		  }
		while (f != NULL);
	      }
	    if (f == NULL || !f->double_colon)
	      {
		if (makefile_mtimes == 0)
		  makefile_mtimes = (time_t *) gmalloc (sizeof (time_t));
		else
		  makefile_mtimes = (time_t *)
		    xrealloc ((char *) makefile_mtimes,
			      (mm_idx + 1) * sizeof (time_t));
		makefile_mtimes[mm_idx++] = file_mtime_no_search (d->file);
		last = d;
		d = d->next;
	      }
	  } /* while */
      }	

      /* Set up `MAKEFLAGS' specially while remaking makefiles.  In
         particular, don't include -j, -l, -n, -q or -t.  */
      define_makeflags (1, 1);

      /* Now do the update. */
      switch (update_goal_chain (read_makefiles, 1))
	{
	default:
	  abort ();
	  
	case -1:
	  /* Did nothing.  */
	  break;
	  
	case 1:
	  /* Failed to update.  Figure out if we care.  */
	  {
	    /* Nonzero if any makefile was successfully remade.  */
	    int any_remade = 0;
	    /* Nonzero if any makefile we care about failed
	       in updating or could not be found at all.  */
	    int any_failed = 0;
	    register unsigned int i;
            struct dep *next = copy_dep_chain (read_makefiles);

	    for (i = 0; next != 0; ++i)
	      {
		struct dep *d = next;
		next = d->next;
		if (d->file->updated)
		  {
		    /* This makefile was updated.  */
		    if (d->file->update_status == 0)
		      {
			/* It was successfully updated.  */
			any_remade |= (file_mtime_no_search (d->file)
				       != makefile_mtimes[i]);
		      }
#ifdef PMAKE
		    else if (d->changed != 1 && d->changed < 3)
#else
		    else if (d->changed != 1)
#endif
		      {
			time_t mtime;
			/* The update failed and this makefile was not
			   from the MAKEFILES variable, so we care.  */
			error ("Failed to remake makefile `%s'.",
			       d->file->name);
			mtime = file_mtime_no_search (d->file);
			any_remade |= (mtime != (time_t) -1
				       && mtime != makefile_mtimes[i]);
		      }
		  }
		else
		  /* This makefile was not found at all.  */
		  switch (d->changed)
		    {
		    case 0:
		      /* A normal makefile.  We must die later.  */
		      error ("Makefile `%s' was not found", dep_name (d));
		      any_failed = 1;
		      break;
		    case 1:
		      /* A makefile from the MAKEFILES variable.
			 We don't care.  */
		      break;
		    case 2:
		      /* An included makefile.  We don't need
			 to die, but we do want to complain.  */
		      error ("Included makefile `%s' was not found.",
			     dep_name (d));
		      break;
#ifdef PMAKE
		    case 3:
		      /* A makefile included with the "-include"
			 directive.  We don't care.  */
		      break;
		    case 4:
		      /* A Makeconf file.  We don't care.  */
		      break;
#endif
		    }

		free ((char *) d);
	      }

	    if (any_remade)
	      goto re_exec;
	    else if (any_failed)
	      die (1);
	    else
	      break;
	  }

	case 0:
	re_exec:;
	  /* Updated successfully...now re-exec ourselves with the
	     updated makefile.  */
	  if (print_directory_flag)
	    log_working_directory (0);
	  if (debug_flag)
	    puts ("Re-execing myself....");
	  if (makefiles != 0)
	    {
	      /* Makefile names might have changed...update argv[]. */
	      register unsigned int i, j = 0;
	      for (i = 1; i < argc; ++i)
		if (!strcmp (argv[i], "-f"))
		    {
		      char *p = &argv[i][2];
		      if (*p == '\0')
			argv[++i] = makefiles->list[j];
		      else
			argv[i] = concat ("-f", makefiles->list[j], "");
		      ++j;
		    }
	    }
          /* Make sure that we re-exec from the original directory. */
	  if (directories != 0 && directories->idx > 0)
	    {
	      char bad;
	      if (starting_directory[0] != '\0')
		{
		  if (chdir (starting_directory) < 0)
		    {
		      perror_with_name ("chdir", "");
		      bad = 1;
		    }
		  else
		    bad = 0;
		}
	      else
		bad = 1;
	      if (bad)
		fatal ("Couldn't change back to original directory.");
	    }
	  fflush (stdout);
	  fflush (stderr);
	  for (p = environ; *p != 0; ++p)
	    if (!strncmp (*p, "MAKELEVEL=", 10))
	      {
		sprintf (*p, "MAKELEVEL=%u", makelevel);
		break;
	      }
	  exec_command (argv, environ);
	  /* NOTREACHED */
	} /* switch */
    } /* if */
#endif

  /* Set up `MAKEFLAGS' again for the normal targets (include all flags). */
  define_makeflags (1, 0);

#ifdef PMAKE
  if ( makeconf_flag && objectdir && !in_objectdir) {

    /* An OBJECTDIR was specified so we must change to the object
       directory.  Also, since some makefiles may have already been
       updated, we reset the updated flags to make sure that we 
       re-search the VPATH and find them from the object directory. */

    struct dep *d;

    change_dir(objectdir);
   
    for (d = read_makefiles; d != 0; d = d->next) {
      if (d->file->updated && (*dep_name(d) != '/')) {
        d->file->updated = 0;
        d->file->command_state = cs_not_started;
        d->file->update_status = -1;
        d->file->tried_implicit = 0;
        d->file->last_mtime = (time_t) 0;
      }
    }
  }
#endif

  {
    int status;

    /* If there were no command-line goals, use the default.  */
    if (goals == 0)
      {
	if (default_goal_file != 0)
	  {
	    goals = (struct dep *) gmalloc (sizeof (struct dep));
	    goals->next = 0;
	    goals->name = 0;
#ifdef PMAKE
            goals->origname = 0;
#endif
 	    goals->file = default_goal_file;
	    lastgoal = goals;
	  }
      }
    else
      lastgoal->next = 0;

#ifdef PMAKE
  
    /* If a .INIT target was specified, add it to front of goal chain */ 
    if ((f = lookup_file (".INIT")) != 0) 
      {
        f->cmd_target = 1;
        f->noparallel = 1;
        if (goals == 0)
          {
            goals = (struct dep *) gmalloc (sizeof (struct dep));
            goals->next = 0;
            lastgoal = goals;
          }
        else
          {
            struct dep *temp = (struct dep *) gmalloc (sizeof (struct dep));
            temp->next = goals;
            goals = temp;
          }
        goals->name = 0;
        goals->origname = 0;
        goals->file = f;
      }
  
    /* If a .EXIT target was specified, add it to end of goal chain */ 
    if ((f = lookup_file (".EXIT")) != 0) 
      {
        f->cmd_target = 1;
        f->noparallel = 1;
        if (goals == 0)
          {
            goals = (struct dep *) gmalloc (sizeof (struct dep));
            lastgoal = goals;
          }
        else
          {
            lastgoal->next = (struct dep *) gmalloc (sizeof (struct dep));
            lastgoal = lastgoal->next;
          }
        lastgoal->name = 0;
        lastgoal->origname = 0;
        lastgoal->file = f;
        lastgoal->next = 0;
      }
#endif

    /* Print the data base under -p.  */
    if (print_data_base_flag)
      print_data_base ();

    if (goals != 0)
      {
	/* Update the goals.  */

	if (debug_flag)
	  puts ("Updating goal targets....");

	switch (update_goal_chain (goals, 0))
	  {
	  case -1:
	    /* Nothing happened.  */
	  case 0:
	    /* Updated successfully.  */
	    status = 0;
	    break;
	  case 1:
	    /* Updating failed.  */
	    status = 1;
	    break;
	  default:
	    abort ();
	  }
      }

    if (goals == 0)
      {
	if (read_makefiles == 0)
	  fatal ("No targets specified and no makefile found");
	else
	  fatal ("No targets");
      }

    /* Exit.  */
    die (status);
  }

  return 0;
}

/***************************************************************************
 *
 *    Calling Sequence:
 *        decode_switches (argc, argv)   
 *
 *    Description:
 *        Parse and decode the command line switches.
 *
 *    Parameters:
 *        argc:		Number of command line arguments.
 *        argv:		Table of arguments.
 *
 *    Returns:
 *        void
 *
 **************************************************************************/

static void
decode_switches (argc, argv)
     int argc;
     char **argv;
{
  char bad = 0;
  register unsigned int i;
  register char *sw;
  register struct command_switch *cs;
  register struct stringlist *sl;
  char *arg;

  decode_env_switches ("MAKEFLAGS", 9);
  decode_env_switches ("MFLAGS", 6);

  other_args = (struct stringlist *) gmalloc (sizeof (struct stringlist));
  other_args->max = 5;
  other_args->list = (char **) gmalloc (5 * sizeof (char *));
  other_args->idx = 1;
  other_args->list[0] = savestring (argv[0], strlen (argv[0]));

					/* Loop through arguments */
  for (i = 1; i < argc; i++)
    {
      sw = argv[i];
      if (*sw++ == '-')
					/* Found a switch -- match to switch
					   table.  */
	while (*sw != '\0')
	  {
	    for (cs = switches; cs->c != '\0'; ++cs)
	      if (cs->c == *sw)
		{
					/* Found a match -- switch type 
					   determines action to be taken. */
		  ++sw;
		  switch (cs->type)
		    {
		    default:
		      abort ();
		    case ignore:
		      break;		/* Ignore the switch. */
		    case flag:
		    case flag_off:	/* Turn flag on or off. */
		      *(int *) cs->value_ptr = cs->type == flag;
		      break;
		    case string:	/* Next arg should be string. */
		      if (*sw == '\0')
			{
			  arg = argv[++i];
			  if (arg == 0)
			    {
			      arg = cs->noarg_value;
			      if (arg == 0)
				{
				  error ("argument required for `-%c' option",
					 cs->c);
				  bad = 1;
				  break;
				}
			    }
			}
		      else
			arg = sw;
					/* Add string to list of strings 
					   for the switch. */
		      sl = *(struct stringlist **) cs->value_ptr;
		      if (sl == 0)
			{
			  sl = (struct stringlist *)
			    gmalloc (sizeof (struct stringlist));
			  sl->max = 5;
			  sl->idx = 0;
			  sl->list = (char **) gmalloc (5 * sizeof (char *));
			  *(struct stringlist **) cs->value_ptr = sl;
			}
		      else if (sl->idx == sl->max - 1)
			{
			  sl->max += 5;
			  sl->list = (char **)
			    xrealloc ((char *) sl->list,
				      sl->max * sizeof (char *));
			}
		      sl->list[sl->idx++] = savestring (arg, strlen (arg));
		      sl->list[sl->idx] = 0;
		      sw = "";
		      break;

		    case positive_int:		/* Next arg should be int. */
		      if (*sw == '\0')
			arg = argv[++i];
		      else
			arg = sw;
		      if (arg != 0 && isdigit (*arg))
			{
			  int i = atoi (arg);
			  if (arg == sw)
			    while (isdigit (*sw))
			      ++sw;
			  if (i < 1)
			    {
			      error ("the `-%c' option requires a \
positive integral argument",
				     cs->c);
			      bad = 1;
			    }
			  else
			    *(unsigned int *) cs->value_ptr = i;
			}
		      else
			{
			  if (cs->noarg_value != 0)
			    *(unsigned int *) cs->value_ptr
			      = *(unsigned int *) cs->noarg_value;
			  else
			    {
			      error ("the `-%c' option requires an argument",
				     cs->c);
			      bad = 1;
			    }

			  if (arg == argv[i])
			    /* We moved to the next arg, so move back.  */
			    --i;
			}
		      break;

		    case floating:		/* Next arg should be float. */
		      if (*sw == '\0')
			arg = argv[++i];
		      else
			arg = sw;
		      if (arg != 0 && (*arg == '.' || isdigit (*arg)))
			{
			  *(double *) cs->value_ptr = atof (arg);
			  if (arg == sw)
			    while (*sw == '.' || isdigit (*sw))
			      ++sw;
			}
		      else
			{
			  if (cs->noarg_value != 0)
			    *(double *) cs->value_ptr
			      = *(double *) cs->noarg_value;
			  else
			    {
			      error ("the `-%c' option requires an argument",
				     cs->c);
			      bad = 1;
			    }

			  if (arg == argv[i])
			    /* We moved to the next arg, so move back.  */
			    --i;
			}
		      break;
		    } /* switch */

#ifdef PMAKE
                  if (cs->origin != 0) {
                    *cs->origin = so_command;
                  }
#endif

		  /* We've found the switch.  Stop looking.  */
		  break;
		} /* if */
		  
	    if (cs->c == '\0')
	      {
		error ("unknown option `-%c'", *sw++);
		bad = 1;
	      }
	  } /* while */
      else
	{
					/* Arg is not a switch -- add to 
					   list of other arguments. */
	  if (other_args->idx == other_args->max - 1)
	    {
	      other_args->max += 5;
	      other_args->list = (char **)
		xrealloc ((char *) other_args->list,
			  other_args->max * sizeof (char *));
	    }
	  other_args->list[other_args->idx++] = argv[i];
	}
    } /* for */

  if (other_args != 0)
    other_args->list[other_args->idx] = 0;

  if (bad)
    die (1);
}

/***************************************************************************
 *
 *    Calling Sequence:
 *        decode_env_switches (envar, len)
 *
 *    Description:
 *        Parse and decode the MFLAGS and MAKEFLAGS environment variables.
 *
 *    Parameters:
 *        envar:	Name of environment variable.
 *        len:		Length of envar.    uments.
 *
 *    Returns:
 *        void
 *
 **************************************************************************/

static void
decode_env_switches (envar, len)
     char *envar;
     unsigned int len;
{
  struct variable *v;
  register char *args;
  register struct command_switch *cs;

						/* Look-up the variable. */
  v = lookup_variable (envar, len);
  if (v == 0 || *v->value == '\0')
    return;
						/* Found the variable, so
						   parse and match against
						   switches. */
  for (args = v->value; *args != '\0'; ++args)
    for (cs = switches; cs->c != '\0'; ++cs)
      if (cs->c == *args)	
						/* Found a match; is the 
						   switch overridable from
						   the environment? */
	if (cs->env) {			
	  switch (cs->type)		/* Action depends on switch type. */
	    {
	    case string:
	      /* None of these allow environment changes.  */
	    default:
	      abort ();
	    case flag:
	    case flag_off:
	      *(int *) cs->value_ptr = cs->type == flag;
	      break;
	    case positive_int:
	      while (isspace (args[1]))
		++args;
	      if (isdigit(args[1]))
		{
		  int i = atoi (&args[1]);
		  while (isdigit (args[1]))
		    ++args;
		  if (i >= 1)
		    *(unsigned int *) cs->value_ptr = i;
		}
	      else
		*(unsigned int *) cs->value_ptr
		  = *(unsigned int *) cs->noarg_value;
	      break;
	    case floating:
	      while (isspace (args[1]))
		++args;
	      if (args[1] == '.' || isdigit (args[1]))
		{
		  *(double *) cs->value_ptr = atof (&args[1]);
		  while (args[1] == '.' || isdigit (args[1]))
		    ++args;
		}
	      else
		*(double *) cs->value_ptr = *(double *) cs->noarg_value;
	      break;
	    }

#ifdef PMAKE
            if (cs->origin != 0) {
              *cs->origin = so_environment;
            }
#endif
          }

}

/* Define the MAKEFLAGS and MFLAGS variables to reflect the settings of the
   command switches.  Include positive_int and floating options if PF.
   Don't include options with the `no_makefile' flag is if MAKEFILE.  */

static void
define_makeflags (pf, makefile)
     int pf, makefile;
{
  register struct command_switch *cs;
  char flags[200];
  register unsigned int i;

  i = 0;
  for (cs = switches; cs->c != '\0'; ++cs)
    if (cs->toenv && (!makefile || !cs->no_makefile))
      {
	if (i == 0 || flags[i - 1] == ' ')
	  flags[i++] = '-';
	switch (cs->type)
	  {
	  default:
	    abort ();

	  case ignore:
	    break;

	  case flag:
	  case flag_off:
	    if (!*(int *) cs->value_ptr == (cs->type == flag_off)
		&& (cs->default_value == 0
		    || *(int *) cs->value_ptr != *(int *) cs->default_value))
	      flags[i++] = cs->c;
	    break;

	  case positive_int:
	    if (pf)
	      {
		if ((cs->default_value != 0
		     && (*(unsigned int *) cs->value_ptr
			 == *(unsigned int *) cs->default_value)))
		  break;
		else if (cs->noarg_value != 0
			 && (*(unsigned int *) cs->value_ptr ==
			     *(unsigned int *) cs->noarg_value))
		  flags[i++] = cs->c;
		else
		  {
		    unsigned int value;
		    if (cs->c == 'j')
		      value = 1;
		    else
		      value = *(unsigned int *) cs->value_ptr;
		    sprintf (&flags[i], "%c%u ", cs->c, value);
		    i += strlen (&flags[i]);
		  }
	      }
	    break;

	  case floating:
	    if (pf)
	      {
		if (cs->default_value != 0
		    && (*(double *) cs->value_ptr
			== *(double *) cs->default_value))
		  break;
		else if (cs->noarg_value != 0
			 && (*(double *) cs->value_ptr
			     == *(double *) cs->noarg_value))
		  flags[i++] = cs->c;
		else
		  {
		    sprintf (&flags[i], "%c%f ",
			     cs->c, *(double *) cs->value_ptr);
		    i += strlen (&flags[i]);
		  }
	      }
	    break;

#ifdef TEST 
	  case string:
	    if (pf)
	      {
                register int j;
                struct stringlist *s = *(struct stringlist **)cs->value_ptr;
                if (s != 0) {
                  for (j = 0; j < s->idx; ++j) {
                    sprintf (&flags[i], "%c%s ", cs->c, s->list[j]);
		    i += strlen (&flags[i]);
                  }
                }
	      }
	    break;
#endif
	  }
      }

  if (i == 0)
    flags[0] = flags[1] = '\0';
  else if (flags[i - 1] == ' ' || flags[i - 1] == '-')
    flags[i - 1] = '\0';
  flags[i] = '\0';

  /* On Sun, the value of MFLAGS starts with a `-' but the
     value of MAKEFLAGS lacks the `-'.  Be compatible.  */
#ifdef PMAKE
  (void) define_variable ("MAKEFLAGS", 9, &flags[1], o_env, 0, 1);
  (void) define_variable ("MFLAGS", 6, flags, o_env, 0, 1);
#else
  (void) define_variable ("MAKEFLAGS", 9, &flags[1], o_env, 0);
  (void) define_variable ("MFLAGS", 6, flags, o_env, 0);
#endif
}

static int printed_version = 0;

/* Print version information.  */

static void
print_version ()
{
  extern char *remote_description;
  char *precede = print_data_base_flag ? "# " : "";

#ifdef PMAKE
  printf ("%spmake version %s\n", precede, cvs_version);

  printf ("%sCopyright (C) 1988-1994 Intel Corporation.\n\n\
%spmake is an extension of GNU Make and is distributed under\n\
%sthe terms of the GNU General Public License.\n\n", 
precede, precede, precede);
#else
  printf ("%sGNU Make version %s", precede, version_string);
  if (remote_description != 0 && *remote_description != '\0')
    printf ("-%s", remote_description);

  printf (", by Richard Stallman and Roland McGrath.\n\
%sCopyright (C) 1988-1991 Free Software Foundation, Inc.\n\
%sThis is free software; see the source for copying conditions.\n\
%sThere is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A\n\
%sPARTICULAR PURPOSE.\n\n", precede, precede, precede, precede);
#endif

  printed_version = 1;

  /* Flush stdout so the user doesn't have to wait to see the
     version information while things are thought about.  */
  fflush (stdout);
}

/* Print a bunch of information about this and that.  */

static void
print_data_base ()
{
  extern char *ctime ();
  time_t when;

  when = time ((time_t *) 0);
  printf ("\n# Make data base, printed on %s", ctime (&when));

  print_variable_data_base ();
  print_dir_data_base ();
  print_rule_data_base ();
  print_file_data_base ();
  print_vpath_data_base ();

  when = time ((time_t *) 0);
  printf ("\n# Finished Make data base on %s\n", ctime (&when));
}

/* Exit with STATUS, cleaning up as necessary.  */

void
die (status)
     int status;
{
  static char dying = 0;

  if (!dying)
    {
      dying = 1;

      if (print_version_flag && !printed_version)
	print_version ();

      /* Wait for children to die.  */
      wait_for_children (0, status != 0);

      /* Remove the intermediate files.  */

      remove_intermediates (0);

      if (print_directory_flag)
	log_working_directory (0);
    }

  exit (status);
}

/* Write a message indicating that we've just entered or
   left (according to ENTERING) the current directory.  */

static void
log_working_directory (entering)
     int entering;
{
  static int entered = 0;
  PATH_VAR (pwdbuf);
  char *message = entering ? "Entering" : "Leaving";

  if (entered && entering)
    /* Don't print the leaving message if we
       haven't printed the entering message.  */
    return;
  else
    entered = 1;

  if (print_data_base_flag)
    fputs ("# ", stdout);

  if (makelevel == 0)
    printf ("%s: %s ", program, message);
  else
    printf ("%s[%u]: %s ", program, makelevel, message);

#ifdef	USG
  /* In some System V's, `getcwd' spawns a child running /bin/pwd.  */
  push_signals_blocked_p (1);
#endif
  if (getwd (pwdbuf) == 0)
    {
#ifdef	USG
      perror_with_name ("getcwd: ", "");
#else
      error ("getwd: %s", pwdbuf);
#endif
      puts ("an unknown directory");
    }
  else
    printf ("directory `%s'\n", pwdbuf);
#ifdef	USG
  pop_signals_blocked_p ();
#endif
}


#ifdef POSIX

/* Support for POSIX signal handling */

SIGHANDLER 
xsignal (sig, handler)
  int 	     sig;
  SIGHANDLER handler;
{
  struct sigaction act, oact;
  
  act.sa_handler = handler;
  act.sa_flags   = SA_RESTART;
  sigemptyset (&act.sa_mask);

  if (sigaction(sig, &act, &oact))
    pfatal_with_name ("sigaction");

  return (SIGHANDLER) oact.sa_handler;
}

#endif
