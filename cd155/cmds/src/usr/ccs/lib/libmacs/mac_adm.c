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
 * File:        mac_adm.c
 *
 * Abstract:	This file contains all the functions for MACS
 *		administration tasks
 *****************************************************************************/
#ifdef LINT
static char     sccs_id[] = "%W% %H%";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <pwd.h>

#include "mac_def.h"
#include "mac.h"
#include "macerr.h"
#include "conf.h"
#include "misc.h"
#include "agdb.h"
#include "filename.h"

#define CHECK(X) do { if (checked) return(X); else exit(X); } while (0)
#ifdef __STDC__
#else
#define const
#endif


extern char		*getlogin();
extern int		cpuctl();
extern int		mac_list();
extern int		mac_modify();
extern int		mac_transfer();
extern int		mac_create();
extern int		mac_delacct();
extern int		mac_deluser();
extern int		mac_update();
extern void		macinit();

extern int		optind;
extern int		opterr;
extern char		*optarg;

/*---------------------------------------------------------------------------*
 * Internal
 *---------------------------------------------------------------------------*/
static void		usage ();

/*===========================================================================*
 * Function:	mac_adm
 *
 * Abstract:	This function performs all the administration tasks of the
 *		MACS.  It can be called from any
 *		application, where the arguments can be either validated
 *		or not.  It will validate the arguments and exit (not
 *		return to the caller) when it's done only if checked=0.
 *
 * Arguments:
 *	argc -	argument count
 *	argv -	argument list
 *	checked - if the argument list was has been validated already.
 *
 * Return value:
 *		0	successful
 *		-1	error
 *
 * Notes:
 *===========================================================================*/
