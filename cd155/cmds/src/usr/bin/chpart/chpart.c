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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/chpart/chpart.c,v 1.29 1994/11/19 01:20:28 mtm Exp $
 *
 */

/* History:
 * $Log: chpart.c,v $
 * Revision 1.29  1994/11/19  01:20:28  mtm
 * Copyright additions/changes
 *
 * Revision 1.28  1994/01/25  22:09:21  carbajal
 *  7848 PARAGON      OPEN      M        carbajal  gregt               R1.2 WW03
 *       MESH UTILS   17-JAN-94 **       17-JAN-94 17-JAN-94
 *       chpart with -sps and -epl # fails with Invalid Scheduling
 *
 *  Reviewer: Cameron
 *  Risk: Low
 *  Benefit or PTS #: 7848
 *  Testing: rmcall, rmcmd, sched EATs, bug report1
 *  Module(s): chpart.c
 *
 * Revision 1.27.2.1  1994/01/25  18:31:26  carbajal
 *  7848 PARAGON      OPEN      M        carbajal  gregt               R1.2 WW03
 *       MESH UTILS   17-JAN-94 **       17-JAN-94 17-JAN-94
 *       chpart with -sps and -epl # fails with Invalid Scheduling
 *
 *  Reviewer: Cameron
 *  Risk: Low
 *  Benefit or PTS #: 7848
 *  Testing: rmcall, rmcmd, sched EATs, bug report1
 *  Module(s): chpart.c
 *
 * Revision 1.27  1993/12/01  01:47:25  carbajal
 * Implemented use of -sps, cleaned up Usage message
 *  Reviewer: NONE
 *  Risk: Low
 *  Benefit or PTS #: R1.2 User Model
 *  Testing:
 *  Module(s):
 *
 * Revision 1.26  1993/11/18  20:13:11  dleslie
 *  Reviewer:shala
 *  Risk: low
 *  Benefit or PTS #: new cmds/libs build scheme
 * 	get mcmsg and nx headers out of export tree
 * 	get libs out of export tree
 *  Testing: built on Suns and 486
 *  Module(s): Makefile
 *
 * Revision 1.25  1993/11/17  03:02:33  carbajal
 *  Reviewer: None
 *  Risk: Medium
 *  Benefit or PTS #: R1.2 User Model SUpport
 *  Testing:
 *  Module(s):
 *
 * Revision 1.24  1993/09/17  00:21:34  carbajal
 * Fixed PTS #6117
 *
 * Revision 1.23  1993/07/20  21:02:11  carbajal
 * Make sure all char * are initialized to NULL!
 *
 * Revision 1.22  1993/07/20  18:15:48  carbajal
 * Added a call to Usage()
 *
 * Revision 1.21  1993/07/18  19:49:16  carbajal
 * call NX_chpart()
 *
 * Revision 1.20  1993/07/13  21:00:16  carbajal
 * Added new parameters to allocator_cmds_setup() and parse_rq
 *
 * Revision 1.19  1993/05/12  16:46:18  shala
 * Include sys/syslimits.h to define PATH_MAX.
 *
 * Revision 1.18  1993/05/06  18:40:31  carbajal
 * Changed call to write_partinfo() to pass in all necessary parameters
 *
 * Revision 1.17  1993/04/29  23:44:58  carbajal
 * If -o option is used and we are not root, the return EACESS instead of
 * EPACESS.
 *
 * Revision 1.16  1993/04/29  23:35:06  carbajal
 * Report error on chpart -o .group partname
 *
 * Revision 1.15  1993/04/29  00:11:08  carbajal
 * Cleaned up usage message
 *
 * Revision 1.14  1993/03/31  01:36:50  carbajal
 * If no group is specified with the -o option keep the
 * original group number
 *
 * Revision 1.13  1993/03/24  01:44:41  dleslie
 * Removed unnecessary include of "bitmap.h" which was causing build to
 * fail on Paragon due to re-definition of MIG_BITMAP_T.
 *
 * Revision 1.12  1992/12/18  02:54:44  carbajal
 * conform to new utils.c and partutils.c:w
 * change Usage printout to say -nm instead of
 * -name
 * chpart -nm option check to see if rename is
 * ok before you call allocator. The new name
 * must be a simple name, ie no "."'s.
 * Usage: change usage printout
 *
 * Revision 1.10  1992/11/09  22:08:35  carbajal
 * Return EPACCES if not superuser and trying to use the -o option.
 *
 * Revision 1.9  1992/11/03  00:29:03  carbajal
 * allocator_cmds_setup no longer uses
 * flockfile so chpart now calls try_lock
 * which is in libnx.a. This lock is
 * controlled by the allocator.
 * funlockfile has been removed.
 *
 * Revision 1.8  1992/10/22  00:55:14  carbajal
 * -nm will now call the allocator to for permission
 * checks. The allocator just returns a yea or a nay.
 *
 * Revision 1.7  1992/10/13  22:29:18  carbajal
 * Added support for renaming partitions with -nm
 * Also moved parsepart.c and parsesched.c into libnx
 *
 * Revision 1.6  1992/10/13  00:45:42  carbajal
 * Pass NULL as the default partition to allocator_cmds_setup.
 *
 * Revision 1.5  1992/10/09  00:11:08  carbajal
 * Removed references to size as per PARTREQ_T changes
 *
 * Revision 1.4  1992/10/06  00:58:17  carbajal
 * added support for locking .partinfo file
 * get parent's proc info to check for superuser
 *
 * Revision 1.3  1992/09/21  23:57:06  carbajal
 * Make one call into allocator for all options.
 * Started making functions out of loose code getting ready for
 * consolidation of all commands.
 * -o option will always require an owner
 * find_uid and find_gid will percolate errors up instead of exiting
 * change -mp option to -epl
 * added CSECT_ENTER and CSECT_EXIT
 *
 * Revision 1.2  1992/09/17  00:35:36  carbajal
 * Added log history for RCS
 * Added rcsid as static char
 *
*/

