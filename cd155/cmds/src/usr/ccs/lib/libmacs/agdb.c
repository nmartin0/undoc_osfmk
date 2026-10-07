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
 * File:        agdb.c
 *
 * Abstract:	The functions in this file implement a simple account
 *		database NXACCOUNT_FNAME and NXACCOMM_FNAME.
 * Note:
 *	The functions can't handle account length of more than LINEMAX.
 *	Should fix it so it breaks them up into multiple lines.
 *****************************************************************************/
#ifdef LINT
static char     sccs_id[] = "@(#)agdb.c	2.18 10/20/92";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pwd.h>

#include "mac_def.h"
#include <nx/nxacct.h>
#include "macerr.h"
#include "filename.h"

#define OK			(0)
#define MANDITORYLOCK		(02600)

#define LINEMAX			6144

static int	pik_agid __((char *buf, int index));

#ifdef DEBUG
extern int	_debug_;
#endif

/*===========================================================================*
 * Function:	delagid
 *
 * Abstract:    This function deletes agid from the account database.
 *		This operation involves rewriting the entire account database.
 *
 * Arguments:
 *	agid - account id to delete from account file.
 *
 * Return value:
 *	0	successful
 *	-1	fail in release write lock on NXACCOUNT_FNAME
 *	-2	fail in open NXACCOMM_FNAME
 *	-3	fail in open NXACCOMM_TEMP_FNAME
 *	-10	Cannot delete a non-empty account
 *	-11	fail in open NXACCOUNT_FNAME
 *	-12	cannot get write lock on NXACCOUNT_FNAME
 *	-13	fail in open NXACCOUNT_TEMP_FNAME
 *
 * Notes:
 *===========================================================================*/
int delagid(agid)
   int agid;
{
   FILE			*fp;
   FILE			*fp_macs;
   FILE			*com;
   FILE			*com_macs;
   char			buf[LINEMAX];
   struct nxacct	*nxstuff;
   struct flock		fl;

   /* check if id is in acount file */
   if ((nxstuff = nx_getaid(agid)) == NULL) {
      return(0);
   }

   /* check if someone is in the account */
   if (nxstuff->acct_mem[0]) {
      return(-10);
   }

   /* Open the file and create it if necessary */
   /* (chmod forces correct perms despite user's umask: PTS 9445) */
   if ((fp = fopen(NXACCOUNT_FNAME, "r+")) == NULL
	|| chmod(NXACCOUNT_FNAME, 0644) != 0 ) { 
      macerrno = ER_FAILOPEN;
      return(-11);
   }

#ifdef FILELOCKS
   /* lock file*/
   fl.l_type	= F_WRLCK;
   fl.l_whence	= SEEK_SET;
   fl.l_start	= 0;
   fl.l_len	= 0;
   if (fcntl(fp->_cnt, F_SETLKW, &fl) < 0) {
      (void) fclose(fp);
      macerrno = ER_BADLOCK;
      return(-12);
   }
#endif /* FILELOCKS */

   /* open a temp file to rebuild NXACCOUNT_FNAME in */
   /* (chmod forces correct perms despite user's umask: PTS 9445) */
   if ((fp_macs = fopen(NXACCOUNT_TEMP_FNAME, "w+")) == NULL
	|| chmod(NXACCOUNT_TEMP_FNAME, 0644) != 0 ) { 
      (void) fclose(fp);
      macerrno = ER_FAILOPEN_TMP;
      return(-13);
   }

   /* copy all lines, except the account to be deleted */
   while (fgets(buf, LINEMAX, fp) != NULL) {
      if (pik_agid(buf, 2) != agid) {
        (void) fputs(buf, fp_macs);
      }
   }

   /*------------------------------------------------------------------------*
    * we don't need to lock because we lock the NXACCOUNT_FNAME
    *------------------------------------------------------------------------*/

   /* open comment file */
   /* (chmod forces correct perms despite user's umask: PTS 9445) */
   if ((com = fopen(NXACCOMM_FNAME, "r+")) == NULL
	|| chmod(NXACCOMM_FNAME, 0644) != 0 ) { 
      (void) fclose(fp_macs);
      (void) fclose(fp);
      macerrno = ER_FAILOPENC;
      return(-2);
   }

   /* open a temp file to rebuild NXACCOMM_FNAME in */
   /* (chmod forces correct perms despite user's umask: PTS 9445) */
   if ((com_macs = fopen(NXACCOMM_TEMP_FNAME, "w+")) == NULL
	|| chmod(NXACCOMM_TEMP_FNAME, 0644) != 0 ) { 
      (void) fclose(com);
      (void) fclose(fp_macs);
      (void) fclose(fp);
      macerrno = ER_FAILOPENC_TMP;
      return(-3);
   }

   /* remove account from NXACCOMM_FNAME */
   while (fgets(buf, LINEMAX, com) != NULL) {
      if (pik_agid(buf, 1) != agid) {
        (void) fputs(buf, com_macs);
      }
   }
   (void) fclose(com_macs);
   (void) fclose(com);
   (void) rename(NXACCOMM_TEMP_FNAME, NXACCOMM_FNAME);

#if FILELOCKS
   /* release lock */
   fl.l_type	= F_UNLCK;
   fl.l_whence	= SEEK_SET;
   fl.l_start	= 0;
   fl.l_len	= 0;
   if (fcntl(fp->_cnt, F_SETLKW, &fl) < 0) {
      (void) fclose(fp_macs);
      (void) fclose(fp);
      macerrno = ER_BADUNLOCK;
      return(-1);
   }
#endif
   (void) fclose(fp_macs);
   (void) fclose(fp);
   (void) rename(NXACCOUNT_TEMP_FNAME, NXACCOUNT_FNAME);
   return(0);
}