int mac_adm(argc, argv, checked, restrict)
  int	argc;
  char	**argv;
  int	checked;
  int   restrict;
{
  int			c;			/* option character */

  int			accts[MAX_AGIDS];
  int			users[MAX_UIDS];
  int			naccts = 0;
  int			nusers = 0;
  int			i;
  int			j;
  int			ret_val;    /* returnvalue */
  int			mflg = 0;     /* mode */
  int			Aflg = 0;     /* new account name, id, description */
  int			aflg = 0;     /* account name or id */
  int			uflg = 0;     /* user name or id */
  int			Fflg = 0;     /* from account name orid */
  int			Tflg = 0;     /* to account name or id */
  int			wflg = 0;     /* weight */
  int			kflg = 0;     /* kill */
  int			lflg = 0;     /* lock */
  int			pflg = 0;     /* grand permission */
  int			Pflg = 0;     /* percent */
  int			tflg = 0;     /* minute time */
  int			rflg = -1;  /* unlimit allocation time, initial false */
  int			nflg = 0;      /* max. nodes number */
  int			fnl_rtrn = 0;	/* final return value */
  int			acct_id;
  int			from_acid;
  int			to_acid;
  int			weight;
  int			kill;
  int			lock;
  int			accsign;
  int			permission;
  int			tmsign;
  int			max_nodes;
  int			nitem;
  double		seconds;

  char			*ptr;

  char			mode[3];
  char			*acct_name;
  char			*acct_desc;
  char			**in_acids = NULL;
  char			**in_uids;
  char			*login;
  char			id_str[128];
  char			errmsg[128];
  struct macsconf	*conf;
  FILE			*logfp;


  if (argc < 2 ) {
    usage(restrict, (char *)NULL);
  }

  optind = opterr = 1;

  if ((conf = readconf()) == NULL) {
    switch (errno) {
      case ENOENT: (void) fprintf(stderr,
                     "Fail in opening %s.\n",
                     MACS_CONF_FILE);
                   break;
      case ENOMEM: (void) fprintf(stderr,
                     "Fail in malloc() in readconf().\n");
                   break;
      case EINVAL: (void) fprintf(stderr,
                     "Invalid enforcement parameter in %s.\n",
                     MACS_CONF_FILE);
                   break;
      default: (void) fprintf(stderr,
                 "Unknown error returned from readconf().\n");
    }
    return (-1);
  }

  /*-------------------------------------------------------------------------*
   * parse option arguments
   *-------------------------------------------------------------------------*/
  while((c = getopt(argc, argv, "%:A:F:T:a:N:l:m:n:p:rt:u:w:")) != -1) {
    switch(c) {
    case '%':
      if (tflg) {
        printf("\n-t%f specified allocation time, -%c%s ignored\n",
               (seconds / 60.0), '%', optarg);
        break;
      }
      if (rflg == 1) {
        printf("\n-r for unlimit allocation, -%c%s ignored\n", '%', optarg);
        break;
      }
      if (optarg[0] == '+') {
        tmsign = 1;
        i = 1;
      } else if (optarg[0] == '-') {
        tmsign = -1;
        i = 1;
      } else {
        tmsign = 0;
        i = 0;
      }
      if (!checked && !percent_syntex_ok(optarg+i)) {
        usage(restrict, "Argument of -% must be a number");
      }
      seconds = atof(optarg+i);
      if (seconds < 0.0 || seconds > 100.0) {
        if (!checked) {
          usage(restrict, "Argument of -% must be a number between 0-100");
        } else {
          printf("\nPercentage out of range, 0-100 please.\n");
          return(-1);
        }
      }
      seconds *= 100.0;
      Pflg = 1;
      break;

    case 'A':				/* new_account_name,id,description */
      acct_name = strtok(optarg, ",");
      if (!checked && !acct_name_syntex_ok(acct_name)) {
        usage(restrict, "A");
      }

      ptr = strtok((char *)NULL, ",");
      if (!checked && !acct_id_syntex_ok(ptr)) {
        usage(restrict, "A");
      }
      acct_id = atoi(ptr);

      acct_desc = strtok((char *)NULL, ",");
      if (!checked && !acct_desc_syntex_ok(acct_desc)) {
        usage(restrict, "A");
      }
      Aflg = 1;
      naccts = 1;
      break;

    case 'F':
      from_acid = acct_exist_ok(optarg);
      if (!checked && from_acid < 0) {
        usage(restrict, "F");
      }
      Fflg = 1;
      break;

    case 'T':
      to_acid = acct_exist_ok(optarg);
      if (!checked && to_acid < 0) {
        usage(restrict, "T");
      }
      Tflg = 1;
      break;

    case 'a':
      in_acids = mkarray(optarg);
      if (!checked && !accts_exist_ok(in_acids)) {
        frarray(in_acids);
        usage(restrict, (char *)NULL);
      }
      for (i = 0; in_acids[i] != NULL; i++) {
        accts[i] = atoi(in_acids[i]);
      }

      if (i <= 0) {
        usage(restrict, "0 valid account input");
      }
      aflg = 1;
      naccts = i;
      break;

    case 'N':
      kill = atoi(optarg);
      if (!checked) {
          if (kill != 1 && kill != 0)
            usage(restrict, "Argument of -k must be either 0 or 1");
      }
      if (kill == 0) kill = 1;
      else kill = 0;
      kflg = 1;
      break;

    case 'l':
      lock = atoi(optarg);
      if (!checked) {
          if (lock != 1 && lock != 0)
          usage(restrict, "Argument of -l must be either 0 or 1");
      }
      lflg = 1;
      break;

    case 'm':				/* mode */
      if (!checked) {
        for (i = 0; i < N_MENU_ITEM; i++) {
          if (strcmp(optarg, menu_arg[i]) == 0) {
            break;
          }
        }

        (void) sprintf(errmsg, "Invalid mode %s", optarg);
        if (i >= N_MENU_ITEM) usage(restrict, errmsg);

	if (restrict && optarg[0] != 'l' && optarg[0] != 'c' && optarg[0] != 't')
	      usage(restrict, errmsg);
      }
      mflg = 1;
      (void) strncpy (mode, optarg, 3);
      mode[2] = '\0';
      break;

    case 'n':
      if (!checked) {
        for (i = 0; optarg[i] != '\0'; i++) {
          if (!isdigit(optarg[i]))
            usage(restrict, "Argument of -n must be an integer");
        }
        if (atoi(optarg) < -1 || atoi(optarg) > conf->total_nodes)
          usage(restrict, "Argument of -n out of range.");
      }
      max_nodes = atoi(optarg);
      nflg = 1;
      break;

    case 'p':
      if (optarg[0] == '+') {
        accsign = 1;
        i = 1;
      } else if (optarg[0] == '-') {
        accsign = -1;
        i = 1;
      } else {
        accsign = 0;
        i = 0;
      }
      for (permission = 0; optarg[i] != '\0'; i++) {
        if (optarg[i] == 'U' || optarg[i] == 'u')
          permission = permission | USE_MASK;
        else if (optarg[i] == 'M' || optarg[i] == 'm')
          permission = permission | MODIFY_MASK;
        else if (optarg[i] == 'T' || optarg[i] == 't')
          permission = permission | TRANSFER_MASK;
        else
          usage(restrict, "Argument of -p must be either M, T or U");
      }
      pflg = 1;
      break;

    case 't':
      if (rflg == 1) {
        printf("\n-r for unlimit allocation, -t%s ignored\n", optarg);
        break;
      }
      if (optarg[0] == '+') {
        tmsign = 1;
        i = 1;
      } else if (optarg[0] == '-') {
        tmsign = -1;
        i = 1;
      } else {
        tmsign = 0;
        i = 0;
      }
      if (!checked) {
        for (j = i; optarg[j] != '\0'; j++)
          if (!isdigit(optarg[j]))
            usage(restrict, "Argument of -t must be an integer");
      }
      seconds = atof(optarg+i) * 60.0;
      tflg = 1;
      rflg = 0;  /* limited allocation */
      break;

    case 'r':
      rflg = 1;
      break;

    case 'u':
      in_uids = mkarray(optarg);
      if (!checked && !(users_exist_ok(in_uids))) {
          frarray(in_uids);
          usage(restrict, (char *)NULL);
      }

      for (i = 0; in_uids[i] != NULL; i++) {
        users[i] = atoi(in_uids[i]);
      }

      if (i <= 0)
        usage(restrict, "0 valid user input");
      uflg = 1;
      nusers = i;
      break;

    case 'w':
      weight = atoi(optarg);
      if (!checked) {
        if (weight != 1 && weight != 0) {
          usage(restrict, "Argument of -w must be either 0 or 1");
        }
      }
      wflg = 1;
      break;

    default:  
      usage(restrict, NULL);
      exit(1);
    }
  }

  /*-------------------------------------------------------------------------*
   * make sure we have all the necessary information, then do it
   *-------------------------------------------------------------------------*/
  if (!mflg) {
    usage(restrict, "-m option is a must");
  }

  /* check for wrong combinations of command-line options */
  if (!checked) { 
    if (mode[1]=='a') {
      if (Pflg && mode[0] != 't') (void) sprintf(errmsg, 
	"Incompatible options -%c and -m%s", '%', mode);
      else if (pflg) (void) sprintf(errmsg, 
	"Incompatible options -p and -m%s", mode);
      else if (uflg) (void) sprintf(errmsg, 
	"Incompatible options -u and -m%s", mode);
      else errmsg[0] = '\0';
      if (errmsg[0] != '\0') usage(restrict, errmsg);
    }
    else if (mode[1]=='u') {
      if (Fflg) (void) sprintf(errmsg,
        "Incompatible options -F and -m%s", mode);
      else if (Tflg) (void) sprintf(errmsg,
        "Incompatible options -T and -m%s", mode);
      else if (wflg) (void) sprintf(errmsg,
        "Incompatible options -w and -m%s", mode);
      else if (kflg) (void) sprintf(errmsg,
        "Incompatible options -N and -m%s", mode);
      else if (lflg) (void) sprintf(errmsg,
        "Incompatible options -l and -m%s", mode);
      else if (Aflg) (void) sprintf(errmsg,
        "Incompatible options -A and -m%s", mode);
      else errmsg[0] = '\0';
      if (errmsg[0] != '\0') usage(restrict, errmsg);
    }
    if (uflg && mode[1] == 'u' && mode[0] != 'a' &&
      !accts_users_exist_ok(in_uids, in_acids)) 
      usage(restrict, (char *)NULL);
  } 

  if (aflg) frarray(in_acids);
  if (uflg) frarray(in_uids);

  /* open log file */
  logfp = fopen(MACADMIN_LOG_FNAME, "a");


  if ((login = getlogin()) == NULL) {
    login = (getpwuid(getuid()))->pw_name;
  }
  sprintf(id_str, "< %s %s >", argv[0], login ? login : "UNKNOWN");

  /*-------------------------------------------------------------------------*
   * Decode mode option
   *-------------------------------------------------------------------------*/
  switch (mode[0]) {
  case 'a':					/* add account or user */
    if (mode[1] == 'a') {
      /*----------------*
       * add new account
       *----------------*/
      if (!Aflg) {
        usage(restrict, "Must specify account name, id, description with -maa option");
      }
      if (!tflg || rflg == 1)
        seconds = atof(DEF_MINUTES) * 60.0;
      if (rflg == -1)
        rflg = atoi(DEF_UNLIMIT);
      if (!wflg)
        weight = atoi(DEF_WEIGHT);
      if (!kflg)
        kill = !(atoi(DEF_KILL));
      if (!lflg)
        lock = atoi(DEF_LOCK);
      if (!nflg) {
        max_nodes = conf->total_nodes;
	if (max_nodes <= 0) max_nodes = DEF_NODES;
      }

      if ((ret_val = addagid(acct_name, acct_id, acct_desc)) < 0) {
	fnl_rtrn = -1;
        if (ret_val == -1) {
/*
          fprintf(stderr, "\nAccount id exists in /etc/nxaccount file, check MACD database ...\n");
*/
        } else if (ret_val == -2) {
          fprintf(stderr, "\nAccount name exists in /etc/nxaccount file, check MACD database ...\n");
        } else if (ret_val == -3) {
          fprintf(stderr, "\nFail release lock on /etc/nxaccount\n");
        } else if (ret_val == -11) {
          fprintf(stderr, "\nFail open /etc/nxaccount, account not added\n");
        } else if (ret_val == -12) {
          fprintf(stderr, "\nFail getting lock on  /etc/nxaccount, account not added\n");
        } else {
          fprintf(stderr, "\nUnknown error from addagid()\n");
        }

        if (ret_val < -10) {
          if (logfp) fclose(logfp);
          CHECK(-1);
        }
      }
      /* connect to mac daemon */
      macinit();
      ret_val = mac_create(acct_id,
                           seconds,
                           weight,
                           kill,
                           lock,
                           rflg,
                           max_nodes);

      if (ret_val == -1) {
	fnl_rtrn = -1;
        fprintf(stderr, "\nError creating new account in database\n");
      }
      if (logfp) {
        fprintf(logfp,
                "%s %s \n\tADD acct=%d, time=%f, weight=%d, Nokill=%d, lock=%d, unlimit=%d, max_nodes=%d",
                timstr(0),
                id_str,
                acct_id,
                seconds/60.0,
                weight,
                !(kill),
                lock,
                rflg,
                max_nodes);
        if (ret_val == -1)
          fprintf(logfp, ", errno=%d", errno);
        fprintf(logfp, "\n");
        fflush(logfp);
      }

    } else {
      /*----------------*
       * add users
       *----------------*/
      if (!aflg) {
        naccts = getagidfromuid(accts, -1);
      }
      if (naccts <= 0) {
        fprintf(stderr, "\nList 0 accounts\n");
        if (logfp) fclose(logfp);
        CHECK(-1);
      }

      if (!uflg) {
        fprintf(stderr, "\nMust specify user login or user id\n");
        if (logfp) fclose(logfp);
        CHECK(-1);
      }

      if (rflg == -1 && !Pflg && !tflg) {
        tmsign = 0;
        seconds = atof(DEF_MINUTES) * 60.0;
        tflg = 1;
      }

      if (!pflg) {
        accsign = 1;
        permission = USE_MASK;
      }

      if (!nflg) {
        max_nodes = conf->total_nodes;
	if (max_nodes <= 0) max_nodes = DEF_NODES;
      }

      macinit();
      for (i = 0; i < naccts; i++) {
        for (j = 0; j < nusers; j++) {
          if ((ret_val = adduagid(users[j], accts[i])) < 0) {
	    fnl_rtrn = -1;
            fprintf(stderr, "\nAdd user %d to account %d ", users[j], accts[i]);
            if (ret_val == -1) {
              fprintf(stderr, "Fail in release write lock on /etc/nxaccount\n");
            } else if (ret_val == -11) {
              fprintf(stderr, "Invalid agid\n");
            } else if (ret_val == -12) {
              fprintf(stderr, "Invalid uid\n");
            } else if (ret_val == -13) {
              fprintf(stderr, "Line exceed 128 char. in /etc/nxaccount\n");
            } else if (ret_val == -14) {
              fprintf(stderr, "Fail open /etc/nxaccount\n");
            } else if (ret_val == -15) {
              fprintf(stderr, "Fail getting write lock on /etc/nxaccount\n");
            } else if (ret_val == -16) {
              fprintf(stderr, "Fail open /etc/nxaccount.macs\n");
            } else {
              fprintf(stderr, "Unknown error from adduagid()\n");
            }

            if (ret_val < 10) {
              fprintf(stderr, "Add aborted.\n");
              if (logfp) fclose(logfp);
              CHECK(-1);
            }
          }

          ret_val = mac_modify(accts[i],
                               users[j],
                               seconds,
                               tmsign,
                               tflg,
                               Pflg,
                               permission,
                               accsign,
                               rflg,
                               max_nodes,
				1);
          if (ret_val == -1) {
	    fnl_rtrn = -1;
            fprintf(stderr, "\nError creating new account\n\n");
          }
          if (logfp) {
            fprintf(logfp, "%s %s\n\tADD uid=%d, acct=%d",
                    timstr(0), id_str, users[j], accts[i]);
            if (rflg == 1) {
              fprintf(logfp, ", time=unlimit");
            } else {
              fprintf(logfp, ", %s=%f", tflg ? "time" : "percent", 
		tflg ? seconds/60.0 : seconds/100.0);
            }
            fprintf(logfp, ", access=%d, max_nodes=%d", permission, max_nodes);
            if (ret_val == -1) {
              fprintf(logfp, ", errno=%d", errno);
            }
            fprintf(logfp, "\n");
            fflush(logfp);
          }
        }
      }
    }
    break;

  case 'c':				/* change accounts/users setting */
    nitem = 0;
    if (mode[1] == 'a') {
      /* change accounts */
      if (!aflg)
        usage(restrict, "Must specify account name or id");
      if (naccts <= 0) {
        fprintf(stderr, "\nChange 0 accounts\n");
        if (logfp) fclose(logfp);
        CHECK(-1);
      }
      if (rflg == 0 || rflg == 1)
        nitem = 1;
      if (!tflg && !Pflg)
        seconds = -1;
      else
        nitem = 1;
      if (!wflg)
        weight = -1;
      else
        nitem = 1;
      if (!kflg)
        kill = -1;
      else
        nitem = 1;
      if (!lflg)
        lock = -1;
      else
        nitem = 1;
      if (!nflg)
        max_nodes = -2;
      else
        nitem = 1;
      if (nitem == 0) {
        fprintf(stderr, "\nNo change is requested to accounts\n");
        if (logfp) fclose(logfp);
        CHECK(-1);
      }

      macinit();
      for (i = 0; i < naccts; i++) {
        ret_val = mac_update(accts[i],
                             seconds,
                             tmsign,
                             weight,
                             kill,
                             lock,
                             rflg,
                             max_nodes);
        if (ret_val == -1) {
	  fnl_rtrn = -1;
          fprintf(stderr, "\nError change account %d\n", accts[i]);
	}
        if (logfp) {
          fprintf(logfp, "%s %s\n\tMODIFY acct=%d", timstr(0), id_str, accts[i]);
          if (rflg == 1)
            fprintf(logfp, ", time=unlimit");
          else if (seconds != -1)
            fprintf(logfp, ", time=%f, sign=%d", seconds/60.0, tmsign);
          if (weight != -1)
            fprintf(logfp, ", weight=%d", weight);
          if (kill != -1)
            fprintf(logfp, ", Nokill=%d", !(kill));
          if (lock != -1)
            fprintf(logfp, ", lock=%d", lock);
          if (max_nodes != -1)
            fprintf(logfp, ", max_nodes=%d", max_nodes);
          if (ret_val == -1)
            fprintf(logfp, ", errno=%d", errno);
          fprintf(logfp, "\n");
          fflush(logfp);
        }
      }

    } else {
      /* change users */
      if (!aflg && !uflg)
        usage(restrict, "Must specify accounts or users");
      if (!tflg && !Pflg)
        seconds = -1;
      else
        nitem = 1;
      if (rflg == 1 || rflg == 0)
        nitem = 1;
      if (!pflg)
        permission = -1;
      else
        nitem = 1;
      if (!nflg)
        max_nodes = -2;
      else
        nitem = 1;
      if (nitem == 0) {
        fprintf(stderr, "\nNo change is requested to users\n");
        if (logfp) fclose(logfp);
        CHECK(-1);
      }

      if (!aflg) {
        /* change all accounts for specified users*/
        macinit();
        for (i = 0; i < nusers; i++) {
          naccts = getagidfromuid(accts,users[i]);
          for (j = 0; j < naccts; j++) {
            ret_val = mac_modify(accts[j],
                                 users[i],
                                 seconds,
                                 tmsign,
                                 tflg,
                                 Pflg,
                                 permission,
                                 accsign,
                                 rflg,
                                 max_nodes,
				 0);
            if (ret_val == -1) {
	      fnl_rtrn = -1;
              fprintf(stderr, "\nError change user %d\n", users[i]);
	    }
            if (logfp) {
              fprintf(logfp, "%s %s\n\tMODIFY uid=%d, acct=%d",
                  timstr(0), id_str, users[i], accts[j]);
              if (rflg == 1)
                fprintf(logfp, ", time=unilmit");
              else if (seconds != -1)
                fprintf(logfp, ", %s=%f, sign=%d", tflg ? "time" : "percent", 
			tflg ? seconds/60.0 : seconds/100.0, tmsign);
              if (permission != -1)
                fprintf(logfp, ", permission=%d, sign=%d", permission, accsign);
              if (max_nodes != -1)
                fprintf(logfp, ", max_nodes=%d", max_nodes);
              if (ret_val == -1)
                fprintf(logfp, ", errno=%d", errno);
              fprintf(logfp, "\n");
              fflush(logfp);
            }
          }
        }

      } else {
        /* accounts specified, users may or may not */
        macinit();
        for (i = 0; i < naccts; i++) {
          if (!uflg && (nusers = getuidfromagid(users, accts[i])) <= 0) {
	    fnl_rtrn = -1;
            fprintf(stderr, "\n0 users in account %d\n",
                accts[i]);
            continue;
          }
          for (j = 0; j<nusers; j++) {
            ret_val = mac_modify(accts[i],users[j],seconds,tmsign,tflg,
                Pflg,permission,accsign,rflg,max_nodes,0);
            if (ret_val == -1) {
	      fnl_rtrn = -1;
              fprintf(stderr, "\nError change user %d\n", users[j]);
	    }
            if (logfp) {
              fprintf(logfp, "%s %s\n\tMODIFY uid=%d, acct=%d",
                  timstr(0), id_str, users[i], accts[j]);
              if (rflg == 1)
                fprintf(logfp, ", time=unlimit");
              else if (seconds != -1)
                fprintf(logfp, ", %s=%f, sign=%d",
                    tflg ? "time" : "percent", 
		    tflg ? seconds/60.0 : seconds/100.0, tmsign);
              if (permission != -1)
                fprintf(logfp, ", permission=%d, sign=%d", permission, accsign);
              if (max_nodes != -1)
                fprintf(logfp, ", max_nodes=%d", max_nodes);
              if (ret_val == -1)
                fprintf(logfp, ", errno=%d", errno);
              fprintf(logfp, "\n");
              fflush(logfp);
            }
          }
        }
      }
    }
    break;

  case 'd':					/* delete accounts or users */
    if (mode[1] == 'a') {
      /* delete accounts */
      if (!aflg)
        usage(restrict, "Must specify accounts with -a");
      macinit();
      for (i = 0; i < naccts; i++) {
        ret_val = mac_delacct(accts[i]);
        if (ret_val == -1) {
	  fnl_rtrn = -1;
          fprintf(stderr, "\nError deleting account %d in database\n",
              accts[i]);
	}
        if (logfp) {
          fprintf(logfp,
                  "%s %s\n\tDELETE acct=%d",
                  timstr(0),
                  id_str,
                  accts[i]);
          if (ret_val == -1) {
            fprintf(logfp, ", errno=%d", errno);
          }
          fprintf(logfp, "\n");
          fflush(logfp);
        }
        ret_val = delagid(accts[i]);
	if (ret_val < 0) {
	  fnl_rtrn = -1;
          fprintf(stderr,
                  "\nError=%d when delete acct=%d from /etc/nxaccount\n",
                  errno, accts[i]);
	  if (ret_val == -1) 
		fprintf(stderr, 
			"Fail in release write-lock on /etc/nxaccount\n");
	  else if (ret_val == -2) 
		fprintf(stderr, 
			"Fail in open /etc/nxaccomm\n");
	  else if (ret_val == -3) 
		fprintf(stderr, 
			"Fail in open /etc/nxaccomm.tmp\n");
	  else if (ret_val == -10) 
		fprintf(stderr, 
			"You must first delete all users from an account before deleting the account itself.\n");
	  else if (ret_val == -11) 
		fprintf(stderr, 
			"Fail in open /etc/nxaccount\n");
	  else if (ret_val == -12) 
		fprintf(stderr, 
			"Fail in getting write-lock on /etc/nxaccount\n");
	  else if (ret_val == -13) 
		fprintf(stderr, 
			"Fail in open /etc/nxaccount.tmp\n");
	  else fprintf(stderr, "return value %d\n", ret_val);
        }
      }

    } else {
      /* delete users from accounts */
      if (!uflg && !aflg)
        usage(restrict, "Must specify accounts or users with -a or -u");
      if (!aflg) {
        /* delete all accounts for specified users */
        macinit();
        for (i = 0; i<nusers; i++) {
          naccts = getagidfromuid(accts,users[i]);
          for (j = 0; j < naccts; j++) {
            ret_val = mac_deluser(accts[j], users[i]);
            if (ret_val == -1) {
	      fnl_rtrn = -1;
              fprintf(stderr,
                      "\nError deleting user %d from account %d in database\n",
                      users[i],
                      accts[j]);
	    }
            if (logfp) {
              fprintf(logfp,
                      "%s %s\n\tDELETE uid=%d, acct=%d",
                      timstr(0),
                      id_str,
                      users[i],
                      accts[j]);
              if (ret_val == -1)
                fprintf(logfp, ", errno=%d", errno);
              fprintf(logfp, "\n");
              fflush(logfp);
            }
            if ((ret_val = deluagid(users[i], accts[j])) < 0) {
	      fnl_rtrn = -1;
              fprintf(stderr,
                      "\nDelete user %d from account %d ",
                      users[i],
                      accts[j]);

              if (ret_val == -1) {
                fprintf(stderr, "Fail in release write lock on /etc/nxaccount\n");
              } else if (ret_val == -11) {
                fprintf(stderr, "Invalid agid\n");
              } else if (ret_val == -12) {
                fprintf(stderr, "Invalid uid\n");
              } else if (ret_val == -13) {
                fprintf(stderr, "Line exceed 128 char. in /etc/nxaccount\n");
              } else if (ret_val == -14) {
                fprintf(stderr, "Fail open /etc/nxaccount\n");
              } else if (ret_val == -15) {
                fprintf(stderr, "Fail getting write lock on /etc/nxaccount\n");
              } else if (ret_val == -16) {
                fprintf(stderr, "Fail open /etc/nxaccount.macs\n");
              } else {
                fprintf(stderr, "Unknown error from deluagid()\n");
              }

              if (ret_val < 10) {
                fprintf(stderr, "Delete aborted.\n");
                if (logfp) fclose(logfp);
                CHECK(-1);
              }
            }
          }
        }

      } else {
        /* accounts specified, users may or may not */
        macinit();
        for (i = 0; i < naccts; i++) {
          if (!uflg &&
              (nusers = getuidfromagid(users, accts[i])) <= 0) {
	    fnl_rtrn = -1;
            fprintf(stderr, "0 users in account %d\n", accts[i]);
            continue;
          }
          for (j = 0; j<nusers; j++) {
            ret_val = mac_deluser(accts[i], users[j]);
            if (ret_val == -1) {
	      fnl_rtrn = -1;
              fprintf(stderr,
                      "\nError deleting user %d from account %d in database\n",
                      users[j], accts[i]);
	    }
            if (logfp) {
              fprintf(logfp,
                      "%s %s\n\tDELETE uid=%d, acct=%d",
                      timstr(0),
                      id_str,
                      users[j],
                      accts[i]);
              if (ret_val == -1)
                fprintf(logfp, ", errno=%d", errno);
              fprintf(logfp, "\n");
              fflush(logfp);
            }
            if (deluagid(users[j], accts[i]) == -1) {
	      fnl_rtrn = -1;
              fprintf(stderr,
                      "\nUser %d account %d is not in /etc/nxaccount file\n",
                      users[j],
                      accts[i]);
            }
          }
        }
      }
    }
    break;

  case 'l':					/* list accounts or users */
    if (mode[1] == 'a') { 	/* list accounts */
      char cmd[256];
      if (!aflg) {
        naccts = getagidfromuid(accts, -1);
      }
      if (naccts <= 0) {
        fprintf(stderr, "\nDatabase is empty\n");
        if (logfp) fclose(logfp);
        CHECK(0);
      }

      if (!aflg) (void) sprintf (cmd, "%s -A", MACLIST_FNAME);
      else {
	int i;
	char *ptr;
	(void) sprintf (cmd, "%s -a%d", MACLIST_FNAME, accts[0]);
	for (i=1, ptr=cmd+strlen(cmd); 
		i<naccts && ptr<cmd+250; i++, ptr=cmd+strlen(cmd)) 
	    (void) sprintf (ptr, ",%d", accts[i]);
      }
      fnl_rtrn = system (cmd);
/*
      macinit();
      if (mac_list(accts, users, naccts, 0, 0, 0) == -1) {
	fnl_rtrn = -1;
        fprintf(stderr, "\nError completing listing\n\n");
      }
*/
    } else { 	/* list users */
      if (!uflg) { 	/* list all users */
	char cmd[256];
        if (!aflg) {
          naccts = getagidfromuid(accts, -1);
        }
        if (naccts <= 0) {
          fprintf(stderr, "\nDatabase is empty\n");
          if (logfp) fclose(logfp);
          CHECK(0);
        }
	if (!aflg) (void) sprintf (cmd, "%s -U", MACLIST_FNAME);
	else {
	    int i;
	    char *ptr;
	    (void) sprintf (cmd, "%s -a%d", MACLIST_FNAME, accts[0]);
	    for (i=1, ptr=cmd+strlen(cmd); 
		i<naccts && ptr<cmd+250; i++, ptr=cmd+strlen(cmd)) 
	        (void) sprintf (ptr, ",%d", accts[i]);
	}
	fnl_rtrn = system (cmd);
/*
        macinit();
        for (i = 0; i < naccts; i++) {
          nusers = getuidfromagid(users, accts[i]);
          if (mac_list(accts+i, users, 1, nusers, 0, 0) == -1) {
	    fnl_rtrn = -1;
            fprintf(stderr, "\nError completing listing\n\n");
          }
        }
*/
      } else { 	/* list users, users specified */
	char cmd[256], *ptr;
	int i;
	(void) sprintf (cmd, "%s", MACLIST_FNAME);
	ptr = cmd+strlen(cmd);
	if (aflg) {
	    (void) sprintf (cmd, "%s -a%d", MACLIST_FNAME, accts[0]);
	    for (i=1, ptr=cmd+strlen(cmd); 
		i<naccts && ptr<cmd+250; i++, ptr=cmd+strlen(cmd)) 
	        (void) sprintf (ptr, ",%d", accts[i]);
	}
        (void) sprintf (ptr, " -u%d", users[0]);
        for (i=1, ptr=cmd+strlen(cmd);
            i<nusers && ptr<cmd+250; i++, ptr=cmd+strlen(cmd))
            (void) sprintf (ptr, ",%d", users[i]);
	fnl_rtrn = system (cmd);
/*
        macinit();
        for (i = 0; i < nusers; i++) {
          if (!aflg && (naccts = getagidfromuid(accts, users[i])) <= 0) {
	    fnl_rtrn = -1;
            fprintf(stderr, "\n0 accounts for user %d\n", users[i]);

          } else if (mac_list(accts, users + i, naccts, 1, 0, 0) == -1) {
	    fnl_rtrn = -1;
            fprintf(stderr, "\nError completing listing\n\n");
          }
        }
*/
      }
    }
    break;

  case 't':			/* transfer allocation between accounts */
    if (!Fflg || !Tflg) {
      usage(restrict, "Must specify -F from_acct and -T to_acct");
    }
    if (from_acid == to_acid)
      usage(restrict, "both accounts are the same one, no transfer needed");
    if (!Pflg && !tflg) {
      if (!checked) {
        usage(restrict, "Must specify -p percent or -t minutes");

      } else {
        printf("Must specify percentage or time\n");
        return(-1);
      }
    }
    macinit();
    ret_val = mac_transfer(from_acid, to_acid, seconds, Pflg);
    if (ret_val == -1) {
      fnl_rtrn = -1;
      fprintf(stderr, "Error transfer account allocation\n");
    }
    if (logfp) {
      fprintf(logfp, "%s %s\n\tTRANSFER from acct=%d to acct=%d, %s=%f",
          timstr(0),
          id_str,
          from_acid,
          to_acid,
          tflg ? "time" : "percent",
          tflg ? seconds/60.0 : seconds/100.0);
      if (ret_val == -1) {
        fprintf(logfp, ", errno=%d", errno);
      }
      fprintf(logfp, "\n");
      fflush(logfp);
    }
    break;
  }

  if (cpuctl(C_CLOSE, 0) == -1) {
    fprintf(stderr, "\n Error closing connection to MACD\n");
  }
  if (logfp) fclose(logfp);
  CHECK(fnl_rtrn);
}