static char rcsid[] = "$Id: chpart.c,v 1.29 1994/11/19 01:20:28 mtm Exp $";

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syslimits.h>
#include <sys/table.h>
#include <sys/user.h>

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <unistd.h>

#include <nx.h>
#include <mcmsg/mcmsg_appl.h>
#include <nx/allocator.h>
#include <nx/bitmap.h>
#include <nx/defines.h>
#include <nx/mkpart.h>
#include <nx/utils.h>
#include <nx/partutils.h>
#include <nx/writepart.h>
#include <nx/nx_getopt.h>
#include <nx/procinfo.h>
#include <nx/parsesched.h>


/* local function prototypes */
void Usage();
#ifdef DEBUG
void print_info();
#endif DEBUG
int find_uid(char *id_str, uid_t *id);
int find_gid(char *id_str, gid_t *id);
int validate_user_group_ids(char *owner,uid_t *uid,gid_t *gid);
int validate_group_id(char *group,gid_t *gid);
int send_chpart(uid_t uid,gid_t gid);
int ok_chown();
int rename_partition(char *from,char *to);
/* end of local function prototypes */


#define	IAM_ROOT	0


/********************************  chpart ********************************
 *        
 *      Calling Sequence:
 *             NONE
 *
 *	Description:
 *		chpart - change attributes of an existing partition.
 *		chpart allows the user to change the rollin quantum
 *		for gang scheduled partitions, the maximum priority
 *		of a partition, the access mode of a partition and
 *		the owner/group of a partition.
 *		See function Usage below for complete docs
 *
 *      Parameters:
 *              argc,argv 
 *
 *      Returns:
 *               exit(0) if partition could be changed as requested
 *		 exit(1) if partition could not be changed as requested
 *		 exit(USAGE_EXIT) if there is a syntax problem
 *
 *
*/

#define MAX_RETRIES	10

