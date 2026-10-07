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
 
/*****************************************************************************
 *              Copyright (c) 1990 San Diego Supercomputer Center.
 *              All rights reserved.  The SDSC software License Agreement
 *              specifies the terms and conditions for redistribution.
 *
 * File:        mac_ui.c
 *
 * Abstract:	This program is the users interface driver to the MACS 
 *		administration utilities
 *****************************************************************************/
#ifdef LINT
static char     sccs_id[] = "%W% %H%";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "mac_def.h"
#include "mac_ui.h"

static void	mac_act();		/* DEBUG */

/*===========================================================================*
 * Function:    main
 *
 * Abstract:    If this program is invoked without command-line options
 *		then the menu-driven user interface will be used to
 *		get input and convert the inputs into command-line
 *		arguments.  This driver will pass the command-line argument
 *		list to the mac_adm() to actually perform the requested
 *		administration tasks.
 *
 * Arguments:	argc -	argument count
 *		argv -	argument list
 *
 * Return value: None
 *
 * Notes:
 *===========================================================================*/
int main (argc, argv)
   int	argc;
   char	*argv[];
{
  char		*argp[20];		/* array of arguments */
  int		args;			/* number of built up arguments */

  char		under_line[80];
  char		prompt[128];
  int		i;
  int		j;
  int		menu_item;
  int		menu_index;
  int		rval;
  char		**arg_line;
  extern void	bcopy();

  /* verify that MACS is in "macwatch" mode */
  is_macwatch();

  /* test if there are command line arguments */
  if (argc > 1) {
#ifdef DEBUG
    (void) mac_act(argc, argv, NOT_CHECKED);		/* DEBUG */
#endif
#ifdef MACADMIN
    (void) mac_adm(argc, argv, NOT_CHECKED, 0);
#else
    (void) mac_adm(argc, argv, NOT_CHECKED, 2);
#endif
    exit(0);
  }

  argp[0] = argv[0];
  args = 1;

  /* fill the under line */
  for (i = 0; i < sizeof(under_line); i++) under_line[i] = '-';

menu:
  for (;;) {
    /*-----------------------------------------------------------------------*
     * Print main menu
     *-----------------------------------------------------------------------*/
    (void) printf("%s", TITLE_LINE);
    for (i = 0; i < N_APPL_MENU; i++) {
      (void) printf("\t%d\t%s\n", i, menu_line[appl_menu[i]]);
      if (i == 4) {
         (void) printf("\n");  /* delimit menu group */
      }
    }
    (void) printf("\t%d\tExit\n\n", N_APPL_MENU);

    /*-----------------------------------------------------------------------*
     * Get user menu selection
     *-----------------------------------------------------------------------*/
    menu_item = get_menu_item(N_APPL_MENU);
    if (menu_item == EOF) {
      exit(0);
    }
    menu_index = appl_menu[menu_item];

item:
    for (;;) {
      argp[1] = (char *)malloc(strlen(menu_arg[menu_index]) + 3);
      (void) sprintf(argp[1], "-%c%s", menu_option, menu_arg[menu_index]);
      args = 2;

      /*---------------------------------------------------------------------*
       * Print selection mesg
       *---------------------------------------------------------------------*/
      (void) printf("\n\n\t%s\n", menu_line[menu_index]);
      i = strlen(menu_line[menu_index]);
      under_line[i] = '\0';
      (void) printf("\t%s\n\n", under_line);
      under_line[i] = '-';

      if (menu_index != ADD_ACCT_IDX &&
          menu_index != ADD_USER_IDX &&
          menu_index != MOV_ALLOC_IDX) {
        (void) printf("Maximum of 10 accounts or users may be specified\n");
        (void) printf("at one time.  Use comma as delimiter.\n");
      }
      (void) printf("Enter q<return> or Q<return> to go back to menu\n\n");

      /*---------------------------------------------------------------------*
       * Get required info from user
       *---------------------------------------------------------------------*/
      for (i = 0; i < N_FUNC_MASK; i++) {
        if ((1 << i) & menu_info_mask[menu_index]) {
          rval = (*check_info[i])(argp + args, menu_def[menu_index]);
          if (rval < 0) {
            /* user entered q or Q or Error */
            for (j = 1; j < args; j++) {
              (void) free(argp[j]);
            }
            args = 1;
            goto menu;

          } else if (rval > OVERSIGN) {
            args += (rval - OVERSIGN);			/* skip next item */
            i++;

          } else {
            args += rval;				/* OK */
          }
        }
      }

      /*---------------------------------------------------------------------*
       * Ask the user if input was correct
       *---------------------------------------------------------------------*/
      if (menu_index != LIST_ACCT_IDX && menu_index != LIST_USER_IDX) {
        switch (get_yes_no("\nInput ok (y/n)? ")) {
        case EOF:	exit(0); 		/* EOF */
        case 0:					/* no */
          for (j = 1; j < args; j++) {
            (void) free(argp[j]);
          }
          args = 1;
          goto item;
        case 1:		break;			/* yes */
        }
      }

      /*---------------------------------------------------------------------*
       * send the request for execution
       *---------------------------------------------------------------------*/
      arg_line  = (char **)malloc((args + 1) * sizeof(char *));
      (void) bcopy(argp, arg_line, args * sizeof(char *));
      arg_line[args] = NULL;

#ifdef DEBUG
      (void) mac_act(args, arg_line, CHECKED);		/* DEBUG */
#endif
      (void) mac_adm(args, arg_line, CHECKED);

      /* clean up */
      for (j = 1; j < args; j++) {
        (void) free(arg_line[j]);
      }
      args = 1;
      (void) free(arg_line);

      /*---------------------------------------------------------------------*
       * Ask user if action is to be repeated
       *---------------------------------------------------------------------*/
      (void) sprintf(prompt, "\ncontinue to %s (y/n)? ", menu_line[menu_index]);
      switch (get_yes_no(prompt)) {
      case EOF:	exit(0);		/* EOF */
      case 0:	goto menu;		/* no */
      case 1:	break;			/* yes */
      }
    }
  }
}


/*===========================================================================*
 * Function:    mac_act
 *
 * Abstract:    This function prints out the parameters passed
 *
 * Arguments:
 *
 * Return value:
 *
 * Notes:
 *===========================================================================*/
static void mac_act(argc, argv, checked)
  int   argc;
  char  **argv;
  int   checked;
{
  int i;

  (void) fprintf(stderr, "argc=%d\n", argc);
  for (i = 0; i < argc; i++) {
    (void) fprintf (stderr, "argv[%d]=%s\n", i, argv[i]);
  }
  (void) fprintf(stderr, "checked=%d\n", checked);
}