/*===========================================================================*
 * Function:
 *
 * Abstract:    This function
 *
 * Arguments:
 *
 * Return value:
 *
 * Notes:
 *===========================================================================*/
static void usage(restrict, msg)
  const int restrict;
  const char *msg;
{
  if (msg != NULL) {
    fprintf(stderr, "Invalid command line option:\n\t%s\n\n", msg);
  }

  if (restrict) fprintf(stderr, "Usage:  macalloc\n");
  else fprintf(stderr, "Usage:  macadmin\n");
  fprintf(stderr, "\t-m mode\n");
  if (!restrict) fprintf(stderr, "\t-A new_account_name,id,description\n");
  fprintf(stderr, "\t-a account_name_or_id_list\n");
  fprintf(stderr, "\t-u user_name_or_id_list\n");
  fprintf(stderr, "\t-F from_an_account_name_or_id\n");
  fprintf(stderr, "\t-T to_an_account_name_or_id\n");
  fprintf(stderr, "\t-w weight\n");
  fprintf(stderr, "\t-N No_kill\n");
  fprintf(stderr, "\t-l lock\n");
  fprintf(stderr, "\t-p user_access_permission\n");
  fprintf(stderr, "\t-%c percentage\n", '%');
  fprintf(stderr, "\t-t minutes\n");
  fprintf(stderr, "\t-r\n");
  fprintf(stderr, "\t-n #_of_nodes\n\n");
  exit(-1);
}