main(int argc,char **argv)
{
	long status;		/* Status */
	long optindold;		/* argv index of the next argument - 1 */ 
	long argcount;		/* count of flags processed */
	char *rollquant;	/* argument to -rq */
	char *access;		/* argument  to mod */
	char *owner;		/* argument to -o */
	char *s;		/* option */
	char *name;		/* Last argument of chpart */
	char *new_name;		/* for -nm option */
	char *partname;
	char *new_grp;		/* new group name for -g option */
	char  *maxpri;		/* Maximum Priority */
	short nameflg,rqflg, 
		eplflg,modflg,spsflg,
		oflg,grpflg;	 /* Option flags */
	uid_t uid;		/* owner user id and process user id */
	gid_t gid,owner_gid;	/* owner gourp id and process group id */
	int   retry;
	nx_part_info_t part;

	name = NULL;
	new_name = NULL;
	new_grp = NULL;
	
	optindold = nx_optind;
	rqflg = 0;
	modflg = 0;
	eplflg = 0;
	oflg = 0;
	grpflg = 0;
	spsflg = 0;
	nameflg = 0;
	argcount = 1;
	status = 0;	
	
	nx_init_partinfo(&part);

    
	/*
	 *  Parse options.
	 *  This loop parses the command specific options. 
	 */
	while((s = nx_getopt(argc,argv, "g:,nm:,rq:,epl:,o:,mod:,sps")) != NULL) {

#ifdef DEBUG
printf("and the option flag is %s \n",s);
#endif

		if( strcmp(s,"rq" ) == 0)  {
			/* change rollin quantum */
			argcount += (nx_optind - optindold);
			optindold = nx_optind;
			rqflg = 1;
			rollquant = strdup(nx_optarg);
		} else if( strcmp(s,"mod") == 0 )  {
			/* change mode of partition */
			argcount += (nx_optind - optindold);
			optindold = nx_optind;
			modflg = 1;
			access = strdup(nx_optarg);
		} else if( strcmp(s,"epl") == 0 )  {
			/* change maximum priority */
			argcount += (nx_optind - optindold);
			optindold = nx_optind;
			eplflg = 1;
			maxpri = strdup(nx_optarg);
		} else if( strcmp(s,"o") == 0 )  {
			/* change owner */
			argcount += (nx_optind - optindold);
			optindold = nx_optind;
			oflg = 1;
			owner = strdup(nx_optarg);
		} else if( strcmp(s,"nm") == 0 )  {
			/* change partition name */
			argcount += (nx_optind - optindold);
			optindold = nx_optind;
			nameflg = 1;
			new_name = strdup(nx_optarg);
		} else if( strcmp(s,"g") == 0 )  {
			/* change just the group */
			argcount += (nx_optind - optindold);
			optindold = nx_optind;
			grpflg = 1;
			new_grp = strdup(nx_optarg);
		} else if( strcmp(s,"sps") == 0 )  {
			argcount += (nx_optind - optindold);
			optindold = nx_optind;
			spsflg = 1;
		}
		else {
			Usage();
			exit(USAGE_EXIT);
		}
	 }

	if ( (partname = get_partition_name(argcount,argc,argv,
			 &name,argv[0],NULL)) == NULL){
		Usage();
		exit(1);
	}

	if (spsflg && rqflg){
		errno = EPINVALSCHED;
		perror(argv[0]);
		exit(1);
	}

	if (spsflg){
		part.flags_or_size |= SPSFLG;
	}

	if( rqflg) {
		/* change rollin quantum */
		status = parse_rq(rollquant,&part.rq);
		if( status < 0 ) {
			perror(argv[0]);
			exit(1);
		}
		part.flags_or_size |= RQFLG;
	} 

	if( modflg ) { 
		/* change access mode */
		if( parse_mod(access,&part.access) < 0 ) {
			errno = EPINVALMOD;
			perror(argv[0]);
			exit(1); 
		}
		part.flags_or_size |= MODFLG;
#ifdef DEBUG
printf("new access %o \n",part.access);
#endif
	  }

	if( eplflg ){
		/* change maximum priority level */
		if (isnumber(maxpri))
			part.epl = atoi(maxpri);
		else
			part.epl = -1;

		if( part.epl < 0 || part.epl > 10 ) {
			errno = EPINVALPRI;
			perror(argv[0]);
			exit(1);
		}
		part.flags_or_size |= EPLFLG;
	}

	if( oflg){
		/* change owner/group of partition */
		owner_gid = -1;
		if (validate_user_group_ids(owner,&part.uid,&owner_gid)){
			if (owner_gid != -1)
				part.gid = owner_gid;
			part.flags_or_size |= OWNERFLG;
		}
		else{
			if (errno == USAGE_ERROR)
				Usage();
			else
				perror(argv[0]);
			exit(1);
		}
	}

	if(grpflg){
		/* change group of partition */
		if (validate_group_id(new_grp,&part.gid)){
			/* now make sure that the caller belongs to this
			 * group
			*/
			part.flags_or_size |= OWNERFLG;
		}
		else{
			perror(argv[0]);
			exit(1);
		}
	}

	if (nameflg)
		part.flags_or_size |= NMFLG;

	if (NX_chpart(partname,part,new_name) != 0){
		perror("chpart");
		exit(1);
	}
	else
		exit(0);

	   
}
			      