/*===========================================================================*
 * Function:	deluagid
 *
 * Abstract:    This function deletes uid from an account database.  
 *		This operation involves rewriting the entire NXACCOUNT_FNAME.
 *
 * Arguments:
 *	uid - id number of user to be deleted.
 *	agid - id number of account from which to delete user.
 *
 * Return value:
 *	0	successful
 *	-1	fail in release write lock on NXACCOUNT_FNAME
 *	-1	invalid agid
 *	-12	invalid uid
 *	-13	a line exceed LINEMAX characters
 *	-14	fail in open NXACCOUNT_FNAME
 *	-15	cannot get write lock on NXACCOUNT_FNAME
 *	-16	fail in open NXACCOUNT_TEMP_FNAME
 *
 * Notes:
 *===========================================================================*/
int deluagid(uid, agid)
   int	uid;
   int	agid;
{
   int			i;
   int			last, found;
   FILE			*fp;
   FILE			*fp_macs;
   char			buf[LINEMAX];
   char			*ptr;
   struct nxacct	*nxstuff;
   struct passwd	*pwent;
   struct flock		fl;

   /* check if account group exits */
   if ((nxstuff = nx_getaid(agid)) == NULL) {
      return(-11);
   }

   /* check if user exits */
   if ((pwent = getpwuid(uid)) == NULL) {
      return(-12);
   }

   /* scan list of account names in account */
   for (i = 0; nxstuff->acct_mem[i] != NULL; i++) {
     if (strcmp(pwent->pw_name, nxstuff->acct_mem[i]) == 0) {
        break;		/* found it */
     }
   }

   /* If the user's name was not found, then we are done. */
   if (nxstuff->acct_mem[i] == NULL) {
      return(0);
   }

   /* Open the file and create it if necessary */
   /* (chmod forces correct perms despite user's umask: PTS 9445) */
   if ((fp = fopen(NXACCOUNT_FNAME, "r+")) == NULL
	|| chmod(NXACCOUNT_FNAME, 0644) != 0 ) { 
      macerrno = ER_FAILOPEN;
      return(-14);
   }

   /* lock file */
   fl.l_type	= F_WRLCK;
   fl.l_whence	= SEEK_SET;
   fl.l_start	= 0;
   fl.l_len	= 0;
#ifdef FILELOCKS
   if (fcntl(fp->_cnt, F_SETLKW, &fl) < 0) {
      (void) fclose(fp);
      macerrno = ER_BADLOCK;
      return(-12);
   }
#endif /* FILELOCKS */

   /* open a temp file to rebuild NXACCOUNT_FNAME in */
   /* (chmod forces correct perms despite user's umask: PTS 9445) */
   if ((fp_macs = fopen(NXACCOUNT_TEMP_FNAME, "w+")) == NULL
	|| chmod(NXACCOUNT_TEMP_FNAME, 0644) != 0 ) { 
      (void) fclose(fp);
      macerrno = ER_FAILOPEN_TMP;
      return(-16);
   }

   /* copy every line except the account to be changed and replace it */
   found = 0;
   while (fgets(buf, LINEMAX, fp) != NULL) {
      if (pik_agid(buf, 2) == agid) {
	 if (found) continue;
	 found = 1;
         /* compose the new line(s) */
         last = 0;
         do {
           (void) sprintf(buf, "%s:%s:%d:",
                   nxstuff->acct_name,
                   nxstuff->acct_passwd,
                   nxstuff->acct_id);

           ptr = buf + strlen(buf);
           for (i = last; nxstuff->acct_mem[i] != NULL; i++) {
             if (strcmp(pwent->pw_name, nxstuff->acct_mem[i]) != 0) {
               if (strlen(buf) + strlen(nxstuff->acct_mem[i]) < LINEMAX) {
                 (void) sprintf(ptr, (i == last) ? "%s" : ",%s", 
			nxstuff->acct_mem[i]);
                 ptr = buf + strlen(buf);
               } else {
                 last = i;
                 break;
               }
             }
	     else if (i == last) last = i + 1;
           }
           (void) sprintf(ptr, "\n");
      	   (void) fputs(buf, fp_macs);
        } while (nxstuff->acct_mem[i] != NULL);
      }
      else (void) fputs(buf, fp_macs);
   }

#if FILELOCKS
   /* release lock */
   fl.l_type	= F_UNLCK;
   fl.l_whence	= SEEK_SET;
   fl.l_start	= 0;
   fl.l_len	= 0;
   if (fcntl(fp->_cnt, F_SETLKW, &fl) < 0) {
      (void) fclose(fp_macs);
      (void) fclose(fp);
      macerrno = ER_BADUNLOCK;
      return(-1);
   }
#endif
   (void) fclose(fp_macs);
   (void) fclose(fp);
   (void) rename(NXACCOUNT_TEMP_FNAME, NXACCOUNT_FNAME);
   return(0);
}

