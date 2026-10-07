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
 * Copyright (c) 1990 San Diego Supercomputer Center.
 * All rights reserved.  The SDSC software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * File:	modify.c
 *
 * mac_modify 
 *
 * Abstract:	add an user as a new member of an account or modify 
 *		an user account attributes in MACD database.  It
 *		assumes a connection with MACD, fetch needed information
 *		from MACD database and validate permissions.  If the
 *		permission is fine, then pack the request message and
 *		send to MACD, wait then process reply message from MACD.
 *
 * Arguments:	int	agid;
 *		int	uid;
 *		double  amount;		in seconds or percentage
 *		int	sign;		+1, -1, 0 : increment, decrement, set
 *		int	alloc_flag;
 *		int	percent_flag;
 *		int	access_flag;	mtu bits or -1 (no change)
 *              int     accsign;
 *		int	unlimit;	unlimit bit ( 0/1 or -1 no change )
 *		int	maxnodes;	< -1 for no change
 *		int	newflg;		1 new account, 0 existing account
 * Return value:	0	successful
 *			-1	failure
 */

#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#include "mac.h"
#include "macerr.h"
#include "ipc.h"

#define PCNT (10000)

int mac_modify(agid, uid, amount, sign, alloc_flag, percent_flag, access_flag,
accsign, unlimit, maxnodes, newflg)
  int		agid;
  int		uid;
  double	amount;
  int		sign;
  int		alloc_flag;
  int		percent_flag;
  int		access_flag;
  int		accsign;
  int		unlimit;
  int		maxnodes;
  int		newflg;
{
  struct cpu_ctrl	acct_data, user_data;
  double		camount, author, new_alloc;
  int			setpriv = 1;
  extern void bzero();

  /*
   * Fill in information for GETLIMIT request
   */

  (void) bzero ((char*)&user_data, sizeof(user_data));
  user_data.uid = getuid();
  user_data.agid = agid;
  user_data.info.id = user_data.uid;

  /* 
   * If not a super-user, get and check user's modify privileges for account
   */

  if (user_data.uid !=0) {
    setpriv = 0;

    /*
     * Get the potential pi's entry from MACD
     */

    if (cpuctl (C_GETLIMIT, &user_data) < 0) {
      errno = EACCES;
      macerr();
      return (-1);
    }

    /*
     * If the user's cpulimit entry doesn't have the modify bit set ding them 
     */

    if (user_data.info.modify == 0) {
      errno = EACCES;
      macerr();
      return (-1);
    }

    if (user_data.info.transfer==1) setpriv = 1;
  }

  /*
   * update information for second GETLIMIT request
   */

  (void) bzero ((char*)&user_data, sizeof(user_data));
  user_data.uid = getuid();
  user_data.agid = agid;
  user_data.info.id = uid;

  /*
   * Get the user entry from MACD
   */
  if (cpuctl(C_GETLIMIT, &user_data) < 0) {
    if (!newflg) {	/* Fail in modifying a non-existing account */
      errno = ENOENT;
      macerr();
      return (-1);
    }
    if (access_flag & USE_MASK) {
      if (accsign == -1) {
        errno = ENOENT;
        macerr();
        return(-1);
      }
    } else {
      errno = EINVAL;
      macerr();
      return (-1);
    }
  }
  else if (newflg) {	/* add account, but already exist */
      errno = EEXIST;
      macerr();
      return (-1);
  }

  /*
   * Fill in information for GETACCT request
   */

  (void) bzero ((char*)&acct_data, sizeof(acct_data));
  acct_data.uid = user_data.uid;
  acct_data.agid = user_data.agid;
  acct_data.info.id = agid;

  /*
   * Get the corresponding account entry
   */

  if (cpuctl (C_GETACCT, &acct_data) < 0) {
    errno = ENOENT;
    macerr();
    return (-1);
  }

  /*
   * User can have unlimit allocation only if the account is unlimit
   */

  if (unlimit == 1 && !acct_data.info.unlimit) {
    fprintf(stderr,
        "Mac_modify: Cannot set unlimit allocation under a limited account\n");
    return (-1);
  }
  else if (unlimit == 1 && (alloc_flag || percent_flag)) {
    (void) fprintf (stderr,
        "Mac_modify: Alloc or pcnt flags cannot be set with unlimit flag\n");
    return (-1);
  }
  else if (unlimit != -1) user_data.info.unlimit = unlimit;

  /*
   * Alloc and percent flags cannot both be set
   */

  if (alloc_flag && percent_flag) {
    fprintf(stderr, "Mac_modify: Alloc and pcnt flags cannot be both set\n");
    return (-1);
  }

  /*
   * If alloc flag set, the input amount is time in seconds
   */

  if (alloc_flag) {
    if (amount < 0.0) {
      (void) fprintf (stderr, "Invalid allocation amount %f\n", amount/60.0);
      return (-1);
    }
    camount=amount;
    author = acct_data.info.authorized;
    if (sign == 0) {  /* set total time to input amount */
      if (!acct_data.info.unlimit) {
        if (camount > author) {
          (void) fprintf (stderr, "Allocation amount %f exceeded allocation %f\n",
		camount/60.0, author/60.0);
          return (-1);
        }
        user_data.info.percent = (PCNT*camount)/author;
      }
      else {
        user_data.info.percent=0;
      }
      user_data.info.authorized=camount;
    }
    else {    /* add or subtract the input amount */
      camount = camount * sign;
      if (!user_data.info.unlimit) {
        new_alloc = user_data.info.authorized + camount;
        if (new_alloc > author) { 
          (void) fprintf (stderr, 
		"New allocation amount %f exceeds account allocation %f\n",
		new_alloc/60.0, author/60.0);
          return (-1);
        }
        if (new_alloc < 0) {
          (void) fprintf (stderr, 
		"Reduction amount %f exceeds amount authorized %f\n",
		camount/60.0, user_data.info.authorized/60.0);
          return (-1);
        }
        user_data.info.percent = (PCNT*new_alloc)/author;
      }
      else {
        user_data.info.percent=0;
      }
      user_data.info.authorized += camount;
      if (user_data.info.authorized < 0.0) user_data.info.authorized = 0.0;
    }
  }

  /*
   * If percent flag set, input amount is percentage
   */

  if (percent_flag) {
    if (amount < 0.0 || amount > (double)PCNT) {
      (void) fprintf (stderr,
          "Invalid allocation percentage %f\n", amount/100.0);
      return (-1);
    }
    if (sign == 0)    /* Set percentage to input amount */
      user_data.info.percent = amount;
    else {    /* Add or subtract input percentage */
      user_data.info.percent += sign * amount;
      if (user_data.info.percent < 0) user_data.info.percent = 0;
      if (user_data.info.percent > PCNT) user_data.info.percent = PCNT;
    }
    user_data.info.authorized = acct_data.info.authorized * user_data.info.percent / 100.0;
    if ( acct_data.info.unlimit) {
      if ( user_data.info.percent > 0.0 )
        user_data.info.unlimit = 1;
      else user_data.info.unlimit = 0;
    }
  }

  /*
   * If access flag is set (0-7), modify access bits in user entry to the value
   * of the flag.  This operation will not performed on non-root PI.
   */

  if (access_flag >= 0 && access_flag <=7 && 
	(getuid() == 0 || (uid && getuid() != uid))) {
    if (access_flag & MODIFY_MASK) {
      if(accsign == -1) user_data.info.modify = 0;
      else user_data.info.modify =1;
    }
    if (access_flag & TRANSFER_MASK) {
      if (accsign == -1)
        user_data.info.transfer = 0;
      else {
        if (setpriv == 1)
          user_data.info.transfer = 1;
        else {
          errno = EACCES;
          macerr();
          return (-1);
        }
      }
    }
    if (access_flag & USE_MASK) {
      if (accsign == -1) {
        user_data.info.use = 0;
#ifdef NEVER
        if (user_data.info.used_time == 0 || 
            getuid() == 0 ) {
          if (cpuctl (C_DELLIMIT, &user_data) < 0) {
            macerr();
            if (deludbacc(user_data.agid,
                user_data.info.id)==-1) {
              fprintf(stderr,
                  "Error deleting account access\n");
              return(-1);
            }
            return (-1);
          }
          if (deludbacc(user_data.agid,
              user_data.info.id)==-1) {
            fprintf(stderr,
                "Error deleting account access\n");
            return(-1);
          }
          return(0);
        }
#endif
      }
      else {
        user_data.info.use = 1;
        if (chkudbacc(user_data.agid, user_data.info.id) == -1) {
          fprintf(stderr, "Error checking/changing account access\n");
          return(-1);
        }
      }
    }
  }

  /*
   * If requested, change maxnodes
   */

  if (maxnodes >= 0 || maxnodes == -1) user_data.info.maxnodes = maxnodes;

  /*
   * If requested, change unlimit bit
   */

  if (unlimit != -1) {
    if (unlimit != 0 && unlimit != 1) {
      (void) fprintf (stderr,
          "Invalid unlimit flag value %d\n", unlimit);
      return (-1);
    }
    else user_data.info.unlimit = unlimit;
  }

  /*
   * Send the modified user entry back to MACD
   */

  if (cpuctl (C_SETLIMIT, &user_data) < 0) {
    macerr();
    return (-1);
  }

  return (0);
}

int chkudbacc(agid, uid)
int agid,uid;
{
  return(adduagid(uid, agid));
}

int deludbacc(agid, uid)
int agid,uid;
{
  return(deluagid(uid, agid));
}