int 
find_last_instance(char *str,char c)
{
	int	i,len;
	int	cpi;

	len = strlen(str);

	cpi = 0;
	for(i = 0; i < len; i++){
		if (str[i] == c) 
			cpi = i;
	}
	return(cpi);
}


/********************************  find_uid  ********************************
 *
 *	  Calling Sequence:
 *	     find_uid(id_str,uid);
 *
 *	  Description:
 *	     Gets the user id from the user database
 *
 *	  Parameters:
 *		id_str  string containing user id
 *		id	pointer to object of uid_t type
 *
 *	  Returns:
 *		TRUE if successful
 *		FALSE otherwise
 *
 *	  Called by:
 *	      validate_user_group_ids
 *
 */
 
int
find_uid(char *id_str, uid_t *id)
{
	struct passwd   *user;
	int	ok;	/* return value */

	ok = TRUE;

	/* see if specified owner is a name of a user id */
	if (!isnumber(id_str)){
	        /* specified user name so lookup user id in database */
	        user = getpwnam(id_str);
	        if ( (char *) user == NULL){
			       /* no user found */
			errno = EPINUSER;
			ok = FALSE;
	        }
		else
	        	*id = user->pw_uid;
	}
	else{
	        /* they specified a decimal value */
	        *id = atoi(id_str);
	        /* validate user id */
	        user = getpwuid(*id);
	        if ( (char *) user == NULL){
			/* Invalid user */
			errno = EPINUSER;
			ok = FALSE;
	        }
	}
	return(ok);
}

/********************  find_gid  ********************************
 *
 *	  Calling Sequence:
 *	     find_gid(id_str,uid);
 *
 *	  Description:
 *	     Gets the group id from the group database
 *
 *	  Parameters:
 *	      id_str  string containing group id
 *	      id      pointer to object of gid_t type
 *
 *	  Returns:
 *	     TRUE if successful
 *		   FALSE otherwise
 *
 *	  Called by:
 *	      validate_user_group_ids
 *
 */
 
int
find_gid(char *id_str, gid_t *id)
{
	struct group   *grp;
	int		ok;	/* return value */

	ok = TRUE;
 
	/* see if specified owner is a name of a group id */
	if (!isnumber(id_str)){
	        /* specified group name so lookup group id in database */
	        grp = getgrnam(id_str);
	        if ( (char *) grp == NULL){
			       /* no group found */
			errno = EPINGRP;
			ok = FALSE;
	        }
		else
	        	*id = grp->gr_gid;
	}
	else{
	        /* they specified a decimal value */
	        *id = atoi(id_str);
	        /* validate user id */
	        grp = getgrgid(*id);
	        if ( (char *) grp == NULL){
			       /* no group found */
			errno = EPINGRP;
			ok = FALSE;
	        }
	}
	return(ok);
}