/*===========================================================================*
 * Function:	getuidfromagid
 *
 * Abstract:    This function returns a list of uids which belong to a
 *		particular agid
 *
 * Arguments:
 *	uids -	user id
 *	agid -	account id
 *
 * Return value:
 *	>=0	number of accounting groups
 *	-1	invalid account id
 *	-2	number of users under the account exceed maximum - it's time
 *		to recompile the utilities with a large MAX_UIDS
 *
 * Notes:
 *===========================================================================*/
int getuidfromagid(uids, agid)
   int		uids[];
   int		agid;
{
   int			i;
   int			count;
   struct nxacct	*nxstuff;
   struct passwd	*pwent;

   /* check if id is in acount file */
   if ((nxstuff = nx_getaid(agid)) == NULL) {
      return(-1);
   }

   /* make list of uids */
   count = 0;
   for (i = 0; nxstuff->acct_mem[i] != NULL; i++) {
      if ((pwent = getpwnam(nxstuff->acct_mem[i])) != NULL) {
         if (count > MAX_UIDS) {
            return(-2);
         } else {
            uids[count++] = pwent->pw_uid;
         }
      }
   }
   return(count);
}

/*===========================================================================*
 * Function:	countagids
 *
 * Abstract:    This function returns the total number of account database.
 *
 * Arguments:
 *	None.
 *
 * Return value:
 *	number of accounts database.
 *
 * Notes:
 *===========================================================================*/
int countagids()
{
   int i;

   (void) nx_setaent();
   for (i = 0; nx_getaent() != NULL; i++)
      ;
   return(i);
}

/*===========================================================================*
 * Function:	getagidfromuid
 *
 * Abstract:    This function returns a list of agids which particular user
 *		belongs to.  If uid is negative, then all agids are returned 
 *		in agids[].  The size of the array passed in is MAXAGIDS,
 *		defined in mac_def.h.
 *
 * Arguments:
 *	agids -	account id
 *	uid -	user id
 *
 * Return value:
 *	>=0	number of agids found.
 *	-1	invalid user id
 *	-2	number of accounts a user can be in has exceed maximum - it's
 *		time to recompile the utilities with a large MAX_AGIDS
 *
 * Notes:
 *===========================================================================*/
int getagidfromuid(agids, uid)
   int	agids[];
   int	uid;
{
   int			j;
   int			count;
   struct nxacct	*nxstuff;
   struct passwd	*pwent;
   char **t;

   /* make sure user has an entry in the passwd file */
   if (uid >= 0 && (pwent = getpwuid(uid)) == NULL) {
      return(-1);
   }

   nx_setaent();
   count = 0;
   while ((nxstuff = nx_getaent()) != NULL) {
      if (count > MAX_AGIDS) {
        return(-2);
      }

      if (uid < 0) {
         agids[count++] = nxstuff->acct_id;
         continue;
      }

      for (t = nxstuff->acct_mem; *t; ++t) {
	 if ((**t == '!') && (strcmp(pwent->pw_name, ((*t)+1)) == 0))
	       break;
	 if ((strcmp(pwent->pw_name, *t) == 0) || (strcmp(*t, "*") == 0))
               agids[count++] = nxstuff->acct_id;
      }
   }
   return(count);
}