/*******  validate_user_group_ids  ********************************
 *
 *	Calling Sequence:
 *		validate_user_group_ids(owner,uid,gid);
 *
 *	Description:
 *		Validates that the string passed as owner is a
 *		valid user id and a valid group id
 *
 *	Parameters:
 *		owner  string containing specified user/group id
 *		uid	place to put user id if valid
 *		gid	place to put group id if valid
 *
 *	Returns:
 *		TRUE if all is ok
 *		FALSE otherwise (errno will be set)
 *
 *	Called by:
 *		chpart
 *
*/
int
validate_user_group_ids(char *owner,uid_t *uid,gid_t *gid)
{
	char    *user_str,*group_str;
	int	ok;

	ok = TRUE;
	/* open user database */
	setpwent();
	/* owner is of the form owner[.group] */

	user_str = strdup(owner);
	group_str = strchr(user_str,'.');
	if (group_str != NULL){
		/* erase '.' from user_str */
		*group_str = '\0';
		/* skip over erased '.' in group string */
		group_str++;
	}

	if (user_str != NULL && user_str[0] == '\0'){
		ok = FALSE;
		errno = USAGE_ERROR;
		goto error;
	}

#ifdef DEBUG
	printf("new user id %s\n new group id %s\n",user_str,group_str);
#endif DEBUG

	/* Make sure that the user name is valid */
	if (find_uid(user_str,uid))
		if (group_str != NULL)
			ok = validate_group_id(group_str,gid);
		else
			ok = TRUE;
	else
		ok = FALSE;

error:
	/* close user database */
	endpwent();
	free(user_str);	
	return(ok);
}
	

/*******  validate_group_id ********************************
 *
 *	Calling Sequence:
 *		validate_group_id(group,gid);
 *
 *	Description:
 *		Validates that the string passed as group is a
 *		valid group id.
 *
 *	Parameters:
 *		group  string containing specified group id
 *		gid	place to put group id if valid
 *
 *	Returns:
 *		TRUE if all is ok
 *		FALSE otherwise (errno will be set)
 *
 *	Called by:
 *		chpart
 *
*/
int
validate_group_id(char *group,gid_t *gid)
{
	int	ok;

	/* open group database */
	setgrent();
	ok = find_gid(group,gid);
	/* close group database */
	endgrent();
	return(ok);
}


/********************************  Usage  ********************************
 *
 *	Calling Sequence:
 *		Usage();
 *
 *	Description:
 *		Prints the Usage message
 *
 *	Parameters:
 *		NONE
 *
 *	Returns:
 *		NONE
 *
 *	Called by:
 *		chpart
 *
 */

void Usage()
{
  fprintf(stderr,"chpart: syntax error\n");
  fprintf(stderr,"Usage: chpart   [ -g group ] [ -mod mode ] [ -nm\n");
  fprintf(stderr,"       name ] [ -o owner[.group] ]\n");
  fprintf(stderr,"       [ -sps | [ -rq time ]] [-epl pri] partition\n");
}


#ifdef DEBUG
/** This a Temporary function written for debugging **/

void print_info()
{
  register i;

  
  printf("************************************************\n");
  printf("Uid  = %d \n", part.uid);
  printf("Gid = %d \n", part.gid);
  printf("Nodes = %d \n", part.nodes);
  printf("Slots = %d \n", part.slots);
  printf("Access = %o \n", part.access);
  printf("Sched = %d \n", part.sched);
  printf("rq = %d \n", part.rq);
  printf("flags = %d\n",part.flags_or_size);
  printf("Ndlist = ");
  for(i = 0; i < part.nodes ; i++) 
	  printf("%d ", p_ndlist[i]);

  printf("\nRect rows = %d \n", part.rect.rows);
  printf("Rect cols = %d \n",part.rect.cols);
  printf("Maxpri = %d \n",part.maxpri);
  if(part.sched == 0)
	  printf("UNIX SCHEDULING\n");
   else
	  printf("GANG SCHEDULING\n");

}
#endif DEBUG