/*===========================================================================*
 * Function:	addagid
 *
 * Abstract:    This function adds an new accoount into the account database
 *		This operation will append a line to account database files.
 *
 * Arguments:
 *	aname -	account name
 *	agid -	account id
 *	comment - account description
 *
 * Return value:
 *	A negative value is returned if there is some problem.
 *	error return < -9 indicates incomplete operation
 *	-1	agid exist in NXACCOUNT_FNAME
 *	-2	aname exist in NXACCOUNT_FNAME
 *	-3	fail in release lock on NXACCOUNT_FNAME
 *	-11	fail open NXACCOUNT_FNAME
 *	-12	fail in getting lock on NXACCOUNT_FNAME
 *
 * Notes:
 *===========================================================================*/
int addagid(aname, agid, comment)
   char		*aname;
   int		agid;
   char		*comment;
{
   FILE			*fp;
   FILE			*com;
   struct flock		fl;

   /* make sure the account name and ids are uniq */
   if (nx_getaid(agid) != NULL) {
      macerrno = ER_AGIDEXISTS;
      return(-1);
   }

   if (nx_getanam(aname) != NULL) {
      macerrno = ER_ANAMEEXISTS;
      return(-2);
   }

   /* Open the file and create it if necessary */
   /* (chmod forces correct perms despite user's umask: PTS 9445) */
   if ((fp = fopen(NXACCOUNT_FNAME, "a")) == NULL
	|| chmod(NXACCOUNT_FNAME, 0644) != 0 ) { 
      macerrno = ER_FAILOPEN;
      return(-11);
   }

#ifdef FILELOCKS
   /* lock file */
   fl.l_type	= F_WRLCK;
   fl.l_whence	= SEEK_SET;
   fl.l_start	= 0;
   fl.l_len	= 0;
   if (fcntl(fp->_cnt, F_SETLKW, &fl) < 0) {
      (void) fclose(fp);
      macerrno = ER_BADLOCK;
      return(-12);
   }
#endif /* FILELOCKS */


   /* add acount without a passwd */
   (void) fprintf(fp, "%s:*:%d:\n", aname, agid);

   /*------------------------------------------------------------------------*
    * we don't need to lock because we lock the NXACCOUNT_FNAME
    *------------------------------------------------------------------------*/

printf("Adding to comm file %s:%d:%s\n", aname, agid, comment);
   /* add account to NXACCOMM_FNAME */
   /* (chmod forces correct perms despite user's umask: PTS 9445) */
   if ((com = fopen(NXACCOMM_FNAME, "a")) == NULL
	|| chmod(NXACCOMM_FNAME, 0644) != 0 ) { 
      macerrno = ER_FAILOPENC;
      return(OK); /* don't return error, accomm file is not important */
   }
   (void) fprintf(com, "%s:%d:%s\n", aname, agid, comment);
   (void) fclose(com);
printf("Added to comm file %s:%d:%s\n", aname, agid, comment);

#if FILELOCKS
   /* release lock */
   fl.l_type	= F_UNLCK;
   fl.l_whence	= SEEK_SET;
   fl.l_start	= 0;
   fl.l_len	= 0;
   if (fcntl(fp->_cnt, F_SETLKW, &fl) < 0) {
      (void) fclose(fp);
      macerrno = ER_BADUNLOCK;
      return(-3);
   }
#endif
   (void) fclose(fp);
   return(0);
}

/*===========================================================================*
 * Function:	adduagid
 *
 * Abstract:    This function adds the user(uid) as a new member to the 
 *		acount in the account database.
 *		This operation involves rewriting the entire NXACCOUNT_FNAME.
 *
 * Arguments:
 *	uid - user id to be add as a new member into an account
 *	agid - account id in which the user is to be added in
 *
 * Return value:
 *	A negative value is returned if there is some problem.
 *	-1	fail in release write lock on NXACCOUNT_FNAME
 *	-11	invalid agid
 *	-12	invalid uid
 *	-13	a line exceed LINEMAX characters
 *	-14	fail in open NXACCOUNT_FNAME
 *	-15	cannot get write lock on NXACCOUNT_FNAME
 *	-16	fail in open NXACCOUNT_TEMP_FNAME
 *
 * Notes:
 *===========================================================================*/
int adduagid(uid, agid)
   int	uid;
   int	agid;
{
   int			i;
   int			last, found;
   FILE			*fp;
   FILE			*fp_macs;
   char			buf[LINEMAX];
   char			*ptr;
   struct nxacct	*nxstuff;
   struct passwd	*pwent;
   struct flock fl;

   /* check validaty */
   if ((nxstuff = nx_getaid(agid)) == NULL) {
      return(-11);
   }

   if ((pwent = getpwuid(uid)) == NULL) {
      return(-12);
   }

   /* test if uid is a member already */
   for (i = 0; nxstuff->acct_mem[i] != NULL; i++) {
     if (strcmp(pwent->pw_name, nxstuff->acct_mem[i]) == 0) {
        return(0);
     }
   }

   /* Open the file and create it if necessary */
   /* (chmod forces correct perms despite user's umask: PTS 9445) */
   if ((fp = fopen(NXACCOUNT_FNAME, "r+")) == NULL
	|| chmod(NXACCOUNT_FNAME, 0644) != 0 ) { 
     macerrno = ER_FAILOPEN;
     return(-14);
   }

   /* lock file */
   fl.l_type	= F_WRLCK;
   fl.l_whence	= SEEK_SET;
   fl.l_start	= 0;
   fl.l_len	= 0;
#ifdef FILELOCKS
   if (fcntl(fp->_cnt, F_SETLKW, &fl) < 0) {
      (void) fclose(fp);
      macerrno = ER_BADLOCK;
      return(-12);
   }
#endif /* FILELOCKS */


   /* open a temp file to rebuild NXACCOUNT_FNAME in */
   /* (chmod forces correct perms despite user's umask: PTS 9445) */
   if ((fp_macs = fopen(NXACCOUNT_TEMP_FNAME, "w+")) == NULL
	|| chmod(NXACCOUNT_TEMP_FNAME, 0644) != 0 ) { 
      macerrno = ER_FAILOPEN_TMP;
      return(-16);
   }

   /* copy every line except the account to be changed and replace it */
   found = 0;
   while (fgets(buf, LINEMAX, fp) != NULL) {
      if (pik_agid(buf, 2) == agid) {
	 if (found) continue;
	 found = 1;
         /* compose the new lines */
         last = 0;
         do {
           if (last) (void) sprintf(buf, "%s:%s:%d:%s",
                   nxstuff->acct_name,
                   nxstuff->acct_passwd,
                   nxstuff->acct_id,
                   nxstuff->acct_mem[last++]);
           else (void) sprintf(buf, "%s:%s:%d:%s",
                   nxstuff->acct_name,
                   nxstuff->acct_passwd,
                   nxstuff->acct_id,
                   pwent->pw_name);
  
           ptr = buf + strlen(buf);
           for (i = last; nxstuff->acct_mem[i] != NULL; i++) {
             if (strlen(buf) + strlen(nxstuff->acct_mem[i]) < LINEMAX) { 
               (void) sprintf(ptr, ",%s", nxstuff->acct_mem[i]);
               ptr = buf + strlen(buf);

             } else {
               last = i;
               break;
             }
           }
           (void) sprintf(ptr, "\n");
      	   (void) fputs(buf, fp_macs);
         } while (nxstuff->acct_mem[i] != NULL);
      }
      else (void) fputs(buf, fp_macs);
   }

#if FILELOCKS
   /* release lock */
   fl.l_type	= F_UNLCK;
   fl.l_whence	= SEEK_SET;
   fl.l_start	= 0;
   fl.l_len	= 0;
   if (fcntl(fp->_cnt, F_SETLKW, &fl) < 0) {
      (void) fclose(fp_macs);
      (void) fclose(fp);
      macerrno = ER_BADUNLOCK;
      return(-1);
   }
#endif

   (void) fclose(fp_macs);
   (void) fclose(fp);
   (void) rename(NXACCOUNT_TEMP_FNAME, NXACCOUNT_FNAME);
   return(OK);
}

/*===========================================================================*
 * Function:	pik_agid
 *
 * Abstract:    This function will get account id number from a NXACCOUNT_FNAME
 *		line
 *
 * Arguments:
 *	buf - buffer containing the line from the nxacct file
 *	index - the field index of the id to be extracted
 *
 * Return value:
 *	>=0	the id in that field
 *	-1 	if the field was empty
 *
 * Notes:
 *===========================================================================*/
static int pik_agid(buf, index)
   char		*buf;
   int		index;
{
   int		i;
   char		*ptr;

   /* skip specified number of fields */
   ptr = buf;
   for (i = 0; i < index; i++) {
      ptr = strchr(ptr, ':');
      if (ptr == NULL) {
         return(-1);		/* next field was empty */
      } else {
         ptr++;			/* skip the colin */
      }
   }


#ifdef DEBUG 
   (void) fprintf(stderr, "pik_agid return %s %d\n", ptr, atoi(ptr));
#endif

   /* atoi is smart enough to handle the mact */
   return(atoi(ptr));
}
